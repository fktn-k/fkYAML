//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.1
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include "catch2/catch.hpp"

#include "fkYAML/Deserializer.hpp"

TEST_CASE("DeserializerClassTest_InputStringTest", "[DeserializerClassTest]")
{
    fkyaml::deserializer deserializer;

    REQUIRE_NOTHROW(deserializer.deserialize("test: hoge"));
    REQUIRE_THROWS_AS(deserializer.deserialize(nullptr), fkyaml::exception);
}

TEST_CASE("DeserializerClassTest_DeserializeKeySeparator", "[DeserializerClassTest]")
{
    fkyaml::deserializer deserializer;
    fkyaml::node root;

    SECTION("normal key-value cases")
    {
        auto input_str = GENERATE(
            std::string("test: hoge"), std::string("test:\n  foo: bar"), std::string("test:\n  - foo\n  - bar"));
        REQUIRE_NOTHROW(root = deserializer.deserialize(input_str.c_str()));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
    }

    SECTION("error cases")
    {
        auto input_str = GENERATE(std::string(": foo"), std::string("- : foo"));
        REQUIRE_THROWS_AS(root = deserializer.deserialize(input_str.c_str()), fkyaml::exception);
    }
}

TEST_CASE("DeserializerClassTest_DeserializeValueSeparator", "[DeserializerClassTest]")
{
    fkyaml::deserializer deserializer;
    fkyaml::node root;

    auto input_str = GENERATE(std::string("test: [ foo, bar ]"), std::string("test: { foo: bar, buz: val }"));
    REQUIRE_NOTHROW(root = deserializer.deserialize(input_str.c_str()));
    REQUIRE(root.is_mapping());
    REQUIRE(root.size() == 1);
}

TEST_CASE("DeserializerClassTest_DeserializeNullValue", "[DeserializerClassTes]")
{
    fkyaml::deserializer deserializer;
    fkyaml::node root;

    auto input_str = GENERATE(std::string("test: null"), std::string("Null: test"));
    REQUIRE_NOTHROW(root = deserializer.deserialize(input_str.c_str()));
    REQUIRE(root.is_mapping());
    REQUIRE(root.size() == 1);
}

TEST_CASE("DeserializerClassTest_DeserializeBooleanValue", "[DeserializerClassTest]")
{
    fkyaml::deserializer deserializer;
    fkyaml::node root;

    auto input_str =
        GENERATE(std::string("test: true"), std::string("test: [ True, False ]"), std::string("True: TRUE"));
    REQUIRE_NOTHROW(root = deserializer.deserialize(input_str.c_str()));
    REQUIRE(root.is_mapping());
    REQUIRE(root.size() == 1);
}

TEST_CASE("DeserializerClassTest_DeserializeNumericKey", "[DeserializerClassTest]")
{
    fkyaml::deserializer deserializer;
    fkyaml::node root;

    using StringValuePair = std::pair<std::string, std::string>;

    auto str_val_pair = GENERATE(StringValuePair("123: foo", "123"), StringValuePair("3.14: foo", "3.14"));
    REQUIRE_NOTHROW(root = deserializer.deserialize(str_val_pair.first.c_str()));
    REQUIRE(root.is_mapping());
    REQUIRE(root.size() == 1);
    REQUIRE(root.contains(str_val_pair.second));
    REQUIRE(root[str_val_pair.second].to_string() == "foo");
}

