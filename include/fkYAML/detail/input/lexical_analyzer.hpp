///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.3
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
#include <fkYAML/detail/encodings/utf8_encoding.hpp>
#include <fkYAML/detail/input/position_tracker.hpp>
#include <fkYAML/detail/meta/input_adapter_traits.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/types/lexical_token_t.hpp>
#include <fkYAML/exception.hpp>

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

/**
 * @namespace detail
 * @brief namespace for internal implementations of fkYAML library.
 */
namespace detail
{

/// @brief A class which lexically analizes YAML formatted inputs.
/// @tparam BasicNodeType A type of the container for YAML values.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
class lexical_analyzer
{
private:
    using char_traits_type = typename std::char_traits<char>;

    enum class block_style_indicator_t
    {
        LITERAL, //!< keeps newlines inside the block as they are indicated by a pipe `|`.
        FOLDED,  //!< replaces newlines inside the block with spaces indicated by a right angle bracket `>`.
    };

    enum class chomping_indicator_t
    {
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
    explicit lexical_analyzer(InputAdapterType&& input_adapter)
    {
        std::forward<InputAdapterType>(input_adapter).fill_buffer(m_input_buffer);
        m_cur_itr = m_token_begin_itr = m_input_buffer.cbegin();
        m_end_itr = m_input_buffer.cend();
        m_pos_tracker.set_target_buffer(m_input_buffer);
    }

    /// @brief Get the next lexical token type by scanning the left of the input buffer.
    /// @return lexical_token_t The next lexical token type.
    lexical_token_t get_next_token()
    {
        skip_white_spaces_and_newline_codes();

        m_token_begin_itr = m_cur_itr;
        m_pos_tracker.update_position(m_cur_itr);
        m_last_token_begin_pos = m_pos_tracker.get_cur_pos_in_line();
        m_last_token_begin_line = m_pos_tracker.get_lines_read();

        if (m_cur_itr == m_end_itr)
        {
            return m_last_token_type = lexical_token_t::END_OF_BUFFER;
        }

        char current = *m_cur_itr;
        if (isdigit(current))
        {
            return m_last_token_type = scan_number();
        }

        switch (current)
        {
        case '?':
            if (++m_cur_itr == m_end_itr)
            {
                m_value_buffer = "?";
                return m_last_token_type = lexical_token_t::STRING_VALUE;
            }

            switch (*m_cur_itr)
            {
            case ' ':
                return m_last_token_type = lexical_token_t::EXPLICIT_KEY_PREFIX;
            default:
                m_value_buffer = "?";
                return m_last_token_type = scan_string(false);
            }
        case ':': { // key separater
            if (++m_cur_itr == m_end_itr)
            {
                return m_last_token_type = lexical_token_t::KEY_SEPARATOR;
            }

            switch (*m_cur_itr)
            {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                break;
            case ',':
            case '[':
            case ']':
            case '{':
            case '}':
                if (m_flow_context_depth > 0)
                {
                    // the above characters are not "safe" to be followed in a flow context.
                    // See https://yaml.org/spec/1.2.2/#733-plain-style for more details.
                    break;
                }
                m_value_buffer = ":";
                return scan_string(false);
            default:
                m_value_buffer = ":";
                return scan_string(false);
            }

            return m_last_token_type = lexical_token_t::KEY_SEPARATOR;
        }
        case ',': // value separater
            ++m_cur_itr;
            return m_last_token_type = lexical_token_t::VALUE_SEPARATOR;
        case '&': { // anchor prefix
            extract_anchor_name();
            bool is_empty = m_value_buffer.empty();
            if (is_empty)
            {
                emit_error("anchor name must not be empty.");
            }
            return m_last_token_type = lexical_token_t::ANCHOR_PREFIX;
        }
        case '*': { // alias prefix
            extract_anchor_name();
            bool is_empty = m_value_buffer.empty();
            if (is_empty)
            {
                emit_error("anchor name must not be empty.");
            }

            return m_last_token_type = lexical_token_t::ALIAS_PREFIX;
        }
        case '!':
            m_value_buffer.clear();
            extract_tag_name();
            return m_last_token_type = lexical_token_t::TAG_PREFIX;
        case '#': // comment prefix
            scan_comment();
            return m_last_token_type = lexical_token_t::COMMENT_PREFIX;
        case '%': // directive prefix
            return m_last_token_type = scan_directive();
        case '-': {
            char next = *(m_cur_itr + 1);
            if (next == ' ')
            {
                // Move a cursor to the beginning of the next token.
                m_cur_itr += 2;
                return m_last_token_type = lexical_token_t::SEQUENCE_BLOCK_PREFIX;
            }

            if (std::isdigit(next))
            {
                return m_last_token_type = scan_number();
            }

            bool is_available = (std::distance(m_cur_itr, m_end_itr) > 2);
            if (is_available)
            {
                m_cur_itr += 3;
                m_value_buffer.assign(m_token_begin_itr, m_cur_itr);
                if (std::equal(m_token_begin_itr, m_cur_itr, "---"))
                {
                    return m_last_token_type = lexical_token_t::END_OF_DIRECTIVES;
                }
            }

            return m_last_token_type = scan_string(!is_available);
        }
        case '[': // sequence flow begin
            m_flow_context_depth++;
            ++m_cur_itr;
            return m_last_token_type = lexical_token_t::SEQUENCE_FLOW_BEGIN;
        case ']': // sequence flow end
            if (m_flow_context_depth == 0)
            {
                emit_error("An invalid flow sequence ending.");
            }
            m_flow_context_depth--;
            ++m_cur_itr;
            return m_last_token_type = lexical_token_t::SEQUENCE_FLOW_END;
        case '{': // mapping flow begin
            m_flow_context_depth++;
            ++m_cur_itr;
            return m_last_token_type = lexical_token_t::MAPPING_FLOW_BEGIN;
        case '}': // mapping flow end
            if (m_flow_context_depth == 0)
            {
                emit_error("An invalid flow mapping ending.");
            }
            m_flow_context_depth--;
            ++m_cur_itr;
            return m_last_token_type = lexical_token_t::MAPPING_FLOW_END;
        case '@':
            emit_error("Any token cannot start with at(@). It is a reserved indicator for YAML.");
        case '`':
            emit_error("Any token cannot start with grave accent(`). It is a reserved indicator for YAML.");
        case '\"':
        case '\'':
            return m_last_token_type = scan_string();
        case '+':
            return m_last_token_type = scan_number();
        case '.': {
            bool is_available = (std::distance(m_cur_itr, m_end_itr) > 2);
            if (is_available)
            {
                if (std::equal(m_cur_itr, m_cur_itr + 3, "..."))
                {
                    m_cur_itr += 3;
                    return m_last_token_type = lexical_token_t::END_OF_DOCUMENT;
                }
            }

            return m_last_token_type = scan_string();
        }
        case '|': {
            chomping_indicator_t chomp_type = chomping_indicator_t::KEEP;
            std::size_t indent = 0;
            get_block_style_metadata(chomp_type, indent);
            return m_last_token_type =
                       scan_block_style_string_token(block_style_indicator_t::LITERAL, chomp_type, indent);
        }
        case '>': {
            chomping_indicator_t chomp_type = chomping_indicator_t::KEEP;
            std::size_t indent = 0;
            get_block_style_metadata(chomp_type, indent);
            return m_last_token_type =
                       scan_block_style_string_token(block_style_indicator_t::FOLDED, chomp_type, indent);
        }
        default:
            return m_last_token_type = scan_string();
        }
    }

