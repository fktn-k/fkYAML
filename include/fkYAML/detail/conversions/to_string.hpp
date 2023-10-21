/**
 *  _______   __ __   __  _____   __  __  __
 * |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
 * |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.1.3
 * |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
 *
 * SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
 * SPDX-License-Identifier: MIT
 *
 * @file
 */

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

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

/**
 * @namespace detail
 * @brief namespace for internal implementations of fkYAML library.
 */
namespace detail
{

template <typename ValueType, typename CharType>
inline void to_string(std::basic_string<CharType>& s, ValueType);

template <>
inline void to_string(std::string& s, std::nullptr_t /*unused*/)
{
    s = "null";
}

template <>
inline void to_string(std::string& s, bool b)
{
    s = b ? "true" : "false";
}

template <typename IntegerType>
inline enable_if_t<is_non_bool_integral<IntegerType>::value> to_string(std::string& s, IntegerType i)
{
    s = std::to_string(i);
}

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
