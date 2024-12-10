//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_META_INPUT_ADAPTER_TRAITS_HPP
#define FK_YAML_DETAIL_META_INPUT_ADAPTER_TRAITS_HPP

#include <array>
#include <string>
#include <type_traits>
#include <vector>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/meta/detect.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>

#if defined(FK_YAML_HAS_CXX_17) && FK_YAML_HAS_INCLUDE(<string_view>)
#include <string_view>
#endif

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
template <typename InputAdapterType, typename = void>
struct has_get_buffer_view : std::false_type {};

/// @brief A partial specialization of has_get_buffer_view if InputAdapterType has get_buffer_view member function.
/// @tparam InputAdapterType A type of a target input adapter.
template <typename InputAdapterType>
struct has_get_buffer_view<InputAdapterType, enable_if_t<is_detected<get_buffer_view_fn_t, InputAdapterType>::value>>
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
struct is_input_adapter<InputAdapterType, enable_if_t<has_get_buffer_view<InputAdapterType>::value>> : std::true_type {
};

/////////////////////////////////////////////////
//   traits for contiguous iterator detection
/////////////////////////////////////////////////

/// @brief Type traits to check if T is a container which has contiguous bytes.
/// @tparam T A target type.
template <typename T>
struct is_contiguous_container : std::false_type {};

/// @brief A partial specialization of is_contiguous_container if T is a std::array.
/// @tparam T Element type.
/// @tparam N Maximum number of elements.
template <typename T, std::size_t N>
struct is_contiguous_container<std::array<T, N>> : std::true_type {};

/// @brief A partial specialization of is_contiguous_container if T is a std::basic_string.
/// @tparam CharT Character type.
/// @tparam Traits Character traits type.
/// @tparam Alloc Allocator type.
template <typename CharT, typename Traits, typename Alloc>
struct is_contiguous_container<std::basic_string<CharT, Traits, Alloc>> : std::true_type {};

#ifdef FK_YAML_HAS_CXX_17

/// @brief A partial specialization of is_contiguous_container if T is a std::basic_string_view.
/// @tparam CharT Character type.
/// @tparam Traits Character traits type.
template <typename CharT, typename Traits>
struct is_contiguous_container<std::basic_string_view<CharT, Traits>> : std::true_type {};

#endif // defined(FK_YAML_HAS_CXX_20)

/// @brief A partial specialization of is_contiguous_container if T is a std::vector.
/// @tparam T Element type.
/// @tparam Alloc Allocator type.
template <typename T, typename Alloc>
struct is_contiguous_container<std::vector<T, Alloc>> : std::true_type {};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_META_INPUT_ADAPTER_TRAITS_HPP */
