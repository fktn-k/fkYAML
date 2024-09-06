///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

// **NOTE FOR LIBARARY DEVELOPERS**:
// Implementations in this header file are intentionally optimized for conversions between YAML scalars and native C++
// types. So, some implementations don't follow the convensions in the standard C++ functions. For example, octals must
// begin with "0o" (not "0"), which is specified in the YAML spec 1.2.

#ifndef FK_YAML_CONVERSIONS_SCALAR_CONV_HPP_
#define FK_YAML_CONVERSIONS_SCALAR_CONV_HPP_

#include <cmath>
#include <cstring>
#include <limits>
#include <string>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

template <std::size_t NumBytes>
struct conv_limits_base {};

template <>
struct conv_limits_base<1> {
    /// max characters for octals (0o377) without the prefix part.
    static constexpr std::size_t max_chars_oct = 3;
    /// max characters for hexadecimals (0xFF) without the prefix part.
    static constexpr std::size_t max_chars_hex = 2;

    static bool check_if_octs_safe(const char* octs, std::size_t len) noexcept {
        return (len < max_chars_oct) || (len == max_chars_oct && octs[0] <= '3');
    }

    static bool check_if_hexs_safe(const char* /*unused*/, std::size_t len) noexcept {
        return len <= max_chars_hex;
    }
};

template <>
struct conv_limits_base<2> {
    /// max characters for octals (0o177777) without the prefix part.
    static constexpr std::size_t max_chars_oct = 6;
    /// max characters for hexadecimals (0xFFFF) without the prefix part.
    static constexpr std::size_t max_chars_hex = 4;

    static bool check_if_octs_safe(const char* octs, std::size_t len) noexcept {
        return (len < max_chars_oct) || (len == max_chars_oct && octs[0] <= '1');
    }

    static bool check_if_hexs_safe(const char* /*unused*/, std::size_t len) noexcept {
        return len <= max_chars_hex;
    }
};

template <>
struct conv_limits_base<4> {
    /// max characters for octals (0o37777777777) without the prefix part.
    static constexpr std::size_t max_chars_oct = 11;
    /// max characters for hexadecimals (0xFFFFFFFF) without the prefix part.
    static constexpr std::size_t max_chars_hex = 8;

    static bool check_if_octs_safe(const char* octs, std::size_t len) noexcept {
        return (len < max_chars_oct) || (len == max_chars_oct && octs[0] <= '3');
    }

    static bool check_if_hexs_safe(const char* /*unused*/, std::size_t len) noexcept {
        return len <= max_chars_hex;
    }
};

template <>
struct conv_limits_base<8> {
    /// max characters for octals (0o1777777777777777777777) without the prefix part.
    static constexpr std::size_t max_chars_oct = 22;
    /// max characters for hexadecimals (0xFFFFFFFFFFFFFFFF) without the prefix part.
    static constexpr std::size_t max_chars_hex = 16;

    static bool check_if_octs_safe(const char* octs, std::size_t len) noexcept {
        return (len < max_chars_oct) || (len == max_chars_oct && octs[0] <= '1');
    }

    static bool check_if_hexs_safe(const char* /*unused*/, std::size_t len) noexcept {
        return len <= max_chars_hex;
    }
};

template <std::size_t NumBytes, bool IsSigned>
struct conv_limits {};

template <>
struct conv_limits<1, true> : conv_limits_base<1> {
    /// with or without sign.
    static constexpr bool is_signed = true;

    /// max characters for decimals (-128..127) without sign.
    static constexpr std::size_t max_chars_dec = 3;

    /// string representation of max decimal value.
    static constexpr char max_value_chars_dec[] = "127";
    /// string representation of min decimal value without sign.
    static constexpr char min_value_chars_dec[] = "128";
};

template <>
struct conv_limits<1, false> : conv_limits_base<1> {
    /// with or without sign.
    static constexpr bool is_signed = false;

    /// max characters for decimals (0..255) without sign.
    static constexpr std::size_t max_chars_dec = 3;

    /// string representation of max decimal value.
    static constexpr char max_value_chars_dec[] = "255";
    /// string representation of min decimal value.
    static constexpr char min_value_chars_dec[] = "0";
};

template <>
struct conv_limits<2, true> : conv_limits_base<2> {
    /// with or without sign.
    static constexpr bool is_signed = true;

    /// max characters for decimals (-32768..32767) without sign.
    static constexpr std::size_t max_chars_dec = 5;

    /// string representation of max decimal value.
    static constexpr char max_value_chars_dec[] = "32767";
    /// string representation of min decimal value without sign.
    static constexpr char min_value_chars_dec[] = "32768";
};

