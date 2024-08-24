///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
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
#include <fkYAML/detail/conversions/from_string.hpp>
#include <fkYAML/detail/encodings/uri_encoding.hpp>
#include <fkYAML/detail/encodings/utf_encodings.hpp>
#include <fkYAML/detail/encodings/yaml_escaper.hpp>
#include <fkYAML/detail/input/scalar_scanner.hpp>
#include <fkYAML/detail/input/position_tracker.hpp>
#include <fkYAML/detail/meta/input_adapter_traits.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/types/lexical_token_t.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief A class which lexically analizes YAML formatted inputs.
/// @tparam BasicNodeType A type of the container for YAML values.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
class lexical_analyzer {
private:
    using char_traits_type = typename std::char_traits<char>;

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
    using boolean_type = typename BasicNodeType::boolean_type;
    using integer_type = typename BasicNodeType::integer_type;
    using float_number_type = typename BasicNodeType::float_number_type;
    using string_type = typename BasicNodeType::string_type;

    /// @brief Construct a new lexical_analyzer object.
    /// @tparam InputAdapterType The type of the input adapter.
    /// @param input_adapter An input adapter object.
    template <typename InputAdapterType, enable_if_t<is_input_adapter<InputAdapterType>::value, int> = 0>
    explicit lexical_analyzer(InputAdapterType&& input_adapter) {
        std::forward<InputAdapterType>(input_adapter).fill_buffer(m_input_buffer);
        m_cur_itr = m_token_begin_itr = m_input_buffer.cbegin();
        m_end_itr = m_input_buffer.cend();
        m_pos_tracker.set_target_buffer(m_input_buffer);
    }

