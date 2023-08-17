/**
 * @file LexicalAnalyzer.hpp
 * @brief Implementation of the lexical analyzer for YAML documents.
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_LEXICAL_ANALIZER_HPP_
#define FK_YAML_LEXICAL_ANALIZER_HPP_

#include <cstdint>
#include <string>

#include "fkYAML/Exception.hpp"

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
namespace fkyaml
{

/**
 * @enum LexicalTokenType
 * @brief Definition of lexical token types.
 */
enum class LexicalTokenType
{
    END_OF_BUFFER,         //!< the end of input buffer.
    KEY_SEPARATOR,         //!< the key separater `:`
    VALUE_SEPARATOR,       //!< the value separater `,`
    ANCHOR_PREFIX,         //!< the character for anchor prefix `&`
    ALIAS_PREFIX,          //!< the character for alias prefix `*`
    COMMENT_PREFIX,        //!< the character for comment prefix `#`
    DIRECTIVE_PREFIX,      //!< the character for directive prefix `%`
    SEQUENCE_BLOCK_PREFIX, //!< the character for sequence block prefix `- `
    SEQUENCE_FLOW_BEGIN,   //!< the character for sequence flow begin `[`
    SEQUENCE_FLOW_END,     //!< the character for sequence flow end `]`
    MAPPING_FLOW_BEGIN,    //!< the character for mapping begin `{`
    MAPPING_FLOW_END,      //!< the character for mapping end `}`
    BOOLEAN_VALUE,         //!< a boolean value found. use GetBoolean() to get a value.
    SIGNED_INT_VALUE,      //!< a signed integer value found. use GetSignedInt() to get a value.
    UNSIGNED_INT_VALUE,    //!< an unsigned integer value found. use GetUnsignedInt() to get a value.
    FLOAT_NUMBER_VALUE,    //!< a float number value found. use GetFloatNumber() to get a value.
    STRING_VALUE,          //!< the character for string begin `"` or any character except the above ones
    LITERAL_NULL,          //!< the `null`|`Null`|`NULL`|`~` literal
    LITERAL_TRUE,          //!< the `true`|`True`|`TRUE` literal
    LITERAL_FALSE,         //!< the `false`|`False`|`FALSE` literal
    LITERAL_INF,           //!< the `.inf`|`.Inf`|`.INF` literal
    LITERAL_NAN,           //!< the `.nan`|`.NaN`|`.NAN` literal
};

/**
 * @class LexicalAnalyzer
 * @brief A class which lexically analizes YAML formatted inputs.
 */
class LexicalAnalyzer
{
private:
    using char_traits_type = std::char_traits<char>;
    using char_int_type = typename char_traits_type::int_type;

    /**
     * @struct Position
     * @brief Information set of analyzed data counters.
     */
    struct Position
    {
        //!< The total read char counts from the input buffer.
        size_t total_read_char_counts = 0;
        //!< The total read line counts.
        size_t total_read_line_counts = 0;
        //!< The total read char counts in the current line.
        size_t read_char_counts_in_line = 0;
        //!< The total char counts in the previous line.
        size_t prev_char_counts_in_line = 0;
    };

public:
    /**
     * @brief Construct a new LexicalAnalyzer object.
     */
    LexicalAnalyzer() = default;

public:
    /**
     * @brief Set an input buffer to be analyzed by this LexicalAnalyzer.
     *
     * @param[in] input_buffer An input buffer to be analyzed.
     */
    void SetInputBuffer(const char* const input_buffer)
    {
        if (!input_buffer || input_buffer[0] == '\0')
        {
            throw Exception("The input buffer for lexical analysis is nullptr or empty.");
        }
        m_input_buffer = input_buffer;
    }

