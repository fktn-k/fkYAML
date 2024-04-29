//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.5
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("Deserializer_EmptyInput") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(" ")));
    REQUIRE(root.is_mapping());
    REQUIRE(root.empty());
}

TEST_CASE("Deserializer_KeySeparator") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("normal key-value cases") {
        auto input_str = GENERATE(
            std::string("test: hoge"), std::string("test:\n  foo: bar"), std::string("test:\n  - foo\n  - bar"));
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input_str)));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
    }

    SECTION("error cases") {
        auto input_str = GENERATE(std::string("- : foo"), std::string("- - : foo"));
        REQUIRE_THROWS_AS(
            root = deserializer.deserialize(fkyaml::detail::input_adapter(input_str)), fkyaml::parse_error);
    }
}

TEST_CASE("Deserializer_ValueSeparator") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    auto input_str = GENERATE(std::string("test: [ foo, bar ]"), std::string("test: { foo: bar, buz: val }"));
    REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input_str)));
    REQUIRE(root.is_mapping());
    REQUIRE(root.size() == 1);
}

TEST_CASE("Deserializer_NullValue") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("key not in a sequence.") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("Null: test")));
        REQUIRE(root.contains(nullptr));
    }

    SECTION("key in a sequence.") {
        auto input = GENERATE(std::string("test:\n  - null: foo"), std::string("test:\n  - null:\n      - true"));
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root["test"][0].contains(nullptr));
    }

    SECTION("mapping value.") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test: null")));
        REQUIRE(root["test"].is_null());
    }

    SECTION("sequence value.") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - null")));
        REQUIRE(root["test"][0].is_null());
    }
}

TEST_CASE("Deserializer_BooleanValue") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("key not in a sequence.") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("true: test")));
        REQUIRE(root.contains(true));
    }

    SECTION("key in a sequence.") {
        auto input = GENERATE(std::string("test:\n  - false: foo"), std::string("test:\n  - false:\n      - null"));
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root["test"][0].contains(false));
    }

    SECTION("mapping value.") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test: TRUE")));
        REQUIRE(root["test"].get_value<bool>() == true);
    }

    SECTION("sequence value.") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - False")));
        REQUIRE(root["test"][0].get_value<bool>() == false);
    }
}

TEST_CASE("Deserializer_IntegerKey") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("key not in a sequence.") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("123: test")));
        REQUIRE(root.contains(123));
    }

    SECTION("key in a sequence.") {
        auto input = GENERATE(std::string("test:\n  - 123: foo"), std::string("test:\n  - 123:\n      - true"));
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root["test"][0].contains(123));
    }

    SECTION("mapping value.") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test: 123")));
        REQUIRE(root["test"].get_value<int>() == 123);
    }

    SECTION("sequence value.") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - 123")));
        REQUIRE(root["test"][0].get_value<int>() == 123);
    }
}

TEST_CASE("Deserializer_FloatingPointNumberKey") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("key not in a sequence.") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("3.14: test")));
        REQUIRE(root.contains(3.14));
    }

    SECTION("key in a sequence.") {
        auto input = GENERATE(std::string("test:\n  - .inf: foo"), std::string("test:\n  - .inf:\n      - true"));
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root["test"][0].contains(std::numeric_limits<fkyaml::node::float_number_type>::infinity()));
    }

    SECTION("mapping value.") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test: .nan")));
        REQUIRE(std::isnan(root["test"].get_value<double>()));
    }

    SECTION("sequence value.") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - 1.23e-5")));
        REQUIRE(root["test"][0].get_value<double>() == 1.23e-5);
    }
}

TEST_CASE("Deserializer_InvalidIndentation") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    REQUIRE_THROWS_AS(
        root = deserializer.deserialize(fkyaml::detail::input_adapter("foo:\n  bar: baz\n qux: true")),
        fkyaml::parse_error);
}