    /// @brief Get the next lexical token type by scanning the left of the input buffer.
    /// @return lexical_token_t The next lexical token type.
    lexical_token_t get_next_token() {
        skip_white_spaces_and_newline_codes();

        m_token_begin_itr = m_cur_itr;
        m_pos_tracker.update_position(m_cur_itr);
        m_last_token_begin_pos = m_pos_tracker.get_cur_pos_in_line();
        m_last_token_begin_line = m_pos_tracker.get_lines_read();

        if (m_cur_itr == m_end_itr) {
            return lexical_token_t::END_OF_BUFFER;
        }

        switch (char current = *m_cur_itr) {
        case '?':
            if (++m_cur_itr == m_end_itr) {
                m_value_buffer = "?";
                return lexical_token_t::STRING_VALUE;
            }

            switch (*m_cur_itr) {
            case ' ':
                return lexical_token_t::EXPLICIT_KEY_PREFIX;
            default:
                return scan_scalar();
            }
        case ':': { // key separater
            if (++m_cur_itr == m_end_itr) {
                return lexical_token_t::KEY_SEPARATOR;
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
                if (m_flow_context_depth > 0) {
                    // the above characters are not "safe" to be followed in a flow context.
                    // See https://yaml.org/spec/1.2.2/#733-plain-style for more details.
                    break;
                }
                return scan_scalar();
            default:
                return scan_scalar();
            }

            return lexical_token_t::KEY_SEPARATOR;
        }
        case ',': // value separater
            ++m_cur_itr;
            return lexical_token_t::VALUE_SEPARATOR;
        case '&': { // anchor prefix
            extract_anchor_name();
            bool is_empty = m_value_buffer.empty();
            if (is_empty) {
                emit_error("anchor name must not be empty.");
            }
            return lexical_token_t::ANCHOR_PREFIX;
        }
        case '*': { // alias prefix
            extract_anchor_name();
            bool is_empty = m_value_buffer.empty();
            if (is_empty) {
                emit_error("anchor name must not be empty.");
            }

            return lexical_token_t::ALIAS_PREFIX;
        }
        case '!':
            extract_tag_name();
            return lexical_token_t::TAG_PREFIX;
        case '#': // comment prefix
            scan_comment();
            return get_next_token();
        case '%': // directive prefix
            return scan_directive();
        case '-': {
            char next = *(m_cur_itr + 1);
            switch (next) {
            case ' ':
            case '\t':
            case '\n':
                // Move a cursor to the beginning of the next token.
                m_cur_itr += 2;
                return lexical_token_t::SEQUENCE_BLOCK_PREFIX;
            default:
                break;
            }

            bool is_available = (std::distance(m_cur_itr, m_end_itr) > 2);
            if (is_available) {
                if (std::equal(m_token_begin_itr, m_cur_itr + 3, "---")) {
                    m_cur_itr += 3;
                    return lexical_token_t::END_OF_DIRECTIVES;
                }
            }

            return scan_scalar();
        }
        case '[': // sequence flow begin
            m_flow_context_depth++;
            ++m_cur_itr;
            return lexical_token_t::SEQUENCE_FLOW_BEGIN;
        case ']': // sequence flow end
            m_flow_context_depth--;
            ++m_cur_itr;
            return lexical_token_t::SEQUENCE_FLOW_END;
        case '{': // mapping flow begin
            m_flow_context_depth++;
            ++m_cur_itr;
            return lexical_token_t::MAPPING_FLOW_BEGIN;
        case '}': // mapping flow end
            m_flow_context_depth--;
            ++m_cur_itr;
            return lexical_token_t::MAPPING_FLOW_END;
        case '@':
            emit_error("Any token cannot start with at(@). It is a reserved indicator for YAML.");
        case '`':
            emit_error("Any token cannot start with grave accent(`). It is a reserved indicator for YAML.");
        case '\"':
        case '\'':
            return scan_scalar();
        case '+':
            return scan_scalar();
        case '.': {
            bool is_available = (std::distance(m_cur_itr, m_end_itr) > 2);
            if (is_available) {
                if (std::equal(m_cur_itr, m_cur_itr + 3, "...")) {
                    m_cur_itr += 3;
                    return lexical_token_t::END_OF_DOCUMENT;
                }
            }

            return scan_scalar();
        }
        case '|': {
            chomping_indicator_t chomp_type = chomping_indicator_t::KEEP;
            uint32_t indent = 0;
            get_block_style_metadata(chomp_type, indent);
            return scan_block_style_string_token(block_style_indicator_t::LITERAL, chomp_type, indent);
        }
        case '>': {
            chomping_indicator_t chomp_type = chomping_indicator_t::KEEP;
            uint32_t indent = 0;
            get_block_style_metadata(chomp_type, indent);
            return scan_block_style_string_token(block_style_indicator_t::FOLDED, chomp_type, indent);
        }
        default:
            return scan_scalar();
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

    /// @brief Convert from string to null and get the converted value.
    /// @return std::nullptr_t A null value converted from one of the followings: "null", "Null", "NULL", "~".
    std::nullptr_t get_null() const {
        return from_string(m_value_buffer, type_tag<std::nullptr_t> {});
    }

    /// @brief Convert from string to boolean and get the converted value.
    /// @retval true  A string token is one of the followings: "true", "True", "TRUE".
    /// @retval false A string token is one of the followings: "false", "False", "FALSE".
    boolean_type get_boolean() const {
        return from_string(m_value_buffer, type_tag<bool> {});
    }

    /// @brief Convert from string to integer and get the converted value.
    /// @return integer_type An integer value converted from the source string.
    integer_type get_integer() const {
        if (m_value_buffer.size() > 2 && m_value_buffer.rfind("0o", 0) != std::string::npos) {
            // Replace the prefix "0o" with "0" since STL functions can detect octal chars.
            // Note that the YAML specifies octal values start with the prefix "0o", not "0".
            // See https://yaml.org/spec/1.2.2/#1032-tag-resolution for more details.
            return from_string("0" + m_value_buffer.substr(2), type_tag<integer_type> {});
        }
        return from_string(m_value_buffer, type_tag<integer_type> {});
    }

    /// @brief Convert from string to float number and get the converted value.
    /// @return float_number_type A float number value converted from the source string.
    float_number_type get_float_number() const {
        return from_string(m_value_buffer, type_tag<float_number_type> {});
    }

    /// @brief Get a scanned string value.
    /// @return const string_type& Constant reference to a scanned string.
    const string_type& get_string() const noexcept {
        // TODO: Provide support for different string types between nodes & inputs.
        static_assert(std::is_same<string_type, std::string>::value, "Unsupported, different string types.");
        return m_value_buffer;
    }

    /// @brief Get the YAML version specification.
    /// @return const string_type& A YAML version specification.
    const string_type& get_yaml_version() const {
        FK_YAML_ASSERT(!m_value_buffer.empty() && m_value_buffer.size() == 3);
        FK_YAML_ASSERT(m_value_buffer == "1.1" || m_value_buffer == "1.2");

        return m_value_buffer;
    }

    /// @brief Get the YAML tag handle defined in the TAG directive.
    /// @return const std::string& A tag handle.
    const std::string& get_tag_handle() const {
        FK_YAML_ASSERT(!m_tag_handle.empty());
        return m_tag_handle;
    }

    /// @brief Get the YAML tag prefix defined in the TAG directive.
    /// @return const std::string A tag prefix.
    const std::string& get_tag_prefix() const {
        FK_YAML_ASSERT(!m_tag_prefix.empty());
        return m_tag_prefix;
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

        m_value_buffer.assign(m_token_begin_itr, m_cur_itr);

        if (m_value_buffer == "TAG") {
            if (!ends_loop) {
                emit_error("There must be at least one white space between \"%TAG\" and tag info.");
            }
            skip_white_spaces();
            return scan_tag_directive();
        }

        if (m_value_buffer == "YAML") {
            if (!ends_loop) {
                emit_error("There must be at least one white space between \"%YAML\" and tag info.");
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
        m_tag_handle.clear();
        m_tag_prefix.clear();
        m_token_begin_itr = m_cur_itr;

        //
        // extract a tag handle
        //

        if (*m_cur_itr != '!') {
            emit_error("Tag handle must start with \'!\'.");
        }

        if (++m_cur_itr == m_end_itr) {
            emit_error("invalid TAG directive is found.");
        }

        switch (*m_cur_itr) {
        case ' ':
        case '\t':
            // primary handle (!)
            break;
        case '!':
            if (++m_cur_itr == m_end_itr) {
                emit_error("invalid TAG directive is found.");
            }
            if (*m_cur_itr != ' ' && *m_cur_itr != '\t') {
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
                    if (next != ' ' && next != '\t') {
                        emit_error("invalid tag handle is found.");
                    }
                    ends_loop = true;
                    break;
                }
                case '-':
                    break;
                default:
                    if (!isalnum(*m_cur_itr)) {
                        // See https://yaml.org/spec/1.2.2/#rule-c-named-tag-handle for more details.
                        emit_error("named handle can contain only numbers(0-9), alphabets(A-Z,a-z) and hyphens(-).");
                    }
                    break;
                }

                if (++m_cur_itr == m_end_itr) {
                    emit_error("invalid TAG directive is found.");
                }
            } while (!ends_loop);
            break;
        }
        }

        m_tag_handle.assign(m_token_begin_itr, m_cur_itr);

        skip_white_spaces();

        //
        // extract a tag prefix.
        //

        m_token_begin_itr = m_cur_itr;
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

        m_tag_prefix.assign(m_token_begin_itr, m_cur_itr);

        bool is_valid = uri_encoding::validate(m_tag_prefix.begin(), m_tag_prefix.end());
        if (!is_valid) {
            emit_error("invalid URI character is found in a tag prefix.");
        }

        return lexical_token_t::TAG_DIRECTIVE;
    }

    /// @brief Scan a YAML version directive.
    /// @note Only 1.1 and 1.2 are supported. If not, throws an exception.
    /// @return lexical_token_t The lexical token type for YAML version directives.
    lexical_token_t scan_yaml_version_directive() {
        m_value_buffer.clear();
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

        m_value_buffer.assign(m_token_begin_itr, m_cur_itr);

        if (m_value_buffer != "1.1" && m_value_buffer != "1.2") {
            emit_error("Only 1.1 and 1.2 can be specified as the YAML version.");
        }

        return lexical_token_t::YAML_VER_DIRECTIVE;
    }

    /// @brief Extracts an anchor name from the input and assigns the result to `m_value_buffer`.
    void extract_anchor_name() {
        FK_YAML_ASSERT(*m_cur_itr == '&' || *m_cur_itr == '*');

        m_value_buffer.clear();
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

        m_value_buffer.assign(m_token_begin_itr, m_cur_itr);
    }

    /// @brief Extracts a tag name from the input and assigns the result to `m_value_buffer`.
    void extract_tag_name() {
        m_value_buffer.clear();

        FK_YAML_ASSERT(*m_cur_itr == '!');

        if (++m_cur_itr == m_end_itr) {
            // Just "!" is a non-specific tag.
            m_value_buffer = "!";
            return;
        }

        bool is_verbatim = false;
        bool allows_another_tag_prefix = false;

        switch (*m_cur_itr) {
        case ' ':
        case '\n':
            // Just "!" is a non-specific tag.
            m_value_buffer = "!";
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
                if (!allows_another_tag_prefix) {
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

        m_value_buffer.assign(m_token_begin_itr, m_cur_itr);

        if (is_verbatim) {
            char last = m_value_buffer.back();
            if (last != '>') {
                emit_error("verbatim tag (!<TAG>) must be ended with \'>\'.");
            }

            auto tag_begin = m_value_buffer.begin() + 2;
            auto tag_end = m_value_buffer.end() - 1;
            if (tag_begin == tag_end) {
                emit_error("verbatim tag(!<TAG>) must not be empty.");
            }

            bool is_valid_uri = uri_encoding::validate(tag_begin, tag_end);
            if (!is_valid_uri) {
                emit_error("invalid URI character is found in a verbatim tag.");
            }

            return;
        }

        if (is_named_handle) {
            char last = m_value_buffer.back();
            if (last == '!') {
                // Tag shorthand must be followed by a non-empty suffix.
                // See the "Tag Shorthands" section in https://yaml.org/spec/1.2.2/#691-node-tags.
                emit_error("named handle has no suffix.");
            }

            std::size_t last_tag_prefix_pos = m_value_buffer.find_last_of('!');
            FK_YAML_ASSERT(last_tag_prefix_pos != std::string::npos);

            bool is_valid_uri =
                uri_encoding::validate(m_value_buffer.begin() + last_tag_prefix_pos + 1, m_value_buffer.end());
            if (!is_valid_uri) {
                emit_error("Invalid URI character is found in a named tag handle.");
            }
        }
    }

    /// @brief Scan a string token, either plain, single-quoted or double-quoted.
    /// @return lexical_token_t The lexical token type for strings.
    lexical_token_t scan_scalar() {
        m_value_buffer.clear();

        bool needs_last_single_quote = false;
        bool needs_last_double_quote = false;
        if (m_cur_itr == m_token_begin_itr) {
            needs_last_single_quote = (*m_cur_itr == '\'');
            needs_last_double_quote = (*m_cur_itr == '\"');
            if (needs_last_double_quote || needs_last_single_quote) {
                m_token_begin_itr = ++m_cur_itr;
            }
        }

        lexical_token_t type = extract_string_token(needs_last_single_quote, needs_last_double_quote);
        FK_YAML_ASSERT(type == lexical_token_t::STRING_VALUE);

        if (needs_last_single_quote || needs_last_double_quote) {
            // just returned the extracted string value if quoted.
            return type;
        }

        return scalar_scanner::scan(m_value_buffer);
    }

    /// @brief Check if the given character is allowed in a single-quoted scalar token.
    /// @param c The character to be checked.
    /// @return true if the given character is allowed, false otherwise.
    bool is_allowed_single(char c) {
        bool ret = false;

        switch (c) {
        case '\n': {
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
            ret = true;
            break;
        }

        case '\'':
            // If single quotation marks are repeated twice in a single-quoted string token,
            // they are considered as an escaped single quotation mark.
            if (m_cur_itr + 1 == m_end_itr) {
                m_value_buffer.append(m_token_begin_itr, m_cur_itr++);
                m_token_begin_itr = m_cur_itr;
                break;
            }

            if (*(m_cur_itr + 1) != '\'') {
                m_value_buffer.append(m_token_begin_itr, m_cur_itr++);
                break;
            }

            m_value_buffer.append(m_token_begin_itr, ++m_cur_itr);
            m_token_begin_itr = m_cur_itr + 1;
            ret = true;
            break;
        }

        return ret;
    }

    /// @brief Check if the given character is allowed in a double-quoted scalar token.
    /// @param c The character to be checked.
    /// @return true if the given character is allowed, false otherwise.
    bool is_allowed_double(char c) {
        bool ret = false;

        switch (c) {
        case '\n': {
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
            ret = true;
            break;
        }

        case '\"':
            m_value_buffer.append(m_token_begin_itr, m_cur_itr++);
            break;

        case '\\':
            m_value_buffer.append(m_token_begin_itr, m_cur_itr);

            // Handle escaped characters.
            // See https://yaml.org/spec/1.2.2/#57-escaped-characters for more details.

            c = *(m_cur_itr + 1);
            if (c != '\n') {
                bool is_valid_escaping = yaml_escaper::unescape(m_cur_itr, m_end_itr, m_value_buffer);
                if (!is_valid_escaping) {
                    emit_error("Unsupported escape sequence is found in a string token.");
                }

                m_token_begin_itr = m_cur_itr + 1;
                ret = true;
                break;
            }

            // move until the next non-space character is found.
            m_cur_itr += 2;
            skip_white_spaces();

            m_token_begin_itr = (m_cur_itr == m_end_itr || *m_cur_itr == '\"') ? m_cur_itr-- : m_cur_itr;
            ret = true;
            break;
        }

        return ret;
    }

    /// @brief Check if the given character is allowed in a plain scalar token.
    /// @param c The character to be checked.
    /// @return true if the given character is allowed, false otherwise.
    bool is_allowed_plain(char c) {
        bool ret = false;

        switch (c) {
        case '\n':
            m_value_buffer.append(m_token_begin_itr, m_cur_itr);
            break;

        case ' ': {
            // Allow a space in an unquoted string only if the space is surrounded by non-space characters.
            // See https://yaml.org/spec/1.2.2/#733-plain-style for more details.
            char next = *(m_cur_itr + 1);
            bool is_appended = false;

            // These characters are permitted when not inside a flow collection, and not inside an implicit key.
            // TODO: Support detection of implicit key context for this check.
            if (m_flow_context_depth > 0) {
                switch (next) {
                case '{':
                case '}':
                case '[':
                case ']':
                case ',':
                    m_value_buffer.append(m_token_begin_itr, m_cur_itr++);
                    is_appended = true;
                    break;
                }

                if (is_appended) {
                    break;
                }
            }

            // " :" is permitted in a plain style string token, but not when followed by a space.
            if (next == ':') {
                char peeked = *(m_cur_itr + 2);
                if (peeked == ' ') {
                    m_value_buffer.append(m_token_begin_itr, m_cur_itr++);
                    is_appended = true;
                }

                if (is_appended) {
                    break;
                }
            }

            switch (next) {
            case ' ':
            case '\n':
            case '#':
            case '\\':
                m_value_buffer.append(m_token_begin_itr, m_cur_itr++);
                is_appended = true;
                break;
            }

            ret = !is_appended;
            break;
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
                m_value_buffer.append(m_token_begin_itr, m_cur_itr);
                break;
            default:
                ret = true;
                break;
            }
            break;
        }

        case '{':
        case '}':
        case '[':
        case ']':
        case ',':
            // just regard the flow indicators as a normal character if plain but not inside a flow context.
            if (m_flow_context_depth == 0) {
                ret = true;
                break;
            }

            m_value_buffer.append(m_token_begin_itr, m_cur_itr);
            break;
        }

        return ret;
    }

    /// @brief Extracts a string token, either plain, single-quoted or double-quoted, from the input buffer.
    /// @return lexical_token_t The lexical token type for strings.
    lexical_token_t extract_string_token(bool needs_last_single_quote, bool needs_last_double_quote) {
        // change behaviors depending on the type of a comming string scalar token.
        // * single quoted
        // * double quoted
        // * plain

        std::string check_filters {"\n"};
        bool (lexical_analyzer::*pfn_is_allowed)(char) = nullptr;

        if (needs_last_single_quote) {
            check_filters.append("\'");
            pfn_is_allowed = &lexical_analyzer::is_allowed_single;
        }
        else if (needs_last_double_quote) {
            check_filters.append("\"\\");
            pfn_is_allowed = &lexical_analyzer::is_allowed_double;
        }
        else // plain scalars
        {
            check_filters.append(" :{}[],");
            pfn_is_allowed = &lexical_analyzer::is_allowed_plain;
        }

        // scan the contents of a string scalar token.

        for (; m_cur_itr != m_end_itr; m_cur_itr = (m_cur_itr == m_end_itr) ? m_cur_itr : ++m_cur_itr) {
            char current = *m_cur_itr;
            uint32_t num_bytes = utf8::get_num_bytes(static_cast<uint8_t>(current));
            if (num_bytes == 1) {
                auto ret = check_filters.find(current);
                if (ret != std::string::npos) {
                    bool is_allowed = (this->*pfn_is_allowed)(current);
                    if (!is_allowed) {
                        return lexical_token_t::STRING_VALUE;
                    }

                    continue;
                }

                // Handle unescaped control characters.
                if (static_cast<uint8_t>(current) <= 0x1F) {
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

        if (needs_last_double_quote) {
            emit_error("Invalid end of input buffer in a double-quoted string token.");
        }

        if (needs_last_single_quote) {
            emit_error("Invalid end of input buffer in a single-quoted string token.");
        }

        m_value_buffer.append(m_token_begin_itr, m_cur_itr);
        return lexical_token_t::STRING_VALUE;
    }

    /// @brief Scan a block style string token either in the literal or folded style.
    /// @param style The style of the given token, either literal or folded.
    /// @param chomp The chomping indicator type of the given token, either strip, keep or clip.
    /// @param indent The indent size specified for the given token.
    /// @return The lexical token type for strings.
    lexical_token_t scan_block_style_string_token(
        block_style_indicator_t style, chomping_indicator_t chomp, uint32_t indent) {
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
            return lexical_token_t::STRING_VALUE;
        }

        m_pos_tracker.update_position(m_cur_itr);
        uint32_t cur_indent = m_pos_tracker.get_cur_pos_in_line();

        // TODO: preserve and compare the last indentation with `cur_indent`
        if (indent == 0) {
            indent = cur_indent;
        }
        else if (cur_indent < indent) {
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
            // m_value_buffer.append(diff, ' ');
            m_token_begin_itr -= diff;
            chars_in_line += diff;
        }

        for (; m_cur_itr != m_end_itr; ++m_cur_itr) {
            if (*m_cur_itr == '\n') {
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

                        char current = *m_cur_itr;
                        if (current == ' ') {
                            continue;
                        }

                        if (current == '\n') {
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
                    else {
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
                    if (*m_cur_itr != ' ') {
                        // Interpret less indented non-space characters as the start of the next token.
                        break;
                    }
                    // skip a space if not yet indented enough
                    continue;
                }

                if (*m_cur_itr == ' ' && style == block_style_indicator_t::FOLDED) {
                    // A line being more indented is not folded.
                    m_value_buffer.push_back('\n');
                    is_extra_indented = true;
                }
                m_token_begin_itr = m_cur_itr;
            }

            // m_value_buffer.push_back(current);
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
            while (m_value_buffer.size() > 1) {
                // Strings with only newlines are handled above, so no check for the case.
                char second_last = *(m_value_buffer.end() - 2);
                if (second_last != '\n') {
                    break;
                }
                m_value_buffer.pop_back();
            }
            break;
        }
        case chomping_indicator_t::KEEP:
            break;
        }

        return lexical_token_t::STRING_VALUE;
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
        switch (*++m_cur_itr) {
        case '-':
            chomp_type = chomping_indicator_t::STRIP;
            ++m_cur_itr;
            break;
        case '+':
            chomp_type = chomping_indicator_t::KEEP;
            ++m_cur_itr;
            break;
        default:
            break;
        }

        if (*m_cur_itr == '0') {
            emit_error("An indentation level for a block style scalar cannot be \'0\'");
        }

        indent = 0;
        if (std::isdigit(*m_cur_itr)) {
            indent = static_cast<char>(*m_cur_itr++ - '0');
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
    std::string::const_iterator m_cur_itr {};
    /// The iterator to the beginning of the current token.
    std::string::const_iterator m_token_begin_itr {};
    /// The iterator to the past-the-end element in the input buffer.
    std::string::const_iterator m_end_itr {};
    /// The current position tracker of the input buffer.
    mutable position_tracker m_pos_tracker {};
    /// A temporal buffer to store a string to be parsed to an actual token value.
    std::string m_value_buffer {};
    /// The last tag handle.
    std::string m_tag_handle {};
    /// The last tag prefix
    std::string m_tag_prefix {};
    /// The beginning position of the last lexical token. (zero origin)
    uint32_t m_last_token_begin_pos {0};
    /// The beginning line of the last lexical token. (zero origin)
    uint32_t m_last_token_begin_line {0};
    /// The current depth of flow context.
    uint32_t m_flow_context_depth {0};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_LEXICAL_ANALIZER_HPP_ */