    /**
     * @brief Get the next lexical token type by scanning the left of the input buffer.
     *
     * @return LexicalTokenType The next lexical token type.
     */
    LexicalTokenType GetNextToken()
    {
        if (m_input_buffer.empty())
        {
            throw Exception("The next token is required before an input buffer is set.");
        }

        const char& current = RefCurrentChar();
        switch (current)
        {
        case s_eof: // end of buffer
            return LexicalTokenType::END_OF_BUFFER;
        case ':': // key separater
            return LexicalTokenType::KEY_SEPARATOR;
        case ',': // value separater
            return LexicalTokenType::VALUE_SEPARATOR;
        case '&': // anchor prefix
            return LexicalTokenType::ANCHOR_PREFIX;
        case '*': // alias prefix
            return LexicalTokenType::ALIAS_PREFIX;
        case '#': // comment prefix
            ScanComment();
            return LexicalTokenType::COMMENT_PREFIX;
        case '%': // directive prefix
            return LexicalTokenType::DIRECTIVE_PREFIX;
        case '-':
            if (RefNextChar() == ' ')
            {
                return LexicalTokenType::SEQUENCE_BLOCK_PREFIX;
            }
            return ScanNumber();
        case '[': // sequence flow begin
            return LexicalTokenType::SEQUENCE_FLOW_BEGIN;
        case ']': // sequence flow end
            return LexicalTokenType::SEQUENCE_FLOW_END;
        case '{': // mapping flow begin
            return LexicalTokenType::MAPPING_FLOW_BEGIN;
        case '}': // mapping flow end
            return LexicalTokenType::MAPPING_FLOW_END;
        case '+':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return ScanNumber();
        default:
            throw Exception("Unsupported lexical token is found.");
        }
    }

private:
    /**
     * @brief Skip until a newline code or a null character is found.
     *
     * @return LexicalTokenType The lexical token type for comments
     */
    LexicalTokenType ScanComment()
    {
        if (RefCurrentChar() != '#')
        {
            throw Exception("Not the beginning of a comment section.");
        }

        while (true)
        {
            switch (GetNextChar())
            {
            case '\r':
                if (RefNextChar() == '\r')
                {
                    GetNextChar();
                }
            case '\n':
            case '\0':
                return LexicalTokenType::COMMENT_PREFIX;
            default:
                break;
            }
        }
    }

    /**
     * @brief Scan and determine a number type(signed/unsigned/float). This method is the entrypoint for all number
     * tokens.
     *
     * @return LexicalTokenType A lexical token type for a determined number type.
     */
    LexicalTokenType ScanNumber()
    {
        m_value_buffer.clear();
        switch (RefCurrentChar())
        {
        case '-':
            m_value_buffer.push_back(RefCurrentChar());
            return ScanNegativeNumber();
        case '0':
            m_value_buffer.push_back(RefCurrentChar());
            return ScanNumberAfterZeroAtFirst();
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            m_value_buffer.push_back(RefCurrentChar());
            return ScanDecimalNumber();
        default:
            throw Exception("Invalid character found in a number token.");
        }
    }

    /**
     * @brief Scan a next character after the negative sign(-).
     *
     * @return LexicalTokenType The lexical token type for either signed or float numbers.
     */
    LexicalTokenType ScanNegativeNumber()
    {
        const char& next = GetNextChar();

        if ('0' <= next && next <= '9')
        {
            m_value_buffer.push_back(next);
            LexicalTokenType ret = ScanDecimalNumber();
            return (ret == LexicalTokenType::FLOAT_NUMBER_VALUE) ? ret : LexicalTokenType::SIGNED_INT_VALUE;
        }

        throw Exception("Invalid character found in a negative number token.");
    }

    /**
     * @brief Scan a next character after '0' at the beginning of a token.
     *
     * @return LexicalTokenType The lexical token type for one of number types(signed/unsigned/float).
     */
    LexicalTokenType ScanNumberAfterZeroAtFirst()
    {
        const char& next = GetNextChar();
        switch (next)
        {
        case '.':
            m_value_buffer.push_back(next);
            return ScanDecimalNumberAfterDecimalPoint();
        case 'o':
            m_value_buffer.push_back(next);
            return ScanOctalNumber();
        case 'x':
            m_value_buffer.push_back(next);
            return ScanHexadecimalNumber();
        default:
            return LexicalTokenType::UNSIGNED_INT_VALUE;
        }
    }

    /**
     * @brief Scan a next character after a decimal point.
     *
     * @return LexicalTokenType The lexical token type for float numbers.
     */
    LexicalTokenType ScanDecimalNumberAfterDecimalPoint()
    {
        const char& next = GetNextChar();

        if ('0' <= next && next <= '9')
        {
            m_value_buffer.push_back(next);
            ScanDecimalNumber();
            return LexicalTokenType::FLOAT_NUMBER_VALUE;
        }

        throw Exception("Invalid character found after a decimal point.");
    }

    /**
     * @brief Scan a next character after exponent(e/E).
     *
     * @return LexicalTokenType The lexical token type for float numbers.
     */
    LexicalTokenType ScanDecimalNumberAfterExponent()
    {
        const char& next = GetNextChar();
        if (next == '+' || next == '-')
        {
            m_value_buffer.push_back(next);
            ScanDecimalNumberAfterSign();
        }
        else if ('0' <= next && next <= '9')
        {
            m_value_buffer.push_back(next);
            ScanDecimalNumber();
        }
        return LexicalTokenType::FLOAT_NUMBER_VALUE;
    }

