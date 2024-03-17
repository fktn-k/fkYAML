///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
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
#include <fkYAML/detail/encodings/utf8_encoding.hpp>
#include <fkYAML/detail/input/input_handler.hpp>
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
template <
    typename BasicNodeType, typename InputAdapterType,
    enable_if_t<conjunction<is_basic_node<BasicNodeType>, is_input_adapter<InputAdapterType>>::value, int> = 0>
class lexical_analyzer
{
private:
    using input_handler_type = input_handler<InputAdapterType>;
    using char_traits_type = typename input_handler_type::char_traits_type;
    using char_type = typename char_traits_type::char_type;
    using char_int_type = typename char_traits_type::int_type;
    using input_string_type = typename input_handler_type::string_type;

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
    /// @param input_adapter An input adapter object.
    explicit lexical_analyzer(InputAdapterType&& input_adapter)
        : m_input_handler(std::move(input_adapter))
    {
    }

    /// @brief Get the next lexical token type by scanning the left of the input buffer.
    /// @return lexical_token_t The next lexical token type.
    lexical_token_t get_next_token()
    {
        skip_white_spaces_and_newline_codes();

        char_int_type current = m_input_handler.get_current();
        m_last_token_begin_pos = m_input_handler.get_cur_pos_in_line();

        if (0x00 <= current && current <= 0x7F && isdigit(current))
        {
            return m_last_token_type = scan_number();
        }

        switch (current)
        {
        case end_of_input: // end of input buffer
            return m_last_token_type = lexical_token_t::END_OF_BUFFER;
        case '?':
            switch (m_input_handler.get_next())
            {
            case ' ':
                return m_last_token_type = lexical_token_t::EXPLICIT_KEY_PREFIX;
            default:
                m_value_buffer = "?";
                return m_last_token_type = scan_string(false);
            }
        case ':': // key separater
            switch (m_input_handler.get_next())
            {
            case ' ': {
                size_t prev_pos = m_input_handler.get_lines_read();
                skip_white_spaces_and_comments();
                size_t cur_pos = m_input_handler.get_lines_read();
                if (prev_pos == cur_pos)
                {
                    current = m_input_handler.get_current();
                    if (current != '\r' && current != '\n')
                    {
                        return m_last_token_type = lexical_token_t::KEY_SEPARATOR;
                    }
                }
                return m_last_token_type = lexical_token_t::MAPPING_BLOCK_PREFIX;
            }
            case '\r': {
                char_int_type next = m_input_handler.get_next();
                if (next == '\n')
                {
                    m_input_handler.get_next();
                }
                return m_last_token_type = lexical_token_t::MAPPING_BLOCK_PREFIX;
            }
            case '\n':
                m_input_handler.get_next();
                return m_last_token_type = lexical_token_t::MAPPING_BLOCK_PREFIX;
            default:
                emit_error("Half-width spaces or newline codes are required after a key separater(:).");
            }
        case ',': // value separater
            m_input_handler.get_next();
            return m_last_token_type = lexical_token_t::VALUE_SEPARATOR;
        case '&': { // anchor prefix
            m_value_buffer.clear();
            while (true)
            {
                char_int_type next = m_input_handler.get_next();
                if (next == end_of_input || next == '\r' || next == '\n')
                {
                    emit_error("An anchor label must be followed by some value.");
                }
                if (next == ' ')
                {
                    m_input_handler.get_next();
                    break;
                }
                m_value_buffer.push_back(char_traits_type::to_char_type(next));
            }
            return m_last_token_type = lexical_token_t::ANCHOR_PREFIX;
        }
        case '*': { // alias prefix
            m_value_buffer.clear();
            while (true)
            {
                char_int_type next = m_input_handler.get_next();
                if (next == ' ' || next == '\r' || next == '\n' || next == end_of_input)
                {
                    if (m_value_buffer.empty())
                    {
                        emit_error("An alias prefix must be followed by some anchor name.");
                    }
                    m_input_handler.get_next();
                    break;
                }
                m_value_buffer.push_back(char_traits_type::to_char_type(next));
            }
            return m_last_token_type = lexical_token_t::ALIAS_PREFIX;
        }
        case '#': // comment prefix
            scan_comment();
            return m_last_token_type = lexical_token_t::COMMENT_PREFIX;
        case '%': // directive prefix
            return m_last_token_type = scan_directive();
        case '-': {
            char_int_type next = m_input_handler.get_next();
            if (next == ' ')
            {
                // Move a cursor to the beginning of the next token.
                m_input_handler.get_next();
                return m_last_token_type = lexical_token_t::SEQUENCE_BLOCK_PREFIX;
            }

            m_input_handler.unget();
            if (std::isdigit(next))
            {
                return m_last_token_type = scan_number();
            }

            char_int_type ret = m_input_handler.get_range(3, m_value_buffer);
            if (ret != end_of_input)
            {
                if (m_value_buffer == "---")
                {
                    m_input_handler.get_next();
                    return m_last_token_type = lexical_token_t::END_OF_DIRECTIVES;
                }

                m_input_handler.unget_range(2);
            }

            return m_last_token_type = scan_string();
        }
        case '[': // sequence flow begin
            m_flow_context_depth++;
            m_input_handler.get_next();
            return m_last_token_type = lexical_token_t::SEQUENCE_FLOW_BEGIN;
        case ']': // sequence flow end
            if (m_flow_context_depth == 0)
            {
                emit_error("An invalid flow sequence ending.");
            }
            m_flow_context_depth--;
            m_input_handler.get_next();
            return m_last_token_type = lexical_token_t::SEQUENCE_FLOW_END;
        case '{': // mapping flow begin
            m_flow_context_depth++;
            m_input_handler.get_next();
            return m_last_token_type = lexical_token_t::MAPPING_FLOW_BEGIN;
        case '}': // mapping flow end
            if (m_flow_context_depth == 0)
            {
                emit_error("An invalid flow mapping ending.");
            }
            m_flow_context_depth--;
            m_input_handler.get_next();
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
            char_int_type ret = m_input_handler.get_range(3, m_value_buffer);
            if (ret != end_of_input)
            {
                if (m_value_buffer == "...")
                {
                    m_input_handler.get_next();
                    return m_last_token_type = lexical_token_t::END_OF_DOCUMENT;
                }

                // revert change in the position to the one before comparison above.
                m_input_handler.unget_range(2);
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
        return m_input_handler.get_lines_read();
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
        static_assert(std::is_same<string_type, input_string_type>::value, "Unsupported, different string types.");
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

private:
    /// @brief A utility function to convert a hexadecimal character to an integer.
    /// @param source A hexadecimal character ('0'~'9', 'A'~'F', 'a'~'f')
    /// @return char A integer converted from @a source.
    char convert_hex_char_to_byte(char_int_type source) const
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
        FK_YAML_ASSERT(m_input_handler.get_current() == '#');

        skip_until_line_end();
        return lexical_token_t::COMMENT_PREFIX;
    }

    /// @brief Scan directives starting with the prefix '%'
    /// @note Currently, only %YAML directive is supported. If not, returns invalid or throws an exception.
    /// @return lexical_token_t The lexical token type for directives.
    lexical_token_t scan_directive()
    {
        FK_YAML_ASSERT(m_input_handler.get_current() == '%');

        switch (m_input_handler.get_next())
        {
        case end_of_input:
            emit_error("invalid eof in a directive.");
        case 'T': {
            if (m_input_handler.get_next() != 'A' || m_input_handler.get_next() != 'G')
            {
                skip_until_line_end();
                return lexical_token_t::INVALID_DIRECTIVE;
            }
            if (m_input_handler.get_next() != ' ')
            {
                emit_error("There must be a half-width space between \"%TAG\" and tag info.");
            }
            // TODO: parse tag directives' information
            return lexical_token_t::TAG_DIRECTIVE;
        }
        case 'Y':
            if (m_input_handler.get_next() != 'A' || m_input_handler.get_next() != 'M' ||
                m_input_handler.get_next() != 'L')
            {
                skip_until_line_end();
                return lexical_token_t::INVALID_DIRECTIVE;
            }
            if (m_input_handler.get_next() != ' ')
            {
                emit_error("There must be a half-width space between \"%YAML\" and a version number.");
            }
            return scan_yaml_version_directive();
        default:
            skip_until_line_end();
            return lexical_token_t::INVALID_DIRECTIVE;
        }
    }

    /// @brief Scan a YAML version directive.
    /// @note Only 1.1 and 1.2 are supported. If not, throws an exception.
    /// @return lexical_token_t The lexical token type for YAML version directives.
    lexical_token_t scan_yaml_version_directive()
    {
        m_value_buffer.clear();

        if (m_input_handler.get_next() != '1')
        {
            emit_error("Invalid YAML major version found.");
        }
        m_value_buffer.push_back(char_traits_type::to_char_type(m_input_handler.get_current()));

        if (m_input_handler.get_next() != '.')
        {
            emit_error("A period must be followed after the YAML major version.");
        }
        m_value_buffer.push_back(char_traits_type::to_char_type(m_input_handler.get_current()));

        switch (m_input_handler.get_next())
        {
        case '1':
        case '2':
            m_value_buffer.push_back(char_traits_type::to_char_type(m_input_handler.get_current()));
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

        if (m_input_handler.get_next() != ' ' && m_input_handler.get_current() != '\r' &&
            m_input_handler.get_current() != '\n')
        {
            emit_error("Only YAML version 1.1/1.2 are supported.");
        }

        return lexical_token_t::YAML_VER_DIRECTIVE;
    }

    /// @brief Scan and determine a number type(integer/float). This method is the entrypoint for all number
    /// tokens.
    /// @return lexical_token_t A lexical token type for a determined number type.
    lexical_token_t scan_number()
    {
        m_value_buffer.clear();

        char_int_type current = m_input_handler.get_current();
        FK_YAML_ASSERT(std::isdigit(current) || current == '-' || current == '+');

        lexical_token_t ret = lexical_token_t::END_OF_BUFFER;
        switch (current)
        {
        case '-':
            m_value_buffer.push_back(char_traits_type::to_char_type(current));
            ret = scan_negative_number();
            break;
        case '+':
            ret = scan_decimal_number();
            break;
        case '0':
            m_value_buffer.push_back(char_traits_type::to_char_type(current));
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
            m_value_buffer.push_back(char_traits_type::to_char_type(current));
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
        default:   // LCOV_EXCL_LINE
            break; // LCOV_EXCL_LINE
        }

        return ret;
    }

    /// @brief Scan a next character after the negative sign(-).
    /// @return lexical_token_t The lexical token type for either integer or float numbers.
    lexical_token_t scan_negative_number()
    {
        char_int_type next = m_input_handler.get_next();

        // The value of `next` must be guranteed to be a digit in the get_next_token() function.
        FK_YAML_ASSERT(std::isdigit(next));
        m_value_buffer.push_back(char_traits_type::to_char_type(next));
        return scan_decimal_number();
    }

    /// @brief Scan a next character after '0' at the beginning of a token.
    /// @return lexical_token_t The lexical token type for one of number types(integer/float).
    lexical_token_t scan_number_after_zero_at_first()
    {
        char_int_type next = m_input_handler.get_next();
        switch (next)
        {
        case '.':
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            return scan_decimal_number_after_decimal_point();
        case 'o':
            // Do not store 'o' since std::strtoull does not support "0o" but "0" as the prefix for octal numbers.
            // YAML specifies octal values start with the prefix "0o".
            // See "10.3.2 Tag Resolution" section in https://yaml.org/spec/1.2.2/
            return scan_octal_number();
        case 'x':
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            return scan_hexadecimal_number();
        default:
            return lexical_token_t::INTEGER_VALUE;
        }
    }

    /// @brief Scan a next character after a decimal point.
    /// @return lexical_token_t The lexical token type for float numbers.
    lexical_token_t scan_decimal_number_after_decimal_point()
    {
        char_int_type next = m_input_handler.get_next();

        if (std::isdigit(next))
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            lexical_token_t token = scan_decimal_number();
            return token == lexical_token_t::STRING_VALUE ? token : lexical_token_t::FLOAT_NUMBER_VALUE;
        }

        emit_error("Invalid character found after a decimal point."); // LCOV_EXCL_LINE
    }

    /// @brief Scan a next character after exponent(e/E).
    /// @return lexical_token_t The lexical token type for float numbers.
    lexical_token_t scan_decimal_number_after_exponent()
    {
        char_int_type next = m_input_handler.get_next();
        if (next == '+' || next == '-')
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            scan_decimal_number_after_sign();
        }
        else if (std::isdigit(next))
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
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
        char_int_type next = m_input_handler.get_next();

        if (std::isdigit(next))
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            return scan_decimal_number();
        }

        emit_error("Non-numeric character found after a sign(+/-) after exponent(e/E)."); // LCOV_EXCL_LINE
    }