TEST_CASE("DeserializerClassTest_DeserializeBlockSequenceTest", "[DeserializerClassTest]")
{
    fkyaml::deserializer deserializer;
    fkyaml::node root;

    SECTION("Input source No.1.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("test:\n  - foo\n  - bar"));

        REQUIRE(root.is_mapping());
        REQUIRE_NOTHROW(root.size());
        REQUIRE(root.size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE_NOTHROW(test_node.size());
        REQUIRE(test_node.size() == 2);

        REQUIRE_NOTHROW(test_node[0]);
        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.is_string());
        REQUIRE_NOTHROW(test_0_node.size());
        REQUIRE(test_0_node.size() == 3);
        REQUIRE_NOTHROW(test_0_node.to_string());
        REQUIRE(test_0_node.to_string().compare("foo") == 0);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_string());
        REQUIRE_NOTHROW(test_1_node.size());
        REQUIRE(test_1_node.size() == 3);
        REQUIRE_NOTHROW(test_1_node.to_string());
        REQUIRE(test_1_node.to_string().compare("bar") == 0);
    }

    SECTION("Input source No.2.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize("test:\n  - foo: true\n    bar: one\n  - foo: false\n    bar: two"));

        REQUIRE(root.is_mapping());
        REQUIRE_NOTHROW(root.size());
        REQUIRE(root.size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE_NOTHROW(test_node.size());
        REQUIRE(test_node.size() == 2);

        REQUIRE_NOTHROW(test_node[0]);
        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.is_mapping());
        REQUIRE_NOTHROW(test_0_node.size());
        REQUIRE(test_0_node.size() == 2);

        REQUIRE_NOTHROW(test_0_node["foo"]);
        fkyaml::node& test_0_foo_node = test_0_node["foo"];
        REQUIRE(test_0_foo_node.is_boolean());
        REQUIRE_NOTHROW(test_0_foo_node.to_boolean());
        REQUIRE(test_0_foo_node.to_boolean() == true);

        REQUIRE_NOTHROW(test_0_node["bar"]);
        fkyaml::node& test_0_bar_node = test_0_node["bar"];
        REQUIRE(test_0_bar_node.is_string());
        REQUIRE_NOTHROW(test_0_bar_node.to_string());
        REQUIRE(test_0_bar_node.to_string().compare("one") == 0);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_mapping());
        REQUIRE_NOTHROW(test_1_node.size());
        REQUIRE(test_1_node.size() == 2);

        REQUIRE_NOTHROW(test_1_node["foo"]);
        fkyaml::node& test_1_foo_node = test_1_node["foo"];
        REQUIRE(test_1_foo_node.is_boolean());
        REQUIRE_NOTHROW(test_1_foo_node.to_boolean());
        REQUIRE(test_1_foo_node.to_boolean() == false);

        REQUIRE_NOTHROW(test_1_node["bar"]);
        fkyaml::node& test_1_bar_node = test_1_node["bar"];
        REQUIRE(test_1_bar_node.is_string());
        REQUIRE_NOTHROW(test_1_bar_node.to_string());
        REQUIRE(test_1_bar_node.to_string().compare("two") == 0);
    }

    SECTION("Input source No.3.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("test:\n  - &anchor true\n  - *anchor"));

        REQUIRE(root.is_mapping());
        REQUIRE_NOTHROW(root.size());
        REQUIRE(root.size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE_NOTHROW(test_node.size());
        REQUIRE(test_node.size() == 2);

        REQUIRE_NOTHROW(test_node[0]);
        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.has_anchor_name());
        REQUIRE(test_0_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(test_0_node.is_boolean());
        REQUIRE_NOTHROW(test_0_node.to_boolean());
        REQUIRE(test_0_node.to_boolean() == true);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_boolean());
        REQUIRE_NOTHROW(test_1_node.to_boolean());
        REQUIRE(test_1_node.to_boolean() == test_0_node.to_boolean());
    }

    SECTION("Input source No.4.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("test:\n  - &anchor -123\n  - *anchor"));

        REQUIRE(root.is_mapping());
        REQUIRE_NOTHROW(root.size());
        REQUIRE(root.size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE_NOTHROW(test_node.size());
        REQUIRE(test_node.size() == 2);

        REQUIRE_NOTHROW(test_node[0]);
        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.has_anchor_name());
        REQUIRE(test_0_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(test_0_node.is_integer());
        REQUIRE_NOTHROW(test_0_node.to_integer());
        REQUIRE(test_0_node.to_integer() == -123);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_integer());
        REQUIRE_NOTHROW(test_1_node.to_integer());
        REQUIRE(test_1_node.to_integer() == test_0_node.to_integer());
    }

    SECTION("Input source No.5.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("test:\n  - &anchor 567\n  - *anchor"));

        REQUIRE(root.is_mapping());
        REQUIRE_NOTHROW(root.size());
        REQUIRE(root.size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE_NOTHROW(test_node.size());
        REQUIRE(test_node.size() == 2);

        REQUIRE_NOTHROW(test_node[0]);
        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.has_anchor_name());
        REQUIRE(test_0_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(test_0_node.is_integer());
        REQUIRE_NOTHROW(test_0_node.to_integer());
        REQUIRE(test_0_node.to_integer() == 567);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_integer());
        REQUIRE_NOTHROW(test_1_node.to_integer());
        REQUIRE(test_1_node.to_integer() == test_0_node.to_integer());
    }

    SECTION("Input source No.6.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("test:\n  - &anchor 3.14\n  - *anchor"));

        REQUIRE(root.is_mapping());
        REQUIRE_NOTHROW(root.size());
        REQUIRE(root.size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE_NOTHROW(test_node.size());
        REQUIRE(test_node.size() == 2);

        REQUIRE_NOTHROW(test_node[0]);
        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.has_anchor_name());
        REQUIRE(test_0_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(test_0_node.is_float_number());
        REQUIRE_NOTHROW(test_0_node.to_float_number());
        REQUIRE(test_0_node.to_float_number() == 3.14);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_float_number());
        REQUIRE_NOTHROW(test_1_node.to_float_number());
        REQUIRE(test_1_node.to_float_number() == test_0_node.to_float_number());
    }

    SECTION("Input source No.7.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("test:\n  - &anchor foo\n  - *anchor"));

        REQUIRE(root.is_mapping());
        REQUIRE_NOTHROW(root.size());
        REQUIRE(root.size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE_NOTHROW(test_node.size());
        REQUIRE(test_node.size() == 2);

        REQUIRE_NOTHROW(test_node[0]);
        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.has_anchor_name());
        REQUIRE(test_0_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(test_0_node.is_string());
        REQUIRE_NOTHROW(test_0_node.size());
        REQUIRE(test_0_node.size() == 3);
        REQUIRE_NOTHROW(test_0_node.to_string());
        REQUIRE(test_0_node.to_string().compare("foo") == 0);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_string());
        REQUIRE_NOTHROW(test_1_node.size());
        REQUIRE(test_1_node.size() == 3);
        REQUIRE_NOTHROW(test_1_node.to_string());
        REQUIRE(test_1_node.to_string().compare("foo") == 0);
    }
}

