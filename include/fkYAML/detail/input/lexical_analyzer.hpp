//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.12
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_INPUT_LEXICAL_ANALIZER_HPP_
#define FK_YAML_DETAIL_INPUT_LEXICAL_ANALIZER_HPP_

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
// whether the curent document part is directive(1) or content(0)
const uint32_t document_directive_bit = 1u << 1u;

} // anonymous namespace

/// @brief A class which lexically analizes YAML formatted inputs.
class lexical_analyzer {
private:
    enum class block_style_indicator_t {
        LITERAL, //!< keeps newlines inside the block as they are indicated by a pipe `|`.
        FOLDED,  //!< replaces newlines inside the block with spaces indicated by a right angle bracket `>`.
    };

    enum class chomping_indicator_t {
        STRIP, //!< excludes final line breaks and traiing empty lines indicated by `-`.
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
        case ':': { // key separater
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
        case ',': // value separater
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

            // get the position of the beginning of a suffix. (!handle!suffix)
            std::size_t last_tag_prefix_pos = token.str.find_last_of('!');
            FK_YAML_ASSERT(last_tag_prefix_pos != str_view::npos);

            str_view tag_uri = token.str.substr(last_tag_prefix_pos + 1);
            bool is_valid_uri = uri_encoding::validate(tag_uri.begin(), tag_uri.end());
            if FK_YAML_UNLIKELY (!is_valid_uri) {
                emit_error("Invalid URI character is found in a named tag handle.");
            }
        }
    }

    /// @brief Scan a scalar token, either plain, single-quoted or double-quoted.
    /// @param token The token into which the scan result is written.
    /// @return lexical_token_t The lexical token type for strings.
    void scan_scalar(lexical_token& token) {
        m_value_buffer.clear();

        bool needs_last_single_quote = false;
        bool needs_last_double_quote = false;
        if (m_cur_itr == m_token_begin_itr) {
            needs_last_single_quote = (*m_cur_itr == '\'');
            needs_last_double_quote = (*m_cur_itr == '\"');
            if (needs_last_double_quote || needs_last_single_quote) {
                m_token_begin_itr = ++m_cur_itr;
                token.type = needs_last_double_quote ? lexical_token_t::DOUBLE_QUOTED_SCALAR
                                                     : lexical_token_t::SINGLE_QUOTED_SCALAR;
            }
            else {
                token.type = lexical_token_t::PLAIN_SCALAR;
            }
        }

        bool is_value_buff_used = extract_string_token(needs_last_single_quote, needs_last_double_quote);

        if (is_value_buff_used) {
            token.str = str_view {m_value_buffer.begin(), m_value_buffer.end()};
        }
        else {
            token.str = str_view {m_token_begin_itr, m_cur_itr};
            if (token.type != lexical_token_t::PLAIN_SCALAR) {
                // If extract_string_token() didn't use m_value_buffer to store mutated scalar value, m_cur_itr is at
                // the last quotation mark, which will cause infinite loops from the next get_next_token() call.
                ++m_cur_itr;
            }
        }
    }

    /// @brief Check if the given character is allowed in a single-quoted scalar token.
    /// @param c The character to be checked.
    /// @param is_value_buffer_used true is assigned when mutated scalar contents is written into m_value_buffer.
    /// @return true if the given character is allowed, false otherwise.
    bool is_allowed_single(char c, bool& is_value_buffer_used) {
        switch (c) {
        case '\n': {
            is_value_buffer_used = true;

            // discard trailing white spaces which preceeds the line break in the current line.
            auto before_trailing_spaces_itr = m_cur_itr - 1;
            bool ends_loop = false;
            while (before_trailing_spaces_itr != m_token_begin_itr) {
                switch (*before_trailing_spaces_itr) {
                case ' ':
                case '\t':
                    --before_trailing_spaces_itr;
                    break;
                default:
                    ends_loop = true;
                    break;
                }

                if (ends_loop) {
                    break;
                }
            }
            m_value_buffer.append(m_token_begin_itr, before_trailing_spaces_itr + 1);

            // move to the beginning of the next line.
            ++m_cur_itr;

            // apply line folding according to the number of following empty lines.
            m_pos_tracker.update_position(m_cur_itr);
            uint32_t line_after_line_break = m_pos_tracker.get_lines_read();
            skip_white_spaces_and_newline_codes();
            m_pos_tracker.update_position(m_cur_itr);
            uint32_t trailing_empty_lines = m_pos_tracker.get_lines_read() - line_after_line_break;
            if (trailing_empty_lines > 0) {
                m_value_buffer.append(trailing_empty_lines, '\n');
            }
            else {
                m_value_buffer.push_back(' ');
            }

            m_token_begin_itr = (m_cur_itr == m_end_itr || *m_cur_itr == '\'') ? m_cur_itr-- : m_cur_itr;
            return true;
        }

        case '\'':
            if (m_cur_itr + 1 == m_end_itr) {
                if (is_value_buffer_used) {
                    m_value_buffer.append(m_token_begin_itr, m_cur_itr++);
                    m_token_begin_itr = m_cur_itr;
                }
                return false;
            }

            if (*(m_cur_itr + 1) != '\'') {
                if (is_value_buffer_used) {
                    m_value_buffer.append(m_token_begin_itr, m_cur_itr++);
                }
                return false;
            }

            // If single quotation marks are repeated twice in a single-quoted string token,
            // they are considered as an escaped single quotation mark.
            is_value_buffer_used = true;

            m_value_buffer.append(m_token_begin_itr, ++m_cur_itr);
            m_token_begin_itr = m_cur_itr + 1;
            return true;

        default:         // LCOV_EXCL_LINE
            return true; // LCOV_EXCL_LINE
        }
    }