    /// @brief Get the beginning position of a last token.
    /// @return std::size_t The beginning position of a last token.
    std::size_t get_last_token_begin_pos() const noexcept
    {
        return m_last_token_begin_pos;
    }

    /// @brief Get the number of lines already processed.
    /// @return std::size_t The number of lines already processed.
    std::size_t get_lines_processed() const noexcept
    {
        return m_last_token_begin_line;
    }

    /// @brief Convert from string to null and get the converted value.
    /// @return std::nullptr_t A null value converted from one of the followings: "null", "Null", "NULL", "~".
    std::nullptr_t get_null() const
    {
        if (m_last_token_type == lexical_token_t::NULL_VALUE)
        {
            return nullptr;
        }
        emit_error("Invalid request for a null value.");
    }

    /// @brief Convert from string to boolean and get the converted value.
    /// @return true  A string token is one of the followings: "true", "True", "TRUE".
    /// @return false A string token is one of the followings: "false", "False", "FALSE".
    boolean_type get_boolean() const
    {
        if (m_last_token_type == lexical_token_t::BOOLEAN_VALUE)
        {
            return m_boolean_val;
        }
        emit_error("Invalid request for a boolean value.");
    }

    /// @brief Convert from string to integer and get the converted value.
    /// @return integer_type An integer value converted from the source string.
    integer_type get_integer() const
    {
        if (m_last_token_type == lexical_token_t::INTEGER_VALUE)
        {
            return m_integer_val;
        }
        emit_error("Invalid request for an integer value.");
    }

    /// @brief Convert from string to float number and get the converted value.
    /// @return float_number_type A float number value converted from the source string.
    float_number_type get_float_number() const
    {
        if (m_last_token_type == lexical_token_t::FLOAT_NUMBER_VALUE)
        {
            return m_float_val;
        }
        emit_error("Invalid request for a float number value.");
    }

    /// @brief Get a scanned string value.
    /// @return const string_type& Constant reference to a scanned string.
    const string_type& get_string() const noexcept
    {
        // TODO: Provide support for different string types between nodes & inputs.
        static_assert(std::is_same<string_type, std::string>::value, "Unsupported, different string types.");
        return m_value_buffer;
    }

    /// @brief Get the YAML version specification.
    /// @return const string_type& A YAML version specification.
    const string_type& get_yaml_version() const
    {
        FK_YAML_ASSERT(!m_value_buffer.empty() && m_value_buffer.size() == 3);
        FK_YAML_ASSERT(m_value_buffer == "1.1" || m_value_buffer == "1.2");

        return m_value_buffer;
    }

    const std::string& get_tag_handle() const
    {
        FK_YAML_ASSERT(!m_tag_handle.empty());
        return m_tag_handle;
    }

    const std::string& get_tag_prefix() const
    {
        FK_YAML_ASSERT(!m_tag_prefix.empty());
        return m_tag_prefix;
    }

private:
    /// @brief A utility function to convert a hexadecimal character to an integer.
    /// @param source A hexadecimal character ('0'~'9', 'A'~'F', 'a'~'f')
    /// @return char A integer converted from @a source.
    char convert_hex_char_to_byte(char source) const
    {
        if ('0' <= source && source <= '9')
        {
            // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
            return static_cast<char>(source - '0');
        }

        if ('A' <= source && source <= 'F')
        {
            // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
            return static_cast<char>(source - 'A' + 10);
        }

        if ('a' <= source && source <= 'f')
        {
            // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
            return static_cast<char>(source - 'a' + 10);
        }

        emit_error("Non-hexadecimal character has been given.");
    }

    /// @brief Skip until a newline code or a null character is found.
    /// @return lexical_token_t The lexical token type for comments
    lexical_token_t scan_comment()
    {
        FK_YAML_ASSERT(*m_cur_itr == '#');

        skip_until_line_end();
        return lexical_token_t::COMMENT_PREFIX;
    }