TEST_CASE("DeserializerClassTest_DeserializeBlockMappingTest", "[DeserializerClassTest]")
{
    fkyaml::deserializer deserializer;
    fkyaml::node root;

    SECTION("Input source No.1.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("foo: one\nbar: true\npi: 3.14"));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 3);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE_NOTHROW(foo_node.to_string());
        REQUIRE(foo_node.to_string().compare("one") == 0);

        REQUIRE_NOTHROW(root["bar"]);
        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_boolean());
        REQUIRE_NOTHROW(bar_node.to_boolean());
        REQUIRE(bar_node.to_boolean() == true);

        REQUIRE_NOTHROW(root["pi"]);
        fkyaml::node& pi_node = root["pi"];
        REQUIRE(pi_node.is_float_number());
        REQUIRE_NOTHROW(pi_node.to_float_number());
        REQUIRE(pi_node.to_float_number() == 3.14);
    }

    SECTION("Input source No.2.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("test:\n  bool: true\n  foo: bar\n  pi: 3.14"));

        REQUIRE(root.is_mapping());
        REQUIRE_NOTHROW(root.size());
        REQUIRE(root.size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_mapping());
        REQUIRE_NOTHROW(test_node.size());
        REQUIRE(test_node.size() == 3);

        REQUIRE_NOTHROW(test_node["bool"]);
        fkyaml::node& test_bool_node = test_node["bool"];
        REQUIRE(test_bool_node.is_boolean());
        REQUIRE_NOTHROW(test_bool_node.to_boolean());
        REQUIRE(test_bool_node.to_boolean() == true);

        REQUIRE_NOTHROW(test_node["foo"]);
        fkyaml::node& test_foo_node = test_node["foo"];
        REQUIRE(test_foo_node.is_string());
        REQUIRE_NOTHROW(test_foo_node.to_string());
        REQUIRE(test_foo_node.to_string().compare("bar") == 0);

        REQUIRE_NOTHROW(test_node["pi"]);
        fkyaml::node& test_pi_node = test_node["pi"];
        REQUIRE(test_pi_node.is_float_number());
        REQUIRE_NOTHROW(test_pi_node.to_float_number());
        REQUIRE(test_pi_node.to_float_number() == 3.14);
    }

    SECTION("Input source No.3.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("foo: &anchor true\nbar: *anchor"));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.has_anchor_name());
        REQUIRE(foo_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(foo_node.is_boolean());
        REQUIRE_NOTHROW(foo_node.to_boolean());
        REQUIRE(foo_node.to_boolean() == true);

        REQUIRE_NOTHROW(root["bar"]);
        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_boolean());
        REQUIRE_NOTHROW(bar_node.to_boolean());
        REQUIRE(bar_node.to_boolean() == foo_node.to_boolean());
    }

    SECTION("Input source No.4.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("foo: &anchor -123\nbar: *anchor"));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.has_anchor_name());
        REQUIRE(foo_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(foo_node.is_integer());
        REQUIRE_NOTHROW(foo_node.to_integer());
        REQUIRE(foo_node.to_integer() == -123);

        REQUIRE_NOTHROW(root["bar"]);
        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_integer());
        REQUIRE_NOTHROW(bar_node.to_integer());
        REQUIRE(bar_node.to_integer() == foo_node.to_integer());
    }

    SECTION("Input source No.5.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("foo: &anchor 567\nbar: *anchor"));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.has_anchor_name());
        REQUIRE(foo_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(foo_node.is_integer());
        REQUIRE_NOTHROW(foo_node.to_integer());
        REQUIRE(foo_node.to_integer() == 567);

        REQUIRE_NOTHROW(root["bar"]);
        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_integer());
        REQUIRE_NOTHROW(bar_node.to_integer());
        REQUIRE(bar_node.to_integer() == foo_node.to_integer());
    }

    SECTION("Input source No.6.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("foo: &anchor 3.14\nbar: *anchor"));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.has_anchor_name());
        REQUIRE(foo_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(foo_node.is_float_number());
        REQUIRE_NOTHROW(foo_node.to_float_number());
        REQUIRE(foo_node.to_float_number() == 3.14);

        REQUIRE_NOTHROW(root["bar"]);
        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_float_number());
        REQUIRE_NOTHROW(bar_node.to_float_number());
        REQUIRE(bar_node.to_float_number() == foo_node.to_float_number());
    }

    SECTION("Input source No.7.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("foo: &anchor one\nbar: *anchor"));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.has_anchor_name());
        REQUIRE(foo_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(foo_node.is_string());
        REQUIRE_NOTHROW(foo_node.to_string());
        REQUIRE(foo_node.to_string().compare("one") == 0);

        REQUIRE_NOTHROW(root["bar"]);
        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_string());
        REQUIRE_NOTHROW(bar_node.to_string());
        REQUIRE(bar_node.to_string() == foo_node.to_string());
    }
}

