///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_META_INPUT_ADAPTER_TRAITS_HPP_
#define FK_YAML_DETAIL_META_INPUT_ADAPTER_TRAITS_HPP_

#include <type_traits>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/detect.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>

/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/////////////////////////////////
//   API representative types
/////////////////////////////////

/// @brief A type which represents T::char_type;
/// @tparam T A target type to check if it has char_type;
template <typename T>
using detect_char_type_helper_t = typename T::char_type;

/// @brief A type which represents get_character function.
/// @tparam T A target type.
template <typename T>
using get_character_fn_t = decltype(std::declval<T>().get_character());

/// @brief Type traits to check if T has char_type as its member.
/// @tparam T A target type.
/// @tparam typename N/A
template <typename T, typename = void>
struct has_char_type : std::false_type
{
};

///////////////////////////////////////////
//   Input Adapter API detection traits
///////////////////////////////////////////

/// @brief A partial specialization of has_char_type if T has char_type as its member.
/// @tparam T A target type.
template <typename T>
struct has_char_type<T, enable_if_t<is_detected<detect_char_type_helper_t, T>::value>> : std::true_type
{
};

/// @brief Type traits to check if InputAdapterType has get_character member function.
/// @tparam InputAdapterType An input adapter type to check if it has get_character function.
/// @tparam typename N/A
template <typename InputAdapterType, typename = void>
struct has_get_character : std::false_type
{
};

/// @brief A partial specialization of has_get_character if InputAdapterType has get_character member function.
/// @tparam InputAdapterType A type of a target input adapter.
template <typename InputAdapterType>
struct has_get_character<InputAdapterType, enable_if_t<is_detected<get_character_fn_t, InputAdapterType>::value>>
    : std::true_type
{
};

////////////////////////////////
//   is_input_adapter traits
////////////////////////////////

/// @brief Type traits to check if T is an input adapter type.
/// @tparam T A target type.
/// @tparam typename N/A
template <typename T, typename = void>
struct is_input_adapter : std::false_type
{
};

/// @brief A partial specialization of is_input_adapter if T is an input adapter type.
/// @tparam InputAdapterType
template <typename InputAdapterType>
struct is_input_adapter<
    InputAdapterType,
    enable_if_t<conjunction<has_char_type<InputAdapterType>, has_get_character<InputAdapterType>>::value>>
    : std::true_type
{
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_META_INPUT_ADAPTER_TRAITS_HPP_ */