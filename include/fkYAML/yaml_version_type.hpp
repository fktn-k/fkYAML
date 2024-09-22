//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.12
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_YAML_VERSION_TYPE_HPP_
#define FK_YAML_YAML_VERSION_TYPE_HPP_

#include <cstdint>

#include <fkYAML/detail/macros/version_macros.hpp>

FK_YAML_NAMESPACE_BEGIN

enum class yaml_version_type : std::uint32_t {
    VERSION_1_1, //!< YAML version 1.1
    VERSION_1_2, //!< YAML version 1.2
};

inline const char* to_string(yaml_version_type t) noexcept {
    switch (t) {
    case yaml_version_type::VERSION_1_1:
        return "VERSION_1_1";
    case yaml_version_type::VERSION_1_2:
        return "VERSION_1_2";
    default:       // LCOV_EXCL_LINE
        return ""; // LCOV_EXCL_LINE
    }
}

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_YAML_VERSION_TYPE_HPP_ */
