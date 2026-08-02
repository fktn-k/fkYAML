//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <doctest/doctest.h>

#include <fkYAML/node.hpp>

TEST_CASE("NodeAttrs_FromNodeType") {
    using test_data_t = std::pair<fkyaml::node_type, fkyaml::detail::node_attr_t>;
    auto test_data = GENERATE(
        test_data_t {fkyaml::node_type::SEQUENCE, fkyaml::detail::node_attr_bits::seq_bit},
        test_data_t {fkyaml::node_type::MAPPING, fkyaml::detail::node_attr_bits::map_bit},
        test_data_t {fkyaml::node_type::NULL_OBJECT, fkyaml::detail::node_attr_bits::null_bit},
        test_data_t {fkyaml::node_type::BOOLEAN, fkyaml::detail::node_attr_bits::bool_bit},
        test_data_t {fkyaml::node_type::INTEGER, fkyaml::detail::node_attr_bits::int_bit},
        test_data_t {fkyaml::node_type::FLOAT, fkyaml::detail::node_attr_bits::float_bit},
        test_data_t {fkyaml::node_type::STRING, fkyaml::detail::node_attr_bits::string_bit});
    REQUIRE(fkyaml::detail::node_attrs(test_data.first).value().get() == test_data.second);
}

TEST_CASE("NodeAttrs_ToNodeType") {
    using test_data_t = std::pair<fkyaml::detail::node_attr_t, fkyaml::node_type>;
    auto test_data = GENERATE(
        test_data_t {fkyaml::detail::node_attr_bits::seq_bit, fkyaml::node_type::SEQUENCE},
        test_data_t {fkyaml::detail::node_attr_bits::map_bit, fkyaml::node_type::MAPPING},
        test_data_t {fkyaml::detail::node_attr_bits::null_bit, fkyaml::node_type::NULL_OBJECT},
        test_data_t {fkyaml::detail::node_attr_bits::bool_bit, fkyaml::node_type::BOOLEAN},
        test_data_t {fkyaml::detail::node_attr_bits::int_bit, fkyaml::node_type::INTEGER},
        test_data_t {fkyaml::detail::node_attr_bits::float_bit, fkyaml::node_type::FLOAT},
        test_data_t {fkyaml::detail::node_attr_bits::string_bit, fkyaml::node_type::STRING});
    REQUIRE(fkyaml::detail::node_attrs(test_data.first).to_node_type() == test_data.second);
}

TEST_CASE("NodeAttrs_GetAnchorOffset") {
    using test_data_t = std::pair<fkyaml::detail::node_attr_t, uint32_t>;
    auto test_data = GENERATE(test_data_t {0, 0}, test_data_t {0x88000000u, 0x88u}, test_data_t {0xFF000000u, 0xFFu});
    REQUIRE(fkyaml::detail::node_attrs(test_data.first).get_anchor_offset() == test_data.second);
}

TEST_CASE("NodeAttrs_SetAnchorOffset") {
    using test_data_t = std::pair<uint32_t, fkyaml::detail::node_attr_t>;
    auto test_data = GENERATE(test_data_t {0, 0}, test_data_t {0x88u, 0x88000000u}, test_data_t {0xFFu, 0xFF000000u});
    fkyaml::detail::node_attrs attrs {0};
    attrs.set_anchor_offset(test_data.first);
    REQUIRE(attrs.get() == test_data.second);
}
