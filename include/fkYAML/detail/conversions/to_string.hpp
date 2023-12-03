///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.2.3
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef TO__string_HPP_
#define TO__string_HPP_

#include <cmath>
#include <limits>
#include <string>
#include <sstream>
#include <type_traits>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>

/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief Converts a ValueType object to a string YAML token.
/// @tparam ValueType A source value type.
/// @tparam CharType The type of characters for the conversion result.
/// @param s A resulting output string.
/// @param v A source value.
template <typename ValueType, typename CharType>
inline void to_string(std::basic_string<CharType>& s, ValueType v);

/// @brief Specialization of to_string() for null values.
/// @param s A resulting string YAML token.
/// @param (unused) nullptr
template <>
inline void to_string(std::string& s, std::nullptr_t /*unused*/)
{
    s = "null";
}

/// @brief Specialization of to_string() for booleans.
/// @param s A resulting string YAML token.
/// @param b A boolean source value.
template <>
inline void to_string(std::string& s, bool b)
{
    s = b ? "true" : "false";
}

/// @brief Specialization of to_string() for integers.
/// @tparam IntegerType An integer type.
/// @param s A resulting string YAML token.
/// @param i An integer source value.
template <typename IntegerType>
inline enable_if_t<is_non_bool_integral<IntegerType>::value> to_string(std::string& s, IntegerType i)
{
    s = std::to_string(i);
}

/// @brief Specialization of to_string() for floating point numbers.
/// @tparam FloatType A floating point number type.
/// @param s A resulting string YAML token.
/// @param f A floating point number source value.
template <typename FloatType>
inline enable_if_t<std::is_floating_point<FloatType>::value> to_string(std::string& s, FloatType f)
{
    if (std::isnan(f))
    {
        s = ".nan";
        return;
    }

    if (std::isinf(f))
    {
        if (f == std::numeric_limits<FloatType>::infinity())
        {
            s = ".inf";
        }
        else
        {
            s = "-.inf";
        }
        return;
    }

    std::ostringstream oss;
    oss << f;
    s = oss.str();
}

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* TO__string_HPP_ */
