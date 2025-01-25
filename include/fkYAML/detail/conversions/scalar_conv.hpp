//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

// **NOTE FOR LIBARARY DEVELOPERS**:
// Implementations in this header file are intentionally optimized for conversions between YAML scalars and native C++
// types. So, some implementations don't follow the convensions in the standard C++ functions. For example, octals must
// begin with "0o" (not "0"), which is specified in the YAML spec 1.2.

#ifndef FK_YAML_CONVERSIONS_SCALAR_CONV_HPP
#define FK_YAML_CONVERSIONS_SCALAR_CONV_HPP

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>

#if FK_YAML_HAS_TO_CHARS
// Prefer std::to_chars() and std::from_chars() functions if available.
#include <charconv>
#else
// Fallback to legacy string conversion functions otherwise.
#include <string> // std::stof(), std::stod(), std::stold()
#endif

FK_YAML_DETAIL_NAMESPACE_BEGIN

//////////////////////////
//   conv_limits_base   //
//////////////////////////

/// @brief A structure which provides limits for conversions between scalars and integers.
/// @note This structure contains common limits in both signed and unsigned integers.
/// @tparam NumBytes The number of bytes for the integer type.
template <std::size_t NumBytes>
struct conv_limits_base {};

/// @brief The specialization of conv_limits_base for 1 byte integers, e.g., int8_t, uint8_t.
template <>
struct conv_limits_base<1u> {
    /// max characters for octals (0o377) without the prefix part.
    static constexpr std::size_t max_chars_oct = 3;
    /// max characters for hexadecimals (0xFF) without the prefix part.
    static constexpr std::size_t max_chars_hex = 2;

    /// @brief Check if the given octals are safely converted into 1 byte integer.
    /// @param octs The pointer to octal characters
    /// @param len The length of octal characters
    /// @return true is safely convertible, false otherwise.
    static bool check_if_octs_safe(const char* octs, std::size_t len) noexcept {
        return (len < max_chars_oct) || (len == max_chars_oct && octs[0] <= '3');
    }

    /// @brief Check if the given hexadecimals are safely converted into 1 byte integer.
    /// @param octs The pointer to hexadecimal characters
    /// @param len The length of hexadecimal characters
    /// @return true is safely convertible, false otherwise.
    static bool check_if_hexs_safe(const char* /*unused*/, std::size_t len) noexcept {
        return len <= max_chars_hex;
    }
};

/// @brief The specialization of conv_limits_base for 2 byte integers, e.g., int16_t, uint16_t.
template <>
struct conv_limits_base<2u> {
    /// max characters for octals (0o177777) without the prefix part.
    static constexpr std::size_t max_chars_oct = 6;
    /// max characters for hexadecimals (0xFFFF) without the prefix part.
    static constexpr std::size_t max_chars_hex = 4;

    /// @brief Check if the given octals are safely converted into 2 byte integer.
    /// @param octs The pointer to octal characters
    /// @param len The length of octal characters
    /// @return true is safely convertible, false otherwise.
    static bool check_if_octs_safe(const char* octs, std::size_t len) noexcept {
        return (len < max_chars_oct) || (len == max_chars_oct && octs[0] <= '1');
    }

    /// @brief Check if the given hexadecimals are safely converted into 2 byte integer.
    /// @param octs The pointer to hexadecimal characters
    /// @param len The length of hexadecimal characters
    /// @return true is safely convertible, false otherwise.
    static bool check_if_hexs_safe(const char* /*unused*/, std::size_t len) noexcept {
        return len <= max_chars_hex;
    }
};

/// @brief The specialization of conv_limits_base for 4 byte integers, e.g., int32_t, uint32_t.
template <>
struct conv_limits_base<4u> {
    /// max characters for octals (0o37777777777) without the prefix part.
    static constexpr std::size_t max_chars_oct = 11;
    /// max characters for hexadecimals (0xFFFFFFFF) without the prefix part.
    static constexpr std::size_t max_chars_hex = 8;

