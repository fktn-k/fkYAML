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

#include <cstring>
#include <limits>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

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

template <typename CharItr, typename IntType>
inline bool atoi_dec(CharItr begin, CharItr end, IntType& i) noexcept {
    static_assert(is_iterator_of<CharItr, char>::value, "atoi_dec() accepts iterators for char type");
    static_assert(
        is_non_bool_integral<IntType>::value, "atoi_dec() accepts non-boolean integral types as an output type");

    if (begin == end) {
        return false;
    }

    i = 0;
    do {
        char c = *begin;
        if (c < '0' || '9' < c) {
            return false;
        }
        i = i * IntType(10) + IntType(c - '0');
    } while (++begin != end);

    return true;
}

template <typename CharItr, typename IntType>
inline bool atoi_oct(CharItr begin, CharItr end, IntType& i) noexcept {
    static_assert(is_iterator_of<CharItr, char>::value, "atoi_oct() accepts iterators for char type");
    static_assert(
        is_non_bool_integral<IntType>::value, "atoi_oct() accepts non-boolean integral types as an output type");

    if (begin == end) {
        return false;
    }

    i = 0;
    do {
        char c = *begin;
        if (c < '0' || '7' < c) {
            return false;
        }
        i = i * IntType(8) + IntType(c - '0');
    } while (++begin != end);

    return true;
}

template <typename CharItr, typename IntType>
inline bool atoi_hex(CharItr begin, CharItr end, IntType& i) noexcept {
    static_assert(is_iterator_of<CharItr, char>::value, "atoi_hex() accepts iterators for char type");
    static_assert(
        is_non_bool_integral<IntType>::value, "atoi_hex() accepts non-boolean integral types as an output type");

    if (begin == end) {
        return false;
    }

    i = 0;
    do {
        char c = *begin;
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
    } while (++begin != end);

    return true;
}

template <typename CharItr, typename IntType>
inline bool atoi(CharItr begin, CharItr end, IntType& i) noexcept {
    static_assert(is_iterator_of<CharItr, char>::value, "atoi() accepts iterators for char type");
    static_assert(is_non_bool_integral<IntType>::value, "atoi() accepts non-boolean integral types as an output type");

    if (begin == end) {
        return false;
    }

    char first = *begin;
    if (first == '+') {
        return atoi_dec(begin + 1, end, i);
    }

    if (first == '-') {
        if (!std::numeric_limits<IntType>::is_signed) {
            return false;
        }

        bool success = atoi_dec(begin + 1, end, i);
        if (success) {
            i *= IntType(-1);
        }

        return success;
    }

    if (first != '0') {
        return atoi_dec(begin, end, i);
    }
    else if (begin + 1 != end) {
        switch (*(begin + 1)) {
        case 'o':
            return atoi_oct(begin + 2, end, i);
        case 'x':
            return atoi_hex(begin + 2, end, i);
        default:
            // The YAML spec doesn't allow decimals starting with 0.
            return false;
        }
    }

    i = 0;
    return true;
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_CONVERSIONS_SCALAR_CONV_HPP_ */
