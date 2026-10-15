//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.5.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

// This file is built into its own app since the macro changes the layout of fkyaml::node.
#define FK_YAML_ENABLE_NODE_LOCATION
#include <doctest/doctest.h>

#include <fkYAML/node.hpp>

#include <string>
#include <typeinfo>

namespace {

void check_location(const fkyaml::node& node, uint32_t line, uint32_t column) {
    CAPTURE(line);
    CAPTURE(column);
    REQUIRE(node.get_location().line == line);
    REQUIRE(node.get_location().column == column);
}

const fkyaml::node& key_of(const fkyaml::node& map, const char* key) {
    for (const auto& pair : map.as_map()) {
        if (pair.first.is_string() && pair.first.as_str() == key) {
            return pair.first;
        }
    }
    FAIL("key not found: " << key);
    return map; // LCOV_EXCL_LINE
}

} // namespace

TEST_CASE("NodeLocation_AbiNamespace") {
    // Every compiler embeds namespace names in type names, so this holds regardless of the name format.
    const std::string type_name = typeid(fkyaml::node).name();
    REQUIRE(type_name.find("_location") != std::string::npos);
}

TEST_CASE("NodeLocation_BlockMapping") {
    const fkyaml::node root = fkyaml::node::deserialize("foo: bar\n"
                                                        "seq:\n"
                                                        "  - 1\n"
                                                        "  - [a, b]\n"
                                                        "map: {x: 1}\n"
                                                        "tagged: &anc !!str baz\n"
                                                        "empty:\n");
    check_location(root, 0, 0);

    check_location(key_of(root, "foo"), 0, 0);
    check_location(root["foo"], 0, 5);

    check_location(key_of(root, "seq"), 1, 0);
    check_location(root["seq"], 2, 2);
    check_location(root["seq"][0], 2, 4);
    check_location(root["seq"][1], 3, 4);
    check_location(root["seq"][1][0], 3, 5);
    check_location(root["seq"][1][1], 3, 8);

    check_location(root["map"], 4, 5);
    check_location(key_of(root["map"], "x"), 4, 6);
    check_location(root["map"]["x"], 4, 9);

    // node properties are not part of the location.
    check_location(root["tagged"], 5, 19);

    // an omitted value is located at its key.
    check_location(root["empty"], 6, 0);
}

TEST_CASE("NodeLocation_BlockSequence") {
    const fkyaml::node root = fkyaml::node::deserialize("- foo\n"
                                                        "- name: bar\n"
                                                        "  age: 1\n"
                                                        "-\n"
                                                        "- |\n"
                                                        "  text\n");
    check_location(root, 0, 0);
    check_location(root[0], 0, 2);
    check_location(root[1], 1, 2);
    check_location(root[1]["age"], 2, 7);
    // an empty entry is located at its entry indicator.
    check_location(root[2], 3, 0);
    check_location(root[3], 4, 2);
}

TEST_CASE("NodeLocation_FlowMappingRoot") {
    const fkyaml::node root = fkyaml::node::deserialize("{a: [1,\n"
                                                        " 2], b: c}\n");
    check_location(root, 0, 0);
    check_location(root["a"], 0, 4);
    check_location(root["a"][1], 1, 1);
    check_location(root["b"], 1, 8);
}

TEST_CASE("NodeLocation_DuplicateExplicitKey") {
    SUBCASE("with a value") {
        const fkyaml::node root = fkyaml::node::deserialize("a: 1\n"
                                                            "? a\n"
                                                            ": 2\n");
        check_location(root["a"], 2, 2);
    }

    SUBCASE("without a value") {
        // the existing value keeps its location.
        const fkyaml::node root = fkyaml::node::deserialize("a: 1\n"
                                                            "? a\n");
        check_location(root["a"], 0, 3);
    }
}

TEST_CASE("NodeLocation_Alias") {
    const fkyaml::node root = fkyaml::node::deserialize("a: &anc 1\n"
                                                        "b: *anc\n");
    check_location(root["a"], 0, 8);
    check_location(root["b"], 1, 3);
}

TEST_CASE("NodeLocation_CopyMoveAndSwap") {
    fkyaml::node root = fkyaml::node::deserialize("a: 1\n"
                                                  "b: 2\n");
    fkyaml::node copied = root["b"];
    check_location(copied, 1, 3);

    fkyaml::node moved = std::move(copied);
    check_location(moved, 1, 3);

    fkyaml::node other = 123;
    check_location(other, 0, 0);
    other.swap(moved);
    check_location(other, 1, 3);
    check_location(moved, 0, 0);
}
