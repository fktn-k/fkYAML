//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("Deserializer_EmptyInput") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("")));
    REQUIRE(root.is_null());

    REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(" ")));
    REQUIRE(root.is_null());
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

    SECTION("root scalar") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("true")));
        REQUIRE(root.is_boolean());
        REQUIRE(root.get_value<bool>() == true);
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

    SECTION("root scalar") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("123")));
        REQUIRE(root.is_integer());
        REQUIRE(root.get_value<int>() == 123);
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

    SECTION("root scalar") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("3.14")));
        REQUIRE(root.is_float_number());
        REQUIRE(root.get_value<double>() == 3.14);
    }
}

TEST_CASE("Deserializer_BlockLiteralScalar") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("not tagged") {
        std::string input = "foo: |\n"
                            "  first sentence.\n"
                            "  second sentence.\n"
                            "  last sentence.\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "first sentence.\nsecond sentence.\nlast sentence.\n");
    }

    SECTION("tagged") {
        std::string input = "foo: !!str |\n"
                            "  first sentence.\n"
                            "  second sentence.\n"
                            "  last sentence.\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.has_tag_name());
        REQUIRE(foo_node.get_tag_name() == "!!str");
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "first sentence.\nsecond sentence.\nlast sentence.\n");
    }

    SECTION("mapping key") {
        std::string input = "? |\n"
                            "  first sentence.\n"
                            "  second sentence.\n"
                            "  last sentence.\n"
                            ": map value";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("first sentence.\nsecond sentence.\nlast sentence.\n"));

        fkyaml::node& val_node = root["first sentence.\nsecond sentence.\nlast sentence.\n"];
        REQUIRE(val_node.is_string());
        REQUIRE(val_node.as_str() == "map value");
    }

    SECTION("root scalar") {
        std::string input = "--- |\n"
                            "  first sentence.\n"
                            "  second sentence.\n"
                            "  last sentence.\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root.is_string());
        REQUIRE(root.as_str() == "first sentence.\nsecond sentence.\nlast sentence.\n");
    }
}

TEST_CASE("Deserializer_BlockFoldedScalar") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("not tagged") {
        std::string input = "foo: >\n"
                            "  first sentence.\n"
                            "  second sentence.\n"
                            "  last sentence.\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "first sentence. second sentence. last sentence.\n");
    }

    SECTION("tagged") {
        std::string input = "foo: !!str >\n"
                            "  first sentence.\n"
                            "  second sentence.\n"
                            "  last sentence.\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.has_tag_name());
        REQUIRE(foo_node.get_tag_name() == "!!str");
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "first sentence. second sentence. last sentence.\n");
    }

    SECTION("mapping key") {
        std::string input = "? >\n"
                            "  first sentence.\n"
                            "  second sentence.\n"
                            "  last sentence.\n"
                            ": map value";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("first sentence. second sentence. last sentence.\n"));

        fkyaml::node& val_node = root["first sentence. second sentence. last sentence.\n"];
        REQUIRE(val_node.is_string());
        REQUIRE(val_node.as_str() == "map value");
    }

    SECTION("root scalar") {
        std::string input = "--- >\n"
                            "  first sentence.\n"
                            "  second sentence.\n"
                            "  last sentence.\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root.is_string());
        REQUIRE(root.as_str() == "first sentence. second sentence. last sentence.\n");
    }
}

