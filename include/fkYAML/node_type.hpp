//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_NODE_TYPE_HPP
#define FK_YAML_NODE_TYPE_HPP

#include <cstdint>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>

FK_YAML_NAMESPACE_BEGIN

enum class node_type : std::uint8_t {
    SEQUENCE,    //!< sequence value type
    MAPPING,     //!< mapping value type
    NULL_OBJECT, //!< null value type
    BOOLEAN,     //!< boolean value type
    INTEGER,     //!< integer value type
    FLOAT,       //!< float point value type
    STRING,      //!< string value type
};

inline const char* to_string(node_type t) noexcept {
    switch (t) {
    case node_type::SEQUENCE:
        return "SEQUENCE";
    case node_type::MAPPING:
        return "MAPPING";
    case node_type::NULL_OBJECT:
        return "NULL_OBJECT";
    case node_type::BOOLEAN:
        return "BOOLEAN";
    case node_type::INTEGER:
        return "INTEGER";
    case node_type::FLOAT:
        return "FLOAT";
    case node_type::STRING:
        return "STRING";
    default:                   // LCOV_EXCL_LINE
        detail::unreachable(); // LCOV_EXCL_LINE
    }
}

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_NODE_TYPE_HPP */
