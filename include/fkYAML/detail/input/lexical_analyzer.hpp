//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_INPUT_LEXICAL_ANALYZER_HPP
#define FK_YAML_DETAIL_INPUT_LEXICAL_ANALYZER_HPP

#include <algorithm>
#include <cctype>
#include <cstdlib>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/assert.hpp>
#include <fkYAML/detail/encodings/uri_encoding.hpp>
#include <fkYAML/detail/encodings/utf_encodings.hpp>
#include <fkYAML/detail/input/block_scalar_header.hpp>
#include <fkYAML/detail/input/position_tracker.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/str_view.hpp>
#include <fkYAML/detail/types/lexical_token_t.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Lexical token information
struct lexical_token {
    lexical_token() = default;

    lexical_token(lexical_token_t t, str_view s) noexcept
        : type(t),
          str(s) {
    }

    lexical_token(lexical_token_t t) noexcept
        : type(t) {
    }

    lexical_token(const lexical_token&) = default;
    lexical_token& operator=(const lexical_token&) = default;
    lexical_token(lexical_token&&) = default;
    lexical_token& operator=(lexical_token&&) = default;
    ~lexical_token() = default;

    /// Lexical token type.
    lexical_token_t type {lexical_token_t::END_OF_BUFFER};
    /// Lexical token contents.
    str_view str;
};

/// @brief A class which lexically analyzes YAML formatted inputs.
class lexical_analyzer {
    // whether the current context is flow(1) or block(0)
    static constexpr uint32_t flow_context_bit = 1u << 0u;
    // whether the current document part is directive(1) or content(0)
    static constexpr uint32_t document_directive_bit = 1u << 1u;

public:
    /// @brief Construct a new lexical_analyzer object.
    /// @param input_buffer An input buffer.
    explicit lexical_analyzer(str_view input_buffer) noexcept
        : m_begin_itr(input_buffer.begin()),
          m_cur_itr(input_buffer.begin()),
          m_end_itr(input_buffer.end()) {
        m_pos_tracker.set_target_buffer(input_buffer);
    }

