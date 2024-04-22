///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.4
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_META_NODE_TRAITS_HPP_
#define FK_YAML_DETAIL_META_NODE_TRAITS_HPP_

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/detect.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>

FK_YAML_NAMESPACE_BEGIN

// forward declaration for basic_node<...>
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType,
    template <typename, typename> class ConverterType>
class basic_node;

FK_YAML_NAMESPACE_END

FK_YAML_DETAIL_NAMESPACE_BEGIN

/////////////////////////////
//   is_basic_node traits
/////////////////////////////

/// @brief A struct to check the template parameter class is a kind of basic_node template class.
/// @tparam T A class to be checked if it's a kind of basic_node template class.
template <typename T>
struct is_basic_node : std::false_type {};

/// @brief A partial specialization of is_basic_node for basic_node template class.
/// @tparam SequenceType A type for sequence node value containers.
/// @tparam MappingType A type for mapping node value containers.
/// @tparam BooleanType A type for boolean node values.
/// @tparam IntegerType A type for integer node values.
/// @tparam FloatNumberType A type for float number node values.
/// @tparam StringType A type for string node values.
/// @tparam Converter A type for
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType,
    template <typename, typename> class Converter>
struct is_basic_node<
    basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, Converter>>
    : std::true_type {};

///////////////////////////////////
//   is_node_ref_storage traits
///////////////////////////////////

// forward declaration for node_ref_storage<...>
template <typename>
class node_ref_storage;

/// @brief A struct to check the template parameter class is a kind of node_ref_storage_template class.
/// @tparam T A type to be checked if it's a kind of node_ref_storage template class.
template <typename T>
struct is_node_ref_storage : std::false_type {};

/// @brief A partial specialization for node_ref_storage template class.
/// @tparam T A template parameter type of node_ref_storage template class.
template <typename T>
struct is_node_ref_storage<node_ref_storage<T>> : std::true_type {};

///////////////////////////////////////////////////////
//   basic_node conversion API representative types
///////////////////////////////////////////////////////

/// @brief A type represent from_node function.
/// @tparam T A type which provides from_node function.
/// @tparam Args Argument types passed to from_node function.
template <typename T, typename... Args>
using from_node_function_t = decltype(T::from_node(std::declval<Args>()...));

/// @brief A type which represent to_node function.
/// @tparam T A type which provides to_node function.
/// @tparam Args Argument types passed to to_node function.
template <typename T, typename... Args>
using to_node_funcion_t = decltype(T::to_node(std::declval<Args>()...));

///////////////////////////////////////////////////
//   basic_node conversion API detection traits
///////////////////////////////////////////////////

/// @brief Type traits to check if T is a compatible type for BasicNodeType in terms of from_node function.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A target type passed to from_node function.
/// @tparam typename N/A
template <typename BasicNodeType, typename T, typename = void>
struct has_from_node : std::false_type {};

/// @brief A partial specialization of has_from_node if T is not a basic_node template instance type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A target type passed to from_node function.
template <typename BasicNodeType, typename T>
struct has_from_node<BasicNodeType, T, enable_if_t<!is_basic_node<T>::value>> {
    using converter = typename BasicNodeType::template value_converter_type<T, void>;

    // NOLINTNEXTLINE(readability-identifier-naming)
    static constexpr bool value =
        is_detected_exact<void, from_node_function_t, converter, const BasicNodeType&, T&>::value;
};

/// @brief Type traits to check if T is a compatible type for BasicNodeType in terms of to_node function.
/// @warning Do not pass basic_node type as BasicNodeType to avoid infinite type instantiation.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A target type passed to to_node function.
/// @tparam typename N/A
template <typename BasicNodeType, typename T, typename = void>
struct has_to_node : std::false_type {};

/// @brief A partial specialization of has_to_node if T is not a basic_node template instance type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A target type passed to to_node function.
template <typename BasicNodeType, typename T>
struct has_to_node<BasicNodeType, T, enable_if_t<!is_basic_node<T>::value>> {
    using converter = typename BasicNodeType::template value_converter_type<T, void>;

    // NOLINTNEXTLINE(readability-identifier-naming)
    static constexpr bool value = is_detected_exact<void, to_node_funcion_t, converter, BasicNodeType&, T>::value;
};

///////////////////////////////////////
//   is_node_compatible_type traits
///////////////////////////////////////

/// @brief Type traits implementation of is_node_compatible_type to check if CompatibleType is a compatible type for
/// BasicNodeType.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam CompatibleType A target type for compatibility check.
/// @tparam typename N/A
template <typename BasicNodeType, typename CompatibleType, typename = void>
struct is_node_compatible_type_impl : std::false_type {};

/// @brief A partial specialization of is_node_compatible_type_impl if CompatibleType is a complete type and is
/// compatible for BasicNodeType.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam CompatibleType A target type for compatibility check.
template <typename BasicNodeType, typename CompatibleType>
struct is_node_compatible_type_impl<
    BasicNodeType, CompatibleType,
    enable_if_t<conjunction<is_complete_type<CompatibleType>, has_to_node<BasicNodeType, CompatibleType>>::value>>
    : std::true_type {};

/// @brief Type traits to check if CompatibleType is a compatible type for BasicNodeType.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam CompatibleType A target type for compatibility check.
template <typename BasicNodeType, typename CompatibleType>
struct is_node_compatible_type : is_node_compatible_type_impl<BasicNodeType, CompatibleType> {};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_META_NODE_TRAITS_HPP_ */
