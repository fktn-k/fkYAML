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

#include <limits>
#include <type_traits>

#include "fkYAML/version_macros.hpp"
#include "fkYAML/stl_supplement.hpp"

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

// forward declaration for fkyaml::basic_node<...>
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType,
    template <typename, typename> class NodeSerializer>
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
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType,
    template <typename, typename> class NodeSerializer>
struct is_basic_node<
    basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, NodeSerializer>>
    : std::true_type
{
};

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
    detail::void_t<
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

/**
 * @brief Type trait to check if IntegralType is of non-boolean integral types.
 *
 * @tparam IntegralType A type to be checked.
 * @tparam typename N/A
 */
template <typename IntegralType, typename = void>
struct is_non_bool_integral : std::false_type
{
};

/**
 * @brief A partial specialization of is_non_bool_integral if IntegralType is of non-boolean integral types.
 *
 * @tparam IntegralType A type to be checked.
 */
template <typename IntegralType>
struct is_non_bool_integral<
    IntegralType,
    detail::enable_if_t<std::is_integral<IntegralType>::value && !std::is_same<bool, IntegralType>::value>>
    : std::true_type
{
};

/**
 * @brief Type trait implementation to check if TargetIntegerType and CompatibleIntegerType are compatible integer
 * types.
 *
 * @tparam TargetIntegerType A target integer type.
 * @tparam CompatibleIntegerType A compatible integer type.
 * @tparam typename N/A
 */
template <typename TargetIntegerType, typename CompatibleIntegerType, typename = void>
struct is_compatible_integer_type_impl : std::false_type
{
};

/**
 * @brief A partial specialization of is_compatible_integer_type_impl if TargetIntegerType and CompatibleIntegerType are
 * compatible integer types.
 *
 * @tparam TargetIntegerType A target integer type.
 * @tparam CompatibleIntegerType A compatible integer type.
 */
template <typename TargetIntegerType, typename CompatibleIntegerType>
struct is_compatible_integer_type_impl<
    TargetIntegerType, CompatibleIntegerType,
    detail::enable_if_t<
        std::is_integral<TargetIntegerType>::value && is_non_bool_integral<CompatibleIntegerType>::value &&
        std::is_constructible<TargetIntegerType, CompatibleIntegerType>::value &&
        std::numeric_limits<TargetIntegerType>::is_signed == std::numeric_limits<CompatibleIntegerType>::is_signed>>
    : std::true_type
{
};

/**
 * @brief Type traits to check if TargetIntegerType and CompatibleIntegerType are compatible integer types.
 *
 * @tparam TargetIntegerType A target integer type.
 * @tparam CompatibleIntegerType A compatible integer type.
 */
template <typename TargetIntegerType, typename CompatibleIntegerType>
struct is_compatible_integer_type : is_compatible_integer_type_impl<TargetIntegerType, CompatibleIntegerType>
{
};

/**
 * @brief Type traits to check if T is a complete type.
 *
 * @tparam T A type to be checked if a complete type.
 * @tparam typename N/A
 */
template <typename T, typename = void>
struct is_complete_type : std::false_type
{
};

/**
 * @brief A partial specialization of is_complete_type if T is a complete type.
 *
 * @tparam T
 */
template <typename T>
struct is_complete_type<T, decltype(void(sizeof(T)))> : std::true_type
{
};

/**
 * @struct none_such
 * @brief A dummy struct to represent detection failure.
 */
struct none_such
{
    none_such() = delete;
    ~none_such() = delete;
    none_such(const none_such&) = delete;
    none_such(none_such&&) = delete;
    none_such& operator=(const none_such&) = delete;
    none_such& operator=(none_such&&) = delete;
};

/**
 * @brief A helper for general type detection.
 *
 * @tparam Default A type to represent detection failure.
 * @tparam AlwaysVoid This must be void type.
 * @tparam Op A type for desired operation type.
 * @tparam Args Argument types passed to desired operation.
 */
template <typename Default, typename AlwaysVoid, template <typename...> class Op, typename... Args>
struct detect_helper : std::false_type
{
    using type = Default;
};

/**
 * @brief A partial specialization of detect_helper if desired operation type is found.
 *
 * @tparam Default A type to represent detection failure.
 * @tparam Op A type for desired operation type.
 * @tparam Args Argument types passed to desired operation.
 */
template <typename Default, template <typename...> class Op, typename... Args>
struct detect_helper<Default, detail::void_t<Op<Args...>>, Op, Args...> : std::true_type
{
    using type = Op<Args...>;
};

