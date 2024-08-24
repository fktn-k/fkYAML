///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_INPUT_SCALAR_SCANNER_HPP_
#define FK_YAML_DETAIL_INPUT_SCALAR_SCANNER_HPP_

#include <algorithm>
#include <string>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/assert.hpp>
#include <fkYAML/detail/types/lexical_token_t.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

namespace {

/// @brief Check if the given character is a digit.
/// @note This function is needed to avoid assertion failures in `std::isdigit()` especially when compiled with MSVC.
/// @param c A character to be checked.
/// @return true if the given character is a digit, false otherwise.
inline bool is_digit(char c) {
    return ('0' <= c && c <= '9');
}

/// @brief Check if the given character is a hex-digit.
/// @note This function is needed to avoid assertion failures in `std::isxdigit()` especially when compiled with MSVC.
/// @param c A character to be checked.
/// @return true if the given character is a hex-digit, false otherwise.
inline bool is_xdigit(char c) {
    return (('0' <= c && c <= '9') || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f'));
}

} // namespace
class scalar_scanner {
public:
    static lexical_token_t scan(const std::string& token) {
        switch (token.size()) {
        case 0:
            return lexical_token_t::STRING_VALUE;
        case 1:
            if (token[0] == '~') {
                return lexical_token_t::NULL_VALUE;
            }
            break;
        case 4:
            switch (token[0]) {
            case 'n':
            case 'N':
                if (token == "null" || token == "Null" || token == "NULL") {
                    return lexical_token_t::NULL_VALUE;
                }
                break;
            case 't':
            case 'T':
                if (token == "true" || token == "True" || token == "TRUE") {
                    return lexical_token_t::BOOLEAN_VALUE;
                }
                break;
            case '.':
                if (token == ".inf" || token == ".Inf" || token == ".INF" || token == ".nan" || token == ".NaN" ||
                    token == ".NAN") {
                    return lexical_token_t::FLOAT_NUMBER_VALUE;
                }
                break;
            }
            break;
        case 5:
            switch (token[0]) {
            case 'f':
            case 'F':
                if (token == "false" || token == "False" || token == "FALSE") {
                    return lexical_token_t::BOOLEAN_VALUE;
                }
                break;
            case '-':
                if (token[1] == '.' && (token == "-.inf" || token == "-.Inf" || token == "-.INF")) {
                    return lexical_token_t::FLOAT_NUMBER_VALUE;
                }
                break;
            }
            break;
        }

        return scan_possible_number_token(token);
    }

private:
    static lexical_token_t scan_possible_number_token(const std::string& token) {
        std::string::const_iterator itr = token.begin();
        std::size_t size = token.size();
        FK_YAML_ASSERT(size > 0);

        switch (*itr) {
        case '-':
            return (size > 1) ? scan_negative_number(++itr, --size) : lexical_token_t::STRING_VALUE;
        case '+':
            return (size > 1) ? scan_decimal_number(++itr, --size, false) : lexical_token_t::STRING_VALUE;
        case '0':
            return (size > 1) ? scan_after_zero_at_first(++itr, --size) : lexical_token_t::INTEGER_VALUE;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return (size > 1) ? scan_decimal_number(++itr, --size, false) : lexical_token_t::INTEGER_VALUE;
        default:
            return lexical_token_t::STRING_VALUE;
        }
    }

    static lexical_token_t scan_negative_number(std::string::const_iterator itr, std::size_t size) {
        FK_YAML_ASSERT(size > 0);

        if (is_digit(*itr)) {
            return (size > 1) ? scan_decimal_number(++itr, --size, false) : lexical_token_t::INTEGER_VALUE;
        }

        return lexical_token_t::STRING_VALUE;
    }

