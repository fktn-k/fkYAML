/**
 *  _______   __ __   __  _____   __  __  __
 * |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
 * |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.1
 * |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
 *
 * SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
 * SPDX-License-Identifier: MIT
 *
 * @file
 */

#ifndef ADL_SERIALIZER_HPP_
#define ADL_SERIALIZER_HPP_

#include <utility>

#include "fkYAML/version_macros.hpp"
#include "fkYAML/from_node.hpp"
#include "fkYAML/to_node.hpp"

FK_YAML_NAMESPACE_BEGIN

/**
 * @class adl_serializer
 * @brief An ADL friendly serializer for YAML nodes.
 *
 * @tparam ValueType A default output data type.
 * @tparam typename N/A
 */
template <typename ValueType, typename>
class adl_serializer
{
public:
    template <typename BasicNodeType, typename TargetType = ValueType>
    static auto from_node(BasicNodeType&& n, TargetType& val) noexcept(
        noexcept(::fkyaml::from_node(std::forward<BasicNodeType>(n), val)))
        -> decltype(::fkyaml::from_node(std::forward<BasicNodeType>(n), val), void())
    {
        ::fkyaml::from_node(std::forward<BasicNodeType>(n), val);
    }

    /**
     * @brief Convert compatible native data into a YAML node.
     *
     * @tparam BasicNodeType A basic_node template instance type.
     * @tparam TargetType A native data type for conversion.
     * @param n A basic_node object.
     * @param val A native data object.
     * @return decltype(::fkyaml::to_node(n, std::forward<TargetType>(val)))
     */
    template <typename BasicNodeType, typename TargetType = ValueType>
    static auto to_node(BasicNodeType& n, TargetType&& val) noexcept(
        noexcept(::fkyaml::to_node(n, std::forward<TargetType>(val))))
        -> decltype(::fkyaml::to_node(n, std::forward<TargetType>(val)))
    {
        ::fkyaml::to_node(n, std::forward<TargetType>(val));
    }
};

FK_YAML_NAMESPACE_END

#endif /* ADL_SERIALIZER_HPP_ */
