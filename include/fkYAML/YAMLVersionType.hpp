/**
 * @file YAMLVersionType.hpp
 * @brief Definitions of YAML version specification types.
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_YAML_VERSION_TYPE_HPP_
#define FK_YAML_YAML_VERSION_TYPE_HPP_

#include <cstdint>

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
namespace fkyaml
{

/**
 * @enum YamlVersionType
 * @brief Definition of YAML version types.
 */
enum class YamlVersionType : std::uint32_t
{
    VER_1_1, //!< YAML version 1.1
    VER_1_2, //!< YAML version 1.2
};

}; // namespace fkyaml

#endif /* FK_YAML_YAML_VERSION_TYPE_HPP_ */
