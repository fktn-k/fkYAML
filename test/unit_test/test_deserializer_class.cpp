//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.2.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#include <fkYAML/detail/input/deserializer.hpp>
#include <fkYAML/detail/input/input_adapter.hpp>
#include <fkYAML/node.hpp>

TEST_CASE("DeserializerClassTest_DeserializeKeySeparator", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("normal key-value cases")
    {
        auto input_str = GENERATE(
            std::string("test: hoge"), std::string("test:\n  foo: bar"), std::string("test:\n  - foo\n  - bar"));
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input_str)));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
    }

    SECTION("error cases")
    {
        auto input_str = GENERATE(std::string(": foo"), std::string("- : foo"), std::string("- - : foo"));
        REQUIRE_THROWS_AS(
            root = deserializer.deserialize(fkyaml::detail::input_adapter(input_str)), fkyaml::parse_error);
    }
}

TEST_CASE("DeserializerClassTest_DeserializeValueSeparator", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    auto input_str = GENERATE(std::string("test: [ foo, bar ]"), std::string("test: { foo: bar, buz: val }"));
    REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input_str)));
    REQUIRE(root.is_mapping());
    REQUIRE(root.size() == 1);
}

TEST_CASE("DeserializerClassTest_DeserializeNullValue", "[DeserializerClassTes]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("key not in a sequence.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("Null: test")));
        REQUIRE(root.contains("Null"));
    }

    SECTION("key in a sequence.")
    {
        auto input = GENERATE(std::string("test:\n  - null: foo"), std::string("test:\n  - null:\n      - true"));
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root["test"][0].contains("null"));
    }

    SECTION("mapping value.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test: null")));
        REQUIRE(root["test"].is_null());
    }

    SECTION("sequence value.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - null")));
        REQUIRE(root["test"][0].is_null());
    }
}

TEST_CASE("DeserializerClassTest_DeserializeBooleanValue", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("key not in a sequence.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("true: test")));
        REQUIRE(root.contains("true"));
    }

    SECTION("key in a sequence.")
    {
        auto input = GENERATE(std::string("test:\n  - false: foo"), std::string("test:\n  - false:\n      - null"));
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root["test"][0].contains("false"));
    }

    SECTION("mapping value.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test: TRUE")));
        REQUIRE(root["test"].get_value<bool>() == true);
    }

    SECTION("sequence value.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - False")));
        REQUIRE(root["test"][0].get_value<bool>() == false);
    }
}

TEST_CASE("DeserializerClassTest_DeserializeIntegerKey", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("key not in a sequence.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("123: test")));
        REQUIRE(root.contains("123"));
    }

    SECTION("key in a sequence.")
    {
        auto input = GENERATE(std::string("test:\n  - 123: foo"), std::string("test:\n  - 123:\n      - true"));
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root["test"][0].contains("123"));
    }

    SECTION("mapping value.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test: 123")));
        REQUIRE(root["test"].get_value<int>() == 123);
    }

    SECTION("sequence value.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - 123")));
        REQUIRE(root["test"][0].get_value<int>() == 123);
    }
}

TEST_CASE("DeserializerClassTest_DeserializeFloatingPointNumberKey", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("key not in a sequence.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("3.14: test")));
        REQUIRE(root.contains("3.14"));
    }

    SECTION("key in a sequence.")
    {
        auto input = GENERATE(std::string("test:\n  - .inf: foo"), std::string("test:\n  - .inf:\n      - true"));
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root["test"][0].contains(".inf"));
    }

    SECTION("mapping value.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test: .nan")));
        REQUIRE(std::isnan(root["test"].get_value<double>()));
    }

    SECTION("sequence value.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - 1.23e-5")));
        REQUIRE(root["test"][0].get_value<double>() == 1.23e-5);
    }
}

TEST_CASE("DeserializerClassTest_DeserializeInvalidIndentation", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    REQUIRE_THROWS_AS(
        root = deserializer.deserialize(fkyaml::detail::input_adapter("foo:\n  bar: baz\n qux: true")),
        fkyaml::parse_error);
}

