//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_CONVERSIONS_TO_STRING_HPP
#define FK_YAML_DETAIL_CONVERSIONS_TO_STRING_HPP

#include <cmath>
#include <limits>
#include <string>
#include <sstream>
#include <type_traits>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Converts a ValueType object to a string YAML token.
/// @tparam ValueType A source value type.
/// @tparam CharType The type of characters for the conversion result.
/// @param s A resulting output string.
/// @param v A source value.
template <typename ValueType, typename CharType>
inline void to_string(ValueType v, std::basic_string<CharType>& s) noexcept;

/// @brief Specialization of to_string() for null values.
/// @param s A resulting string YAML token.
/// @param (unused) nullptr
template <>
inline void to_string(std::nullptr_t /*unused*/, std::string& s) noexcept {
    s = "null";
}

/// @brief Specialization of to_string() for booleans.
/// @param s A resulting string YAML token.
/// @param v A boolean source value.
template <>
inline void to_string(bool v, std::string& s) noexcept {
    s = v ? "true" : "false";
}

/// @brief Specialization of to_string() for integers.
/// @tparam IntegerType An integer type.
/// @param s A resulting string YAML token.
/// @param i An integer source value.
template <typename IntegerType>
inline enable_if_t<is_non_bool_integral<IntegerType>::value> to_string(IntegerType v, std::string& s) noexcept {
    s = std::to_string(v);
}

/// @brief Specialization of to_string() for floating point numbers.
/// @tparam FloatType A floating point number type.
/// @param s A resulting string YAML token.
/// @param f A floating point number source value.
template <typename FloatType>
inline enable_if_t<std::is_floating_point<FloatType>::value> to_string(FloatType v, std::string& s) noexcept {
    if (std::isnan(v)) {
        s = ".nan";
        return;
    }

    if (std::isinf(v)) {
        if (v == std::numeric_limits<FloatType>::infinity()) {
            s = ".inf";
        }
        else {
            s = "-.inf";
        }
        return;
    }

    std::ostringstream oss;
    oss << v;
    s = oss.str();

    // If `v` is actually an integer and no scientific notation is used for serialization, ".0" must be appended.
    // The result would cause a roundtrip issue otherwise. https://github.com/fktn-k/fkYAML/issues/405
    const std::size_t pos = s.find_first_of(".e");
    if (pos == std::string::npos) {
        s += ".0";
    }
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_CONVERSIONS_TO_STRING_HPP */