    /// @brief Get the next lexical token by scanning the left of the input buffer.
    /// @return lexical_token The next lexical token.
    lexical_token get_next_token() {
        skip_white_spaces_and_newline_codes();

        m_token_begin_itr = m_cur_itr;
        m_pos_tracker.update_position(m_cur_itr);
        m_last_token_begin_pos = m_pos_tracker.get_cur_pos_in_line();
        m_last_token_begin_line = m_pos_tracker.get_lines_read();

        if (m_cur_itr == m_end_itr) {
            return {lexical_token_t::END_OF_BUFFER};
        }

        switch (*m_cur_itr) {
        case '?':
            if (++m_cur_itr == m_end_itr) {
                return {lexical_token_t::PLAIN_SCALAR, {m_token_begin_itr, 1}};
            }

            if (*m_cur_itr == ' ') {
                return {lexical_token_t::EXPLICIT_KEY_PREFIX};
            }
            break;
        case ':': // key separator
            if (++m_cur_itr == m_end_itr) {
                return {lexical_token_t::KEY_SEPARATOR};
            }

            switch (*m_cur_itr) {
            case ' ':
            case '\t':
            case '\n':
                return {lexical_token_t::KEY_SEPARATOR};
            default:
                if ((m_state & flow_context_bit) == 0) {
                    // in a block context
                    break;
                }

                switch (*m_cur_itr) {
                case ',':
                case '[':
                case ']':
                case '{':
                case '}':
                    // Flow indicators are not "safe" to be followed in a flow context.
                    // See https://yaml.org/spec/1.2.2/#733-plain-style for more details.
                    return {lexical_token_t::KEY_SEPARATOR};
                default:
                    // At least '{' or '[' must precedes this token.
                    FK_YAML_ASSERT(m_token_begin_itr != m_begin_itr);

                    // if a key inside a flow mapping is JSON-like (surrounded by indicators, see below), YAML allows
                    // the following value to be specified adjacent to the ":" mapping value indicator.
                    // ```yaml
                    // # the following flow mapping entries are all valid.
                    // {
                    //   "foo":true,
                    //   'bar':false,          # 'bar' is actually not JSON but allowed in YAML
                    //                         # since its surrounded by the single quotes.
                    //   {[1,2,3]:null}:"baz"
                    // }
                    // ```
                    switch (*(m_token_begin_itr - 1)) {
                    case '\'':
                    case '\"':
                    case ']':
                    case '}':
                        return {lexical_token_t::KEY_SEPARATOR};
                    default:
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        case ',': // value separator
            ++m_cur_itr;
            return {lexical_token_t::VALUE_SEPARATOR};
        case '&': // anchor prefix
            return {lexical_token_t::ANCHOR_PREFIX, extract_anchor_name()};
        case '*': // alias prefix
            return {lexical_token_t::ALIAS_PREFIX, extract_anchor_name()};
        case '!': // tag prefix
            return {lexical_token_t::TAG_PREFIX, extract_tag_name()};
        case '#': // comment prefix
            scan_comment();
            return get_next_token();
        case '%': // directive prefix
            if (m_state & document_directive_bit) {
                return {scan_directive()};
            }
            // The '%' character can be safely used as the first character in document contents.
            // See https://yaml.org/spec/1.2.2/#912-document-markers for more details.
            break;
        case '-': {
            switch (*(m_cur_itr + 1)) {
            case ' ':
            case '\t':
            case '\n':
                // Move a cursor to the beginning of the next token.
                m_cur_itr += 2;
                return {lexical_token_t::SEQUENCE_BLOCK_PREFIX};
            default:
                break;
            }

            if (m_pos_tracker.get_cur_pos_in_line() == 0) {
                if ((m_end_itr - m_cur_itr) > 2) {
                    const bool is_dir_end = std::equal(m_token_begin_itr, m_cur_itr + 3, "---");
                    if (is_dir_end) {
                        m_cur_itr += 3;
                        return {lexical_token_t::END_OF_DIRECTIVES};
                    }
                }
            }

            break;
        }
        case '[': // sequence flow begin
            ++m_cur_itr;
            return {lexical_token_t::SEQUENCE_FLOW_BEGIN};
        case ']': // sequence flow end
            ++m_cur_itr;
            return {lexical_token_t::SEQUENCE_FLOW_END};
        case '{': // mapping flow begin
            ++m_cur_itr;
            return {lexical_token_t::MAPPING_FLOW_BEGIN};
        case '}': // mapping flow end
            ++m_cur_itr;
            return {lexical_token_t::MAPPING_FLOW_END};
        case '@':
            emit_error("Any token cannot start with at(@). It is a reserved indicator for YAML.");
        case '`':
            emit_error("Any token cannot start with grave accent(`). It is a reserved indicator for YAML.");
        case '\"':
            ++m_token_begin_itr;
            return {lexical_token_t::DOUBLE_QUOTED_SCALAR, determine_double_quoted_scalar_range()};
        case '\'':
            ++m_token_begin_itr;
            return {lexical_token_t::SINGLE_QUOTED_SCALAR, determine_single_quoted_scalar_range()};
        case '.': {
            if (m_pos_tracker.get_cur_pos_in_line() == 0) {
                const auto rem_size = m_end_itr - m_cur_itr;
                if FK_YAML_LIKELY (rem_size > 2) {
                    const bool is_doc_end = std::equal(m_cur_itr, m_cur_itr + 3, "...");
                    if (is_doc_end) {
                        if (rem_size > 3) {
                            switch (*(m_cur_itr + 3)) {
                            case ' ':
                            case '\t':
                            case '\n':
                                m_cur_itr += 4;
                                break;
                            default:
                                // See https://yaml.org/spec/1.2.2/#912-document-markers for more details.
                                emit_error("The document end marker \"...\" must not be followed by non-ws char.");
                            }
                        }
                        else {
                            m_cur_itr += 3;
                        }
                        return {lexical_token_t::END_OF_DOCUMENT};
                    }
                }
            }
            break;
        }
        case '|':
        case '>': {
            const str_view sv {m_token_begin_itr, m_end_itr};
            const std::size_t header_end_pos = sv.find('\n');
            FK_YAML_ASSERT(header_end_pos != str_view::npos);
            const uint32_t base_indent = get_current_indent_level(&sv[header_end_pos]);

            const lexical_token_t type = *m_token_begin_itr == '|' ? lexical_token_t::BLOCK_LITERAL_SCALAR
                                                                   : lexical_token_t::BLOCK_FOLDED_SCALAR;
            const str_view header_line = sv.substr(1, header_end_pos - 1);
            m_block_scalar_header = convert_to_block_scalar_header(header_line);

            m_token_begin_itr = sv.begin() + (header_end_pos + 1);

            return {
                type,
                determine_block_scalar_content_range(
                    base_indent, m_block_scalar_header.indent, m_block_scalar_header.indent)};
        }
        default:
            break;
        }

        return {lexical_token_t::PLAIN_SCALAR, determine_plain_scalar_range()};
    }

    /// @brief Get the beginning position of a last token.
    /// @return uint32_t The beginning position of a last token.
    uint32_t get_last_token_begin_pos() const noexcept {
        return m_last_token_begin_pos;
    }

    /// @brief Get the number of lines already processed.
    /// @return uint32_t The number of lines already processed.
    uint32_t get_lines_processed() const noexcept {
        return m_last_token_begin_line;
    }

    /// @brief Get the YAML version specification.
    /// @return str_view A YAML version specification.
    str_view get_yaml_version() const noexcept {
        return m_yaml_version;
    }

    /// @brief Get the YAML tag handle defined in the TAG directive.
    /// @return str_view A tag handle.
    str_view get_tag_handle() const noexcept {
        return m_tag_handle;
    }

    /// @brief Get the YAML tag prefix defined in the TAG directive.
    /// @return str_view A tag prefix.
    str_view get_tag_prefix() const noexcept {
        return m_tag_prefix;
    }

    /// @brief Get block scalar header information.
    /// @return block_scalar_header Block scalar header information.
    block_scalar_header get_block_scalar_header() const noexcept {
        return m_block_scalar_header;
    }

    /// @brief Toggles the context state between flow and block.
    /// @param is_flow_context true: flow context, false: block context
    void set_context_state(bool is_flow_context) noexcept {
        m_state &= ~flow_context_bit;
        if (is_flow_context) {
            m_state |= flow_context_bit;
        }
    }

    /// @brief Toggles the document state between directive and content.
    /// @param is_directive true: directive, false: content
    void set_document_state(bool is_directive) noexcept {
        m_state &= ~document_directive_bit;
        if (is_directive) {
            m_state |= document_directive_bit;
        }
    }

private:
    uint32_t get_current_indent_level(const char* p_line_end) {
        // get the beginning position of the current line.
        std::size_t line_begin_pos = str_view(m_begin_itr, p_line_end - 1).find_last_of('\n');
        if (line_begin_pos == str_view::npos) {
            line_begin_pos = 0;
        }
        else {
            ++line_begin_pos;
        }
        const char* p_line_begin = m_begin_itr + line_begin_pos;
        const char* cur_itr = p_line_begin;

        // get the indentation of the current line.
        uint32_t indent = 0;
        bool indent_found = false;
        // 0: none, 1: block seq item, 2: explicit map key, 3: explicit map value
        uint32_t context = 0;
        while (cur_itr != p_line_end && !indent_found) {
            switch (*cur_itr) {
            case ' ':
                ++indent;
                ++cur_itr;
                break;
            case '-':
                switch (*(cur_itr + 1)) {
                case ' ':
                case '\t':
                    indent += 2;
                    cur_itr += 2;
                    context = 1;
                    break;
                default:
                    indent_found = true;
                    break;
                }
                break;
            case '?':
                if (*(cur_itr + 1) == ' ') {
                    indent += 2;
                    cur_itr += 2;
                    context = 2;
                    break;
                }

                indent_found = true;
                break;
            case ':':
                switch (*(cur_itr + 1)) {
                case ' ':
                case '\t':
                    indent += 2;
                    cur_itr += 2;
                    context = 3;
                    break;
                default:
                    indent_found = true;
                    break;
                }
                break;
            default:
                indent_found = true;
                break;
            }
        }

        // If "- ", "? " and/or ": " occur in the first line of this plain scalar content.
        if (context > 0) {
            // Check if the first line contains the key separator ": ".
            // If so, the indent value remains the current one.
            // Otherwise, the indent value is changed based on the last ocurrence of the above 3.
            // In any case, multiline plain scalar content must be indented more than the indent value.
            const str_view line_content_part {p_line_begin + indent, p_line_end};
            std::size_t key_sep_pos = line_content_part.find(": ");
            if (key_sep_pos == str_view::npos) {
                key_sep_pos = line_content_part.find(":\t");
            }

            if (key_sep_pos == str_view::npos) {
                constexpr char targets[] = "-?:";
                FK_YAML_ASSERT(context - 1 < sizeof(targets));
                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                const char target_char = targets[context - 1];

                // Find the position of the last ocuurence of "- ", "? " or ": ".
                const str_view line_indent_part {p_line_begin, indent};
                const std::size_t block_seq_item_begin_pos = line_indent_part.find_last_of(target_char);
                FK_YAML_ASSERT(block_seq_item_begin_pos != str_view::npos);
                indent = static_cast<uint32_t>(block_seq_item_begin_pos);
            }
        }

        return indent;
    }

    /// @brief Skip until a newline code or a null character is found.
    void scan_comment() {
        FK_YAML_ASSERT(*m_cur_itr == '#');
        if FK_YAML_LIKELY (m_cur_itr != m_begin_itr) {
            switch (*(m_cur_itr - 1)) {
            case ' ':
            case '\t':
            case '\n':
                break;
            default:
                emit_error("Comment must not begin right after non-break characters");
            }
        }
        skip_until_line_end();
    }

    /// @brief Scan directives starting with the prefix '%'
    /// @note Currently, only %YAML directive is supported. If not, returns invalid or throws an exception.
    /// @return lexical_token_t The lexical token type for directives.
    lexical_token_t scan_directive() {
        FK_YAML_ASSERT(*m_cur_itr == '%');

        m_token_begin_itr = ++m_cur_itr;

        bool ends_loop = false;
        while (!ends_loop && m_cur_itr != m_end_itr) {
            switch (*m_cur_itr) {
            case ' ':
            case '\t':
                ends_loop = true;
                break;
            case '\n':
                skip_until_line_end();
                return lexical_token_t::INVALID_DIRECTIVE;
            default:
                ++m_cur_itr;
                break;
            }
        }

        const str_view dir_name(m_token_begin_itr, m_cur_itr);

        if (dir_name == "TAG") {
            if FK_YAML_UNLIKELY (!ends_loop) {
                emit_error("There must be at least one white space between \"%TAG\" and tag info.");
            }
            skip_white_spaces();
            return scan_tag_directive();
        }

        if (dir_name == "YAML") {
            if FK_YAML_UNLIKELY (!ends_loop) {
                emit_error("There must be at least one white space between \"%YAML\" and version.");
            }
            skip_white_spaces();
            return scan_yaml_version_directive();
        }

        skip_until_line_end();
        return lexical_token_t::INVALID_DIRECTIVE;
    }

    /// @brief Scan a YAML tag directive.
    /// @return lexical_token_t The lexical token type for YAML tag directives.
    lexical_token_t scan_tag_directive() {
        m_token_begin_itr = m_cur_itr;

        //
        // extract a tag handle
        //

        if FK_YAML_UNLIKELY (*m_cur_itr != '!') {
            emit_error("Tag handle must start with \'!\'.");
        }

        if FK_YAML_UNLIKELY (++m_cur_itr == m_end_itr) {
            emit_error("invalid TAG directive is found.");
        }

        switch (*m_cur_itr) {
        case ' ':
        case '\t':
            // primary handle (!)
            break;
        case '!':
            if FK_YAML_UNLIKELY (++m_cur_itr == m_end_itr) {
                emit_error("invalid TAG directive is found.");
            }
            if FK_YAML_UNLIKELY (*m_cur_itr != ' ' && *m_cur_itr != '\t') {
                emit_error("invalid tag handle is found.");
            }
            break;
        default: {
            bool ends_loop = false;
            do {
                switch (*m_cur_itr) {
                case ' ':
                case '\t':
                    emit_error("invalid tag handle is found.");
                case '!': {
                    if (m_cur_itr + 1 == m_end_itr) {
                        ends_loop = true;
                        break;
                    }
                    const char next = *(m_cur_itr + 1);
                    if FK_YAML_UNLIKELY (next != ' ' && next != '\t') {
                        emit_error("invalid tag handle is found.");
                    }
                    ends_loop = true;
                    break;
                }
                case '-':
                    break;
                default:
                    if FK_YAML_UNLIKELY (!isalnum(*m_cur_itr)) {
                        // See https://yaml.org/spec/1.2.2/#rule-c-named-tag-handle for more details.
                        emit_error("named handle can contain only numbers(0-9), alphabets(A-Z,a-z) and hyphens(-).");
                    }
                    break;
                }

                if FK_YAML_UNLIKELY (++m_cur_itr == m_end_itr) {
                    emit_error("invalid TAG directive is found.");
                }
            } while (!ends_loop);
            break;
        }
        }

        m_tag_handle = str_view {m_token_begin_itr, m_cur_itr};

        skip_white_spaces();

        //
        // extract a tag prefix.
        //

        m_token_begin_itr = m_cur_itr;
        const char* p_tag_prefix_begin = m_cur_itr;
        switch (*m_cur_itr) {
        // a tag prefix must not start with flow indicators to avoid ambiguity.
        // See https://yaml.org/spec/1.2.2/#rule-ns-global-tag-prefix for more details.
        case ',':
        case '[':
        case ']':
        case '{':
        case '}':
            emit_error("tag prefix must not start with flow indicators (\',\', [], {}).");
        default:
            break;
        }

        // extract the rest of a tag prefix.
        bool ends_loop = false;
        do {
            switch (*m_cur_itr) {
            case ' ':
            case '\t':
            case '\n':
                ends_loop = true;
                break;
            default:
                break;
            }
        } while (!ends_loop && ++m_cur_itr != m_end_itr);

        const bool is_valid = uri_encoding::validate(p_tag_prefix_begin, m_cur_itr);
        if FK_YAML_UNLIKELY (!is_valid) {
            emit_error("invalid URI character is found in a tag prefix.");
        }

        m_tag_prefix = str_view {p_tag_prefix_begin, m_cur_itr};

        return lexical_token_t::TAG_DIRECTIVE;
    }

    /// @brief Scan a YAML version directive.
    /// @note Only 1.1 and 1.2 are supported. If not, throws an exception.
    /// @return lexical_token_t The lexical token type for YAML version directives.
    lexical_token_t scan_yaml_version_directive() {
        m_token_begin_itr = m_cur_itr;

        bool ends_loop = false;
        while (!ends_loop && m_cur_itr != m_end_itr) {
            switch (*m_cur_itr) {
            case ' ':
            case '\t':
            case '\n':
                ends_loop = true;
                break;
            default:
                ++m_cur_itr;
                break;
            }
        }

        m_yaml_version = str_view {m_token_begin_itr, m_cur_itr};

        if FK_YAML_UNLIKELY (m_yaml_version.compare("1.1") != 0 && m_yaml_version.compare("1.2") != 0) {
            emit_error("Only 1.1 and 1.2 can be specified as the YAML version.");
        }

        return lexical_token_t::YAML_VER_DIRECTIVE;
    }

    /// @brief Extracts an anchor name from the input.
    /// @return The extracted anchor name.
    str_view extract_anchor_name() {
        FK_YAML_ASSERT(*m_cur_itr == '&' || *m_cur_itr == '*');

        m_token_begin_itr = ++m_cur_itr;

        bool ends_loop = false;
        for (; m_cur_itr != m_end_itr; ++m_cur_itr) {
            switch (*m_cur_itr) {
            // anchor name must not contain white spaces, newline codes and flow indicators.
            // See https://yaml.org/spec/1.2.2/#692-node-anchors for more details.
            case ' ':
            case '\t':
            case '\n':
            case '{':
            case '}':
            case '[':
            case ']':
            case ',':
                ends_loop = true;
                break;
            default:
                break;
            }

            if (ends_loop) {
                break;
            }
        }

        if FK_YAML_UNLIKELY (m_token_begin_itr == m_cur_itr) {
            emit_error("anchor name must not be empty.");
        }

        return {m_token_begin_itr, m_cur_itr};
    }

    /// @brief Extracts a tag name from the input.
    /// @return A tag name.
    str_view extract_tag_name() {
        FK_YAML_ASSERT(*m_cur_itr == '!');

        if (++m_cur_itr == m_end_itr) {
            // Just "!" is a non-specific tag.
            return {m_token_begin_itr, m_end_itr};
        }

        bool is_verbatim = false;
        bool allows_another_tag_prefix = false;

        switch (*m_cur_itr) {
        case ' ':
        case '\n':
            // Just "!" is a non-specific tag.
            return {m_token_begin_itr, m_cur_itr};
        case '!':
            // Secondary tag handles (!!suffix)
            break;
        case '<':
            // Verbatim tags (!<TAG>)
            is_verbatim = true;
            ++m_cur_itr;
            break;
        default:
            // Either local tags (!suffix) or named handles (!tag!suffix)
            allows_another_tag_prefix = true;
            break;
        }

        bool is_named_handle = false;
        bool ends_loop = false;
        do {
            if (++m_cur_itr == m_end_itr) {
                break;
            }

            switch (*m_cur_itr) {
            // Tag names must not contain spaces or newline codes.
            case ' ':
            case '\t':
            case '\n':
                ends_loop = true;
                break;
            case '!':
                if FK_YAML_UNLIKELY (!allows_another_tag_prefix) {
                    emit_error("invalid tag prefix (!) is found.");
                }

                is_named_handle = true;
                // tag prefix must not appear three times.
                allows_another_tag_prefix = false;
                break;
            default:
                break;
            }
        } while (!ends_loop);

        str_view tag_name {m_token_begin_itr, m_cur_itr};

        if (is_verbatim) {
            const char last = tag_name.back();
            if FK_YAML_UNLIKELY (last != '>') {
                emit_error("verbatim tag (!<TAG>) must be ended with \'>\'.");
            }

            // only the `TAG` part of the `!<TAG>` for URI validation.
            const str_view tag_body = tag_name.substr(2, tag_name.size() - 3);
            if FK_YAML_UNLIKELY (tag_body.empty()) {
                emit_error("verbatim tag(!<TAG>) must not be empty.");
            }

            const bool is_valid_uri = uri_encoding::validate(tag_body.begin(), tag_body.end());
            if FK_YAML_UNLIKELY (!is_valid_uri) {
                emit_error("invalid URI character is found in a verbatim tag.");
            }

            return tag_name;
        }

        if (is_named_handle) {
            const char last = tag_name.back();
            if FK_YAML_UNLIKELY (last == '!') {
                // Tag shorthand must be followed by a non-empty suffix.
                // See the "Tag Shorthands" section in https://yaml.org/spec/1.2.2/#691-node-tags.
                emit_error("named handle has no suffix.");
            }
        }

        // get the position of last tag prefix character (!) to extract body of tag shorthands.
        // tag shorthand is either primary(!tag), secondary(!!tag) or named(!handle!tag).
        const std::size_t last_tag_prefix_pos = tag_name.find_last_of('!');
        FK_YAML_ASSERT(last_tag_prefix_pos != str_view::npos);

        const str_view tag_uri = tag_name.substr(last_tag_prefix_pos + 1);
        const bool is_valid_uri = uri_encoding::validate(tag_uri.begin(), tag_uri.end());
        if FK_YAML_UNLIKELY (!is_valid_uri) {
            emit_error("Invalid URI character is found in a named tag handle.");
        }

        // Tag shorthands cannot contain flow indicators({}[],).
        // See the "Tag Shorthands" section in https://yaml.org/spec/1.2.2/#691-node-tags.
        const std::size_t invalid_char_pos = tag_uri.find_first_of("{}[],");
        if (invalid_char_pos != str_view::npos) {
            emit_error("Tag shorthand cannot contain flow indicators({}[],).");
        }

        return tag_name;
    }

    /// @brief Determines the range of single quoted scalar by scanning remaining input buffer contents.
    /// @return A single quoted scalar.
    str_view determine_single_quoted_scalar_range() {
        const str_view sv {m_token_begin_itr, m_end_itr};

        std::size_t pos = sv.find('\'');
        while (pos != str_view::npos) {
            FK_YAML_ASSERT(pos < sv.size());
            if FK_YAML_LIKELY (pos == sv.size() - 1 || sv[pos + 1] != '\'') {
                // closing single quote is found.
                m_cur_itr = m_token_begin_itr + (pos + 1);
                str_view single_quoted_scalar {m_token_begin_itr, pos};
                check_scalar_content(single_quoted_scalar);
                return single_quoted_scalar;
            }

            // If single quotation marks are repeated twice in a single quoted scalar, they are considered as an
            // escaped single quotation mark. Skip the second one which would otherwise be detected as a closing
            // single quotation mark in the next loop.
            pos = sv.find('\'', pos + 2);
        }

        m_cur_itr = m_end_itr; // update for error information
        emit_error("Invalid end of input buffer in a single-quoted scalar token.");
    }

    /// @brief Determines the range of double quoted scalar by scanning remaining input buffer contents.
    /// @return A double quoted scalar.
    str_view determine_double_quoted_scalar_range() {
        const str_view sv {m_token_begin_itr, m_end_itr};

        std::size_t pos = sv.find('\"');
        while (pos != str_view::npos) {
            FK_YAML_ASSERT(pos < sv.size());

            bool is_closed = true;
            if FK_YAML_LIKELY (pos > 0) {
                // Double quotation marks can be escaped by a preceding backslash and the number of backslashes matters
                // to determine if the found double quotation mark is escaped since the backslash itself can also be
                // escaped:
                // * odd number of backslashes  -> double quotation mark IS escaped (e.g., "\\\"")
                // * even number of backslashes -> double quotation mark IS NOT escaped (e.g., "\\"")
                uint32_t backslash_counts = 0;
                const char* p = m_token_begin_itr + (pos - 1);
                do {
                    if (*p-- != '\\') {
                        break;
                    }
                    ++backslash_counts;
                } while (p != m_token_begin_itr);
                is_closed = ((backslash_counts & 1u) == 0); // true: even, false: odd
            }

            if (is_closed) {
                // closing double quote is found.
                m_cur_itr = m_token_begin_itr + (pos + 1);
                str_view double_quoted_salar {m_token_begin_itr, pos};
                check_scalar_content(double_quoted_salar);
                return double_quoted_salar;
            }

            pos = sv.find('\"', pos + 1);
        }

        m_cur_itr = m_end_itr; // update for error information
        emit_error("Invalid end of input buffer in a double-quoted scalar token.");
    }

    /// @brief Determines the range of plain scalar by scanning remaining input buffer contents.
    /// @return A plain scalar.
    str_view determine_plain_scalar_range() {
        const str_view sv {m_token_begin_itr, m_end_itr};

        // flow indicators are checked only within a flow context.
        const str_view filter = (m_state & flow_context_bit) ? "\n :{}[]," : "\n :";
        std::size_t pos = sv.find_first_of(filter);
        if FK_YAML_UNLIKELY (pos == str_view::npos) {
            check_scalar_content(sv);
            m_cur_itr = m_end_itr;
            return sv;
        }

        bool ends_loop = false;
        uint32_t indent = std::numeric_limits<uint32_t>::max();
        do {
            FK_YAML_ASSERT(pos < sv.size());
            switch (sv[pos]) {
            case '\n': {
                if (indent == std::numeric_limits<uint32_t>::max()) {
                    indent = get_current_indent_level(&sv[pos]);
                }

                constexpr str_view space_filter {" \t\n"};
                const std::size_t non_space_pos = sv.find_first_not_of(space_filter, pos);
                const std::size_t last_newline_pos = sv.find_last_of('\n', non_space_pos);
                FK_YAML_ASSERT(last_newline_pos != str_view::npos);

                if (non_space_pos == str_view::npos || non_space_pos - last_newline_pos - 1 <= indent) {
                    ends_loop = true;
                    break;
                }

                pos = non_space_pos;
                break;
            }
            case ' ':
                if FK_YAML_UNLIKELY (pos == sv.size() - 1) {
                    // trim trailing space.
                    ends_loop = true;
                    break;
                }

                // Allow a space in a plain scalar only if the space is surrounded by non-space characters, but not
                // followed by the comment prefix " #".
                // Also, flow indicators are not allowed to be followed after a space in a flow context.
                // See https://yaml.org/spec/1.2.2/#733-plain-style for more details.
                switch (sv[pos + 1]) {
                case ' ':
                case '\t':
                case '\n':
                case '#':
                    ends_loop = true;
                    break;
                case ':':
                    // " :" is permitted in a plain style string token, but not when followed by a space.
                    ends_loop = (pos < sv.size() - 2) && (sv[pos + 2] == ' ');
                    break;
                case '{':
                case '}':
                case '[':
                case ']':
                case ',':
                    ends_loop = (m_state & flow_context_bit);
                    break;
                default:
                    break;
                }
                break;
            case ':':
                if FK_YAML_LIKELY (pos + 1 < sv.size()) {
                    switch (sv[pos + 1]) {
                    case ' ':
                    case '\t':
                    case '\n':
                        ends_loop = true;
                        break;
                    default:
                        break;
                    }
                }
                break;
            case '{':
            case '}':
            case '[':
            case ']':
            case ',':
                // This check is enabled only in a flow context.
                ends_loop = true;
                break;
            default:                   // LCOV_EXCL_LINE
                detail::unreachable(); // LCOV_EXCL_LINE
            }

            if (ends_loop) {
                break;
            }

            pos = sv.find_first_of(filter, pos + 1);
        } while (pos != str_view::npos);

        str_view plain_scalar = sv.substr(0, pos);
        check_scalar_content(plain_scalar);
        m_cur_itr = plain_scalar.end();
        return plain_scalar;
    }

    /// @brief Scan a block style string token either in the literal or folded style.
    /// @param base_indent The base indent level of the block scalar.
    /// @param indicated_indent The indicated indent level in the block scalar header. 0 means it's not indicated.
    /// @param token Storage for the scanned block scalar range.
    /// @return The content indentation level of the block scalar.
    str_view determine_block_scalar_content_range(
        uint32_t base_indent, uint32_t indicated_indent, uint32_t& content_indent) {
        const str_view sv {m_token_begin_itr, m_end_itr};
        const std::size_t remain_input_len = sv.size();

        // Handle leading all-space lines.
        uint32_t cur_indent = 0;
        uint32_t max_leading_indent = 0;
        const char* cur_itr = m_token_begin_itr;
        bool stop_increment = false;

        while (cur_itr != m_end_itr) {
            switch (*cur_itr++) {
            case ' ':
                if FK_YAML_LIKELY (!stop_increment) {
                    ++cur_indent;
                }
                continue;
            case '\t':
                // Tabs are not counted as an indent character but still part of an empty line.
                // See https://yaml.org/spec/1.2.2/#rule-s-indent and https://yaml.org/spec/1.2.2/#64-empty-lines.
                stop_increment = true;
                continue;
            case '\n':
                max_leading_indent = std::max(cur_indent, max_leading_indent);
                cur_indent = 0;
                stop_increment = false;
                continue;
            default:
                break;
            }
            break;
        }

        // all the block scalar contents are empty lines, and no subsequent token exists.
        if FK_YAML_UNLIKELY (cur_itr == m_end_itr) {
            // Without the following iterator update, lexer cannot reach the end of input buffer and causes infinite
            // loops from the next loop. (https://github.com/fktn-k/fkYAML/pull/410)
            m_cur_itr = m_end_itr;

            // If there's no non-empty line, the content indentation level is equal to the number of spaces on the
            // longest line. https://yaml.org/spec/1.2.2/#8111-block-indentation-indicator
            content_indent =
                indicated_indent == 0 ? std::max(cur_indent, max_leading_indent) : base_indent + indicated_indent;
            return sv;
        }

        // Any leading empty line must not contain more spaces than the first non-empty line.
        if FK_YAML_UNLIKELY (cur_indent < max_leading_indent) {
            emit_error("Any leading empty line must not be more indented than the first non-empty line.");
        }

        if (indicated_indent == 0) {
            FK_YAML_ASSERT(base_indent < cur_indent);
            indicated_indent = cur_indent - base_indent;
        }
        else if FK_YAML_UNLIKELY (cur_indent < base_indent + indicated_indent) {
            emit_error("The first non-empty line in the block scalar is less indented.");
        }

        std::size_t last_newline_pos = sv.find('\n', cur_itr - m_token_begin_itr + 1);
        if (last_newline_pos == str_view::npos) {
            last_newline_pos = remain_input_len;
        }

        content_indent = base_indent + indicated_indent;
        while (last_newline_pos < remain_input_len) {
            std::size_t cur_line_end_pos = sv.find('\n', last_newline_pos + 1);
            if (cur_line_end_pos == str_view::npos) {
                cur_line_end_pos = remain_input_len;
            }

            const std::size_t cur_line_content_begin_pos = sv.find_first_not_of(' ', last_newline_pos + 1);
            if (cur_line_content_begin_pos == str_view::npos) {
                last_newline_pos = cur_line_end_pos;
                continue;
            }

            FK_YAML_ASSERT(last_newline_pos < cur_line_content_begin_pos);
            cur_indent = static_cast<uint32_t>(cur_line_content_begin_pos - last_newline_pos - 1);
            if (cur_indent < content_indent && sv[cur_line_content_begin_pos] != '\n') {
                if FK_YAML_UNLIKELY (cur_indent > base_indent) {
                    // This path assumes an input like the following:
                    // ```yaml
                    // foo: |
                    //   text
                    //  invalid # this line is less indented than the content indent level (2)
                    //          # but more indented than the base indent level (0)
                    // ```
                    // In such cases, the less indented line cannot be the start of the next token.
                    emit_error("A content line of the block scalar is less indented.");
                }

                // Interpret less indented non-space characters as the start of the next token.
                break;
            }

            last_newline_pos = cur_line_end_pos;
        }

        // include last newline character if not all characters have been consumed yet.
        if (last_newline_pos < remain_input_len) {
            ++last_newline_pos;
        }

        m_cur_itr = m_token_begin_itr + last_newline_pos;
        return sv.substr(0, last_newline_pos);
    }

    /// @brief Checks if the given scalar contains no unescaped control characters.
    /// @param scalar Scalar contents.
    void check_scalar_content(const str_view& scalar) const {
        const char* p_current = scalar.begin();
        const char* p_end = scalar.end();

        while (p_current != p_end) {
            const uint32_t num_bytes = utf8::get_num_bytes(static_cast<uint8_t>(*p_current));
            if (num_bytes > 1) {
                // Multibyte characters are already checked in the input_adapter module.
                p_current += num_bytes;
                continue;
            }

            switch (*p_current++) {
            // 0x00(NULL) has already been handled above.
            case 0x01:
                emit_error("Control character U+0001 (SOH) must be escaped to \\u0001.");
            case 0x02:
                emit_error("Control character U+0002 (STX) must be escaped to \\u0002.");
            case 0x03:
                emit_error("Control character U+0003 (ETX) must be escaped to \\u0003.");
            case 0x04:
                emit_error("Control character U+0004 (EOT) must be escaped to \\u0004.");
            case 0x05:
                emit_error("Control character U+0005 (ENQ) must be escaped to \\u0005.");
            case 0x06:
                emit_error("Control character U+0006 (ACK) must be escaped to \\u0006.");
            case 0x07:
                emit_error("Control character U+0007 (BEL) must be escaped to \\a or \\u0007.");
            case 0x08:
                emit_error("Control character U+0008 (BS) must be escaped to \\b or \\u0008.");
            case 0x09: // HT
                // horizontal tabs (\t) are safe to use without escaping.
                break;
            // 0x0A(LF) has already been handled above.
            case 0x0B:
                emit_error("Control character U+000B (VT) must be escaped to \\v or \\u000B.");
            case 0x0C:
                emit_error("Control character U+000C (FF) must be escaped to \\f or \\u000C.");
            // 0x0D(CR) has already been handled above.
            case 0x0E:
                emit_error("Control character U+000E (SO) must be escaped to \\u000E.");
            case 0x0F:
                emit_error("Control character U+000F (SI) must be escaped to \\u000F.");
            case 0x10:
                emit_error("Control character U+0010 (DLE) must be escaped to \\u0010.");
            case 0x11:
                emit_error("Control character U+0011 (DC1) must be escaped to \\u0011.");
            case 0x12:
                emit_error("Control character U+0012 (DC2) must be escaped to \\u0012.");
            case 0x13:
                emit_error("Control character U+0013 (DC3) must be escaped to \\u0013.");
            case 0x14:
                emit_error("Control character U+0014 (DC4) must be escaped to \\u0014.");
            case 0x15:
                emit_error("Control character U+0015 (NAK) must be escaped to \\u0015.");
            case 0x16:
                emit_error("Control character U+0016 (SYN) must be escaped to \\u0016.");
            case 0x17:
                emit_error("Control character U+0017 (ETB) must be escaped to \\u0017.");
            case 0x18:
                emit_error("Control character U+0018 (CAN) must be escaped to \\u0018.");
            case 0x19:
                emit_error("Control character U+0019 (EM) must be escaped to \\u0019.");
            case 0x1A:
                emit_error("Control character U+001A (SUB) must be escaped to \\u001A.");
            case 0x1B:
                emit_error("Control character U+001B (ESC) must be escaped to \\e or \\u001B.");
            case 0x1C:
                emit_error("Control character U+001C (FS) must be escaped to \\u001C.");
            case 0x1D:
                emit_error("Control character U+001D (GS) must be escaped to \\u001D.");
            case 0x1E:
                emit_error("Control character U+001E (RS) must be escaped to \\u001E.");
            case 0x1F:
                emit_error("Control character U+001F (US) must be escaped to \\u001F.");
            default:
                break;
            }
        }
    }

    /// @brief Gets the metadata of a following block style string scalar.
    /// @param chomp_type A variable to store the retrieved chomping style type.
    /// @param indent A variable to store the retrieved indent size.
    /// @return Block scalar header information converted from the header line.
    block_scalar_header convert_to_block_scalar_header(str_view line) {
        constexpr str_view comment_prefix {" #"};
        const std::size_t comment_begin_pos = line.find(comment_prefix);
        if (comment_begin_pos != str_view::npos) {
            line = line.substr(0, comment_begin_pos);
        }

        if (line.empty()) {
            return {};
        }

        block_scalar_header header {};
        for (const char c : line) {
            switch (c) {
            case '-':
                if FK_YAML_UNLIKELY (header.chomp != chomping_indicator_t::CLIP) {
                    emit_error("Too many block chomping indicators specified.");
                }
                header.chomp = chomping_indicator_t::STRIP;
                break;
            case '+':
                if FK_YAML_UNLIKELY (header.chomp != chomping_indicator_t::CLIP) {
                    emit_error("Too many block chomping indicators specified.");
                }
                header.chomp = chomping_indicator_t::KEEP;
                break;
            case '0':
                emit_error("An indentation level for a block scalar cannot be 0.");
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if FK_YAML_UNLIKELY (header.indent > 0) {
                    emit_error("Invalid indentation level for a block scalar. It must be between 1 and 9.");
                }
                header.indent = static_cast<uint32_t>(c - '0');
                break;
            case ' ':
            case '\t':
                break;
            default:
                emit_error("Invalid character found in a block scalar header.");
            }
        }

        return header;
    }

    /// @brief Skip white spaces (half-width spaces and tabs) from the current position.
    void skip_white_spaces() {
        m_cur_itr = std::find_if_not(m_cur_itr, m_end_itr, [](char c) { return (c == ' ' || c == '\t'); });
    }

    /// @brief Skip white spaces and newline codes (CR/LF) from the current position.
    void skip_white_spaces_and_newline_codes() {
        if (m_cur_itr != m_end_itr) {
            m_cur_itr = std::find_if_not(m_cur_itr, m_end_itr, [](char c) {
                switch (c) {
                case ' ':
                case '\t':
                case '\n':
                    return true;
                default:
                    return false;
                }
            });
        }
    }

    /// @brief Skip the rest in the current line.
    void skip_until_line_end() {
        while (m_cur_itr != m_end_itr) {
            switch (*m_cur_itr) {
            case '\n':
                ++m_cur_itr;
                return;
            default:
                ++m_cur_itr;
                break;
            }
        }
    }

    /// @brief Emits an error with the given message.
    /// @param msg A message for the resulting error.
    [[noreturn]] void emit_error(const char* msg) const {
        m_pos_tracker.update_position(m_cur_itr);
        throw fkyaml::parse_error(msg, m_pos_tracker.get_lines_read(), m_pos_tracker.get_cur_pos_in_line());
    }

private:
    /// The iterator to the first element in the input buffer.
    const char* m_begin_itr {};
    /// The iterator to the current character in the input buffer.
    const char* m_cur_itr {};
    /// The iterator to the beginning of the current token.
    const char* m_token_begin_itr {};
    /// The iterator to the past-the-end element in the input buffer.
    const char* m_end_itr {};
    /// The current position tracker of the input buffer.
    mutable position_tracker m_pos_tracker {};
    /// The last yaml version.
    str_view m_yaml_version;
    /// The last tag handle.
    str_view m_tag_handle;
    /// The last tag prefix.
    str_view m_tag_prefix;
    /// The last block scalar header.
    block_scalar_header m_block_scalar_header {};
    /// The beginning position of the last lexical token. (zero origin)
    uint32_t m_last_token_begin_pos {0};
    /// The beginning line of the last lexical token. (zero origin)
    uint32_t m_last_token_begin_line {0};
    /// The current depth of flow context.
    uint32_t m_state {0};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_LEXICAL_ANALYZER_HPP */
