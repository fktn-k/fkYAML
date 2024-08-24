///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_CONVERSIONS_FROM_STRING_HPP_
#define FK_YAML_DETAIL_CONVERSIONS_FROM_STRING_HPP_

#include <cmath>
#include <cstdlib>
#include <limits>
#include <string>
#include <type_traits>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Convert a string YAML token to a ValueType object.
/// @tparam ValueType A target value type.
/// @tparam CharType The type of characters in a source string.
template <typename ValueType, typename CharType>
inline ValueType from_string(const std::basic_string<CharType>& s, type_tag<ValueType> /*unused*/);

/// @brief Specialization of from_string() for null values with std::string
/// @tparam  N/A
template <>
inline std::nullptr_t from_string(const std::string& s, type_tag<std::nullptr_t> /*unused*/) {
    if (s == "null" || s == "Null" || s == "NULL" || s == "~") {
        return nullptr;
    }

    throw exception("Cannot convert a string into a null value.");
}

/// @brief Specialization of from_string() for boolean values with std::string.
/// @tparam  N/A
template <>
inline bool from_string(const std::string& s, type_tag<bool> /*unused*/) {
    if (s == "true" || s == "True" || s == "TRUE") {
        return true;
    }

    if (s == "false" || s == "False" || s == "FALSE") {
        return false;
    }

    throw exception("Cannot convert a string into a boolean value.");
}

/// @brief Specialization of from_string() for int values with std::string.
/// @tparam  N/A
template <>
inline int from_string(const std::string& s, type_tag<int> /*unused*/) {
    std::size_t idx = 0;
    long ret = 0;

    try {
        ret = std::stoi(s, &idx, 0);
    }
    catch (const std::exception& /*unused*/) {
        throw exception("Failed to convert a string into an int value.");
    }

    return ret;
}

/// @brief Specialization of from_string() for long values with std::string.
/// @tparam  N/A
template <>
inline long from_string(const std::string& s, type_tag<long> /*unused*/) {
    std::size_t idx = 0;
    long ret = 0;

    try {
        ret = std::stol(s, &idx, 0);
    }
    catch (const std::exception& /*unused*/) {
        throw exception("Failed to convert a string into a long value.");
    }

    return ret;
}

/// @brief Specialization of from_string() for long long values with std::string.
/// @tparam  N/A
template <>
inline long long from_string(const std::string& s, type_tag<long long> /*unused*/) {
    std::size_t idx = 0;
    long long ret = 0;

    try {
        ret = std::stoll(s, &idx, 0);
    }
    catch (const std::exception& /*unused*/) {
        throw exception("Failed to convert a string into a long long value.");
    }

    return ret;
}

/// @brief Partial specialization of from_string() for other signed integer types with std::string.
/// @tparam SignedIntType A signed integer type other than long long.
template <typename SignedIntType>
inline enable_if_t<
    conjunction<
        is_non_bool_integral<SignedIntType>, std::is_signed<SignedIntType>, negation<std::is_same<SignedIntType, int>>,
        negation<std::is_same<SignedIntType, long>>, negation<std::is_same<SignedIntType, long long>>>::value,
    SignedIntType>
from_string(const std::string& s, type_tag<SignedIntType> /*unused*/) {
    const auto tmp_ret = from_string(s, type_tag<int> {});
    if (static_cast<long long>(std::numeric_limits<SignedIntType>::max()) < tmp_ret) {
        throw exception("Failed to convert a long long value into a SignedIntegerType value.");
    }

    return static_cast<SignedIntType>(tmp_ret);
}

/// @brief Specialization of from_string() for unsigned long values with std::string.
/// @tparam  N/A
template <>
inline unsigned long from_string(const std::string& s, type_tag<unsigned long> /*unused*/) {
    std::size_t idx = 0;
    unsigned long ret = 0;

    try {
        ret = std::stoul(s, &idx, 0);
    }
    catch (const std::exception& /*unused*/) {
        throw exception("Failed to convert a string into an unsigned long value.");
    }

    return ret;
}

/// @brief Specialization of from_string() for unsigned long long values with std::string.
/// @tparam  N/A
template <>
inline unsigned long long from_string(const std::string& s, type_tag<unsigned long long> /*unused*/) {
    std::size_t idx = 0;
    unsigned long long ret = 0;

    try {
        ret = std::stoull(s, &idx, 0);
    }
    catch (const std::exception& /*unused*/) {
        throw exception("Failed to convert a string into an unsigned long long value.");
    }

    return ret;
}

/// @brief Partial specialization of from_string() for other unsigned integer types with std::string.
/// @tparam UnsignedIntType An unsigned integer type other than unsigned long long.
template <typename UnsignedIntType>
inline enable_if_t<
    conjunction<
        is_non_bool_integral<UnsignedIntType>, std::is_unsigned<UnsignedIntType>,
        negation<std::is_same<UnsignedIntType, unsigned long>>,
        negation<std::is_same<UnsignedIntType, unsigned long long>>>::value,
    UnsignedIntType>
from_string(const std::string& s, type_tag<UnsignedIntType> /*unused*/) {
    const auto tmp_ret = from_string(s, type_tag<unsigned long> {});
    if (static_cast<long long>(std::numeric_limits<UnsignedIntType>::max()) < tmp_ret) {
        throw exception("Failed to convert an unsigned long long into an unsigned integer value.");
    }

    return static_cast<UnsignedIntType>(tmp_ret);
}

/// @brief Specialization of from_string() for float values with std::string.
/// @tparam  N/A
template <>
inline float from_string(const std::string& s, type_tag<float> /*unused*/) {
    if (s == ".inf" || s == ".Inf" || s == ".INF") {
        return std::numeric_limits<float>::infinity();
    }

    if (s == "-.inf" || s == "-.Inf" || s == "-.INF") {
        static_assert(std::numeric_limits<float>::is_iec559, "IEEE 754 required.");
        return -1 * std::numeric_limits<float>::infinity();
    }

    if (s == ".nan" || s == ".NaN" || s == ".NAN") {
        return std::nanf("");
    }

    std::size_t idx = 0;
    float ret = 0.0f;

    try {
        ret = std::stof(s, &idx);
    }
    catch (const std::exception& /*unused*/) {
        throw exception("Failed to convert a string into a float value.");
    }

    return ret;
}

/// @brief Specialization of from_string() for double values with std::string.
/// @tparam  N/A
template <>
inline double from_string(const std::string& s, type_tag<double> /*unused*/) {
    if (s == ".inf" || s == ".Inf" || s == ".INF") {
        return std::numeric_limits<double>::infinity();
    }

    if (s == "-.inf" || s == "-.Inf" || s == "-.INF") {
        static_assert(std::numeric_limits<double>::is_iec559, "IEEE 754 required.");
        return -1 * std::numeric_limits<double>::infinity();
    }

    if (s == ".nan" || s == ".NaN" || s == ".NAN") {
        return std::nan("");
    }

    std::size_t idx = 0;
    double ret = 0.0;

    try {
        ret = std::stod(s, &idx);
    }
    catch (const std::exception& /*unused*/) {
        throw exception("Failed to convert a string into a double value.");
    }

    return ret;
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_CONVERSIONS_FROM_STRING_HPP_ */
