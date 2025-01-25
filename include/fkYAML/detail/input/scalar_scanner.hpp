//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_INPUT_SCALAR_SCANNER_HPP
#define FK_YAML_DETAIL_INPUT_SCALAR_SCANNER_HPP

#include <cstring>
#include <string>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/assert.hpp>
#include <fkYAML/node_type.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief The class which detects a scalar value type by scanning contents.
class scalar_scanner {
public:
    /// @brief Detects a scalar value type by scanning the contents ranged by the given iterators.
    /// @param begin The iterator to the first element of the scalar.
    /// @param end The iterator to the past-the-end element of the scalar.
    /// @return A detected scalar value type.
    static node_type scan(const char* begin, const char* end) noexcept {
        if (begin == end) {
            return node_type::STRING;
        }

        const auto len = static_cast<uint32_t>(std::distance(begin, end));
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
                const bool is_inf_or_nan_scalar =
                    (std::strncmp(p_from_second, "inf", 3) == 0) || (std::strncmp(p_from_second, "Inf", 3) == 0) ||
                    (std::strncmp(p_from_second, "INF", 3) == 0) || (std::strncmp(p_from_second, "nan", 3) == 0) ||
                    (std::strncmp(p_from_second, "NaN", 3) == 0) || (std::strncmp(p_from_second, "NAN", 3) == 0);
                if (is_inf_or_nan_scalar) {
                    return node_type::FLOAT;
                }
                // maybe a number.
                break;
            }
            default:
                break;
            }
            break;
        case 5:
            switch (*p_begin) {
            case 'f':
                // no possible case of being a number otherwise.
                return (std::strncmp(p_begin + 1, "alse", 4) == 0) ? node_type::BOOLEAN : node_type::STRING;
            case 'F':
                // no possible case of being a number otherwise.
                return ((std::strncmp(p_begin + 1, "alse", 4) == 0) || (std::strncmp(p_begin + 1, "ALSE", 4) == 0))
                           ? node_type::BOOLEAN
                           : node_type::STRING;
            case '+':
            case '-':
                if (*(p_begin + 1) == '.') {
                    const char* p_from_third = p_begin + 2;
                    const bool is_min_inf = (std::strncmp(p_from_third, "inf", 3) == 0) ||
                                            (std::strncmp(p_from_third, "Inf", 3) == 0) ||
                                            (std::strncmp(p_from_third, "INF", 3) == 0);
                    if (is_min_inf) {
                        return node_type::FLOAT;
                    }
                }
                // maybe a number.
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }

        return scan_possible_number_token(begin, len);
    }

