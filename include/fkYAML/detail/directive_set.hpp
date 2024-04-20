///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.4
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_DIRECTIVE_SET_HPP_
#define FK_YAML_DETAIL_DIRECTIVE_SET_HPP_

#include <string>
#include <map>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/types/yaml_version_t.hpp>

/// @namespace fkyaml
/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @namespace detail
/// @brief namespace for internal implementaions of fkYAML library.
namespace detail
{

/// @brief The set of directives for a YAML document.
struct directive_set
{
    /// The YAML version used for the YAML document.
    yaml_version_t version {yaml_version_t::VER_1_2};
    /// Whether or not the YAML version has been specified.
    bool is_version_specified {false};
    /// The prefix of the primary handle.
    std::string primary_handle_prefix {};
    /// The prefix of the secondary handle.
    std::string secondary_handle_prefix {};
    /// The map of handle-prefix pairs.
    std::map<std::string /*handle*/, std::string /*prefix*/> named_handle_map {};
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_DIRECTIVE_SET_HPP_ */
