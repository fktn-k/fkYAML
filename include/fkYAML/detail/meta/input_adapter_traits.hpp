///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_META_INPUT_ADAPTER_TRAITS_HPP_
#define FK_YAML_DETAIL_META_INPUT_ADAPTER_TRAITS_HPP_

#include <string>
#include <type_traits>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/detect.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

///////////////////////////////////////////
//   Input Adapter API detection traits
///////////////////////////////////////////

/// @brief A type which represents get_character function.
/// @tparam T A target type.
template <typename T>
using fill_buffer_fn_t = decltype(std::declval<T>().fill_buffer(std::declval<std::string&>()));

/// @brief Type traits to check if InputAdapterType has get_character member function.
/// @tparam InputAdapterType An input adapter type to check if it has get_character function.
/// @tparam typename N/A
template <typename InputAdapterType, typename = void>
struct has_fill_buffer : std::false_type {};

/// @brief A partial specialization of has_fill_buffer if InputAdapterType has get_character member function.
/// @tparam InputAdapterType A type of a target input adapter.
template <typename InputAdapterType>
struct has_fill_buffer<InputAdapterType, enable_if_t<is_detected<fill_buffer_fn_t, InputAdapterType>::value>>
    : std::true_type {};

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
struct is_input_adapter<InputAdapterType, enable_if_t<has_fill_buffer<InputAdapterType>::value>> : std::true_type {};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_META_INPUT_ADAPTER_TRAITS_HPP_ */