/**
 * @brief Type traits to detect Op operation with Args argument types
 *
 * @tparam Op A desired operation type.
 * @tparam Args Argument types passed to desired operation.
 */
template <template <typename...> class Op, typename... Args>
using is_detected = detect_helper<none_such, void, Op, Args...>;

/**
 * @brief Type traits to represent a detected type.
 *
 * @tparam Op A type for desired operation type.
 * @tparam Args Argument types passed to desired operation.
 */
template <template <typename...> class Op, typename... Args>
using detected_t = typename detect_helper<none_such, void, Op, Args...>::type;

/**
 * @brief Type traits to check if Expected and a detected type are exactly the same.
 *
 * @tparam Expected An expected detection result type.
 * @tparam Op A type for desired operation.
 * @tparam Args Argument types passed to desired operation.
 */
template <typename Expected, template <typename...> class Op, typename... Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

/**
 * @brief A type represent to_node function.
 *
 * @tparam BasicNodeType A basic_node template instance type.
 * @tparam Args A type passed to to_node function.
 */
template <typename BasicNodeType, typename... Args>
using to_node_funcion_t = decltype(BasicNodeType::to_node(std::declval<Args>()...));

/**
 * @brief Type traits to check if T is a compatible type for BasicNodeType.
 * @warning Do not pass basic_node type as BasicNodeType to avoid infinite type instantiation.
 *
 * @tparam BasicNodeType A basic_node template instance type.
 * @tparam T A target type passed to to_node function.
 * @tparam typename N/A
 */
template <typename BasicNodeType, typename T, typename = void>
struct has_to_node : std::false_type
{
};

/**
 * @brief A partial specialization of has_to_node if BasicNodeType is a basic_node template instance type.
 *
 * @tparam BasicNodeType A basic_node template instance type.
 * @tparam T A target type passed to to_node function.
 */
template <typename BasicNodeType, typename T>
struct has_to_node<BasicNodeType, T, detail::enable_if_t<!is_basic_node<T>::value>>
{
    using serializer = typename BasicNodeType::template node_serializer<T, void>;

    // NOLINTNEXTLINE(readability-identifier-naming)
    static constexpr bool value = is_detected_exact<void, to_node_funcion_t, serializer, BasicNodeType&, T>::value;
};

/**
 * @brief Type traits implementation of is_compatible_type to check if CompatibleType is a compatible type for
 * BasicNodeType.
 *
 * @tparam BasicNodeType A basic_node template instance type.
 * @tparam CompatibleType A target type for compatibility check.
 * @tparam typename N/A
 */
template <typename BasicNodeType, typename CompatibleType, typename = void>
struct is_compatible_type_impl : std::false_type
{
};

/**
 * @brief A partial specialization of is_compatible_type_impl if CompatibleType is a complete type and is compatible for
 * BasicNodeType.
 *
 * @tparam BasicNodeType A basic_node template instance type.
 * @tparam CompatibleType A target type for compatibility check.
 */
template <typename BasicNodeType, typename CompatibleType>
struct is_compatible_type_impl<
    BasicNodeType, CompatibleType,
    detail::enable_if_t<is_complete_type<CompatibleType>::value && has_to_node<BasicNodeType, CompatibleType>::value>>
    : std::true_type
{
};

/**
 * @brief Type traits to check if CompatibleType is a compatible type for BasicNodeType.
 *
 * @tparam BasicNodeType A basic_node template instance type.
 * @tparam CompatibleType A target type for compatibility check.
 */
template <typename BasicNodeType, typename CompatibleType>
struct is_compatible_type : is_compatible_type_impl<BasicNodeType, CompatibleType>
{
};

/**
 * @namespace detail
 * @brief namespace for internal implementations of fkYAML library.
 */
namespace detail
{

/**
 * @brief A utility struct to generate static constant instance.
 *
 * @tparam T A target type for the resulting static constant instance.
 */
template <typename T>
struct static_const
{
    static FK_YAML_INLINE_VAR constexpr T value {}; // NOLINT(readability-identifier-naming)
};

#ifndef FK_YAML_HAS_CXX_17
/**
 * @brief A instantiation of static_const::value instance.
 * @note This is required if inline variables are not available. C++11-14 do not provide such a feature yet.
 *
 * @tparam T A target type for the resulting static constant instance.
 */
template <typename T>
constexpr T static_const<T>::value;
#endif

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_NODE_TYPE_TRAITS_HPP_ */
