//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <doctest/doctest.h>

#include <fkYAML/node.hpp>

TEST_CASE("NodeAttrs_UintBit_IsNonZero") {
    REQUIRE(fkyaml::detail::node_attr_bits::uint_bit != 0u);
}

TEST_CASE("NodeAttrs_UintBit_LivesInStyleArea") {
    // uint_bit must live in 0x00FF0000 so it is orthogonal to the value-type area.
    REQUIRE((fkyaml::detail::node_attr_bits::uint_bit & fkyaml::detail::node_attr_mask::style) != 0u);
    REQUIRE((fkyaml::detail::node_attr_bits::uint_bit & fkyaml::detail::node_attr_mask::value) == 0u);
    REQUIRE((fkyaml::detail::node_attr_bits::uint_bit & fkyaml::detail::node_attr_mask::anchoring) == 0u);
}

TEST_CASE("NodeAttrs_UintBit_NotInScalarBits") {
    // scalar_bits is a type-classification mask. uint_bit is a metadata annotation
    // on an INTEGER node, not a separate type. It must not pollute scalar_bits.
    REQUIRE((fkyaml::detail::node_attr_bits::uint_bit & fkyaml::detail::node_attr_bits::scalar_bits) == 0u);
}

TEST_CASE("NodeAttrs_UintBit_InteractsWithIntBit") {
    fkyaml::detail::node_attr_t attrs = 0u;

    SUBCASE("both bits can be set simultaneously") {
        attrs |= fkyaml::detail::node_attr_bits::int_bit;
        attrs |= fkyaml::detail::node_attr_bits::uint_bit;
        REQUIRE((attrs & fkyaml::detail::node_attr_bits::int_bit) != 0u);
        REQUIRE((attrs & fkyaml::detail::node_attr_bits::uint_bit) != 0u);
    }

    SUBCASE("clearing uint_bit leaves int_bit intact") {
        attrs = fkyaml::detail::node_attr_bits::int_bit | fkyaml::detail::node_attr_bits::uint_bit;
        attrs &= ~fkyaml::detail::node_attr_bits::uint_bit;
        REQUIRE((attrs & fkyaml::detail::node_attr_bits::int_bit) != 0u);
        REQUIRE((attrs & fkyaml::detail::node_attr_bits::uint_bit) == 0u);
    }
}

TEST_CASE("NodeAttrs_UintBit_CtorWithNodeTypeNeverSetsIt") {
    // node_attrs constructor with node_type does not take care of uint_bit; it must not accidentally set it.
    fkyaml::detail::node_attrs attrs {fkyaml::node_type::INTEGER};
    REQUIRE(attrs.has(fkyaml::detail::node_attr_bits::int_bit));
    REQUIRE(!attrs.has(fkyaml::detail::node_attr_bits::uint_bit));
}

TEST_CASE("NodeAttrs_UintBit_ToNodeTypeStillReturnsInteger") {
    // uint_bit lives outside the value mask, so type classification must be unchanged
    // when both int_bit and uint_bit are present.
    const fkyaml::detail::node_attr_t attrs =
        fkyaml::detail::node_attr_bits::int_bit | fkyaml::detail::node_attr_bits::uint_bit;
    REQUIRE(fkyaml::detail::node_attrs(attrs).to_node_type() == fkyaml::node_type::INTEGER);
}
