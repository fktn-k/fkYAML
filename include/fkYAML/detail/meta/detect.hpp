//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.13
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_META_DETECT_HPP
#define FK_YAML_DETAIL_META_DETECT_HPP

#include <type_traits>

#include <fkYAML/detail/macros/version_macros.hpp>
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

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_META_DETECT_HPP */