TEST_CASE("Deserializer_ScalarConversionErrorHandling") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    auto input = GENERATE(
        std::string("!!null foo: bar"),
        std::string("!!bool foo: bar"),
        std::string("!!int foo: bar"),
        std::string("!!float foo: bar"));
    REQUIRE_THROWS_AS(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
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
        REQUIRE(root.contains("test"));

        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE_NOTHROW(test_node.size());
        REQUIRE(test_node.size() == 2);

        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.is_string());
        REQUIRE(test_0_node.as_str() == "foo");

        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_string());
        REQUIRE(test_1_node.as_str() == "bar");
    }

    SECTION("child block sequence whose prefixes are put as indentation") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n- foo\n- 123")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("test"));

        fkyaml::node& root_test_node = root["test"];
        REQUIRE(root_test_node.is_sequence());
        REQUIRE(root_test_node.size() == 2);

        fkyaml::node& root_test_0_node = root_test_node[0];
        REQUIRE(root_test_0_node.is_string());
        REQUIRE(root_test_0_node.as_str() == "foo");

        fkyaml::node& root_test_1_node = root_test_node[1];
        REQUIRE(root_test_1_node.is_integer());
        REQUIRE(root_test_1_node.get_value<int>() == 123);
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
        REQUIRE(test_node.size() == 2);

        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.is_mapping());
        REQUIRE(test_0_node.size() == 2);
        REQUIRE(test_0_node.contains("foo"));
        REQUIRE(test_0_node.contains("bar"));

        fkyaml::node& test_0_foo_node = test_0_node["foo"];
        REQUIRE(test_0_foo_node.is_boolean());
        REQUIRE(test_0_foo_node.get_value<bool>() == true);

        fkyaml::node& test_0_bar_node = test_0_node["bar"];
        REQUIRE(test_0_bar_node.is_string());
        REQUIRE(test_0_bar_node.as_str() == "one");

        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_mapping());
        REQUIRE_NOTHROW(test_1_node.size());
        REQUIRE(test_1_node.size() == 2);
        REQUIRE(test_1_node.contains("foo"));
        REQUIRE(test_1_node.contains("bar"));

        fkyaml::node& test_1_foo_node = test_1_node["foo"];
        REQUIRE(test_1_foo_node.is_boolean());
        REQUIRE(test_1_foo_node.get_value<bool>() == false);

        fkyaml::node& test_1_bar_node = test_1_node["bar"];
        REQUIRE(test_1_bar_node.is_string());
        REQUIRE(test_1_bar_node.as_str() == "two");
    }

    SECTION("block mapping with a comment in between") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  # comment\n  - item: 123")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("test"));

        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE(test_node.size() == 1);

        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.is_mapping());
        REQUIRE(test_0_node.size() == 1);
        REQUIRE(test_0_node.contains("item"));

        fkyaml::node& test_0_item_node = test_0_node["item"];
        REQUIRE(test_0_item_node.is_integer());
        REQUIRE(test_0_item_node.get_value<int>() == 123);
    }

    SECTION("block mapping with a comment next to the key") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: # comment\n  - bar\n")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_sequence());
        REQUIRE(foo_node.size() == 1);

        fkyaml::node& foo_0_node = foo_node[0];
        REQUIRE(foo_0_node.is_string());
        REQUIRE(foo_0_node.as_str() == "bar");
    }

    SECTION("root sequence") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("- foo\n- 123\n- 3.14")));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 3);

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.is_string());
        REQUIRE(root_0_node.as_str() == "foo");

        fkyaml::node& root_1_node = root[1];
        REQUIRE(root_1_node.is_integer());
        REQUIRE(root_1_node.get_value<int>() == 123);

        fkyaml::node& root_2_node = root[2];
        REQUIRE(root_2_node.is_float_number());
        REQUIRE(root_2_node.get_value<double>() == 3.14);
    }

    SECTION("root sequence with nested child block sequence") {
        std::string input = "- - foo\n"
                            "  - 123\n"
                            "- 3.14\n"
                            "- - True\n"
                            "  - null";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 3);

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.is_sequence());
        REQUIRE(root_0_node.size() == 2);

        fkyaml::node& root_0_0_node = root_0_node[0];
        REQUIRE(root_0_0_node.is_string());
        REQUIRE(root_0_0_node.as_str() == "foo");

        fkyaml::node& root_0_1_node = root_0_node[1];
        REQUIRE(root_0_1_node.is_integer());
        REQUIRE(root_0_1_node.get_value<int>() == 123);

        fkyaml::node& root_1_node = root[1];
        REQUIRE(root_1_node.is_float_number());
        REQUIRE(root_1_node.get_value<double>() == 3.14);

        fkyaml::node& root_2_node = root[2];
        REQUIRE(root_2_node.is_sequence());
        REQUIRE(root_2_node.size() == 2);

        fkyaml::node& root_2_0_node = root_2_node[0];
        REQUIRE(root_2_0_node.is_boolean());
        REQUIRE(root_2_0_node.get_value<bool>() == true);

        fkyaml::node& root_2_1_node = root_2_node[1];
        REQUIRE(root_2_1_node.is_null());
    }

    SECTION("root sequence with child flow sequence") {
        std::string input = "- [username, identifier, score]\n"
                            "- [booker12, 9012      , 61.25]\n"
                            "- [grey07  , 2070      , 84.50]";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 3);

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.is_sequence());
        REQUIRE(root_0_node.size() == 3);

        fkyaml::node& root_0_0_node = root_0_node[0];
        REQUIRE(root_0_0_node.is_string());
        REQUIRE(root_0_0_node.as_str() == "username");

        fkyaml::node& root_0_1_node = root_0_node[1];
        REQUIRE(root_0_1_node.is_string());
        REQUIRE(root_0_1_node.as_str() == "identifier");

        fkyaml::node& root_0_2_node = root_0_node[2];
        REQUIRE(root_0_2_node.is_string());
        REQUIRE(root_0_2_node.as_str() == "score");

        fkyaml::node& root_1_node = root[1];
        REQUIRE(root_1_node.is_sequence());
        REQUIRE(root_1_node.size() == 3);

        fkyaml::node& root_1_0_node = root_1_node[0];
        REQUIRE(root_1_0_node.is_string());
        REQUIRE(root_1_0_node.as_str() == "booker12");

        fkyaml::node& root_1_1_node = root_1_node[1];
        REQUIRE(root_1_1_node.is_integer());
        REQUIRE(root_1_1_node.get_value<int>() == 9012);

        fkyaml::node& root_1_2_node = root_1_node[2];
        REQUIRE(root_1_2_node.is_float_number());
        REQUIRE(root_1_2_node.get_value<double>() == 61.25);

        fkyaml::node& root_2_node = root[2];
        REQUIRE(root_2_node.is_sequence());
        REQUIRE(root_2_node.size() == 3);

        fkyaml::node& root_2_0_node = root_2_node[0];
        REQUIRE(root_2_0_node.is_string());
        REQUIRE(root_2_0_node.as_str() == "grey07");

        fkyaml::node& root_2_1_node = root_2_node[1];
        REQUIRE(root_2_1_node.is_integer());
        REQUIRE(root_2_1_node.get_value<int>() == 2070);

        fkyaml::node& root_2_2_node = root_2_node[2];
        REQUIRE(root_2_2_node.is_float_number());
        REQUIRE(root_2_2_node.get_value<double>() == 84.50);
    }

    SECTION("root sequence with child flow mapping") {
        std::string input = "- {foo: false, null: 123}\n"
                            "- {true: 3.14, bar: [0x30,0o30]}";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 2);

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.is_mapping());
        REQUIRE(root_0_node.size() == 2);
        REQUIRE(root_0_node.contains("foo"));
        REQUIRE(root_0_node.contains(nullptr));

        fkyaml::node& root_0_foo_node = root_0_node["foo"];
        REQUIRE(root_0_foo_node.is_boolean());
        REQUIRE(root_0_foo_node.get_value<bool>() == false);

        fkyaml::node& root_0_null_node = root_0_node[nullptr];
        REQUIRE(root_0_null_node.is_integer());
        REQUIRE(root_0_null_node.get_value<int>() == 123);

        fkyaml::node& root_1_node = root[1];
        REQUIRE(root_1_node.is_mapping());
        REQUIRE(root_1_node.size() == 2);
        REQUIRE(root_1_node.contains(true));
        REQUIRE(root_1_node.contains("bar"));

        fkyaml::node& root_1_true_node = root_1_node[true];
        REQUIRE(root_1_true_node.is_float_number());
        REQUIRE(root_1_true_node.get_value<double>() == 3.14);

        fkyaml::node& root_1_bar_node = root_1_node["bar"];
        REQUIRE(root_1_bar_node.is_sequence());
        REQUIRE(root_1_bar_node.size() == 2);

        fkyaml::node& root_1_bar_0_node = root_1_bar_node[0];
        REQUIRE(root_1_bar_0_node.is_integer());
        REQUIRE(root_1_bar_0_node.get_value<int>() == 0x30);

        fkyaml::node& root_1_bar_1_node = root_1_bar_node[1];
        REQUIRE(root_1_bar_1_node.is_integer());
        REQUIRE(root_1_bar_1_node.get_value<int>() == 030);
    }

    SECTION("block mapping with child block mapping (split by a newline code)") {
        std::string input = "-\n"
                            "  name: Mark McGwire\n"
                            "  hr:   65\n"
                            "  avg:  0.278\n"
                            "-\n"
                            "  name: Sammy Sosa\n"
                            "  hr:   63\n"
                            "  avg:  0.288";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 2);

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.is_mapping());
        REQUIRE(root_0_node.size() == 3);
        REQUIRE(root_0_node.contains("name"));
        REQUIRE(root_0_node.contains("hr"));
        REQUIRE(root_0_node.contains("avg"));

        fkyaml::node& root_0_name_node = root_0_node["name"];
        REQUIRE(root_0_name_node.is_string());
        REQUIRE(root_0_name_node.as_str() == "Mark McGwire");

        fkyaml::node& root_0_hr_node = root_0_node["hr"];
        REQUIRE(root_0_hr_node.is_integer());
        REQUIRE(root_0_hr_node.get_value<int>() == 65);

        fkyaml::node& root_0_avg_node = root_0_node["avg"];
        REQUIRE(root_0_avg_node.is_float_number());
        REQUIRE(root_0_avg_node.get_value<double>() == 0.278);

        fkyaml::node& root_1_node = root[1];
        REQUIRE(root_1_node.is_mapping());
        REQUIRE(root_1_node.size() == 3);
        REQUIRE(root_1_node.contains("name"));
        REQUIRE(root_1_node.contains("hr"));
        REQUIRE(root_1_node.contains("avg"));

        fkyaml::node& root_1_name_node = root_1_node["name"];
        REQUIRE(root_1_name_node.is_string());
        REQUIRE(root_1_name_node.as_str() == "Sammy Sosa");

        fkyaml::node& root_1_hr_node = root_1_node["hr"];
        REQUIRE(root_1_hr_node.is_integer());
        REQUIRE(root_1_hr_node.get_value<int>() == 63);

        fkyaml::node& root_1_avg_node = root_1_node["avg"];
        REQUIRE(root_1_avg_node.is_float_number());
        REQUIRE(root_1_avg_node.get_value<double>() == 0.288);
    }

    SECTION("block sequence as a nested block mapping value in the middle") {
        std::string input = "foo:\n"
                            "  bar:\n"
                            "  - 123\n"
                            "  baz: true\n";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_mapping());
        REQUIRE(foo_node.size() == 2);
        REQUIRE(foo_node.contains("bar"));
        REQUIRE(foo_node.contains("baz"));

        fkyaml::node& foo_bar_node = foo_node["bar"];
        REQUIRE(foo_bar_node.is_sequence());
        REQUIRE(foo_bar_node.size() == 1);

        fkyaml::node& foo_bar_0_node = foo_bar_node[0];
        REQUIRE(foo_bar_0_node.is_integer());
        REQUIRE(foo_bar_0_node.get_value<int>() == 123);

        fkyaml::node& foo_baz_node = foo_node["baz"];
        REQUIRE(foo_baz_node.is_boolean());
        REQUIRE(foo_baz_node.get_value<bool>() == true);
    }

    SECTION("empty block sequence entries") {
        std::string input = "- -\n"
                            "  - 123\n"
                            "  -\n"
                            "-\n";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 2);

        fkyaml::node& root_0 = root[0];
        REQUIRE(root_0.is_sequence());
        REQUIRE(root_0.size() == 3);
        REQUIRE(root_0[0].is_null());
        REQUIRE(root_0[1].is_integer());
        REQUIRE(root_0[1].get_value<int>() == 123);
        REQUIRE(root_0[2].is_null());

        REQUIRE(root[1].is_null());
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
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains("bar"));
        REQUIRE(root.contains("pi"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "one");

        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_boolean());
        REQUIRE(bar_node.get_value<bool>() == true);

        fkyaml::node& pi_node = root["pi"];
        REQUIRE(pi_node.is_float_number());
        REQUIRE(pi_node.get_value<double>() == 3.14);
    }

    // regression test for https://github.com/fktn-k/fkYAML/pull/437
    SECTION("indented block mapping beginning with a newline") {
        std::string input = R"(
    foo: true
    bar: 123
    baz: 3.14)";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 3);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains("bar"));
        REQUIRE(root.contains("baz"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_boolean());
        REQUIRE(foo_node.get_value<bool>() == true);

        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_integer());
        REQUIRE(bar_node.get_value<int>() == 123);

        fkyaml::node& baz_node = root["baz"];
        REQUIRE(baz_node.is_float_number());
        REQUIRE(baz_node.get_value<double>() == 3.14);
    };

    SECTION("nested block mapping") {
        std::string input = "test:\n"
                            "  bool: true\n"
                            "  foo: bar\n"
                            "  pi: 3.14";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE_NOTHROW(root.size());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("test"));

        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_mapping());
        REQUIRE_NOTHROW(test_node.size());
        REQUIRE(test_node.size() == 3);
        REQUIRE(test_node.contains("bool"));
        REQUIRE(test_node.contains("foo"));
        REQUIRE(test_node.contains("pi"));

        fkyaml::node& test_bool_node = test_node["bool"];
        REQUIRE(test_bool_node.is_boolean());
        REQUIRE(test_bool_node.get_value<bool>() == true);

        fkyaml::node& test_foo_node = test_node["foo"];
        REQUIRE(test_foo_node.is_string());
        REQUIRE(test_foo_node.as_str().compare("bar") == 0);

        fkyaml::node& test_pi_node = test_node["pi"];
        REQUIRE(test_pi_node.is_float_number());
        REQUIRE(test_pi_node.get_value<double>() == 3.14);
    }

    SECTION("block mapping with several nested children") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(
                fkyaml::detail::input_adapter("foo:\n  bar: baz\nqux: 123\nquux:\n  corge: grault")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 3);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains("qux"));
        REQUIRE(root.contains("quux"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_mapping());
        REQUIRE(foo_node.size() == 1);
        REQUIRE(foo_node.contains("bar"));

        fkyaml::node& foo_bar_node = foo_node["bar"];
        REQUIRE(foo_bar_node.is_string());
        REQUIRE(foo_bar_node.as_str() == "baz");

        fkyaml::node& qux_node = root["qux"];
        REQUIRE(qux_node.is_integer());
        REQUIRE(qux_node.get_value<int>() == 123);

        fkyaml::node& quux_node = root["quux"];
        REQUIRE(quux_node.is_mapping());
        REQUIRE(quux_node.size() == 1);
        REQUIRE(quux_node.contains("corge"));

        fkyaml::node& quux_corge_node = quux_node["corge"];
        REQUIRE(quux_corge_node.is_string());
        REQUIRE(quux_corge_node.as_str() == "grault");
    }

    SECTION("block mapping with a more nested child") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo:\n  bar:\n    baz: 123\nqux: true")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains("qux"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_mapping());
        REQUIRE(foo_node.size() == 1);
        REQUIRE(foo_node.contains("bar"));

        fkyaml::node& foo_bar_node = foo_node["bar"];
        REQUIRE(foo_bar_node.is_mapping());
        REQUIRE(foo_bar_node.size() == 1);
        REQUIRE(foo_bar_node.contains("baz"));

        fkyaml::node& foo_bar_baz_node = foo_bar_node["baz"];
        REQUIRE(foo_bar_baz_node.is_integer());
        REQUIRE(foo_bar_baz_node.get_value<int>() == 123);

        fkyaml::node& qux_node = root["qux"];
        REQUIRE(qux_node.is_boolean());
        REQUIRE(qux_node.get_value<bool>() == true);
    }

    SECTION("block mapping with a child block sequence") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo:\n  - bar\n  - 123\nbaz: qux")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains("baz"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_sequence());
        REQUIRE(foo_node.size() == 2);

        fkyaml::node& foo_0_node = foo_node[0];
        REQUIRE(foo_0_node.is_string());
        REQUIRE(foo_0_node.as_str() == "bar");

        fkyaml::node& foo_1_node = foo_node[1];
        REQUIRE(foo_1_node.is_integer());
        REQUIRE(foo_1_node.get_value<int>() == 123);

        fkyaml::node& baz_node = root["baz"];
        REQUIRE(baz_node.is_string());
        REQUIRE(baz_node.as_str() == "qux");
    }

    SECTION("block mapping with a block sequence of a single nested mapping") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo:\n  - bar: baz\nqux: corge")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains("qux"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_sequence());
        REQUIRE(foo_node.size() == 1);

        fkyaml::node& foo_0_node = foo_node[0];
        REQUIRE(foo_0_node.is_mapping());
        REQUIRE(foo_0_node.size() == 1);
        REQUIRE(foo_0_node.contains("bar"));

        fkyaml::node& foo_0_bar_node = foo_0_node["bar"];
        REQUIRE(foo_0_bar_node.is_string());
        REQUIRE(foo_0_bar_node.as_str() == "baz");

        fkyaml::node& qux_node = root["qux"];
        REQUIRE(qux_node.is_string());
        REQUIRE(qux_node.as_str() == "corge");
    }

    SECTION("block mapping with keys containing flow indicators") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("Foo,Bar: true\nBaz[123]: 3.14")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);
        REQUIRE(root.contains("Foo,Bar"));
        REQUIRE(root.contains("Baz[123]"));

        fkyaml::node& foobar_node = root["Foo,Bar"];
        REQUIRE(foobar_node.is_boolean());
        REQUIRE(foobar_node.get_value<bool>() == true);

        fkyaml::node& baz123_node = root["Baz[123]"];
        REQUIRE(baz123_node.is_float_number());
        REQUIRE(baz123_node.get_value<double>() == 3.14);
    }

    SECTION("Flow indicators inside unquoted plain scalar values") {
        SECTION("plain scalar contains \'{\'") {
            REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("Foo: Bar, abc{abc")));

            REQUIRE(root.is_mapping());
            REQUIRE(root.size() == 1);
            REQUIRE(root.contains("Foo"));

            fkyaml::node& foo_node = root["Foo"];
            REQUIRE(foo_node.is_string());
            REQUIRE(foo_node.as_str() == "Bar, abc{abc");
        }

        SECTION("plain scalar contains \'}\'") {
            REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("Foo: Bar, abc}abc")));

            REQUIRE(root.is_mapping());
            REQUIRE(root.size() == 1);
            REQUIRE(root.contains("Foo"));

            fkyaml::node& foo_node = root["Foo"];
            REQUIRE(foo_node.is_string());
            REQUIRE(foo_node.as_str() == "Bar, abc}abc");
        }

        SECTION("plain scalar contains \'[\'") {
            REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("Foo: Bar, abc[abc")));

            REQUIRE(root.is_mapping());
            REQUIRE(root.size() == 1);
            REQUIRE(root.contains("Foo"));

            fkyaml::node& foo_node = root["Foo"];
            REQUIRE(foo_node.is_string());
            REQUIRE(foo_node.as_str() == "Bar, abc[abc");
        }

        SECTION("plain scalar contains \']\'") {
            REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("Foo: Bar, abc]abc")));

            REQUIRE(root.is_mapping());
            REQUIRE(root.size() == 1);
            REQUIRE(root.contains("Foo"));

            fkyaml::node& foo_node = root["Foo"];
            REQUIRE(foo_node.is_string());
            REQUIRE(foo_node.as_str() == "Bar, abc]abc");
        }

        SECTION("plain scalar contains \':\'") {
            REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("Foo: Bar, {[123] :3.14}")));

            REQUIRE(root.is_mapping());
            REQUIRE(root.size() == 1);
            REQUIRE(root.contains("Foo"));

            fkyaml::node& foo_node = root["Foo"];
            REQUIRE(foo_node.is_string());
            REQUIRE(foo_node.as_str() == "Bar, {[123] :3.14}");
        }

        SECTION("plain scalar contains \": \"") {
            REQUIRE_THROWS_AS(
                root = deserializer.deserialize(fkyaml::detail::input_adapter("Foo: Bar, {[123] : 3.14}")),
                fkyaml::parse_error);
        }
    }

    SECTION("a comment right after a block mapping key.") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("baz: # comment2\n  qux: 123\n")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("baz"));

        fkyaml::node& baz_node = root["baz"];
        REQUIRE(baz_node.is_mapping());
        REQUIRE(baz_node.size() == 1);
        REQUIRE(baz_node.contains("qux"));

        fkyaml::node& baz_qux_node = baz_node["qux"];
        REQUIRE(baz_qux_node.is_integer());
        REQUIRE(baz_qux_node.get_value<int>() == 123);
    }

    SECTION("mapping entries split across newlines") {
        std::string input = "foo:\n"
                            "  bar\n"
                            "baz:\n"
                            "  123\n"
                            "null:\n"
                            "  {false: 3.14}\n"
                            "qux:\n"
                            "  [r, g, b]";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 4);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains("baz"));
        REQUIRE(root.contains(nullptr));
        REQUIRE(root.contains("qux"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "bar");

        fkyaml::node& baz_node = root["baz"];
        REQUIRE(baz_node.is_integer());
        REQUIRE(baz_node.get_value<int>() == 123);

        fkyaml::node& null_node = root[nullptr];
        REQUIRE(null_node.is_mapping());
        REQUIRE(null_node.contains(false));

        fkyaml::node& null_false_node = null_node[false];
        REQUIRE(null_false_node.is_float_number());
        REQUIRE(null_false_node.get_value<double>() == 3.14);

        fkyaml::node& qux_node = root["qux"];
        REQUIRE(qux_node.is_sequence());
        REQUIRE(qux_node.size() == 3);

        fkyaml::node& qux_0_node = qux_node[0];
        REQUIRE(qux_0_node.is_string());
        REQUIRE(qux_0_node.as_str() == "r");

        fkyaml::node& qux_1_node = qux_node[1];
        REQUIRE(qux_1_node.is_string());
        REQUIRE(qux_1_node.as_str() == "g");

        fkyaml::node& qux_2_node = qux_node[2];
        REQUIRE(qux_2_node.is_string());
        REQUIRE(qux_2_node.as_str() == "b");
    }

    // // regression test for https://github.com/fktn-k/fkYAML/issues/449
    SECTION("missing the \":\" mapping value indicator after key (root)") {
        std::string input = "1:\n"
                            "1";

        REQUIRE_THROWS_AS(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    // regression test for https://github.com/fktn-k/fkYAML/issues/449
    SECTION("missing the \":\" mapping value indicator after key (nested)") {
        std::string input = "abc:\n"
                            "  def: ghi\n"
                            "  jkl mno";

        REQUIRE_THROWS_AS(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("block mapping which contains empty mapping values") {
        std::string input = "foo:\n"
                            "bar:\n"
                            "  foo:\n"
                            "  bar:\n"
                            "baz:\n";

        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 3);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains("bar"));
        REQUIRE(root.contains("baz"));

        REQUIRE(root["foo"].is_null());
        REQUIRE(root["bar"].is_mapping());
        REQUIRE(root["bar"].size() == 2);
        REQUIRE(root["bar"].contains("foo"));
        REQUIRE(root["bar"].contains("bar"));
        REQUIRE(root["bar"]["foo"].is_null());
        REQUIRE(root["bar"]["bar"].is_null());
        REQUIRE(root["baz"].is_null());
    }

    // regression test for https://github.com/fktn-k/fkYAML/issues/487
    SECTION("block mapping after an empty block sequence entry (same indentation)") {
        std::string input = "test:\n"
                            "    - coords:\n"
                            "      -\n"
                            "      -\n"
                            "      name: \"a\"\n";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("test"));

        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE(test_node.size() == 1);

        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.is_mapping());
        REQUIRE(test_0_node.size() == 2);
        REQUIRE(test_0_node.contains("coords"));
        REQUIRE(test_0_node.contains("name"));

        fkyaml::node& test_0_coords_node = test_0_node["coords"];
        REQUIRE(test_0_coords_node.is_sequence());
        REQUIRE(test_0_coords_node.size() == 2);

        REQUIRE(test_0_coords_node[0].is_null());
        REQUIRE(test_0_coords_node[1].is_null());

        fkyaml::node& test_0_name_node = test_0_node["name"];
        REQUIRE(test_0_name_node.is_string());
        REQUIRE(test_0_name_node.as_str() == "a");
    }

    SECTION("block mapping after an empty block sequence entry (less indented)") {
        std::string input = "test:\n"
                            "    - coords:\n"
                            "        -\n"
                            "        -\n"
                            "      name: \"a\"\n";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("test"));

        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE(test_node.size() == 1);

        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.is_mapping());
        REQUIRE(test_0_node.size() == 2);
        REQUIRE(test_0_node.contains("coords"));
        REQUIRE(test_0_node.contains("name"));

        fkyaml::node& test_0_coords_node = test_0_node["coords"];
        REQUIRE(test_0_coords_node.is_sequence());
        REQUIRE(test_0_coords_node.size() == 2);

        REQUIRE(test_0_coords_node[0].is_null());
        REQUIRE(test_0_coords_node[1].is_null());

        fkyaml::node& test_0_name_node = test_0_node["name"];
        REQUIRE(test_0_name_node.is_string());
        REQUIRE(test_0_name_node.as_str() == "a");
    }
}