    /// @brief Scan directives starting with the prefix '%'
    /// @note Currently, only %YAML directive is supported. If not, returns invalid or throws an exception.
    /// @return lexical_token_t The lexical token type for directives.
    lexical_token_t scan_directive()
    {
        FK_YAML_ASSERT(*m_cur_itr == '%');

        if (++m_cur_itr == m_end_itr)
        {
            emit_error("invalid eof in a directive.");
        }

        switch (*m_cur_itr)
        {
        case 'T': {
            if (*++m_cur_itr != 'A' || *++m_cur_itr != 'G')
            {
                skip_until_line_end();
                return lexical_token_t::INVALID_DIRECTIVE;
            }
            char current = *++m_cur_itr;
            if (current != ' ' && current != '\t')
            {
                emit_error("There must be at least one white space between \"%TAG\" and tag info.");
            }

            skip_white_spaces();

            // TODO: parse tag directives' information
            return scan_tag_directive();
        }
        case 'Y': {
            if (*++m_cur_itr != 'A' || *++m_cur_itr != 'M' || *++m_cur_itr != 'L')
            {
                skip_until_line_end();
                return lexical_token_t::INVALID_DIRECTIVE;
            }
            char current = *++m_cur_itr;
            if (current != ' ' && current != '\t')
            {
                emit_error("There must be at least one white space between \"%YAML\" and a version number.");
            }

            skip_white_spaces();

            return scan_yaml_version_directive();
        }
        default:
            skip_until_line_end();
            return lexical_token_t::INVALID_DIRECTIVE;
        }
    }

    lexical_token_t scan_tag_directive()
    {
        m_tag_handle.clear();
        m_tag_prefix.clear();

        //
        // extract a tag handle
        //

        char current = *m_cur_itr;
        if (current != '!')
        {
            emit_error("Tag handle must start with \'!\'.");
        }

        m_tag_handle = "!";

        if (++m_cur_itr == m_end_itr)
        {
            emit_error("invalid TAG directive is found.");
        }

        current = *m_cur_itr;
        switch (current)
        {
        case ' ':
        case '\t':
            // primary handle (!)
            break;
        case '!':
            if (++m_cur_itr == m_end_itr)
            {
                emit_error("invalid TAG directive is found.");
            }
            current = *m_cur_itr;
            if (current != ' ' && current != '\t')
            {
                emit_error("invalid tag handle is found.");
            }
            m_tag_handle.push_back('!');
            break;
        default: {
            bool ends_loop = false;
            do
            {
                switch (current)
                {
                case ' ':
                case '\t':
                    emit_error("invalid tag handle is found.");
                case '!':
                    current = *(m_cur_itr + 1);
                    if (current != ' ' && current != '\t')
                    {
                        emit_error("invalid tag handle is found.");
                    }
                    m_tag_handle.push_back('!');
                    ends_loop = true;
                    break;
                case '-':
                    m_tag_handle.push_back('-');
                    break;
                default:
                    if (!isalnum(current))
                    {
                        // See https://yaml.org/spec/1.2.2/#rule-c-named-tag-handle for more details.
                        emit_error("named handle can contain only numbers(0-9), alphabets(A-Z,a-z) and hyphens(-).");
                    }
                    m_tag_handle.push_back(current);
                    break;
                }

                if (++m_cur_itr == m_end_itr)
                {
                    emit_error("invalid TAG directive is found.");
                }

                current = *m_cur_itr;
            } while (!ends_loop);
            break;
        }
        }

        skip_white_spaces();

        //
        // extract a tag prefix.
        //

        current = *m_cur_itr;
        switch (current)
        {
        case '!':
            // a local tag prefix
            m_tag_prefix = "!";
            current = *++m_cur_itr;
            break;
        // a tag prefix must not start with flow indicators to avoid ambiguity.
        // See https://yaml.org/spec/1.2.2/#rule-ns-global-tag-prefix for more details.
        case ',':
        case '[':
        case ']':
        case '{':
        case '}':
            emit_error("tag prefix must not start with flow indicators (\',\', [], {}).");
        default:
            // a global tag prefix
            break;
        }

        // extract the rest of a tag prefix.
        bool ends_loop = false;
        while (m_cur_itr != m_end_itr && !ends_loop)
        {
            switch (current)
            {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                ends_loop = true;
                break;
            default:
                m_tag_prefix.push_back(current);
                break;
            }

            if (++m_cur_itr != m_end_itr)
            {
                current = *m_cur_itr;
            }
        }

        bool is_valid = uri_encoding::validate(m_tag_prefix.begin(), m_tag_prefix.end());
        if (!is_valid)
        {
            emit_error("invalid URI character is found in a tag prefix.");
        }

        return lexical_token_t::TAG_DIRECTIVE;
    }

    /// @brief Scan a YAML version directive.
    /// @note Only 1.1 and 1.2 are supported. If not, throws an exception.
    /// @return lexical_token_t The lexical token type for YAML version directives.
    lexical_token_t scan_yaml_version_directive()
    {
        m_value_buffer.clear();

        if (std::distance(m_cur_itr, m_end_itr) < 3)
        {
            emit_error("invalid YAML directive is found.");
        }

        if (*m_cur_itr != '1')
        {
            emit_error("Invalid YAML major version found.");
        }
        m_value_buffer.push_back(*m_cur_itr);

        if (*++m_cur_itr != '.')
        {
            emit_error("A period must be followed after the YAML major version.");
        }
        m_value_buffer.push_back(*m_cur_itr);

        switch (*++m_cur_itr)
        {
        case '1':
        case '2':
            m_value_buffer.push_back(*m_cur_itr);
            break;
        case '0':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            emit_error("Unsupported YAML version.");
        default:
            emit_error("YAML version must be specified with digits and periods.");
        }

        if (++m_cur_itr == m_end_itr)
        {
            return lexical_token_t::YAML_VER_DIRECTIVE;
        }

        switch (*m_cur_itr)
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            break;
        default:
            emit_error("Only YAML version 1.1/1.2 are supported.");
        }

        return lexical_token_t::YAML_VER_DIRECTIVE;
    }