TEST_CASE("Deserializer_DuplicateKeys") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    REQUIRE_THROWS_AS(
        root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: bar\nfoo: baz")), fkyaml::parse_error);
}

TEST_CASE("Deserializer_BlockSequence") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("simple block sequence.") {
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

    SECTION("block sequence with nested mappings") {
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

    SECTION("block mapping with a comment in between") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  # comment\n  - item: 123")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("test"));

        REQUIRE(root["test"].is_sequence());
        REQUIRE(root["test"].size() == 1);

        REQUIRE(root["test"][0].is_mapping());
        REQUIRE(root["test"][0].size() == 1);
        REQUIRE(root["test"][0].contains("item"));

        REQUIRE(root["test"][0]["item"].is_integer());
        REQUIRE(root["test"][0]["item"].get_value<int>() == 123);
    }

    SECTION("block mapping with a comment next to the key") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: # comment\n  - bar\n")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));

        REQUIRE(root["foo"].is_sequence());
        REQUIRE(root["foo"].size() == 1);

        REQUIRE(root["foo"][0].is_string());
        REQUIRE(root["foo"][0].get_value_ref<std::string&>() == "bar");
    }

    SECTION("root sequence") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("- foo\n- 123\n- 3.14")));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 3);

        REQUIRE(root[0].is_string());
        REQUIRE(root[0].get_value_ref<std::string&>() == "foo");

        REQUIRE(root[1].is_integer());
        REQUIRE(root[1].get_value<int>() == 123);

        REQUIRE(root[2].is_float_number());
        REQUIRE(root[2].get_value<double>() == 3.14);
    }

    SECTION("root sequence with nested child block sequence") {
        std::string input = "- - foo\n"
                            "  - 123\n"
                            "- 3.14";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 2);

        REQUIRE(root[0].is_sequence());
        REQUIRE(root[0].size() == 2);
        REQUIRE(root[0][0].is_string());
        REQUIRE(root[0][0].get_value_ref<std::string&>() == "foo");
        REQUIRE(root[0][1].is_integer());
        REQUIRE(root[0][1].get_value<int>() == 123);

        REQUIRE(root[1].is_float_number());
        REQUIRE(root[1].get_value<double>() == 3.14);
    }
}