    /// @brief Check if the given octals are safely converted into 4 byte integer.
    /// @param octs The pointer to octal characters
    /// @param len The length of octal characters
    /// @return true is safely convertible, false otherwise.
    static bool check_if_octs_safe(const char* octs, std::size_t len) noexcept {
        return (len < max_chars_oct) || (len == max_chars_oct && octs[0] <= '3');
    }

    /// @brief Check if the given hexadecimals are safely converted into 4 byte integer.
    /// @param octs The pointer to hexadecimal characters
    /// @param len The length of hexadecimal characters
    /// @return true is safely convertible, false otherwise.
    static bool check_if_hexs_safe(const char* /*unused*/, std::size_t len) noexcept {
        return len <= max_chars_hex;
    }
};

/// @brief The specialization of conv_limits_base for 8 byte integers, e.g., int64_t, uint64_t.
template <>
struct conv_limits_base<8u> {
    /// max characters for octals (0o1777777777777777777777) without the prefix part.
    static constexpr std::size_t max_chars_oct = 22;
    /// max characters for hexadecimals (0xFFFFFFFFFFFFFFFF) without the prefix part.
    static constexpr std::size_t max_chars_hex = 16;

    /// @brief Check if the given octals are safely converted into 8 byte integer.
    /// @param octs The pointer to octal characters
    /// @param len The length of octal characters
    /// @return true is safely convertible, false otherwise.
    static bool check_if_octs_safe(const char* octs, std::size_t len) noexcept {
        return (len < max_chars_oct) || (len == max_chars_oct && octs[0] <= '1');
    }

    /// @brief Check if the given hexadecimals are safely converted into 8 byte integer.
    /// @param octs The pointer to hexadecimal characters
    /// @param len The length of hexadecimal characters
    /// @return true is safely convertible, false otherwise.
    static bool check_if_hexs_safe(const char* /*unused*/, std::size_t len) noexcept {
        return len <= max_chars_hex;
    }
};

/////////////////////
//   conv_limits   //
/////////////////////

/// @brief A structure which provides limits for conversions between scalars and integers.
/// @note This structure contains limits which differs based on signedness.
/// @tparam NumBytes The number of bytes for the integer type.
/// @tparam IsSigned Whether an integer is signed or unsigned
template <std::size_t NumBytes, bool IsSigned>
struct conv_limits {};

/// @brief The specialization of conv_limits for 1 byte signed integers, e.g., int8_t.
template <>
struct conv_limits<1u, true> : conv_limits_base<1u> {
    /// with or without sign.
    static constexpr bool is_signed = true;

    /// max characters for decimals (-128..127) without sign.
    static constexpr std::size_t max_chars_dec = 3;

    /// string representation of max decimal value.
    static const char* max_value_chars_dec() noexcept {
        // Making this function a static constexpr variable, a link error happens.
        // Although the issue has been fixed since C++17, this workaround is necessary to let this functionality work
        // with C++11 (the library's default C++ standard version).
        // The same thing is applied to similar functions in the other specializations.

        static constexpr char max_value_chars[] = "127";
        return &max_value_chars[0];
    }

    /// string representation of min decimal value without sign.
    static const char* min_value_chars_dec() noexcept {
        static constexpr char min_value_chars[] = "128";
        return &min_value_chars[0];
    }
};

/// @brief The specialization of conv_limits for 1 byte unsigned integers, e.g., uint8_t.
template <>
struct conv_limits<1u, false> : conv_limits_base<1u> {
    /// with or without sign.
    static constexpr bool is_signed = false;

    /// max characters for decimals (0..255) without sign.
    static constexpr std::size_t max_chars_dec = 3;

    /// string representation of max decimal value.
    static const char* max_value_chars_dec() noexcept {
        static constexpr char max_value_chars[] = "255";
        return &max_value_chars[0];
    }

