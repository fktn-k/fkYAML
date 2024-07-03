///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.9
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_INPUT_LEXICAL_ANALIZER_HPP_
#define FK_YAML_DETAIL_INPUT_LEXICAL_ANALIZER_HPP_

#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <type_traits>
#include <vector>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/assert.hpp>
#include <fkYAML/detail/encodings/utf_encodings.hpp>
#include <fkYAML/detail/encodings/yaml_escaper.hpp>
#include <fkYAML/detail/input/position_tracker.hpp>
#include <fkYAML/detail/meta/input_adapter_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/types/lexical_token_t.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief A class which lexically analizes YAML formatted inputs.
class lexical_analyzer {
private:
    using iterator_type = typename std::string::const_iterator;

public:
    enum class block_style_indicator_t {
        LITERAL, //!< keeps newlines inside the block as they are indicated by a pipe `|`.
        FOLDED,  //!< replaces newlines inside the block with spaces indicated by a right angle bracket `>`.
    };

    enum class chomping_indicator_t {
        STRIP, //!< excludes final line breaks and traiing empty lines indicated by `-`.
        KEEP,  //!< preserves final line breaks but excludes trailing empty lines. no indicator means this type.
        CLIP,  //!< preserves final line breaks and trailing empty lines indicated by `+`.
    };

    struct block_scalar_header {
        block_style_indicator_t style_type {block_style_indicator_t::LITERAL};
        chomping_indicator_t chomping_type {chomping_indicator_t::KEEP};
        uint32_t indent {0};
    };

    struct lexical_token {
        lexical_token() = default;
        ~lexical_token() {
            if (type == lexical_token_t::BLOCK_SCALAR) {
                delete reinterpret_cast<block_scalar_header*>(p_extra_info);
                p_extra_info = nullptr;
            }
        }

        lexical_token_t type {lexical_token_t::END_OF_BUFFER};
        uint32_t begin_line {0};
        uint32_t begin_pos {0};
        iterator_type begin_itr {};
        iterator_type end_itr {};
        void* p_extra_info {nullptr};
    };

    /// @brief Construct a new lexical_analyzer object.
    /// @param input_buffer An input buffer which must be alive during this lexical_analyzer's lifetime.
    explicit lexical_analyzer(const std::string& input_buffer) {
        m_cur_itr = input_buffer.cbegin();
        m_end_itr = input_buffer.cend();
        m_pos_tracker.set_target_buffer(input_buffer);
    }

    std::vector<lexical_token> get_tokens() {
        std::vector<lexical_token> tokens {};

        bool reaches_end_of_buffer = false;
        do {
            lexical_token token = get_next_token();
            reaches_end_of_buffer = token.type == lexical_token_t::END_OF_BUFFER;
            tokens.emplace_back(std::move(token));
        } while (!reaches_end_of_buffer);

        return tokens;
    }

