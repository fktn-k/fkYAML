/**
 *  _______   __ __   __  _____   __  __  __
 * |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
 * |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.1.1
 * |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
 *
 * SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
 * SPDX-License-Identifier: MIT
 *
 * @file
 */

#ifndef FK_YAML_DETAIL_CONVERSIONS_FROM_STRING_HPP_
#define FK_YAML_DETAIL_CONVERSIONS_FROM_STRING_HPP_

#include <cmath>
#include <limits>
#include <string>
#include <type_traits>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_NAMESPACE_BEGIN

/**
 * @namespace detail
 * @brief namespace for internal implementations of fkYAML library.
 */
namespace detail
{

using fkyaml::exception;

template <typename ValueType, typename CharType>
inline ValueType from_string(const std::basic_string<CharType>& s, type_tag<ValueType> /*unused*/);

template <>
inline std::nullptr_t from_string(const std::string& s, type_tag<std::nullptr_t> /*unused*/)
{
    if (s == "null" || s == "Null" || s == "NULL" || s == "~")
    {
        return nullptr;
    }

    throw exception("Cannot convert a string into a null value.");
}

template <>
inline bool from_string(const std::string& s, type_tag<bool> /*unused*/)
{
    if (s == "true" || s == "True" || s == "TRUE")
    {
        return true;
    }

    if (s == "false" || s == "False" || s == "FALSE")
    {
        return false;
    }

    throw exception("Cannot convert a string into a boolean value.");
}

template <typename SignedIntType>
inline enable_if_t<conjunction<
    is_non_bool_integral<SignedIntType>,
    std::is_signed<SignedIntType>,
    negation<std::is_same<SignedIntType, long long>>>::value, SignedIntType>
from_string(const std::string& s, type_tag<SignedIntType> /*unused*/)
{
    const auto tmp_ret = from_string(s, type_tag<long long>{});
    if (static_cast<long long>(std::numeric_limits<SignedIntType>::max()) < tmp_ret)
    {
        throw exception("Failed to convert a string into an integer value.");
    }

    return static_cast<SignedIntType>(tmp_ret);
}

template <>
inline long long from_string(const std::string& s, type_tag<long long> /*unused*/)
{
    char* endptr = nullptr;
    const auto ret = std::strtoll(s.data(), &endptr, 0);

    if (endptr != s.data() + s.size() || errno != 0)
    {
        throw exception("Failed to convert a string into an integer value.");
    }

    return ret;
}

template <typename UnsignedIntType>
inline enable_if_t<conjunction<
    is_non_bool_integral<UnsignedIntType>,
    std::is_unsigned<UnsignedIntType>,
    negation<std::is_same<UnsignedIntType, unsigned long long>>>::value, UnsignedIntType>
from_string(const std::string& s, type_tag<UnsignedIntType> /*unused*/)
{
    const auto tmp_ret = from_string(s, type_tag<unsigned long long>{});
    if (static_cast<long long>(std::numeric_limits<UnsignedIntType>::max()) < tmp_ret)
    {
        throw exception("Failed to convert a string into an integer value.");
    }

    return static_cast<UnsignedIntType>(tmp_ret);
}

template <>
inline unsigned long long from_string(const std::string& s, type_tag<unsigned long long> /*unused*/)
{
    char* endptr = nullptr;
    const auto ret = std::strtoull(s.data(), &endptr, 0);

    if (endptr != s.data() + s.size() || errno != 0)
    {
        throw exception("Failed to convert a string into an integer value.");
    }

    return ret;
}

template <>
inline float from_string(const std::string& s, type_tag<float> /*unused*/)
{
        if (s == ".inf" || s == ".Inf" || s == ".INF")
        {
            return std::numeric_limits<float>::infinity();
        }

        if (s == "-.inf" || s == "-.Inf" || s == "-.INF")
        {
            static_assert(std::numeric_limits<float>::is_iec559, "IEEE 754 required.");
            return -1 * std::numeric_limits<float>::infinity();
        }

        if (s == ".nan" || s == ".NaN" || s == ".NAN")
        {
            return std::nanf("");
        }

        char* endptr = nullptr;
        const auto ret = std::strtof(s.data(), &endptr);

        if (endptr != s.data() + s.size())
        {
            throw exception("Failed to a string into a floating point number value.");
        }

        return ret;
}

template <>
inline double from_string(const std::string& s, type_tag<double> /*unused*/)
{
        if (s == ".inf" || s == ".Inf" || s == ".INF")
        {
            return std::numeric_limits<double>::infinity();
        }

        if (s == "-.inf" || s == "-.Inf" || s == "-.INF")
        {
            static_assert(std::numeric_limits<double>::is_iec559, "IEEE 754 required.");
            return -1 * std::numeric_limits<double>::infinity();
        }

        if (s == ".nan" || s == ".NaN" || s == ".NAN")
        {
            return std::nan("");
        }

        char* endptr = nullptr;
        const auto ret = std::strtod(s.data(), &endptr);

        if (endptr != s.data() + s.size())
        {
            throw exception("Failed to a string into a floating point number value.");
        }

        return ret;
}

template <>
inline long double from_string(const std::string& s, type_tag<long double> /*unused*/)
{
        if (s == ".inf" || s == ".Inf" || s == ".INF")
        {
            return std::numeric_limits<long double>::infinity();
        }

        if (s == "-.inf" || s == "-.Inf" || s == "-.INF")
        {
            static_assert(std::numeric_limits<long double>::is_iec559, "IEEE 754 required.");
            return -1 * std::numeric_limits<long double>::infinity();
        }

        if (s == ".nan" || s == ".NaN" || s == ".NAN")
        {
            return std::nanl("");
        }

        char* endptr = nullptr;
        const auto ret = std::strtold(s.data(), &endptr);

        if (endptr != s.data() + s.size())
        {
            throw exception("Failed to a string into a floating point number value.");
        }

        return ret;
}

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_CONVERSIONS_FROM_STRING_HPP_ */