    /// @brief Check if the given character is allowed in a double-quoted scalar token.
    /// @param c The character to be checked.
    /// @param is_value_buffer_used true is assigned when mutated scalar contents is written into m_value_buffer.
    /// @return true if the given character is allowed, false otherwise.
    bool is_allowed_double(char c, bool& is_value_buffer_used) {
        switch (c) {
        case '\n': {
            is_value_buffer_used = true;

            // discard trailing white spaces which preceeds the line break in the current line.
            auto before_trailing_spaces_itr = m_cur_itr - 1;
            bool ends_loop = false;
            while (before_trailing_spaces_itr != m_token_begin_itr) {
                switch (*before_trailing_spaces_itr) {
                case ' ':
                case '\t':
                    --before_trailing_spaces_itr;
                    break;
                default:
                    ends_loop = true;
                    break;
                }

                if (ends_loop) {
                    break;
                }
            }
            m_value_buffer.append(m_token_begin_itr, before_trailing_spaces_itr + 1);

            // move to the beginning of the next line.
            ++m_cur_itr;

            // apply line folding according to the number of following empty lines.
            m_pos_tracker.update_position(m_cur_itr);
            uint32_t line_after_line_break = m_pos_tracker.get_lines_read();
            skip_white_spaces_and_newline_codes();
            m_pos_tracker.update_position(m_cur_itr);
            uint32_t trailing_empty_lines = m_pos_tracker.get_lines_read() - line_after_line_break;
            if (trailing_empty_lines > 0) {
                m_value_buffer.append(trailing_empty_lines, '\n');
            }
            else {
                m_value_buffer.push_back(' ');
            }

            m_token_begin_itr = (m_cur_itr == m_end_itr || *m_cur_itr == '\"') ? m_cur_itr-- : m_cur_itr;
            return true;
        }

        case '\"':
            if (is_value_buffer_used) {
                m_value_buffer.append(m_token_begin_itr, m_cur_itr++);
            }
            return false;

        case '\\':
            is_value_buffer_used = true;

            m_value_buffer.append(m_token_begin_itr, m_cur_itr);

            // Handle escaped characters.
            // See https://yaml.org/spec/1.2.2/#57-escaped-characters for more details.

            c = *(m_cur_itr + 1);
            if (c != '\n') {
                bool is_valid_escaping = yaml_escaper::unescape(m_cur_itr, m_end_itr, m_value_buffer);
                if FK_YAML_UNLIKELY (!is_valid_escaping) {
                    emit_error("Unsupported escape sequence is found in a string token.");
                }

                m_token_begin_itr = m_cur_itr + 1;
                return true;
            }

            // move until the next non-space character is found.
            m_cur_itr += 2;
            skip_white_spaces();

            m_token_begin_itr = (m_cur_itr == m_end_itr || *m_cur_itr == '\"') ? m_cur_itr-- : m_cur_itr;
            return true;

        default:         // LCOV_EXCL_LINE
            return true; // LCOV_EXCL_LINE
        }
    }

    /// @brief Check if the given character is allowed in a plain scalar token outside a flow context.
    /// @param c The character to be checked.
    /// @return true if the given character is allowed, false otherwise.
    bool is_allowed_plain(char c, bool& /*unused*/) {
        switch (c) {
        case '\n':
            return false;

        case ' ': {
            // Allow a space in a plain scalar only if the space is surrounded by non-space characters.
            // See https://yaml.org/spec/1.2.2/#733-plain-style for more details.

            switch (*(m_cur_itr + 1)) {
            case ':': {
                // " :" is permitted in a plain style string token, but not when followed by a space.
                char peeked = *(m_cur_itr + 2);
                if (peeked == ' ') {
                    return false;
                }
                return true;
            }
            case ' ':
            case '\n':
            case '#':
            case '\\':
                return false;
            }

            return true;
        }

        case ':': {
            // A colon as a key separator must be followed by
            // * a white space or
            // * a newline code.
            switch (*(m_cur_itr + 1)) {
            case ' ':
            case '\t':
            case '\n':
                return false;
            }
            return true;
        }

        default:         // LCOV_EXCL_LINE
            return true; // LCOV_EXCL_LINE
        }
    }