template <>
struct conv_limits<2, false> : conv_limits_base<2> {
    /// with or without sign.
    static constexpr bool is_signed = false;

    /// max characters for decimals (0..65535) without sign.
    static constexpr std::size_t max_chars_dec = 5;

    /// string representation of max decimal value.
    static constexpr char max_value_chars_dec[] = "65535";
    /// string representation of min decimal value.
    static constexpr char min_value_chars_dec[] = "0";
};

template <>
struct conv_limits<4, true> : conv_limits_base<4> {
    /// with or without sign.
    static constexpr bool is_signed = true;

    /// max characters for decimals (-2147483648..2147483647) without sign.
    static constexpr std::size_t max_chars_dec = 10;

    /// string representation of max decimal value.
    static constexpr char max_value_chars_dec[] = "2147483647";
    /// string representation of min decimal value without sign.
    static constexpr char min_value_chars_dec[] = "2147483648";
};

template <>
struct conv_limits<4, false> : conv_limits_base<4> {
    /// with or without sign.
    static constexpr bool is_signed = false;

    /// max characters for decimals (0..4294967295) without sign.
    static constexpr std::size_t max_chars_dec = 10;

    /// string representation of max decimal value.
    static constexpr char max_value_chars_dec[] = "4294967295";
    /// string representation of min decimal value.
    static constexpr char min_value_chars_dec[] = "0";
};

template <>
struct conv_limits<8, true> : conv_limits_base<8> {
    /// with or without sign.
    static constexpr bool is_signed = true;

    /// max characters for decimals (-9223372036854775808..9223372036854775807) without sign.
    static constexpr std::size_t max_chars_dec = 19;

    /// string representation of max decimal value.
    static constexpr char max_value_chars_dec[] = "9223372036854775807";
    /// string representation of min decimal value without sign.
    static constexpr char min_value_chars_dec[] = "9223372036854775808";
};

template <>
struct conv_limits<8, false> : conv_limits_base<8> {
    /// with or without sign.
    static constexpr bool is_signed = false;

    /// max characters for decimals (0..18446744073709551615) without sign.
    static constexpr std::size_t max_chars_dec = 20;

    /// string representation of max decimal value.
    static constexpr char max_value_chars_dec[] = "18446744073709551615";
    /// string representation of min decimal value.
    static constexpr char min_value_chars_dec[] = "0";
};

template <typename CharItr>
inline bool aton(CharItr begin, CharItr end, std::nullptr_t& /*unused*/) noexcept {
    static_assert(is_iterator_of<CharItr, char>::value, "atoi_dec() accepts iterators for char type");

    if (begin == end) {
        return false;
    }

    uint32_t len = static_cast<uint32_t>(std::distance(begin, end));

    // This path is the most probable case, so check it first.
    if (len == 4) {
        const char* p_begin = &*begin;
        return (std::strncmp(p_begin, "null", 4) == 0) || (std::strncmp(p_begin, "Null", 4) == 0) ||
               (std::strncmp(p_begin, "NULL", 4) == 0);
    }

    if (len == 1) {
        return *begin == '~';
    }

    return false;
}

template <typename CharItr, typename BoolType>
inline bool atob(CharItr begin, CharItr end, BoolType& boolean) noexcept {
    static_assert(is_iterator_of<CharItr, char>::value, "atoi_dec() accepts iterators for char type");

    if (begin == end) {
        return false;
    }

    uint32_t len = static_cast<uint32_t>(std::distance(begin, end));
    const char* p_begin = &*begin;

    if (len == 4) {
        bool is_true_scalar = (std::strncmp(p_begin, "true", 4) == 0) || (std::strncmp(p_begin, "True", 4) == 0) ||
                              (std::strncmp(p_begin, "TRUE", 4) == 0);

        if (is_true_scalar) {
            boolean = true;
        }
        return is_true_scalar;
    }

    if (len == 5) {
        bool is_false_scalar = (std::strncmp(p_begin, "false", 5) == 0) || (std::strncmp(p_begin, "False", 5) == 0) ||
                               (std::strncmp(p_begin, "FALSE", 5) == 0);

        if (is_false_scalar) {
            boolean = false;
        }
        return is_false_scalar;
    }

    return false;
}

template <typename IntType>
inline bool atoi_dec_common(const char* p_begin, const char* p_end, IntType& i) noexcept {
    static_assert(
        is_non_bool_integral<IntType>::value, "atoi_dec() accepts non-boolean integral types as an output type");

    i = 0;
    do {
        char c = *p_begin;
        if (c < '0' || '9' < c) {
            return false;
        }
        i = i * IntType(10) + IntType(c - '0');
    } while (++p_begin != p_end);

    return true;
}

