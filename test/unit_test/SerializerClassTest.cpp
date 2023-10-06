//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.1
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include "catch2/catch.hpp"

#include <limits>

#include "fkYAML/Node.hpp"
#include "fkYAML/Serializer.hpp"

TEST_CASE("SerializerClassTest_SerializeSequenceNode", "[SerializerClassTest]")
{
    using NodeStrPair = std::pair<fkyaml::Node, std::string>;
    auto node_str_pair = GENERATE(
        NodeStrPair(
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(true), fkyaml::Node::BooleanScalar(false)}),
            "- true\n- false\n"),
        NodeStrPair(
            fkyaml::Node::Sequence(
                {fkyaml::Node::Mapping({{"foo", fkyaml::Node::IntegerScalar(-1234)}, {"bar", fkyaml::Node()}})}),
            "-\n  foo: -1234\n  bar: null\n"));
    fkyaml::Serializer serializer;
    REQUIRE(serializer.Serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("SerializerClassTest_SerializeMappingNode", "[SerializerClassTest]")
{
    using NodeStrPair = std::pair<fkyaml::Node, std::string>;
    auto node_str_pair = GENERATE(
        NodeStrPair(
            fkyaml::Node::Mapping({{"foo", fkyaml::Node::IntegerScalar(-1234)}, {"bar", fkyaml::Node()}}),
            "foo: -1234\nbar: null\n"),
        NodeStrPair(
            fkyaml::Node::Mapping(
                {{"foo",
                  fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(true), fkyaml::Node::BooleanScalar(false)})}}),
            "foo:\n  - true\n  - false\n"));
    fkyaml::Serializer serializer;
    REQUIRE(serializer.Serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("SerializerClassTest_SerializeNullNode", "[SerializerClassTest]")
{
    fkyaml::Serializer serializer;
    fkyaml::Node node;
    REQUIRE(serializer.Serialize(node) == "null");
}

TEST_CASE("SerializerClassTest_SerializeBooleanNode", "[SerializerClassTest]")
{
    using NodeStrPair = std::pair<fkyaml::Node, std::string>;
    auto node_str_pair = GENERATE(
        NodeStrPair(fkyaml::Node::BooleanScalar(false), "false"),
        NodeStrPair(fkyaml::Node::BooleanScalar(true), "true"));
    fkyaml::Serializer serializer;
    REQUIRE(serializer.Serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("SerializerClassTest_SerializeIntegerNode", "[SerializerClassTest]")
{
    using NodeStrPair = std::pair<fkyaml::Node, std::string>;
    auto node_str_pair = GENERATE(
        NodeStrPair(fkyaml::Node::IntegerScalar(-1234), "-1234"),
        NodeStrPair(fkyaml::Node::IntegerScalar(5678), "5678"));
    fkyaml::Serializer serializer;
    REQUIRE(serializer.Serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("SerializeClassTest_SerializeFloatNumberNode", "[SerializeClassTest]")
{
    using NodeStrPair = std::pair<fkyaml::Node, std::string>;
    auto node_str_pair = GENERATE(
        NodeStrPair(fkyaml::Node::FloatNumberScalar(3.14), "3.14"),
        NodeStrPair(fkyaml::Node::FloatNumberScalar(-53.97), "-53.97"),
        NodeStrPair(
            fkyaml::Node::FloatNumberScalar(std::numeric_limits<fkyaml::NodeFloatNumberType>::infinity()), ".inf"),
        NodeStrPair(
            fkyaml::Node::FloatNumberScalar(-1 * std::numeric_limits<fkyaml::NodeFloatNumberType>::infinity()),
            "-.inf"),
        NodeStrPair(fkyaml::Node::FloatNumberScalar(std::nan("")), ".nan"));
    fkyaml::Serializer serializer;
    REQUIRE(serializer.Serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("SerializerClassTest_SerializeStringNode", "[SerializerClassTest]")
{
    using NodeStrPair = std::pair<fkyaml::Node, std::string>;
    auto node_str_pair = GENERATE(
        NodeStrPair(fkyaml::Node::StringScalar("test"), "test"),
        NodeStrPair(fkyaml::Node::StringScalar("foo bar"), "foo bar"));

    fkyaml::Serializer serializer;
    REQUIRE(serializer.Serialize(node_str_pair.first) == node_str_pair.second);
}