TEST_CASE("Deserializer_BlockMapping") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("simple block mapping") {
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

    SECTION("nested block mapping") {
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

    SECTION("block mapping with several nested children") {
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

    SECTION("block mapping with a more nested child") {
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

    SECTION("block mapping with a child block sequence") {
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

    SECTION("block mapping with a block sequence of a single nested mapping") {
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

    SECTION("block mapping with a block sequence of a block mapping with several key-value pairs") {
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

    SECTION("block mapping with a block sequence of block mappings") {
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

    SECTION("block mapping with a block sequence of more nested block mappings") {
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

    SECTION("block mapping with explicit block mappings") {
        auto input_adapter = fkyaml::detail::input_adapter("null: 3.14\n"
                                                           "foo:\n"
                                                           "  ? bar\n"
                                                           "  : baz\n"
                                                           "? - 123\n"
                                                           "  - foo:\n"
                                                           "      ? bar\n"
                                                           "      : baz\n"
                                                           ": one: true\n"
                                                           "? ? foo\n"
                                                           "  : bar\n"
                                                           ": - baz\n"
                                                           "  - qux\n");

        REQUIRE_NOTHROW(root = deserializer.deserialize(std::move(input_adapter)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 4);

        REQUIRE(root.contains(nullptr));
        REQUIRE(root[nullptr].is_float_number());
        REQUIRE(root[nullptr].get_value<double>() == 3.14);

        REQUIRE(root.contains("foo"));
        REQUIRE(root["foo"].is_mapping());
        REQUIRE(root["foo"].size() == 1);

        REQUIRE(root["foo"].contains("bar"));
        REQUIRE(root["foo"]["bar"].is_string());
        REQUIRE(root["foo"]["bar"].get_value_ref<std::string&>() == "baz");

        fkyaml::node key = {123, {{"foo", {{"bar", "baz"}}}}};
        REQUIRE(root.contains(key));
        REQUIRE(root[key].is_mapping());
        REQUIRE(root[key].size() == 1);

        REQUIRE(root[key].contains("one"));
        REQUIRE(root[key]["one"].is_boolean());
        REQUIRE(root[key]["one"].get_value<bool>() == true);

        key = {{"foo", "bar"}};
        REQUIRE(root.contains(key));
        REQUIRE(root[key].is_sequence());
        REQUIRE(root[key].size() == 2);
        REQUIRE(root[key][0].is_string());
        REQUIRE(root[key][0].get_value_ref<std::string&>() == "baz");
        REQUIRE(root[key][1].is_string());
        REQUIRE(root[key][1].get_value_ref<std::string&>() == "qux");
    }

    SECTION("block mapping with keys containing flow indicators") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("Foo,Bar: true\nBaz[123]: 3.14")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE(root.contains("Foo,Bar"));
        REQUIRE(root["Foo,Bar"].get_value<bool>() == true);

        REQUIRE(root.contains("Baz[123]"));
        REQUIRE(root["Baz[123]"].get_value<double>() == 3.14);
    }

    SECTION("Flow indicators inside unquoted plain scalar values") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("Foo: Bar, abc{abc")));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("Foo"));
        REQUIRE(root["Foo"].is_string());
        REQUIRE(root["Foo"].get_value_ref<std::string&>() == "Bar, abc{abc");

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("Foo: Bar, abc}abc")));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("Foo"));
        REQUIRE(root["Foo"].is_string());
        REQUIRE(root["Foo"].get_value_ref<std::string&>() == "Bar, abc}abc");

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("Foo: Bar, abc[abc")));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("Foo"));
        REQUIRE(root["Foo"].is_string());
        REQUIRE(root["Foo"].get_value_ref<std::string&>() == "Bar, abc[abc");

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("Foo: Bar, abc]abc")));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("Foo"));
        REQUIRE(root["Foo"].is_string());
        REQUIRE(root["Foo"].get_value_ref<std::string&>() == "Bar, abc]abc");

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("Foo: Bar, {[123] :3.14}")));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("Foo"));
        REQUIRE(root["Foo"].is_string());
        REQUIRE(root["Foo"].get_value_ref<std::string&>() == "Bar, {[123] :3.14}");
        REQUIRE_THROWS_AS(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("Foo: Bar, {[123] : 3.14}")),
            fkyaml::parse_error);
    }

    SECTION("a comment right after a block mapping key.") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("baz: # comment2\n  qux: 123\n")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("baz"));

        REQUIRE(root["baz"].is_mapping());
        REQUIRE(root["baz"].size() == 1);
        REQUIRE(root["baz"].contains("qux"));

        REQUIRE(root["baz"]["qux"].is_integer());
        REQUIRE(root["baz"]["qux"].get_value<int>() == 123);
    }

    SECTION("mapping entries split across newlines") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo:\n"
                                                                          "  bar\n"
                                                                          "baz:\n"
                                                                          "  123\n"
                                                                          "null:\n"
                                                                          "  {false: 3.14}\n"
                                                                          "qux:\n"
                                                                          "  [r, g, b]")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 4);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains("baz"));
        REQUIRE(root.contains(nullptr));
        REQUIRE(root.contains("qux"));

        REQUIRE(root["foo"].is_string());
        REQUIRE(root["foo"].get_value_ref<std::string&>() == "bar");

        REQUIRE(root["baz"].is_integer());
        REQUIRE(root["baz"].get_value<int>() == 123);

        REQUIRE(root[nullptr].is_mapping());
        REQUIRE(root[nullptr].contains(false));
        REQUIRE(root[nullptr][false].is_float_number());
        REQUIRE(root[nullptr][false].get_value<double>() == 3.14);

        REQUIRE(root["qux"].is_sequence());
        REQUIRE(root["qux"].size() == 3);

        REQUIRE(root["qux"][0].is_string());
        REQUIRE(root["qux"][0].get_value_ref<std::string&>() == "r");
        REQUIRE(root["qux"][1].is_string());
        REQUIRE(root["qux"][1].get_value_ref<std::string&>() == "g");
        REQUIRE(root["qux"][2].is_string());
        REQUIRE(root["qux"][2].get_value_ref<std::string&>() == "b");
    }
}

