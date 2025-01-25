//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_NODE_ATTRS_HPP
#define FK_YAML_DETAIL_NODE_ATTRS_HPP

#include <cstdint>
#include <limits>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/node_type.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief The type for node attribute bits.
using node_attr_t = uint32_t;

/// @brief The namespace to define bit masks for node attribute bits.
namespace node_attr_mask {

/// The bit mask for node value type bits.
constexpr node_attr_t value = 0x0000FFFFu;
/// The bit mask for node style type bits. (bits are not yet defined.)
constexpr node_attr_t style = 0x00FF0000u;
/// The bit mask for node property related bits.
constexpr node_attr_t props = 0xFF000000u;
/// The bit mask for anchor/alias node type bits.
constexpr node_attr_t anchoring = 0x03000000u;
/// The bit mask for anchor offset value bits.
constexpr node_attr_t anchor_offset = 0xFC000000u;
/// The bit mask for all the bits for node attributes.
constexpr node_attr_t all = std::numeric_limits<node_attr_t>::max();

} // namespace node_attr_mask

/// @brief The namespace to define bits for node attributes.
namespace node_attr_bits {

/// The sequence node bit.
constexpr node_attr_t seq_bit = 1u << 0;
/// The mapping node bit.
constexpr node_attr_t map_bit = 1u << 1;
/// The null scalar node bit.
constexpr node_attr_t null_bit = 1u << 2;
/// The boolean scalar node bit.
constexpr node_attr_t bool_bit = 1u << 3;
/// The integer scalar node bit.
constexpr node_attr_t int_bit = 1u << 4;
/// The floating point scalar node bit.
constexpr node_attr_t float_bit = 1u << 5;
/// The string scalar node bit.
constexpr node_attr_t string_bit = 1u << 6;

/// A utility bit set to filter scalar node bits.
constexpr node_attr_t scalar_bits = null_bit | bool_bit | int_bit | float_bit | string_bit;

/// The anchor node bit.
constexpr node_attr_t anchor_bit = 0x01000000u;
/// The alias node bit.
constexpr node_attr_t alias_bit = 0x02000000u;

/// A utility bit set for initialization.
constexpr node_attr_t default_bits = null_bit;

/// @brief Converts a node_type value to a node_attr_t value.
/// @param t A type of node value.
/// @return The associated node value bit.
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

/// @brief Converts a node_attr_t value to a node_type value.
/// @param bits node attribute bits
/// @return An associated node value type with the given node value bit.
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
    default:                   // LCOV_EXCL_LINE
        detail::unreachable(); // LCOV_EXCL_LINE
    }
}

/// @brief Get an anchor offset used to reference an anchor node from the given attribute bits.
/// @param attrs node attribute bits
/// @return An anchor offset value.
inline uint32_t get_anchor_offset(node_attr_t attrs) noexcept {
    return (attrs & node_attr_mask::anchor_offset) >> 26;
}

/// @brief Set an anchor offset value to the appropriate bits.
/// @param offset An anchor offset value.
/// @param attrs node attribute bit set into which the offset value is written.
inline void set_anchor_offset(uint32_t offset, node_attr_t& attrs) noexcept {
    attrs &= ~node_attr_mask::anchor_offset;
    attrs |= (offset & 0x3Fu) << 26;
}

} // namespace node_attr_bits

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_NODE_ATTRS_HPP */