    /// @brief Extracts an anchor name from the input and assigns the result to `m_value_buffer`.
    void extract_anchor_name()
    {
        char current = *m_cur_itr++;
        FK_YAML_ASSERT(current == '&' || current == '*');

        m_value_buffer.clear();

        for (; m_cur_itr != m_end_itr; ++m_cur_itr)
        {
            current = *m_cur_itr;
            switch (current)
            {
            // anchor name must not contain white spaces, newline codes and flow indicators.
            // See https://yaml.org/spec/1.2.2/#692-node-anchors for more details.
            case ' ':
            case '\t':
            case '\r':
            case '\n':
            case '{':
            case '}':
            case '[':
            case ']':
            case ',':
                return;
            case ':': {
                auto next_itr = m_cur_itr + 1;
                if (next_itr == m_end_itr)
                {
                    m_value_buffer.push_back(*++m_cur_itr);
                    return;
                }
                switch (*next_itr)
                {
                case ' ':
                case '\t':
                case '\r':
                case '\n':
                    // Stop the extraction at the key separator.
                    return;
                }
                m_value_buffer.push_back(current);
                break;
            }
            default:
                m_value_buffer.push_back(current);
                break;
            }
        }
    }

    /// @brief Extracts a tag name from the input and assigns the result to `m_value_buffer`.
    void extract_tag_name()
    {
        char current = *m_cur_itr;
        FK_YAML_ASSERT(current == '!');

        m_value_buffer = "!";

        if (++m_cur_itr == m_end_itr)
        {
            // Just "!" is a non-specific tag.
            return;
        }

        bool is_verbatim = false;
        bool allows_another_tag_prefix = false;

        current = *m_cur_itr;
        switch (current)
        {
        case ' ':
        case '\r':
        case '\n':
            // Just "!" is a non-specific tag.
            return;
        case '!':
            // Secondary tag handles (!!suffix)
            m_value_buffer += "!";
            break;
        case '<':
            // Verbatim tags (!<TAG>)
            is_verbatim = true;
            m_value_buffer.push_back(current);
            current = *++m_cur_itr;
            m_value_buffer.push_back(current);
            break;
        default:
            // Either local tags (!suffix) or named handles (!tag!suffix)
            allows_another_tag_prefix = true;
            m_value_buffer.push_back(current);
            break;
        }

        bool is_named_handle = false;
        bool ends_loop = false;
        do
        {
            if (++m_cur_itr == m_end_itr)
            {
                break;
            }

            current = *m_cur_itr;
            switch (current)
            {
            // Tag names must not contain spaces or newline codes.
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                ends_loop = true;
                break;
            case '!':
                if (!allows_another_tag_prefix)
                {
                    emit_error("invalid tag prefix (!) is found.");
                }

                is_named_handle = true;
                m_value_buffer.push_back('!');
                // tag prefix must not appear three times.
                allows_another_tag_prefix = false;
                break;
            default:
                m_value_buffer.push_back(current);
                break;
            }
        } while (!ends_loop);

        if (is_verbatim)
        {
            char last = m_value_buffer.back();
            if (last != '>')
            {
                emit_error("verbatim tag (!<TAG>) must be ended with \'>\'.");
            }

            auto tag_begin = m_value_buffer.begin() + 2;
            auto tag_end = m_value_buffer.end() - 1;
            if (tag_begin == tag_end)
            {
                emit_error("verbatim tag(!<TAG>) must not be empty.");
            }

            bool is_valid_uri = uri_encoding::validate(tag_begin, tag_end);
            if (!is_valid_uri)
            {
                emit_error("invalid URI character is found in a verbatim tag.");
            }

            return;
        }

        if (is_named_handle)
        {
            char last = m_value_buffer.back();
            if (last == '!')
            {
                // Tag shorthand must be followed by a non-empty suffix.
                // See the "Tag Shorthands" section in https://yaml.org/spec/1.2.2/#691-node-tags.
                emit_error("named handle has no suffix.");
            }

            std::size_t last_tag_prefix_pos = m_value_buffer.find_last_of('!');
            bool is_valid_uri =
                uri_encoding::validate(m_value_buffer.begin() + last_tag_prefix_pos + 1, m_value_buffer.end());
            if (!is_valid_uri)
            {
                emit_error("Invalid URI character is found in a named tag handle.");
            }
        }
    }

    /// @brief Scan and determine a number type(integer/float). This method is the entrypoint for all number
    /// tokens.
    /// @return lexical_token_t A lexical token type for a determined number type.
    lexical_token_t scan_number()
    {
        m_value_buffer.clear();

        int current = *m_cur_itr;
        FK_YAML_ASSERT(std::isdigit(current) || current == '-' || current == '+');

        lexical_token_t ret = lexical_token_t::END_OF_BUFFER;
        switch (current)
        {
        case '-':
            m_value_buffer.push_back(current);
            ret = scan_negative_number();
            break;
        case '+':
            ret = scan_decimal_number();
            break;
        case '0':
            m_value_buffer.push_back(current);
            ret = scan_number_after_zero_at_first();
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            m_value_buffer.push_back(current);
            ret = scan_decimal_number();
            break;
        default:                                                      // LCOV_EXCL_LINE
            emit_error("Invalid character found in a number token."); // LCOV_EXCL_LINE
        }

        switch (ret)
        {
        case lexical_token_t::INTEGER_VALUE:
            m_integer_val = from_string(m_value_buffer, type_tag<integer_type> {});
            break;
        case lexical_token_t::FLOAT_NUMBER_VALUE:
            m_float_val = from_string(m_value_buffer, type_tag<float_number_type> {});
            break;
        default:
            break;
        }

        return ret;
    }

    /// @brief Scan a next character after the negative sign(-).
    /// @return lexical_token_t The lexical token type for either integer or float numbers.
    lexical_token_t scan_negative_number()
    {
        char next = *++m_cur_itr;

        // The value of `next` must be guranteed to be a digit in the get_next_token() function.
        FK_YAML_ASSERT(std::isdigit(next));
        m_value_buffer.push_back(next);
        return scan_decimal_number();
    }

    /// @brief Scan a next character after '0' at the beginning of a token.
    /// @return lexical_token_t The lexical token type for one of number types(integer/float).
    lexical_token_t scan_number_after_zero_at_first()
    {
        char next = *++m_cur_itr;
        switch (next)
        {
        case '.':
            m_value_buffer.push_back(next);
            return scan_decimal_number_after_decimal_point();
        case 'o':
            // Do not store 'o' since std::stoXXX does not support "0o" but "0" as the prefix for octal numbers.
            // YAML specifies octal values start with the prefix "0o".
            // See https://yaml.org/spec/1.2.2/#1032-tag-resolution for more details.
            return scan_octal_number();
        case 'x':
            m_value_buffer.push_back(next);
            return scan_hexadecimal_number();
        default:
            return lexical_token_t::INTEGER_VALUE;
        }
    }

