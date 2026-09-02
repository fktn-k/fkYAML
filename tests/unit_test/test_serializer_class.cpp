//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.4
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <limits>

#include <doctest/doctest.h>

#include <fkYAML/node.hpp>

TEST_CASE("Serializer_SequenceNode") {
    using node_str_pair_t = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(
        node_str_pair_t({true, false}, "- true\n- false\n"),
        node_str_pair_t({{{"foo", -1234}, {"bar", nullptr}}}, "-\n  bar: null\n  foo: -1234\n"));
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("Serializer_MappingNode") {
    using node_str_pair_t = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(
        node_str_pair_t({{"foo", -1234}, {"bar", nullptr}}, "bar: null\nfoo: -1234\n"),
        node_str_pair_t({{"foo", {true, false}}}, "foo:\n  - true\n  - false\n"));
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("Serializer_EmptyCollectionNode") {
    auto seq = fkyaml::node::sequence();
    auto map = fkyaml::node::mapping();
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;

    SUBCASE("child sequence item is an empty sequence node") {
        seq.as_seq().emplace_back(fkyaml::node::sequence());
        std::string expected = "- []\n";
        REQUIRE(serializer.serialize(seq) == expected);
    }

    SUBCASE("child sequence item is an empty mapping node") {
        seq.as_seq().emplace_back(fkyaml::node::mapping());
        std::string expected = "- {}\n";
        REQUIRE(serializer.serialize(seq) == expected);
    }

    SUBCASE("mapping value is an empty sequence node") {
        map["foo"] = seq;
        std::string expected = "foo: []\n";
        REQUIRE(serializer.serialize(map) == expected);
    }

    SUBCASE("mapping value is an empty mapping node") {
        map["foo"] = fkyaml::node::mapping();
        std::string expected = "foo: {}\n";
        REQUIRE(serializer.serialize(map) == expected);
    }

    SUBCASE("root empty sequence") {
        std::string expected = "[]\n";
        REQUIRE(serializer.serialize(seq) == expected);
    }

    SUBCASE("root empty mapping") {
        std::string expected = "{}\n";
        REQUIRE(serializer.serialize(map) == expected);
    }
}

TEST_CASE("Serializer_NullNode") {
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    fkyaml::node node;
    REQUIRE(serializer.serialize(node) == "null");
}

TEST_CASE("Serializer_BooleanNode") {
    using node_str_pair_t = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(node_str_pair_t(false, "false"), node_str_pair_t(true, "true"));
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("Serializer_IntegerNode") {
    using node_str_pair_t = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(node_str_pair_t(-1234, "-1234"), node_str_pair_t(5678, "5678"));
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("SerializeClassTest_FloatNode") {
    using node_str_pair_t = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(
        node_str_pair_t(0.0, "0.0"),
        node_str_pair_t(-2.0, "-2.0"),
        node_str_pair_t(2.0, "2.0"),
        node_str_pair_t(-2.10, "-2.1"),
        node_str_pair_t(2.10, "2.1"),
        node_str_pair_t(3.14, "3.14"),
        node_str_pair_t(-53.97, "-53.97"),
        node_str_pair_t(23000000.0, "2.3e+07"),
        node_str_pair_t(-23000000.0, "-2.3e+07"),
        node_str_pair_t(std::numeric_limits<fkyaml::node::float_number_type>::infinity(), ".inf"),
        node_str_pair_t(-1 * std::numeric_limits<fkyaml::node::float_number_type>::infinity(), "-.inf"),
        node_str_pair_t(std::nan(""), ".nan"));
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("Serializer_StringNode") {
    using node_str_pair_t = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(
        node_str_pair_t("test", "test"),
        node_str_pair_t("foo bar", "\"foo bar\""),
        node_str_pair_t("", "\"\""),
        node_str_pair_t("null", "\"null\""),
        node_str_pair_t("Null", "\"Null\""),
        node_str_pair_t("NULL", "\"NULL\""),
        node_str_pair_t("~", "\"~\""),
        node_str_pair_t("true", "\"true\""),
        node_str_pair_t("True", "\"True\""),
        node_str_pair_t("TRUE", "\"TRUE\""),
        node_str_pair_t("false", "\"false\""),
        node_str_pair_t("False", "\"False\""),
        node_str_pair_t("FALSE", "\"FALSE\""),
        node_str_pair_t("123", "\"123\""),
        node_str_pair_t("-567", "\"-567\""),
        node_str_pair_t("3.14", "\"3.14\""),
        node_str_pair_t("1.23e-4", "\"1.23e-4\""),
        node_str_pair_t(".inf", "\".inf\""),
        node_str_pair_t(".Inf", "\".Inf\""),
        node_str_pair_t(".INF", "\".INF\""),
        node_str_pair_t("-.inf", "\"-.inf\""),
        node_str_pair_t("-.Inf", "\"-.Inf\""),
        node_str_pair_t("-.INF", "\"-.INF\""),
        node_str_pair_t(".nan", "\".nan\""),
        node_str_pair_t(".NaN", "\".NaN\""),
        node_str_pair_t(".NAN", "\".NAN\""),
        node_str_pair_t("%", "\"%\""),
        node_str_pair_t("%test", "\"%test\""),
        node_str_pair_t("@test", "\"@test\""),
        node_str_pair_t("`test", "\"`test\""),
        node_str_pair_t("#test", "\"#test\""),
        node_str_pair_t("&test", "\"&test\""),
        node_str_pair_t("*test", "\"*test\""),
        node_str_pair_t("!test", "\"!test\""),
        node_str_pair_t("|test", "\"|test\""),
        node_str_pair_t(">test", "\">test\""),
        node_str_pair_t("[test", "\"[test\""),
        node_str_pair_t("]test", "\"]test\""),
        node_str_pair_t("{test", "\"{test\""),
        node_str_pair_t("}test", "\"}test\""),
        node_str_pair_t(",test", "\",test\""),
        node_str_pair_t("foo,bar", "\"foo,bar\""),
        node_str_pair_t("foo[bar", "\"foo[bar\""),
        node_str_pair_t("foo]bar", "\"foo]bar\""),
        node_str_pair_t("foo{bar", "\"foo{bar\""),
        node_str_pair_t("foo}bar", "\"foo}bar\""),
        node_str_pair_t("'test", "\"'test\""),
        node_str_pair_t(" test", "\" test\""),
        node_str_pair_t("test ", "\"test \""),
        node_str_pair_t("-", "\"-\""),
        node_str_pair_t("- test", "\"- test\""),
        node_str_pair_t("-test", "-test"),
        node_str_pair_t("? test", "\"? test\""),
        node_str_pair_t("?test", "?test"),
        node_str_pair_t(": test", "\": test\""),
        node_str_pair_t(":test", ":test"),
        node_str_pair_t("foo: bar", "\"foo: bar\""),
        node_str_pair_t("foo #bar", "\"foo #bar\""),
        node_str_pair_t("foo\"bar", "\"foo\\\"bar\""),
        node_str_pair_t(fkyaml::node::string_type({char(0xC2u), char(0xA1u)}), std::string({char(0xC2u), char(0xA1u)})),
        node_str_pair_t(
            fkyaml::node::string_type({char(0xE3u), char(0x80u), char(0xA8u)}),
            std::string({char(0xE3u), char(0x80u), char(0xA8u)})),
        node_str_pair_t(
            fkyaml::node::string_type({char(0xE2u), char(0x81u), char(0xA8u)}),
            std::string({char(0xE2u), char(0x81u), char(0xA8u)})),
        node_str_pair_t(
            fkyaml::node::string_type({char(0xE2u), char(0x80u), char(0xAAu)}),
            std::string({char(0xE2u), char(0x80u), char(0xAAu)})));

    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("Serializer_StringNodeStartingWithDirectiveIndicatorInMapping") {
    const std::string yaml = "project: fkYAML\n"
                             "percent: %\n"
                             "startingChar: %test\n";

    const fkyaml::node node = fkyaml::node::deserialize(yaml);

    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(
        serializer.serialize(node) == "percent: \"%\"\n"
                                      "project: fkYAML\n"
                                      "startingChar: \"%test\"\n");
}

TEST_CASE("Serializer_MappingKeyNode") {
    fkyaml::node map_key = {{true, 123}};
    fkyaml::node seq_key = {3.14, nullptr};
    fkyaml::node node = {{map_key, 3.14}, {seq_key, "foo"}};
    std::string expected = "? - 3.14\n"
                           "  - null\n"
                           ": foo\n"
                           "? true: 123\n"
                           ": 3.14\n";

    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node) == expected);
}

TEST_CASE("Serializer_AnchorNode") {
    fkyaml::node node = {{"foo", 123}, {nullptr, {true, "bar", 3.14}}};
    node[nullptr].add_anchor_name("A");
    node[nullptr][2].add_anchor_name("B");
    fkyaml::node key = "baz";
    key.add_anchor_name("C");
    node.as_map().emplace(key, "qux");
    node.add_anchor_name("anchor");

    std::string expected = "&anchor\n"
                           "null: &A\n"
                           "  - true\n"
                           "  - bar\n"
                           "  - &B 3.14\n"
                           "&C baz: qux\n"
                           "foo: 123\n";

    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node) == expected);
}

TEST_CASE("Serializer_AliasNode") {
    std::string expected = "foo: &A 123\n"
                           "null:\n"
                           "  - bar\n"
                           "  - *A\n"
                           "true: *A\n"
                           "*A : 3.14\n";
    const fkyaml::node node = fkyaml::node::deserialize(expected);
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node) == expected);
}