TEST_CASE("Deserializer_FlowContainerKey") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("mapping with flow mapping keys") {
        std::string input = "{foo: bar}:\n"
                            "  true:\n"
                            "  - false\n"
                            "  {123: 456}:\n"
                            "    {qux: 789}: true\n"
                            "  {baz: null}: 123\n"
                            "{true: 123}: \n"
                            "  3.14";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);
        fkyaml::node foobar_mapkey = {{"foo", "bar"}};
        REQUIRE(root.contains(foobar_mapkey));
        fkyaml::node true123_mapkey = {{true, 123}};
        REQUIRE(root.contains(true123_mapkey));

        fkyaml::node& root_foobar_mapkey_node = root[std::move(foobar_mapkey)];
        REQUIRE(root_foobar_mapkey_node.is_mapping());
        REQUIRE(root_foobar_mapkey_node.size() == 3);
        REQUIRE(root_foobar_mapkey_node.contains(true));
        fkyaml::node _123_456_mapkey = {{123, 456}};
        REQUIRE(root_foobar_mapkey_node.contains(_123_456_mapkey));
        fkyaml::node baznull_mapkey = {{"baz", nullptr}};
        REQUIRE(root_foobar_mapkey_node.contains(baznull_mapkey));

        fkyaml::node& root_foobar_mapkey_true_node = root_foobar_mapkey_node[true];
        REQUIRE(root_foobar_mapkey_true_node.is_sequence());
        REQUIRE(root_foobar_mapkey_true_node.size() == 1);

        fkyaml::node& root_foobar_mapkey_true_0_node = root_foobar_mapkey_true_node[0];
        REQUIRE(root_foobar_mapkey_true_0_node.is_boolean());
        REQUIRE(root_foobar_mapkey_true_0_node.get_value<bool>() == false);

        fkyaml::node& root_foobar_mapkey_123_456_mapkey_node = root_foobar_mapkey_node[std::move(_123_456_mapkey)];
        REQUIRE(root_foobar_mapkey_123_456_mapkey_node.is_mapping());
        REQUIRE(root_foobar_mapkey_123_456_mapkey_node.size() == 1);
        fkyaml::node qux789_mapkey = {{"qux", 789}};
        REQUIRE(root_foobar_mapkey_123_456_mapkey_node.contains(qux789_mapkey));

        fkyaml::node& root_foobar_mapkey_123_456_mapkey_qux789_mapkey_node =
            root_foobar_mapkey_123_456_mapkey_node[std::move(qux789_mapkey)];
        REQUIRE(root_foobar_mapkey_123_456_mapkey_qux789_mapkey_node.is_boolean());
        REQUIRE(root_foobar_mapkey_123_456_mapkey_qux789_mapkey_node.get_value<bool>() == true);

        fkyaml::node& root_foobar_mapkey_baznull_mapkey_node = root_foobar_mapkey_node[std::move(baznull_mapkey)];
        REQUIRE(root_foobar_mapkey_baznull_mapkey_node.is_integer());
        REQUIRE(root_foobar_mapkey_baznull_mapkey_node.get_value<int>() == 123);

        fkyaml::node& root_true123_mapkey_node = root[std::move(true123_mapkey)];
        REQUIRE(root_true123_mapkey_node.is_float_number());
        REQUIRE(root_true123_mapkey_node.get_value<double>() == 3.14);
    }

    SECTION("invalid indentation of a flow mapping key") {
        std::string input = "{foo: bar}:\n"
                            "  {123: 456}:\n"
                            "    {qux: 789}: true\n"
                            "   {baz: null}: 123";
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("mapping with flow sequence keys") {
        std::string input = "[foo,bar]:\n"
                            "  true:\n"
                            "  - false\n"
                            "  [123]:\n"
                            "    [qux]: true\n"
                            "  [baz,null]: 123\n"
                            "[true,123]: \n"
                            "  3.14";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);
        fkyaml::node foobar_seqkey = {"foo", "bar"};
        REQUIRE(root.contains(foobar_seqkey));
        fkyaml::node true123_seqkey = {true, 123};
        REQUIRE(root.contains(true123_seqkey));

        fkyaml::node& root_foobar_seqkey_node = root[std::move(foobar_seqkey)];
        REQUIRE(root_foobar_seqkey_node.is_mapping());
        REQUIRE(root_foobar_seqkey_node.size() == 3);
        REQUIRE(root_foobar_seqkey_node.contains(true));
        fkyaml::node baznull_seqkey = {"baz", nullptr};
        REQUIRE(root_foobar_seqkey_node.contains(baznull_seqkey));
        fkyaml::node _123_seqkey = {123};
        REQUIRE(root_foobar_seqkey_node.contains(_123_seqkey));

        fkyaml::node& root_foobar_seqkey_true_node = root_foobar_seqkey_node[true];
        REQUIRE(root_foobar_seqkey_true_node.is_sequence());
        REQUIRE(root_foobar_seqkey_true_node.size() == 1);

        fkyaml::node& root_foobar_seqkey_true_0_node = root_foobar_seqkey_true_node[0];
        REQUIRE(root_foobar_seqkey_true_0_node.is_boolean());
        REQUIRE(root_foobar_seqkey_true_0_node.get_value<bool>() == false);

        fkyaml::node& root_foobar_seqkey_123_seqkey_node = root_foobar_seqkey_node[std::move(_123_seqkey)];
        REQUIRE(root_foobar_seqkey_123_seqkey_node.is_mapping());
        REQUIRE(root_foobar_seqkey_123_seqkey_node.size() == 1);
        fkyaml::node qux_seqkey = {"qux"};
        REQUIRE(root_foobar_seqkey_123_seqkey_node.contains(qux_seqkey));

        fkyaml::node& root_foobar_seqkey_123_seqkey_qux_seqkey_node =
            root_foobar_seqkey_123_seqkey_node[std::move(qux_seqkey)];
        REQUIRE(root_foobar_seqkey_123_seqkey_qux_seqkey_node.is_boolean());
        REQUIRE(root_foobar_seqkey_123_seqkey_qux_seqkey_node.get_value<bool>() == true);

        fkyaml::node& root_foobar_seqkey_baznull_seqkey_node = root_foobar_seqkey_node[std::move(baznull_seqkey)];
        REQUIRE(root_foobar_seqkey_baznull_seqkey_node.is_integer());
        REQUIRE(root_foobar_seqkey_baznull_seqkey_node.get_value<int>() == 123);

        fkyaml::node& root_true123_seqkey_node = root[std::move(true123_seqkey)];
        REQUIRE(root_true123_seqkey_node.is_float_number());
        REQUIRE(root_true123_seqkey_node.get_value<double>() == 3.14);
    }

    SECTION("invalid indentation of a flow sequence key") {
        std::string input = "[foo,bar]:\n"
                            "  [123]:\n"
                            "    [qux]: true\n"
                            "   [baz,null]: 123";
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }
}