TEST_CASE("Deserializer_FlowSequence") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("simple flow sequence") {
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

    SECTION("lack the beginning of a flow sequence") {
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter("test: ]")), fkyaml::parse_error);
    }

    SECTION("root flow sequence") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("[foo,123,3.14]")));
        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 3);

        REQUIRE(root[0].is_string());
        REQUIRE(root[0].get_value_ref<std::string&>() == "foo");

        REQUIRE(root[1].is_integer());
        REQUIRE(root[1].get_value<int>() == 123);

        REQUIRE(root[2].is_float_number());
        REQUIRE(root[2].get_value<double>() == 3.14);
    }
}

TEST_CASE("Deserializer_FlowMapping") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("simple flow mapping") {
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

    SECTION("Correct traversal after deserializing flow mapping value") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("test: { foo: bar }\n"
                                                                          "sibling: a_string_val")));
        REQUIRE(root.is_mapping());
        REQUIRE_NOTHROW(root.size());
        REQUIRE(root.size() == 2);

        REQUIRE_NOTHROW(root["test"]);
        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_mapping());
        REQUIRE_NOTHROW(test_node.size());
        REQUIRE(test_node.size() == 1);

        REQUIRE_NOTHROW(test_node["foo"]);
        fkyaml::node& test_foo_node = test_node["foo"];
        REQUIRE(test_foo_node.is_string());
        REQUIRE_NOTHROW(test_foo_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(test_foo_node.get_value_ref<fkyaml::node::string_type&>().compare("bar") == 0);

        REQUIRE_NOTHROW(root["sibling"]);
        fkyaml::node& sibling_node = root["sibling"];
        REQUIRE(sibling_node.is_string());
        REQUIRE_NOTHROW(sibling_node.get_value_ref<fkyaml::node::string_type&>());
        REQUIRE(sibling_node.get_value_ref<fkyaml::node::string_type&>().compare("a_string_val") == 0);
    }

    SECTION("lack the beginning of a flow mapping") {
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter("test: }")), fkyaml::parse_error);
    }

    SECTION("flow mapping with a flow sequence") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test: {foo: [true,123]}")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("test"));

        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_mapping());
        REQUIRE(test_node.size() == 1);
        REQUIRE(test_node.contains("foo"));

        fkyaml::node& test_foo_node = test_node["foo"];
        REQUIRE(test_foo_node.is_sequence());
        REQUIRE(test_foo_node.size() == 2);

        REQUIRE(test_foo_node[0].is_boolean());
        REQUIRE(test_foo_node[0].get_value<bool>() == true);

        REQUIRE(test_foo_node[1].is_integer());
        REQUIRE(test_foo_node[1].get_value<int>() == 123);
    }

    SECTION("root flow mapping") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("{foo: 123, true: 3.14}")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains(true));

        REQUIRE(root["foo"].is_integer());
        REQUIRE(root["foo"].get_value<int>() == 123);

        REQUIRE(root[true].is_float_number());
        REQUIRE(root[true].get_value<double>() == 3.14);
    }
}

