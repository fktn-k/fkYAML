///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_TYPES_NODE_T_HPP_
#define FK_YAML_DETAIL_TYPES_NODE_T_HPP_

#include <cstdint>
#include <string>

#include <fkYAML/detail/macros/version_macros.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Definition of node value types.
enum class node_t : std::uint32_t {
    SEQUENCE,     //!< sequence value type
    MAPPING,      //!< mapping value type
    NULL_OBJECT,  //!< null value type
    BOOLEAN,      //!< boolean value type
    INTEGER,      //!< integer value type
    FLOAT_NUMBER, //!< float number value type
    STRING,       //!< string value type
};

inline const char* to_string(node_t t) noexcept {
    switch (t) {
    case node_t::SEQUENCE:
        return "sequence";
    case node_t::MAPPING:
        return "mapping";
    case node_t::NULL_OBJECT:
        return "null";
    case node_t::BOOLEAN:
        return "boolean";
    case node_t::INTEGER:
        return "integer";
    case node_t::FLOAT_NUMBER:
        return "float";
    case node_t::STRING:
        return "string";
    default:       // LCOV_EXCL_LINE
        return ""; // LCOV_EXCL_LINE
    }
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_TYPES_NODE_T_HPP_ */