    /// @brief Scan a next character for decimal numbers.
    /// @return lexical_token_t The lexical token type for one of number types(integer/float)
    lexical_token_t scan_decimal_number()
    {
        char_int_type next = m_input_handler.get_next();

        if (std::isdigit(next))
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
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
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            return scan_decimal_number_after_decimal_point();
        }

        if (next == 'e' || next == 'E')
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            return scan_decimal_number_after_exponent();
        }

        return lexical_token_t::INTEGER_VALUE;
    }

    /// @brief Scan a next character for octal numbers.
    /// @return lexical_token_t The lexical token type for integers.
    lexical_token_t scan_octal_number()
    {
        char_int_type next = m_input_handler.get_next();
        if ('0' <= next && next <= '7')
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            scan_octal_number();
        }
        return lexical_token_t::INTEGER_VALUE;
    }

    /// @brief Scan a next character for hexadecimal numbers.
    /// @return lexical_token_t The lexical token type for integers.
    lexical_token_t scan_hexadecimal_number()
    {
        char_int_type next = m_input_handler.get_next();
        if (std::isxdigit(next))
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
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

            needs_last_single_quote = (m_input_handler.get_current() == '\'');
            needs_last_double_quote = (m_input_handler.get_current() == '\"');
            if (needs_last_double_quote || needs_last_single_quote)
            {
                m_input_handler.get_next();
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
        char_int_type current = m_input_handler.get_current();

        for (;; current = m_input_handler.get_next())
        {
            // Handle the end of input buffer.
            if (current == end_of_input)
            {
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

            if (current == ' ')
            {
                if (!needs_last_double_quote && !needs_last_single_quote)
                {
                    // Allow a space in an unquoted string only if the space is surrounded by non-space characters.
                    // See "7.3.3 Plain Style" section in https://yaml.org/spec/1.2.2/
                    current = m_input_handler.get_next();
                    switch (current)
                    {
                    case ' ':
                    case '\r':
                    case '\n':
                    case '{':
                    case '}':
                    case '[':
                    case ']':
                    case ',':
                    case ':':
                    case '#':
                    case '\\':
                        return lexical_token_t::STRING_VALUE;
                    }
                    m_input_handler.unget();
                    current = m_input_handler.get_current();
                }
                m_value_buffer.push_back(char_traits_type::to_char_type(current));
                continue;
            }

            if (current == '\"')
            {
                if (needs_last_double_quote)
                {
                    m_input_handler.get_next();
                    return lexical_token_t::STRING_VALUE;
                }

                // if the target is a plain/single-quoted string token.
                m_value_buffer.push_back(char_traits_type::to_char_type(current));
                continue;
            }

            // Handle single quotation marks.
            if (current == '\'')
            {
                if (needs_last_single_quote)
                {
                    // If single quotation marks are repeated twice in a single-quoted string token, they are considered
                    // as an escaped single quotation mark.
                    current = m_input_handler.get_next();
                    if (current != '\'')
                    {
                        return lexical_token_t::STRING_VALUE;
                    }
                }

                m_value_buffer.push_back(char_traits_type::to_char_type(current));
                continue;
            }

            // Handle colons.
            if (current == ':')
            {
                // Just regard a colon as a character if surrounded by quotation marks.
                if (needs_last_double_quote || needs_last_single_quote)
                {
                    m_value_buffer.push_back(char_traits_type::to_char_type(current));
                    continue;
                }

                char_int_type next = m_input_handler.get_next();
                m_input_handler.unget();

                // A colon as a key separator must be followed by a space or a newline code.
                if (next != ' ' && next != '\r' && next != '\n')
                {
                    m_value_buffer.push_back(char_traits_type::to_char_type(current));
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
                        m_value_buffer.push_back(char_traits_type::to_char_type(current));
                    }
                    else if (m_flow_context_depth == 0)
                    {
                        m_value_buffer.push_back(char_traits_type::to_char_type(current));
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
            // See "5.7 Escaped Characters" section in https://yaml.org/spec/1.2.2/
            if (current == '\\')
            {
                if (!needs_last_double_quote)
                {
                    emit_error("Escaped characters are only available in a double-quoted string token.");
                }

                current = m_input_handler.get_next();
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
                    m_value_buffer.push_back(char_type(0x1B));
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
                    for (size_t i = 0; i < m_encoded_size; i++)
                    {
                        m_value_buffer.push_back(m_encode_buffer[i]);
                    }
                    break;
                case '_': // non-breaking space
                    utf8_encoding::from_utf32(0xA0u, m_encode_buffer, m_encoded_size);
                    for (size_t i = 0; i < m_encoded_size; i++)
                    {
                        m_value_buffer.push_back(m_encode_buffer[i]);
                    }
                    break;
                case 'L': // line separator
                    utf8_encoding::from_utf32(0x2028u, m_encode_buffer, m_encoded_size);
                    for (size_t i = 0; i < m_encoded_size; i++)
                    {
                        m_value_buffer.push_back(m_encode_buffer[i]);
                    }
                    break;
                case 'P': // paragraph separator
                    utf8_encoding::from_utf32(0x2029u, m_encode_buffer, m_encoded_size);
                    for (size_t i = 0; i < m_encoded_size; i++)
                    {
                        m_value_buffer.push_back(m_encode_buffer[i]);
                    }
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
                    emit_error("Unsupported escape sequence found in a string token.");
                }
                continue;
            }

            // Handle unescaped control characters.
            if (current <= 0x1F)
            {
                handle_unescaped_control_char(current);
                continue;
            }

            // Handle ASCII characters except control characters.
            if (current <= 0x7E)
            {
                m_value_buffer.push_back(char_traits_type::to_char_type(current));
                continue;
            }

            // Handle 2-byte characters encoded in UTF-8. (U+0080..U+07FF)
            if (current <= 0xDF)
            {
                std::array<char_int_type, 2> byte_array = {{current, m_input_handler.get_next()}};
                if (!utf8_encoding::validate(byte_array))
                {
                    throw fkyaml::invalid_encoding("ill-formed UTF-8 encoded character found", byte_array);
                }

                m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[0]));
                m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[1]));
                continue;
            }

            // Handle 3-byte characters encoded in UTF-8. (U+1000..U+D7FF,U+E000..U+FFFF)
            if (current <= 0xEF)
            {
                std::array<char_int_type, 3> byte_array = {
                    {current, m_input_handler.get_next(), m_input_handler.get_next()}};
                if (!utf8_encoding::validate(byte_array))
                {
                    throw fkyaml::invalid_encoding("ill-formed UTF-8 encoded character found", byte_array);
                }

                m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[0]));
                m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[1]));
                m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[2]));

                continue;
            }

            // Handle 4-byte characters encoded in UTF-8. (U+10000..U+FFFFF,U+100000..U+10FFFF)
            std::array<char_int_type, 4> byte_array = {
                {current, m_input_handler.get_next(), m_input_handler.get_next(), m_input_handler.get_next()}};
            if (!utf8_encoding::validate(byte_array))
            {
                throw fkyaml::invalid_encoding("ill-formed UTF-8 encoded character found", byte_array);
            }

            m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[0]));
            m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[1]));
            m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[2]));
            m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[3]));
        }
    }

    lexical_token_t scan_block_style_string_token(
        block_style_indicator_t style, chomping_indicator_t chomp, std::size_t indent)
    {
        m_value_buffer.clear();

        // Handle leading all-space lines.
        char_int_type current = m_input_handler.get_current();
        for (;; current = m_input_handler.get_next())
        {
            if (current == ' ')
            {
                continue;
            }

            if (current == '\r')
            {
                current = m_input_handler.get_next();
            }
            if (current == '\n')
            {
                m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                continue;
            }

            if (current == end_of_input)
            {
                if (chomp != chomping_indicator_t::KEEP)
                {
                    m_value_buffer.clear();
                }
                return lexical_token_t::STRING_VALUE;
            }

            break;
        }

        std::size_t cur_indent = m_input_handler.get_cur_pos_in_line();
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
                m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                is_extra_indented = true;
            }
            while (diff > 0)
            {
                m_value_buffer.push_back(char_traits_type::to_char_type(' '));
                --diff;
                ++chars_in_line;
            }
        }

        for (; current != end_of_input; current = m_input_handler.get_next())
        {
            if (current == '\r')
            {
                // Ignore CR assuming the next character is LF.
                continue;
            }

            if (current == '\n')
            {
                if (style == block_style_indicator_t::LITERAL)
                {
                    m_value_buffer.push_back(char_traits_type::to_char_type(current));
                }
                else // block_style_indicator_t::FOLDED
                {
                    if (chars_in_line == 0)
                    {
                        // Just append a newline if the current line is empty.
                        m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                        is_extra_indented = false;
                        continue;
                    }

                    if (is_extra_indented)
                    {
                        // A line being more indented is not folded.
                        m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                        chars_in_line = 0;
                        is_extra_indented = false;
                        continue;
                    }

                    // Append a newline if the next line is an empty line.
                    bool is_end_of_token = false;
                    bool is_next_empty = false;
                    for (std::size_t i = 0; i < indent; i++)
                    {
                        current = m_input_handler.get_next();
                        if (current == ' ')
                        {
                            continue;
                        }

                        if (current == '\r')
                        {
                            current = m_input_handler.get_next();
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
                        m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                        break;
                    }

                    if (is_next_empty)
                    {
                        m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                    }
                    else
                    {
                        switch (m_input_handler.get_next())
                        {
                        case '\r': {
                            m_input_handler.get_next();
                            FK_YAML_ASSERT(m_input_handler.get_current() == '\n');
                            m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                            break;
                        }
                        case '\n':
                            m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                            break;
                        case ' ':
                            // The next line is more indented, so a newline will be appended in the next loop.
                            m_input_handler.unget();
                            break;
                        default:
                            m_value_buffer.push_back(char_traits_type::to_char_type(' '));
                            m_input_handler.unget();
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
            cur_indent = m_input_handler.get_cur_pos_in_line();
            if (cur_indent < indent)
            {
                if (current != ' ')
                {
                    // Interpret less indented non-space characters as the start of next token.
                    break;
                }
                // skip a space if not yet indented enough
                continue;
            }

            if (style == block_style_indicator_t::FOLDED && chars_in_line == 0 && current == ' ')
            {
                // A line being more indented is not folded.
                m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                is_extra_indented = true;
            }
            m_value_buffer.push_back(char_traits_type::to_char_type(current));
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
    void handle_unescaped_control_char(char_int_type c)
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
            m_value_buffer.push_back(char_traits_type::to_char_type(c));
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
            char four_bits = convert_hex_char_to_byte(m_input_handler.get_next());
            // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
            code_point |= static_cast<char32_t>(four_bits << (4 * i));
        }

        // Treats the code point as a UTF-32 encoded character.
        utf8_encoding::from_utf32(code_point, m_encode_buffer, m_encoded_size);
        for (size_t i = 0; i < m_encoded_size; i++)
        {
            m_value_buffer.push_back(m_encode_buffer[i]);
        }
    }

    void get_block_style_metadata(chomping_indicator_t& chomp_type, std::size_t& indent)
    {
        char_int_type ch = m_input_handler.get_next();

        chomp_type = chomping_indicator_t::CLIP;
        if (ch == '-')
        {
            chomp_type = chomping_indicator_t::STRIP;
            ch = m_input_handler.get_next();
        }
        else if (ch == '+')
        {
            chomp_type = chomping_indicator_t::KEEP;
            ch = m_input_handler.get_next();
        }

        if (ch == '0')
        {
            emit_error("An indentation level for a block style scalar cannot be \'0\'");
        }

        indent = 0;
        if (std::isdigit(ch))
        {
            indent = convert_hex_char_to_byte(ch);
            ch = m_input_handler.get_next();
        }

        // skip characters including comments.
        skip_until_line_end();
    }

    /// @brief Skip white spaces (half-width spaces and tabs) from the current position.
    void skip_white_spaces()
    {
        do
        {
            switch (m_input_handler.get_current())
            {
            case ' ':
            case '\t':
                break;
            default:
                return;
            }
        } while (m_input_handler.get_next() != end_of_input);
    }

    /// @brief Skip white spaces and newline codes (CR/LF) from the current position.
    void skip_white_spaces_and_newline_codes()
    {
        do
        {
            skip_white_spaces();

            switch (m_input_handler.get_current())
            {
            case '\n':
            case '\r':
                break;
            default:
                return;
            }
        } while (m_input_handler.get_next() != end_of_input);
    }

    /// @brief Skip white spaces and comments from the current position.
    void skip_white_spaces_and_comments()
    {
        do
        {
            skip_white_spaces();

            switch (m_input_handler.get_current())
            {
            case '#': {
                scan_comment();
                break;
            }
            default:
                return;
            }
        } while (m_input_handler.get_next() != end_of_input);
    }

    /// @brief Skip the rest in the current line.
    void skip_until_line_end()
    {
        do
        {
            switch (m_input_handler.get_current())
            {
            case '\r':
                if (m_input_handler.get_next() == '\n')
                {
                    m_input_handler.get_next();
                }
                return;
            case '\n':
                m_input_handler.get_next();
                return;
            default:
                break;
            }
        } while (m_input_handler.get_next() != end_of_input);
    }

    [[noreturn]] void emit_error(const char* msg) const
    {
        throw fkyaml::parse_error(msg, m_input_handler.get_lines_read(), m_input_handler.get_cur_pos_in_line());
    }

private:
    /// The value of EOF for the target characters.
    static constexpr char_int_type end_of_input = char_traits_type::eof();

    /// An input buffer adapter to be analyzed.
    input_handler_type m_input_handler;
    /// A temporal buffer to store a string to be parsed to an actual datum.
    input_string_type m_value_buffer {};
    std::array<char, 4> m_encode_buffer {};
    std::size_t m_encoded_size {0};
    std::size_t m_last_token_begin_pos {0};
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