TEST_CASE("DeserializerClassTest_DeserializeDuplicateKeys", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    REQUIRE_THROWS_AS(
        root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: bar\nfoo: baz")), fkyaml::parse_error);
}

TEST_CASE("DeserializerClassTest_DeserializeBlockSequenceTest", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("Input source No.1.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - \'foo\'\n  - bar")));

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
        REQUIRE_NOTHROW(test_0_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(test_0_node.get_value_ref<fkyaml::node::string_type&>().compare("foo") == 0);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_string());
        REQUIRE_NOTHROW(test_1_node.size());
        REQUIRE(test_1_node.size() == 3);
        REQUIRE_NOTHROW(test_1_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(test_1_node.get_value_ref<fkyaml::node::string_type&>().compare("bar") == 0);
    }

    SECTION("Input source No.2.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(
                fkyaml::detail::input_adapter("test:\n  - foo: true\n    bar: one\n  - foo: false\n    bar: two")));

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
        REQUIRE_NOTHROW(test_0_foo_node.get_value<fkyaml::node::boolean_type>());
        REQUIRE(test_0_foo_node.get_value<fkyaml::node::boolean_type>() == true);

        REQUIRE_NOTHROW(test_0_node["bar"]);
        fkyaml::node& test_0_bar_node = test_0_node["bar"];
        REQUIRE(test_0_bar_node.is_string());
        REQUIRE_NOTHROW(test_0_bar_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(test_0_bar_node.get_value_ref<fkyaml::node::string_type&>().compare("one") == 0);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_mapping());
        REQUIRE_NOTHROW(test_1_node.size());
        REQUIRE(test_1_node.size() == 2);

        REQUIRE_NOTHROW(test_1_node["foo"]);
        fkyaml::node& test_1_foo_node = test_1_node["foo"];
        REQUIRE(test_1_foo_node.is_boolean());
        REQUIRE_NOTHROW(test_1_foo_node.get_value<fkyaml::node::boolean_type>());
        REQUIRE(test_1_foo_node.get_value<fkyaml::node::boolean_type>() == false);

        REQUIRE_NOTHROW(test_1_node["bar"]);
        fkyaml::node& test_1_bar_node = test_1_node["bar"];
        REQUIRE(test_1_bar_node.is_string());
        REQUIRE_NOTHROW(test_1_bar_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(test_1_bar_node.get_value_ref<fkyaml::node::string_type&>().compare("two") == 0);
    }

    SECTION("Input source No.3.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - &anchor true\n  - *anchor")));

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
        REQUIRE_NOTHROW(test_0_node.get_value<fkyaml::node::boolean_type>());
        REQUIRE(test_0_node.get_value<fkyaml::node::boolean_type>() == true);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_boolean());
        REQUIRE_NOTHROW(test_1_node.get_value<fkyaml::node::boolean_type>());
        REQUIRE(
            test_1_node.get_value<fkyaml::node::boolean_type>() == test_0_node.get_value<fkyaml::node::boolean_type>());
    }

    SECTION("Input source No.4.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - &anchor -123\n  - *anchor")));

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
        REQUIRE_NOTHROW(test_0_node.get_value<fkyaml::node::integer_type>());
        REQUIRE(test_0_node.get_value<fkyaml::node::integer_type>() == -123);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_integer());
        REQUIRE_NOTHROW(test_1_node.get_value<fkyaml::node::integer_type>());
        REQUIRE(
            test_1_node.get_value<fkyaml::node::integer_type>() == test_0_node.get_value<fkyaml::node::integer_type>());
    }

    SECTION("Input source No.5.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - &anchor 567\n  - *anchor")));

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
        REQUIRE_NOTHROW(test_0_node.get_value<fkyaml::node::integer_type>());
        REQUIRE(test_0_node.get_value<fkyaml::node::integer_type>() == 567);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_integer());
        REQUIRE_NOTHROW(test_1_node.get_value<fkyaml::node::integer_type>());
        REQUIRE(
            test_1_node.get_value<fkyaml::node::integer_type>() == test_0_node.get_value<fkyaml::node::integer_type>());
    }

    SECTION("Input source No.6.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - &anchor 3.14\n  - *anchor")));

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
        REQUIRE_NOTHROW(test_0_node.get_value<fkyaml::node::float_number_type>());
        REQUIRE(test_0_node.get_value<fkyaml::node::float_number_type>() == 3.14);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_float_number());
        REQUIRE_NOTHROW(test_1_node.get_value<fkyaml::node::float_number_type>());
        REQUIRE(
            test_1_node.get_value<fkyaml::node::float_number_type>() ==
            test_0_node.get_value<fkyaml::node::float_number_type>());
    }

    SECTION("Input source No.7.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - &anchor foo\n  - *anchor")));

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
        REQUIRE_NOTHROW(test_0_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(test_0_node.get_value_ref<fkyaml::node::string_type&>().compare("foo") == 0);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_string());
        REQUIRE_NOTHROW(test_1_node.size());
        REQUIRE(test_1_node.size() == 3);
        REQUIRE_NOTHROW(test_1_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(test_1_node.get_value_ref<fkyaml::node::string_type&>().compare("foo") == 0);
    }
}