    static lexical_token_t scan_after_zero_at_first(std::string::const_iterator itr, std::size_t size) {
        FK_YAML_ASSERT(size > 0);

        if (is_digit(*itr)) {
            // a token consisting of the beginning '0' and some following numbers, e.g., `0123`, is not an integer
            // according to https://yaml.org/spec/1.2.2/#10213-integer.
            return lexical_token_t::STRING_VALUE;
        }

        switch (*itr) {
        case '.': {
            if (size == 1) {
                // 0 is omitted after `0.`.
                return lexical_token_t::FLOAT_NUMBER_VALUE;
            }
            lexical_token_t ret = scan_after_decimal_point(++itr, --size, true);
            return (ret == lexical_token_t::STRING_VALUE) ? lexical_token_t::STRING_VALUE
                                                          : lexical_token_t::FLOAT_NUMBER_VALUE;
        }
        case 'o':
            return (size > 1) ? scan_octal_number(++itr, --size) : lexical_token_t::STRING_VALUE;
        case 'x':
            return (size > 1) ? scan_hexadecimal_number(++itr, --size) : lexical_token_t::STRING_VALUE;
        default:
            return lexical_token_t::STRING_VALUE;
        }
    }

    static lexical_token_t scan_decimal_number(
        std::string::const_iterator itr, std::size_t size, bool has_decimal_point) {
        FK_YAML_ASSERT(size > 0);

        if (is_digit(*itr)) {
            return (size > 1) ? scan_decimal_number(++itr, --size, has_decimal_point) : lexical_token_t::INTEGER_VALUE;
        }

        switch (*itr) {
        case '.': {
            if (has_decimal_point) {
                // the token has more than one period, e.g., a semantic version `1.2.3`.
                return lexical_token_t::STRING_VALUE;
            }
            if (size == 1) {
                // 0 is omitted after the decimal point
                return lexical_token_t::FLOAT_NUMBER_VALUE;
            }
            lexical_token_t ret = scan_after_decimal_point(++itr, --size, true);
            return (ret == lexical_token_t::STRING_VALUE) ? lexical_token_t::STRING_VALUE
                                                          : lexical_token_t::FLOAT_NUMBER_VALUE;
        }
        case 'e':
        case 'E':
            return (size > 1) ? scan_after_exponent(++itr, --size, has_decimal_point) : lexical_token_t::STRING_VALUE;
        default:
            return lexical_token_t::STRING_VALUE;
        }
    }

    static lexical_token_t scan_after_decimal_point(
        std::string::const_iterator itr, std::size_t size, bool has_decimal_point) {
        FK_YAML_ASSERT(size > 0);

        if (is_digit(*itr)) {
            return (size > 1) ? scan_decimal_number(++itr, --size, has_decimal_point)
                              : lexical_token_t::FLOAT_NUMBER_VALUE;
        }

        return lexical_token_t::STRING_VALUE;
    }

    static lexical_token_t scan_after_exponent(
        std::string::const_iterator itr, std::size_t size, bool has_decimal_point) {
        FK_YAML_ASSERT(size > 0);

        if (is_digit(*itr)) {
            return (size > 1) ? scan_decimal_number(++itr, --size, has_decimal_point)
                              : lexical_token_t::FLOAT_NUMBER_VALUE;
        }

        switch (*itr) {
        case '+':
        case '-':
            return (size > 1) ? scan_decimal_number(++itr, --size, has_decimal_point) : lexical_token_t::STRING_VALUE;
        default:
            return lexical_token_t::STRING_VALUE;
        }
    }

    static lexical_token_t scan_octal_number(std::string::const_iterator itr, std::size_t size) {
        FK_YAML_ASSERT(size > 0);

        switch (*itr) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            return (size > 1) ? scan_octal_number(++itr, --size) : lexical_token_t::INTEGER_VALUE;
        default:
            return lexical_token_t::STRING_VALUE;
        }
    }

    static lexical_token_t scan_hexadecimal_number(std::string::const_iterator itr, std::size_t size) {
        FK_YAML_ASSERT(size > 0);

        if (is_xdigit(*itr)) {
            return (size > 1) ? scan_hexadecimal_number(++itr, --size) : lexical_token_t::INTEGER_VALUE;
        }
        return lexical_token_t::STRING_VALUE;
    }
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_SCALAR_SCANNER_HPP_ */
