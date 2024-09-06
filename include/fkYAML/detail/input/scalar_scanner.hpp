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

#include <cstring>
#include <string>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/assert.hpp>
#include <fkYAML/node_type.hpp>

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
    static node_type scan(std::string::const_iterator begin, std::string::const_iterator end) {
        if (begin == end) {
            return node_type::STRING;
        }

        uint32_t len = static_cast<uint32_t>(std::distance(begin, end));
        if (len > 5) {
            return scan_possible_number_token(begin, len);
        }

        const char* p_begin = &*begin;

        switch (len) {
        case 1:
            if (*p_begin == '~') {
                return node_type::NULL_OBJECT;
            }
            break;
        case 4:
            switch (*p_begin) {
            case 'n':
                // no possible case of begin a number otherwise.
                return (std::strncmp(p_begin + 1, "ull", 3) == 0) ? node_type::NULL_OBJECT : node_type::STRING;
            case 'N':
                // no possible case of begin a number otherwise.
                return ((std::strncmp(p_begin + 1, "ull", 3) == 0) || (std::strncmp(p_begin + 1, "ULL", 3) == 0))
                           ? node_type::NULL_OBJECT
                           : node_type::STRING;
            case 't':
                // no possible case of being a number otherwise.
                return (std::strncmp(p_begin + 1, "rue", 3) == 0) ? node_type::BOOLEAN : node_type::STRING;
            case 'T':
                // no possible case of being a number otherwise.
                return ((std::strncmp(p_begin + 1, "rue", 3) == 0) || (std::strncmp(p_begin + 1, "RUE", 3) == 0))
                           ? node_type::BOOLEAN
                           : node_type::STRING;
            case '.': {
                const char* p_from_second = p_begin + 1;
                bool is_inf_or_nan_scalar =
                    (std::strncmp(p_from_second, "inf", 3) == 0) || (std::strncmp(p_from_second, "Inf", 3) == 0) ||
                    (std::strncmp(p_from_second, "INF", 3) == 0) || (std::strncmp(p_from_second, "nan", 3) == 0) ||
                    (std::strncmp(p_from_second, "NaN", 3) == 0) || (std::strncmp(p_from_second, "NAN", 3) == 0);
                if (is_inf_or_nan_scalar) {
                    return node_type::FLOAT;
                }
                break;
            }
            }
            break;
        case 5:
            switch (*p_begin) {
            case 'f':
                if (std::strncmp(p_begin + 1, "alse", 4) == 0) {
                    return node_type::BOOLEAN;
                }
                break;
            case 'F': {
                bool is_false_scalar =
                    (std::strncmp(p_begin + 1, "alse", 4) == 0) || (std::strncmp(p_begin + 1, "ALSE", 4) == 0);
                if (is_false_scalar) {
                    return node_type::BOOLEAN;
                }
                break;
            }
            case '-':
                if (*(p_begin + 1) == '.') {
                    const char* p_from_third = p_begin + 2;
                    bool is_min_inf_scalar = (std::strncmp(p_from_third, "inf", 3) == 0) ||
                                             (std::strncmp(p_from_third, "Inf", 3) == 0) ||
                                             (std::strncmp(p_from_third, "INF", 3) == 0);
                    if (is_min_inf_scalar) {
                        return node_type::FLOAT;
                    }
                }
                break;
            }
            break;
        }

        return scan_possible_number_token(begin, len);
    }

