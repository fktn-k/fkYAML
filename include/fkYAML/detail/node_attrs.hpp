//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.5.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_NODE_ATTRS_HPP
#define FK_YAML_DETAIL_NODE_ATTRS_HPP

#include <cstdint>
#include <limits>
#include <utility>

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

/// @brief The class which manages node attributes.
/// @details This class provides an interface to manipulate and query the attributes of a node,
/// including its type, style, and other properties. It encapsulates the underlying bit representation
/// and provides utility functions to set, unset, and check specific attribute bits.
class node_attrs {
public:
    /// @brief Creates a default-initialized node_attrs object.
    node_attrs() noexcept = default;

    /// @brief Creates a node_attrs object with the specified attribute bits.
    /// @param attrs The initial attribute bits to set.
    explicit node_attrs(node_attr_t attrs) noexcept
        : m_attrs(attrs) {
    }

    /// @brief Creates a node_attrs object with the specified node type.
    /// @param type The node type to set.
    explicit node_attrs(node_type type) noexcept
        : m_attrs(from_node_type(type)) {
    }

    /// @brief Creates a copy of the specified node_attrs object.
    /// @param other The node_attrs object to copy.
    node_attrs(const node_attrs&) noexcept = default;

    /// @brief Moves the specified node_attrs object into a new one.
    /// @param other The node_attrs object to move.
    node_attrs(node_attrs&&) noexcept = default;

    /// @brief Copy-assigns the specified node_attrs object.
    /// @param other The node_attrs object to copy.
    /// @return A reference to this node_attrs object.
    node_attrs& operator=(const node_attrs&) noexcept = default;

    /// @brief Move-assigns the specified node_attrs object.
    /// @param other The node_attrs object to move.
    /// @return A reference to this node_attrs object.
    node_attrs& operator=(node_attrs&&) noexcept = default;

    /// @brief Destroys the node_attrs object.
    ~node_attrs() noexcept = default;

    /// @brief Retrieves the raw attribute bits of the node.
    /// @return The raw attribute bits.
    node_attr_t get() const noexcept {
        return m_attrs;
    }

    /// @brief Retrieves the value-related attribute bits of the node.
    /// @return The value-related attribute bits.
    node_attr_t get_value_bits() const noexcept {
        return m_attrs & node_attr_mask::value;
    }

    /// @brief Sets the specified attribute bits.
    /// @param mask The attribute bits to set.
    void set(node_attr_t mask) noexcept {
        m_attrs |= mask;
    }

    /// @brief Sets the specified value-related attribute bit, unsetting any existing value-related bits.
    /// @param bit The value-related attribute bit to set.
    void set_value_bit(node_attr_t bit) noexcept {
        unset(node_attr_mask::value);
        set(bit);
    }

    /// @brief Unsets the specified attribute bits.
    /// @param mask The attribute bits to unset.
    void unset(node_attr_t mask) noexcept {
        m_attrs &= ~mask;
    }

    /// @brief Checks if all of the specified attribute bits are set.
    /// @param mask The attribute bits to check.
    /// @return True if all specified bits are set, false otherwise.
    bool has(node_attr_t mask) const noexcept {
        return (m_attrs & mask) == mask;
    }

    /// @brief Checks if any of the specified attribute bits are set.
    /// @param mask The attribute bits to check.
    /// @return True if any specified bit is set, false otherwise.
    bool has_any(node_attr_t mask) const noexcept {
        return (m_attrs & mask) != 0;
    }

    /// @brief Retrieves the node type based on the value-related attribute bits.
    /// @return The node type.
    node_type get_node_type() const noexcept {
        switch (get_value_bits()) {
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

    /// @brief Checks if the node is a sequence.
    /// @return True if the node is a sequence, false otherwise.
    bool is_sequence() const noexcept {
        return has(node_attr_bits::seq_bit);
    }

    /// @brief Checks if the node is a mapping.
    /// @return True if the node is a mapping, false otherwise.
    bool is_mapping() const noexcept {
        return has(node_attr_bits::map_bit);
    }

    /// @brief Checks if the node is null.
    /// @return True if the node is null, false otherwise.
    bool is_null() const noexcept {
        return has(node_attr_bits::null_bit);
    }

    /// @brief Checks if the node is a boolean.
    /// @return True if the node is a boolean, false otherwise.
    bool is_boolean() const noexcept {
        return has(node_attr_bits::bool_bit);
    }

    /// @brief Checks if the node is an integer.
    /// @return True if the node is an integer, false otherwise.
    bool is_integer() const noexcept {
        return has(node_attr_bits::int_bit);
    }

    /// @brief Checks if the node is an unsigned integer.
    /// @return True if the node is an unsigned integer, false otherwise.
    bool is_uint() const noexcept {
        return has(node_attr_bits::int_bit | node_attr_bits::uint_bit);
    }

    /// @brief Checks if the node is a floating-point number.
    /// @return True if the node is a floating-point number, false otherwise.
    bool is_float_number() const noexcept {
        return has(node_attr_bits::float_bit);
    }

    /// @brief Checks if the node is a string.
    /// @return True if the node is a string, false otherwise.
    bool is_string() const noexcept {
        return has(node_attr_bits::string_bit);
    }

    /// @brief Checks if the node is a scalar.
    /// @return True if the node is a scalar, false otherwise.
    bool is_scalar() const noexcept {
        return has_any(node_attr_bits::scalar_bits);
    }

    /// @brief Checks if the node is an anchor.
    /// @return True if the node is an anchor, false otherwise.
    bool is_anchor() const noexcept {
        return has(node_attr_bits::anchor_bit);
    }

    /// @brief Checks if the node is an alias.
    /// @return True if the node is an alias, false otherwise.
    bool is_alias() const noexcept {
        return has(node_attr_bits::alias_bit);
    }

    /// @brief Retrieves the anchor offset of the node.
    /// @return The anchor offset.
    uint32_t get_anchor_offset() const noexcept {
        return (m_attrs & node_attr_mask::anchor_offset) >> 26;
    }

    /// @brief Sets the anchor offset of the node.
    /// @param offset The anchor offset to set.
    void set_anchor_offset(uint32_t offset) noexcept {
        m_attrs &= ~node_attr_mask::anchor_offset;
        m_attrs |= (offset & 0x3Fu) << 26;
    }

    /// @brief Swaps the contents of this node_attrs object with another.
    /// @param other The other node_attrs object to swap with.
    void swap(node_attrs& other) noexcept {
        std::swap(m_attrs, other.m_attrs);
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
        default:                   // LCOV_EXCL_LINE
            detail::unreachable(); // LCOV_EXCL_LINE
        }
    }

    node_attr_t m_attrs {node_attr_bits::default_bits};
};

/// @brief Swaps the contents of two node_attrs objects.
/// @param lhs The first node_attrs object.
/// @param rhs The second node_attrs object.
inline void swap(node_attrs& lhs, node_attrs& rhs) noexcept {
    lhs.swap(rhs);
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_NODE_ATTRS_HPP */