    /// @brief Get the next lexical token type by scanning the left of the input buffer.
    /// @return lexical_token_t The next lexical token type.
    lexical_token get_next_token() {
        skip_white_spaces_and_newline_codes();
        m_pos_tracker.update_position(m_cur_itr);

        lexical_token token {};
        token.begin_line = m_pos_tracker.get_lines_read();
        token.begin_pos = m_pos_tracker.get_cur_pos_in_line();
        token.begin_itr = m_cur_itr;

        if (m_cur_itr == m_end_itr) {
            // token.type is already initialized with lexical_token_t::END_OF_BUFFER.
            token.end_itr = m_end_itr;
            return token;
        }

        switch (*m_cur_itr) {
        case '?':
            if (++m_cur_itr == m_end_itr) {
                token.type = lexical_token_t::FLOW_PLAIN_SCALAR;
                token.end_itr = m_cur_itr;
                break;
            }

            if (*m_cur_itr == ' ') {
                token.type = lexical_token_t::EXPLICIT_KEY_PREFIX;
                token.end_itr = m_cur_itr;
                break;
            }

            token.type = scan_scalar(token.end_itr);
            break;
        case ':': { // key separater
            if (++m_cur_itr == m_end_itr) {
                token.type = lexical_token_t::KEY_SEPARATOR;
                token.end_itr = m_end_itr;
                break;
            }

            switch (*m_cur_itr) {
            case ' ':
            case '\t':
            case '\n':
                token.type = lexical_token_t::KEY_SEPARATOR;
                token.end_itr = m_cur_itr + 1;
                break;
            case ',':
            case '[':
            case ']':
            case '{':
            case '}':
                if (m_flow_context_depth > 0) {
                    // the above characters are not "safe" to be followed in a flow context.
                    // See https://yaml.org/spec/1.2.2/#733-plain-style for more details.
                    token.type = lexical_token_t::KEY_SEPARATOR;
                    token.end_itr = m_cur_itr;
                    break;
                }
                token.type = scan_scalar(token.end_itr);
                break;
            default:
                token.type = scan_scalar(token.end_itr);
                break;
            }

            break;
        }
        case ',': // value separater
            token.type = lexical_token_t::VALUE_SEPARATOR;
            token.end_itr = ++m_cur_itr;
            break;
        case '&': { // anchor prefix
            extract_anchor_name(token.end_itr);
            if (token.begin_itr + 1 == token.end_itr) {
                emit_error("anchor name must not be empty.");
            }
            token.type = lexical_token_t::ANCHOR;
            break;
        }
        case '*': { // alias prefix
            extract_anchor_name(token.end_itr);
            if (token.begin_itr + 1 == token.end_itr) {
                emit_error("anchor name must not be empty.");
            }
            token.type = lexical_token_t::ALIAS;
            break;
        }
        case '!':
            extract_tag_name(token.end_itr);
            token.type = lexical_token_t::TAG;
            break;
        case '#': // comment prefix
            scan_comment();
            return get_next_token();
        case '%': // directive prefix
            token.type = scan_directive(token.end_itr);
            break;
        case '-': {
            switch (*(m_cur_itr + 1)) {
            case ' ':
            case '\t':
            case '\n':
                // Move a cursor to the beginning of the next token.
                token.type = lexical_token_t::BLOCK_SEQUENCE_ITEM_PREFIX;
                m_cur_itr += 2;
                token.end_itr = m_cur_itr;
                return token;
            default:
                break;
            }

            bool is_available = (std::distance(m_cur_itr, m_end_itr) > 2);
            if (is_available) {
                if (std::equal(token.begin_itr, m_cur_itr + 3, "---")) {
                    token.type = lexical_token_t::END_OF_DIRECTIVES;
                    m_cur_itr += 3;
                    token.end_itr = m_cur_itr;
                    return token;
                }
            }

            token.type = scan_scalar(token.end_itr);
            break;
        }
        case '[': // sequence flow begin
            m_flow_context_depth++;
            token.type = lexical_token_t::FLOW_SEQUENCE_BEGIN;
            token.end_itr = ++m_cur_itr;
            return token;
        case ']': // sequence flow end
            m_flow_context_depth--;
            token.type = lexical_token_t::FLOW_SEQUENCE_END;
            token.end_itr = ++m_cur_itr;
            return token;
        case '{': // mapping flow begin
            m_flow_context_depth++;
            token.type = lexical_token_t::FLOW_MAPPING_BEGIN;
            token.end_itr = ++m_cur_itr;
            return token;
        case '}': // mapping flow end
            m_flow_context_depth--;
            token.type = lexical_token_t::FLOW_MAPPING_END;
            token.end_itr = ++m_cur_itr;
            return token;
        case '@':
            emit_error("Any token cannot start with at(@). It is a reserved indicator for YAML.");
        case '`':
            emit_error("Any token cannot start with grave accent(`). It is a reserved indicator for YAML.");
        case '\"':
        case '\'':
            token.type = scan_scalar(token.end_itr);
            return token;
        case '+':
            token.type = scan_scalar(token.end_itr);
            return token;
        case '.': {
            bool is_available = (std::distance(m_cur_itr, m_end_itr) > 2);
            if (is_available) {
                if (std::equal(m_cur_itr, m_cur_itr + 3, "...")) {
                    m_cur_itr += 3;
                    token.type = lexical_token_t::END_OF_DOCUMENT;
                    token.end_itr = m_cur_itr;
                    return token;
                }
            }

            token.type = scan_scalar(token.end_itr);
            return token;
        }
        case '|': {
            block_scalar_header* p_header = reinterpret_cast<block_scalar_header*>(token.p_extra_info);
            p_header = new block_scalar_header();
            p_header->style_type = block_style_indicator_t::LITERAL;

            get_block_style_metadata(*p_header);
            token.begin_itr = m_cur_itr;
            token.type = determine_block_scalar_range(*p_header, token.end_itr);
            return token;
        }
        case '>': {
            block_scalar_header* p_header = reinterpret_cast<block_scalar_header*>(token.p_extra_info);
            p_header = new block_scalar_header();
            p_header->style_type = block_style_indicator_t::FOLDED;

            get_block_style_metadata(*p_header);
            token.begin_itr = m_cur_itr;
            token.type = determine_block_scalar_range(*p_header, token.end_itr);
            return token;
        }
        default:
            token.type = scan_scalar(token.end_itr);
            return token;
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
    lexical_token_t scan_directive(iterator_type& end_itr) {
        FK_YAML_ASSERT(*m_cur_itr == '%');
        ++m_cur_itr;

        bool ends_loop = false;
        while (!ends_loop && m_cur_itr != m_end_itr) {
            if (*m_cur_itr == '\n') {
                break;
            }
            ++m_cur_itr;
        }

        end_itr = m_cur_itr;
        return lexical_token_t::DIRECTIVE;
    }

    /// @brief Extracts an anchor name from the input.
    void extract_anchor_name(iterator_type& end_itr) {
        FK_YAML_ASSERT(*m_cur_itr == '&' || *m_cur_itr == '*');

        for (++m_cur_itr; m_cur_itr != m_end_itr; ++m_cur_itr) {
            bool ends_loop = false;
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
                ++m_cur_itr;
                break;
            }
        }

        end_itr = m_cur_itr;
    }

    /// @brief Extracts a tag name from the input.
    void extract_tag_name(iterator_type& end_itr) {
        FK_YAML_ASSERT(*m_cur_itr == '!');

        if (++m_cur_itr == m_end_itr) {
            // Just "!" is a non-specific tag.
            end_itr = m_cur_itr;
            return;
        }

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
            default:
                break;
            }
        } while (!ends_loop);