TEST_CASE("DeserializerClassTest_DeserializeFlowSequenceTest", "[DeserializerClassTest]")
{
    fkyaml::deserializer deserializer;
    fkyaml::node root;

    SECTION("Input source No.1.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("test: [ foo, bar ]"));

        REQUIRE(root.is_mapping());
        REQUIRE_NOTHROW(root.size());
        REQUIRE(root.size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE_NOTHROW(test_node.size());
        REQUIRE(test_node.size() == 2);

        REQUIRE_NOTHROW(test_node[0]);
        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.is_string());
        REQUIRE_NOTHROW(test_0_node.to_string());
        REQUIRE(test_0_node.to_string().compare("foo") == 0);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_string());
        REQUIRE_NOTHROW(test_1_node.to_string());
        REQUIRE(test_1_node.to_string().compare("bar") == 0);
    }
}

TEST_CASE("DeserializerClassTest_DeserializeFlowMappingTest", "[DeserializerClassTest]")
{
    fkyaml::deserializer deserializer;
    fkyaml::node root;

    SECTION("Input source No.1.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("test: { bool: true, foo: bar, pi: 3.14 }"));

        REQUIRE(root.is_mapping());
        REQUIRE_NOTHROW(root.size());
        REQUIRE(root.size() == 1);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_mapping());
        REQUIRE_NOTHROW(test_node.size());
        REQUIRE(test_node.size() == 3);

        REQUIRE_NOTHROW(test_node["bool"]);
        fkyaml::node& test_bool_node = test_node["bool"];
        REQUIRE(test_bool_node.is_boolean());
        REQUIRE_NOTHROW(test_bool_node.to_boolean());
        REQUIRE(test_bool_node.to_boolean() == true);

        REQUIRE_NOTHROW(test_node["foo"]);
        fkyaml::node& test_foo_node = test_node["foo"];
        REQUIRE(test_foo_node.is_string());
        REQUIRE_NOTHROW(test_foo_node.to_string());
        REQUIRE(test_foo_node.to_string().compare("bar") == 0);

        REQUIRE_NOTHROW(test_node["pi"]);
        fkyaml::node& test_pi_node = test_node["pi"];
        REQUIRE(test_pi_node.is_float_number());
        REQUIRE_NOTHROW(test_pi_node.to_float_number());
        REQUIRE(test_pi_node.to_float_number() == 3.14);
    }
}