TEST_CASE("Deserializer_BlockMappingAsBlockSequenceEntry") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("block mapping with a block sequence of a block mapping with several key-value pairs") {
        std::string input = "foo:\n"
                            "  - bar: true\n"
                            "    baz: 123\n"
                            "qux: corge";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains("qux"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_sequence());
        REQUIRE(foo_node.size() == 1);

        fkyaml::node& foo_0_node = foo_node[0];
        REQUIRE(foo_0_node.is_mapping());
        REQUIRE(foo_0_node.size() == 2);
        REQUIRE(foo_0_node.contains("bar"));
        REQUIRE(foo_0_node.contains("baz"));

        fkyaml::node& foo_0_bar_node = foo_0_node["bar"];
        REQUIRE(foo_0_bar_node.is_boolean());
        REQUIRE(foo_0_bar_node.get_value<bool>() == true);

        fkyaml::node& foo_0_baz_node = foo_0_node["baz"];
        REQUIRE(foo_0_baz_node.is_integer());
        REQUIRE(foo_0_baz_node.get_value<int>() == 123);

        fkyaml::node& qux_node = root["qux"];
        REQUIRE(qux_node.is_string());
        REQUIRE(qux_node.as_str() == "corge");
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

        fkyaml::node& stuff_node = root["stuff"];
        REQUIRE(stuff_node.is_sequence());
        REQUIRE(stuff_node.size() == 2);

        fkyaml::node& stuff_0_node = stuff_node[0];
        REQUIRE(stuff_0_node.is_mapping());
        REQUIRE(stuff_0_node.size() == 4);
        REQUIRE(stuff_0_node.contains("id"));
        REQUIRE(stuff_0_node.contains("name"));
        REQUIRE(stuff_0_node.contains("tags"));
        REQUIRE(stuff_0_node.contains("params"));

        fkyaml::node& stuff_0_id_node = stuff_0_node["id"];
        REQUIRE(stuff_0_id_node.is_string());
        REQUIRE(stuff_0_id_node.as_str() == "foo");

        fkyaml::node& stuff_0_name_node = stuff_0_node["name"];
        REQUIRE(stuff_0_name_node.is_string());
        REQUIRE(stuff_0_name_node.as_str() == "Foo");

        fkyaml::node& stuff_0_tags_node = stuff_0_node["tags"];
        REQUIRE(stuff_0_tags_node.is_sequence());
        REQUIRE(stuff_0_tags_node.size() == 1);

        fkyaml::node& stuff_0_tags_0_node = stuff_0_tags_node[0];
        REQUIRE(stuff_0_tags_0_node.is_string());
        REQUIRE(stuff_0_tags_0_node.as_str() == "baz");

        fkyaml::node& stuff_0_params_node = stuff_0_node["params"];
        REQUIRE(stuff_0_params_node.is_mapping());
        REQUIRE(stuff_0_params_node.size() == 1);
        REQUIRE(stuff_0_params_node.contains("key"));

        fkyaml::node& stuff_0_params_key_node = stuff_0_params_node["key"];
        REQUIRE(stuff_0_params_key_node.is_integer());
        REQUIRE(stuff_0_params_key_node.get_value<int>() == 1);

        fkyaml::node& stuff_1_node = stuff_node[1];
        REQUIRE(stuff_1_node.is_mapping());
        REQUIRE(stuff_1_node.size() == 2);
        REQUIRE(stuff_1_node.contains("id"));
        REQUIRE(stuff_1_node.contains("name"));

        fkyaml::node& stuff_1_id_node = stuff_1_node["id"];
        REQUIRE(stuff_1_id_node.is_string());
        REQUIRE(stuff_1_id_node.as_str() == "bar");

        fkyaml::node& stuff_1_name_node = stuff_1_node["name"];
        REQUIRE(stuff_1_name_node.is_string());
        REQUIRE(stuff_1_name_node.as_str() == "Bar");
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

        fkyaml::node& stuff_node = root["stuff"];
        REQUIRE(stuff_node.is_sequence());
        REQUIRE(stuff_node.size() == 2);

        fkyaml::node& stuff_0_node = stuff_node[0];
        REQUIRE(stuff_0_node.is_mapping());
        REQUIRE(stuff_0_node.size() == 4);
        REQUIRE(stuff_0_node.contains("id"));
        REQUIRE(stuff_0_node.contains("name"));
        REQUIRE(stuff_0_node.contains("tags"));
        REQUIRE(stuff_0_node.contains("params"));

        fkyaml::node& stuff_0_id_node = stuff_0_node["id"];
        REQUIRE(stuff_0_id_node.is_string());
        REQUIRE(stuff_0_id_node.as_str() == "foo");

        fkyaml::node& stuff_0_name_node = stuff_0_node["name"];
        REQUIRE(stuff_0_name_node.is_string());
        REQUIRE(stuff_0_name_node.as_str() == "Foo");

        fkyaml::node& stuff_0_tags_node = stuff_0_node["tags"];
        REQUIRE(stuff_0_tags_node.is_sequence());
        REQUIRE(stuff_0_tags_node.size() == 1);

        REQUIRE(stuff_0_tags_node[0].is_string());
        REQUIRE(stuff_0_tags_node[0].as_str() == "baz");

        fkyaml::node& stuff_0_params_node = stuff_0_node["params"];
        REQUIRE(stuff_0_params_node.is_mapping());
        REQUIRE(stuff_0_params_node.size() == 1);
        REQUIRE(stuff_0_params_node.contains("key"));

        fkyaml::node& stuff_0_params_key_node = stuff_0_params_node["key"];
        REQUIRE(stuff_0_params_key_node.is_integer());
        REQUIRE(stuff_0_params_key_node.get_value<int>() == 1);

        fkyaml::node& stuff_1_node = stuff_node[1];
        REQUIRE(stuff_1_node.is_mapping());
        REQUIRE(stuff_1_node.size() == 2);
        REQUIRE(stuff_1_node.contains("id"));
        REQUIRE(stuff_1_node.contains("name"));

        fkyaml::node& stuff_1_id_node = stuff_1_node["id"];
        REQUIRE(stuff_1_id_node.is_string());
        REQUIRE(stuff_1_id_node.as_str() == "bar");

        fkyaml::node& stuff_1_name_node = stuff_1_node["name"];
        REQUIRE(stuff_1_name_node.is_string());
        REQUIRE(stuff_1_name_node.as_str() == "Bar");
    }

    SECTION("block mapping entry with child block sequence of block mapping entries") {
        std::string input = "contexts:\n"
                            "- context:\n"
                            "    cluster: abcdef\n"
                            "    extension:\n"
                            "    - extension:\n"
                            "        last-update: blah\n"
                            "        version: 0.1.0\n"
                            "      name: blah\n"
                            "    bug: default\n"
                            "  ctx: ctx";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("contexts"));

        fkyaml::node& contexts_node = root["contexts"];
        REQUIRE(contexts_node.is_sequence());
        REQUIRE(contexts_node.size() == 1);

        fkyaml::node& contexts_0_node = contexts_node[0];
        REQUIRE(contexts_0_node.is_mapping());
        REQUIRE(contexts_0_node.size() == 2);
        REQUIRE(contexts_0_node.contains("context"));
        REQUIRE(contexts_0_node.contains("ctx"));

        fkyaml::node& contexts_0_context_node = contexts_0_node["context"];
        REQUIRE(contexts_0_context_node.is_mapping());
        REQUIRE(contexts_0_context_node.size() == 3);
        REQUIRE(contexts_0_context_node.contains("cluster"));
        REQUIRE(contexts_0_context_node.contains("extension"));
        REQUIRE(contexts_0_context_node.contains("bug"));

        fkyaml::node& contexts_0_context_cluster_node = contexts_0_context_node["cluster"];
        REQUIRE(contexts_0_context_cluster_node.is_string());
        REQUIRE(contexts_0_context_cluster_node.as_str() == "abcdef");

        fkyaml::node& contexts_0_context_extension_node = contexts_0_context_node["extension"];
        REQUIRE(contexts_0_context_extension_node.is_sequence());
        REQUIRE(contexts_0_context_extension_node.size() == 1);

        fkyaml::node& contexts_0_context_extension_0_node = contexts_0_context_extension_node[0];
        REQUIRE(contexts_0_context_extension_0_node.is_mapping());
        REQUIRE(contexts_0_context_extension_0_node.size() == 2);
        REQUIRE(contexts_0_context_extension_0_node.contains("extension"));
        REQUIRE(contexts_0_context_extension_0_node.contains("name"));

        fkyaml::node& contexts_0_context_extension_0_extension_node = contexts_0_context_extension_0_node["extension"];
        REQUIRE(contexts_0_context_extension_0_extension_node.is_mapping());
        REQUIRE(contexts_0_context_extension_0_extension_node.size() == 2);
        REQUIRE(contexts_0_context_extension_0_extension_node.contains("last-update"));
        REQUIRE(contexts_0_context_extension_0_extension_node.contains("version"));

        fkyaml::node& contexts_0_context_extension_0_extension_lastupdate_node =
            contexts_0_context_extension_0_extension_node["last-update"];
        REQUIRE(contexts_0_context_extension_0_extension_lastupdate_node.is_string());
        REQUIRE(contexts_0_context_extension_0_extension_lastupdate_node.as_str() == "blah");

        fkyaml::node& contexts_0_context_extension_0_extension_version_node =
            contexts_0_context_extension_0_extension_node["version"];
        REQUIRE(contexts_0_context_extension_0_extension_version_node.is_string());
        REQUIRE(contexts_0_context_extension_0_extension_version_node.as_str() == "0.1.0");

        fkyaml::node& contexts_0_context_extension_0_name_node = contexts_0_context_extension_0_node["name"];
        REQUIRE(contexts_0_context_extension_0_name_node.is_string());
        REQUIRE(contexts_0_context_extension_0_name_node.as_str() == "blah");

        fkyaml::node& contexts_0_context_bug_node = contexts_0_context_node["bug"];
        REQUIRE(contexts_0_context_bug_node.is_string());
        REQUIRE(contexts_0_context_bug_node.as_str() == "default");

        fkyaml::node& contexts_0_ctx_node = contexts_0_node["ctx"];
        REQUIRE(contexts_0_ctx_node.is_string());
        REQUIRE(contexts_0_ctx_node.as_str() == "ctx");
    }
}