    /// string representation of min decimal value.
    static const char* min_value_chars_dec() noexcept {
        static constexpr char min_value_chars[] = "0";
        return &min_value_chars[0];
    }
};

/// @brief The specialization of conv_limits for 2 byte signed integers, e.g., int16_t.
template <>
struct conv_limits<2u, true> : conv_limits_base<2u> {
    /// with or without sign.
    static constexpr bool is_signed = true;

    /// max characters for decimals (-32768..32767) without sign.
    static constexpr std::size_t max_chars_dec = 5;

    /// string representation of max decimal value.
    static const char* max_value_chars_dec() noexcept {
        static constexpr char max_value_chars[] = "32767";
        return &max_value_chars[0];
    }

    /// string representation of min decimal value without sign.
    static const char* min_value_chars_dec() noexcept {
        static constexpr char min_value_chars[] = "32768";
        return &min_value_chars[0];
    }
};

/// @brief The specialization of conv_limits for 2 byte unsigned integers, e.g., uint16_t.
template <>
struct conv_limits<2u, false> : conv_limits_base<2u> {
    /// with or without sign.
    static constexpr bool is_signed = false;

    /// max characters for decimals (0..65535) without sign.
    static constexpr std::size_t max_chars_dec = 5;

    /// string representation of max decimal value.
    static const char* max_value_chars_dec() noexcept {
        static constexpr char max_value_chars[] = "65535";
        return &max_value_chars[0];
    }

    /// string representation of min decimal value.
    static const char* min_value_chars_dec() noexcept {
        static constexpr char min_value_chars[] = "0";
        return &min_value_chars[0];
    }
};

/// @brief The specialization of conv_limits for 4 byte signed integers, e.g., int32_t.
template <>
struct conv_limits<4u, true> : conv_limits_base<4u> {
    /// with or without sign.
    static constexpr bool is_signed = true;

    /// max characters for decimals (-2147483648..2147483647) without sign.
    static constexpr std::size_t max_chars_dec = 10;

    /// string representation of max decimal value.
    static const char* max_value_chars_dec() noexcept {
        static constexpr char max_value_chars[] = "2147483647";
        return &max_value_chars[0];
    }

    /// string representation of min decimal value without sign.
    static const char* min_value_chars_dec() noexcept {
        static constexpr char min_value_chars[] = "2147483648";
        return &min_value_chars[0];
    }
};

/// @brief The specialization of conv_limits for 4 byte unsigned integers, e.g., uint32_t.
template <>
struct conv_limits<4u, false> : conv_limits_base<4u> {
    /// with or without sign.
    static constexpr bool is_signed = false;

    /// max characters for decimals (0..4294967295) without sign.
    static constexpr std::size_t max_chars_dec = 10;

    /// string representation of max decimal value.
    static const char* max_value_chars_dec() noexcept {
        static constexpr char max_value_chars[] = "4294967295";
        return &max_value_chars[0];
    }

    /// string representation of min decimal value.
    static const char* min_value_chars_dec() noexcept {
        static constexpr char min_value_chars[] = "0";
        return &min_value_chars[0];
    }
};

/// @brief The specialization of conv_limits for 8 byte signed integers, e.g., int64_t.
template <>
struct conv_limits<8u, true> : conv_limits_base<8u> {
    /// with or without sign.
    static constexpr bool is_signed = true;

    /// max characters for decimals (-9223372036854775808..9223372036854775807) without sign.
    static constexpr std::size_t max_chars_dec = 19;

    /// string representation of max decimal value.
    static const char* max_value_chars_dec() noexcept {
        static constexpr char max_value_chars[] = "9223372036854775807";
        return &max_value_chars[0];
    }

    /// string representation of min decimal value without sign.
    static const char* min_value_chars_dec() noexcept {
        static constexpr char min_value_chars[] = "9223372036854775808";
        return &min_value_chars[0];
    }
};