TEST_CASE("DeserializerClassTest_DeserializeBlockMappingTest", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("Input source No.1.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: one\nbar: true\npi: 3.14")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 3);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE_NOTHROW(foo_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(foo_node.get_value_ref<fkyaml::node::string_type&>().compare("one") == 0);

        REQUIRE_NOTHROW(root["bar"]);
        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_boolean());
        REQUIRE_NOTHROW(bar_node.get_value<fkyaml::node::boolean_type>());
        REQUIRE(bar_node.get_value<fkyaml::node::boolean_type>() == true);

        REQUIRE_NOTHROW(root["pi"]);
        fkyaml::node& pi_node = root["pi"];
        REQUIRE(pi_node.is_float_number());
        REQUIRE_NOTHROW(pi_node.get_value<fkyaml::node::float_number_type>());
        REQUIRE(pi_node.get_value<fkyaml::node::float_number_type>() == 3.14);
    }

    SECTION("Input source No.2.")
    {
        REQUIRE_NOTHROW(
            root =
                deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  bool: true\n  foo: bar\n  pi: 3.14")));

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
        REQUIRE_NOTHROW(test_bool_node.get_value<fkyaml::node::boolean_type>());
        REQUIRE(test_bool_node.get_value<fkyaml::node::boolean_type>() == true);

        REQUIRE_NOTHROW(test_node["foo"]);
        fkyaml::node& test_foo_node = test_node["foo"];
        REQUIRE(test_foo_node.is_string());
        REQUIRE_NOTHROW(test_foo_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(test_foo_node.get_value_ref<fkyaml::node::string_type&>().compare("bar") == 0);

        REQUIRE_NOTHROW(test_node["pi"]);
        fkyaml::node& test_pi_node = test_node["pi"];
        REQUIRE(test_pi_node.is_float_number());
        REQUIRE_NOTHROW(test_pi_node.get_value<fkyaml::node::float_number_type>());
        REQUIRE(test_pi_node.get_value<fkyaml::node::float_number_type>() == 3.14);
    }

    SECTION("Input source No.3.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: &anchor true\nbar: *anchor")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.has_anchor_name());
        REQUIRE(foo_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(foo_node.is_boolean());
        REQUIRE_NOTHROW(foo_node.get_value<fkyaml::node::boolean_type>());
        REQUIRE(foo_node.get_value<fkyaml::node::boolean_type>() == true);

        REQUIRE_NOTHROW(root["bar"]);
        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_boolean());
        REQUIRE_NOTHROW(bar_node.get_value<fkyaml::node::boolean_type>());
        REQUIRE(bar_node.get_value<fkyaml::node::boolean_type>() == foo_node.get_value<fkyaml::node::boolean_type>());
    }

    SECTION("Input source No.4.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: &anchor -123\nbar: *anchor")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.has_anchor_name());
        REQUIRE(foo_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(foo_node.is_integer());
        REQUIRE_NOTHROW(foo_node.get_value<fkyaml::node::integer_type>());
        REQUIRE(foo_node.get_value<fkyaml::node::integer_type>() == -123);

        REQUIRE_NOTHROW(root["bar"]);
        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_integer());
        REQUIRE_NOTHROW(bar_node.get_value<fkyaml::node::integer_type>());
        REQUIRE(bar_node.get_value<fkyaml::node::integer_type>() == foo_node.get_value<fkyaml::node::integer_type>());
    }

    SECTION("Input source No.5.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: &anchor 567\nbar: *anchor")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.has_anchor_name());
        REQUIRE(foo_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(foo_node.is_integer());
        REQUIRE_NOTHROW(foo_node.get_value<fkyaml::node::integer_type>());
        REQUIRE(foo_node.get_value<fkyaml::node::integer_type>() == 567);

        REQUIRE_NOTHROW(root["bar"]);
        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_integer());
        REQUIRE_NOTHROW(bar_node.get_value<fkyaml::node::integer_type>());
        REQUIRE(bar_node.get_value<fkyaml::node::integer_type>() == foo_node.get_value<fkyaml::node::integer_type>());
    }

    SECTION("Input source No.6.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: &anchor 3.14\nbar: *anchor")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.has_anchor_name());
        REQUIRE(foo_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(foo_node.is_float_number());
        REQUIRE_NOTHROW(foo_node.get_value<fkyaml::node::float_number_type>());
        REQUIRE(foo_node.get_value<fkyaml::node::float_number_type>() == 3.14);

        REQUIRE_NOTHROW(root["bar"]);
        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_float_number());
        REQUIRE_NOTHROW(bar_node.get_value<fkyaml::node::float_number_type>());
        REQUIRE(
            bar_node.get_value<fkyaml::node::float_number_type>() ==
            foo_node.get_value<fkyaml::node::float_number_type>());
    }

    SECTION("Input source No.7.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: &anchor one\nbar: *anchor")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.has_anchor_name());
        REQUIRE(foo_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(foo_node.is_string());
        REQUIRE_NOTHROW(foo_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(foo_node.get_value_ref<fkyaml::node::string_type&>().compare("one") == 0);

        REQUIRE_NOTHROW(root["bar"]);
        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_string());
        REQUIRE_NOTHROW(bar_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(
            bar_node.get_value_ref<fkyaml::node::string_type&>() ==
            foo_node.get_value_ref<fkyaml::node::string_type&>());
    }

    SECTION("Input source No.8.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(
                fkyaml::detail::input_adapter("foo:\n  bar: baz\nqux: 123\nquux:\n  corge: grault")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 3);

        REQUIRE(root.contains("foo"));
        REQUIRE(root["foo"].is_mapping());
        REQUIRE(root["foo"].size() == 1);
        REQUIRE(root["foo"].contains("bar"));
        REQUIRE(root["foo"]["bar"].is_string());
        REQUIRE(root["foo"]["bar"].get_value_ref<fkyaml::node::string_type&>() == "baz");

        REQUIRE(root.contains("qux"));
        REQUIRE(root["qux"].is_integer());
        REQUIRE(root["qux"].get_value<fkyaml::node::integer_type>() == 123);

        REQUIRE(root.contains("quux"));
        REQUIRE(root["quux"].is_mapping());
        REQUIRE(root["quux"].size() == 1);
        REQUIRE(root["quux"].contains("corge"));
        REQUIRE(root["quux"]["corge"].is_string());
        REQUIRE(root["quux"]["corge"].get_value_ref<fkyaml::node::string_type&>() == "grault");
    }

    SECTION("Input source No.9.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo:\n  bar:\n    baz: 123\nqux: true")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE(root.contains("foo"));
        REQUIRE(root["foo"].is_mapping());
        REQUIRE(root["foo"].size() == 1);

        REQUIRE(root["foo"].contains("bar"));
        REQUIRE(root["foo"]["bar"].is_mapping());
        REQUIRE(root["foo"]["bar"].size() == 1);

        REQUIRE(root["foo"]["bar"].contains("baz"));
        REQUIRE(root["foo"]["bar"]["baz"].is_integer());
        REQUIRE(root["foo"]["bar"]["baz"].get_value<fkyaml::node::integer_type>() == 123);

        REQUIRE(root.contains("qux"));
        REQUIRE(root["qux"].is_boolean());
        REQUIRE(root["qux"].get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("Input source No.10.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo:\n  - bar\n  - 123\nbaz: qux")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE(root.contains("foo"));
        REQUIRE(root["foo"].is_sequence());
        REQUIRE(root["foo"].size() == 2);

        REQUIRE(root["foo"][0].is_string());
        REQUIRE(root["foo"][0].get_value_ref<std::string&>() == "bar");

        REQUIRE(root["foo"][1].is_integer());
        REQUIRE(root["foo"][1].get_value<int>() == 123);

        REQUIRE(root.contains("baz"));
        REQUIRE(root["baz"].is_string());
        REQUIRE(root["baz"].get_value_ref<std::string&>() == "qux");
    }

    SECTION("Input source No.11.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo:\n  - bar: baz\nqux: corge")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE(root.contains("foo"));
        REQUIRE(root["foo"].is_sequence());
        REQUIRE(root["foo"].size() == 1);

        REQUIRE(root["foo"][0].is_mapping());
        REQUIRE(root["foo"][0].size() == 1);

        REQUIRE(root["foo"][0].contains("bar"));
        REQUIRE(root["foo"][0]["bar"].is_string());
        REQUIRE(root["foo"][0]["bar"].get_value_ref<std::string&>() == "baz");

        REQUIRE(root.contains("qux"));
        REQUIRE(root["qux"].is_string());
        REQUIRE(root["qux"].get_value_ref<std::string&>() == "corge");
    }

    SECTION("Input source No.12.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(
                fkyaml::detail::input_adapter("foo:\n  - bar: true\n    baz: 123\nqux: corge")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE(root.contains("foo"));
        REQUIRE(root["foo"].is_sequence());
        REQUIRE(root["foo"].size() == 1);

        REQUIRE(root["foo"][0].is_mapping());
        REQUIRE(root["foo"][0].size() == 2);

        REQUIRE(root["foo"][0].contains("bar"));
        REQUIRE(root["foo"][0]["bar"].is_boolean());
        REQUIRE(root["foo"][0]["bar"].get_value<bool>() == true);

        REQUIRE(root["foo"][0].contains("baz"));
        REQUIRE(root["foo"][0]["baz"].is_integer());
        REQUIRE(root["foo"][0]["baz"].get_value<int>() == 123);

        REQUIRE(root.contains("qux"));
        REQUIRE(root["qux"].is_string());
        REQUIRE(root["qux"].get_value_ref<std::string&>() == "corge");
    }

    SECTION("Input source No.13.")
    {
        auto input_adapter = fkyaml::detail::input_adapter("stuff:\n"
                                                           "  - id: \"foo\"\n"
                                                           "    name: Foo\n"
                                                           "    tags:\n"
                                                           "      - baz\n"
                                                           "    params:\n"
                                                           "      key: 1\n"
                                                           "  - id: \"bar\"\n"
                                                           "    name: Bar");

        REQUIRE_NOTHROW(root = deserializer.deserialize(std::move(input_adapter)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);

        REQUIRE(root.contains("stuff"));
        REQUIRE(root["stuff"].is_sequence());
        REQUIRE(root["stuff"].size() == 2);

        REQUIRE(root["stuff"][0].is_mapping());
        REQUIRE(root["stuff"][0].size() == 4);

        REQUIRE(root["stuff"][0].contains("id"));
        REQUIRE(root["stuff"][0]["id"].is_string());
        REQUIRE(root["stuff"][0]["id"].get_value_ref<std::string&>() == "foo");

        REQUIRE(root["stuff"][0].contains("name"));
        REQUIRE(root["stuff"][0]["name"].is_string());
        REQUIRE(root["stuff"][0]["name"].get_value_ref<std::string&>() == "Foo");

        REQUIRE(root["stuff"][0].contains("tags"));
        REQUIRE(root["stuff"][0]["tags"].is_sequence());
        REQUIRE(root["stuff"][0]["tags"].size() == 1);

        REQUIRE(root["stuff"][0]["tags"][0].is_string());
        REQUIRE(root["stuff"][0]["tags"][0].get_value_ref<std::string&>() == "baz");

        REQUIRE(root["stuff"][0].contains("params"));
        REQUIRE(root["stuff"][0]["params"].is_mapping());
        REQUIRE(root["stuff"][0]["params"].size() == 1);

        REQUIRE(root["stuff"][0]["params"].contains("key"));
        REQUIRE(root["stuff"][0]["params"]["key"].is_integer());
        REQUIRE(root["stuff"][0]["params"]["key"].get_value<int>() == 1);

        REQUIRE(root["stuff"][1].is_mapping());
        REQUIRE(root["stuff"][1].size() == 2);

        REQUIRE(root["stuff"][1].contains("id"));
        REQUIRE(root["stuff"][1]["id"].is_string());
        REQUIRE(root["stuff"][1]["id"].get_value_ref<std::string&>() == "bar");

        REQUIRE(root["stuff"][1].contains("name"));
        REQUIRE(root["stuff"][1]["name"].is_string());
        REQUIRE(root["stuff"][1]["name"].get_value_ref<std::string&>() == "Bar");
    }

    SECTION("Input source No.14.")
    {
        auto input_adapter = fkyaml::detail::input_adapter("stuff:\n"
                                                           "  - id: \"foo\"\n"
                                                           "    name: Foo\n"
                                                           "    params:\n"
                                                           "      key: 1\n"
                                                           "    tags:\n"
                                                           "      - baz\n"
                                                           "  - id: \"bar\"\n"
                                                           "    name: Bar");

        REQUIRE_NOTHROW(root = deserializer.deserialize(std::move(input_adapter)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);

        REQUIRE(root.contains("stuff"));
        REQUIRE(root["stuff"].is_sequence());
        REQUIRE(root["stuff"].size() == 2);

        REQUIRE(root["stuff"][0].is_mapping());
        REQUIRE(root["stuff"][0].size() == 4);

        REQUIRE(root["stuff"][0].contains("id"));
        REQUIRE(root["stuff"][0]["id"].is_string());
        REQUIRE(root["stuff"][0]["id"].get_value_ref<std::string&>() == "foo");

        REQUIRE(root["stuff"][0].contains("name"));
        REQUIRE(root["stuff"][0]["name"].is_string());
        REQUIRE(root["stuff"][0]["name"].get_value_ref<std::string&>() == "Foo");

        REQUIRE(root["stuff"][0].contains("tags"));
        REQUIRE(root["stuff"][0]["tags"].is_sequence());
        REQUIRE(root["stuff"][0]["tags"].size() == 1);

        REQUIRE(root["stuff"][0]["tags"][0].is_string());
        REQUIRE(root["stuff"][0]["tags"][0].get_value_ref<std::string&>() == "baz");

        REQUIRE(root["stuff"][0].contains("params"));
        REQUIRE(root["stuff"][0]["params"].is_mapping());
        REQUIRE(root["stuff"][0]["params"].size() == 1);

        REQUIRE(root["stuff"][0]["params"].contains("key"));
        REQUIRE(root["stuff"][0]["params"]["key"].is_integer());
        REQUIRE(root["stuff"][0]["params"]["key"].get_value<int>() == 1);

        REQUIRE(root["stuff"][1].is_mapping());
        REQUIRE(root["stuff"][1].size() == 2);

        REQUIRE(root["stuff"][1].contains("id"));
        REQUIRE(root["stuff"][1]["id"].is_string());
        REQUIRE(root["stuff"][1]["id"].get_value_ref<std::string&>() == "bar");

        REQUIRE(root["stuff"][1].contains("name"));
        REQUIRE(root["stuff"][1]["name"].is_string());
        REQUIRE(root["stuff"][1]["name"].get_value_ref<std::string&>() == "Bar");
    }
}

TEST_CASE("DeserializerClassTest_DeserializeFlowSequenceTest", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("Input source No.1.")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test: [ foo, bar ]")));

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
        REQUIRE_NOTHROW(test_0_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(test_0_node.get_value_ref<fkyaml::node::string_type&>().compare("foo") == 0);

        REQUIRE_NOTHROW(test_node[1]);
        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_string());
        REQUIRE_NOTHROW(test_1_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(test_1_node.get_value_ref<fkyaml::node::string_type&>().compare("bar") == 0);
    }
}

TEST_CASE("DeserializerClassTest_DeserializeFlowMappingTest", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("Input source No.1.")
    {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("test: { bool: true, foo: bar, pi: 3.14 }")));

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
        REQUIRE_NOTHROW(test_bool_node.get_value<fkyaml::node::boolean_type>());
        REQUIRE(test_bool_node.get_value<fkyaml::node::boolean_type>() == true);

        REQUIRE_NOTHROW(test_node["foo"]);
        fkyaml::node& test_foo_node = test_node["foo"];
        REQUIRE(test_foo_node.is_string());
        REQUIRE_NOTHROW(test_foo_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(test_foo_node.get_value_ref<fkyaml::node::string_type&>().compare("bar") == 0);

        REQUIRE_NOTHROW(test_node["pi"]);
        fkyaml::node& test_pi_node = test_node["pi"];
        REQUIRE(test_pi_node.is_float_number());
        REQUIRE_NOTHROW(test_pi_node.get_value<fkyaml::node::float_number_type>());
        REQUIRE(test_pi_node.get_value<fkyaml::node::float_number_type>() == 3.14);
    }

    SECTION("Input source No.2. (invalid)")
    {
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter("test: }")), fkyaml::parse_error);
    }
}