TEST_CASE("Deserializer_ExplicitBlockMapping") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

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
        REQUIRE(root.contains("foo"));
        REQUIRE(root["foo"].contains("bar"));
        fkyaml::node key1 = {123, {{"foo", {{"bar", "baz"}}}}};
        REQUIRE(root.contains(key1));
        fkyaml::node key2 = {{"foo", "bar"}};
        REQUIRE(root.contains(key2));

        fkyaml::node& null_node = root[nullptr];
        REQUIRE(null_node.is_float_number());
        REQUIRE(null_node.get_value<double>() == 3.14);

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_mapping());
        REQUIRE(foo_node.size() == 1);

        fkyaml::node& foo_bar_node = foo_node["bar"];
        REQUIRE(foo_bar_node.is_string());
        REQUIRE(foo_bar_node.as_str() == "baz");

        fkyaml::node& key1_node = root[key1];
        REQUIRE(key1_node.is_mapping());
        REQUIRE(key1_node.size() == 1);
        REQUIRE(key1_node.contains("one"));

        fkyaml::node& key1_one_node = key1_node["one"];
        REQUIRE(key1_one_node.is_boolean());
        REQUIRE(key1_one_node.get_value<bool>() == true);

        fkyaml::node& key2_node = root[key2];
        REQUIRE(key2_node.is_sequence());
        REQUIRE(key2_node.size() == 2);

        fkyaml::node& key2_0_node = key2_node[0];
        REQUIRE(key2_0_node.is_string());
        REQUIRE(key2_0_node.as_str() == "baz");

        fkyaml::node& key2_1_node = key2_node[1];
        REQUIRE(key2_1_node.is_string());
        REQUIRE(key2_1_node.as_str() == "qux");
    }

    SECTION("Explicit block mapping as block sequence entry") {
        std::string input = "- ? foo: 123\n"
                            "  : true: 3.14\n"
                            "- ? - bar\n"
                            "  : - null\n"
                            "- ? - ? baz: false\n"
                            "      : 456\n"
                            "  : - ? qux: 789\n"
                            "      : 1.41";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 3);

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.is_mapping());
        REQUIRE(root_0_node.size() == 1);
        fkyaml::node foo123_mapkey = {{"foo", 123}};
        REQUIRE(root_0_node.contains(foo123_mapkey));

        fkyaml::node& root_0_foo123_mapkey_node = root_0_node[std::move(foo123_mapkey)];
        REQUIRE(root_0_foo123_mapkey_node.is_mapping());
        REQUIRE(root_0_foo123_mapkey_node.size() == 1);
        REQUIRE(root_0_foo123_mapkey_node.contains(true));

        fkyaml::node& root_0_foo123_mapkey_true_node = root_0_foo123_mapkey_node[true];
        REQUIRE(root_0_foo123_mapkey_true_node.is_float_number());
        REQUIRE(root_0_foo123_mapkey_true_node.get_value<double>() == 3.14);

        fkyaml::node& root_1_node = root[1];
        REQUIRE(root_1_node.is_mapping());
        REQUIRE(root_1_node.size() == 1);
        fkyaml::node bar_seqkey = {"bar"};
        REQUIRE(root_1_node.contains(bar_seqkey));

        fkyaml::node& root_1_bar_seqkey_node = root_1_node[std::move(bar_seqkey)];
        REQUIRE(root_1_bar_seqkey_node.is_sequence());
        REQUIRE(root_1_bar_seqkey_node.size() == 1);

        fkyaml::node& root_1_bar_seqkey_0_node = root_1_bar_seqkey_node[0];
        REQUIRE(root_1_bar_seqkey_0_node.is_null());
        REQUIRE(root_1_bar_seqkey_0_node.get_value<std::nullptr_t>() == nullptr);

        fkyaml::node& root_2_node = root[2];
        REQUIRE(root_2_node.is_mapping());
        REQUIRE(root_2_node.size() == 1);
        fkyaml::node bazfalse_map = {{"baz", false}};
        fkyaml::node bazfalse_456_map = {{std::move(bazfalse_map), 456}};
        fkyaml::node bazfalse_456_seqmapkey = fkyaml::node::sequence();
        bazfalse_456_seqmapkey.as_seq().emplace_back(std::move(bazfalse_456_map));
        REQUIRE(root_2_node.contains(bazfalse_456_seqmapkey));

        fkyaml::node& root_2_bazfalse_456_seqmapkey_node = root_2_node[std::move(bazfalse_456_seqmapkey)];
        REQUIRE(root_2_bazfalse_456_seqmapkey_node.is_sequence());
        REQUIRE(root_2_bazfalse_456_seqmapkey_node.size() == 1);

        fkyaml::node& root_2_bazfalse_456_seqmapkey_0_node = root_2_bazfalse_456_seqmapkey_node[0];
        REQUIRE(root_2_bazfalse_456_seqmapkey_0_node.is_mapping());
        REQUIRE(root_2_bazfalse_456_seqmapkey_0_node.size() == 1);
        fkyaml::node qux789_mapkey = {{"qux", 789}};
        REQUIRE(root_2_bazfalse_456_seqmapkey_0_node.contains(qux789_mapkey));

        fkyaml::node& root_2_bazfalse_456_seqmapkey_0_qux789_mapkey_node =
            root_2_bazfalse_456_seqmapkey_0_node[std::move(qux789_mapkey)];
        REQUIRE(root_2_bazfalse_456_seqmapkey_0_qux789_mapkey_node.is_float_number());
        REQUIRE(root_2_bazfalse_456_seqmapkey_0_qux789_mapkey_node.get_value<double>() == 1.41);
    }

    SECTION("nested explicit mapping keys in various ways") {
        std::string input = "foo:\n"
                            "  ? ? foo\n"
                            "    : bar\n"
                            "  : ? baz\n"
                            "    : - ? qux\n"
                            "        : 123\n";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_mapping());
        REQUIRE(foo_node.size() == 1);
        fkyaml::node map_key = {{"foo", "bar"}};
        REQUIRE(foo_node.contains(map_key));

        fkyaml::node& foo_inner_node = foo_node[map_key];
        REQUIRE(foo_inner_node.is_mapping());
        REQUIRE(foo_inner_node.size() == 1);
        REQUIRE(foo_inner_node.contains("baz"));

        fkyaml::node& foo_inner_baz_node = foo_inner_node["baz"];
        REQUIRE(foo_inner_baz_node.is_sequence());
        REQUIRE(foo_inner_baz_node.size() == 1);

        fkyaml::node& foo_inner_baz_0_node = foo_inner_baz_node[0];
        REQUIRE(foo_inner_baz_0_node.is_mapping());
        REQUIRE(foo_inner_baz_0_node.size() == 1);
        REQUIRE(foo_inner_baz_0_node.contains("qux"));
        REQUIRE(foo_inner_baz_0_node["qux"].is_integer());
        REQUIRE(foo_inner_baz_0_node["qux"].get_value<int>() == 123);
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
        REQUIRE(root.contains("test"));

        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE(test_node.size() == 2);

        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.is_string());
        REQUIRE(test_0_node.as_str() == "foo");

        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_string());
        REQUIRE(test_1_node.as_str() == "bar");
    }

    SECTION("lack the beginning of a flow sequence") {
        auto input = GENERATE(std::string("test: {]}"), std::string("test: {foo: bar]}"), std::string("test: bar  ]"));
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("root flow sequence") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("[foo,123,3.14]")));
        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 3);

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.is_string());
        REQUIRE(root_0_node.as_str() == "foo");

        fkyaml::node& root_1_node = root[1];
        REQUIRE(root_1_node.is_integer());
        REQUIRE(root_1_node.get_value<int>() == 123);

        fkyaml::node& root_2_node = root[2];
        REQUIRE(root_2_node.is_float_number());
        REQUIRE(root_2_node.get_value<double>() == 3.14);
    }

    SECTION("root flow sequence with nested flow sequences") {
        std::string input = "[\n"
                            "  [\n"
                            "    \"a\",\n"
                            "    \"b\",\n"
                            "  ],\n"
                            "  [\n"
                            "    123,\n"
                            "    true,\n"
                            "  ],\n"
                            "]";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 2);

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.is_sequence());
        REQUIRE(root_0_node.size() == 2);

        fkyaml::node& root_0_0_node = root_0_node[0];
        REQUIRE(root_0_0_node.is_string());
        REQUIRE(root_0_0_node.as_str() == "a");

        fkyaml::node& root_0_1_node = root_0_node[1];
        REQUIRE(root_0_1_node.is_string());
        REQUIRE(root_0_1_node.as_str() == "b");

        fkyaml::node& root_1_node = root[1];
        REQUIRE(root_1_node.is_sequence());
        REQUIRE(root_1_node.size() == 2);

        fkyaml::node& root_1_0_node = root_1_node[0];
        REQUIRE(root_1_0_node.is_integer());
        REQUIRE(root_1_0_node.get_value<int>() == 123);

        fkyaml::node& root_1_1_node = root_1_node[1];
        REQUIRE(root_1_1_node.is_boolean());
        REQUIRE(root_1_1_node.get_value<bool>() == true);
    }

    SECTION("root flow sequence with nested flow mappings") {
        std::string input = "[\n"
                            "  {\n"
                            "    true: 1.23,\n"
                            "    null: 123,\n"
                            "  },\n"
                            "  {\n"
                            "    \"a\": foo,\n"
                            "    \"b\": bar,\n"
                            "  },\n"
                            "]";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 2);

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.is_mapping());
        REQUIRE(root_0_node.size() == 2);
        REQUIRE(root_0_node.contains(true));
        REQUIRE(root_0_node.contains(nullptr));

        fkyaml::node& root_0_true_node = root_0_node[true];
        REQUIRE(root_0_true_node.is_float_number());
        REQUIRE(root_0_true_node.get_value<double>() == 1.23);

        fkyaml::node& root_0_null_node = root_0_node[nullptr];
        REQUIRE(root_0_null_node.is_integer());
        REQUIRE(root_0_null_node.get_value<int>() == 123);

        fkyaml::node& root_1_node = root[1];
        REQUIRE(root_1_node.is_mapping());
        REQUIRE(root_1_node.size() == 2);
        REQUIRE(root_1_node.contains("a"));
        REQUIRE(root_1_node.contains("b"));

        fkyaml::node& root_1_a_node = root_1_node["a"];
        REQUIRE(root_1_a_node.is_string());
        REQUIRE(root_1_a_node.as_str() == "foo");

        fkyaml::node& root_1_b_node = root_1_node["b"];
        REQUIRE(root_1_b_node.is_string());
        REQUIRE(root_1_b_node.as_str() == "bar");
    }

    SECTION("missing value separators") {
        auto input = GENERATE(
            std::string("[123  true, 3.14]"),
            std::string("[123, true  3.14]"),
            // std::string("[123  [true, 3.14]]"),
            std::string("[123, [true  3.14]]"),
            // std::string("[123  {foo: true, bar: 3.14}]"),
            std::string("[123, {foo: true  bar: 3.14}]"));
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("too many value separators") {
        std::string input = "[123,,true]";
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("empty flow sequence") {
        std::string input = "foo: []";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_sequence());
        REQUIRE(foo_node.empty());
    }
}

TEST_CASE("Deserializer_FlowMapping") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("simple flow mapping") {
        std::string input = "test: { bool: true, foo: bar, pi: 3.14 }";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE_NOTHROW(root.size());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("test"));

        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_mapping());
        REQUIRE(test_node.size() == 3);
        REQUIRE(test_node.contains("bool"));
        REQUIRE(test_node.contains("foo"));
        REQUIRE(test_node.contains("pi"));

        fkyaml::node& test_bool_node = test_node["bool"];
        REQUIRE(test_bool_node.is_boolean());
        REQUIRE(test_bool_node.get_value<bool>() == true);

        fkyaml::node& test_foo_node = test_node["foo"];
        REQUIRE(test_foo_node.is_string());
        REQUIRE(test_foo_node.as_str() == "bar");

        fkyaml::node& test_pi_node = test_node["pi"];
        REQUIRE(test_pi_node.is_float_number());
        REQUIRE(test_pi_node.get_value<double>() == 3.14);
    }

    SECTION("Correct traversal after deserializing flow mapping value") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("test: { foo: bar }\n"
                                                                          "sibling: a_string_val")));
        REQUIRE(root.is_mapping());
        REQUIRE_NOTHROW(root.size());
        REQUIRE(root.size() == 2);
        REQUIRE(root.contains("test"));
        REQUIRE(root.contains("sibling"));

        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_mapping());
        REQUIRE(test_node.size() == 1);
        REQUIRE(test_node.contains("foo"));

        fkyaml::node& test_foo_node = test_node["foo"];
        REQUIRE(test_foo_node.is_string());
        REQUIRE(test_foo_node.as_str() == "bar");

        fkyaml::node& sibling_node = root["sibling"];
        REQUIRE(sibling_node.is_string());
        REQUIRE(sibling_node.as_str() == "a_string_val");
    }

    SECTION("lack the beginning of a flow mapping") {
        auto input = GENERATE(std::string("test: [}]"), std::string("test: [true}]"), std::string("test: foo  }"));
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("flow mapping with child flow sequence") {
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

        fkyaml::node& test_foo_0_node = test_foo_node[0];
        REQUIRE(test_foo_0_node.is_boolean());
        REQUIRE(test_foo_0_node.get_value<bool>() == true);

        fkyaml::node& test_foo_1_node = test_foo_node[1];
        REQUIRE(test_foo_1_node.is_integer());
        REQUIRE(test_foo_1_node.get_value<int>() == 123);
    }

    SECTION("flow mapping with child flow mapping") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("test: {foo: {true: 123}}")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("test"));

        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_mapping());
        REQUIRE(test_node.size() == 1);
        REQUIRE(test_node.contains("foo"));

        fkyaml::node& test_foo_node = test_node["foo"];
        REQUIRE(test_foo_node.is_mapping());
        REQUIRE(test_foo_node.size() == 1);
        REQUIRE(test_foo_node.contains(true));

        fkyaml::node& test_foo_true_node = test_foo_node[true];
        REQUIRE(test_foo_true_node.is_integer());
        REQUIRE(test_foo_true_node.get_value<int>() == 123);
    }

    SECTION("root flow mapping") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("{foo: 123,-4: null,true: 3.14}")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 3);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains(-4));
        REQUIRE(root.contains(true));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_integer());
        REQUIRE(foo_node.get_value<int>() == 123);

        fkyaml::node& minus4_node = root[-4];
        REQUIRE(minus4_node.is_null());

        fkyaml::node& true_node = root[true];
        REQUIRE(true_node.is_float_number());
        REQUIRE(true_node.get_value<double>() == 3.14);
    }

    SECTION("root flow mapping with nested flow sequences") {
        std::string input = "{\n"
                            "  \"a\": [\n"
                            "    \"a\",\n"
                            "    \"b\",\n"
                            "  ],\n"
                            "  \"b\": [\n"
                            "    123,\n"
                            "    true,\n"
                            "  ],\n"
                            "}";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);
        REQUIRE(root.contains("a"));
        REQUIRE(root.contains("b"));

        fkyaml::node& root_a_node = root["a"];
        REQUIRE(root_a_node.is_sequence());
        REQUIRE(root_a_node.size() == 2);

        fkyaml::node& root_a_0_node = root_a_node[0];
        REQUIRE(root_a_0_node.is_string());
        REQUIRE(root_a_0_node.as_str() == "a");

        fkyaml::node& root_a_1_node = root_a_node[1];
        REQUIRE(root_a_1_node.is_string());
        REQUIRE(root_a_1_node.as_str() == "b");

        fkyaml::node& root_b_node = root["b"];
        REQUIRE(root_b_node.is_sequence());
        REQUIRE(root_b_node.size() == 2);

        fkyaml::node& root_b_0_node = root_b_node[0];
        REQUIRE(root_b_0_node.is_integer());
        REQUIRE(root_b_0_node.get_value<int>() == 123);

        fkyaml::node& root_b_1_node = root_b_node[1];
        REQUIRE(root_b_1_node.is_boolean());
        REQUIRE(root_b_1_node.get_value<bool>() == true);
    }

    SECTION("root flow mapping with nested flow mappings") {
        std::string input = "{\n"
                            "  \"a\": {\n"
                            "    true: 1.23,\n"
                            "    null: 123,\n"
                            "  },\n"
                            "  \"b\": {\n"
                            "    \"a\": foo,\n"
                            "    \"b\": bar,\n"
                            "  },\n"
                            "}";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);
        REQUIRE(root.contains("a"));
        REQUIRE(root.contains("b"));

        fkyaml::node& root_a_node = root["a"];
        REQUIRE(root_a_node.is_mapping());
        REQUIRE(root_a_node.size() == 2);
        REQUIRE(root_a_node.contains(true));
        REQUIRE(root_a_node.contains(nullptr));

        fkyaml::node& root_a_true_node = root_a_node[true];
        REQUIRE(root_a_true_node.is_float_number());
        REQUIRE(root_a_true_node.get_value<double>() == 1.23);

        fkyaml::node& root_a_null_node = root_a_node[nullptr];
        REQUIRE(root_a_null_node.is_integer());
        REQUIRE(root_a_null_node.get_value<int>() == 123);

        fkyaml::node& root_b_node = root["b"];
        REQUIRE(root_b_node.is_mapping());
        REQUIRE(root_b_node.size() == 2);
        REQUIRE(root_b_node.contains("a"));
        REQUIRE(root_b_node.contains("b"));

        fkyaml::node& root_a_a_node = root_b_node["a"];
        REQUIRE(root_a_a_node.is_string());
        REQUIRE(root_a_a_node.as_str() == "foo");

        fkyaml::node& root_b_b_node = root_b_node["b"];
        REQUIRE(root_b_b_node.is_string());
        REQUIRE(root_b_b_node.as_str() == "bar");
    }

    SECTION("flow mapping key of a flow mapping (compact)") {
        std::string input = "{{\"foo\": true}: \"bar\"}";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        fkyaml::node mapkey = {{"foo", true}};
        REQUIRE(root.contains(mapkey));

        fkyaml::node& root_mapkey_node = root[std::move(mapkey)];
        REQUIRE(root_mapkey_node.is_string());
        REQUIRE(root_mapkey_node.as_str() == "bar");
    }

    SECTION("flow mapping key of a flow mapping (not compact)") {
        std::string input = "{\n"
                            "  {\n"
                            "    \"foo\": true\n"
                            "  }: \"bar\"\n"
                            "}";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        fkyaml::node mapkey = {{"foo", true}};
        REQUIRE(root.contains(mapkey));

        fkyaml::node& root_mapkey_node = root[std::move(mapkey)];
        REQUIRE(root_mapkey_node.is_string());
        REQUIRE(root_mapkey_node.as_str() == "bar");
    }

    SECTION("flow sequence key of a flow mapping (compact)") {
        std::string input = "{[\"foo\",true]: \"bar\"}";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        fkyaml::node seqkey = {"foo", true};
        REQUIRE(root.contains(seqkey));

        fkyaml::node& root_seqkey_node = root[std::move(seqkey)];
        REQUIRE(root_seqkey_node.is_string());
        REQUIRE(root_seqkey_node.as_str() == "bar");
    }

    SECTION("flow sequence key of a flow mapping (not compact)") {
        std::string input = "{\n"
                            "  [\n"
                            "    \"foo\",\n"
                            "    true\n"
                            "  ]: \"bar\"\n"
                            "}";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        fkyaml::node seqkey = {"foo", true};
        REQUIRE(root.contains(seqkey));

        fkyaml::node& root_seqkey_node = root[std::move(seqkey)];
        REQUIRE(root_seqkey_node.is_string());
        REQUIRE(root_seqkey_node.as_str() == "bar");
    }

    SECTION("missing value separators") {
        auto input = GENERATE(
            std::string("{foo: 123  bar: true, baz: 3.14}"),
            std::string("{foo: 123, bar: true  baz: 3.14}"),
            std::string("{foo: 123  {bar: true, baz: 3.14}: child}"),
            std::string("{foo: 123, {bar: true  baz: 3.14}: child}"),
            std::string("{foo: 123  [bar: true, baz: 3.14]: child}"),
            std::string("{foo: 123, [bar: true  baz: 3.14]: child}"));
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("too many value separators") {
        std::string input = "{foo: 123,,bar: true}";
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("empty flow mapping") {
        std::string input = "foo: {}";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_mapping());
        REQUIRE(foo_node.empty());
    }
}

