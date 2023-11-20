///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.2.1
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
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
        skip_white_spaces();

        char_int_type current = m_input_handler.get_current();
        m_last_token_begin_pos = m_input_handler.get_cur_pos_in_line();

        if (0x00 <= current && current <= 0x7F && isdigit(current))
        {
            return m_last_token_type = scan_number();
        }

        switch (current)
        {
        case end_of_input: // end of input buffer
            return lexical_token_t::END_OF_BUFFER;
        case ':': // key separater
            switch (m_input_handler.get_next())
            {
            case ' ':
                break;
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
                throw fkyaml::exception("Half-width spaces or newline codes are required after a key separater(:).");
            }
            m_input_handler.get_next();
            return m_last_token_type = lexical_token_t::KEY_SEPARATOR;
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
                    throw fkyaml::exception("An anchor label must be followed by some value.");
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
                        throw fkyaml::exception("An alias prefix must be followed by some anchor name.");
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
            bool is_next_space = m_input_handler.test_next_char(' ');
            if (!is_next_space)
            {
                char_int_type ret = m_input_handler.get_range(3, m_value_buffer);
                if (ret != end_of_input)
                {
                    if (m_value_buffer == "---")
                    {
                        m_input_handler.get_next();
                        return m_last_token_type = lexical_token_t::END_OF_DIRECTIVES;
                    }

                    // revert change in the position to the one before comparison above.
                    m_input_handler.unget_range(2);
                }
                return m_last_token_type = scan_number();
            }

            // Move a cursor to the beginning of the next token.
            m_input_handler.get_next();
            m_input_handler.get_next();

            return m_last_token_type = lexical_token_t::SEQUENCE_BLOCK_PREFIX;
        }
        case '[': // sequence flow begin
            m_input_handler.get_next();
            return m_last_token_type = lexical_token_t::SEQUENCE_FLOW_BEGIN;
        case ']': // sequence flow end
            m_input_handler.get_next();
            return m_last_token_type = lexical_token_t::SEQUENCE_FLOW_END;
        case '{': // mapping flow begin
            m_input_handler.get_next();
            return m_last_token_type = lexical_token_t::MAPPING_FLOW_BEGIN;
        case '}': // mapping flow end
            m_input_handler.get_next();
            return m_last_token_type = lexical_token_t::MAPPING_FLOW_END;
        case '@':
            throw fkyaml::exception("Any token cannot start with at(@). It is a reserved indicator for YAML.");
        case '`':
            throw fkyaml::exception(
                "Any token cannot start with grave accent(`). It is a reserved indicator for YAML.");
        case '\"':
        case '\'':
            return m_last_token_type = scan_string();
        case '~':
            m_value_buffer = char_traits_type::to_char_type(current);
            return m_last_token_type = lexical_token_t::NULL_VALUE;
        case '+':
            return m_last_token_type = scan_number();
        case '.': {
            char_int_type ret = m_input_handler.get_range(4, m_value_buffer);
            if (ret != end_of_input)
            {
                try
                {
                    // try convert to an infinite/nan value.
                    m_float_val = from_string(m_value_buffer, type_tag<float_number_type> {});
                    m_input_handler.get_next();
                    return m_last_token_type = lexical_token_t::FLOAT_NUMBER_VALUE;
                }
                catch (const fkyaml::exception& /*unused*/)
                {
                    // revert change in the position to the one before comparison above.
                    m_input_handler.unget_range(3);
                    return m_last_token_type = scan_string();
                }
            }

            ret = m_input_handler.get_range(3, m_value_buffer);
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
        case 'F':
        case 'f': {
            // YAML specifies that only these words represent the boolean value `false`.
            // See "10.3.2 Tag Resolution" section in https://yaml.org/spec/1.2.2/
            char_int_type ret = m_input_handler.get_range(5, m_value_buffer);
            if (ret == end_of_input)
            {
                return m_last_token_type = scan_string();
            }

            try
            {
                // try convert to a boolean false value.
                m_boolean_val = from_string(m_value_buffer, type_tag<boolean_type> {});
                m_input_handler.get_next();
                return m_last_token_type = lexical_token_t::BOOLEAN_VALUE;
            }
            catch (const fkyaml::exception& /*unused*/)
            {
                // revert change in the position to the one before comparison above.
                m_input_handler.unget_range(4);
                return m_last_token_type = scan_string();
            }
        }
        case 'N':
        case 'n': {
            // YAML specifies that these words and a tilde represent a null value.
            // Tildes are already checked above, so no check is needed here.
            // See "10.3.2 Tag Resolution" section in https://yaml.org/spec/1.2.2/
            char_int_type ret = m_input_handler.get_range(4, m_value_buffer);
            if (ret == end_of_input)
            {
                return m_last_token_type = scan_string();
            }

            try
            {
                // try convert to a null value.
                from_string(m_value_buffer, type_tag<std::nullptr_t> {});
                m_input_handler.get_next();
                return m_last_token_type = lexical_token_t::NULL_VALUE;
            }
            catch (const fkyaml::exception& /*unused*/)
            {
                // revert change in the position to the one before comparison above.
                m_input_handler.unget_range(3);
                return m_last_token_type = scan_string();
            }
        }
        case 'T':
        case 't': {
            // YAML specifies that only these words represent the boolean value `true`.
            // See "10.3.2 Tag Resolution" section in https://yaml.org/spec/1.2.2/
            char_int_type ret = m_input_handler.get_range(4, m_value_buffer);
            if (ret == end_of_input)
            {
                return m_last_token_type = scan_string();
            }

            try
            {
                // try convert to a boolean true value.
                m_boolean_val = from_string(m_value_buffer, type_tag<boolean_type> {});
                m_input_handler.get_next();
                return m_last_token_type = lexical_token_t::BOOLEAN_VALUE;
            }
            catch (const fkyaml::exception& /*unused*/)
            {
                // revert change in the position to the one before comparison above.
                m_input_handler.unget_range(3);
                return m_last_token_type = scan_string();
            }
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

    /// @brief Convert from string to null and get the converted value.
    /// @return std::nullptr_t A null value converted from one of the followings: "null", "Null", "NULL", "~".
    std::nullptr_t get_null() const
    {
        if (m_last_token_type == lexical_token_t::NULL_VALUE)
        {
            return nullptr;
        }
        throw fkyaml::exception("Invalid request for a null value.");
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
        throw fkyaml::exception("Invalid request for a boolean value.");
    }

    /// @brief Convert from string to integer and get the converted value.
    /// @return integer_type An integer value converted from the source string.
    integer_type get_integer() const
    {
        if (m_last_token_type == lexical_token_t::INTEGER_VALUE)
        {
            return m_integer_val;
        }
        throw fkyaml::exception("Invalid request for an integer value.");
    }

    /// @brief Convert from string to float number and get the converted value.
    /// @return float_number_type A float number value converted from the source string.
    float_number_type get_float_number() const
    {
        if (m_last_token_type == lexical_token_t::FLOAT_NUMBER_VALUE)
        {
            return m_float_val;
        }
        throw fkyaml::exception("Invalid request for a float number value.");
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
    static char convert_hex_char_to_byte(char_int_type source)
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

        throw fkyaml::exception("Non-hexadecimal character has been given.");
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
            throw fkyaml::exception("invalid eof in a directive.");
        case 'T': {
            if (m_input_handler.get_next() != 'A' || m_input_handler.get_next() != 'G')
            {
                skip_until_line_end();
                return lexical_token_t::INVALID_DIRECTIVE;
            }
            if (m_input_handler.get_next() != ' ')
            {
                throw fkyaml::exception("There must be a half-width space between \"%TAG\" and tag info.");
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
                throw fkyaml::exception("There must be a half-width space between \"%YAML\" and a version number.");
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
            throw fkyaml::exception("Invalid YAML major version found.");
        }
        m_value_buffer.push_back(char_traits_type::to_char_type(m_input_handler.get_current()));

        if (m_input_handler.get_next() != '.')
        {
            throw fkyaml::exception("A period must be followed after the YAML major version.");
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
            throw fkyaml::exception("Unsupported YAML version.");
        default:
            throw fkyaml::exception("YAML version must be specified with digits and periods.");
        }

        if (m_input_handler.get_next() != ' ' && m_input_handler.get_current() != '\r' &&
            m_input_handler.get_current() != '\n')
        {
            throw fkyaml::exception("Only YAML version 1.1/1.2 are supported.");
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
        default:                                                                   // LCOV_EXCL_LINE
            throw fkyaml::exception("Invalid character found in a number token."); // LCOV_EXCL_LINE
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
        FK_YAML_ASSERT(std::isdigit(next) || next == '.');

        if (std::isdigit(next))
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            return scan_decimal_number();
        }

        char_int_type ret = m_input_handler.get_range(4, m_value_buffer);
        if (ret != end_of_input)
        {
            try
            {
                // check if convertible to an infinite value.
                from_string(m_value_buffer, type_tag<float_number_type> {});
                m_input_handler.get_next();
                return lexical_token_t::FLOAT_NUMBER_VALUE;
            }
            catch (const fkyaml::exception& /*unused*/)
            {
                // handle this error below.
            }
        }

        throw fkyaml::exception("Invalid character found in a negative number token."); // LCOV_EXCL_LINE
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
            scan_decimal_number();
            return lexical_token_t::FLOAT_NUMBER_VALUE;
        }

        throw fkyaml::exception("Invalid character found after a decimal point."); // LCOV_EXCL_LINE
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
            throw fkyaml::exception("unexpected character found after exponent.");
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

        throw fkyaml::exception("Non-numeric character found after a sign(+/-) after exponent(e/E)."); // LCOV_EXCL_LINE
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
            if (m_value_buffer.find(char_traits_type::to_char_type(next)) != string_type::npos)
            {
                // TODO: support this use case (e.g. version info like 1.0.0)
                throw fkyaml::exception("Multiple decimal points found in a token.");
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
    /// @note Multibyte characters(including escaped ones) are currently unsupported.
    /// @return lexical_token_t The lexical token type for strings.
    lexical_token_t scan_string()
    {
        m_value_buffer.clear();

        const bool needs_last_double_quote = (m_input_handler.get_current() == '\"');
        const bool needs_last_single_quote = (m_input_handler.get_current() == '\'');
        char_int_type current = (needs_last_double_quote || needs_last_single_quote) ? m_input_handler.get_next()
                                                                                     : m_input_handler.get_current();

        for (;; current = m_input_handler.get_next())
        {
            // Handle the end of input buffer.
            if (current == end_of_input)
            {
                if (needs_last_double_quote)
                {
                    throw fkyaml::exception("Invalid end of input buffer in a double-quoted string token.");
                }

                if (needs_last_single_quote)
                {
                    throw fkyaml::exception("Invalid end of input buffer in a single-quoted string token.");
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

                if (!needs_last_single_quote)
                {
                    throw fkyaml::exception("Invalid double quotation mark found in a string token.");
                }

                // if the target is a single-quoted string token.
                m_value_buffer.push_back(char_traits_type::to_char_type(current));
                continue;
            }

            // Handle single quotation marks.
            if (current == '\'')
            {
                if (needs_last_single_quote)
                {
                    // If single quotation marks are repeated twice in a single-quoted string token, they are considered as
                    // an escaped single quotation mark.
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

            // Handle commas.
            if (current == ',')
            {
                // Just regard a comma as a character if surrounded by quotation marks.
                if (needs_last_double_quote || needs_last_single_quote)
                {
                    m_value_buffer.push_back(char_traits_type::to_char_type(current));
                    continue;
                }

                return lexical_token_t::STRING_VALUE;
            }

            // Handle right square brackets.
            if (current == ']')
            {
                // just regard a right square bracket as a character if surrounded by quotation marks.
                if (needs_last_double_quote || needs_last_single_quote)
                {
                    m_value_buffer.push_back(char_traits_type::to_char_type(current));
                    continue;
                }

                return lexical_token_t::STRING_VALUE;
            }

            // Handle right curly braces.
            if (current == '}')
            {
                // just regard a right curly brace as a character if surrounded by quotation marks.
                if (needs_last_double_quote || needs_last_single_quote)
                {
                    m_value_buffer.push_back(char_traits_type::to_char_type(current));
                    continue;
                }

                return lexical_token_t::STRING_VALUE;
            }

            // Handle newline codes.
            if (current == '\r' || current == '\n')
            {
                if (!(needs_last_double_quote || needs_last_single_quote))
                {
                    return lexical_token_t::STRING_VALUE;
                }

                // TODO: Support multi-line string tokens.
                throw fkyaml::exception("multi-line string tokens are unsupported.");
            }

            // Handle escaped characters.
            // See "5.7 Escaped Characters" section in https://yaml.org/spec/1.2.2/
            if (current == '\\')
            {
                if (!needs_last_double_quote)
                {
                    throw fkyaml::exception("Escaped characters are only available in a double-quoted string token.");
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
                    handle_unicode_code_point(0x85u);
                    break;
                case '_': // non-breaking space
                    handle_unicode_code_point(0xA0u);
                    break;
                case 'L': // line separator
                    handle_unicode_code_point(0x2028u);
                    break;
                case 'P': // paragraph separator
                    handle_unicode_code_point(0x2029u);
                    break;
                case 'x': {
                    uint32_t code_point = 0;
                    for (int i = 1; i >= 0; --i)
                    {
                        char four_bits = convert_hex_char_to_byte(m_input_handler.get_next());
                        // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
                        code_point |= static_cast<uint32_t>(four_bits << (4 * i));
                    }
                    handle_unicode_code_point(code_point);
                    break;
                }
                case 'u': {
                    uint32_t code_point = 0;
                    for (int i = 3; i >= 0; --i)
                    {
                        char four_bits = convert_hex_char_to_byte(m_input_handler.get_next());
                        // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
                        code_point |= static_cast<uint32_t>(four_bits << (4 * i));
                    }
                    handle_unicode_code_point(code_point);
                    break;
                }
                case 'U': {
                    uint32_t code_point = 0;
                    for (int i = 7; i >= 0; --i)
                    {
                        current = m_input_handler.get_next();
                        char four_bits = convert_hex_char_to_byte(current);
                        // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
                        code_point |= static_cast<uint32_t>(four_bits << (4 * i));
                    }
                    handle_unicode_code_point(code_point);
                    break;
                }
                default:
                    throw fkyaml::exception("Unsupported escape sequence found in a string token.");
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
            //   1st Byte: 0xC2..0xDF
            //   2nd Byte: 0x80..0xBF
            if (0xC2 <= current && current <= 0xDF)
            {
                char_int_type second_byte_char = m_input_handler.get_next();
                if (0x80 <= second_byte_char && second_byte_char <= 0xBF)
                {
                    m_value_buffer.push_back(char_traits_type::to_char_type(current));
                    m_value_buffer.push_back(char_traits_type::to_char_type(second_byte_char));
                    continue;
                }
                throw fkyaml::exception("ill-formed UTF-8 encoded character found");
            }

            // Handle 3-byte characters encoded in UTF-8. (U+1000..U+CFFF)
            //   1st Byte: 0xE0..0xEC
            //   2nd Byte: 0x80..0xBF
            //   3rd Byte: 0x80..0xBF
            if (0xE0 <= current && current <= 0xEC)
            {
                char_int_type second_byte_char = m_input_handler.get_next();
                if (0x80 <= second_byte_char && second_byte_char <= 0xBF)
                {
                    char_int_type third_byte_char = m_input_handler.get_next();
                    if (0x80 <= third_byte_char && third_byte_char <= 0xBF)
                    {
                        m_value_buffer.push_back(char_traits_type::to_char_type(current));
                        m_value_buffer.push_back(char_traits_type::to_char_type(second_byte_char));
                        m_value_buffer.push_back(char_traits_type::to_char_type(third_byte_char));
                        continue;
                    }
                }
                throw fkyaml::exception("ill-formed UTF-8 encoded character found");
            }

            // Handle 3-byte characters encoded in UTF-8. (U+D000..U+D7FF)
            //   1st Byte: 0xED
            //   2nd Byte: 0x80..0x9F
            //   3rd Byte: 0x80..0xBF
            if (current == 0xED)
            {
                char_int_type second_byte_char = m_input_handler.get_next();
                if (0x80 <= second_byte_char && second_byte_char <= 0x9F)
                {
                    char_int_type third_byte_char = m_input_handler.get_next();
                    if (0x80 <= third_byte_char && third_byte_char <= 0xBF)
                    {
                        m_value_buffer.push_back(char_traits_type::to_char_type(current));
                        m_value_buffer.push_back(char_traits_type::to_char_type(second_byte_char));
                        m_value_buffer.push_back(char_traits_type::to_char_type(third_byte_char));
                        continue;
                    }
                }
                throw fkyaml::exception("ill-formed UTF-8 encoded character found");
            }

            // Handle 3-byte characters encoded in UTF-8. (U+E000..U+FFFF)
            //   1st Byte: 0xEE..0xEF
            //   2nd Byte: 0x80..0xBF
            //   3rd Byte: 0x80..0xBF
            if (current == 0xEE || current == 0xEF)
            {
                char_int_type second_byte_char = m_input_handler.get_next();
                if (0x80 <= second_byte_char && second_byte_char <= 0xBF)
                {
                    char_int_type third_byte_char = m_input_handler.get_next();
                    if (0x80 <= third_byte_char && third_byte_char <= 0xBF)
                    {
                        m_value_buffer.push_back(char_traits_type::to_char_type(current));
                        m_value_buffer.push_back(char_traits_type::to_char_type(second_byte_char));
                        m_value_buffer.push_back(char_traits_type::to_char_type(third_byte_char));
                        continue;
                    }
                }
                throw fkyaml::exception("ill-formed UTF-8 encoded character found");
            }

            // Handle 4-byte characters encoded in UTF-8. (U+10000..U+3FFFF)
            //   1st Byte: 0xF0
            //   2nd Byte: 0x90..0xBF
            //   3rd Byte: 0x80..0xBF
            //   4th Byte: 0x80..0xBF
            if (current == 0xF0)
            {
                char_int_type second_byte_char = m_input_handler.get_next();
                if (0x90 <= second_byte_char && second_byte_char <= 0xBF)
                {
                    char_int_type third_byte_char = m_input_handler.get_next();
                    if (0x80 <= third_byte_char && third_byte_char <= 0xBF)
                    {
                        char_int_type fourth_byte_char = m_input_handler.get_next();
                        if (0x80 <= fourth_byte_char && fourth_byte_char <= 0xBF)
                        {
                            m_value_buffer.push_back(char_traits_type::to_char_type(current));
                            m_value_buffer.push_back(char_traits_type::to_char_type(second_byte_char));
                            m_value_buffer.push_back(char_traits_type::to_char_type(third_byte_char));
                            m_value_buffer.push_back(char_traits_type::to_char_type(fourth_byte_char));
                            continue;
                        }
                    }
                }
                throw fkyaml::exception("ill-formed UTF-8 encoded character found");
            }

            // Handle 4-byte characters encoded in UTF-8. (U+40000..U+FFFFF)
            //   1st Byte: 0xF1..0xF3
            //   2nd Byte: 0x80..0xBF
            //   3rd Byte: 0x80..0xBF
            //   4th Byte: 0x80..0xBF
            if (0xF1 <= current && current <= 0xF3)
            {
                char_int_type second_byte_char = m_input_handler.get_next();
                if (0x80 <= second_byte_char && second_byte_char <= 0xBF)
                {
                    char_int_type third_byte_char = m_input_handler.get_next();
                    if (0x80 <= third_byte_char && third_byte_char <= 0xBF)
                    {
                        char_int_type fourth_byte_char = m_input_handler.get_next();
                        if (0x80 <= fourth_byte_char && fourth_byte_char <= 0xBF)
                        {
                            m_value_buffer.push_back(char_traits_type::to_char_type(current));
                            m_value_buffer.push_back(char_traits_type::to_char_type(second_byte_char));
                            m_value_buffer.push_back(char_traits_type::to_char_type(third_byte_char));
                            m_value_buffer.push_back(char_traits_type::to_char_type(fourth_byte_char));
                            continue;
                        }
                    }
                }
                throw fkyaml::exception("ill-formed UTF-8 encoded character found");
            }

            // Handle 4-byte characters encoded in UTF-8. (U+100000..U+10FFFF)
            //   1st Byte: 0xF4
            //   2nd Byte: 0x80..0x8F
            //   3rd Byte: 0x80..0xBF
            //   4th Byte: 0x80..0xBF
            if (current == 0xF4)
            {
                char_int_type second_byte_char = m_input_handler.get_next();
                if (0x80 <= second_byte_char && second_byte_char <= 0x8F)
                {
                    char_int_type third_byte_char = m_input_handler.get_next();
                    if (0x80 <= third_byte_char && third_byte_char <= 0xBF)
                    {
                        char_int_type fourth_byte_char = m_input_handler.get_next();
                        if (0x80 <= fourth_byte_char && fourth_byte_char <= 0xBF)
                        {
                            m_value_buffer.push_back(char_traits_type::to_char_type(current));
                            m_value_buffer.push_back(char_traits_type::to_char_type(second_byte_char));
                            m_value_buffer.push_back(char_traits_type::to_char_type(third_byte_char));
                            m_value_buffer.push_back(char_traits_type::to_char_type(fourth_byte_char));
                            continue;
                        }
                    }
                }
                throw fkyaml::exception("ill-formed UTF-8 encoded character found");
            }

            // remaining bytes (0x80..0xC1 and 0xF5..0xFF) are ill formed.
            throw fkyaml::exception("Unsupported multibytes character found.");
        }
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
            throw fkyaml::exception("Control character U+0001 (SOH) must be escaped to \\u0001.");
        case 0x02:
            throw fkyaml::exception("Control character U+0002 (STX) must be escaped to \\u0002.");
        case 0x03:
            throw fkyaml::exception("Control character U+0003 (ETX) must be escaped to \\u0003.");
        case 0x04:
            throw fkyaml::exception("Control character U+0004 (EOT) must be escaped to \\u0004.");
        case 0x05:
            throw fkyaml::exception("Control character U+0005 (ENQ) must be escaped to \\u0005.");
        case 0x06:
            throw fkyaml::exception("Control character U+0006 (ACK) must be escaped to \\u0006.");
        case 0x07:
            throw fkyaml::exception("Control character U+0007 (BEL) must be escaped to \\a or \\u0007.");
        case 0x08:
            throw fkyaml::exception("Control character U+0008 (BS) must be escaped to \\b or \\u0008.");
        case 0x09: // HT
            m_value_buffer.push_back(char_traits_type::to_char_type(c));
            break;
        // 0x0A(LF) has already been handled above.
        case 0x0B:
            throw fkyaml::exception("Control character U+000B (VT) must be escaped to \\v or \\u000B.");
        case 0x0C:
            throw fkyaml::exception("Control character U+000C (FF) must be escaped to \\f or \\u000C.");
        // 0x0D(CR) has already been handled above.
        case 0x0E:
            throw fkyaml::exception("Control character U+000E (SO) must be escaped to \\u000E.");
        case 0x0F:
            throw fkyaml::exception("Control character U+000F (SI) must be escaped to \\u000F.");
        case 0x10:
            throw fkyaml::exception("Control character U+0010 (DLE) must be escaped to \\u0010.");
        case 0x11:
            throw fkyaml::exception("Control character U+0011 (DC1) must be escaped to \\u0011.");
        case 0x12:
            throw fkyaml::exception("Control character U+0012 (DC2) must be escaped to \\u0012.");
        case 0x13:
            throw fkyaml::exception("Control character U+0013 (DC3) must be escaped to \\u0013.");
        case 0x14:
            throw fkyaml::exception("Control character U+0014 (DC4) must be escaped to \\u0014.");
        case 0x15:
            throw fkyaml::exception("Control character U+0015 (NAK) must be escaped to \\u0015.");
        case 0x16:
            throw fkyaml::exception("Control character U+0016 (SYN) must be escaped to \\u0016.");
        case 0x17:
            throw fkyaml::exception("Control character U+0017 (ETB) must be escaped to \\u0017.");
        case 0x18:
            throw fkyaml::exception("Control character U+0018 (CAN) must be escaped to \\u0018.");
        case 0x19:
            throw fkyaml::exception("Control character U+0019 (EM) must be escaped to \\u0019.");
        case 0x1A:
            throw fkyaml::exception("Control character U+001A (SUB) must be escaped to \\u001A.");
        case 0x1B:
            throw fkyaml::exception("Control character U+001B (ESC) must be escaped to \\e or \\u001B.");
        case 0x1C:
            throw fkyaml::exception("Control character U+001C (FS) must be escaped to \\u001C.");
        case 0x1D:
            throw fkyaml::exception("Control character U+001D (GS) must be escaped to \\u001D.");
        case 0x1E:
            throw fkyaml::exception("Control character U+001E (RS) must be escaped to \\u001E.");
        case 0x1F:
            throw fkyaml::exception("Control character U+001F (US) must be escaped to \\u001F.");
        }
    }

    void handle_unicode_code_point(uint32_t code_point)
    {
        bool is_valid = false;

        if (code_point < 0x80)
        {
            m_value_buffer.push_back(static_cast<char_type>(code_point & 0x007F));
            is_valid = true;
        }
        else if (code_point <= 0x7FF)
        {
            uint16_t utf8_encoded = 0b1100000010000000;
            utf8_encoded |= static_cast<uint16_t>((code_point & 0x07C0) << 2);
            utf8_encoded |= static_cast<uint16_t>((code_point & 0x003F));
            m_value_buffer.push_back(static_cast<char_type>((utf8_encoded & 0xFF00) >> 8));
            m_value_buffer.push_back(static_cast<char_type>(utf8_encoded & 0x00FF));
            is_valid = true;
        }
        else if (code_point <= 0xFFFF)
        {
            uint32_t utf8_encoded = 0b111000001000000010000000;
            utf8_encoded |= static_cast<uint32_t>((code_point & 0xF000) << 4);
            utf8_encoded |= static_cast<uint32_t>((code_point & 0x0FC0) << 2);
            utf8_encoded |= static_cast<uint32_t>((code_point & 0x003F));
            m_value_buffer.push_back(static_cast<char_type>((utf8_encoded & 0xFF0000) >> 16));
            m_value_buffer.push_back(static_cast<char_type>((utf8_encoded & 0x00FF00) >> 8));
            m_value_buffer.push_back(static_cast<char_type>(utf8_encoded & 0x0000FF));
            is_valid = true;
        }
        else if (code_point <= 0x10FFFF)
        {
            uint32_t utf8_encoded = 0b11110000100000001000000010000000;
            utf8_encoded |= static_cast<uint32_t>((code_point & 0x1C0000) << 6);
            utf8_encoded |= static_cast<uint32_t>((code_point & 0x03F000) << 4);
            utf8_encoded |= static_cast<uint32_t>((code_point & 0x000FC0) << 2);
            utf8_encoded |= static_cast<uint32_t>((code_point & 0x00003F));
            m_value_buffer.push_back(static_cast<char_type>((utf8_encoded & 0xFF000000) >> 24));
            m_value_buffer.push_back(static_cast<char_type>((utf8_encoded & 0x00FF0000) >> 16));
            m_value_buffer.push_back(static_cast<char_type>((utf8_encoded & 0x0000FF00) >> 8));
            m_value_buffer.push_back(static_cast<char_type>(utf8_encoded & 0x000000FF));
            is_valid = true;
        }

        if (!is_valid)
        {
            throw fkyaml::exception("Invalid Unicode code point.");
        }
    }

    /// @brief Skip white spaces, tabs and newline codes until any other kind of character is found.
    void skip_white_spaces()
    {
        while (true)
        {
            switch (m_input_handler.get_current())
            {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                break;
            default:
                return;
            }
            m_input_handler.get_next();
        }
    }

    /// @brief Skip reading in the current line.
    void skip_until_line_end()
    {
        while (true)
        {
            switch (m_input_handler.get_current())
            {
            case end_of_input:
                return;
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
            m_input_handler.get_next();
        }
    }

private:
    /// The value of EOF for the target characters.
    static constexpr char_int_type end_of_input = char_traits_type::eof();

    /// An input buffer adapter to be analyzed.
    input_handler_type m_input_handler;
    /// A temporal buffer to store a string to be parsed to an actual datum.
    input_string_type m_value_buffer {};
    std::size_t m_last_token_begin_pos {0};
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
