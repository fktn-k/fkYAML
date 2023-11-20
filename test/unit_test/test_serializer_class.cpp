//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.2.1
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <limits>

#include <catch2/catch.hpp>

#include <fkYAML/detail/output/serializer.hpp>
#include <fkYAML/node.hpp>

TEST_CASE("SerializerClassTest_SerializeSequenceNode", "[SerializerClassTest]")
{
    using node_str_pair_t = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(
        node_str_pair_t({true, false}, "- true\n- false\n"),
        node_str_pair_t({{{"foo", -1234}, {"bar", nullptr}}}, "-\n  bar: null\n  foo: -1234\n"));
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("SerializerClassTest_SerializeMappingNode", "[SerializerClassTest]")
{
    using node_str_pair_t = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(
        node_str_pair_t({{"foo", -1234}, {"bar", nullptr}}, "bar: null\nfoo: -1234\n"),
        node_str_pair_t({{"foo", {true, false}}}, "foo:\n  - true\n  - false\n"));
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("SerializerClassTest_SerializeNullNode", "[SerializerClassTest]")
{
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    fkyaml::node node;
    REQUIRE(serializer.serialize(node) == "null");
}

TEST_CASE("SerializerClassTest_SerializeBooleanNode", "[SerializerClassTest]")
{
    using node_str_pair_t = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(node_str_pair_t(false, "false"), node_str_pair_t(true, "true"));
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("SerializerClassTest_SerializeIntegerNode", "[SerializerClassTest]")
{
    using node_str_pair_t = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(node_str_pair_t(-1234, "-1234"), node_str_pair_t(5678, "5678"));
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("SerializeClassTest_SerializeFloatNumberNode", "[SerializeClassTest]")
{
    using node_str_pair_t = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(
        node_str_pair_t(3.14, "3.14"),
        node_str_pair_t(-53.97, "-53.97"),
        node_str_pair_t(std::numeric_limits<fkyaml::node::float_number_type>::infinity(), ".inf"),
        node_str_pair_t(-1 * std::numeric_limits<fkyaml::node::float_number_type>::infinity(), "-.inf"),
        node_str_pair_t(std::nan(""), ".nan"));
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("SerializerClassTest_SerializeStringNode", "[SerializerClassTest]")
{
    using node_str_pair_t = std::pair<fkyaml::node, std::string>;
    const char NEXT_LINE[] = { char(0xC2u), char(0x85u), char(0) };

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
        node_str_pair_t("\a", "\"\\a\""),
        node_str_pair_t("\b", "\"\\b\""),
        node_str_pair_t("\t", "\"\\t\""),
        node_str_pair_t("\n", "\"\\n\""),
        node_str_pair_t("\v", "\"\\v\""),
        node_str_pair_t("\f", "\"\\f\""),
        node_str_pair_t("\r", "\"\\r\""),
        node_str_pair_t(fkyaml::node::string_type({char(0x1Bu)}), "\"\\e\""),
        node_str_pair_t("\"", "\"\\\"\""),
        node_str_pair_t("\\", "\"\\\\\""),
        node_str_pair_t(NEXT_LINE, "\"\\N\""),
        node_str_pair_t(fkyaml::node::string_type({char(0xC2u), char(0xA0u)}), "\"\\_\""),
        node_str_pair_t(fkyaml::node::string_type({char(0xC3u), char(0xA0u)}), std::string({char(0xC3u), char(0xA0u)})),
        node_str_pair_t(fkyaml::node::string_type({char(0xC2u), char(0xA1u)}), std::string({char(0xC2u), char(0xA1u)})),
        node_str_pair_t(fkyaml::node::string_type({char(0xE2u), char(0x80u), char(0xA8u)}), "\"\\L\""),
        node_str_pair_t(fkyaml::node::string_type({char(0xE3u), char(0x80u), char(0xA8u)}), std::string({char(0xE3u), char(0x80u), char(0xA8u)})),
        node_str_pair_t(fkyaml::node::string_type({char(0xE2u), char(0x81u), char(0xA8u)}), std::string({char(0xE2u), char(0x81u), char(0xA8u)})),
        node_str_pair_t(fkyaml::node::string_type({char(0xE2u), char(0x80u), char(0xAAu)}), std::string({char(0xE2u), char(0x80u), char(0xAAu)})),
        node_str_pair_t(fkyaml::node::string_type({char(0xE2u), char(0x80u), char(0xA9u)}), "\"\\P\""));

    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}
