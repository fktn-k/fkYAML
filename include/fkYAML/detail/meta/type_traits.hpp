//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.13
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_META_TYPE_TRAITS_HPP
#define FK_YAML_DETAIL_META_TYPE_TRAITS_HPP

#include <iterator>
#include <limits>
#include <type_traits>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/detect.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Type trait to check if T and U are comparable types.
/// @tparam Comparator An object type to compare T and U objects.
/// @tparam T A type for comparison.
/// @tparam U The other type for comparison.
/// @tparam typename Placeholder for determining T and U are comparable types.
template <typename Comparator, typename T, typename U, typename = void>
struct is_comparable : std::false_type {};

/// @brief A partial specialization of is_comparable if T and U are comparable types.
/// @tparam Comparator An object type to compare T and U objects.
/// @tparam T A type for comparison.
/// @tparam U The other type for comparison.
template <typename Comparator, typename T, typename U>
struct is_comparable<
    Comparator, T, U,
    void_t<
        decltype(std::declval<Comparator>()(std::declval<T>(), std::declval<U>())),
        decltype(std::declval<Comparator>()(std::declval<U>(), std::declval<T>()))>> : std::true_type {};

/// @brief Type trait to check if KeyType can be used as key type.
/// @tparam Comparator An object type to compare T and U objects.
/// @tparam ObjectKeyType The original key type.
/// @tparam KeyType A type to be used as key type.
template <typename Comparator, typename ObjectKeyType, typename KeyType>
using is_usable_as_key_type = typename std::conditional<
    is_comparable<Comparator, ObjectKeyType, KeyType>::value, std::true_type, std::false_type>::type;

/// @brief Type trait to check if IntegralType is of non-boolean integral types.
/// @tparam IntegralType A type to be checked.
/// @tparam typename N/A
template <typename IntegralType, typename = void>
struct is_non_bool_integral : std::false_type {};

/// @brief A partial specialization of is_non_bool_integral if IntegralType is of non-boolean integral types.
/// @tparam IntegralType A type to be checked.
template <typename IntegralType>
struct is_non_bool_integral<
    IntegralType,
    enable_if_t<conjunction<std::is_integral<IntegralType>, negation<std::is_same<bool, IntegralType>>>::value>>
    : std::true_type {};

/// @brief Type traits to check if Types are all signed arithmetic types.
/// @tparam Types Types to check if they are all signed arithmetic types.
template <typename... Types>
using is_all_signed = conjunction<std::is_signed<Types>...>;

/// @brief Type traits to check if Types are all unsigned arithmetic types.
/// @tparam Types Types to check if they are all unsigned arithmetic types.
template <typename... Types>
using is_all_unsigned = conjunction<std::is_unsigned<Types>...>;

/// @brief Type trait implementation to check if TargetIntegerType and CompatibleIntegerType are compatible integer
/// types.
/// @tparam TargetIntegerType A target integer type.
/// @tparam CompatibleIntegerType A compatible integer type.
/// @tparam typename N/A
template <typename TargetIntegerType, typename CompatibleIntegerType, typename = void>
struct is_compatible_integer_type_impl : std::false_type {};

/// @brief A partial specialization of is_compatible_integer_type_impl if TargetIntegerType and CompatibleIntegerType
/// are compatible integer types.
/// @tparam TargetIntegerType A target integer type.
/// @tparam CompatibleIntegerType A compatible integer type.
template <typename TargetIntegerType, typename CompatibleIntegerType>
struct is_compatible_integer_type_impl<
    TargetIntegerType, CompatibleIntegerType, enable_if_t<is_non_bool_integral<CompatibleIntegerType>::value>>
    : std::true_type {};

/// @brief Type traits to check if TargetIntegerType and CompatibleIntegerType are compatible integer types.
/// @tparam TargetIntegerType A target integer type.
/// @tparam CompatibleIntegerType A compatible integer type.
template <typename TargetIntegerType, typename CompatibleIntegerType>
struct is_compatible_integer_type : is_compatible_integer_type_impl<TargetIntegerType, CompatibleIntegerType> {};

/// @brief Type traits to check if T is a complete type.
/// @tparam T A type to be checked if a complete type.
/// @tparam typename N/A
template <typename T, typename = void>
struct is_complete_type : std::false_type {};

/// @brief A partial specialization of is_complete_type if T is a complete type.
/// @tparam T
template <typename T>
struct is_complete_type<T, decltype(void(sizeof(T)))> : std::true_type {};

/// @brief A utility alias to test if the value type of `ItrType` is `T`.
/// @tparam ItrType An iterator type.
/// @tparam T The target iterator value type.
template <typename ItrType, typename T>
using is_iterator_of = std::is_same<remove_cv_t<typename std::iterator_traits<ItrType>::value_type>, T>;

/// @brief A utility struct to generate static constant instance.
/// @tparam T A target type for the resulting static constant instance.
template <typename T>
struct static_const {
    static FK_YAML_INLINE_VAR constexpr T value {}; // NOLINT(readability-identifier-naming)
};

#ifndef FK_YAML_HAS_CXX_17
/// @brief A instantiation of static_const::value instance.
/// @note This is required if inline variables are not available. C++11-14 do not provide such a feature yet.
/// @tparam T A target type for the resulting static constant instance.
template <typename T>
constexpr T static_const<T>::value;
#endif

/// @brief A helper structure for tag dispatch.
/// @tparam T A tag type.
template <typename T>
struct type_tag {
    /// @brief A tagged type.
    using type = T;
};

/// @brief A utility struct to retrieve the first type in variadic template arguments.
/// @tparam Types Types of variadic template arguments.
template <typename... Types>
struct get_head_type;

/// @brief A specialization of get_head_type if variadic template has no arguments.
/// @tparam  N/A
template <>
struct get_head_type<> {
    /// @brief A head type
    using type = void;
};

/// @brief A partial specialization of get_head_type if variadic template has one or more argument(s).
/// @tparam First The first type in the arguments
/// @tparam Rest The rest of the types in the arguments.
template <typename First, typename... Rest>
struct get_head_type<First, Rest...> {
    /// @brief A head type.
    using type = First;
};

/// @brief An alias template to retrieve the first type in variadic template arguments.
/// @tparam Types Types of variadic template arguments.
template <typename... Types>
using head_type = typename get_head_type<Types...>::type;

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_META_TYPE_TRAITS_HPP */