TEST_CASE("Deserializer_InputWithComment") {
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

TEST_CASE("Deserializer_YAMLVerDirective") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("YAML 1.1") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("%YAML 1.1\n---\nfoo: one")));

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

    SECTION("YAML 1.2") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("%YAML 1.2\n---\nfoo: one")));

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

    SECTION("YAML directive in the content to be ignored") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: bar\n%YAML 1.1\ntrue: 123")));

        REQUIRE(root.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_2);
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE(root.contains("foo"));
        REQUIRE(root["foo"].is_string());
        REQUIRE(root["foo"].get_value_ref<std::string&>() == "bar");

        REQUIRE(root.contains(true));
        REQUIRE(root[true].is_integer());
        REQUIRE(root[true].get_value<int>() == 123);
    }

    SECTION("YAML directive more than once") {
        REQUIRE_THROWS_AS(
            deserializer.deserialize(fkyaml::detail::input_adapter("%YAML 1.1\n%YAML 1.2\n")), fkyaml::parse_error);
    }
}

TEST_CASE("Deserializer_TagDirective") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("primary tag handle") {
        std::string input = "%TAG ! tag:test.com,2000:\n"
                            "---\n"
                            "foo: !local bar";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);

        REQUIRE(root.contains("foo"));
        REQUIRE(root["foo"].is_string());
        REQUIRE(root["foo"].get_value_ref<std::string&>() == "bar");
        REQUIRE(root["foo"].has_tag_name());
        REQUIRE(root["foo"].get_tag_name() == "!local");
    }

    SECTION("primary tag handle more than once") {
        std::string input = "%TAG ! tag:test.com,2000:\n"
                            "%TAG ! tag:test.com,2000:\n"
                            "---\n"
                            "foo: bar";
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("secondary tag handle") {
        std::string input = "%TAG !! tag:test.com,2000:\n"
                            "---\n"
                            "foo: !!local bar";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);

        REQUIRE(root.contains("foo"));
        REQUIRE(root["foo"].is_string());
        REQUIRE(root["foo"].get_value_ref<std::string&>() == "bar");
        REQUIRE(root["foo"].has_tag_name());
        REQUIRE(root["foo"].get_tag_name() == "!!local");
    }

    SECTION("secondary tag handle more than once") {
        std::string input = "%TAG !! tag:test.com,2000:\n"
                            "%TAG !! tag:test.com,2000:\n"
                            "---\n"
                            "foo: bar";
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("named tag handles") {
        std::string input = "%TAG !e! tag:test.com,2000:\n"
                            "%TAG !f! !foo-\n"
                            "---\n"
                            "foo: !e!global bar\n"
                            "baz: !f!local qux";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);

        REQUIRE(root.contains("foo"));
        REQUIRE(root["foo"].is_string());
        REQUIRE(root["foo"].get_value_ref<std::string&>() == "bar");
        REQUIRE(root["foo"].has_tag_name());
        REQUIRE(root["foo"].get_tag_name() == "!e!global");

        REQUIRE(root.contains("baz"));
        REQUIRE(root["baz"].is_string());
        REQUIRE(root["baz"].get_value_ref<std::string&>() == "qux");
        REQUIRE(root["baz"].has_tag_name());
        REQUIRE(root["baz"].get_tag_name() == "!f!local");
    }

    SECTION("named tag handle more than once") {
        std::string input = "%TAG !e! tag:test.com,2000:\n"
                            "%TAG !e! !foo-\n"
                            "---\n"
                            "foo: bar";
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }
}

TEST_CASE("Deserializer_InvalidDirective") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("%INVALID foo bar")));
    REQUIRE(root.is_mapping());
    REQUIRE(root.empty());
}

