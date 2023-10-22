//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.1.3
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
    using NodeStrPair = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(
        NodeStrPair(
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(true), fkyaml::node::boolean_scalar(false)}),
            "- true\n- false\n"),
        NodeStrPair(
            fkyaml::node::sequence(
                {fkyaml::node::mapping({{"foo", fkyaml::node::integer_scalar(-1234)}, {"bar", fkyaml::node()}})}),
            "-\n  bar: null\n  foo: -1234\n"));
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("SerializerClassTest_SerializeMappingNode", "[SerializerClassTest]")
{
    using NodeStrPair = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(
        NodeStrPair(
            fkyaml::node::mapping({{"foo", fkyaml::node::integer_scalar(-1234)}, {"bar", fkyaml::node()}}),
            "bar: null\nfoo: -1234\n"),
        NodeStrPair(
            fkyaml::node::mapping(
                {{"foo",
                  fkyaml::node::sequence({fkyaml::node::boolean_scalar(true), fkyaml::node::boolean_scalar(false)})}}),
            "foo:\n  - true\n  - false\n"));
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
    using NodeStrPair = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(
        NodeStrPair(fkyaml::node::boolean_scalar(false), "false"),
        NodeStrPair(fkyaml::node::boolean_scalar(true), "true"));
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("SerializerClassTest_SerializeIntegerNode", "[SerializerClassTest]")
{
    using NodeStrPair = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(
        NodeStrPair(fkyaml::node::integer_scalar(-1234), "-1234"),
        NodeStrPair(fkyaml::node::integer_scalar(5678), "5678"));
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("SerializeClassTest_SerializeFloatNumberNode", "[SerializeClassTest]")
{
    using NodeStrPair = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(
        NodeStrPair(fkyaml::node::float_number_scalar(3.14), "3.14"),
        NodeStrPair(fkyaml::node::float_number_scalar(-53.97), "-53.97"),
        NodeStrPair(
            fkyaml::node::float_number_scalar(std::numeric_limits<fkyaml::node_float_number_type>::infinity()), ".inf"),
        NodeStrPair(
            fkyaml::node::float_number_scalar(-1 * std::numeric_limits<fkyaml::node_float_number_type>::infinity()),
            "-.inf"),
        NodeStrPair(fkyaml::node::float_number_scalar(std::nan("")), ".nan"));
    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}

TEST_CASE("SerializerClassTest_SerializeStringNode", "[SerializerClassTest]")
{
    using node_str_pair_t = std::pair<fkyaml::node, std::string>;
    auto node_str_pair = GENERATE(
        node_str_pair_t(fkyaml::node::string_scalar("test"), "test"),
        node_str_pair_t(fkyaml::node::string_scalar("foo bar"), "foo bar"));

    fkyaml::detail::basic_serializer<fkyaml::node> serializer;
    REQUIRE(serializer.serialize(node_str_pair.first) == node_str_pair.second);
}