    /**
     * @brief Scan a next character after a sign(+/-) after exponent(e/E).
     *
     * @return LexicalTokenType The lexical token type for one of number types(signed/unsigned/float)
     */
    LexicalTokenType ScanDecimalNumberAfterSign()
    {
        const char& next = GetNextChar();
        if ('0' <= next && next <= '9')
        {
            m_value_buffer.push_back(next);
            return ScanDecimalNumber();
        }
        throw Exception("Non-numeric character found after a sign(+/-) after exponent(e/E).");
    }

    /**
     * @brief Scan a next character for decimal numbers.
     *
     * @return LexicalTokenType The lexical token type for one of number types(signed/unsigned/float)
     */
    LexicalTokenType ScanDecimalNumber()
    {
        const char& next = GetNextChar();

        if ('0' <= next && next <= '9')
        {
            m_value_buffer.push_back(next);
            return ScanDecimalNumber();
        }

        if (next == '.')
        {
            if (m_value_buffer.find(next) != std::string::npos)
            {
                throw Exception("Multiple decimal points found in a token.");
            }
            m_value_buffer.push_back(next);
            return ScanDecimalNumberAfterDecimalPoint();
        }

        if (next == 'e' || next == 'E')
        {
            m_value_buffer.push_back(next);
            return ScanDecimalNumberAfterExponent();
        }

        return LexicalTokenType::UNSIGNED_INT_VALUE;
    }

    /**
     * @brief Scan a next character for octal numbers.
     * @note All octal numbers are interpreted as unsigned integers.
     *
     * @return LexicalTokenType The lexical token type for unsigned numbers.
     */
    LexicalTokenType ScanOctalNumber()
    {
        const char& next = GetNextChar();
        if ('0' <= next && next <= '7')
        {
            m_value_buffer.push_back(next);
            ScanOctalNumber();
            return LexicalTokenType::UNSIGNED_INT_VALUE;
        }
        throw Exception("Invalid character found in a token with the octal number prefix(0o).");
    }

    /**
     * @brief Scan a next character for hexadecimal numbers.
     * @note All hexadecimal numbers are interpreted as unsigned integers.
     *
     * @return LexicalTokenType The lexical token type for unsigned numbers.
     */
    LexicalTokenType ScanHexadecimalNumber()
    {
        const char& next = GetNextChar();
        if (('0' <= next && next <= '9') || ('A' <= next && next <= 'F') || ('a' <= next && next <= 'f'))
        {
            m_value_buffer.push_back(next);
            ScanHexadecimalNumber();
            return LexicalTokenType::UNSIGNED_INT_VALUE;
        }
        throw Exception("Invalid character found in a token with the hexadecimal number prefix(0x).");
    }

    /**
     * @brief Get reference to the current character from the input buffer without position updates.
     *
     * @return const char& Constant reference to the current character.
     */
    const char& RefCurrentChar() const noexcept
    {
        return m_input_buffer[m_position_info.total_read_char_counts];
    }

    /**
     * @brief Get reference to the next character from the input buffer without position updates.
     *
     * @return const char& Constant reference to the next character.
     */
    const char& RefNextChar() const noexcept
    {
        return m_input_buffer[m_position_info.total_read_char_counts + 1];
    }

    /**
     * @brief Get reference to the next character from the input buffer with position updates.
     *
     * @return const char& Constant reference to the next character.
     */
    const char& GetNextChar() noexcept
    {
        const char& current = m_input_buffer[++m_position_info.total_read_char_counts];
        ++m_position_info.read_char_counts_in_line;
        if (current == '\n')
        {
            ++m_position_info.total_read_line_counts;
            m_position_info.prev_char_counts_in_line = m_position_info.read_char_counts_in_line;
            m_position_info.read_char_counts_in_line = 0;
        }
        return current;
    }

    /**
     * @brief Skip white spaces, tabs and newline codes until any other kind of character is found.
     */
    void SkipWhiteSpaces()
    {
        while (true)
        {
            switch (GetNextChar())
            {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                break;
            default:
                return;
            }
        }
    }

private:
    //!< The value of EOF.
    static constexpr char_int_type s_eof = char_traits_type::eof();

    //!< An input buffer to be analyzed.
    std::string m_input_buffer {};
    //!< The information set for the input buffer.
    Position m_position_info {};
    //!< The current indent width.
    size_t m_current_indent_width = 0;
    //!< A temporal buffer to store a string to be parsed to an actual datum.
    std::string m_value_buffer {};
};

} // namespace fkyaml

#endif /* FK_YAML_LEXICAL_ANALIZER_HPP_ */