template <typename IntType>
inline bool atoi_dec_pos(const char* p_begin, const char* p_end, IntType& i) noexcept {
    static_assert(
        is_non_bool_integral<IntType>::value, "atoi_dec() accepts non-boolean integral types as an output type");

    if (p_begin == p_end) {
        return false;
    }

    using conv_limits_type = conv_limits<sizeof(IntType), std::is_signed<IntType>::value>;

    std::size_t len = static_cast<std::size_t>(p_end - p_begin);
    if (len > conv_limits_type::max_chars_dec) {
        // Overflow will happen.
        return false;
    }

    if (len == conv_limits_type::max_chars_dec) {
        for (std::size_t i = 0; i < conv_limits_type::max_chars_dec; i++) {
            if (p_begin[i] < conv_limits_type::max_value_chars_dec[i]) {
                // No need to check the lower digits. Overflow will no longer happen.
                break;
            }

            if (p_begin[i] > conv_limits_type::max_value_chars_dec[i]) {
                // Overflow will happen.
                return false;
            }
        }
    }

    return atoi_dec_common(p_begin, p_end, i);
}

template <typename IntType>
inline bool atoi_dec_neg(const char* p_begin, const char* p_end, IntType& i) noexcept {
    static_assert(
        is_non_bool_integral<IntType>::value, "atoi_dec() accepts non-boolean integral types as an output type");

    if (p_begin == p_end) {
        return false;
    }

    using conv_limits_type = conv_limits<sizeof(IntType), std::is_signed<IntType>::value>;

    std::size_t len = static_cast<std::size_t>(p_end - p_begin);
    if (len > conv_limits_type::max_chars_dec) {
        // Underflow will happen.
        return false;
    }

    if (len == conv_limits_type::max_chars_dec) {
        for (std::size_t i = 0; i < conv_limits_type::max_chars_dec; i++) {
            if (p_begin[i] < conv_limits_type::min_value_chars_dec[i]) {
                // No need to check the lower digits. Underflow will no longer happen.
                break;
            }

            if (p_begin[i] > conv_limits_type::min_value_chars_dec[i]) {
                // Underflow will happen.
                return false;
            }
        }
    }

    return atoi_dec_common(p_begin, p_end, i);
}

template <typename IntType>
inline bool atoi_oct(const char* p_begin, const char* p_end, IntType& i) noexcept {
    static_assert(
        is_non_bool_integral<IntType>::value, "atoi_oct() accepts non-boolean integral types as an output type");

    if (p_begin == p_end) {
        return false;
    }

    using conv_limits_type = conv_limits<sizeof(IntType), std::is_signed<IntType>::value>;

    std::size_t len = static_cast<std::size_t>(p_end - p_begin);
    if (!conv_limits_type::check_if_octs_safe(p_begin, len)) {
        return false;
    }

    i = 0;
    do {
        char c = *p_begin;
        if (c < '0' || '7' < c) {
            return false;
        }
        i = i * IntType(8) + IntType(c - '0');
    } while (++p_begin != p_end);

    return true;
}

template <typename IntType>
inline bool atoi_hex(const char* p_begin, const char* p_end, IntType& i) noexcept {
    static_assert(
        is_non_bool_integral<IntType>::value, "atoi_hex() accepts non-boolean integral types as an output type");

    if (p_begin == p_end) {
        return false;
    }

    using conv_limits_type = conv_limits<sizeof(IntType), std::is_signed<IntType>::value>;

    std::size_t len = static_cast<std::size_t>(p_end - p_begin);
    if (!conv_limits_type::check_if_hexs_safe(p_begin, len)) {
        return false;
    }

    i = 0;
    do {
        char c = *p_begin;
        IntType ci = 0;
        if ('0' <= c && c <= '9') {
            ci = IntType(c - '0');
        }
        else if ('A' <= c && c <= 'F') {
            ci = IntType(c - 'A' + 10);
        }
        else if ('a' <= c && c <= 'f') {
            ci = IntType(c - 'a' + 10);
        }
        else {
            return false;
        }
        i = i * IntType(16) + ci;
    } while (++p_begin != p_end);

    return true;
}

