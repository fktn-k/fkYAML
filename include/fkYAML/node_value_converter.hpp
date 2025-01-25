//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_NODE_VALUE_CONVERTER_HPP
#define FK_YAML_NODE_VALUE_CONVERTER_HPP

#include <utility>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/conversions/from_node.hpp>
#include <fkYAML/detail/conversions/to_node.hpp>

FK_YAML_NAMESPACE_BEGIN

/// @brief An ADL friendly converter between basic_node objects and native data objects.
/// @tparam ValueType A default target data type.
/// @sa https://fktn-k.github.io/fkYAML/api/node_value_converter/
template <typename ValueType, typename>
class node_value_converter {
public:
    /// @brief Convert a YAML node value into compatible native data.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @tparam TargetType A native data type for conversion.
    /// @param n A basic_node object.
    /// @param val A native data object.
    /// @sa https://fktn-k.github.io/fkYAML/api/node_value_converter/from_node/
    template <typename BasicNodeType, typename TargetType = ValueType>
    static auto from_node(BasicNodeType&& n, TargetType& val) noexcept(
        noexcept(::fkyaml::from_node(std::forward<BasicNodeType>(n), val)))
        -> decltype(::fkyaml::from_node(std::forward<BasicNodeType>(n), val), void()) {
        ::fkyaml::from_node(std::forward<BasicNodeType>(n), val);
    }

    /// @brief Convert compatible native data into a YAML node.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @tparam TargetType A native data type for conversion.
    /// @param n A basic_node object.
    /// @param val A native data object.
    /// @sa https://fktn-k.github.io/fkYAML/api/node_value_converter/to_node/
    template <typename BasicNodeType, typename TargetType = ValueType>
    static auto to_node(BasicNodeType& n, TargetType&& val) noexcept(noexcept(::fkyaml::to_node(
        n, std::forward<TargetType>(val)))) -> decltype(::fkyaml::to_node(n, std::forward<TargetType>(val))) {
        ::fkyaml::to_node(n, std::forward<TargetType>(val));
    }
};

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_NODE_VALUE_CONVERTER_HPP */
