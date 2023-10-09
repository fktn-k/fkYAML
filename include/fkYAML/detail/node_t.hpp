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

#ifndef FK_YAML_DETAIL_NODE_T_HPP_
#define FK_YAML_DETAIL_NODE_T_HPP_

#include <cstdint>

#include "fkYAML/detail/version_macros.hpp"

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

/**
 * @namespace detail
 * @brief namespace for internal implementations of fkYAML library.
 */
namespace detail
{

/**
 * @enum node_t
 * @brief Definition of node value types.
 */
enum class node_t : std::uint32_t
{
    SEQUENCE,     //!< sequence value type
    MAPPING,      //!< mapping value type
    NULL_OBJECT,  //!< null value type
    BOOLEAN,      //!< boolean value type
    INTEGER,      //!< integer value type
    FLOAT_NUMBER, //!< float number value type
    STRING,       //!< string value type
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_NODE_T_HPP_ */
