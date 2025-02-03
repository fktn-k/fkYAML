//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <limits>

#include <catch2/catch.hpp>

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

    SECTION("child sequence item is an empty sequence node") {
        seq.as_seq().emplace_back(fkyaml::node::sequence());
        std::string expected = "- []\n";
        REQUIRE(serializer.serialize(seq) == expected);
    }

    SECTION("child sequence item is an empty mapping node") {
        seq.as_seq().emplace_back(fkyaml::node::mapping());
        std::string expected = "- {}\n";
        REQUIRE(serializer.serialize(seq) == expected);
    }

    SECTION("mapping value is an empty sequence node") {
        map["foo"] = seq;
        std::string expected = "foo: []\n";
        REQUIRE(serializer.serialize(map) == expected);
    }

    SECTION("mapping value is an empty mapping node") {
        map["foo"] = fkyaml::node::mapping();
        std::string expected = "foo: {}\n";
        REQUIRE(serializer.serialize(map) == expected);
    }

    SECTION("root empty sequence") {
        std::string expected = "[]\n";
        REQUIRE(serializer.serialize(seq) == expected);
    }

    SECTION("root empty mapping") {
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

TEST_CASE("SerializeClassTest_FloatNode", "[SerializeClassTest]") {
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
    auto node_str_pair = GENERATE_REF(
        node_str_pair_t("test", "test"),
        node_str_pair_t("foo bar", "foo bar"),
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
    fkyaml::node node = {{"foo", 123}};
    node["foo"].add_anchor_name("A");
    node.as_map().emplace(true, fkyaml::node::alias_of(node["foo"]));
    node.as_map().emplace(fkyaml::node::alias_of(node["foo"]), 3.14);
    node[nullptr] = {"bar", fkyaml::node::alias_of(node["foo"])};

    // FIXME: Semantic equality between the input & the output is not guranteed
    //        when anchors/aliases are contained in a YAML document.
    //        This is because mappings have no information to correctly revoke
    //        the original relations between anchors & aliases.
    //        Using fkyaml::ordered_map as the type of mappings should solve the
    //        issue.
    std::string expected = "null:\n"
                           "  - bar\n"
                           "  - *A\n"
                           "true: *A\n"
                           "*A : 3.14\n"
                           "foo: &A 123\n";

    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node) == expected);
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

    SECTION("YAML version 1.1") {
        std::string expected = "%YAML 1.1\n"
                               "---\n"
                               "foo: 123\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(expected)));
        REQUIRE(serializer.serialize(root) == expected);
    }

    SECTION("YAML version 1.2") {
        std::string expected = "%YAML 1.2\n"
                               "---\n"
                               "foo: 123\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(expected)));
        REQUIRE(serializer.serialize(root) == expected);
    }

    SECTION("primary handle prefix") {
        std::string expected = "%TAG ! tag:example.com,2000:\n"
                               "---\n"
                               "foo: 123\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(expected)));
        REQUIRE(serializer.serialize(root) == expected);
    }

    SECTION("secondary handle prefix") {
        std::string expected = "%TAG !! tag:example.com,2000:\n"
                               "---\n"
                               "foo: 123\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(expected)));
        REQUIRE(serializer.serialize(root) == expected);
    }

    SECTION("named handles") {
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

    SECTION("bare documents") {
        std::string expected = "foo: bar\n"
                               "...\n"
                               "123: true\n";

        REQUIRE_NOTHROW(docs = deserializer.deserialize_docs(fkyaml::detail::input_adapter(expected)));
        REQUIRE(serializer.serialize_docs(docs) == expected);
    }

    SECTION("with directives") {
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