TEST_CASE("Serializer_ShouldPreserveAnchorAliasResolutionOrder") {
    SUBCASE("Nested mapping anchor") {
        const std::string input = "root:\n"
                                  "  z_anchor:\n"
                                  "    value: &anchor 456\n"
                                  "  a_alias: *anchor\n";
        const fkyaml::node node = fkyaml::node::deserialize(input);
        fkyaml::detail::basic_serializer<fkyaml::node> serializer;
        const std::string output = serializer.serialize(node);
        REQUIRE(output == input);

        const fkyaml::node redeserialized_node = fkyaml::node::deserialize(output);
        const fkyaml::node& root = redeserialized_node["root"];
        REQUIRE(root["z_anchor"]["value"].get_value<int>() == 456);
        REQUIRE(root["a_alias"].get_value<int>() == 456);
    }

    SUBCASE("Multiple nested mapping anchors") {
        const std::string input = "root:\n"
                                  "  z_first:\n"
                                  "    value: &anchor 123\n"
                                  "  a_first_ref: *anchor\n"
                                  "  z_second:\n"
                                  "    value: &anchor 456\n"
                                  "  a_second_ref: *anchor\n";
        const fkyaml::node node = fkyaml::node::deserialize(input);
        fkyaml::detail::basic_serializer<fkyaml::node> serializer;
        const std::string output = serializer.serialize(node);
        REQUIRE(output == input);

        const fkyaml::node redeserialized_node = fkyaml::node::deserialize(output);
        const fkyaml::node& root = redeserialized_node["root"];
        REQUIRE(root["a_first_ref"].get_value<int>() == 123);
        REQUIRE(root["a_second_ref"].get_value<int>() == 456);
        REQUIRE(root["z_first"]["value"].get_value<int>() == 123);
        REQUIRE(root["z_second"]["value"].get_value<int>() == 456);
    }

    SUBCASE("Anchors and aliases in a sequence") {
        const std::string input = "- &anchor foo: 123\n"
                                  "  bar: *anchor\n"
                                  "  baz:\n"
                                  "    - &anchor 456\n"
                                  "  *anchor : qux\n"
                                  "- *anchor\n"
                                  "- &anchor 789\n"
                                  "- *anchor : foo\n"
                                  "  bar: &anchor false\n"
                                  "  *anchor : baz\n";
        const fkyaml::node node = fkyaml::node::deserialize(input);
        // The deserialized node has the following structure which invalidates the anchor resolution order:
        // ```yaml
        // - *anchor: qux        # *anchor=456
        //   bar: *anchor        # *anchor=foo
        //   baz:
        //     - &anchor 456
        //   &anchor foo: 123
        // - *anchor             # *anchor=456
        // - &anchor 789
        // - *anchor : baz       # *anchor=false
        //   *anchor : baz       # *anchor=789
        //   bar: &anchor false
        // ```
        fkyaml::detail::basic_serializer<fkyaml::node> serializer;
        const std::string output = serializer.serialize(node);
        const fkyaml::node roundtrip = fkyaml::node::deserialize(output);

        REQUIRE(roundtrip.is_sequence());
        REQUIRE(roundtrip.size() == 4);

        const fkyaml::node& seq0 = roundtrip[0];
        REQUIRE(seq0.is_mapping());
        REQUIRE(seq0["bar"].as_str() == "foo");
        REQUIRE(seq0["baz"][0].get_value<int>() == 456);
        REQUIRE(seq0["foo"].get_value<int>() == 123);

        bool found_seq0_alias_key = false;
        for (auto item : seq0.map_items()) {
            if (item.key().is_alias()) {
                REQUIRE(item.key().get_value<int>() == 456);
                REQUIRE(item.value().as_str() == "qux");
                found_seq0_alias_key = true;
            }
        }
        REQUIRE(found_seq0_alias_key);

        REQUIRE(roundtrip[1].get_value<int>() == 456);
        REQUIRE(roundtrip[2].get_value<int>() == 789);

        const fkyaml::node& seq3 = roundtrip[3];
        REQUIRE(seq3.is_mapping());
        REQUIRE(seq3["bar"].get_value<bool>() == false);

        bool found_seq3_alias_key_for_789 = false;
        bool found_seq3_alias_key_for_false = false;
        for (auto item : seq3.map_items()) {
            if (!item.key().is_alias()) {
                continue;
            }

            if (item.key().is_integer()) {
                REQUIRE(item.key().get_value<int>() == 789);
                REQUIRE(item.value().as_str() == "foo");
                found_seq3_alias_key_for_789 = true;
                continue;
            }

            if (item.key().is_boolean()) {
                REQUIRE(item.key().get_value<bool>() == false);
                REQUIRE(item.value().as_str() == "baz");
                found_seq3_alias_key_for_false = true;
            }
        }
        REQUIRE(found_seq3_alias_key_for_789);
        REQUIRE(found_seq3_alias_key_for_false);
    }

    SUBCASE("Deserialization does not invalidate anchor resolution order") {
        const std::string input = "&anchor a: 123\n"
                                  "b: *anchor\n"
                                  "c:\n"
                                  "  - &anchor d\n"
                                  "*anchor : qux\n";
        const fkyaml::node node = fkyaml::node::deserialize(input);
        fkyaml::detail::basic_serializer<fkyaml::node> serializer;
        const std::string output = serializer.serialize(node);
        REQUIRE(output == input);
    }
}