/// @brief The specialization of conv_limits for 8 byte unsigned integers, e.g., uint64_t.
template <>
struct conv_limits<8u, false> : conv_limits_base<8u> {
    /// with or without sign.
    static constexpr bool is_signed = false;

    /// max characters for decimals (0..18446744073709551615) without sign.
    static constexpr std::size_t max_chars_dec = 20;

    /// string representation of max decimal value.
    static const char* max_value_chars_dec() noexcept {
        static constexpr char max_value_chars[] = "18446744073709551615";
        return &max_value_chars[0];
    }

    /// string representation of min decimal value.
    static const char* min_value_chars_dec() noexcept {
        static constexpr char min_value_chars[] = "0";
        return &min_value_chars[0];
    }
};

//////////////////////////
//   scalar <--> null   //
//////////////////////////

/// @brief Converts a scalar into a null value
/// @tparam CharItr Type of char iterators. Its value type must be `char` (maybe cv-qualified).
/// @param begin The iterator to the first element of the scalar.
/// @param end The iterator to the past-the-end element of the scalar.
/// @param /*unused*/ The null value holder (unused since it can only have `nullptr`)
/// @return true if the conversion completes successfully, false otherwise.
template <typename CharItr>
inline bool aton(CharItr begin, CharItr end, std::nullptr_t& /*unused*/) noexcept {
    static_assert(is_iterator_of<CharItr, char>::value, "aton() accepts iterators for char type");

    if FK_YAML_UNLIKELY (begin == end) {
        return false;
    }

    const auto len = static_cast<uint32_t>(std::distance(begin, end));

    // This path is the most probable case, so check it first.
    if FK_YAML_LIKELY (len == 4) {
        const char* p_begin = &*begin;
        return (std::strncmp(p_begin, "null", 4) == 0) || (std::strncmp(p_begin, "Null", 4) == 0) ||
               (std::strncmp(p_begin, "NULL", 4) == 0);
    }

    if (len == 1) {
        return *begin == '~';
    }

    return false;
}

/////////////////////////////
//   scalar <--> boolean   //
/////////////////////////////

/// @brief Converts a scalar into a boolean value
/// @tparam CharItr The type of char iterators. Its value type must be `char` (maybe cv-qualified).
/// @tparam BoolType The output boolean type.
/// @param begin The iterator to the first element of the scalar.
/// @param end The iterator to the past-the-end element of the scalar.
/// @param boolean The boolean value holder.
/// @return true if the conversion completes successfully, false otherwise.
template <typename CharItr, typename BoolType>
inline bool atob(CharItr begin, CharItr end, BoolType& boolean) noexcept {
    static_assert(is_iterator_of<CharItr, char>::value, "atob() accepts iterators for char type");

    if FK_YAML_UNLIKELY (begin == end) {
        return false;
    }

    const auto len = static_cast<uint32_t>(std::distance(begin, end));
    const char* p_begin = &*begin;

    if (len == 4) {
        const bool is_true = (std::strncmp(p_begin, "true", 4) == 0) || (std::strncmp(p_begin, "True", 4) == 0) ||
                             (std::strncmp(p_begin, "TRUE", 4) == 0);

        if FK_YAML_LIKELY (is_true) {
            boolean = static_cast<BoolType>(true);
        }
        return is_true;
    }

    if (len == 5) {
        const bool is_false = (std::strncmp(p_begin, "false", 5) == 0) || (std::strncmp(p_begin, "False", 5) == 0) ||
                              (std::strncmp(p_begin, "FALSE", 5) == 0);

        if FK_YAML_LIKELY (is_false) {
            boolean = static_cast<BoolType>(false);
        }
        return is_false;
    }

    return false;
}

/////////////////////////////
//   scalar <--> integer   //
/////////////////////////////

//
// scalar --> decimals
//

