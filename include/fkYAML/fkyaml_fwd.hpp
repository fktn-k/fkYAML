//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_FKYAML_FWD_HPP
#define FK_YAML_FKYAML_FWD_HPP

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <fkYAML/detail/macros/version_macros.hpp>

FK_YAML_NAMESPACE_BEGIN

/// @brief An ADL friendly converter between basic_node objects and native data objects.
/// @tparam ValueType A target data type.
/// @sa https://fktn-k.github.io/fkYAML/api/node_value_converter/
template <typename ValueType, typename = void>
class node_value_converter;

/// @brief A class to store value of YAML nodes.
/// @sa https://fktn-k.github.io/fkYAML/api/basic_node/
template <
    template <typename, typename...> class SequenceType = std::vector,
    template <typename, typename, typename...> class MappingType = std::map, typename BooleanType = bool,
    typename IntegerType = std::int64_t, typename FloatNumberType = double, typename StringType = std::string,
    template <typename, typename = void> class ConverterType = node_value_converter>
class basic_node;

/// @brief default YAML node value container.
/// @sa https://fktn-k.github.io/fkYAML/api/basic_node/node/
using node = basic_node<>;

/// @brief A minimal map-like container which preserves insertion order.
/// @tparam Key A type for keys.
/// @tparam Value A type for values.
/// @tparam IgnoredCompare A placeholder for key comparison. This will be ignored.
/// @tparam Allocator A class for allocators.
/// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/
template <typename Key, typename Value, typename IgnoredCompare, typename Allocator>
class ordered_map;

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_FKYAML_FWD_HPP */