TEST_CASE("Serializer_AnchorDefinitionOrderDiffersFromMapOrder") {
    const std::string input = "root:\n"
                              "  z_first:\n"
                              "    value: &first 123\n"
                              "  a_second:\n"
                              "    value: &second 456\n"
                              "  z_first_ref: *first\n"
                              "  a_second_ref: *second\n";
    const fkyaml::node node = fkyaml::node::deserialize(input);
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;

    const std::string output = serializer.serialize(node);
    REQUIRE(
        output == "root:\n"
                  "  a_second:\n"
                  "    value: &second 456\n"
                  "  a_second_ref: *second\n"
                  "  z_first:\n"
                  "    value: &first 123\n"
                  "  z_first_ref: *first\n");
    const fkyaml::node serialized_node = fkyaml::node::deserialize(output);
    const fkyaml::node& root = serialized_node["root"];
    REQUIRE(root["z_first_ref"].get_value<int>() == 123);
    REQUIRE(root["a_second_ref"].get_value<int>() == 456);
}

TEST_CASE("Serializer_AliasAfterAnchorInSameMappingEntry") {
    const std::string input = "root:\n"
                              "  &anchor z_key: *anchor\n"
                              "  a_reference: *anchor\n";
    const fkyaml::node node = fkyaml::node::deserialize(input);
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;

    const std::string output = serializer.serialize(node);
    REQUIRE(output == input);

    const fkyaml::node serialized_node = fkyaml::node::deserialize(output);
    REQUIRE(serialized_node["root"]["z_key"].get_value<std::string>() == "z_key");
    REQUIRE(serialized_node["root"]["a_reference"].get_value<std::string>() == "z_key");
}

