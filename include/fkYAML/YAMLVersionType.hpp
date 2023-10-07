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
 * @enum yaml_version_t
 * @brief Definition of YAML version types.
 */
enum class yaml_version_t : std::uint32_t
{
    VER_1_1, //!< YAML version 1.1
    VER_1_2, //!< YAML version 1.2
};

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_YAML_VERSION_TYPE_HPP_ */