TEST_CASE("Deserializer_BadIndentation") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("implicit mapping entries") {
        std::string input = "abc: def ghi\n"
                            "  jkl: mno";

        REQUIRE_THROWS_AS(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("nested implicit mapping entry with too much indentation") {
        std::string input = "abc:\n"
                            "  def: ghi\n"
                            "    jkl: mno";

        REQUIRE_THROWS_AS(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("nested implicit mapping entry with less indentation") {
        std::string input = "foo:\n"
                            "  bar: baz\n"
                            " qux: true";

        REQUIRE_THROWS_AS(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    // regression test for https://github.com/fktn-k/fkYAML/issues/449
    SECTION("implicit mapping entries with a value on a separate line") {
        std::string input = "abc:\n"
                            "  def ghi\n"
                            "  jkl: mno";

        REQUIRE_THROWS_AS(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("explicit mapping entry with an implicit mapping as its key") {
        std::string input = "? abc: def\n"
                            "    def: ghi\n"
                            ": jkl: mno";

        REQUIRE_THROWS_AS(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("block sequence entries") {
        std::string input = "- \"abc\"\n"
                            " - def";

        REQUIRE_THROWS_AS(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("nested block sequence entries") {
        std::string input = "- - \"abc\"\n"
                            "    - def\n";

        REQUIRE_THROWS_AS(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }
}

TEST_CASE("Deserializer_InputWithComment") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    REQUIRE_NOTHROW(
        root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: one # comment\nbar: true\npi: 3.14")));

    REQUIRE(root.is_mapping());
    REQUIRE(root.size() == 3);
    REQUIRE(root.contains("foo"));
    REQUIRE(root.contains("bar"));
    REQUIRE(root.contains("pi"));

    fkyaml::node& foo_node = root["foo"];
    REQUIRE(foo_node.is_string());
    REQUIRE(foo_node.as_str() == "one");

    fkyaml::node& bar_node = root["bar"];
    REQUIRE(bar_node.is_boolean());
    REQUIRE(bar_node.get_value<bool>() == true);

    fkyaml::node& pi_node = root["pi"];
    REQUIRE(pi_node.is_float_number());
    REQUIRE(pi_node.get_value<double>() == 3.14);
}

TEST_CASE("Deserializer_YAMLVerDirective") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("YAML 1.1") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("%YAML 1.1\n---\nfoo: one")));

        REQUIRE(root.get_yaml_version_type() == fkyaml::yaml_version_type::VERSION_1_1);
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(root.get_yaml_version_type() == fkyaml::yaml_version_type::VERSION_1_1);
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "one");
    }

    SECTION("YAML 1.2") {
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("%YAML 1.2\n---\nfoo: one")));

        REQUIRE(root.get_yaml_version_type() == fkyaml::yaml_version_type::VERSION_1_2);
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(root.get_yaml_version_type() == fkyaml::yaml_version_type::VERSION_1_2);
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "one");
    }

    SECTION("YAML directive in the content is a valid scalar") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: bar\n%YAML 1.1: is valid\ntrue: 123")));

        REQUIRE(root.get_yaml_version_type() == fkyaml::yaml_version_type::VERSION_1_2);
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 3);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains("%YAML 1.1"));
        REQUIRE(root.contains(true));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "bar");

        fkyaml::node& yaml11_node = root["%YAML 1.1"];
        REQUIRE(yaml11_node.is_string());
        REQUIRE(yaml11_node.as_str() == "is valid");

        fkyaml::node& true_node = root[true];
        REQUIRE(true_node.is_integer());
        REQUIRE(true_node.get_value<int>() == 123);
    }

    SECTION("YAML directive more than once") {
        REQUIRE_THROWS_AS(
            deserializer.deserialize(fkyaml::detail::input_adapter("%YAML 1.1\n%YAML 1.2\n")), fkyaml::parse_error);
    }

    SECTION("lacks the end of directives marker after YAML directive") {
        REQUIRE_THROWS_AS(
            deserializer.deserialize(fkyaml::detail::input_adapter("%YAML 1.2\nfoo: bar")), fkyaml::parse_error);
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

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "bar");
        REQUIRE(foo_node.has_tag_name());
        REQUIRE(foo_node.get_tag_name() == "!local");
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

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "bar");
        REQUIRE(foo_node.has_tag_name());
        REQUIRE(foo_node.get_tag_name() == "!!local");
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
        REQUIRE(root.contains("baz"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "bar");
        REQUIRE(foo_node.has_tag_name());
        REQUIRE(foo_node.get_tag_name() == "!e!global");

        fkyaml::node& baz_node = root["baz"];
        REQUIRE(baz_node.is_string());
        REQUIRE(baz_node.as_str() == "qux");
        REQUIRE(baz_node.has_tag_name());
        REQUIRE(baz_node.get_tag_name() == "!f!local");
    }

    SECTION("named tag handle more than once") {
        std::string input = "%TAG !e! tag:test.com,2000:\n"
                            "%TAG !e! !foo-\n"
                            "---\n"
                            "foo: bar";
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("lacks the end of directives marker after TAG directive") {
        std::string input = "%TAG ! tag:test.com,2000:\n"
                            "foo: bar";
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }
}