TEST_CASE("Serializer_AliasAfterAnchorInFirstMappingEntry") {
    const std::string input = "root:\n"
                              "  &anchor a_key: *anchor\n"
                              "  b_reference: *anchor\n";
    const fkyaml::node node = fkyaml::node::deserialize(input);
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;

    const std::string output = serializer.serialize(node);
    REQUIRE(output == input);

    const fkyaml::node serialized_node = fkyaml::node::deserialize(output);
    REQUIRE(serialized_node["root"]["a_key"].get_value<std::string>() == "a_key");
    REQUIRE(serialized_node["root"]["b_reference"].get_value<std::string>() == "a_key");
}

TEST_CASE("Serializer_ExternalAliasesDoNotRequireMappingReordering") {
    const std::string input = "external: &external 123\n"
                              "root:\n"
                              "  z_local: &local 456\n"
                              "  a_external_ref: *external\n";
    const fkyaml::node node = fkyaml::node::deserialize(input);
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;

    const std::string output = serializer.serialize(node);
    REQUIRE(
        output == "external: &external 123\n"
                  "root:\n"
                  "  a_external_ref: *external\n"
                  "  z_local: &local 456\n");

    const fkyaml::node serialized_node = fkyaml::node::deserialize(output);
    REQUIRE(serialized_node["root"]["a_external_ref"].get_value<int>() == 123);
    REQUIRE(serialized_node["root"]["z_local"].get_value<int>() == 456);
}

