///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_NODE_ATTRS_HPP_
#define FK_YAML_DETAIL_NODE_ATTRS_HPP_

#include <cstdint>
#include <limits>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/node_type.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

using node_attr_t = uint32_t;

namespace node_attr_mask {

const node_attr_t value = 0x0000FFFFu;
const node_attr_t style = 0x00FF0000u;
const node_attr_t props = 0xFF000000u;
const node_attr_t anchoring = 0x03000000u;
const node_attr_t anchor_offset = 0xFC000000u;
const node_attr_t all = std::numeric_limits<node_attr_t>::max();

} // namespace node_attr_mask

namespace node_attr_bits {

const node_attr_t seq_bit = 1u << 0;
const node_attr_t map_bit = 1u << 1;
const node_attr_t null_bit = 1u << 2;
const node_attr_t bool_bit = 1u << 3;
const node_attr_t int_bit = 1u << 4;
const node_attr_t float_bit = 1u << 5;
const node_attr_t string_bit = 1u << 6;

const node_attr_t scalar_bits = null_bit | bool_bit | int_bit | float_bit | string_bit;

const node_attr_t block_bit = 0x00000000u;
const node_attr_t flow_bit = 0x00010000u;

const node_attr_t anchor_bit = 0x01000000u;
const node_attr_t alias_bit = 0x02000000u;

const node_attr_t default_bits = null_bit;

inline node_attr_t from_node_type(node_type t) noexcept {
    switch (t) {
    case node_type::SEQUENCE:
        return seq_bit;
    case node_type::MAPPING:
        return map_bit;
    case node_type::NULL_OBJECT:
        return null_bit;
    case node_type::BOOLEAN:
        return bool_bit;
    case node_type::INTEGER:
        return int_bit;
    case node_type::FLOAT:
        return float_bit;
    case node_type::STRING:
        return string_bit;
    default:                        // LCOV_EXCL_LINE
        return node_attr_mask::all; // LCOV_EXCL_LINE
    }
}

inline node_type to_node_type(node_attr_t bits) noexcept {
    switch (bits & node_attr_mask::value) {
    case seq_bit:
        return node_type::SEQUENCE;
    case map_bit:
        return node_type::MAPPING;
    case null_bit:
        return node_type::NULL_OBJECT;
    case bool_bit:
        return node_type::BOOLEAN;
    case int_bit:
        return node_type::INTEGER;
    case float_bit:
        return node_type::FLOAT;
    case string_bit:
        return node_type::STRING;
    default:                           // LCOV_EXCL_LINE
        return node_type::NULL_OBJECT; // LCOV_EXCL_LINE
    }
}

inline uint32_t get_anchor_offset(node_attr_t attrs) noexcept {
    return (attrs & node_attr_mask::anchor_offset) >> 26;
}

inline void set_anchor_offset(uint32_t offset, node_attr_t& attrs) noexcept {
    attrs &= ~node_attr_mask::anchor_offset;
    attrs |= (offset & 0x3Fu) << 26;
}

} // namespace node_attr_bits

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_NODE_ATTRS_HPP_ */
