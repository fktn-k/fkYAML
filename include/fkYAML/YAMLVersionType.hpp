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

#include "fkYAML/VersioningMacros.hpp"

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

/**
 * @enum YamlVersionType
 * @brief Definition of YAML version types.
 */
enum class YamlVersionType : std::uint32_t
{
    VER_1_1, //!< YAML version 1.1
    VER_1_2, //!< YAML version 1.2
};

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_YAML_VERSION_TYPE_HPP_ */
