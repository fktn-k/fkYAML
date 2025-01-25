//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_META_DETECT_HPP
#define FK_YAML_DETAIL_META_DETECT_HPP

#include <iterator>
#include <type_traits>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief A dummy struct to represent detection failure.
struct nonesuch {
    nonesuch() = delete;
    ~nonesuch() = delete;
    nonesuch(const nonesuch&) = delete;
    nonesuch(nonesuch&&) = delete;
    nonesuch& operator=(const nonesuch&) = delete;
    nonesuch& operator=(nonesuch&&) = delete;
};

/// @brief namespace to implement detector type traits
namespace detector_impl {

/// @brief A helper for general type detection.
/// @tparam Default A type to represent detection failure.
/// @tparam AlwaysVoid This must be void type.
/// @tparam Op A type for desired operation type.
/// @tparam Args Argument types passed to desired operation.
template <typename Default, typename AlwaysVoid, template <typename...> class Op, typename... Args>
struct detector : std::false_type {
    /// @brief A type which represents detection failure.
    using type = Default;
};

/// @brief A partial specialization of detector if desired operation type is found.
/// @tparam Default A type to represent detection failure.
/// @tparam Op A type for desired operation type.
/// @tparam Args Argument types passed to desired operation.
template <typename Default, template <typename...> class Op, typename... Args>
struct detector<Default, void_t<Op<Args...>>, Op, Args...> : std::true_type {
    /// @brief A detected type.
    using type = Op<Args...>;
};

} // namespace detector_impl

/// @brief Type traits to detect Op operation with Args argument types
/// @tparam Op A desired operation type.
/// @tparam Args Argument types passed to desired operation.
template <template <typename...> class Op, typename... Args>
using is_detected = detector_impl::detector<nonesuch, void, Op, Args...>;

/// @brief Type traits to represent a detected type.
/// @tparam Op A type for desired operation type.
/// @tparam Args Argument types passed to desired operation.
template <template <typename...> class Op, typename... Args>
using detected_t = typename detector_impl::detector<nonesuch, void, Op, Args...>::type;

/// @brief Type traits to check if Expected and a detected type are exactly the same.
/// @tparam Expected An expected detection result type.
/// @tparam Op A type for desired operation.
/// @tparam Args Argument types passed to desired operation.
template <typename Expected, template <typename...> class Op, typename... Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

/// @brief namespace for member type detections of aliases and functions.
namespace detect {

/// @brief The type which represents `iterator` member type.
/// @tparam T A target type.
template <typename T>
using iterator_t = typename T::iterator;

/// @brief The type which represents `key_type` member type.
/// @tparam T A target type.
template <typename T>
using key_type_t = typename T::key_type;

/// @brief The type which represents `mapped_type` member type.
/// @tparam T A target type.
template <typename T>
using mapped_type_t = typename T::mapped_type;

/// @brief The type which represents `value_type` member type.
/// @tparam T A target type.
template <typename T>
using value_type_t = typename T::value_type;

/// @brief The type which represents `difference_type` member type.
/// @tparam T A target type.
template <typename T>
using difference_type_t = typename T::difference_type;

/// @brief The type which represents `pointer` member type.
/// @tparam T A target type.
template <typename T>
using pointer_t = typename T::pointer;

/// @brief The type which represents `reference` member type.
/// @tparam T A target type.
template <typename T>
using reference_t = typename T::reference;

/// @brief The type which represents `iterator_category` member type.
/// @tparam T A target type.
template <typename T>
using iterator_category_t = typename T::iterator_category;

/// @brief The type which represents `container_type` member type.
/// @tparam T A target type.
template <typename T>
using container_type_t = typename T::container_type;

/// @brief The type which represents emplace member function.
/// @tparam T A target type.
template <typename T, typename... Args>
using emplace_fn_t = decltype(std::declval<T>().emplace(std::declval<Args>()...));

/// @brief The type which represents reserve member function.
/// @tparam T A target type.
template <typename T>
using reserve_fn_t = decltype(std::declval<T>().reserve(std::declval<typename remove_cvref_t<T>::size_type>()));

/// @brief Type traits to check if T has `iterator` member type.
/// @tparam T A target type.
template <typename T>
using has_iterator = is_detected<iterator_t, remove_cvref_t<T>>;

/// @brief Type traits to check if T has `key_type` member type.
/// @tparam T A target type.
template <typename T>
using has_key_type = is_detected<key_type_t, remove_cvref_t<T>>;

/// @brief Type traits to check if T has `mapped_type` member type.
/// @tparam T A target type.
template <typename T>
using has_mapped_type = is_detected<mapped_type_t, remove_cvref_t<T>>;

/// @brief Type traits to check if T has `value_type` member type.
/// @tparam T A target type.
template <typename T>
using has_value_type = is_detected<value_type_t, remove_cvref_t<T>>;

/// @brief Type traits to check if T is a std::iterator_traits like type.
/// @tparam T A target type.
template <typename T>
struct is_iterator_traits : conjunction<
                                is_detected<difference_type_t, remove_cvref_t<T>>, has_value_type<remove_cvref_t<T>>,
                                is_detected<pointer_t, remove_cvref_t<T>>, is_detected<reference_t, remove_cvref_t<T>>,
                                is_detected<iterator_category_t, remove_cvref_t<T>>> {};

/// @brief Type traits to check if T has `container_type` member type.
/// @tparam T A target type.
template <typename T>
using has_container_type = is_detected<container_type_t, remove_cvref_t<T>>;

/// @brief Type traits to check if T has reserve member function.
/// @tparam T A target type.
template <typename T>
using has_reserve = is_detected<reserve_fn_t, T>;

// fallback to these STL functions.
using std::begin;
using std::end;

/// @brief Type traits to check if begin/end functions can be called on a T object.
/// @tparam T A target type.
template <typename T, typename = void>
struct has_begin_end : std::false_type {};

/// @brief Type traits to check if begin/end functions can be called on a T object.
/// @tparam T A target type.
template <typename T>
struct has_begin_end<T, void_t<decltype(begin(std::declval<T>()), end(std::declval<T>()))>> : std::true_type {};

} // namespace detect

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_META_DETECT_HPP */
