///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_NODE_PROPERTY_HPP_
#define FK_YAML_DETAIL_NODE_PROPERTY_HPP_

#include <string>

#include <fkYAML/detail/macros/version_macros.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

enum class anchor_status_t {
    NONE,
    ANCHOR,
    ALIAS,
};

struct node_property {
    /// The tag name property.
    std::string tag {};
    /// The status regarding node anchoring/aliasing.
    anchor_status_t anchor_status {anchor_status_t::NONE};
    /// The anchor name property.
    std::string anchor {};
    /// The offset index value used to reference the anchor node implementation.
    uint32_t anchor_offset {0};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_NODE_PROPERTY_HPP_ */