/// @brief Converts a scalar into decimals. This is common implementation for both signed/unsigned integer types.
/// @warning
/// This function does NOT care about overflows if IntType is unsigned. The source string value must be validated
/// beforehand by calling either atoi_dec_pos() or atoi_dec_neg() functions.
/// Furthermore, `p_begin` and `p_end` must NOT be null. Validate them before calling this function.
/// @tparam IntType The output integer type. It can be either signed or unsigned.
/// @param p_begin The pointer to the first element of the scalar.
/// @param p_end The pointer to the past-the-end element of the scalar.
/// @param i The output integer value holder.
/// @return true if the conversion completes successfully, false otherwise.
template <typename IntType>
inline bool atoi_dec_unchecked(const char* p_begin, const char* p_end, IntType& i) noexcept {
    static_assert(
        is_non_bool_integral<IntType>::value,
        "atoi_dec_unchecked() accepts non-boolean integral types as an output type");

    i = 0;
    do {
        const char c = *p_begin;
        if FK_YAML_UNLIKELY (c < '0' || '9' < c) {
            return false;
        }
        // Overflow is intentional when the IntType is signed.
        i = i * static_cast<IntType>(10) + static_cast<IntType>(c - '0');
    } while (++p_begin != p_end);

    return true;
}

/// @brief Converts a scalar into positive decimals. This function executes bounds check to avoid overflow.
/// @warning `p_begin` and `p_end` must not be null. Validate them before calling this function.
/// @tparam IntType The output integer type. It can be either signed or unsigned.
/// @param p_begin The pointer to the first element of the scalar.
/// @param p_end The pointer to the past-the-end element of the scalar.
/// @param i The output integer value holder.
/// @return true if the conversion completes successfully, false otherwise.
template <typename IntType>
inline bool atoi_dec_pos(const char* p_begin, const char* p_end, IntType& i) noexcept {
    static_assert(
        is_non_bool_integral<IntType>::value, "atoi_dec_pos() accepts non-boolean integral types as an output type");

    if FK_YAML_UNLIKELY (p_begin == p_end) {
        return false;
    }

    using conv_limits_type = conv_limits<sizeof(IntType), std::is_signed<IntType>::value>;

    const auto len = static_cast<std::size_t>(p_end - p_begin);
    if FK_YAML_UNLIKELY (len > conv_limits_type::max_chars_dec) {
        // Overflow will happen.
        return false;
    }

    if (len == conv_limits_type::max_chars_dec) {
        const char* p_max_value_chars_dec = conv_limits_type::max_value_chars_dec();

        for (std::size_t idx = 0; idx < conv_limits_type::max_chars_dec; idx++) {
            if (p_begin[idx] < p_max_value_chars_dec[idx]) {
                // No need to check the lower digits. Overflow will no longer happen.
                break;
            }

            if FK_YAML_UNLIKELY (p_begin[idx] > p_max_value_chars_dec[idx]) {
                // Overflow will happen.
                return false;
            }
        }
    }

    return atoi_dec_unchecked(p_begin, p_end, i);
}