TEST_CASE("Deserializer_InvalidDirective") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter("%INVALID foo bar")));
    REQUIRE(root.is_null());
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
        REQUIRE(root.contains("test"));

        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE(test_node.size() == 2);

        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.is_anchor());
        REQUIRE(test_0_node.has_anchor_name());
        REQUIRE(test_0_node.get_anchor_name() == "anchor");
        REQUIRE(test_0_node.is_boolean());
        REQUIRE(test_0_node.get_value<bool>() == true);

        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_alias());
        REQUIRE(test_1_node.has_anchor_name());
        REQUIRE(test_1_node.get_anchor_name() == "anchor");
        REQUIRE(test_1_node.is_boolean());
        REQUIRE(test_1_node.get_value<bool>() == test_0_node.get_value<bool>());
    }

    SECTION("block sequence with anchored integer scalar") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - &anchor -123\n  - *anchor")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("test"));

        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE(test_node.size() == 2);

        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.has_anchor_name());
        REQUIRE(test_0_node.get_anchor_name().compare("anchor") == 0);
        REQUIRE(test_0_node.is_integer());
        REQUIRE(test_0_node.get_value<int>() == -123);

        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_alias());
        REQUIRE(test_1_node.has_anchor_name());
        REQUIRE(test_1_node.get_anchor_name() == "anchor");
        REQUIRE(test_1_node.is_integer());
        REQUIRE(test_1_node.get_value<int>() == test_0_node.get_value<int>());
    }

    SECTION("block sequence with anchored floating point number scalar") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - &anchor 3.14\n  - *anchor")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("test"));

        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE(test_node.size() == 2);

        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.is_anchor());
        REQUIRE(test_0_node.has_anchor_name());
        REQUIRE(test_0_node.get_anchor_name() == "anchor");
        REQUIRE(test_0_node.is_float_number());
        REQUIRE(test_0_node.get_value<double>() == 3.14);

        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_alias());
        REQUIRE(test_1_node.has_anchor_name());
        REQUIRE(test_1_node.get_anchor_name() == "anchor");
        REQUIRE(test_1_node.is_float_number());
        REQUIRE(test_1_node.get_value<double>() == test_0_node.get_value<double>());
    }

    SECTION("block sequence with anchored string scalar") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("test:\n  - &anchor foo\n  - *anchor")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("test"));

        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_sequence());
        REQUIRE(test_node.size() == 2);

        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.is_anchor());
        REQUIRE(test_0_node.has_anchor_name());
        REQUIRE(test_0_node.get_anchor_name() == "anchor");
        REQUIRE(test_0_node.is_string());
        REQUIRE(test_0_node.as_str() == "foo");

        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_alias());
        REQUIRE(test_1_node.has_anchor_name());
        REQUIRE(test_1_node.get_anchor_name() == "anchor");
        REQUIRE(test_1_node.is_string());
        REQUIRE(test_1_node.as_str() == test_0_node.as_str());
    }

    SECTION("block mapping with anchored boolean scalar") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: &anchor true\nbar: *anchor")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains("bar"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_anchor());
        REQUIRE(foo_node.has_anchor_name());
        REQUIRE(foo_node.get_anchor_name() == "anchor");
        REQUIRE(foo_node.is_boolean());
        REQUIRE(foo_node.get_value<bool>() == true);

        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_alias());
        REQUIRE(bar_node.has_anchor_name());
        REQUIRE(bar_node.get_anchor_name() == "anchor");
        REQUIRE(bar_node.is_boolean());
        REQUIRE(bar_node.get_value<bool>() == foo_node.get_value<bool>());
    }

    SECTION("block mapping with anchored integer scalar") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: &anchor -123\nbar: *anchor")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains("bar"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_anchor());
        REQUIRE(foo_node.has_anchor_name());
        REQUIRE(foo_node.get_anchor_name() == "anchor");
        REQUIRE(foo_node.is_integer());
        REQUIRE(foo_node.get_value<int>() == -123);

        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_alias());
        REQUIRE(bar_node.has_anchor_name());
        REQUIRE(bar_node.get_anchor_name() == "anchor");
        REQUIRE(bar_node.is_integer());
        REQUIRE(bar_node.get_value<int>() == foo_node.get_value<int>());
    }

    SECTION("block mapping with anchored floating point number scalar") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: &anchor 3.14\nbar: *anchor")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains("bar"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_anchor());
        REQUIRE(foo_node.has_anchor_name());
        REQUIRE(foo_node.get_anchor_name() == "anchor");
        REQUIRE(foo_node.is_float_number());
        REQUIRE(foo_node.get_value<double>() == 3.14);

        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_alias());
        REQUIRE(bar_node.has_anchor_name());
        REQUIRE(bar_node.get_anchor_name() == "anchor");
        REQUIRE(bar_node.is_float_number());
        REQUIRE(bar_node.get_value<double>() == foo_node.get_value<double>());
    }

    SECTION("block mapping with anchored string scalar") {
        REQUIRE_NOTHROW(
            root = deserializer.deserialize(fkyaml::detail::input_adapter("foo: &anchor one\nbar: *anchor")));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.contains("bar"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_anchor());
        REQUIRE(foo_node.has_anchor_name());
        REQUIRE(foo_node.get_anchor_name() == "anchor");
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "one");

        fkyaml::node& bar_node = root["bar"];
        REQUIRE(bar_node.is_alias());
        REQUIRE(bar_node.has_anchor_name());
        REQUIRE(bar_node.get_anchor_name() == "anchor");
        REQUIRE(bar_node.is_string());
        REQUIRE(bar_node.as_str() == foo_node.as_str());
    }

    SECTION("parse alias mapping key") {
        std::string input = "&anchor foo:\n"
                            "  bar: false\n"
                            "  *anchor : 123";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_mapping());
        REQUIRE(foo_node.size() == 2);
        REQUIRE(foo_node.contains("bar"));
        REQUIRE(foo_node.contains("foo"));

        fkyaml::node& foo_bar_node = foo_node["bar"];
        REQUIRE(foo_bar_node.is_boolean());
        REQUIRE(foo_bar_node.get_value<bool>() == false);

        fkyaml::node& foo_foo_node = foo_node["foo"];
        REQUIRE(foo_foo_node.is_integer());
        REQUIRE(foo_foo_node.get_value<int>() == 123);
    }

    SECTION("parse anchored child block sequence") {
        std::string input = "test: &anchor\n"
                            "- foo\n"
                            "- 123";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("test"));

        fkyaml::node& test_node = root["test"];
        REQUIRE(test_node.is_anchor());
        REQUIRE(test_node.has_anchor_name());
        REQUIRE(test_node.get_anchor_name() == "anchor");
        REQUIRE(test_node.is_sequence());
        REQUIRE(test_node.size() == 2);

        fkyaml::node& test_0_node = test_node[0];
        REQUIRE(test_0_node.is_string());
        REQUIRE(test_0_node.as_str() == "foo");

        fkyaml::node& test_1_node = test_node[1];
        REQUIRE(test_1_node.is_integer());
        REQUIRE(test_1_node.get_value<int>() == 123);
    }

    SECTION("anchor for the root block mapping node") {
        std::string input = "&anchor\n"
                            "foo: bar";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.is_anchor());
        REQUIRE(root.has_anchor_name());
        REQUIRE(root.get_anchor_name() == "anchor");

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "bar");
    }

    SECTION("anchor for the root block sequence node") {
        std::string input = "&anchor\n"
                            "- foo: bar";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 1);
        REQUIRE(root.is_anchor());
        REQUIRE(root.has_anchor_name());
        REQUIRE(root.get_anchor_name() == "anchor");

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.is_mapping());
        REQUIRE(root_0_node.size() == 1);
        REQUIRE(root_0_node.contains("foo"));

        fkyaml::node& root_0_foo_node = root_0_node["foo"];
        REQUIRE(root_0_foo_node.is_string());
        REQUIRE(root_0_foo_node.as_str() == "bar");
    }

    SECTION("anchor for the root flow mapping node") {
        std::string input = "&anchor {foo: bar}";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.is_anchor());
        REQUIRE(root.has_anchor_name());
        REQUIRE(root.get_anchor_name() == "anchor");

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "bar");
    }

    SECTION("anchor for the root flow sequence node") {
        std::string input = "&anchor [{foo: bar}]";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 1);
        REQUIRE(root.is_anchor());
        REQUIRE(root.has_anchor_name());
        REQUIRE(root.get_anchor_name() == "anchor");

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.is_mapping());
        REQUIRE(root_0_node.size() == 1);
        REQUIRE(root_0_node.contains("foo"));

        fkyaml::node& root_0_foo_node = root_0_node["foo"];
        REQUIRE(root_0_foo_node.is_string());
        REQUIRE(root_0_foo_node.as_str() == "bar");
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
        REQUIRE(root.contains("int"));
        REQUIRE(root.contains("nil"));
        REQUIRE(root.contains("bool"));
        REQUIRE(root.contains("float"));
        REQUIRE(root.contains("non specific"));
        REQUIRE(root.contains("custom"));
        REQUIRE(root.contains("map"));
        REQUIRE(root.contains("map_flow"));
        REQUIRE(root.contains("seq"));
        REQUIRE(root.contains("seq_flow"));

        fkyaml::node& str_node = root["str"];
        REQUIRE(str_node.has_tag_name());
        REQUIRE(str_node.get_tag_name() == "!!str");
        REQUIRE(str_node.is_string());
        REQUIRE(str_node.as_str() == "true");

        fkyaml::node& int_node = root["int"];
        REQUIRE(int_node.has_tag_name());
        REQUIRE(int_node.get_tag_name() == "!<tag:yaml.org,2002:int>");
        REQUIRE(int_node.is_integer());
        REQUIRE(int_node.get_value<int>() == 123);

        fkyaml::node& nil_node = root["nil"];
        REQUIRE(nil_node.has_tag_name());
        REQUIRE(nil_node.get_tag_name() == "!!null");
        REQUIRE(nil_node.is_null());
        REQUIRE(nil_node.get_value<std::nullptr_t>() == nullptr);

        fkyaml::node& bool_node = root["bool"];
        REQUIRE(bool_node.has_tag_name());
        REQUIRE(bool_node.get_tag_name() == "!!bool");
        REQUIRE(bool_node.is_boolean());
        REQUIRE(bool_node.get_value<bool>() == false);

        fkyaml::node& float_node = root["float"];
        REQUIRE(float_node.has_tag_name());
        REQUIRE(float_node.get_tag_name() == "!!float");
        REQUIRE(float_node.is_float_number());
        REQUIRE(float_node.get_value<double>() == 3.14);

        fkyaml::node& non_specific_node = root["non specific"];
        REQUIRE(non_specific_node.has_tag_name());
        REQUIRE(non_specific_node.get_tag_name() == "!");
        REQUIRE(non_specific_node.is_string());
        REQUIRE(non_specific_node.as_str() == "non specific");

        fkyaml::node& custom_node = root["custom"];
        REQUIRE(custom_node.has_tag_name());
        REQUIRE(custom_node.get_tag_name() == "!local");
        REQUIRE(custom_node.is_string());
        REQUIRE(custom_node.as_str() == "value");

        fkyaml::node& map_node = root["map"];
        REQUIRE(map_node.has_tag_name());
        REQUIRE(map_node.get_tag_name() == "!!map");
        REQUIRE(map_node.is_mapping());
        REQUIRE(map_node.size() == 1);
        REQUIRE(map_node.begin().key().has_tag_name());
        REQUIRE(map_node.begin().key().get_tag_name() == "!!str");
        REQUIRE(map_node.contains("foo"));

        fkyaml::node& map_foo_node = map_node["foo"];
        REQUIRE(map_foo_node.is_string());
        REQUIRE(map_foo_node.as_str() == "bar");

        fkyaml::node& map_flow_node = root["map_flow"];
        REQUIRE(map_flow_node.has_tag_name());
        REQUIRE(map_flow_node.get_tag_name() == "!<tag:yaml.org,2002:map>");
        REQUIRE(map_flow_node.is_mapping());
        REQUIRE(map_flow_node.size() == 1);
        REQUIRE(map_flow_node.contains("foo"));

        fkyaml::node& map_flow_foo_node = map_flow_node["foo"];
        REQUIRE(map_flow_foo_node.is_string());
        REQUIRE(map_flow_foo_node.as_str() == "bar");

        fkyaml::node& seq_node = root["seq"];
        REQUIRE(seq_node.has_tag_name());
        REQUIRE(seq_node.get_tag_name() == "!<tag:yaml.org,2002:seq>");
        REQUIRE(seq_node.is_sequence());
        REQUIRE(seq_node.size() == 2);

        fkyaml::node& seq_0_node = seq_node[0];
        REQUIRE(seq_0_node.is_integer());
        REQUIRE(seq_0_node.get_value<int>() == 123);

        fkyaml::node& seq_1_node = seq_node[1];
        REQUIRE(seq_1_node.is_float_number());
        REQUIRE(seq_1_node.get_value<float>() == 3.14f);

        fkyaml::node& seq_flow_node = root["seq_flow"];
        REQUIRE(seq_flow_node.has_tag_name());
        REQUIRE(seq_flow_node.get_tag_name() == "!!seq");
        REQUIRE(seq_flow_node.is_sequence());
        REQUIRE(seq_flow_node.size() == 2);

        fkyaml::node& seq_flow_0_node = seq_flow_node[0];
        REQUIRE(seq_flow_0_node.is_integer());
        REQUIRE(seq_flow_0_node.get_value<int>() == 123);

        fkyaml::node& seq_flow_1_node = seq_flow_node[1];
        REQUIRE(seq_flow_1_node.is_float_number());
        REQUIRE(seq_flow_1_node.get_value<float>() == 3.14f);
    }

    SECTION("valid tags for block sequence/mapping") {
        std::string input = "seq: !!seq\n"
                            "- !!bool true\n"
                            "- !!seq\n"
                            " - !!str true\n"
                            "map: !!map\n"
                            " foo: bar";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 2);
        REQUIRE(root.contains("seq"));
        REQUIRE(root.contains("map"));

        fkyaml::node& seq_node = root["seq"];
        REQUIRE(seq_node.has_tag_name());
        REQUIRE(seq_node.get_tag_name() == "!!seq");
        REQUIRE(seq_node.is_sequence());
        REQUIRE(seq_node.size() == 2);

        fkyaml::node& seq_0_node = seq_node[0];
        REQUIRE(seq_0_node.has_tag_name());
        REQUIRE(seq_0_node.get_tag_name() == "!!bool");
        REQUIRE(seq_0_node.is_boolean());
        REQUIRE(seq_0_node.get_value<bool>() == true);

        fkyaml::node& seq_1_node = seq_node[1];
        REQUIRE(seq_1_node.has_tag_name());
        REQUIRE(seq_1_node.get_tag_name() == "!!seq");
        REQUIRE(seq_1_node.is_sequence());
        REQUIRE(seq_1_node.size() == 1);

        fkyaml::node& seq_1_0_node = seq_1_node[0];
        REQUIRE(seq_1_0_node.has_tag_name());
        REQUIRE(seq_1_0_node.get_tag_name() == "!!str");
        REQUIRE(seq_1_0_node.is_string());
        REQUIRE(seq_1_0_node.as_str() == "true");

        fkyaml::node& map_node = root["map"];
        REQUIRE(map_node.has_tag_name());
        REQUIRE(map_node.get_tag_name() == "!!map");
        REQUIRE(map_node.is_mapping());
        REQUIRE(map_node.size() == 1);
        REQUIRE(map_node.contains("foo"));

        // Make sure that the !!map tag is not applied to the child mapping key.
        REQUIRE_FALSE(map_node.begin().value().has_tag_name());

        fkyaml::node& map_foo_node = map_node["foo"];
        REQUIRE(map_foo_node.is_string());
        REQUIRE(map_foo_node.as_str() == "bar");
    }

    SECTION("specify tags using TAG directives") {
        std::string input = "%TAG !e! tag:example.com,2000:app/\n"
                            "---\n"
                            "- !e!foo \"bar\"";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 1);

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.has_tag_name());
        REQUIRE(root_0_node.get_tag_name() == "!e!foo");
        REQUIRE(root_0_node.is_string());
        REQUIRE(root_0_node.as_str() == "bar");
    }

    SECTION("tag for the root block mapping node") {
        std::string input = "!!map\n"
                            "foo: bar";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.has_tag_name());
        REQUIRE(root.get_tag_name() == "!!map");

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "bar");
    }

    SECTION("tag for the root block sequence node") {
        std::string input = "!!seq\n"
                            "- foo: bar";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 1);
        REQUIRE(root.has_tag_name());
        REQUIRE(root.get_tag_name() == "!!seq");

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.is_mapping());
        REQUIRE(root_0_node.size() == 1);
        REQUIRE(root_0_node.contains("foo"));

        fkyaml::node& root_0_foo_node = root_0_node["foo"];
        REQUIRE(root_0_foo_node.is_string());
        REQUIRE(root_0_foo_node.as_str() == "bar");
    }

    SECTION("tag for the root flow mapping node") {
        std::string input = "!!map {foo: bar}";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.has_tag_name());
        REQUIRE(root.get_tag_name() == "!!map");

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "bar");
    }

    SECTION("tag for the root flow sequence node") {
        std::string input = "!!seq [{foo: bar}]";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 1);
        REQUIRE(root.has_tag_name());
        REQUIRE(root.get_tag_name() == "!!seq");

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.is_mapping());
        REQUIRE(root_0_node.size() == 1);
        REQUIRE(root_0_node.contains("foo"));

        fkyaml::node& root_0_foo_node = root_0_node["foo"];
        REQUIRE(root_0_foo_node.is_string());
        REQUIRE(root_0_foo_node.as_str() == "bar");
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

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_mapping());
        REQUIRE(foo_node.has_anchor_name());
        REQUIRE(foo_node.get_anchor_name() == "anchor");
        REQUIRE(foo_node.has_tag_name());
        REQUIRE(foo_node.get_tag_name() == "!!map");
        REQUIRE(foo_node.size() == 1);
        REQUIRE(foo_node.contains("bar"));

        fkyaml::node& foo_bar_node = foo_node["bar"];
        REQUIRE(foo_bar_node.is_string());
        REQUIRE(foo_bar_node.as_str() == "baz");
    }

    SECTION("alias node with tag") {
        std::string input = "&anchor foo: !!str *anchor";
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("alias node with anchor") {
        std::string input = "&anchor foo: &anchor2 *anchor";
        REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("parse anchored child block mapping as a block sequence entry") {
        std::string input = "values:\n"
                            "- &anchor !XXX\n"
                            "  source: !YYY\n"
                            "    name: foo\n"
                            "  include: false";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("values"));

        fkyaml::node& values_node = root["values"];
        REQUIRE(values_node.is_sequence());
        REQUIRE(values_node.size() == 1);

        fkyaml::node& values_0_node = values_node[0];
        REQUIRE(values_0_node.is_mapping());
        REQUIRE(values_0_node.size() == 2);
        REQUIRE(values_0_node.contains("source"));
        REQUIRE(values_0_node.contains("include"));
        REQUIRE(values_0_node.has_anchor_name());
        REQUIRE(values_0_node.get_anchor_name() == "anchor");
        REQUIRE(values_0_node.has_tag_name());
        REQUIRE(values_0_node.get_tag_name() == "!XXX");

        fkyaml::node& values_0_source_node = values_0_node["source"];
        REQUIRE(values_0_source_node.is_mapping());
        REQUIRE(values_0_source_node.size() == 1);
        REQUIRE(values_0_source_node.contains("name"));
        REQUIRE(values_0_source_node.has_tag_name());
        REQUIRE(values_0_source_node.get_tag_name() == "!YYY");

        fkyaml::node& values_0_source_name_node = values_0_source_node["name"];
        REQUIRE(values_0_source_name_node.is_string());
        REQUIRE(values_0_source_name_node.as_str() == "foo");

        fkyaml::node& values_0_include_node = values_0_node["include"];
        REQUIRE(values_0_include_node.is_boolean());
        REQUIRE(values_0_include_node.get_value<bool>() == false);
    }

    SECTION("anchor and tag for the root block mapping node") {
        std::string input = "&anchor !!map\n"
                            "foo: bar";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.is_anchor());
        REQUIRE(root.has_anchor_name());
        REQUIRE(root.get_anchor_name() == "anchor");
        REQUIRE(root.has_tag_name());
        REQUIRE(root.get_tag_name() == "!!map");

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "bar");
    }

    SECTION("anchor and tag for the root block sequence node") {
        std::string input = "&anchor !!seq\n"
                            "- foo: bar";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 1);
        REQUIRE(root.is_anchor());
        REQUIRE(root.has_anchor_name());
        REQUIRE(root.get_anchor_name() == "anchor");
        REQUIRE(root.has_tag_name());
        REQUIRE(root.get_tag_name() == "!!seq");

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.is_mapping());
        REQUIRE(root_0_node.size() == 1);
        REQUIRE(root_0_node.contains("foo"));

        fkyaml::node& root_0_foo_node = root_0_node["foo"];
        REQUIRE(root_0_foo_node.is_string());
        REQUIRE(root_0_foo_node.as_str() == "bar");
    }

    SECTION("anchor and tag for the root flow mapping node") {
        std::string input = "&anchor !!map {foo: bar}";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.is_anchor());
        REQUIRE(root.has_anchor_name());
        REQUIRE(root.get_anchor_name() == "anchor");
        REQUIRE(root.has_tag_name());
        REQUIRE(root.get_tag_name() == "!!map");

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "bar");
    }

    SECTION("anchor and tag for the root flow sequence node") {
        std::string input = "&anchor !!seq [{foo: bar}]";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_sequence());
        REQUIRE(root.size() == 1);
        REQUIRE(root.is_anchor());
        REQUIRE(root.has_anchor_name());
        REQUIRE(root.get_anchor_name() == "anchor");
        REQUIRE(root.has_tag_name());
        REQUIRE(root.get_tag_name() == "!!seq");

        fkyaml::node& root_0_node = root[0];
        REQUIRE(root_0_node.is_mapping());
        REQUIRE(root_0_node.size() == 1);
        REQUIRE(root_0_node.contains("foo"));

        fkyaml::node& root_0_foo_node = root_0_node["foo"];
        REQUIRE(root_0_foo_node.is_string());
        REQUIRE(root_0_foo_node.as_str() == "bar");
    }

    SECTION("anchor and tag for the root block mapping node with the end-of-directives marker") {
        std::string input = "--- &anchor !!map\n"
                            "foo: bar";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));

        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.contains("foo"));
        REQUIRE(root.is_anchor());
        REQUIRE(root.has_anchor_name());
        REQUIRE(root.get_anchor_name() == "anchor");
        REQUIRE(root.has_tag_name());
        REQUIRE(root.get_tag_name() == "!!map");

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "bar");
    }
}