template <typename CharItr, typename IntType>
inline bool atoi(CharItr begin, CharItr end, IntType& i) noexcept {
    static_assert(is_iterator_of<CharItr, char>::value, "atoi() accepts iterators for char type");
    static_assert(is_non_bool_integral<IntType>::value, "atoi() accepts non-boolean integral types as an output type");

    if (begin == end) {
        return false;
    }

    uint32_t len = static_cast<uint32_t>(std::distance(begin, end));
    const char* p_begin = &*begin;
    const char* p_end = p_begin + len;

    char first = *begin;
    if (first == '+') {
        return atoi_dec_pos(p_begin + 1, p_end, i);
    }

    if (first == '-') {
        if (!std::numeric_limits<IntType>::is_signed) {
            return false;
        }

        bool success = atoi_dec_neg(p_begin + 1, p_end, i);
        if (success) {
            i *= IntType(-1);
        }

        return success;
    }

    if (first != '0') {
        return atoi_dec_pos(p_begin, p_end, i);
    }
    else if (p_begin + 1 != p_end) {
        switch (*(p_begin + 1)) {
        case 'o':
            return atoi_oct(p_begin + 2, p_end, i);
        case 'x':
            return atoi_hex(p_begin + 2, p_end, i);
        default:
            // The YAML spec doesn't allow decimals starting with 0.
            return false;
        }
    }

    i = 0;
    return true;
}

template <typename FloatType>
inline void set_infinity(FloatType& f, const FloatType sign) noexcept;

template <>
inline void set_infinity<float>(float& f, const float sign) noexcept {
    f = std::numeric_limits<float>::infinity() * sign;
}

template <>
inline void set_infinity<double>(double& f, const double sign) noexcept {
    f = std::numeric_limits<double>::infinity() * sign;
}

template <>
inline void set_infinity<long double>(long double& f, const long double sign) noexcept {
    f = std::numeric_limits<long double>::infinity() * sign;
}

template <typename FloatType>
inline void set_nan(FloatType& f) noexcept;

template <>
inline void set_nan<float>(float& f) noexcept {
    f = std::nanf("");
}

template <>
inline void set_nan<double>(double& f) noexcept {
    f = std::nan("");
}

template <>
inline void set_nan<long double>(long double& f) noexcept {
    f = std::nanl("");
}

template <typename CharItr>
inline bool atof_impl(CharItr begin, CharItr end, float& f) {
    std::size_t idx = 0;
    f = std::stof(std::string(begin, end), &idx);
    return idx == static_cast<std::size_t>(std::distance(begin, end));
}

template <typename CharItr>
inline bool atof_impl(CharItr begin, CharItr end, double& f) {
    std::size_t idx = 0;
    f = std::stod(std::string(begin, end), &idx);
    return idx == static_cast<std::size_t>(std::distance(begin, end));
}

template <typename CharItr>
inline bool atof_impl(CharItr begin, CharItr end, long double& f) {
    std::size_t idx = 0;
    f = std::stold(std::string(begin, end), &idx);
    return idx == static_cast<std::size_t>(std::distance(begin, end));
}

template <typename CharItr, typename FloatType>
inline bool atof(CharItr begin, CharItr end, FloatType& f) {
    static_assert(is_iterator_of<CharItr, char>::value, "atof() accepts iterators for char type");
    static_assert(std::is_floating_point<FloatType>::value, "atof() accepts floating point types as an output type");

    if (begin == end) {
        return false;
    }

    uint32_t len = static_cast<uint32_t>(std::distance(begin, end));
    const char* p_begin = &*begin;
    // const char* p_end = p_begin + len;

    if (*p_begin == '-') {
        if (len == 5) {
            const char* p_from_second = p_begin + 1;
            bool is_min_inf_scalar = (std::strncmp(p_from_second, ".inf", 4) == 0) ||
                                     (std::strncmp(p_from_second, ".Inf", 4) == 0) ||
                                     (std::strncmp(p_from_second, ".INF", 4) == 0);

            if (is_min_inf_scalar) {
                set_infinity(f, FloatType(-1.));
                return true;
            }
        }
    }
    else if (len == 4) {
        bool is_inf_scalar = (std::strncmp(p_begin, ".inf", 4) == 0) || (std::strncmp(p_begin, ".Inf", 4) == 0) ||
                             (std::strncmp(p_begin, ".INF", 4) == 0);
        bool is_nan_scalar = false;
        if (!is_inf_scalar) {
            is_nan_scalar = (std::strncmp(p_begin, ".nan", 4) == 0) || (std::strncmp(p_begin, ".NaN", 4) == 0) ||
                            (std::strncmp(p_begin, ".NAN", 4) == 0);
        }

        if (is_inf_scalar) {
            set_infinity(f, FloatType(1.));
            return true;
        }

        if (is_nan_scalar) {
            set_nan(f);
            return true;
        }
    }

    bool success = false;
    try {
        success = atof_impl(begin, end, f);
    }
    catch (const std::exception& /*unused*/) {
        success = false;
    }

    return success;
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_CONVERSIONS_SCALAR_CONV_HPP_ */