/// @brief Converts a scalar into negative decimals. This function executes bounds check to avoid underflow.
/// @warning `p_begin` and `p_end` must not be null. Validate them before calling this function.
/// @tparam IntType The output integer type. It must be signed.
/// @param p_begin The pointer to the first element of the scalar.
/// @param p_end The pointer to the past-the-end element of the scalar.
/// @param i The output integer value holder.
/// @return true if the conversion completes successfully, false otherwise.
template <typename IntType>
inline bool atoi_dec_neg(const char* p_begin, const char* p_end, IntType& i) noexcept {
    static_assert(
        is_non_bool_integral<IntType>::value, "atoi_dec_neg() accepts non-boolean integral types as an output type");

    if FK_YAML_UNLIKELY (p_begin == p_end) {
        return false;
    }

    using conv_limits_type = conv_limits<sizeof(IntType), std::is_signed<IntType>::value>;

    const auto len = static_cast<std::size_t>(p_end - p_begin);
    if FK_YAML_UNLIKELY (len > conv_limits_type::max_chars_dec) {
        // Underflow will happen.
        return false;
    }

    if (len == conv_limits_type::max_chars_dec) {
        const char* p_min_value_chars_dec = conv_limits_type::min_value_chars_dec();

        for (std::size_t idx = 0; idx < conv_limits_type::max_chars_dec; idx++) {
            if (p_begin[idx] < p_min_value_chars_dec[idx]) {
                // No need to check the lower digits. Underflow will no longer happen.
                break;
            }

            if FK_YAML_UNLIKELY (p_begin[idx] > p_min_value_chars_dec[idx]) {
                // Underflow will happen.
                return false;
            }
        }
    }

    return atoi_dec_unchecked(p_begin, p_end, i);
}

//
// scalar --> octals
//

/// @brief Converts a scalar into octals. This function executes bounds check to avoid overflow.
/// @warning `p_begin` and `p_end` must not be null. Validate them before calling this function.
/// @tparam IntType The output integer type. It can be either signed or unsigned.
/// @param p_begin The pointer to the first element of the scalar.
/// @param p_end The pointer to the past-the-end element of the scalar.
/// @param i The output integer value holder.
/// @return true if the conversion completes successfully, false otherwise.
template <typename IntType>
inline bool atoi_oct(const char* p_begin, const char* p_end, IntType& i) noexcept {
    static_assert(
        is_non_bool_integral<IntType>::value, "atoi_oct() accepts non-boolean integral types as an output type");

    if FK_YAML_UNLIKELY (p_begin == p_end) {
        return false;
    }

    using conv_limits_type = conv_limits<sizeof(IntType), std::is_signed<IntType>::value>;

    const auto len = static_cast<std::size_t>(p_end - p_begin);
    if FK_YAML_UNLIKELY (!conv_limits_type::check_if_octs_safe(p_begin, len)) {
        return false;
    }

    i = 0;
    do {
        const char c = *p_begin;
        if FK_YAML_UNLIKELY (c < '0' || '7' < c) {
            return false;
        }
        i = i * static_cast<IntType>(8) + static_cast<IntType>(c - '0');
    } while (++p_begin != p_end);

    return true;
}

//
// scalar --> hexadecimals
//

/// @brief Converts a scalar into hexadecimals. This function executes bounds check to avoid overflow.
/// @warning `p_begin` and `p_end` must not be null. Validate them before calling this function.
/// @tparam IntType The output integer type. It can be either signed or unsigned.
/// @param p_begin The pointer to the first element of the scalar.
/// @param p_end The pointer to the past-the-end element of the scalar.
/// @param i The output integer value holder.
/// @return true if the conversion completes successfully, false otherwise.
template <typename IntType>
inline bool atoi_hex(const char* p_begin, const char* p_end, IntType& i) noexcept {
    static_assert(
        is_non_bool_integral<IntType>::value, "atoi_hex() accepts non-boolean integral types as an output type");

    if FK_YAML_UNLIKELY (p_begin == p_end) {
        return false;
    }

    using conv_limits_type = conv_limits<sizeof(IntType), std::is_signed<IntType>::value>;

    const auto len = static_cast<std::size_t>(p_end - p_begin);
    if FK_YAML_UNLIKELY (!conv_limits_type::check_if_hexs_safe(p_begin, len)) {
        return false;
    }

    i = 0;
    do {
        // NOLINTBEGIN(bugprone-misplaced-widening-cast)
        const char c = *p_begin;
        IntType ci = 0;
        if ('0' <= c && c <= '9') {
            ci = static_cast<IntType>(c - '0');
        }
        else if ('A' <= c && c <= 'F') {
            ci = static_cast<IntType>(c - 'A' + 10);
        }
        else if ('a' <= c && c <= 'f') {
            ci = static_cast<IntType>(c - 'a' + 10);
        }
        else {
            return false;
        }
        i = i * static_cast<IntType>(16) + ci;
        // NOLINTEND(bugprone-misplaced-widening-cast)
    } while (++p_begin != p_end);

    return true;
}

