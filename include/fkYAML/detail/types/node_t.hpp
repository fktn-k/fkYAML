//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_TYPES_NODE_T_HPP
#define FK_YAML_DETAIL_TYPES_NODE_T_HPP

#include <cstdint>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/node_type.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Definition of node value types.
enum class node_t : std::uint8_t {
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
    default:                   // LCOV_EXCL_LINE
        detail::unreachable(); // LCOV_EXCL_LINE
    }
}

inline node_t convert_from_node_type(node_type t) {
    switch (t) {
    case node_type::SEQUENCE:
        return node_t::SEQUENCE;
    case node_type::MAPPING:
        return node_t::MAPPING;
    case node_type::NULL_OBJECT:
        return node_t::NULL_OBJECT;
    case node_type::BOOLEAN:
        return node_t::BOOLEAN;
    case node_type::INTEGER:
        return node_t::INTEGER;
    case node_type::FLOAT:
        return node_t::FLOAT_NUMBER;
    case node_type::STRING:
        return node_t::STRING;
    default:                   // LCOV_EXCL_LINE
        detail::unreachable(); // LCOV_EXCL_LINE
    }
}

inline node_type convert_to_node_type(node_t t) {
    switch (t) {
    case node_t::SEQUENCE:
        return node_type::SEQUENCE;
    case node_t::MAPPING:
        return node_type::MAPPING;
    case node_t::NULL_OBJECT:
        return node_type::NULL_OBJECT;
    case node_t::BOOLEAN:
        return node_type::BOOLEAN;
    case node_t::INTEGER:
        return node_type::INTEGER;
    case node_t::FLOAT_NUMBER:
        return node_type::FLOAT;
    case node_t::STRING:
        return node_type::STRING;
    default:                   // LCOV_EXCL_LINE
        detail::unreachable(); // LCOV_EXCL_LINE
    }
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_TYPES_NODE_T_HPP */