TEST_CASE("DeserializerClassTest_DeserializeInputWithCommentTest", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    REQUIRE_NOTHROW(
        root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: one # comment\nbar: true\npi: 3.14")));

    REQUIRE(root.is_mapping());
    REQUIRE(root.size() == 3);

    REQUIRE_NOTHROW(root["foo"]);
    fkyaml::node& foo_node = root["foo"];
    REQUIRE(foo_node.is_string());
    REQUIRE_NOTHROW(foo_node.get_value_ref<fkyaml::node::string_type&>());
    REQUIRE(foo_node.get_value_ref<fkyaml::node::string_type&>().compare("one") == 0);

    REQUIRE_NOTHROW(root["bar"]);
    fkyaml::node& bar_node = root["bar"];
    REQUIRE(bar_node.is_boolean());
    REQUIRE_NOTHROW(bar_node.get_value<fkyaml::node::boolean_type>());
    REQUIRE(bar_node.get_value<fkyaml::node::boolean_type>() == true);

    REQUIRE_NOTHROW(root["pi"]);
    fkyaml::node& pi_node = root["pi"];
    REQUIRE(pi_node.is_float_number());
    REQUIRE_NOTHROW(pi_node.get_value<fkyaml::node::float_number_type>());
    REQUIRE(pi_node.get_value<fkyaml::node::float_number_type>() == 3.14);
}

TEST_CASE("DeserializerClassTest_DeserializeYAMLVerDirectiveTest", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("YAML 1.1")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("%YAML 1.1\nfoo: one")));

        REQUIRE(root.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_1);
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(root.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_1);
        REQUIRE(foo_node.is_string());
        REQUIRE_NOTHROW(foo_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(foo_node.get_value_ref<fkyaml::node::string_type&>().compare("one") == 0);
    }

    SECTION("YAML 1.2")
    {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("%YAML 1.2\nfoo: one")));

        REQUIRE(root.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_2);
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);

        REQUIRE_NOTHROW(root["foo"]);
        fkyaml::node& foo_node = root["foo"];
        REQUIRE(root.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_2);
        REQUIRE(foo_node.is_string());
        REQUIRE_NOTHROW(foo_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(foo_node.get_value_ref<fkyaml::node::string_type&>().compare("one") == 0);
    }
}

TEST_CASE("DeserializerClassTest_TagDirectiveTest", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    // TODO: add actual tests after tag directives get supported supported.
    REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("%TAG foo bar")));
}

TEST_CASE("DeserializerClassTest_DeserializeNoMachingAnchorTest", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter("foo: *anchor")), fkyaml::parse_error);
}

TEST_CASE("DeserializerClassTest_DeserializeDocumentWithMarkersTest", "[DeserializerClassTest]")
{
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("%YAML 1.2\n---\nfoo: one\n...")));
    REQUIRE(root.is_mapping());
    REQUIRE(root.size() == 1);
    REQUIRE(root.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_2);

    REQUIRE(root.contains("foo"));
    REQUIRE(root["foo"].get_value_ref<std::string&>() == "one");
}
