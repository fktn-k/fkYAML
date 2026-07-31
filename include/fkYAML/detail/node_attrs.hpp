//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
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

/// The unsigned integer flag bit.
/// Set on INTEGER nodes whose stored int64_t value represents a uint64_t that exceeds INT64_MAX.
/// This allows values such as xxHash/UUID results to round-trip correctly through get_value<uint64_t>().
constexpr node_attr_t uint_bit = 1u << 16; // lives in the style bits area (0x00FF0000)

/// The anchor node bit.
constexpr node_attr_t anchor_bit = 0x01000000u;
/// The alias node bit.
constexpr node_attr_t alias_bit = 0x02000000u;

/// A utility bit set for initialization.
constexpr node_attr_t default_bits = null_bit;

} // namespace node_attr_bits

class node_attrs {
public:
    node_attrs() noexcept = default;

    /// @brief Creates a node_attrs from a node_attr_t value.
    /// @param attrs node attribute bits
    explicit node_attrs(node_attr_t attrs) noexcept
        : m_attrs(attrs) {
    }

    /// @brief Creates a node_attrs from a node_type value.
    /// @param t A type of node value.
    explicit node_attrs(node_type t) noexcept
        : m_attrs(from_node_type(t)) {
    }

    /// @brief Checks if the specified bits are set.
    /// @param bits The bits to check.
    /// @return True if all the specified bits are set, false otherwise.
    bool has(node_attr_t bits) const noexcept {
        return (m_attrs & bits) == bits;
    }

    /// @brief Checks if any of the specified bits are set.
    /// @param bits The bits to check.
    /// @return True if any of the specified bits are set, false otherwise.
    bool has_any(node_attr_t bits) const noexcept {
        return (m_attrs & bits) != 0u;
    }

    /// @brief Sets the specified bits.
    /// @param bits The bits to set.
    void set(node_attr_t bits) noexcept {
        m_attrs |= bits;
    }

    /// @brief Clears the specified bits.
    /// @param bits The bits to clear.
    void clear(node_attr_t bits) noexcept {
        m_attrs &= ~bits;
    }

    /// @brief Gets the raw node attribute bits.
    /// @return The raw node attribute bits.
    node_attr_t get() const noexcept {
        return m_attrs;
    }

    /// @brief Converts a value bit to a node_type value.
    /// @return A node value type.
    node_type to_node_type() const noexcept {
        switch (value().get()) {
        case node_attr_bits::seq_bit:
            return node_type::SEQUENCE;
        case node_attr_bits::map_bit:
            return node_type::MAPPING;
        case node_attr_bits::null_bit:
            return node_type::NULL_OBJECT;
        case node_attr_bits::bool_bit:
            return node_type::BOOLEAN;
        case node_attr_bits::int_bit:
            return node_type::INTEGER;
        case node_attr_bits::float_bit:
            return node_type::FLOAT;
        case node_attr_bits::string_bit:
            return node_type::STRING;
        default:                   // LCOV_EXCL_LINE
            detail::unreachable(); // LCOV_EXCL_LINE
        }
    }

    /// @brief Gets the value bits of the node attributes.
    /// @return A node_attrs object containing only the value bits.
    node_attrs value() const noexcept {
        return node_attrs(m_attrs & node_attr_mask::value);
    }

    /// @brief Gets the style bits of the node attributes.
    /// @return A node_attrs object containing only the style bits.
    node_attrs style() const noexcept {
        return node_attrs(m_attrs & node_attr_mask::style);
    }

    /// @brief Checks if the node is a sequence node.
    /// @return True if the node is a sequence node, false otherwise.
    bool is_sequence() const noexcept {
        return has(node_attr_bits::seq_bit);
    }

    /// @brief Checks if the node is a mapping node.
    /// @return True if the node is a mapping node, false otherwise.
    bool is_mapping() const noexcept {
        return has(node_attr_bits::map_bit);
    }

    /// @brief Checks if the node is a null node.
    /// @return True if the node is a null node, false otherwise.
    bool is_null() const noexcept {
        return has(node_attr_bits::null_bit);
    }

    /// @brief Checks if the node is a boolean node.
    /// @return True if the node is a boolean node, false otherwise.
    bool is_boolean() const noexcept {
        return has(node_attr_bits::bool_bit);
    }

    /// @brief Checks if the node is an integer node.
    /// @return True if the node is an integer node, false otherwise.
    bool is_integer() const noexcept {
        return has(node_attr_bits::int_bit);
    }

    /// @brief Checks if the node is a float number node.
    /// @return True if the node is a float number node, false otherwise.
    bool is_float_number() const noexcept {
        return has(node_attr_bits::float_bit);
    }

    /// @brief Checks if the node is a string node.
    /// @return True if the node is a string node, false otherwise.
    bool is_string() const noexcept {
        return has(node_attr_bits::string_bit);
    }

    /// @brief Checks if the node is a scalar node.
    /// @return True if the node is a scalar node, false otherwise.
    bool is_scalar() const noexcept {
        return has_any(node_attr_bits::scalar_bits);
    }

    /// @brief Checks if the node is an unsigned integer node.
    /// @return True if the node is an unsigned integer node, false otherwise.
    bool is_unsigned_integer() const noexcept {
        return has(node_attr_bits::uint_bit);
    }

    /// @brief Checks if the node is an anchor node.
    /// @return True if the node is an anchor node, false otherwise.
    bool is_anchor() const noexcept {
        return has(node_attr_bits::anchor_bit);
    }

    /// @brief Checks if the node is an alias node.
    /// @return True if the node is an alias node, false otherwise.
    bool is_alias() const noexcept {
        return has(node_attr_bits::alias_bit);
    }

    /// @brief Gets the anchor offset used to reference an anchor node.
    /// @return The anchor offset value.
    uint32_t get_anchor_offset() const noexcept {
        return (m_attrs & node_attr_mask::anchor_offset) >> 26;
    }

    /// @brief Sets an anchor offset value to the appropriate bits.
    /// @param offset The anchor offset value.
    void set_anchor_offset(uint32_t offset) noexcept {
        clear(node_attr_mask::anchor_offset);
        set((offset & 0x3Fu) << 26);
    }

private:
    /// @brief Converts a node_type value to a node_attr_t value.
    /// @param t A type of node value.
    /// @return The associated node value bit.
    static node_attr_t from_node_type(node_type t) noexcept {
        switch (t) {
        case node_type::SEQUENCE:
            return node_attr_bits::seq_bit;
        case node_type::MAPPING:
            return node_attr_bits::map_bit;
        case node_type::NULL_OBJECT:
            return node_attr_bits::null_bit;
        case node_type::BOOLEAN:
            return node_attr_bits::bool_bit;
        case node_type::INTEGER:
            return node_attr_bits::int_bit;
        case node_type::FLOAT:
            return node_attr_bits::float_bit;
        case node_type::STRING:
            return node_attr_bits::string_bit;
        default:                        // LCOV_EXCL_LINE
            return node_attr_mask::all; // LCOV_EXCL_LINE
        }
    }

    /// @brief The raw node attribute bits.
    node_attr_t m_attrs {node_attr_bits::default_bits};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_NODE_ATTRS_HPP */
