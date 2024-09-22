//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.12
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_TYPES_YAML_VERSION_T_HPP_
#define FK_YAML_DETAIL_TYPES_YAML_VERSION_T_HPP_

#include <cstdint>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/yaml_version_type.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Definition of YAML version types.
enum class yaml_version_t : std::uint32_t {
    VER_1_1, //!< YAML version 1.1
    VER_1_2, //!< YAML version 1.2
};

inline yaml_version_t convert_from_yaml_version_type(yaml_version_type t) noexcept {
    switch (t) {
    case yaml_version_type::VERSION_1_1:
        return yaml_version_t::VER_1_1;
    case yaml_version_type::VERSION_1_2:
        return yaml_version_t::VER_1_2;
    default:                            // LCOV_EXCL_LINE
        return yaml_version_t::VER_1_2; // LCOV_EXCL_LINE
    }
}

inline yaml_version_type convert_to_yaml_version_type(yaml_version_t t) noexcept {
    switch (t) {
    case yaml_version_t::VER_1_1:
        return yaml_version_type::VERSION_1_1;
    case yaml_version_t::VER_1_2:
        return yaml_version_type::VERSION_1_2;
    default:                                   // LCOV_EXCL_LINE
        return yaml_version_type::VERSION_1_2; // LCOV_EXCL_LINE
    }
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_TYPES_YAML_VERSION_T_HPP_ */
