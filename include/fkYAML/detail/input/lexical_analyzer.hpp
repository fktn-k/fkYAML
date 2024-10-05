//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.12
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_INPUT_LEXICAL_ANALYZER_HPP
#define FK_YAML_DETAIL_INPUT_LEXICAL_ANALYZER_HPP

#include <algorithm>
#include <cctype>
#include <cstdlib>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/assert.hpp>
#include <fkYAML/detail/encodings/uri_encoding.hpp>
#include <fkYAML/detail/encodings/utf_encodings.hpp>
#include <fkYAML/detail/encodings/yaml_escaper.hpp>
#include <fkYAML/detail/input/position_tracker.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/str_view.hpp>
#include <fkYAML/detail/types/lexical_token_t.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

struct lexical_token {
    lexical_token_t type {lexical_token_t::END_OF_BUFFER};
    str_view str {};
};

namespace {

// whether the current context is flow(1) or block(0)
const uint32_t flow_context_bit = 1u << 0u;
// whether the current document part is directive(1) or content(0)
const uint32_t document_directive_bit = 1u << 1u;

} // anonymous namespace

/// @brief A class which lexically analyzes YAML formatted inputs.
class lexical_analyzer {
private:
    enum class block_style_indicator_t {
        LITERAL, //!< keeps newlines inside the block as they are indicated by a pipe `|`.
        FOLDED,  //!< replaces newlines inside the block with spaces indicated by a right angle bracket `>`.
    };

    enum class chomping_indicator_t {
        STRIP, //!< excludes final line breaks and trailing empty lines indicated by `-`.
        KEEP,  //!< preserves final line breaks but excludes trailing empty lines. no indicator means this type.
        CLIP,  //!< preserves final line breaks and trailing empty lines indicated by `+`.
    };

public:
    /// @brief Construct a new lexical_analyzer object.
    /// @tparam InputAdapterType The type of the input adapter.
    /// @param input_adapter An input adapter object.
    explicit lexical_analyzer(str_view input_buffer) noexcept
        : m_input_buffer(input_buffer),
          m_cur_itr(m_input_buffer.begin()),
          m_end_itr(m_input_buffer.end()) {
        m_pos_tracker.set_target_buffer(m_input_buffer);
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
            return {};
        }

        lexical_token token {};
        token.type = lexical_token_t::PLAIN_SCALAR;