    /// @brief Scan a next character after a decimal point.
    /// @return lexical_token_t The lexical token type for float numbers.
    lexical_token_t scan_decimal_number_after_decimal_point()
    {
        char next = *++m_cur_itr;

        if (std::isdigit(next))
        {
            m_value_buffer.push_back(next);
            lexical_token_t token = scan_decimal_number();
            return token == lexical_token_t::STRING_VALUE ? token : lexical_token_t::FLOAT_NUMBER_VALUE;
        }

        emit_error("Invalid character found after a decimal point."); // LCOV_EXCL_LINE
    }

    /// @brief Scan a next character after exponent(e/E).
    /// @return lexical_token_t The lexical token type for float numbers.
    lexical_token_t scan_decimal_number_after_exponent()
    {
        char next = *++m_cur_itr;
        if (next == '+' || next == '-')
        {
            m_value_buffer.push_back(next);
            scan_decimal_number_after_sign();
        }
        else if (std::isdigit(next))
        {
            m_value_buffer.push_back(next);
            scan_decimal_number();
        }
        else
        {
            emit_error("unexpected character found after exponent.");
        }
        return lexical_token_t::FLOAT_NUMBER_VALUE;
    }

    /// @brief Scan a next character after a sign(+/-) after exponent(e/E).
    /// @return lexical_token_t The lexical token type for one of number types(integer/float)
    lexical_token_t scan_decimal_number_after_sign()
    {
        char next = *++m_cur_itr;

        if (std::isdigit(next))
        {
            m_value_buffer.push_back(next);
            return scan_decimal_number();
        }

        emit_error("Non-numeric character found after a sign(+/-) after exponent(e/E)."); // LCOV_EXCL_LINE
    }

    /// @brief Scan a next character for decimal numbers.
    /// @return lexical_token_t The lexical token type for one of number types(integer/float)
    lexical_token_t scan_decimal_number()
    {
        char next = *++m_cur_itr;

        if (std::isdigit(next))
        {
            m_value_buffer.push_back(next);
            return scan_decimal_number();
        }

        if (next == '.')
        {
            // NOLINTNEXTLINE(abseil-string-find-str-contains)
            if (m_value_buffer.find('.') != string_type::npos)
            {
                // This path is for strings like 1.2.3
                return scan_string(false);
            }
            m_value_buffer.push_back(next);
            return scan_decimal_number_after_decimal_point();
        }

        if (next == 'e' || next == 'E')
        {
            m_value_buffer.push_back(next);
            return scan_decimal_number_after_exponent();
        }

        return lexical_token_t::INTEGER_VALUE;
    }

    /// @brief Scan a next character for octal numbers.
    /// @return lexical_token_t The lexical token type for integers.
    lexical_token_t scan_octal_number()
    {
        char next = *++m_cur_itr;
        if ('0' <= next && next <= '7')
        {
            m_value_buffer.push_back(next);
            scan_octal_number();
        }
        return lexical_token_t::INTEGER_VALUE;
    }

    /// @brief Scan a next character for hexadecimal numbers.
    /// @return lexical_token_t The lexical token type for integers.
    lexical_token_t scan_hexadecimal_number()
    {
        char next = *++m_cur_itr;
        if (std::isxdigit(next))
        {
            m_value_buffer.push_back(next);
            scan_hexadecimal_number();
        }
        return lexical_token_t::INTEGER_VALUE;
    }

    /// @brief Scan a string token(unquoted/single-quoted/double-quoted).
    /// @return lexical_token_t The lexical token type for strings.
    lexical_token_t scan_string(bool needs_clear = true)
    {
        bool needs_last_single_quote = false;
        bool needs_last_double_quote = false;

        if (needs_clear)
        {
            m_value_buffer.clear();

            needs_last_single_quote = (*m_cur_itr == '\'');
            needs_last_double_quote = (*m_cur_itr == '\"');
            if (needs_last_double_quote || needs_last_single_quote)
            {
                ++m_cur_itr;
            }
        }

        lexical_token_t type = extract_string_token(needs_last_single_quote, needs_last_double_quote);
        FK_YAML_ASSERT(type == lexical_token_t::STRING_VALUE);

        if (needs_last_single_quote || needs_last_double_quote)
        {
            // just returned the extracted string value if quoted.
            return type;
        }

        if (m_value_buffer == "~")
        {
            return lexical_token_t::NULL_VALUE;
        }

        size_t val_size = m_value_buffer.size();
        if (val_size == 4)
        {
            if (m_value_buffer == "null" || m_value_buffer == "Null" || m_value_buffer == "NULL")
            {
                from_string(m_value_buffer, type_tag<std::nullptr_t> {});
                return lexical_token_t::NULL_VALUE;
            }

            if (m_value_buffer == "true" || m_value_buffer == "True" || m_value_buffer == "TRUE")
            {
                m_boolean_val = from_string(m_value_buffer, type_tag<boolean_type> {});
                return lexical_token_t::BOOLEAN_VALUE;
            }

            if (m_value_buffer == ".inf" || m_value_buffer == ".Inf" || m_value_buffer == ".INF" ||
                m_value_buffer == ".nan" || m_value_buffer == ".NaN" || m_value_buffer == ".NAN")
            {
                m_float_val = from_string(m_value_buffer, type_tag<float_number_type> {});
                return lexical_token_t::FLOAT_NUMBER_VALUE;
            }
        }
        else if (val_size == 5)
        {
            if (m_value_buffer == "false" || m_value_buffer == "False" || m_value_buffer == "FALSE")
            {
                m_boolean_val = from_string(m_value_buffer, type_tag<boolean_type> {});
                return lexical_token_t::BOOLEAN_VALUE;
            }

            if (m_value_buffer == "-.inf" || m_value_buffer == "-.Inf" || m_value_buffer == "-.INF")
            {
                m_float_val = from_string(m_value_buffer, type_tag<float_number_type> {});
                return lexical_token_t::FLOAT_NUMBER_VALUE;
            }
        }

        return type;
    }