private:
    static node_type scan_possible_number_token(std::string::const_iterator itr, uint32_t len) {
        FK_YAML_ASSERT(len > 0);

        switch (*itr) {
        case '-':
            return (len > 1) ? scan_negative_number(++itr, --len) : node_type::STRING;
        case '+':
            return (len > 1) ? scan_decimal_number(++itr, --len, false) : node_type::STRING;
        case '0':
            return (len > 1) ? scan_after_zero_at_first(++itr, --len) : node_type::INTEGER;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return (len > 1) ? scan_decimal_number(++itr, --len, false) : node_type::INTEGER;
        default:
            return node_type::STRING;
        }
    }

    static node_type scan_negative_number(std::string::const_iterator itr, uint32_t len) {
        FK_YAML_ASSERT(len > 0);

        if (is_digit(*itr)) {
            return (len > 1) ? scan_decimal_number(++itr, --len, false) : node_type::INTEGER;
        }

        return node_type::STRING;
    }

    static node_type scan_after_zero_at_first(std::string::const_iterator itr, uint32_t len) {
        FK_YAML_ASSERT(len > 0);

        if (is_digit(*itr)) {
            // a token consisting of the beginning '0' and some following numbers, e.g., `0123`, is not an integer
            // according to https://yaml.org/spec/1.2.2/#10213-integer.
            return node_type::STRING;
        }

        switch (*itr) {
        case '.': {
            if (len == 1) {
                // 0 is omitted after `0.`.
                return node_type::FLOAT;
            }
            node_type ret = scan_after_decimal_point(++itr, --len, true);
            return (ret == node_type::STRING) ? node_type::STRING : node_type::FLOAT;
        }
        case 'o':
            return (len > 1) ? scan_octal_number(++itr, --len) : node_type::STRING;
        case 'x':
            return (len > 1) ? scan_hexadecimal_number(++itr, --len) : node_type::STRING;
        default:
            return node_type::STRING;
        }
    }

    static node_type scan_decimal_number(std::string::const_iterator itr, uint32_t len, bool has_decimal_point) {
        FK_YAML_ASSERT(len > 0);

        if (is_digit(*itr)) {
            return (len > 1) ? scan_decimal_number(++itr, --len, has_decimal_point) : node_type::INTEGER;
        }

        switch (*itr) {
        case '.': {
            if (has_decimal_point) {
                // the token has more than one period, e.g., a semantic version `1.2.3`.
                return node_type::STRING;
            }
            if (len == 1) {
                // 0 is omitted after the decimal point
                return node_type::FLOAT;
            }
            node_type ret = scan_after_decimal_point(++itr, --len, true);
            return (ret == node_type::STRING) ? node_type::STRING : node_type::FLOAT;
        }
        case 'e':
        case 'E':
            return (len > 1) ? scan_after_exponent(++itr, --len, has_decimal_point) : node_type::STRING;
        default:
            return node_type::STRING;
        }
    }

    static node_type scan_after_decimal_point(std::string::const_iterator itr, uint32_t len, bool has_decimal_point) {
        FK_YAML_ASSERT(len > 0);

        if (is_digit(*itr)) {
            return (len > 1) ? scan_decimal_number(++itr, --len, has_decimal_point) : node_type::FLOAT;
        }

        return node_type::STRING;
    }

    static node_type scan_after_exponent(std::string::const_iterator itr, uint32_t len, bool has_decimal_point) {
        FK_YAML_ASSERT(len > 0);

        if (is_digit(*itr)) {
            return (len > 1) ? scan_decimal_number(++itr, --len, has_decimal_point) : node_type::FLOAT;
        }

        switch (*itr) {
        case '+':
        case '-':
            return (len > 1) ? scan_decimal_number(++itr, --len, has_decimal_point) : node_type::STRING;
        default:
            return node_type::STRING;
        }
    }

    static node_type scan_octal_number(std::string::const_iterator itr, uint32_t len) {
        FK_YAML_ASSERT(len > 0);

        switch (*itr) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            return (len > 1) ? scan_octal_number(++itr, --len) : node_type::INTEGER;
        default:
            return node_type::STRING;
        }
    }

    static node_type scan_hexadecimal_number(std::string::const_iterator itr, uint32_t len) {
        FK_YAML_ASSERT(len > 0);

        if (is_xdigit(*itr)) {
            return (len > 1) ? scan_hexadecimal_number(++itr, --len) : node_type::INTEGER;
        }
        return node_type::STRING;
    }
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_SCALAR_SCANNER_HPP_ */