        switch (*m_cur_itr) {
        case '?':
            if (++m_cur_itr == m_end_itr) {
                token.str = str_view {m_token_begin_itr, m_end_itr};
                return token;
            }

            switch (*m_cur_itr) {
            case ' ':
                token.type = lexical_token_t::EXPLICIT_KEY_PREFIX;
                return token;
            default:
                scan_scalar(token);
                return token;
            }
        case ':': { // key separator
            if (++m_cur_itr == m_end_itr) {
                token.type = lexical_token_t::KEY_SEPARATOR;
                return token;
            }

            switch (*m_cur_itr) {
            case ' ':
            case '\t':
            case '\n':
                break;
            case ',':
            case '[':
            case ']':
            case '{':
            case '}':
                if (m_state & flow_context_bit) {
                    // the above characters are not "safe" to be followed in a flow context.
                    // See https://yaml.org/spec/1.2.2/#733-plain-style for more details.
                    break;
                }
                scan_scalar(token);
                return token;
            default:
                scan_scalar(token);
                return token;
            }

            token.type = lexical_token_t::KEY_SEPARATOR;
            return token;
        }
        case ',': // value separator
            ++m_cur_itr;
            token.type = lexical_token_t::VALUE_SEPARATOR;
            return token;
        case '&': { // anchor prefix
            extract_anchor_name(token);
            bool is_empty = token.str.empty();
            if FK_YAML_UNLIKELY (is_empty) {
                emit_error("anchor name must not be empty.");
            }

            token.type = lexical_token_t::ANCHOR_PREFIX;
            return token;
        }
        case '*': { // alias prefix
            extract_anchor_name(token);
            bool is_empty = token.str.empty();
            if FK_YAML_UNLIKELY (is_empty) {
                emit_error("anchor name must not be empty.");
            }

            token.type = lexical_token_t::ALIAS_PREFIX;
            return token;
        }
        case '!':
            extract_tag_name(token);
            token.type = lexical_token_t::TAG_PREFIX;
            return token;
        case '#': // comment prefix
            scan_comment();
            return get_next_token();
        case '%': // directive prefix
            if (m_state & document_directive_bit) {
                token.type = scan_directive();
            }
            else {
                // The '%' character can be safely used as the first character in document contents.
                // See https://yaml.org/spec/1.2.2/#912-document-markers for more details.
                scan_scalar(token);
            }
            return token;
        case '-': {
            char next = *(m_cur_itr + 1);
            switch (next) {
            case ' ':
            case '\t':
            case '\n':
                // Move a cursor to the beginning of the next token.
                m_cur_itr += 2;
                token.type = lexical_token_t::SEQUENCE_BLOCK_PREFIX;
                return token;
            default:
                break;
            }

            bool is_available = ((m_end_itr - m_cur_itr) > 2);
            if (is_available) {
                bool is_dir_end = std::equal(m_token_begin_itr, m_cur_itr + 3, "---");
                if (is_dir_end) {
                    m_cur_itr += 3;
                    token.type = lexical_token_t::END_OF_DIRECTIVES;
                    return token;
                }
            }

            scan_scalar(token);
            return token;
        }
        case '[': // sequence flow begin
            ++m_cur_itr;
            token.type = lexical_token_t::SEQUENCE_FLOW_BEGIN;
            return token;
        case ']': // sequence flow end
            ++m_cur_itr;
            token.type = lexical_token_t::SEQUENCE_FLOW_END;
            return token;
        case '{': // mapping flow begin
            ++m_cur_itr;
            token.type = lexical_token_t::MAPPING_FLOW_BEGIN;
            return token;
        case '}': // mapping flow end
            ++m_cur_itr;
            token.type = lexical_token_t::MAPPING_FLOW_END;
            return token;
        case '@':
            emit_error("Any token cannot start with at(@). It is a reserved indicator for YAML.");
        case '`':
            emit_error("Any token cannot start with grave accent(`). It is a reserved indicator for YAML.");
        case '\"':
        case '\'':
            scan_scalar(token);
            return token;
        case '+':
            scan_scalar(token);
            return token;
        case '.': {
            bool is_available = ((m_end_itr - m_cur_itr) > 2);
            if (is_available) {
                bool is_doc_end = std::equal(m_cur_itr, m_cur_itr + 3, "...");
                if (is_doc_end) {
                    m_cur_itr += 3;
                    token.type = lexical_token_t::END_OF_DOCUMENT;
                    return token;
                }
            }

            scan_scalar(token);
            return token;
        }
        case '|': {
            chomping_indicator_t chomp_type = chomping_indicator_t::KEEP;
            uint32_t indent = 0;
            ++m_cur_itr;
            get_block_style_metadata(chomp_type, indent);
            scan_block_style_string_token(block_style_indicator_t::LITERAL, chomp_type, indent);
            token.type = lexical_token_t::BLOCK_SCALAR;
            token.str = m_value_buffer;
            return token;
        }
        case '>': {
            chomping_indicator_t chomp_type = chomping_indicator_t::KEEP;
            uint32_t indent = 0;
            ++m_cur_itr;
            get_block_style_metadata(chomp_type, indent);
            scan_block_style_string_token(block_style_indicator_t::FOLDED, chomp_type, indent);
            token.type = lexical_token_t::BLOCK_SCALAR;
            token.str = m_value_buffer;
            return token;
        }
        default:
            scan_scalar(token);
            return token;
        }
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
    /// @brief Skip until a newline code or a null character is found.
    void scan_comment() {
        FK_YAML_ASSERT(*m_cur_itr == '#');
        skip_until_line_end();
    }

