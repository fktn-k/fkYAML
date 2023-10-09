/**
 *  _______   __ __   __  _____   __  __  __
 * |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
 * |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.1
 * |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
 *
 * SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
 * SPDX-License-Identifier: MIT
 *
 * @file
 */

#ifndef FK_YAML_DETAIL_STL_SUPPLEMENT_HPP_
#define FK_YAML_DETAIL_STL_SUPPLEMENT_HPP_

#include <type_traits>

#include "fkYAML/detail/version_macros.hpp"

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//   For contributors:
//     This file is for supplementing future C++ STL implementations to utilize some useful features
//     implemented in C++14 or later.
//     This file is needed to keep the fkYAML library requirement to C++11.
//     **DO NOT** implement features which are not included any version of STL in this file.
//     Such implementations must be in the type_traits.hpp file.
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FK_YAML_HAS_CXX_14

/**
 * @brief An alias template for std::enable_if::type with C++11.
 * @note std::enable_if_t is available since C++14.
 * @sa https://en.cppreference.com/w/cpp/types/enable_if
 *
 * @tparam Condition A condition tested at compile time.
 * @tparam T The type defined only if Condition is true.
 */
template <bool Condition, typename T = void>
using enable_if_t = typename std::enable_if<Condition, T>::type;

#else

using std::enable_if_t;

#endif

#ifndef FK_YAML_HAS_CXX_17

/**
 * @brief A simple implementation to use std::void_t even with C++11-14.
 * @note
 * std::conjunction is available since C++17.
 * This is applied when no traits are specified as inputs.
 * @sa https://en.cppreference.com/w/cpp/types/conjunction
 *
 * @tparam Traits Type traits to be checked if their ::value are all true.
 */
template <typename... Traits>
struct conjunction : std::true_type
{
};

/**
 * @brief A partial specialization of conjunction if only one Trait is given.
 *
 * @tparam Trait Type trait to be checked if its ::value is true.
 */
template <typename Trait>
struct conjunction<Trait> : Trait
{
};

/**
 * @brief A partial specialization of conjunction if more than one traits are given.
 *
 * @tparam First The first type trait to be checked if its ::value is true.
 * @tparam Rest The rest of traits passed as another conjunction template arguments if First::value is true.
 */
template <typename First, typename... Rest>
struct conjunction<First, Rest...> : std::conditional<First::value, conjunction<Rest...>, First>::type
{
};

/**
 * @brief A simple implementation to use std::disjunction even with C++11-14.
 * @note
 * std::disjunction is available since C++17.
 * This is applied when no traits are specified as inputs.
 * @sa https://en.cppreference.com/w/cpp/types/disjunction
 *
 * @tparam Traits Type traits to be checked if at least one of their ::value is true.
 */
template <typename... Traits>
struct disjunction : std::false_type
{
};

/**
 * @brief A partial specialization of disjunction if only one Trait is given.
 *
 * @tparam Trait Type trait to be checked if its ::value is true.
 */
template <typename Trait>
struct disjunction<Trait> : Trait
{
};

/**
 * @brief A partial specialization of disjunction if more than one traits are given.
 *
 * @tparam First The first type trait to be checked if its ::value is true.
 * @tparam Rest The rest of traits passed as another conjunction template arguments if First::value is false.
 */
template <typename First, typename... Rest>
struct disjunction<First, Rest...> : std::conditional<First::value, First, disjunction<Rest...>>::type
{
};

/**
 * @brief A simple implementation to use std::negation even with C++11-14.
 * @note std::negation is available since C++17.
 * @sa https://en.cppreference.com/w/cpp/types/negation
 *
 * @tparam Trait Type trait whose ::value is negated.
 */
template <typename Trait>
struct negation : std::integral_constant<bool, !Trait::value>
{
};

/**
 * @brief A helper for void_t.
 *
 * @tparam Types Any types to be transformed to void type.
 */
template <typename... Types>
struct make_void
{
    using type = void;
};

/**
 * @brief A simple implementation to use std::void_t even with C++11-14.
 * @note std::void_t is available since C++17.
 * @sa https://en.cppreference.com/w/cpp/types/void_t
 *
 * @tparam Types Any types to be transformed to void type.
 */
template <typename... Types>
using void_t = typename make_void<Types...>::type;

#else

using std::conjunction;
using std::disjunction;
using std::negation;
using std::void_t;

#endif

#ifndef FK_YAML_HAS_CXX_20

/**
 * @brief A simple implementation to use std::remove_cvref_t with C++11-17.
 * @note std::remove_cvref & std::remove_cvref_t are available since C++20.
 * @sa https://en.cppreference.com/w/cpp/types/remove_cvref
 *
 * @tparam T A type from which cv-qualifiers and reference are removed.
 */
template <typename T>
using remove_cvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

#else

using std::remove_cvref_t;

#endif

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_STL_SUPPLEMENT_HPP_ */