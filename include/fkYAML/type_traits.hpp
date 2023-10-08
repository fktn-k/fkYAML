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

#ifndef FK_YAML_NODE_TYPE_TRAITS_HPP_
#define FK_YAML_NODE_TYPE_TRAITS_HPP_

#include <type_traits>

#include "fkYAML/version_macros.hpp"

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

// forward declaration for fkyaml::basic_node<...>
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType>
class basic_node;

/**
 * @struct is_basic_node
 * @brief A struct to check the template parameter class is a kind of basic_node template class.
 *
 * @tparam T A class to be checked if it's a kind of basic_node template class.
 */
template <typename T>
struct is_basic_node : std::false_type
{
};

/**
 * @brief A partial specialization of is_basic_node for basic_node template class.
 *
 * @tparam SequenceType A type for sequence node value containers.
 * @tparam MappingType A type for mapping node value containers.
 * @tparam BooleanType A type for boolean node values.
 * @tparam IntegerType A type for integer node values.
 * @tparam FloatNumberType A type for float number node values.
 * @tparam StringType A type for string node values.
 */
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType>
struct is_basic_node<basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType>>
    : std::true_type
{
};

#ifndef FK_YAML_HAS_CXX_17
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
 *
 * @tparam Types Any types to be transformed to void type.
 */
template <typename... Types>
using void_t = typename make_void<Types...>::type;
#else
using std::void_t;
#endif

#ifndef FK_YAML_HAS_CXX_14
/**
 * @brief An alias template for std::enable_if::type with C++11.
 * @note std::enable_if_t is available since C++14.
 *
 * @tparam Condition A condition tested at compile time.
 * @tparam T The type defined only if Condition is true.
 */
template <bool Condition, typename T = void>
using enable_if_t = typename std::enable_if<Condition, T>::type;
#else
using std::enable_if_t;
#endif

/**
 * @brief Type trait to check if T and U are comparable types.
 *
 * @tparam Comparator An object type to compare T and U objects.
 * @tparam T A type for comparison.
 * @tparam U The other type for comparison.
 * @tparam typename Placeholder for determining T and U are comparable types.
 */
template <typename Comparator, typename T, typename U, typename = void>
struct is_comparable : std::false_type
{
};

/**
 * @brief A partial specialization of is_comparable if T and U are comparable types.
 *
 * @tparam Comparator An object type to compare T and U objects.
 * @tparam T A type for comparison.
 * @tparam U Ther other type for comparison.
 */
template <typename Comparator, typename T, typename U>
struct is_comparable<
    Comparator, T, U,
    void_t<
        decltype(std::declval<Comparator>()(std::declval<T>(), std::declval<U>())),
        decltype(std::declval<Comparator>()(std::declval<U>(), std::declval<T>()))>> : std::true_type
{
};

/**
 * @brief Type trait to check if KeyType can be used as key type.
 *
 * @tparam Comparator An object type to compare T and U objects.
 * @tparam ObjectKeyType The original key type.
 * @tparam KeyType A type to be used as key type.
 */
template <typename Comparator, typename ObjectKeyType, typename KeyType>
using is_usable_as_key_type = typename std::conditional<
    is_comparable<Comparator, ObjectKeyType, KeyType>::value, std::true_type, std::false_type>::type;

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_NODE_TYPE_TRAITS_HPP_ */