TEST_CASE("DeserializerClassTest_DeserializeInputWithCommentTest", "[DeserializerClassTest]")
{
    fkyaml::deserializer deserializer;
    fkyaml::node root;

    REQUIRE_NOTHROW(root = deserializer.deserialize("foo: one # comment\nbar: true\npi: 3.14"));

    REQUIRE(root.is_mapping());
    REQUIRE(root.size() == 3);

    REQUIRE_NOTHROW(root["foo"]);
    fkyaml::node& foo_node = root["foo"];
    REQUIRE(foo_node.is_string());
    REQUIRE_NOTHROW(foo_node.to_string());
    REQUIRE(foo_node.to_string().compare("one") == 0);

    REQUIRE_NOTHROW(root["bar"]);
    fkyaml::node& bar_node = root["bar"];
    REQUIRE(bar_node.is_boolean());
    REQUIRE_NOTHROW(bar_node.to_boolean());
    REQUIRE(bar_node.to_boolean() == true);

    REQUIRE_NOTHROW(root["pi"]);
    fkyaml::node& pi_node = root["pi"];
    REQUIRE(pi_node.is_float_number());
    REQUIRE_NOTHROW(pi_node.to_float_number());
    REQUIRE(pi_node.to_float_number() == 3.14);
}

TEST_CASE("DeserializerClassTest_DeserializeYAMLVerDirectiveTest", "[DeserializerClassTest]")
{
    fkyaml::deserializer deserializer;
    fkyaml::node root;

    SECTION("YAML 1.1")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("%YAML 1.1\nfoo: one"));

        REQUIRE(root.get_yaml_version() == fkyaml::yaml_version_t::VER_1_1);
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(root.get_yaml_version() == fkyaml::yaml_version_t::VER_1_1);
        REQUIRE(foo_node.is_string());
        REQUIRE_NOTHROW(foo_node.to_string());
        REQUIRE(foo_node.to_string().compare("one") == 0);
    }

    SECTION("YAML 1.2")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize("%YAML 1.2\nfoo: one"));

        REQUIRE(root.get_yaml_version() == fkyaml::yaml_version_t::VER_1_2);
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(root.get_yaml_version() == fkyaml::yaml_version_t::VER_1_2);
        REQUIRE(foo_node.is_string());
        REQUIRE_NOTHROW(foo_node.to_string());
        REQUIRE(foo_node.to_string().compare("one") == 0);
    }
}

TEST_CASE("DeserializerClassTest_DeserializeNoMachingAnchorTest", "[DeserializerClassTest]")
{
    fkyaml::deserializer deserializer;
    REQUIRE_THROWS_AS(deserializer.deserialize("foo: *anchor"), fkyaml::exception);
}
