//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.13
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
#include <fkYAML/detail/input/block_scalar_header.hpp>
#include <fkYAML/detail/input/position_tracker.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/str_view.hpp>
#include <fkYAML/detail/types/lexical_token_t.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Lexical token information
struct lexical_token {
    /// Lexical token type.
    lexical_token_t type {lexical_token_t::END_OF_BUFFER};
    /// Lexical token contents.
    str_view str {};
};

namespace {

// whether the current context is flow(1) or block(0)
const uint32_t flow_context_bit = 1u << 0u;
// whether the current document part is directive(1) or content(0)
const uint32_t document_directive_bit = 1u << 1u;

namespace filter {

constexpr str_view space = " \t";
constexpr str_view space_and_break = " \t\n";
constexpr str_view flow_indicators = "{}[],";

} // namespace filter

} // anonymous namespace

/// @brief A class which lexically analyzes YAML formatted inputs.
class lexical_analyzer {
public:
    /// @brief Construct a new lexical_analyzer object.
    /// @param input_buffer An input buffer.
    explicit lexical_analyzer(str_view input_buffer) noexcept
        : m_input_buffer(input_buffer) {
        m_pos_tracker.set_target_buffer(m_input_buffer);
    }

    /// @brief Get the next lexical token by scanning the left of the input buffer.
    /// @return lexical_token The next lexical token.
    lexical_token get_next_token() {
        skip_white_spaces_and_newline_codes();

        m_pos_tracker.update_position(m_input_buffer.begin());
        m_last_token_begin_pos = m_pos_tracker.get_cur_pos_in_line();
        m_last_token_begin_line = m_pos_tracker.get_lines_read();

        if (m_input_buffer.empty()) {
            return {};
        }

        lexical_token token {};
        token.type = lexical_token_t::PLAIN_SCALAR;

        switch (m_input_buffer.front()) {
        case '?':
            if (m_input_buffer.size() == 1) {
                token.str = m_input_buffer;
                m_input_buffer.remove_prefix(1);
                return token;
            }

            if (m_input_buffer[1] == ' ') {
                m_input_buffer.remove_prefix(2);
                token.type = lexical_token_t::EXPLICIT_KEY_PREFIX;
                return token;
            }
            break;
        case ':': // key separator
            if (m_input_buffer.size() == 1) {
                m_input_buffer.remove_prefix(1);
                token.type = lexical_token_t::KEY_SEPARATOR;
                return token;
            }

            switch (m_input_buffer[1]) {
            case ' ':
            case '\t':
            case '\n':
                m_input_buffer.remove_prefix(2);
                token.type = lexical_token_t::KEY_SEPARATOR;
                return token;
            case ',':
            case '[':
            case ']':
            case '{':
            case '}':
                if (m_state & flow_context_bit) {
                    // Flow indicators are not "safe" to be followed in a flow context.
                    // See https://yaml.org/spec/1.2.2/#733-plain-style for more details.
                    m_input_buffer.remove_prefix(1);
                    token.type = lexical_token_t::KEY_SEPARATOR;
                    return token;
                }
                break;
            default:
                break;
            }
            break;
        case ',': // value separator
            m_input_buffer.remove_prefix(1);
            token.type = lexical_token_t::VALUE_SEPARATOR;
            return token;
        case '&': // anchor prefix
            extract_anchor_name(token.str);

            token.type = lexical_token_t::ANCHOR_PREFIX;
            return token;
        case '*': // alias prefix
            extract_anchor_name(token.str);
            token.type = lexical_token_t::ALIAS_PREFIX;
            return token;
        case '!':
            extract_tag_name(token.str);
            token.type = lexical_token_t::TAG_PREFIX;
            return token;
        case '#': // comment prefix
            scan_comment();
            return get_next_token();
        case '%': // directive prefix
            if (m_state & document_directive_bit) {
                token.type = scan_directive();
                return token;
            }
            // The '%' character can be safely used as the first character in document contents.
            // See https://yaml.org/spec/1.2.2/#912-document-markers for more details.
            break;
        case '-':
            if (m_input_buffer.size() == 1) {
                token.str = m_input_buffer;
                m_input_buffer.remove_prefix(1);
                return token;
            }

            switch (m_input_buffer[1]) {
            case ' ':
            case '\t':
            case '\n':
                // Move a cursor to the beginning of the next token.
                m_input_buffer.remove_prefix(2);
                token.type = lexical_token_t::SEQUENCE_BLOCK_PREFIX;
                return token;
            default:
                break;
            }

            if (m_input_buffer.starts_with("---")) {
                m_input_buffer.remove_prefix(3);
                token.type = lexical_token_t::END_OF_DIRECTIVES;
                return token;
            }

            break;
        case '[': // sequence flow begin
            m_input_buffer.remove_prefix(1);
            token.type = lexical_token_t::SEQUENCE_FLOW_BEGIN;
            return token;
        case ']': // sequence flow end
            m_input_buffer.remove_prefix(1);
            token.type = lexical_token_t::SEQUENCE_FLOW_END;
            return token;
        case '{': // mapping flow begin
            m_input_buffer.remove_prefix(1);
            token.type = lexical_token_t::MAPPING_FLOW_BEGIN;
            return token;
        case '}': // mapping flow end
            m_input_buffer.remove_prefix(1);
            token.type = lexical_token_t::MAPPING_FLOW_END;
            return token;
        case '@':
            emit_error("Any token cannot start with at(@). It is a reserved indicator for YAML.");
        case '`':
            emit_error("Any token cannot start with grave accent(`). It is a reserved indicator for YAML.");
        case '\"':
            m_input_buffer.remove_prefix(1);
            token.type = lexical_token_t::DOUBLE_QUOTED_SCALAR;
            determine_double_quoted_scalar_range(token.str);
            check_scalar_content(token.str);
            return token;
        case '\'':
            m_input_buffer.remove_prefix(1);
            token.type = lexical_token_t::SINGLE_QUOTED_SCALAR;
            determine_single_quoted_scalar_range(token.str);
            check_scalar_content(token.str);
            return token;
        case '.': {
            if (m_input_buffer.starts_with("...")) {
                m_input_buffer.remove_prefix(3);
                token.type = lexical_token_t::END_OF_DOCUMENT;
                return token;
            }
            break;
        }
        case '|':
        case '>': {
            token.type = (m_input_buffer[0] == '|') ? lexical_token_t::BLOCK_LITERAL_SCALAR
                                                    : lexical_token_t::BLOCK_FOLDED_SCALAR;

            std::size_t header_end_pos = m_input_buffer.find('\n');
            FK_YAML_ASSERT(header_end_pos != str_view::npos);
            str_view header_line = m_input_buffer.substr(1, header_end_pos - 1);
            m_block_scalar_header = convert_to_block_scalar_header(header_line);

            m_input_buffer.remove_prefix(header_end_pos + 1);
            scan_block_style_string_token(m_block_scalar_header.indent, token.str);

            return token;
        }
        default:
            break;
        }

        token.type = lexical_token_t::PLAIN_SCALAR;
        determine_plain_scalar_range(token.str);
        check_scalar_content(token.str);
        return token;
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
    /// @brief Skip until a newline code or a null character is found.
    void scan_comment() {
        FK_YAML_ASSERT(m_input_buffer[0] == '#');
        skip_until_line_end();
    }

    /// @brief Scan directives starting with the prefix '%'
    /// @note Currently, only %YAML directive is supported. If not, returns invalid or throws an exception.
    /// @return lexical_token_t The lexical token type for directives.
    lexical_token_t scan_directive() {
        FK_YAML_ASSERT(m_input_buffer[0] == '%');

        std::size_t line_end_pos = m_input_buffer.find('\n');
        if (line_end_pos == str_view::npos) {
            line_end_pos = m_input_buffer.size();
        }

        constexpr str_view tag_directive_prefix = "%TAG";
        constexpr str_view yaml_directive_prefix = "%YAML";

        str_view directive_line = m_input_buffer.substr(0, line_end_pos);
        m_input_buffer.remove_prefix(line_end_pos);

        // remove comment if any
        std::size_t comment_begin_pos = directive_line.find('#');
        if (comment_begin_pos != str_view::npos) {
            directive_line = directive_line.substr(0, comment_begin_pos);
        }

        if (directive_line.starts_with(tag_directive_prefix)) {
            return scan_tag_directive(directive_line);
        }
        else if (directive_line.starts_with(yaml_directive_prefix)) {
            return scan_yaml_version_directive(directive_line);
        }

        return lexical_token_t::INVALID_DIRECTIVE;
    }

    /// @brief Scan a YAML tag directive.
    /// @return lexical_token_t The lexical token type for YAML tag directives.
    lexical_token_t scan_tag_directive(str_view directive) {
        FK_YAML_ASSERT(directive.starts_with("%TAG"));

        std::size_t after_space_pos = directive.find_first_not_of(filter::space, 4);
        if FK_YAML_UNLIKELY (after_space_pos == str_view::npos) {
            emit_error("TAG directive has no handle and prefix.");
        }
        if FK_YAML_UNLIKELY (after_space_pos == 4) {
            return lexical_token_t::INVALID_DIRECTIVE;
        }

        directive.remove_prefix(after_space_pos);
        if FK_YAML_UNLIKELY (directive.size() < 3) {
            // string after "%TAG " must be larger than 2 character length.
            //   %TAG ! e
            //        ^^^
            emit_error("invalid TAG directive is found.");
        }

        //
        // extract a tag handle
        //

        if FK_YAML_UNLIKELY (directive[0] != '!') {
            emit_error("Tag handle must start with \'!\'.");
        }

        if (filter::space.contains(directive[1])) {
            // primary handle (!)
            m_tag_handle = directive.substr(0, 1);
        }
        else if (directive[1] == '!') {
            // secondary handle (!!)
            if FK_YAML_UNLIKELY (!filter::space.contains(directive[2])) {
                emit_error("invalid tag handle is found.");
            }
            m_tag_handle = directive.substr(0, 2);
        }
        else {
            // named handle (!handle!)
            std::size_t handle_end_pos = directive.find('!', 2);
            if FK_YAML_UNLIKELY (handle_end_pos == str_view::npos || handle_end_pos == directive.size() - 1) {
                emit_error("invalid tag handle is found.");
            }

            str_view handle = directive.substr(1, handle_end_pos - 1);

            std::size_t space_pos = handle.find_first_of(filter::space);
            if FK_YAML_UNLIKELY (space_pos != str_view::npos) {
                emit_error("invalid tag handle is found.");
            }

            for (const auto c : handle) {
                if FK_YAML_UNLIKELY (!isalnum(c) && (c != '-')) {
                    // See https://yaml.org/spec/1.2.2/#rule-c-named-tag-handle for more details.
                    emit_error("named handle can contain only numbers(0-9), alphabets(A-Z,a-z) and hyphens(-).");
                }
            }

            m_tag_handle = directive.substr(0, handle_end_pos + 1);
        }

        directive.remove_prefix(m_tag_handle.size());
        after_space_pos = directive.find_first_not_of(filter::space);
        if (after_space_pos == str_view::npos) {
            emit_error("Tag directive lacks tag prefix.");
        }
        else if (after_space_pos == 0) {
            emit_error("There must be at least one space character between tag handle and prefix.");
        }

        directive.remove_prefix(after_space_pos);

        //
        // extract a tag prefix.
        //

        if FK_YAML_UNLIKELY (filter::flow_indicators.contains(directive[0])) {
            // Tag prefix must not start with flow indicators to avoid ambiguity.
            // See https://yaml.org/spec/1.2.2/#rule-ns-global-tag-prefix for more details.
            emit_error("tag prefix must not start with flow indicators (\',\', [], {}).");
        }

        std::size_t prefix_end_pos = directive.find_first_of(filter::space_and_break);
        if (prefix_end_pos == str_view::npos) {
            prefix_end_pos = directive.size();
        }

        m_tag_prefix = directive.substr(0, prefix_end_pos);

        bool is_valid = uri_encoding::validate(m_tag_prefix.begin(), m_tag_prefix.end());
        if FK_YAML_UNLIKELY (!is_valid) {
            emit_error("invalid URI character is found in a tag prefix.");
        }

        return lexical_token_t::TAG_DIRECTIVE;
    }

    /// @brief Scan a YAML version directive.
    /// @note Only 1.1 and 1.2 are supported. If not, throws an exception.
    /// @return lexical_token_t The lexical token type for YAML version directives.
    lexical_token_t scan_yaml_version_directive(str_view directive) {
        FK_YAML_ASSERT(directive.starts_with("%YAML"));

        std::size_t after_space_pos = directive.find_first_not_of(filter::space, 5);
        if FK_YAML_UNLIKELY (after_space_pos == str_view::npos) {
            emit_error("YAML directive has no version.");
        }
        if FK_YAML_UNLIKELY (after_space_pos == 5) {
            return lexical_token_t::INVALID_DIRECTIVE;
        }

        directive.remove_prefix(after_space_pos);
        if FK_YAML_UNLIKELY (directive.size() < 3) {
            // string after "%TAG " must be larger than 3 character length, i.e., "<major>.<minor>".
            //   %YAML 1.2
            //         ^^^
            emit_error("invalid YAML directive is found.");
        }

        std::size_t version_end_pos = directive.find_first_of(filter::space_and_break);
        if (version_end_pos == str_view::npos) {
            version_end_pos = directive.size();
        }

        m_yaml_version = directive.substr(0, version_end_pos);

        if FK_YAML_UNLIKELY (m_yaml_version.compare("1.1") != 0 && m_yaml_version.compare("1.2") != 0) {
            emit_error("Only 1.1 and 1.2 can be specified as the YAML version.");
        }

        return lexical_token_t::YAML_VER_DIRECTIVE;
    }

    /// @brief Extracts an anchor name from the input.
    /// @param anchor_name Storage for the extracted anchor name.
    void extract_anchor_name(str_view& anchor_name) {
        FK_YAML_ASSERT(m_input_buffer[0] == '&' || m_input_buffer[0] == '*');

        m_input_buffer.remove_prefix(1);
        constexpr str_view anchor_term_chars = " \t\n{}[],";
        std::size_t anchor_end_pos = m_input_buffer.find_first_of(anchor_term_chars);
        if (anchor_end_pos != str_view::npos) {
            anchor_name = m_input_buffer.substr(0, anchor_end_pos);
            m_input_buffer.remove_prefix(anchor_end_pos);
        }
        else {
            anchor_name = m_input_buffer;
            m_input_buffer.remove_prefix(m_input_buffer.size());
        }

        if FK_YAML_UNLIKELY (anchor_name.empty()) {
            emit_error("anchor name must not be empty.");
        }
    }

    /// @brief Extracts a tag name from the input.
    /// @param tag_name Storage for the extracted tag name.
    void extract_tag_name(str_view& tag_name) {
        FK_YAML_ASSERT(m_input_buffer[0] == '!');

        if (m_input_buffer.size() == 1) {
            // Just "!" is a non-specific tag.
            tag_name = m_input_buffer;
            m_input_buffer.remove_prefix(1);
            return;
        }

        constexpr str_view secondary_tag_handle = "!!";
        constexpr str_view verbatim_tag_handle = "!<";

        std::size_t suffix_begin_pos = 0;
        if (filter::space_and_break.contains(m_input_buffer[1])) {
            // Non-specific tag (!)
            tag_name = m_input_buffer.substr(0, 1);
            m_input_buffer.remove_prefix(2);
            return;
        }
        else if (m_input_buffer.starts_with(secondary_tag_handle)) {
            // Secondary tag handles (!!suffix)
            std::size_t suffix_end_pos = m_input_buffer.find_first_of(filter::space_and_break, 2);
            if FK_YAML_LIKELY (suffix_end_pos != str_view::npos) {
                tag_name = m_input_buffer.substr(0, suffix_end_pos);
                m_input_buffer.remove_prefix(suffix_end_pos + 1);
            }
            else {
                tag_name = m_input_buffer;
                m_input_buffer.remove_prefix(m_input_buffer.size());
            }
            suffix_begin_pos = 2;
        }
        else if (m_input_buffer.starts_with(verbatim_tag_handle)) {
            // Verbatim tags (!<TAG>)
            std::size_t verbatim_end_pos = m_input_buffer.find('>', 2);
            if FK_YAML_UNLIKELY (verbatim_end_pos == str_view::npos) {
                emit_error("verbatim tag must be ended with '>'.");
            }

            tag_name = m_input_buffer.substr(0, verbatim_end_pos + 1);

            // only the `TAG` part of the `!<TAG>` for URI validation.
            str_view tag_body = tag_name.substr(2, tag_name.size() - 3);
            if FK_YAML_UNLIKELY (tag_body.empty()) {
                emit_error("verbatim tag(!<TAG>) must not be empty.");
            }

            bool is_valid_uri = uri_encoding::validate(tag_body.begin(), tag_body.end());
            if FK_YAML_UNLIKELY (!is_valid_uri) {
                emit_error("invalid URI character is found in a verbatim tag.");
            }

            m_input_buffer.remove_prefix(verbatim_end_pos + 1);

            return;
        }
        else {
            // Either local tag (!suffix) or named handle (!handle!suffix)
            std::size_t tag_end_pos = m_input_buffer.find_first_of(filter::space_and_break, 1);
            if FK_YAML_LIKELY (tag_end_pos != str_view::npos) {
                tag_name = m_input_buffer.substr(0, tag_end_pos);
                m_input_buffer.remove_prefix(tag_end_pos);
            }
            else {
                tag_name = m_input_buffer;
                m_input_buffer.remove_prefix(m_input_buffer.size());
            }

            std::size_t named_handle_end_pos = tag_name.find('!', 1);
            if (named_handle_end_pos != str_view::npos) {
                // named handle (!handle!suffix)
                if FK_YAML_UNLIKELY (named_handle_end_pos == tag_name.size() - 1) {
                    emit_error("named handle's suffix must not be empty.");
                }

                std::size_t another_prefix_pos = tag_name.find('!', named_handle_end_pos + 1);
                if FK_YAML_UNLIKELY (another_prefix_pos != str_view::npos) {
                    emit_error("named handle's suffix must not contain tag prefix character(!).");
                }
                suffix_begin_pos = named_handle_end_pos + 1;
            }
            else {
                suffix_begin_pos = 1;
            }
        }

        // get the position of last tag prefix character (!) to extract suffix.
        // tag shorthand is either primary(!tag), secondary(!!tag) or named(!handle!tag).
        str_view suffix = tag_name.substr(suffix_begin_pos);

        bool is_valid_uri = uri_encoding::validate(suffix.begin(), suffix.end());
        if FK_YAML_UNLIKELY (!is_valid_uri) {
            emit_error("invalid URI character is found in a tag suffix.");
        }

        // Tag shorthand's suffix must not contain tag prefix(!) and flow indicators({}[],).
        // See the "Tag Shorthands" section in https://yaml.org/spec/1.2.2/#691-node-tags.
        constexpr str_view invalid_suffix_chars = "!{}[],";
        std::size_t invalid_char_pos = suffix.find_first_of(invalid_suffix_chars);
        if FK_YAML_UNLIKELY (invalid_char_pos != str_view::npos) {
            emit_error("tag shorthand's suffix must not contain flow indicators({}[],).");
        }
    }

    /// @brief Determines the range of single quoted scalar by scanning remaining input buffer contents.
    /// @param token Storage for the range of single quoted scalar.
    void determine_single_quoted_scalar_range(str_view& token) {
        std::size_t size = m_input_buffer.size();

        std::size_t pos = m_input_buffer.find('\'');
        while (pos != str_view::npos) {
            FK_YAML_ASSERT(pos < size);
            if FK_YAML_LIKELY (pos == size - 1 || m_input_buffer[pos + 1] != '\'') {
                // closing single quote is found.
                token = m_input_buffer.substr(0, pos);
                m_input_buffer.remove_prefix(pos + 1);
                return;
            }

            // If single quotation marks are repeated twice in a single quoted scalar, they are considered as an
            // escaped single quotation mark. Skip the second one which would otherwise be detected as a closing
            // single quotation mark in the next loop.
            pos = m_input_buffer.find('\'', pos + 2);
        }

        emit_error("Invalid end of input buffer in a single-quoted scalar token.");
    }

    /// @brief Determines the range of double quoted scalar by scanning remaining input buffer contents.
    /// @param token Storage for the range of double quoted scalar.
    void determine_double_quoted_scalar_range(str_view& token) {
        const char* p_begin = m_input_buffer.begin();

        std::size_t pos = m_input_buffer.find('\"');
        while (pos != str_view::npos) {
            FK_YAML_ASSERT(pos < m_input_buffer.size());

            bool is_closed = true;
            if FK_YAML_LIKELY (pos > 0) {
                // Double quotation marks can be escaped by a preceding backslash and the number of backslashes matters
                // to determine if the found double quotation mark is escaped since the backslash itself can also be
                // escaped:
                // * odd number of backslashes  -> double quotation mark IS escaped (e.g., "\\\"")
                // * even number of backslashes -> double quotation mark IS NOT escaped (e.g., "\\"")
                uint32_t backslash_counts = 0;
                const char* p_cur = p_begin + (pos - 1);
                do {
                    if (*p_cur != '\\') {
                        break;
                    }
                    ++backslash_counts;
                } while (--p_cur != p_begin);
                is_closed = !(backslash_counts & 1u); // true: even, false: odd
            }

            if (is_closed) {
                // closing double quote is found.
                token = m_input_buffer.substr(0, pos);
                m_input_buffer.remove_prefix(pos + 1);
                return;
            }

            pos = m_input_buffer.find('\"', pos + 1);
        }

        emit_error("Invalid end of input buffer in a double-quoted scalar token.");
    }

    /// @brief Determines the range of plain scalar by scanning remaining input buffer contents.
    /// @param token Storage for the range of plain scalar.
    void determine_plain_scalar_range(str_view& token) {
        const std::size_t size = m_input_buffer.size();

        constexpr str_view scalar_term_chars_flow = "\n :{}[],";
        constexpr str_view scalar_term_chars_block = "\n :";
        str_view scalar_term_chars = (m_state & flow_context_bit) ? scalar_term_chars_flow : scalar_term_chars_block;

        std::size_t pos = m_input_buffer.find_first_of(scalar_term_chars);
        if FK_YAML_UNLIKELY (pos == str_view::npos) {
            token = m_input_buffer;
            m_input_buffer.remove_prefix(size);
            return;
        }

        bool ends_loop = false;
        do {
            FK_YAML_ASSERT(pos < size);
            switch (m_input_buffer[pos]) {
            case '\n':
                ends_loop = true;
                break;
            case ' ':
                if FK_YAML_UNLIKELY (pos == size - 1) {
                    // trim trailing space.
                    ends_loop = true;
                    break;
                }

                // Allow a space in a plain scalar only if the space is surrounded by non-space characters, but not
                // followed by the comment prefix " #".
                // Also, flow indicators are not allowed to be followed after a space in a flow context.
                // See https://yaml.org/spec/1.2.2/#733-plain-style for more details.
                switch (m_input_buffer[pos + 1]) {
                case ' ':
                case '\t':
                case '\n':
                case '#':
                    ends_loop = true;
                    break;
                case ':':
                    // " :" is permitted in a plain style string token, but not when followed by a space.
                    ends_loop = (pos + 2 < size) && (m_input_buffer[pos + 2] == ' ');
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
                if FK_YAML_LIKELY (pos + 1 < size) {
                    switch (m_input_buffer[pos + 1]) {
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

            pos = m_input_buffer.find_first_of(scalar_term_chars, pos + 1);
        } while (pos != str_view::npos);

        if (pos == str_view::npos) {
            pos = size;
        }

        token = m_input_buffer.substr(0, pos);
        m_input_buffer.remove_prefix(pos);
    }

    /// @brief Scan a block style string token either in the literal or folded style.
    /// @param style The style of the given token, either literal or folded.
    /// @param chomp The chomping indicator type of the given token, either strip, keep or clip.
    /// @param indent The indent size specified for the given token.
    void scan_block_style_string_token(uint32_t& indent, str_view& token) {
        const std::size_t size = m_input_buffer.size();

        // Handle leading all-space lines.
        std::size_t first_non_space_pos = m_input_buffer.find_first_not_of(filter::space_and_break);
        if (first_non_space_pos == str_view::npos) {
            // empty block scalar with no subsequent tokens.
            indent = static_cast<uint32_t>(size);
            token = m_input_buffer;

            // Without the following iterator update, lexer cannot reach the end of input buffer and causes infinite
            // loops from the next loop. (https://github.com/fktn-k/fkYAML/pull/410)
            m_input_buffer.remove_prefix(size);
            return;
        }

        // get indentation of the first non-space character.
        std::size_t last_newline_pos = m_input_buffer.substr(0, first_non_space_pos).find_last_of('\n');
        if (last_newline_pos == str_view::npos) {
            // first_non_space_pos in on the first line.
            uint32_t cur_indent = static_cast<uint32_t>(first_non_space_pos);
            if (indent == 0) {
                indent = cur_indent;
            }
            else if FK_YAML_UNLIKELY (cur_indent < indent) {
                emit_error("A block style scalar is less indented than the indicated level.");
            }

            last_newline_pos = 0;
        }
        else {
            FK_YAML_ASSERT(last_newline_pos < first_non_space_pos);
            uint32_t cur_indent = static_cast<uint32_t>(first_non_space_pos - last_newline_pos - 1);

            // TODO: preserve and compare the last indentation with `cur_indent`
            if (indent == 0) {
                indent = cur_indent;
            }
            else if FK_YAML_UNLIKELY (cur_indent < indent) {
                emit_error("A block style scalar is less indented than the indicated level.");
            }
        }

        last_newline_pos = m_input_buffer.find('\n', first_non_space_pos + 1);
        if (last_newline_pos == str_view::npos) {
            last_newline_pos = size;
        }

        while (last_newline_pos < size) {
            std::size_t cur_line_end_pos = m_input_buffer.find('\n', last_newline_pos + 1);
            if (cur_line_end_pos == str_view::npos) {
                cur_line_end_pos = size;
            }

            std::size_t cur_line_content_begin_pos = m_input_buffer.find_first_not_of(' ', last_newline_pos + 1);
            if (cur_line_content_begin_pos == str_view::npos) {
                last_newline_pos = cur_line_end_pos;
                continue;
            }

            FK_YAML_ASSERT(last_newline_pos < cur_line_content_begin_pos);
            uint32_t cur_indent = static_cast<uint32_t>(cur_line_content_begin_pos - last_newline_pos - 1);
            if (cur_indent < indent && m_input_buffer[cur_line_content_begin_pos] != '\n') {
                // Interpret less indented non-space characters as the start of the next token.
                break;
            }

            last_newline_pos = cur_line_end_pos;
        }

        // include last newline character if not all characters have been consumed yet.
        if (last_newline_pos < size) {
            ++last_newline_pos;
        }

        token = m_input_buffer.substr(0, last_newline_pos);
        m_input_buffer.remove_prefix(last_newline_pos);
    }

    /// @brief Checks if the given scalar contains no unescaped control characters.
    /// @param scalar Scalar contents.
    void check_scalar_content(str_view scalar) const {
        for (auto c : scalar) {
            switch (c) {
            case 0x00:
                emit_error("control character U+0000 (NUL) must be escaped to \\u0000.");
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
            // 0x09(HT, \t) is safe to use without escaping.
            // 0x0A(LF, \n) is safe to use without escaping.
            case 0x0B:
                emit_error("Control character U+000B (VT) must be escaped to \\v or \\u000B.");
            case 0x0C:
                emit_error("Control character U+000C (FF) must be escaped to \\f or \\u000C.");
            // 0x0D(CR, \r) is safe to use without escaping.
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
    }

    /// @brief Gets the metadata of a following block style string scalar.
    /// @param chomp_type A variable to store the retrieved chomping style type.
    /// @param indent A variable to store the retrieved indent size.
    /// @return Block scalar header information converted from the header line.
    block_scalar_header convert_to_block_scalar_header(str_view& line) {
        constexpr str_view comment_prefix = " #";
        std::size_t comment_begin_pos = line.find(comment_prefix);
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
                header.indent = static_cast<char>(c - '0');
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
        std::size_t pos = m_input_buffer.find_first_not_of(filter::space);
        if (pos == str_view::npos) {
            pos = m_input_buffer.size();
        }
        m_input_buffer.remove_prefix(pos);
    }

    /// @brief Skip white spaces and newline codes (CR/LF) from the current position.
    void skip_white_spaces_and_newline_codes() {
        std::size_t pos = m_input_buffer.find_first_not_of(filter::space_and_break);
        if (pos == str_view::npos) {
            pos = m_input_buffer.size();
        }
        m_input_buffer.remove_prefix(pos);
    }

    /// @brief Skip the rest in the current line.
    void skip_until_line_end() {
        std::size_t pos = m_input_buffer.find('\n');
        if (pos != str_view::npos) {
            m_input_buffer.remove_prefix(pos + 1);
        }
        else {
            m_input_buffer.remove_prefix(m_input_buffer.size());
        }
    }

    /// @brief Emits an error with the given message.
    /// @param msg A message for the resulting error.
    [[noreturn]] void emit_error(const char* msg) const {
        m_pos_tracker.update_position(m_input_buffer.begin());
        throw fkyaml::parse_error(msg, m_pos_tracker.get_lines_read(), m_pos_tracker.get_cur_pos_in_line());
    }

private:
    /// An input buffer adapter to be analyzed.
    str_view m_input_buffer {};
    /// The current position tracker of the input buffer.
    mutable position_tracker m_pos_tracker {};
    /// The last yaml version.
    str_view m_yaml_version {};
    /// The last tag handle.
    str_view m_tag_handle {};
    /// The last tag prefix.
    str_view m_tag_prefix {};
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