//
// atoi() & itoa()
//

/// @brief Converts a scalar into integers. This function executes bounds check to avoid overflow/underflow.
/// @tparam CharItr The type of char iterators. Its value type must be char (maybe cv-qualified).
/// @tparam IntType The output integer type. It can be either signed or unsigned.
/// @param begin The iterator to the first element of the scalar.
/// @param end The iterator to the past-the-end element of the scalar.
/// @param i The output integer value holder.
/// @return true if the conversion completes successfully, false otherwise.
template <typename CharItr, typename IntType>
inline bool atoi(CharItr begin, CharItr end, IntType& i) noexcept {
    static_assert(is_iterator_of<CharItr, char>::value, "atoi() accepts iterators for char type");
    static_assert(is_non_bool_integral<IntType>::value, "atoi() accepts non-boolean integral types as an output type");

    if FK_YAML_UNLIKELY (begin == end) {
        return false;
    }

    const auto len = static_cast<uint32_t>(std::distance(begin, end));
    const char* p_begin = &*begin;
    const char* p_end = p_begin + len;

    const char first = *begin;
    if (first == '+') {
        return atoi_dec_pos(p_begin + 1, p_end, i);
    }

    if (first == '-') {
        if (!std::numeric_limits<IntType>::is_signed) {
            return false;
        }

        const bool success = atoi_dec_neg(p_begin + 1, p_end, i);
        if (success) {
            i *= static_cast<IntType>(-1);
        }

        return success;
    }

    if (first != '0') {
        return atoi_dec_pos(p_begin, p_end, i);
    }

    if (p_begin + 1 != p_end) {
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

///////////////////////////
//   scalar <--> float   //
///////////////////////////

/// @brief Set an infinite `float` value based on the given signedness.
/// @param f The output `float` value holder.
/// @param sign Whether the infinite value should be positive or negative.
inline void set_infinity(float& f, const float sign) noexcept {
    f = std::numeric_limits<float>::infinity() * sign;
}

/// @brief Set an infinite `double` value based on the given signedness.
/// @param f The output `double` value holder.
/// @param sign Whether the infinite value should be positive or negative.
inline void set_infinity(double& f, const double sign) noexcept {
    f = std::numeric_limits<double>::infinity() * sign;
}

/// @brief Set a NaN `float` value.
/// @param f The output `float` value holder.
inline void set_nan(float& f) noexcept {
    f = std::nanf("");
}

/// @brief Set a NaN `double` value.
/// @param f The output `double` value holder.
inline void set_nan(double& f) noexcept {
    f = std::nan("");
}

#if FK_YAML_HAS_TO_CHARS

/// @brief Converts a scalar into a floating point value.
/// @warning `p_begin` and `p_end` must not be null. Validate them before calling this function.
/// @param p_begin The pointer to the first element of the scalar.
/// @param p_end The pointer to the past-the-end element of the scalar.
/// @param f The output floating point value holder.
/// @return true if the conversion completes successfully, false otherwise.
template <typename FloatType>
inline bool atof_impl(const char* p_begin, const char* p_end, FloatType& f) noexcept {
    static_assert(std::is_floating_point_v<FloatType>, "atof_impl() accepts floating point types as an output type");
    if (auto [ptr, ec] = std::from_chars(p_begin, p_end, f); ec == std::errc {}) {
        return ptr == p_end;
    }
    return false;
}

#else

/// @brief Converts a scalar into a `float` value.
/// @warning `p_begin` and `p_end` must not be null. Validate them before calling this function.
/// @param p_begin The pointer to the first element of the scalar.
/// @param p_end The pointer to the past-the-end element of the scalar.
/// @param f The output `float` value holder.
/// @return true if the conversion completes successfully, false otherwise.
inline bool atof_impl(const char* p_begin, const char* p_end, float& f) {
    std::size_t idx = 0;
    f = std::stof(std::string(p_begin, p_end), &idx);
    return idx == static_cast<std::size_t>(p_end - p_begin);
}

/// @brief Converts a scalar into a `double` value.
/// @warning `p_begin` and `p_end` must not be null. Validate them before calling this function.
/// @param p_begin The pointer to the first element of the scalar.
/// @param p_end The pointer to the past-the-end element of the scalar.
/// @param f The output `double` value holder.
/// @return true if the conversion completes successfully, false otherwise.
inline bool atof_impl(const char* p_begin, const char* p_end, double& f) {
    std::size_t idx = 0;
    f = std::stod(std::string(p_begin, p_end), &idx);
    return idx == static_cast<std::size_t>(p_end - p_begin);
}

#endif // FK_YAML_HAS_TO_CHARS

/// @brief Converts a scalar into a floating point value.
/// @tparam CharItr The type of char iterators. Its value type must be char (maybe cv-qualified).
/// @tparam FloatType The output floating point value type.
/// @param begin The iterator to the first element of the scalar.
/// @param end The iterator to the past-the-end element of the scalar.
/// @param f The output floating point value holder.
/// @return true if the conversion completes successfully, false otherwise.
template <typename CharItr, typename FloatType>
inline bool atof(CharItr begin, CharItr end, FloatType& f) noexcept(noexcept(atof_impl(&*begin, &*begin, f))) {
    static_assert(is_iterator_of<CharItr, char>::value, "atof() accepts iterators for char type");
    static_assert(std::is_floating_point<FloatType>::value, "atof() accepts floating point types as an output type");

    if FK_YAML_UNLIKELY (begin == end) {
        return false;
    }

    const auto len = static_cast<uint32_t>(std::distance(begin, end));
    const char* p_begin = &*begin;
    const char* p_end = p_begin + len;

    if (*p_begin == '-' || *p_begin == '+') {
        if (len == 5) {
            const char* p_from_second = p_begin + 1;
            const bool is_inf = (std::strncmp(p_from_second, ".inf", 4) == 0) ||
                                (std::strncmp(p_from_second, ".Inf", 4) == 0) ||
                                (std::strncmp(p_from_second, ".INF", 4) == 0);
            if (is_inf) {
                set_infinity(f, *p_begin == '-' ? static_cast<FloatType>(-1.) : static_cast<FloatType>(1.));
                return true;
            }
        }

        if (*p_begin == '+') {
            // Skip the positive sign since it's sometimes not recognized as part of float value.
            ++p_begin;
        }
    }
    else if (len == 4) {
        const bool is_inf = (std::strncmp(p_begin, ".inf", 4) == 0) || (std::strncmp(p_begin, ".Inf", 4) == 0) ||
                            (std::strncmp(p_begin, ".INF", 4) == 0);
        if (is_inf) {
            set_infinity(f, static_cast<FloatType>(1.));
            return true;
        }

        const bool is_nan = (std::strncmp(p_begin, ".nan", 4) == 0) || (std::strncmp(p_begin, ".NaN", 4) == 0) ||
                            (std::strncmp(p_begin, ".NAN", 4) == 0);
        if (is_nan) {
            set_nan(f);
            return true;
        }
    }

#if FK_YAML_HAS_TO_CHARS
    return atof_impl(p_begin, p_end, f);
#else
    bool success = false;
    try {
        success = atof_impl(p_begin, p_end, f);
    }
    catch (const std::exception& /*unused*/) {
        success = false;
    }

    return success;
#endif
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_CONVERSIONS_SCALAR_CONV_HPP */