private:
    /// @brief Detects a scalar value type from the contents (possibly an integer or a floating-point value).
    /// @param itr The iterator to the first element of the scalar.
    /// @param len The length of the scalar contents.
    /// @return A detected scalar value type.
    static node_type scan_possible_number_token(const char* itr, uint32_t len) noexcept {
        FK_YAML_ASSERT(len > 0);

        switch (*itr) {
        case '-':
            return (len > 1) ? scan_negative_number(++itr, --len) : node_type::STRING;
        case '+':
            return (len > 1) ? scan_decimal_number(++itr, --len) : node_type::STRING;
        case '.':
            // some integer(s) required after the decimal point as a floating point value.
            return (len > 1) ? scan_after_decimal_point(++itr, --len) : node_type::STRING;
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
            return (len > 1) ? scan_decimal_number(++itr, --len) : node_type::INTEGER;
        default:
            return node_type::STRING;
        }
    }

    /// @brief Detects a scalar value type by scanning the contents right after the negative sign.
    /// @param itr The iterator to the past-the-negative-sign element of the scalar.
    /// @param len The length of the scalar contents left unscanned.
    /// @return A detected scalar value type.
    static node_type scan_negative_number(const char* itr, uint32_t len) noexcept {
        FK_YAML_ASSERT(len > 0);

        if (is_digit(*itr)) {
            return (len > 1) ? scan_decimal_number(++itr, --len) : node_type::INTEGER;
        }

        if (*itr == '.') {
            // some integer(s) required after "-." as a floating point value.
            return (len > 1) ? scan_after_decimal_point(++itr, --len) : node_type::STRING;
        }

        return node_type::STRING;
    }

    /// @brief Detects a scalar value type by scanning the contents right after the beginning 0.
    /// @param itr The iterator to the past-the-zero element of the scalar.
    /// @param len The length of the scalar left unscanned.
    /// @return A detected scalar value type.
    static node_type scan_after_zero_at_first(const char* itr, uint32_t len) noexcept {
        FK_YAML_ASSERT(len > 0);

        if (is_digit(*itr)) {
            // a token consisting of the beginning '0' and some following numbers, e.g., `0123`, is not an integer
            // according to https://yaml.org/spec/1.2.2/#10213-integer.
            return node_type::STRING;
        }

        switch (*itr) {
        case '.':
            // 0 can be omitted after `0.`.
            return (len > 1) ? scan_after_decimal_point(++itr, --len) : node_type::FLOAT;
        case 'e':
        case 'E':
            // some integer(s) required after the exponent sign as a floating point value.
            return (len > 1) ? scan_after_exponent(++itr, --len) : node_type::STRING;
        case 'o':
            return (len > 1) ? scan_octal_number(++itr, --len) : node_type::STRING;
        case 'x':
            return (len > 1) ? scan_hexadecimal_number(++itr, --len) : node_type::STRING;
        default:
            return node_type::STRING;
        }
    }

    /// @brief Detects a scalar value type by scanning the contents part starting with a decimal.
    /// @param itr The iterator to the beginning decimal element of the scalar.
    /// @param len The length of the scalar left unscanned.
    /// @return A detected scalar value type.
    static node_type scan_decimal_number(const char* itr, uint32_t len) noexcept {
        FK_YAML_ASSERT(len > 0);

        if (is_digit(*itr)) {
            return (len > 1) ? scan_decimal_number(++itr, --len) : node_type::INTEGER;
        }

        switch (*itr) {
        case '.': {
            // 0 can be omitted after the decimal point
            return (len > 1) ? scan_after_decimal_point(++itr, --len) : node_type::FLOAT;
        }
        case 'e':
        case 'E':
            // some integer(s) required after the exponent
            return (len > 1) ? scan_after_exponent(++itr, --len) : node_type::STRING;
        default:
            return node_type::STRING;
        }
    }

    /// @brief Detects a scalar value type by scanning the contents right after a decimal point.
    /// @param itr The iterator to the past-the-decimal-point element of the scalar.
    /// @param len The length of the scalar left unscanned.
    /// @return A detected scalar value type.
    static node_type scan_after_decimal_point(const char* itr, uint32_t len) noexcept {
        FK_YAML_ASSERT(len > 0);

        for (uint32_t i = 0; i < len; i++) {
            const char c = *itr++;

            if (is_digit(c)) {
                continue;
            }

            if (c == 'e' || c == 'E') {
                if (i == len - 1) {
                    // some integer(s) required after the exponent
                    return node_type::STRING;
                }
                return scan_after_exponent(itr, len - i - 1);
            }

            return node_type::STRING;
        }

        return node_type::FLOAT;
    }

    /// @brief Detects a scalar value type by scanning the contents right after the exponent prefix ("e" or "E").
    /// @param itr The iterator to the past-the-exponent-prefix element of the scalar.
    /// @param len The length of the scalar left unscanned.
    /// @return A detected scalar value type.
    static node_type scan_after_exponent(const char* itr, uint32_t len) noexcept {
        FK_YAML_ASSERT(len > 0);

        const char c = *itr;
        if (c == '+' || c == '-') {
            if (len == 1) {
                // some integer(s) required after the sign.
                return node_type::STRING;
            }
            ++itr;
            --len;
        }

        for (uint32_t i = 0; i < len; i++) {
            if (!is_digit(*itr++)) {
                return node_type::STRING;
            }
        }

        return node_type::FLOAT;
    }

    /// @brief Detects a scalar value type by scanning the contents assuming octal numbers.
    /// @param itr The iterator to the octal-number element of the scalar.
    /// @param len The length of the scalar left unscanned.
    /// @return A detected scalar value type.
    static node_type scan_octal_number(const char* itr, uint32_t len) noexcept {
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

    /// @brief Detects a scalar value type by scanning the contents assuming hexadecimal numbers.
    /// @param itr The iterator to the hexadecimal-number element of the scalar.
    /// @param len The length of the scalar left unscanned.
    /// @return A detected scalar value type.
    static node_type scan_hexadecimal_number(const char* itr, uint32_t len) noexcept {
        FK_YAML_ASSERT(len > 0);

        if (is_xdigit(*itr)) {
            return (len > 1) ? scan_hexadecimal_number(++itr, --len) : node_type::INTEGER;
        }
        return node_type::STRING;
    }

    /// @brief Check if the given character is a digit.
    /// @note This function is needed to avoid assertion failures in `std::isdigit()` especially when compiled with
    /// MSVC.
    /// @param c A character to be checked.
    /// @return true if the given character is a digit, false otherwise.
    static bool is_digit(char c) {
        return ('0' <= c && c <= '9');
    }

    /// @brief Check if the given character is a hex-digit.
    /// @note This function is needed to avoid assertion failures in `std::isxdigit()` especially when compiled with
    /// MSVC.
    /// @param c A character to be checked.
    /// @return true if the given character is a hex-digit, false otherwise.
    static bool is_xdigit(char c) {
        return (('0' <= c && c <= '9') || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f'));
    }
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_SCALAR_SCANNER_HPP */
