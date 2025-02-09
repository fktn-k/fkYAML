//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_META_TYPE_TRAITS_HPP
#define FK_YAML_DETAIL_META_TYPE_TRAITS_HPP

#include <iterator>
#include <type_traits>

#include <fkYAML/detail/macros/define_macros.hpp>
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
using is_usable_as_key_type = is_comparable<Comparator, ObjectKeyType, KeyType>;

/// @brief Type trait to check if T is of non-boolean integral types.
/// @tparam T A type to be checked.
template <typename T>
using is_non_bool_integral = conjunction<std::is_integral<T>, negation<std::is_same<bool, T>>>;

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

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_META_TYPE_TRAITS_HPP */