TEST_CASE("Deserializer_NoMachingAnchor") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    REQUIRE_THROWS_AS(deserializer.deserialize(fkyaml::detail::input_adapter("foo: *anchor")), fkyaml::parse_error);
}

TEST_CASE("Deserializer_DocumentWithMarkers") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;

    SECTION("valid YAML document") {
        std::string input = "%YAML 1.2\n"
                            "---\n"
                            "foo: one\n"
                            "...";
        REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
        REQUIRE(root.is_mapping());
        REQUIRE(root.size() == 1);
        REQUIRE(root.get_yaml_version_type() == fkyaml::yaml_version_type::VERSION_1_2);
        REQUIRE(root.contains("foo"));

        fkyaml::node& foo_node = root["foo"];
        REQUIRE(foo_node.is_string());
        REQUIRE(foo_node.as_str() == "one");
    }

    SECTION("invalid directives end marker(---) in a flow collection") {
        std::string input = "[\n"
                            "---\n"
                            "]";
        REQUIRE_THROWS_AS(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }

    SECTION("invalid document end marker(...) in a flow collection") {
        std::string input = "[\n"
                            "...\n"
                            "]";
        REQUIRE_THROWS_AS(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)), fkyaml::parse_error);
    }
}

TEST_CASE("Deserializer_MultipleDocuments") {
    fkyaml::detail::basic_deserializer<fkyaml::node> deserializer;
    fkyaml::node root;
    std::vector<fkyaml::node> docs;

    SECTION("both directives/document end markers") {
        std::string input = "%YAML 1.1\n"
                            "---\n"
                            "foo: 123\n"
                            "...\n"
                            "%TAG ! tag:com.example,2024:\n"
                            "---\n"
                            "- !foo bar\n"
                            "- 3.14\n"
                            "- Null";

        SECTION("parse only the first document") {
            REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
            REQUIRE(root.is_mapping());
            REQUIRE(root.size() == 1);
            REQUIRE(root.contains("foo"));

            fkyaml::node& foo_node = root["foo"];
            REQUIRE(foo_node.is_integer());
            REQUIRE(foo_node.get_value<int>() == 123);
        }

        SECTION("parse all documents") {
            REQUIRE_NOTHROW(docs = deserializer.deserialize_docs(fkyaml::detail::input_adapter(input)));
            REQUIRE(docs.size() == 2);

            fkyaml::node& root0 = docs[0];
            REQUIRE(root0.is_mapping());
            REQUIRE(root0.size() == 1);
            REQUIRE(root0.contains("foo"));

            fkyaml::node& foo_node = root0["foo"];
            REQUIRE(foo_node.is_integer());
            REQUIRE(foo_node.get_value<int>() == 123);

            fkyaml::node& root1 = docs[1];
            REQUIRE(root1.is_sequence());
            REQUIRE(root1.size() == 3);

            fkyaml::node& seq0 = root1[0];
            REQUIRE(seq0.has_tag_name());
            REQUIRE(seq0.get_tag_name() == "!foo");
            REQUIRE(seq0.is_string());
            REQUIRE(seq0.as_str() == "bar");

            fkyaml::node& seq1 = root1[1];
            REQUIRE(seq1.is_float_number());
            REQUIRE(seq1.get_value<double>() == 3.14);

            fkyaml::node& seq2 = root1[2];
            REQUIRE(seq2.is_null());
        }
    }

    SECTION("document end marker omitted") {
        std::string input = "--- !!map\n"
                            "? a\n"
                            ": b\n"
                            "--- !!seq\n"
                            "- !!str c\n"
                            "--- !!map\n"
                            "d: e";

        SECTION("parse only the first document") {
            REQUIRE_NOTHROW(root = deserializer.deserialize(fkyaml::detail::input_adapter(input)));
            REQUIRE(root.is_mapping());
            REQUIRE(root.size() == 1);
            REQUIRE(root.contains("a"));
            REQUIRE(root.has_tag_name());
            REQUIRE(root.get_tag_name() == "!!map");

            fkyaml::node& a_node = root["a"];
            REQUIRE(a_node.is_string());
            REQUIRE(a_node.as_str() == "b");
        }

        SECTION("parse all documents") {
            REQUIRE_NOTHROW(docs = deserializer.deserialize_docs(fkyaml::detail::input_adapter(input)));
            REQUIRE(docs.size() == 3);

            fkyaml::node& root0 = docs[0];
            REQUIRE(root0.is_mapping());
            REQUIRE(root0.size() == 1);
            REQUIRE(root0.contains("a"));
            REQUIRE(root0.has_tag_name());
            REQUIRE(root0.get_tag_name() == "!!map");

            fkyaml::node& a_node = root0["a"];
            REQUIRE(a_node.is_string());
            REQUIRE(a_node.as_str() == "b");

            fkyaml::node& root1 = docs[1];
            REQUIRE(root1.is_sequence());
            REQUIRE(root1.size() == 1);
            REQUIRE(root1.has_tag_name());
            REQUIRE(root1.get_tag_name() == "!!seq");

            fkyaml::node& seq0_node = root1[0];
            REQUIRE(seq0_node.is_string());
            REQUIRE(seq0_node.as_str() == "c");
            REQUIRE(seq0_node.has_tag_name());
            REQUIRE(seq0_node.get_tag_name() == "!!str");

            fkyaml::node& root2 = docs[2];
            REQUIRE(root2.is_mapping());
            REQUIRE(root2.size() == 1);
            REQUIRE(root2.contains("d"));
            REQUIRE(root2.has_tag_name());
            REQUIRE(root2.get_tag_name() == "!!map");

            fkyaml::node& d_node = root2["d"];
            REQUIRE(d_node.is_string());
            REQUIRE(d_node.as_str() == "e");
        }
    }
}