TEST_CASE("Deserializer_Anchor") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("block sequence with anchored boolean scalar") {
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

    SECTION("block sequence with anchored integer scalar") {
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

    SECTION("block sequence with anchored floating point number scalar") {
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

    SECTION("block sequence with anchored string scalar") {
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

    SECTION("block mapping with anchored boolean scalar") {
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

    SECTION("block mapping with anchored integer scalar") {
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

    SECTION("block mapping with anchored floating point number scalar") {
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

    SECTION("block mapping with anchored string scalar") {
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

    SECTION("parse alias mapping key") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("&anchor foo:\n  *anchor: 123")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));
        REQUIRE(root["foo"].is_mapping());
        REQUIRE(root["foo"].size() == 1);
        REQUIRE(root["foo"].contains("foo"));
        REQUIRE(root["foo"]["foo"].is_integer());
        REQUIRE(root["foo"]["foo"].get_value<int>() == 123);
    }

    SECTION("multiple anchors specified") {
        auto input =
            GENERATE(std::string("foo: &anchor &anchor2\n  bar: baz"), std::string("&anchor &anchor2 foo: bar"));
        REQUIRE_THROWS_AS(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }
}

TEST_CASE("Deserializer_Tag") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("valid tags") {
        std::string input = "str: !!str true\n"
                            "int: !<tag:yaml.org,2002:int> 123\n"
                            "nil: !!null null\n"
                            "bool: !!bool false\n"
                            "float: !!float 3.14\n"
                            "non specific: ! non specific\n"
                            "custom: !local value\n"
                            "map: !!map\n"
                            "  !!str foo: bar\n"
                            "map_flow: !<tag:yaml.org,2002:map> {foo: bar}\n"
                            "seq: !<tag:yaml.org,2002:seq>\n"
                            "  - 123\n"
                            "  - 3.14\n"
                            "seq_flow: !!seq [123, 3.14]";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 11);

        REQUIRE(root.contains("str"));
        REQUIRE(root["str"].has_tag_name());
        REQUIRE(root["str"].get_tag_name() == "!!str");
        REQUIRE(root["str"].is_string());
        REQUIRE(root["str"].get_value_ref<std::string&>() == "true");

        REQUIRE(root.contains("int"));
        REQUIRE(root["int"].has_tag_name());
        REQUIRE(root["int"].get_tag_name() == "!<tag:yaml.org,2002:int>");
        REQUIRE(root["int"].is_integer());
        REQUIRE(root["int"].get_value<int>() == 123);

        REQUIRE(root.contains("nil"));
        REQUIRE(root["nil"].has_tag_name());
        REQUIRE(root["nil"].get_tag_name() == "!!null");
        REQUIRE(root["nil"].is_null());
        REQUIRE(root["nil"].get_value<std::nullptr_t>() == nullptr);

        REQUIRE(root.contains("bool"));
        REQUIRE(root["bool"].has_tag_name());
        REQUIRE(root["bool"].get_tag_name() == "!!bool");
        REQUIRE(root["bool"].is_boolean());
        REQUIRE(root["bool"].get_value<bool>() == false);

        REQUIRE(root.contains("float"));
        REQUIRE(root["float"].has_tag_name());
        REQUIRE(root["float"].get_tag_name() == "!!float");
        REQUIRE(root["float"].is_float_number());
        REQUIRE(root["float"].get_value<double>() == 3.14);

        REQUIRE(root.contains("non specific"));
        REQUIRE(root["non specific"].has_tag_name());
        REQUIRE(root["non specific"].get_tag_name() == "!");
        REQUIRE(root["non specific"].is_string());
        REQUIRE(root["non specific"].get_value_ref<std::string&>() == "non specific");

        REQUIRE(root.contains("custom"));
        REQUIRE(root["custom"].has_tag_name());
        REQUIRE(root["custom"].get_tag_name() == "!local");
        REQUIRE(root["custom"].is_string());
        REQUIRE(root["custom"].get_value_ref<std::string&>() == "value");

        REQUIRE(root.contains("map"));
        REQUIRE(root["map"].has_tag_name());
        REQUIRE(root["map"].get_tag_name() == "!!map");
        REQUIRE(root["map"].is_mapping());
        REQUIRE(root["map"].size() == 1);
        REQUIRE(root["map"].begin().key().has_tag_name());
        REQUIRE(root["map"].begin().key().get_tag_name() == "!!str");
        REQUIRE(root["map"].contains("foo"));
        REQUIRE(root["map"]["foo"].get_value_ref<std::string&>() == "bar");

        REQUIRE(root.contains("map_flow"));
        REQUIRE(root["map_flow"].has_tag_name());
        REQUIRE(root["map_flow"].get_tag_name() == "!<tag:yaml.org,2002:map>");
        REQUIRE(root["map_flow"].is_mapping());
        REQUIRE(root["map_flow"].size() == 1);
        REQUIRE(root["map_flow"].contains("foo"));
        REQUIRE(root["map_flow"]["foo"].get_value_ref<std::string&>() == "bar");

        REQUIRE(root.contains("seq"));
        REQUIRE(root["seq"].has_tag_name());
        REQUIRE(root["seq"].get_tag_name() == "!<tag:yaml.org,2002:seq>");
        REQUIRE(root["seq"].is_sequence());
        REQUIRE(root["seq"].size() == 2);
        REQUIRE(root["seq"][0].get_value<int>() == 123);
        REQUIRE(root["seq"][1].get_value<float>() == 3.14f);

        REQUIRE(root.contains("seq_flow"));
        REQUIRE(root["seq_flow"].has_tag_name());
        REQUIRE(root["seq_flow"].get_tag_name() == "!!seq");
        REQUIRE(root["seq_flow"].is_sequence());
        REQUIRE(root["seq_flow"].size() == 2);
        REQUIRE(root["seq_flow"][0].get_value<int>() == 123);
        REQUIRE(root["seq_flow"][1].get_value<float>() == 3.14f);
    }

    SECTION("specify tags using TAG directives") {
        std::string input = "%TAG !e! tag:example.com,2000:app/\n"
                            "---\n"
                            "- !e!foo \"bar\"";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 1);

        REQUIRE(root[0].has_tag_name());
        REQUIRE(root[0].get_tag_name() == "!e!foo");
        REQUIRE(root[0].is_string());
        REQUIRE(root[0].get_value_ref<std::string&>() == "bar");
    }

    SECTION("multiple tags specified") {
        auto input = GENERATE(std::string("foo: !!map !!map\n  bar: baz"), std::string("!!str !!bool true: 123"));
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }
}

TEST_CASE("Deserializer_NodeProperties") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("both tag and anchor specified") {
        auto input = GENERATE(
            std::string("foo: !!map &anchor\n  bar: baz"), // tag -> anchor
            std::string("foo: &anchor !!map\n  bar: baz")  // anchor -> tag
        );
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);

        REQUIRE(root.contains("foo"));
        REQUIRE(root["foo"].is_mapping());
        REQUIRE(root["foo"].has_anchor_name());
        REQUIRE(root["foo"].get_anchor_name() == "anchor");
        REQUIRE(root["foo"].has_tag_name());
        REQUIRE(root["foo"].get_tag_name() == "!!map");
        REQUIRE(root["foo"].size() == 1);

        REQUIRE(root["foo"].contains("bar"));
        REQUIRE(root["foo"]["bar"].is_string());
        REQUIRE(root["foo"]["bar"].get_value_ref<std::string&>() == "baz");
    }

    SECTION("alias node with tag") {
        REQUIRE_THROWS_AS(
            deserializer.deserialize(fkyaml::detail::input_adapter("&anchor foo: !!str *anchor")), fkyaml::parse_error);
    }
}

TEST_CASE("Deserializer_NoMachingAnchor") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter("foo: *anchor")), fkyaml::parse_error);
}

TEST_CASE("Deserializer_DocumentWithMarkers") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("%YAML 1.2\n---\nfoo: one\n...")));
    REQUIRE(root.is_mapping());
    REQUIRE(root.size() == 1);
    REQUIRE(root.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_2);

    REQUIRE(root.contains("foo"));
    REQUIRE(root["foo"].get_value_ref<std::string&>() == "one");
}
