//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_DOCUMENT_METAINFO_HPP
#define FK_YAML_DETAIL_DOCUMENT_METAINFO_HPP

#include <string>
#include <map>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/yaml_version_type.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief The set of directives for a YAML document.
template <typename BasicNodeType, typename = enable_if_t<is_basic_node<BasicNodeType>::value>>
struct document_metainfo {
    /// The YAML version used for the YAML document.
    yaml_version_type version {yaml_version_type::VERSION_1_2};
    /// Whether the YAML version has been specified.
    bool is_version_specified {false};
    /// The prefix of the primary handle.
    std::string primary_handle_prefix;
    /// The prefix of the secondary handle.
    std::string secondary_handle_prefix;
    /// The map of handle-prefix pairs.
    std::map<std::string /*handle*/, std::string /*prefix*/> named_handle_map;
    /// The map of anchor node which allows for key duplication.
    std::multimap<std::string /*anchor name*/, BasicNodeType> anchor_table {};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_DOCUMENT_METAINFO_HPP */