    /// @brief Scan a string token(unquoted/single-quoted/double-quoted).
    /// @return lexical_token_t The lexical token type for strings.
    lexical_token_t extract_string_token(bool needs_last_single_quote, bool needs_last_double_quote)
    {
        for (; m_cur_itr != m_end_itr; ++m_cur_itr)
        {
            char current = *m_cur_itr;

            if (current == ' ')
            {
                if (!needs_last_double_quote && !needs_last_single_quote)
                {
                    // Allow a space in an unquoted string only if the space is surrounded by non-space characters.
                    // See https://yaml.org/spec/1.2.2/#733-plain-style for more details.
                    char next = *(m_cur_itr + 1);

                    // These characters are permitted when not inside a flow collection, and not inside an implicit key.
                    // TODO: Support detection of implicit key context for this check.
                    if (m_flow_context_depth > 0)
                    {
                        switch (next)
                        {
                        case '{':
                        case '}':
                        case '[':
                        case ']':
                        case ',':
                            ++m_cur_itr;
                            return lexical_token_t::STRING_VALUE;
                        }
                    }

                    // " :" is permitted in a plain style string token, but not when followed by a space.
                    if (next == ':')
                    {
                        char peeked = *(m_cur_itr + 2);
                        if (peeked == ' ')
                        {
                            ++m_cur_itr;
                            return lexical_token_t::STRING_VALUE;
                        }
                    }

                    switch (next)
                    {
                    case ' ':
                    case '\r':
                    case '\n':
                    case '#':
                    case '\\':
                        ++m_cur_itr;
                        return lexical_token_t::STRING_VALUE;
                    }
                }
                m_value_buffer.push_back(current);
                continue;
            }

            if (current == '\"')
            {
                if (needs_last_double_quote)
                {
                    ++m_cur_itr;
                    return lexical_token_t::STRING_VALUE;
                }

                // if the target is a plain/single-quoted string token.
                m_value_buffer.push_back(current);
                continue;
            }

            // Handle single quotation marks.
            if (current == '\'')
            {
                if (needs_last_single_quote)
                {
                    // If single quotation marks are repeated twice in a single-quoted string token, they are considered
                    // as an escaped single quotation mark.
                    current = *++m_cur_itr;
                    if (current != '\'')
                    {
                        return lexical_token_t::STRING_VALUE;
                    }
                }

                m_value_buffer.push_back(current);
                continue;
            }

            // Handle colons.
            if (current == ':')
            {
                // Just regard a colon as a character if surrounded by quotation marks.
                if (needs_last_double_quote || needs_last_single_quote)
                {
                    m_value_buffer.push_back(current);
                    continue;
                }

                char next = *(m_cur_itr + 1);

                // A colon as a key separator must be followed by a space or a newline code.
                if (next != ' ' && next != '\r' && next != '\n')
                {
                    m_value_buffer.push_back(current);
                    continue;
                }

                return lexical_token_t::STRING_VALUE;
            }

            // Handle flow indicators.
            {
                bool flow_indicator_appended = false;
                switch (current)
                {
                case '{':
                case '}':
                case '[':
                case ']':
                case ',':
                    // just regard the flow indicators as a normal character:
                    // - if single or double quotated,
                    // - or if not inside flow context.

                    if (needs_last_single_quote || needs_last_double_quote)
                    {
                        m_value_buffer.push_back(current);
                    }
                    else if (m_flow_context_depth == 0)
                    {
                        m_value_buffer.push_back(current);
                    }
                    else
                    {
                        return lexical_token_t::STRING_VALUE;
                    }

                    flow_indicator_appended = true;
                    break;
                default:
                    break;
                }

                if (flow_indicator_appended)
                {
                    continue;
                }
            }

            // Handle newline codes.
            if (current == '\r' || current == '\n')
            {
                if (!(needs_last_double_quote || needs_last_single_quote))
                {
                    return lexical_token_t::STRING_VALUE;
                }

                // TODO: Support multi-line string tokens.
                emit_error("multi-line string tokens are unsupported.");
            }

            // Handle escaped characters.
            // See https://yaml.org/spec/1.2.2/#57-escaped-characters for more details.
            if (current == '\\')
            {
                if (!needs_last_double_quote)
                {
                    emit_error("Escaped characters are only available in a double-quoted string token.");
                }

                current = *++m_cur_itr;
                switch (current)
                {
                case 'a':
                    m_value_buffer.push_back('\a');
                    break;
                case 'b':
                    m_value_buffer.push_back('\b');
                    break;
                case 't':
                    m_value_buffer.push_back('\t');
                    break;
                case 'n':
                    m_value_buffer.push_back('\n');
                    break;
                case 'v':
                    m_value_buffer.push_back('\v');
                    break;
                case 'f':
                    m_value_buffer.push_back('\f');
                    break;
                case 'r':
                    m_value_buffer.push_back('\r');
                    break;
                case 'e':
                    m_value_buffer.push_back(char(0x1B));
                    break;
                case ' ':
                    m_value_buffer.push_back(' ');
                    break;
                case '\"':
                    m_value_buffer.push_back('\"');
                    break;
                case '/':
                    m_value_buffer.push_back('/');
                    break;
                case '\\':
                    m_value_buffer.push_back('\\');
                    break;
                case 'N': // next line
                    utf8_encoding::from_utf32(0x85u, m_encode_buffer, m_encoded_size);
                    m_value_buffer.append(m_encode_buffer.data(), m_encoded_size);
                    break;
                case '_': // non-breaking space
                    utf8_encoding::from_utf32(0xA0u, m_encode_buffer, m_encoded_size);
                    m_value_buffer.append(m_encode_buffer.data(), m_encoded_size);
                    break;
                case 'L': // line separator
                    utf8_encoding::from_utf32(0x2028u, m_encode_buffer, m_encoded_size);
                    m_value_buffer.append(m_encode_buffer.data(), m_encoded_size);
                    break;
                case 'P': // paragraph separator
                    utf8_encoding::from_utf32(0x2029u, m_encode_buffer, m_encoded_size);
                    m_value_buffer.append(m_encode_buffer.data(), m_encoded_size);
                    break;
                case 'x':
                    handle_escaped_unicode(1);
                    break;
                case 'u':
                    handle_escaped_unicode(2);
                    break;
                case 'U':
                    handle_escaped_unicode(4);
                    break;
                default:
                    emit_error("Unsupported escape sequence is found in a string token.");
                }
                continue;
            }

            uint8_t byte = static_cast<uint8_t>(current);

            // Handle unescaped control characters.
            if (byte <= 0x1Fu)
            {
                handle_unescaped_control_char(current);
                continue;
            }

            // The other characters are already checked while creating an input handler.

            // Handle ASCII characters except control characters.
            else if (byte <= 0x7Eu)
            {
                m_value_buffer.push_back(current);
                continue;
            }

            // Handle 2-byte characters encoded in UTF-8. (U+0080..U+07FF)
            else if (byte <= 0xDFu)
            {
                m_value_buffer.push_back(current);
                m_value_buffer.push_back(*++m_cur_itr);
                continue;
            }

            // Handle 3-byte characters encoded in UTF-8. (U+1000..U+D7FF,U+E000..U+FFFF)
            else if (byte <= 0xEFu)
            {
                m_value_buffer.push_back(current);
                m_value_buffer.push_back(*++m_cur_itr);
                m_value_buffer.push_back(*++m_cur_itr);

                continue;
            }

            // Handle 4-byte characters encoded in UTF-8. (U+10000..U+FFFFF,U+100000..U+10FFFF)
            m_value_buffer.push_back(current);
            m_value_buffer.push_back(*++m_cur_itr);
            m_value_buffer.push_back(*++m_cur_itr);
            m_value_buffer.push_back(*++m_cur_itr);
        }

        // Handle the end of input buffer.
        if (needs_last_double_quote)
        {
            emit_error("Invalid end of input buffer in a double-quoted string token.");
        }

        if (needs_last_single_quote)
        {
            emit_error("Invalid end of input buffer in a single-quoted string token.");
        }

        return lexical_token_t::STRING_VALUE;
    }

