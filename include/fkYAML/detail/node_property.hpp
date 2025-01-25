//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_NODE_PROPERTY_HPP
#define FK_YAML_DETAIL_NODE_PROPERTY_HPP

#include <string>

#include <fkYAML/detail/macros/define_macros.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

struct node_property {
    /// The tag name property.
    std::string tag {}; // NOLINT(readability-redundant-member-init) necessary for older compilers
    /// The anchor name property.
    std::string anchor {}; // NOLINT(readability-redundant-member-init) necessary for older compilers
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_NODE_PROPERTY_HPP */
