//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_TYPES_VERSION_TYPE_HPP
#define FK_YAML_DETAIL_TYPES_VERSION_TYPE_HPP

#include <cstdint>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/yaml_version_type.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Definition of YAML version types.
enum class version_type : std::uint8_t {
    NOT_SPECIFIED, //!< YAML version is not specified.
    VERSION_1_1,   //!< YAML version 1.1
    VERSION_1_2,   //!< YAML version 1.2
};

/// @brief Convert a yaml_version_type to a version_type.
/// @param t The YAML version type to convert.
/// @return The corresponding version_type value.
inline version_type convert_from_yaml_version_type(yaml_version_type t) noexcept {
    switch (t) {
    case yaml_version_type::VERSION_1_1:
        return version_type::VERSION_1_1;
    case yaml_version_type::VERSION_1_2:
    default:
        return version_type::VERSION_1_2;
    }
}

/// @brief Convert a version_type to a yaml_version_type.
/// @param t The version_type value to convert.
/// @return The corresponding yaml_version_type value.
inline yaml_version_type convert_to_yaml_version_type(version_type t) noexcept {
    switch (t) {
    case version_type::VERSION_1_1:
        return yaml_version_type::VERSION_1_1;
    case version_type::NOT_SPECIFIED: // 1.2 is the default version when not specified.
    case version_type::VERSION_1_2:
    default:
        return yaml_version_type::VERSION_1_2;
    }
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_TYPES_VERSION_TYPE_HPP */