    lexical_token_t scan_block_style_string_token(
        block_style_indicator_t style, chomping_indicator_t chomp, std::size_t indent)
    {
        m_value_buffer.clear();

        // Handle leading all-space lines.
        for (char current = 0; m_cur_itr != m_end_itr; ++m_cur_itr)
        {
            current = *m_cur_itr;

            if (current == ' ')
            {
                continue;
            }

            if (current == '\r')
            {
                current = *++m_cur_itr;
            }
            if (current == '\n')
            {
                m_value_buffer.push_back('\n');
                continue;
            }

            break;
        }

        if (m_cur_itr == m_end_itr)
        {
            if (chomp != chomping_indicator_t::KEEP)
            {
                m_value_buffer.clear();
            }
            return lexical_token_t::STRING_VALUE;
        }

        m_pos_tracker.update_position(m_cur_itr);
        std::size_t cur_indent = m_pos_tracker.get_cur_pos_in_line();
        if (indent > 0 && cur_indent < indent)
        {
            emit_error("A block style scalar is less indented than the indicated level.");
        }

        // TODO: preserve and compare the last indentation with `cur_indent`
        if (indent == 0)
        {
            indent = cur_indent;
        }

        int chars_in_line = 0;
        bool is_extra_indented = false;
        if (cur_indent > indent)
        {
            std::size_t diff = cur_indent - indent;
            if (style == block_style_indicator_t::FOLDED)
            {
                m_value_buffer.push_back('\n');
                is_extra_indented = true;
            }
            while (diff > 0)
            {
                m_value_buffer.push_back(' ');
                --diff;
                ++chars_in_line;
            }
        }

        for (char current = 0; m_cur_itr != m_end_itr; ++m_cur_itr)
        {
            current = *m_cur_itr;

            if (current == '\r')
            {
                // Ignore CR assuming the next character is LF.
                continue;
            }

            if (current == '\n')
            {
                if (style == block_style_indicator_t::LITERAL)
                {
                    m_value_buffer.push_back(current);
                }
                else // block_style_indicator_t::FOLDED
                {
                    if (chars_in_line == 0)
                    {
                        // Just append a newline if the current line is empty.
                        m_value_buffer.push_back('\n');
                        is_extra_indented = false;
                        continue;
                    }

                    if (is_extra_indented)
                    {
                        // A line being more indented is not folded.
                        m_value_buffer.push_back('\n');
                        chars_in_line = 0;
                        is_extra_indented = false;
                        continue;
                    }

                    // Append a newline if the next line is an empty line.
                    bool is_end_of_token = false;
                    bool is_next_empty = false;
                    for (std::size_t i = 0; i < indent; i++)
                    {
                        if (++m_cur_itr == m_end_itr)
                        {
                            is_end_of_token = true;
                            break;
                        }

                        current = *m_cur_itr;
                        if (current == ' ')
                        {
                            continue;
                        }

                        if (current == '\r')
                        {
                            current = *++m_cur_itr;
                        }
                        if (current == '\n')
                        {
                            is_next_empty = true;
                            break;
                        }

                        is_end_of_token = true;
                        break;
                    }

                    if (is_end_of_token)
                    {
                        m_value_buffer.push_back('\n');
                        break;
                    }

                    if (is_next_empty)
                    {
                        m_value_buffer.push_back('\n');
                    }
                    else
                    {
                        switch (char next = *(m_cur_itr + 1))
                        {
                        case '\r': {
                            ++m_cur_itr;
                            next = *++m_cur_itr;
                            FK_YAML_ASSERT(next == '\n');
                            m_value_buffer.push_back(next);
                            break;
                        }
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
                }

                // Reset the values for the next line.
                chars_in_line = 0;
                is_extra_indented = false;

                continue;
            }

            // Handle indentation
            m_pos_tracker.update_position(m_cur_itr);
            cur_indent = m_pos_tracker.get_cur_pos_in_line();
            if (cur_indent < indent)
            {
                if (current != ' ')
                {
                    // Interpret less indented non-space characters as the start of the next token.
                    break;
                }
                // skip a space if not yet indented enough
                continue;
            }

            if (style == block_style_indicator_t::FOLDED && chars_in_line == 0 && current == ' ')
            {
                // A line being more indented is not folded.
                m_value_buffer.push_back('\n');
                is_extra_indented = true;
            }
            m_value_buffer.push_back(current);
            ++chars_in_line;
        }

        // Manipulate the trailing line endings chomping indicator type.
        switch (chomp)
        {
        case chomping_indicator_t::STRIP:
            while (true)
            {
                // Empty strings are handled above, so no check for the case.
                auto last_char = m_value_buffer.back();
                if (last_char != '\n')
                {
                    break;
                }
                m_value_buffer.pop_back();
            }
            break;
        case chomping_indicator_t::CLIP: {
            auto last_char = m_value_buffer.back();
            if (last_char != '\n')
            {
                // No need to chomp the trailing newlines.
                break;
            }
            while (true)
            {
                // Strings with only newlines are handled above, so no check for the case.
                auto second_last_char = m_value_buffer[m_value_buffer.size() - 2];
                if (second_last_char != '\n')
                {
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
    void handle_unescaped_control_char(int c)
    {
        FK_YAML_ASSERT(0x00 <= c && c <= 0x1F);

        switch (c)
        {
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
            m_value_buffer.push_back(c);
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

    void handle_escaped_unicode(int bytes_to_read)
    {
        int read_size = bytes_to_read * 2;
        char32_t code_point = 0;
        for (int i = read_size - 1; i >= 0; i--)
        {
            char four_bits = convert_hex_char_to_byte(*++m_cur_itr);
            // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
            code_point |= static_cast<char32_t>(four_bits << (4 * i));
        }

        // Treats the code point as a UTF-32 encoded character.
        utf8_encoding::from_utf32(code_point, m_encode_buffer, m_encoded_size);
        m_value_buffer.append(m_encode_buffer.data(), m_encoded_size);
    }

    void get_block_style_metadata(chomping_indicator_t& chomp_type, std::size_t& indent)
    {
        int ch = *++m_cur_itr;

        chomp_type = chomping_indicator_t::CLIP;
        switch (ch)
        {
        case '-':
            chomp_type = chomping_indicator_t::STRIP;
            ch = *++m_cur_itr;
            break;
        case '+':
            chomp_type = chomping_indicator_t::KEEP;
            ch = *++m_cur_itr;
            break;
        default:
            break;
        }

        if (ch == '0')
        {
            emit_error("An indentation level for a block style scalar cannot be \'0\'");
        }

        indent = 0;
        if (std::isdigit(ch))
        {
            indent = convert_hex_char_to_byte(ch);
            ch = *++m_cur_itr;
        }

        // skip characters including comments.
        skip_until_line_end();
    }

    /// @brief Skip white spaces (half-width spaces and tabs) from the current position.
    void skip_white_spaces()
    {
        do
        {
            switch (*m_cur_itr)
            {
            case ' ':
            case '\t':
                break;
            default:
                return;
            }
        } while (++m_cur_itr != m_end_itr);
    }

    /// @brief Skip white spaces and newline codes (CR/LF) from the current position.
    void skip_white_spaces_and_newline_codes()
    {
        do
        {
            skip_white_spaces();

            if (m_cur_itr == m_end_itr)
            {
                return;
            }

            switch (*m_cur_itr)
            {
            case '\n':
            case '\r':
                break;
            default:
                return;
            }
        } while (++m_cur_itr != m_end_itr);
    }

    /// @brief Skip white spaces and comments from the current position.
    void skip_white_spaces_and_comments()
    {
        do
        {
            skip_white_spaces();

            if (m_cur_itr == m_end_itr)
            {
                return;
            }

            switch (*m_cur_itr)
            {
            case '#': {
                scan_comment();
                break;
            }
            default:
                return;
            }
        } while (++m_cur_itr != m_end_itr);
    }

    /// @brief Skip the rest in the current line.
    void skip_until_line_end()
    {
        do
        {
            switch (*m_cur_itr)
            {
            case '\r':
                if (*++m_cur_itr == '\n')
                {
                    ++m_cur_itr;
                }
                return;
            case '\n':
                ++m_cur_itr;
                return;
            default:
                break;
            }
        } while (++m_cur_itr != m_end_itr);
    }

    [[noreturn]] void emit_error(const char* msg) const
    {
        m_pos_tracker.update_position(m_cur_itr);
        throw fkyaml::parse_error(msg, m_pos_tracker.get_lines_read(), m_pos_tracker.get_cur_pos_in_line());
    }

private:
    /// The value of EOF for the target characters.
    static constexpr char s_end_of_input = char(0xFF);

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
    /// A temporal buffer to store a UTF-8 encoded char sequence.
    std::array<char, 4> m_encode_buffer {};
    /// The actual size of a UTF-8 encoded char sequence.
    std::size_t m_encoded_size {0};
    /// The beginning position of the last lexical token. (zero origin)
    std::size_t m_last_token_begin_pos {0};
    /// The beginning line of the last lexical token. (zero origin)
    std::size_t m_last_token_begin_line {0};
    /// The current depth of flow context.
    uint32_t m_flow_context_depth {0};
    /// The last found token type.
    lexical_token_t m_last_token_type {lexical_token_t::END_OF_BUFFER};
    /// A temporal bool holder.
    boolean_type m_boolean_val {false};
    /// A temporal integer holder.
    integer_type m_integer_val {0};
    /// A temporal floating point number holder.
    float_number_type m_float_val {0.0};
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_LEXICAL_ANALIZER_HPP_ */