    /// @brief Check if the given character is allowed in a plain scalar token inside a flow context.
    /// @param c The character to be checked.
    /// @return true if the given character is allowed, false otherwise.
    bool is_allowed_plain_flow(char c, bool& /*unused*/) {
        switch (c) {
        case '\n':
            return false;

        case ' ': {
            // Allow a space in an unquoted string only if the space is surrounded by non-space characters.
            // See https://yaml.org/spec/1.2.2/#733-plain-style for more details.
            char next = *(m_cur_itr + 1);

            // These characters are permitted when not inside a flow collection, and not inside an implicit key.
            // TODO: Support detection of implicit key context for this check.
            switch (next) {
            case '{':
            case '}':
            case '[':
            case ']':
            case ',':
                return false;
            }

            // " :" is permitted in a plain style string token, but not when followed by a space.
            if (next == ':') {
                char peeked = *(m_cur_itr + 2);
                if (peeked == ' ') {
                    return false;
                }
            }

            switch (next) {
            case ' ':
            case '\n':
            case '#':
            case '\\':
                return false;
            }

            return true;
        }

        case ':': {
            char next = *(m_cur_itr + 1);

            // A colon as a key separator must be followed by
            // * a white space or
            // * a newline code.
            switch (next) {
            case ' ':
            case '\t':
            case '\n':
                return false;
            }
            return true;
        }

        case '{':
        case '}':
        case '[':
        case ']':
        case ',':
            return false;

        default:         // LCOV_EXCL_LINE
            return true; // LCOV_EXCL_LINE
        }
    }

    /// @brief Extracts a string token, either plain, single-quoted or double-quoted, from the input buffer.
    /// @return true if mutated scalar contents is stored in m_value_buffer, false otherwise.
    bool extract_string_token(bool needs_last_single_quote, bool needs_last_double_quote) {
        // change behaviors depending on the type of a comming string scalar token.
        // * single quoted
        // * double quoted
        // * plain

        std::string check_filters {"\n"};
        bool (lexical_analyzer::*pfn_is_allowed)(char, bool&) = nullptr;

        if (needs_last_single_quote) {
            check_filters.append("\'");
            pfn_is_allowed = &lexical_analyzer::is_allowed_single;
        }
        else if (needs_last_double_quote) {
            check_filters.append("\"\\");
            pfn_is_allowed = &lexical_analyzer::is_allowed_double;
        }
        else if (m_state & flow_context_bit) {
            // plain scalar inside flow contexts
            check_filters.append(" :{}[],");
            pfn_is_allowed = &lexical_analyzer::is_allowed_plain_flow;
        }
        else {
            // plain scalar outside flow contexts
            check_filters.append(" :");
            pfn_is_allowed = &lexical_analyzer::is_allowed_plain;
        }

        // scan the contents of a string scalar token.

        bool is_value_buffer_used = false;
        for (; m_cur_itr != m_end_itr; ++m_cur_itr) {
            char current = *m_cur_itr;
            uint32_t num_bytes = utf8::get_num_bytes(static_cast<uint8_t>(current));
            if FK_YAML_LIKELY (num_bytes == 1) {
                auto ret = check_filters.find(current);
                if (ret != std::string::npos) {
                    bool is_allowed = (this->*pfn_is_allowed)(current, is_value_buffer_used);
                    if (!is_allowed) {
                        return is_value_buffer_used;
                    }

                    continue;
                }

                // Handle unescaped control characters.
                if FK_YAML_UNLIKELY (static_cast<uint8_t>(current) <= 0x1F) {
                    handle_unescaped_control_char(current);
                    continue;
                }

                continue;
            }

            // Multi-byte characters are already validated while creating an input handler.
            // So just advance the iterator.
            m_cur_itr += num_bytes - 1;
        }

        // Handle the end of input buffer.

        if FK_YAML_UNLIKELY (needs_last_double_quote) {
            emit_error("Invalid end of input buffer in a double-quoted string token.");
        }

        if FK_YAML_UNLIKELY (needs_last_single_quote) {
            emit_error("Invalid end of input buffer in a single-quoted string token.");
        }

        return is_value_buffer_used;
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

#endif /* FK_YAML_DETAIL_INPUT_LEXICAL_ANALIZER_HPP_ */
