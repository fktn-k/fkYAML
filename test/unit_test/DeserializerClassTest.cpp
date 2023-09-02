/**
 * DeserializerClassTest.cpp - implementation of test functions for the Deserializer class
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#include "catch2/catch.hpp"

#include "fkYAML/Deserializer.hpp"

TEST_CASE("DeserializerClassTest_DeserializeBlockSequenceTest", "[DeserializerClassTest]")
{
    fkyaml::Deserializer deserializer;
    fkyaml::Node root;

    SECTION("Input source No.1.")
    {
        REQUIRE_NOTHROW(root = deserializer.Deserialize("test:\n  - foo\n  - bar"));

        REQUIRE(root.IsMapping());
        REQUIRE_NOTHROW(root.Size());
        REQUIRE(root.Size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::Node& test_node = root["test"];
        REQUIRE(test_node.IsSequence());
        REQUIRE_NOTHROW(test_node.Size());
        REQUIRE(test_node.Size() == 2);

        REQUIRE_NOTHROW(test_node[0]);
        fkyaml::Node& test_0_node = test_node[0];
        REQUIRE(test_0_node.IsString());
        REQUIRE_NOTHROW(test_0_node.Size());
        REQUIRE(test_0_node.Size() == 3);
        REQUIRE_NOTHROW(test_0_node.ToString());
        REQUIRE(test_0_node.ToString().compare("foo") == 0);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::Node& test_1_node = test_node[1];
        REQUIRE(test_1_node.IsString());
        REQUIRE_NOTHROW(test_1_node.Size());
        REQUIRE(test_1_node.Size() == 3);
        REQUIRE_NOTHROW(test_1_node.ToString());
        REQUIRE(test_1_node.ToString().compare("bar") == 0);
    }

    SECTION("Input source No.2.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.Deserialize("test:\n  - foo: true\n    bar: one\n  - foo: false\n    bar: two"));

        REQUIRE(root.IsMapping());
        REQUIRE_NOTHROW(root.Size());
        REQUIRE(root.Size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::Node& test_node = root["test"];
        REQUIRE(test_node.IsSequence());
        REQUIRE_NOTHROW(test_node.Size());
        REQUIRE(test_node.Size() == 2);

        REQUIRE_NOTHROW(test_node[0]);
        fkyaml::Node& test_0_node = test_node[0];
        REQUIRE(test_0_node.IsMapping());
        REQUIRE_NOTHROW(test_0_node.Size());
        REQUIRE(test_0_node.Size() == 2);

        REQUIRE_NOTHROW(test_0_node["foo"]);
        fkyaml::Node& test_0_foo_node = test_0_node["foo"];
        REQUIRE(test_0_foo_node.IsBoolean());
        REQUIRE_NOTHROW(test_0_foo_node.ToBoolean());
        REQUIRE(test_0_foo_node.ToBoolean() == true);

        REQUIRE_NOTHROW(test_0_node["bar"]);
        fkyaml::Node& test_0_bar_node = test_0_node["bar"];
        REQUIRE(test_0_bar_node.IsString());
        REQUIRE_NOTHROW(test_0_bar_node.ToString());
        REQUIRE(test_0_bar_node.ToString().compare("one") == 0);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::Node& test_1_node = test_node[1];
        REQUIRE(test_1_node.IsMapping());
        REQUIRE_NOTHROW(test_1_node.Size());
        REQUIRE(test_1_node.Size() == 2);

        REQUIRE_NOTHROW(test_1_node["foo"]);
        fkyaml::Node& test_1_foo_node = test_1_node["foo"];
        REQUIRE(test_1_foo_node.IsBoolean());
        REQUIRE_NOTHROW(test_1_foo_node.ToBoolean());
        REQUIRE(test_1_foo_node.ToBoolean() == false);

        REQUIRE_NOTHROW(test_1_node["bar"]);
        fkyaml::Node& test_1_bar_node = test_1_node["bar"];
        REQUIRE(test_1_bar_node.IsString());
        REQUIRE_NOTHROW(test_1_bar_node.ToString());
        REQUIRE(test_1_bar_node.ToString().compare("two") == 0);
    }

    SECTION("Input source No.3.")
    {
        REQUIRE_NOTHROW(root = deserializer.Deserialize("test:\n  - &anchor foo\n  - *anchor"));

        REQUIRE(root.IsMapping());
        REQUIRE_NOTHROW(root.Size());
        REQUIRE(root.Size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::Node& test_node = root["test"];
        REQUIRE(test_node.IsSequence());
        REQUIRE_NOTHROW(test_node.Size());
        REQUIRE(test_node.Size() == 2);

        REQUIRE_NOTHROW(test_node[0]);
        fkyaml::Node& test_0_node = test_node[0];
        REQUIRE(test_0_node.HasAnchorName());
        REQUIRE(test_0_node.GetAnchorName().compare("anchor") == 0);
        REQUIRE(test_0_node.IsString());
        REQUIRE_NOTHROW(test_0_node.Size());
        REQUIRE(test_0_node.Size() == 3);
        REQUIRE_NOTHROW(test_0_node.ToString());
        REQUIRE(test_0_node.ToString().compare("foo") == 0);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::Node& test_1_node = test_node[1];
        REQUIRE(test_1_node.IsString());
        REQUIRE_NOTHROW(test_1_node.Size());
        REQUIRE(test_1_node.Size() == 3);
        REQUIRE_NOTHROW(test_1_node.ToString());
        REQUIRE(test_1_node.ToString().compare("foo") == 0);
    }
}

TEST_CASE("DeserializerClassTest_DeserializeBlockMappingTest", "[DeserializerClassTest]")
{
    fkyaml::Deserializer deserializer;
    fkyaml::Node root;

    SECTION("Input source No.1.")
    {
        REQUIRE_NOTHROW(root = deserializer.Deserialize("foo: one\nbar: true\npi: 3.14"));

        REQUIRE(root.IsMapping());
        REQUIRE(root.Size() == 3);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::Node& foo_node = root["foo"];
        REQUIRE(foo_node.IsString());
        REQUIRE_NOTHROW(foo_node.ToString());
        REQUIRE(foo_node.ToString().compare("one") == 0);

        REQUIRE_NOTHROW(root["bar"]);
        fkyaml::Node& bar_node = root["bar"];
        REQUIRE(bar_node.IsBoolean());
        REQUIRE_NOTHROW(bar_node.ToBoolean());
        REQUIRE(bar_node.ToBoolean() == true);

        REQUIRE_NOTHROW(root["pi"]);
        fkyaml::Node& pi_node = root["pi"];
        REQUIRE(pi_node.IsFloatNumber());
        REQUIRE_NOTHROW(pi_node.ToFloatNumber());
        REQUIRE(pi_node.ToFloatNumber() == 3.14);
    }

    SECTION("Input source No.1.")
    {
        REQUIRE_NOTHROW(root = deserializer.Deserialize("test:\n  bool: true\n  foo: bar\n  pi: 3.14"));

        REQUIRE(root.IsMapping());
        REQUIRE_NOTHROW(root.Size());
        REQUIRE(root.Size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::Node& test_node = root["test"];
        REQUIRE(test_node.IsMapping());
        REQUIRE_NOTHROW(test_node.Size());
        REQUIRE(test_node.Size() == 3);

        REQUIRE_NOTHROW(test_node["bool"]);
        fkyaml::Node& test_bool_node = test_node["bool"];
        REQUIRE(test_bool_node.IsBoolean());
        REQUIRE_NOTHROW(test_bool_node.ToBoolean());
        REQUIRE(test_bool_node.ToBoolean() == true);

        REQUIRE_NOTHROW(test_node["foo"]);
        fkyaml::Node& test_foo_node = test_node["foo"];
        REQUIRE(test_foo_node.IsString());
        REQUIRE_NOTHROW(test_foo_node.ToString());
        REQUIRE(test_foo_node.ToString().compare("bar") == 0);

        REQUIRE_NOTHROW(test_node["pi"]);
        fkyaml::Node& test_pi_node = test_node["pi"];
        REQUIRE(test_pi_node.IsFloatNumber());
        REQUIRE_NOTHROW(test_pi_node.ToFloatNumber());
        REQUIRE(test_pi_node.ToFloatNumber() == 3.14);
    }
}

TEST_CASE("DeserializerClassTest_DeserializeFlowSequenceTest", "[DeserializerClassTest]")
{
    fkyaml::Deserializer deserializer;
    fkyaml::Node root;

    SECTION("Input source No.1.")
    {
        REQUIRE_NOTHROW(root = deserializer.Deserialize("test: [ foo, bar ]"));

        REQUIRE(root.IsMapping());
        REQUIRE_NOTHROW(root.Size());
        REQUIRE(root.Size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::Node& test_node = root["test"];
        REQUIRE(test_node.IsSequence());
        REQUIRE_NOTHROW(test_node.Size());
        REQUIRE(test_node.Size() == 2);

        REQUIRE_NOTHROW(test_node[0]);
        fkyaml::Node& test_0_node = test_node[0];
        REQUIRE(test_0_node.IsString());
        REQUIRE_NOTHROW(test_0_node.ToString());
        REQUIRE(test_0_node.ToString().compare("foo") == 0);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::Node& test_1_node = test_node[1];
        REQUIRE(test_1_node.IsString());
        REQUIRE_NOTHROW(test_1_node.ToString());
        REQUIRE(test_1_node.ToString().compare("bar") == 0);
    }
}

TEST_CASE("DeserializerClassTest_DeserializeFlowMappingTest", "[DeserializerClassTest]")
{
    fkyaml::Deserializer deserializer;
    fkyaml::Node root;

    SECTION("Input source No.1.")
    {
        REQUIRE_NOTHROW(root = deserializer.Deserialize("test: { bool: true, foo: bar, pi: 3.14 }"));

        REQUIRE(root.IsMapping());
        REQUIRE_NOTHROW(root.Size());
        REQUIRE(root.Size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::Node& test_node = root["test"];
        REQUIRE(test_node.IsMapping());
        REQUIRE_NOTHROW(test_node.Size());
        REQUIRE(test_node.Size() == 3);

        REQUIRE_NOTHROW(test_node["bool"]);
        fkyaml::Node& test_bool_node = test_node["bool"];
        REQUIRE(test_bool_node.IsBoolean());
        REQUIRE_NOTHROW(test_bool_node.ToBoolean());
        REQUIRE(test_bool_node.ToBoolean() == true);

        REQUIRE_NOTHROW(test_node["foo"]);
        fkyaml::Node& test_foo_node = test_node["foo"];
        REQUIRE(test_foo_node.IsString());
        REQUIRE_NOTHROW(test_foo_node.ToString());
        REQUIRE(test_foo_node.ToString().compare("bar") == 0);

        REQUIRE_NOTHROW(test_node["pi"]);
        fkyaml::Node& test_pi_node = test_node["pi"];
        REQUIRE(test_pi_node.IsFloatNumber());
        REQUIRE_NOTHROW(test_pi_node.ToFloatNumber());
        REQUIRE(test_pi_node.ToFloatNumber() == 3.14);
    }
}