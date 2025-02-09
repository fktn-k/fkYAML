//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_META_INPUT_ADAPTER_TRAITS_HPP
#define FK_YAML_DETAIL_META_INPUT_ADAPTER_TRAITS_HPP

#include <type_traits>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/meta/detect.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

///////////////////////////////////////////
//   Input Adapter API detection traits
///////////////////////////////////////////

/// @brief A type which represents get_buffer_view function.
/// @tparam T A target type.
template <typename T>
using get_buffer_view_fn_t = decltype(std::declval<T>().get_buffer_view());

/// @brief Type traits to check if InputAdapterType has get_buffer_view member function.
/// @tparam InputAdapterType An input adapter type to check if it has get_buffer_view function.
/// @tparam typename N/A
template <typename InputAdapterType>
using has_get_buffer_view = is_detected<get_buffer_view_fn_t, InputAdapterType>;

////////////////////////////////
//   is_input_adapter traits
////////////////////////////////

/// @brief Type traits to check if T is an input adapter type.
/// @tparam T A target type.
/// @tparam typename N/A
template <typename T, typename = void>
struct is_input_adapter : std::false_type {};

/// @brief A partial specialization of is_input_adapter if T is an input adapter type.
/// @tparam InputAdapterType
template <typename InputAdapterType>
struct is_input_adapter<InputAdapterType, enable_if_t<has_get_buffer_view<InputAdapterType>::value>> : std::true_type {
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_META_INPUT_ADAPTER_TRAITS_HPP */
