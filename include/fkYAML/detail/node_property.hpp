///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
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

/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

enum class anchor_status_t
{
    NONE,
    ANCHOR,
    ALIAS,
};

struct node_property
{
    std::string tag {};
    anchor_status_t anchor_status {anchor_status_t::NONE};
    std::string anchor {};
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_NODE_PROPERTY_HPP_ */