        end_itr = m_cur_itr;
    }

    /// @brief Scan a string token, either plain, single-quoted or double-quoted.
    /// @return lexical_token_t The lexical token type for strings.
    lexical_token_t scan_scalar(iterator_type& end_itr) {
        bool needs_last_single_quote = false;
        bool needs_last_double_quote = false;
        if (m_cur_itr == m_token_begin_itr) {
            needs_last_single_quote = (*m_cur_itr == '\'');
            needs_last_double_quote = (*m_cur_itr == '\"');
            if (needs_last_double_quote || needs_last_single_quote) {
                ++m_cur_itr;
            }
        }

        lexical_token_t type = extract_string_token(needs_last_single_quote, needs_last_double_quote, end_itr);
        FK_YAML_ASSERT(
            (needs_last_single_quote && type == lexical_token_t::FLOW_SINGLE_QUOTED_SCALAR) ||
            (needs_last_double_quote && type == lexical_token_t::FLOW_DOUBLE_QUOTED_SCALAR) ||
            type == lexical_token_t::FLOW_PLAIN_SCALAR);

        return type;
    }

    /// @brief Check if the given character is allowed in a single-quoted scalar token.
    /// @param c The character to be checked.
    /// @return true if the given character is allowed, false otherwise.
    bool is_allowed_single(char c) {
        switch (c) {
        case '\'':
            // If single quotation marks are repeated twice in a single-quoted string token,
            // they are considered as an escaped single quotation mark.
            if (++m_cur_itr == m_end_itr) {
                return false;
            }

            if (*m_cur_itr != '\'') {
                return false;
            }

            return true;
        default:
            return true;
        }
    }

    /// @brief Check if the given character is allowed in a double-quoted scalar token.
    /// @param c The character to be checked.
    /// @return true if the given character is allowed, false otherwise.
    bool is_allowed_double(char c) {
        switch (c) {
        case '\"':
            return false;

        case '\\':
            // Handle escaped characters.
            // See https://yaml.org/spec/1.2.2/#57-escaped-characters for details.

            c = *(m_cur_itr + 1);

            if (c != '\n') {
                uint32_t size = 0;
                bool is_valid_escaping = yaml_escaper::check_escape_sequence(m_cur_itr, m_end_itr, size);
                if (!is_valid_escaping) {
                    emit_error("Unsupported escape sequence is found in a string token.");
                }

                std::advance(m_cur_itr, size - 1);
            }

            return true;

        default:
            return true;
        }
    }

    /// @brief Check if the given character is allowed in a plain scalar token.
    /// @param c The character to be checked.
    /// @return true if the given character is allowed, false otherwise.
    bool is_allowed_plain(char c) {
        switch (c) {
        case '\n':
            return false;

        case ' ': {
            // Allow a space in an unquoted string only if the space is surrounded by non-space characters.
            // See https://yaml.org/spec/1.2.2/#733-plain-style for more details.

            switch (*(m_cur_itr + 1)) {
            case '{':
            case '}':
            case '[':
            case ']':
            case ',':
                // These characters are permitted when not inside a flow collection, and not inside an implicit key.
                // TODO: Support detection of implicit key context for this check.
                return (m_flow_context_depth == 0);
            case ':':
                // " :" is permitted in a plain style string token, but not when followed by a space.
                return (*(m_cur_itr + 2) == ' ');
            case ' ':
            case '\n':
            case '#':
            case '\\':
                return false;
            default:
                return true;
            }
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
            default:
                return true;
            }
        }

        case '{':
        case '}':
        case '[':
        case ']':
        case ',':
            // just regard the flow indicators as a normal character if plain but not inside a flow context.
            return (m_flow_context_depth == 0);
        }

        return true;
    }

    /// @brief Extracts a string token, either plain, single-quoted or double-quoted, from the input buffer.
    /// @return lexical_token_t The lexical token type for strings.
    lexical_token_t extract_string_token(
        bool needs_last_single_quote, bool needs_last_double_quote, iterator_type& end_itr) {
        // change behaviors depending on the type of a comming string scalar token.
        // * single quoted
        // * double quoted
        // * plain

        lexical_token_t ret_type {lexical_token_t::FLOW_PLAIN_SCALAR};
        std::string check_filters {};
        bool (lexical_analyzer::*pfn_is_allowed)(char) = nullptr;

        if (needs_last_single_quote) {
            ret_type = lexical_token_t::FLOW_SINGLE_QUOTED_SCALAR;
            check_filters.append("\'");
            pfn_is_allowed = &lexical_analyzer::is_allowed_single;
        }
        else if (needs_last_double_quote) {
            ret_type = lexical_token_t::FLOW_DOUBLE_QUOTED_SCALAR;
            check_filters.append("\"\\");
            pfn_is_allowed = &lexical_analyzer::is_allowed_double;
        }
        else // plain scalars
        {
            check_filters.append(" :\n{}[],");
            pfn_is_allowed = &lexical_analyzer::is_allowed_plain;
        }

        // scan the contents of a string scalar token.

        while (m_cur_itr != m_end_itr) {
            char current = *m_cur_itr;
            uint32_t num_bytes = utf8::get_num_bytes(static_cast<uint8_t>(current));
            if (num_bytes == 1) {
                auto ret = check_filters.find(current);
                if (ret != std::string::npos) {
                    bool is_allowed = (this->*pfn_is_allowed)(current);
                    if (!is_allowed) {
                        end_itr = m_cur_itr;
                        break;
                    }
                }

                // Handle unescaped control characters.
                if (static_cast<uint8_t>(current) <= 0x1F) {
                    handle_unescaped_control_char(current);
                }
            }

            // Advance the iterator based on the byte size for the current character.
            // Note multi-byte characters are already validated while creating an input handler.
            m_cur_itr += num_bytes;
        }

        // Handle the end of input buffer.

        if (needs_last_double_quote && *(m_cur_itr - 1) != '\"') {
            emit_error("Invalid end of a double-quoted string token.");
        }

        if (needs_last_single_quote && *(m_cur_itr - 1) != '\'') {
            emit_error("Invalid end of a single-quoted string token.");
        }

        end_itr = m_cur_itr;
        return ret_type;
    }

    /// @brief Scan a block style string token either in the literal or folded style.
    /// @param style The style of the given token, either literal or folded.
    /// @param chomp The chomping indicator type of the given token, either strip, keep or clip.
    /// @param indent The indent size specified for the given token.
    /// @return The lexical token type for strings.
    lexical_token_t determine_block_scalar_range(block_scalar_header& header, iterator_type& end_itr) {
        // Handle leading all-space lines.
        skip_white_spaces_and_newline_codes();

        if (m_cur_itr == m_end_itr) {
            end_itr = m_cur_itr;
            return lexical_token_t::BLOCK_SCALAR;
        }

        m_pos_tracker.update_position(m_cur_itr);
        uint32_t cur_indent = m_pos_tracker.get_cur_pos_in_line();

        // TODO: preserve and compare the last indentation with `cur_indent`
        if (header.indent == 0) {
            header.indent = cur_indent;
        }
        else if (cur_indent < header.indent) {
            emit_error("A block style scalar is less indented than the indicated level.");
        }

        // Skip reading the first content line which is known as part of the block scalar contents.
        skip_until_line_end();

        while (m_cur_itr != m_end_itr) {
            end_itr = m_cur_itr;
            for (uint32_t i = cur_indent; i < header.indent && m_cur_itr != m_end_itr; i++, ++m_cur_itr) {
                switch (*m_cur_itr) {
                case ' ':
                case '\t':
                case '\n':
                    // Interpret less indented non-space characters as the start of the next token.
                    return lexical_token_t::BLOCK_SCALAR;
                }
            }
            skip_until_line_end();
        }

        // Reach here when the end of the block scalar is also the end of the input.
        end_itr = m_end_itr;
        return lexical_token_t::BLOCK_SCALAR;
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
    void get_block_style_metadata(block_scalar_header& header) {
        header.chomping_type = chomping_indicator_t::CLIP;
        switch (*++m_cur_itr) {
        case '-':
            header.chomping_type = chomping_indicator_t::STRIP;
            ++m_cur_itr;
            break;
        case '+':
            header.chomping_type = chomping_indicator_t::KEEP;
            ++m_cur_itr;
            break;
        default:
            break;
        }

        if (*m_cur_itr == '0') {
            emit_error("An indentation level for a block style scalar cannot be \'0\'");
        }

        header.indent = 0;
        if (std::isdigit(*m_cur_itr)) {
            header.indent = static_cast<uint32_t>(*m_cur_itr++ - '0');
        }

        // skip characters including comments.
        skip_until_line_end();
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
    std::string m_input_buffer {};
    /// The iterator to the current character in the input buffer.
    iterator_type m_cur_itr {};
    /// The iterator to the past-the-end element in the input buffer.
    iterator_type m_end_itr {};
    /// The beginning iterator of the current token.
    iterator_type m_token_begin_itr {};
    /// The past-the-end iterator of the current token.
    iterator_type m_token_end_itr {};
    /// The current position tracker of the input buffer.
    mutable position_tracker m_pos_tracker {};
    /// The current depth of flow context.
    uint32_t m_flow_context_depth {0};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_LEXICAL_ANALIZER_HPP_ */