TEST_CASE("Serializer_TaggedNode") {
    fkyaml::node root = fkyaml::node::mapping();
    fkyaml::node str_node("foo");
    str_node.add_tag_name("!!str");
    fkyaml::node null_node {};
    null_node.add_tag_name("!!null");
    fkyaml::node bool_node(true);
    bool_node.add_tag_name("!<tag:yaml.org,2002:bool>");
    fkyaml::node int_node(123);
    int_node.add_tag_name("!!int");
    fkyaml::node float_node(3.14);
    float_node.add_tag_name("!<tag:yaml.org,2002:float>");
    fkyaml::node map_node = {{"bar", false}};
    map_node.add_tag_name("!!map");
    fkyaml::node seq_node = {nullptr, 456};
    seq_node.add_tag_name("!!seq");

    auto& mapping = root.as_map();
    mapping.emplace(str_node, null_node);
    mapping.emplace(bool_node, int_node);
    mapping.emplace(null_node, float_node);
    mapping.emplace("map", map_node);
    mapping.emplace("seq", seq_node);

    root.add_tag_name("!!map");

    std::string expected = "!!map\n"
                           "!!null null: !<tag:yaml.org,2002:float> 3.14\n"
                           "!<tag:yaml.org,2002:bool> true: !!int 123\n"
                           "!!str foo: !!null null\n"
                           "map: !!map\n"
                           "  bar: false\n"
                           "seq: !!seq\n"
                           "  - null\n"
                           "  - 456\n";

    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(root) == expected);
}

TEST_CASE("Serializer_RootNodeWithDirectivesAndNodeProperties") {
    fkyaml::node root;
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;

    std::string expected = "%YAML 1.2\n"
                           "--- &anchor !!map\n"
                           "foo: bar\n";

    REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(expected)));

    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(root) == expected);
}

TEST_CASE("Serializer_NodesWithDirectives") {
    fkyaml::node root;
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;

    SUBCASE("YAML version 1.1") {
        std::string expected = "%YAML 1.1\n"
                               "---\n"
                               "foo: 123\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(expected)));
        REQUIRE(serializer.serialize(root) == expected);
    }

    SUBCASE("YAML version 1.2") {
        std::string expected = "%YAML 1.2\n"
                               "---\n"
                               "foo: 123\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(expected)));
        REQUIRE(serializer.serialize(root) == expected);
    }

    SUBCASE("primary handle prefix") {
        std::string expected = "%TAG ! tag:example.com,2000:\n"
                               "---\n"
                               "foo: 123\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(expected)));
        REQUIRE(serializer.serialize(root) == expected);
    }

    SUBCASE("secondary handle prefix") {
        std::string expected = "%TAG !! tag:example.com,2000:\n"
                               "---\n"
                               "foo: 123\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(expected)));
        REQUIRE(serializer.serialize(root) == expected);
    }

    SUBCASE("named handles") {
        std::string expected = "%TAG !e! tag:example.com,2000:\n"
                               "%TAG !t! !test-\n"
                               "---\n"
                               "foo: 123\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(expected)));
        REQUIRE(serializer.serialize(root) == expected);
    }
}

TEST_CASE("Serializer_MultipleDocuments") {
    std::vector<fkyaml::node> docs;
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;

    SUBCASE("bare documents") {
        std::string expected = "foo: bar\n"
                               "...\n"
                               "123: true\n";

        REQUIRE_NOTHROW(docs = deserializer.deserialize_docs(fkyaml::detail::input_adapter(expected)));
        REQUIRE(serializer.serialize_docs(docs) == expected);
    }

    SUBCASE("with directives") {
        std::string expected = "%YAML 1.2\n"
                               "---\n"
                               "foo: !!str bar\n"
                               "...\n"
                               "%TAG !t! !test-\n"
                               "---\n"
                               "test: !t!result success\n";

        REQUIRE_NOTHROW(docs = deserializer.deserialize_docs(fkyaml::detail::input_adapter(expected)));
        REQUIRE(serializer.serialize_docs(docs) == expected);
    }
}