    /// @brief Scan directives starting with the prefix '%'
    /// @note Currently, only %YAML directive is supported. If not, returns invalid or throws an exception.
    /// @return lexical_token_t The lexical token type for directives.
    lexical_token_t scan_directive() {
        FK_YAML_ASSERT(*m_cur_itr == '%');

        m_value_buffer.clear();
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

        str_view dir_name(m_token_begin_itr, m_cur_itr);

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
                    char next = *(m_cur_itr + 1);
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
            }
        } while (!ends_loop && ++m_cur_itr != m_end_itr);

        bool is_valid = uri_encoding::validate(p_tag_prefix_begin, m_cur_itr);
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
    /// @param token The token into which the extraction result is written.
    void extract_anchor_name(lexical_token& token) {
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

        token.str = str_view {m_token_begin_itr, m_cur_itr};
    }

    /// @brief Extracts a tag name from the input.
    /// @param token The token into which the extraction result is written.
    void extract_tag_name(lexical_token& token) {
        FK_YAML_ASSERT(*m_cur_itr == '!');

        if (++m_cur_itr == m_end_itr) {
            // Just "!" is a non-specific tag.
            token.str = str_view {m_token_begin_itr, m_end_itr};
            return;
        }

        bool is_verbatim = false;
        bool allows_another_tag_prefix = false;

        switch (*m_cur_itr) {
        case ' ':
        case '\n':
            // Just "!" is a non-specific tag.
            token.str = str_view {m_token_begin_itr, m_cur_itr};
            return;
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

        token.str = str_view {m_token_begin_itr, m_cur_itr};

        if (is_verbatim) {
            char last = token.str.back();
            if FK_YAML_UNLIKELY (last != '>') {
                emit_error("verbatim tag (!<TAG>) must be ended with \'>\'.");
            }

            // only the `TAG` part of the `!<TAG>` for URI validation.
            str_view tag_body = token.str.substr(2, token.str.size() - 3);
            if FK_YAML_UNLIKELY (tag_body.empty()) {
                emit_error("verbatim tag(!<TAG>) must not be empty.");
            }

            bool is_valid_uri = uri_encoding::validate(tag_body.begin(), tag_body.end());
            if FK_YAML_UNLIKELY (!is_valid_uri) {
                emit_error("invalid URI character is found in a verbatim tag.");
            }

            return;
        }

        if (is_named_handle) {
            char last = token.str.back();
            if FK_YAML_UNLIKELY (last == '!') {
                // Tag shorthand must be followed by a non-empty suffix.
                // See the "Tag Shorthands" section in https://yaml.org/spec/1.2.2/#691-node-tags.
                emit_error("named handle has no suffix.");
            }
        }

        // get the position of last tag prefix character (!) to extract body of tag shorthands.
        // tag shorthand is either primary(!tag), secondary(!!tag) or named(!handle!tag).
        std::size_t last_tag_prefix_pos = token.str.find_last_of('!');
        FK_YAML_ASSERT(last_tag_prefix_pos != str_view::npos);

        str_view tag_uri = token.str.substr(last_tag_prefix_pos + 1);
        bool is_valid_uri = uri_encoding::validate(tag_uri.begin(), tag_uri.end());
        if FK_YAML_UNLIKELY (!is_valid_uri) {
            emit_error("Invalid URI character is found in a named tag handle.");
        }

        // Tag shorthands cannot contain flow indicators({}[],).
        // See the "Tag Shorthands" section in https://yaml.org/spec/1.2.2/#691-node-tags.
        std::size_t invalid_char_pos = tag_uri.find_first_of("{}[],");
        if (invalid_char_pos != str_view::npos) {
            emit_error("Tag shorthand cannot contain flow indicators({}[],).");
        }
    }

    /// @brief Scan a scalar token, either plain, single-quoted or double-quoted.
    /// @param token The token into which the scan result is written.
    /// @return lexical_token_t The lexical token type for strings.
    void scan_scalar(lexical_token& token) {
        switch (*m_token_begin_itr) {
        case '\'':
            ++m_token_begin_itr;
            token.type = lexical_token_t::SINGLE_QUOTED_SCALAR;
            determine_single_quoted_scalar_range(token.str);
            break;
        case '\"':
            ++m_token_begin_itr;
            token.type = lexical_token_t::DOUBLE_QUOTED_SCALAR;
            determine_double_quoted_scalar_range(token.str);
            break;
        default:
            token.type = lexical_token_t::PLAIN_SCALAR;
            determine_plain_scalar_range(token.str);
            break;
        }

        for (const auto c : token.str) {
            if FK_YAML_UNLIKELY (0 <= c && c < 0x20) {
                handle_unescaped_control_char(c);
            }
        }
    }

    void determine_single_quoted_scalar_range(str_view& token) {
        str_view sv {m_token_begin_itr, m_end_itr};

        std::size_t pos = sv.find('\'');
        while (pos != str_view::npos) {
            FK_YAML_ASSERT(pos < sv.size());
            if FK_YAML_LIKELY (pos == sv.size() - 1 || sv[pos + 1] != '\'') {
                // closing single quote is found.
                token = {m_token_begin_itr, pos};
                m_cur_itr = sv.begin() + (pos + 1);
                return;
            }

            // If single quotation marks are repeated twice in a single quoted scalar, they are considered as an
            // escaped single quotation mark. Skip the second one which would otherwise be detected as a closing
            // single quotation mark in the next loop.
            pos = sv.find('\'', pos + 2);
        }

        m_cur_itr = m_end_itr; // update for error information
        emit_error("Invalid end of input buffer in a single-quoted scalar token.");
    }

    void determine_double_quoted_scalar_range(str_view& token) {
        str_view sv {m_token_begin_itr, m_end_itr};

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
                token = {m_token_begin_itr, pos};
                m_cur_itr = sv.begin() + (pos + 1);
                return;
            }

            pos = sv.find('\"', pos + 1);
        }

        m_cur_itr = m_end_itr; // update for error information
        emit_error("Invalid end of input buffer in a double-quoted scalar token.");
    }

    void determine_plain_scalar_range(str_view& token) {
        str_view sv {m_token_begin_itr, m_end_itr};

        str_view check_filter {"\n :"};
        if (m_state & flow_context_bit) {
            check_filter = "\n :{}[],";
        }

        std::size_t pos = sv.find_first_of(check_filter);
        if FK_YAML_UNLIKELY (pos == str_view::npos) {
            token = sv;
            m_cur_itr = m_end_itr;
            return;
        }

        bool ends_loop = false;
        do {
            FK_YAML_ASSERT(pos < sv.size());
            switch (sv[pos]) {
            case '\n':
                ends_loop = true;
                break;
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
            default:   // LCOV_EXCL_LINE
                break; // LCOV_EXCL_LINE
            }

            if (ends_loop) {
                break;
            }

            pos = sv.find_first_of(check_filter, pos + 1);
        } while (pos != str_view::npos);

        token = sv.substr(0, pos);
        m_cur_itr = token.end();
    }

    /// @brief Scan a block style string token either in the literal or folded style.
    /// @param style The style of the given token, either literal or folded.
    /// @param chomp The chomping indicator type of the given token, either strip, keep or clip.
    /// @param indent The indent size specified for the given token.
    void scan_block_style_string_token(block_style_indicator_t style, chomping_indicator_t chomp, uint32_t indent) {
        m_value_buffer.clear();

        // Handle leading all-space lines.
        for (char current = 0; m_cur_itr != m_end_itr; ++m_cur_itr) {
            current = *m_cur_itr;

            if (current == ' ') {
                continue;
            }

            if (current == '\n') {
                m_value_buffer.push_back('\n');
                continue;
            }

            break;
        }

        if (m_cur_itr == m_end_itr) {
            if (chomp != chomping_indicator_t::KEEP) {
                m_value_buffer.clear();
            }
            return;
        }

        m_pos_tracker.update_position(m_cur_itr);
        uint32_t cur_indent = m_pos_tracker.get_cur_pos_in_line();

        // TODO: preserve and compare the last indentation with `cur_indent`
        if (indent == 0) {
            indent = cur_indent;
        }
        else if FK_YAML_UNLIKELY (cur_indent < indent) {
            emit_error("A block style scalar is less indented than the indicated level.");
        }

        uint32_t chars_in_line = 0;
        bool is_extra_indented = false;
        m_token_begin_itr = m_cur_itr;
        if (cur_indent > indent) {
            if (style == block_style_indicator_t::FOLDED) {
                m_value_buffer.push_back('\n');
                is_extra_indented = true;
            }

            uint32_t diff = cur_indent - indent;
            m_token_begin_itr -= diff;
            chars_in_line = diff;
        }

        for (; m_cur_itr != m_end_itr; ++m_cur_itr) {
            char current = *m_cur_itr;
            if (current == '\n') {
                if (style == block_style_indicator_t::LITERAL) {
                    if (chars_in_line == 0) {
                        m_value_buffer.push_back('\n');
                    }
                    else {
                        m_value_buffer.append(m_token_begin_itr, m_cur_itr + 1);
                    }
                }
                // block_style_indicator_t::FOLDED
                else if (chars_in_line == 0) {
                    // Just append a newline if the current line is empty.
                    m_value_buffer.push_back('\n');
                }
                else if (is_extra_indented) {
                    // A line being more indented is not folded.
                    m_value_buffer.append(m_token_begin_itr, m_cur_itr + 1);
                }
                else {
                    m_value_buffer.append(m_token_begin_itr, m_cur_itr);

                    // Append a newline if the next line is empty.
                    bool is_end_of_token = false;
                    bool is_next_empty = false;
                    for (uint32_t i = 0; i < indent; i++) {
                        if (++m_cur_itr == m_end_itr) {
                            is_end_of_token = true;
                            break;
                        }

                        char c = *m_cur_itr;
                        if (c == ' ') {
                            continue;
                        }

                        if (c == '\n') {
                            is_next_empty = true;
                            break;
                        }

                        is_end_of_token = true;
                        break;
                    }

                    if (is_end_of_token) {
                        m_value_buffer.push_back('\n');
                        chars_in_line = 0;
                        break;
                    }

                    if (is_next_empty) {
                        m_value_buffer.push_back('\n');
                        chars_in_line = 0;
                        continue;
                    }

                    switch (char next = *(m_cur_itr + 1)) {
                    case '\n':
                        ++m_cur_itr;
                        m_value_buffer.push_back(next);
                        break;
                    case ' ':
                        // The next line is more indented, so a newline will be appended in the coming loops.
                        break;
                    default:
                        m_value_buffer.push_back(' ');
                        break;
                    }
                }

                // Reset the values for the next line.
                m_token_begin_itr = m_cur_itr + 1;
                chars_in_line = 0;
                is_extra_indented = false;

                continue;
            }

            // Handle indentation
            if (chars_in_line == 0) {
                m_pos_tracker.update_position(m_cur_itr);
                cur_indent = m_pos_tracker.get_cur_pos_in_line();
                if (cur_indent < indent) {
                    if (current != ' ') {
                        // Interpret less indented non-space characters as the start of the next token.
                        break;
                    }
                    // skip a space if not yet indented enough
                    continue;
                }

                if (current == ' ' && style == block_style_indicator_t::FOLDED) {
                    // A line being more indented is not folded.
                    m_value_buffer.push_back('\n');
                    is_extra_indented = true;
                }
                m_token_begin_itr = m_cur_itr;
            }

            ++chars_in_line;
        }

        if (chars_in_line > 0) {
            m_value_buffer.append(m_token_begin_itr, m_cur_itr);
        }

        // Manipulate the trailing line endings chomping indicator type.
        switch (chomp) {
        case chomping_indicator_t::STRIP:
            while (!m_value_buffer.empty()) {
                // Empty strings are handled above, so no check for the case.
                char last = m_value_buffer.back();
                if (last != '\n') {
                    break;
                }
                m_value_buffer.pop_back();
            }
            break;
        case chomping_indicator_t::CLIP: {
            char last = m_value_buffer.back();
            if (last != '\n') {
                // No need to chomp the trailing newlines.
                break;
            }
            uint32_t buf_size = static_cast<uint32_t>(m_value_buffer.size());
            while (buf_size > 1) {
                // Strings with only newlines are handled above, so no check for the case.
                char second_last = m_value_buffer[buf_size - 2];
                if (second_last != '\n') {
                    break;
                }
                m_value_buffer.pop_back();
                --buf_size;
            }
            break;
        }
        case chomping_indicator_t::KEEP:
            break;
        }
    }

    /// @brief Handle unescaped control characters.
    /// @param c A target character.
    void handle_unescaped_control_char(char c) {
        FK_YAML_ASSERT(0x00 <= c && c <= 0x1F);

        switch (c) {
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
        }
    }

    /// @brief Gets the metadata of a following block style string scalar.
    /// @param chomp_type A variable to store the retrieved chomping style type.
    /// @param indent A variable to store the retrieved indent size.
    void get_block_style_metadata(chomping_indicator_t& chomp_type, uint32_t& indent) {
        chomp_type = chomping_indicator_t::CLIP;
        indent = 0;

        while (m_cur_itr != m_end_itr) {
            switch (*m_cur_itr) {
            case '-':
                if FK_YAML_UNLIKELY (chomp_type != chomping_indicator_t::CLIP) {
                    emit_error("Too many block chomping indicators specified.");
                }
                chomp_type = chomping_indicator_t::STRIP;
                break;
            case '+':
                if FK_YAML_UNLIKELY (chomp_type != chomping_indicator_t::CLIP) {
                    emit_error("Too many block chomping indicators specified.");
                }
                chomp_type = chomping_indicator_t::KEEP;
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
                if FK_YAML_UNLIKELY (indent > 0) {
                    emit_error("Invalid indentation level for a block scalar. It must be between 1 and 9.");
                }
                indent = static_cast<char>(*m_cur_itr - '0');
                break;
            case ' ':
            case '\t':
                break;
            case '\n':
                ++m_cur_itr;
                return;
            case '#':
                skip_until_line_end();
                return;
            default:
                emit_error("Invalid character found in a block scalar header.");
            }

            ++m_cur_itr;
        }
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
    /// An input buffer adapter to be analyzed.
    str_view m_input_buffer {};
    /// The iterator to the current character in the input buffer.
    const char* m_cur_itr {};
    /// The iterator to the beginning of the current token.
    const char* m_token_begin_itr {};
    /// The iterator to the past-the-end element in the input buffer.
    const char* m_end_itr {};
    /// The current position tracker of the input buffer.
    mutable position_tracker m_pos_tracker {};
    /// A temporal buffer to store a string to be parsed to an actual token value.
    std::string m_value_buffer {};
    /// The last yaml version.
    str_view m_yaml_version {};
    /// The last tag handle.
    str_view m_tag_handle {};
    /// The last tag prefix.
    str_view m_tag_prefix {};
    /// The beginning position of the last lexical token. (zero origin)
    uint32_t m_last_token_begin_pos {0};
    /// The beginning line of the last lexical token. (zero origin)
    uint32_t m_last_token_begin_line {0};
    /// The current depth of flow context.
    uint32_t m_state {0};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_LEXICAL_ANALYZER_HPP */
