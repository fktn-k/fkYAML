//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_TYPES_YAML_VERSION_T_HPP
#define FK_YAML_DETAIL_TYPES_YAML_VERSION_T_HPP

#include <cstdint>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/yaml_version_type.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Definition of YAML version types.
enum class yaml_version_t : std::uint8_t {
    VER_1_1, //!< YAML version 1.1
    VER_1_2, //!< YAML version 1.2
};

/// @brief A namespace for conversion functions between yaml_version_t and yaml_version_type.
/// @note This namespace is for internal use only until `basic_node::yaml_version_t` is removed from the public API
/// to avoid name collision with conversion functions for `detail::version_type`.
namespace yaml_version_t_impl {

inline yaml_version_t from_yaml_version_type(yaml_version_type t) noexcept {
    switch (t) {
    case yaml_version_type::VERSION_1_1:
        return yaml_version_t::VER_1_1;
    case yaml_version_type::VERSION_1_2:
    default:
        return yaml_version_t::VER_1_2;
    }
}

inline yaml_version_type to_yaml_version_type(yaml_version_t t) noexcept {
    switch (t) {
    case yaml_version_t::VER_1_1:
        return yaml_version_type::VERSION_1_1;
    case yaml_version_t::VER_1_2:
    default:
        return yaml_version_type::VERSION_1_2;
    }
}

} /* namespace yaml_version_t_impl */

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_TYPES_YAML_VERSION_T_HPP */
