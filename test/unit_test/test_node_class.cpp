//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.6
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <cmath>
#include <cfloat>
#include <fstream>
#include <sstream>
#include <map>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

// generated in test/unit_test/CMakeLists.txt
#include <test_data.hpp>

#ifdef FK_YAML_HAS_CXX_17
#include <string_view>
#endif

//
// test cases for constructors
//

TEST_CASE("Node_DefaultCtor") {
    fkyaml::node node;
    REQUIRE(node.is_null());
}

TEST_CASE("Node_SequenceTypeCtor") {
    fkyaml::node node(fkyaml::node::node_t::SEQUENCE);
    REQUIRE(node.is_sequence());
    REQUIRE(node.size() == 0);
}

TEST_CASE("Node_MappingTypeCtor") {
    fkyaml::node node(fkyaml::node::node_t::MAPPING);
    REQUIRE(node.is_mapping());
    REQUIRE(node.size() == 0);
}

TEST_CASE("Node_NullTypeCtor") {
    fkyaml::node node(fkyaml::node::node_t::NULL_OBJECT);
    REQUIRE(node.is_null());
}

TEST_CASE("Node_BooleanTypeCtor") {
    fkyaml::node node(fkyaml::node::node_t::BOOLEAN);
    REQUIRE(node.is_boolean());
    REQUIRE(node.get_value_ref<fkyaml::node::boolean_type&>() == false);
}

TEST_CASE("Node_IntegerTypeCtor") {
    fkyaml::node node(fkyaml::node::node_t::INTEGER);
    REQUIRE(node.is_integer());
    REQUIRE(node.get_value_ref<fkyaml::node::integer_type&>() == 0);
}

TEST_CASE("Node_FloatNumberTypeCtor") {
    fkyaml::node node(fkyaml::node::node_t::FLOAT_NUMBER);
    REQUIRE(node.is_float_number());
    REQUIRE(node.get_value_ref<fkyaml::node::float_number_type&>() == 0.0);
}

TEST_CASE("Node_StringTypeCtor") {
    fkyaml::node node(fkyaml::node::node_t::STRING);
    REQUIRE(node.is_string());
    REQUIRE(node.size() == 0);
}

TEST_CASE("Node_ThrowingSpecializationTypeCtor") {
    struct String {
        String() {
            throw fkyaml::exception();
        }
    };

    using NodeType = fkyaml::basic_node<std::vector, std::map, bool, int64_t, double, String>;
    REQUIRE_THROWS_AS(NodeType(NodeType::node_t::STRING), fkyaml::exception);
}

TEST_CASE("Node_SequenceCtor") {
    fkyaml::node node(fkyaml::node::sequence_type {fkyaml::node(true), fkyaml::node(false)});
    REQUIRE(node.type() == fkyaml::node::node_t::SEQUENCE);
    REQUIRE(node.is_sequence());
    REQUIRE(node.size() == 2);
    REQUIRE(node[0].is_boolean());
    REQUIRE(node[0].get_value_ref<fkyaml::node::boolean_type&>() == true);
    REQUIRE(node[1].is_boolean());
    REQUIRE(node[1].get_value_ref<fkyaml::node::boolean_type&>() == false);
}

TEST_CASE("Node_MappingCtor") {
    fkyaml::node node(fkyaml::node::mapping_type {{"test", fkyaml::node(true)}});
    REQUIRE(node.type() == fkyaml::node::node_t::MAPPING);
    REQUIRE(node.is_mapping());
    REQUIRE(node.size() == 1);
    REQUIRE(node.contains("test"));
    REQUIRE(node["test"].is_boolean());
    REQUIRE(node["test"].get_value_ref<fkyaml::node::boolean_type&>() == true);
}

TEST_CASE("Node_NullCtor") {
    fkyaml::node node(nullptr);
    REQUIRE(node.type() == fkyaml::node::node_t::NULL_OBJECT);
    REQUIRE(node.is_null());
}

TEST_CASE("Node_BooleanCtor") {
    fkyaml::node node(true);
    REQUIRE(node.type() == fkyaml::node::node_t::BOOLEAN);
    REQUIRE(node.is_boolean());
    REQUIRE(node.get_value_ref<fkyaml::node::boolean_type&>() == true);
}

TEST_CASE("Node_IntegerCtor") {
    fkyaml::node node(23467);
    REQUIRE(node.type() == fkyaml::node::node_t::INTEGER);
    REQUIRE(node.is_integer());
    REQUIRE(node.get_value_ref<fkyaml::node::integer_type&>() == 23467);
}

TEST_CASE("Node_FloatNumberCtor") {
    fkyaml::node node(3.14);
    REQUIRE(node.type() == fkyaml::node::node_t::FLOAT_NUMBER);
    REQUIRE(node.is_float_number());
    REQUIRE(node.get_value_ref<fkyaml::node::float_number_type&>() == 3.14);
}

TEST_CASE("Node_StringCtor") {
    auto node = GENERATE(fkyaml::node(std::string("test")));
    REQUIRE(node.type() == fkyaml::node::node_t::STRING);
    REQUIRE(node.is_string());
    REQUIRE(node.size() == 4);
    REQUIRE(node.get_value_ref<fkyaml::node::string_type&>() == "test");
}

#ifdef FK_YAML_HAS_CXX_17
TEST_CASE("Node_StringViewCtor") {
    using namespace std::string_view_literals;
    auto node = fkyaml::node("test"sv);
    REQUIRE(node.type() == fkyaml::node::node_t::STRING);
    REQUIRE(node.is_string());
    REQUIRE(node.size() == 4);
    REQUIRE(node.get_value_ref<fkyaml::node::string_type&>() == "test");
}
#endif

TEST_CASE("Node_SequenceCopyCtor") {
    fkyaml::node n = "test";

    fkyaml::node copied = {true, "test"};
    fkyaml::node node(copied);
    REQUIRE(node.is_sequence());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 2);
    REQUIRE_NOTHROW(node[0]);
    REQUIRE(node[0].is_boolean());
    REQUIRE_NOTHROW(node[0].get_value_ref<fkyaml::node::boolean_type&>());
    REQUIRE(node[0].get_value_ref<fkyaml::node::boolean_type&>() == true);
    REQUIRE_NOTHROW(node[1]);
    REQUIRE(node[1].is_string());
    REQUIRE_NOTHROW(node[1].get_value_ref<fkyaml::node::string_type&>());
    REQUIRE_NOTHROW(node[1].get_value_ref<fkyaml::node::string_type&>().size());
    REQUIRE(node[1].get_value_ref<fkyaml::node::string_type&>().size() == 4);
    REQUIRE(node[1].get_value_ref<fkyaml::node::string_type&>().compare("test") == 0);
}

TEST_CASE("Node_MappingCopyCtor") {
    fkyaml::node copied = {{"test0", 123}, {"test1", 3.14}};
    fkyaml::node node(copied);
    REQUIRE(node.is_mapping());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 2);
    REQUIRE_NOTHROW(node["test0"]);
    REQUIRE(node["test0"].is_integer());
    REQUIRE_NOTHROW(node["test0"].get_value_ref<fkyaml::node::integer_type&>());
    REQUIRE(node["test0"].get_value_ref<fkyaml::node::integer_type&>() == 123);
    REQUIRE_NOTHROW(node["test1"]);
    REQUIRE(node["test1"].is_float_number());
    REQUIRE_NOTHROW(node["test1"].get_value_ref<fkyaml::node::float_number_type&>());
    REQUIRE(node["test1"].get_value_ref<fkyaml::node::float_number_type&>() == 3.14);
}

TEST_CASE("Node_NullCopyCtor") {
    fkyaml::node copied;
    fkyaml::node node(copied);
    REQUIRE(node.is_null());
}

TEST_CASE("Node_BooleanCopyCtor") {
    fkyaml::node copied = true;
    fkyaml::node node(copied);
    REQUIRE(node.is_boolean());
    REQUIRE_NOTHROW(node.get_value_ref<fkyaml::node::boolean_type&>());
    REQUIRE(node.get_value_ref<fkyaml::node::boolean_type&>() == true);
}

TEST_CASE("Node_IntegerCopyCtor") {
    fkyaml::node copied = 123;
    fkyaml::node node(copied);
    REQUIRE(node.is_integer());
    REQUIRE_NOTHROW(node.get_value_ref<fkyaml::node::integer_type&>());
    REQUIRE(node.get_value_ref<fkyaml::node::integer_type&>() == 123);
}

TEST_CASE("Node_FloatNumberCopyCtor") {
    fkyaml::node copied = 3.14;
    fkyaml::node node(copied);
    REQUIRE(node.is_float_number());
    REQUIRE_NOTHROW(node.get_value_ref<fkyaml::node::float_number_type&>());
    REQUIRE(node.get_value_ref<fkyaml::node::float_number_type&>() == 3.14);
}

TEST_CASE("Node_StringCopyCtor") {
    fkyaml::node copied = "test";
    fkyaml::node node(copied);
    REQUIRE(node.is_string());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 4);
    REQUIRE_NOTHROW(node.get_value_ref<fkyaml::node::string_type&>());
    REQUIRE(node.get_value_ref<fkyaml::node::string_type&>().compare("test") == 0);
}

TEST_CASE("Node_AliasCopyCtor") {
    fkyaml::node tmp = true;
    tmp.add_anchor_name("anchor_name");
    fkyaml::node tmp_alias = fkyaml::node::alias_of(tmp);
    fkyaml::node alias(tmp_alias);
    REQUIRE(alias.is_boolean());
    REQUIRE_NOTHROW(alias.get_value_ref<fkyaml::node::boolean_type&>());
    REQUIRE(alias.get_value_ref<fkyaml::node::boolean_type&>() == true);
}

TEST_CASE("Node_SequenceMoveCtor") {
    fkyaml::node moved = {true, "test"};
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_sequence());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 2);
    REQUIRE_NOTHROW(node[0]);
    REQUIRE(node[0].is_boolean());
    REQUIRE_NOTHROW(node[0].get_value_ref<fkyaml::node::boolean_type&>());
    REQUIRE(node[0].get_value_ref<fkyaml::node::boolean_type&>() == true);
    REQUIRE_NOTHROW(node[1]);
    REQUIRE(node[1].is_string());
    REQUIRE_NOTHROW(node[1].get_value_ref<fkyaml::node::string_type&>());
    REQUIRE_NOTHROW(node[1].get_value_ref<fkyaml::node::string_type&>().size());
    REQUIRE(node[1].get_value_ref<fkyaml::node::string_type&>().size() == 4);
    REQUIRE(node[1].get_value_ref<fkyaml::node::string_type&>().compare("test") == 0);
}

TEST_CASE("Node_MappingMoveCtor") {
    fkyaml::node moved = {{"test0", 123}, {"test1", 3.14}};
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_mapping());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 2);
    REQUIRE_NOTHROW(node["test0"]);
    REQUIRE(node["test0"].is_integer());
    REQUIRE_NOTHROW(node["test0"].get_value_ref<fkyaml::node::integer_type&>());
    REQUIRE(node["test0"].get_value_ref<fkyaml::node::integer_type&>() == 123);
    REQUIRE_NOTHROW(node["test1"]);
    REQUIRE(node["test1"].is_float_number());
    REQUIRE_NOTHROW(node["test1"].get_value_ref<fkyaml::node::float_number_type&>());
    REQUIRE(node["test1"].get_value_ref<fkyaml::node::float_number_type&>() == 3.14);
}

TEST_CASE("Node_NullMoveCtor") {
    fkyaml::node moved;
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_null());
}

TEST_CASE("Node_BooleanMoveCtor") {
    fkyaml::node moved = true;
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_boolean());
    REQUIRE_NOTHROW(node.get_value_ref<fkyaml::node::boolean_type&>());
    REQUIRE(node.get_value_ref<fkyaml::node::boolean_type&>() == true);
}

TEST_CASE("Node_IntegerMoveCtor") {
    fkyaml::node moved = 123;
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_integer());
    REQUIRE_NOTHROW(node.get_value_ref<fkyaml::node::integer_type&>());
    REQUIRE(node.get_value_ref<fkyaml::node::integer_type&>() == 123);
}

TEST_CASE("Node_FloatNumberMoveCtor") {
    fkyaml::node moved = 3.14;
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_float_number());
    REQUIRE_NOTHROW(node.get_value_ref<fkyaml::node::float_number_type&>());
    REQUIRE(node.get_value_ref<fkyaml::node::float_number_type&>() == 3.14);
}

TEST_CASE("Node_StringMoveCtor") {
    fkyaml::node moved = "test";
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_string());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 4);
    REQUIRE_NOTHROW(node.get_value_ref<fkyaml::node::string_type&>());
    REQUIRE(node.get_value_ref<fkyaml::node::string_type&>().compare("test") == 0);
}

TEST_CASE("Node_AliasMoveCtor") {
    fkyaml::node tmp = true;
    tmp.add_anchor_name("anchor_name");
    fkyaml::node tmp_alias = fkyaml::node::alias_of(tmp);
    fkyaml::node alias(std::move(tmp_alias));
    REQUIRE(alias.is_boolean());
    REQUIRE_NOTHROW(alias.get_value_ref<fkyaml::node::boolean_type&>());
    REQUIRE(alias.get_value_ref<fkyaml::node::boolean_type&>() == true);
}

TEST_CASE("Node_InitializerListCtor") {
    fkyaml::node node = {
        {"foo", 3.14},
        {true, 123},
        {567, {true, false}},
        {{true, 1.57}, nullptr},
        {{{"bar", nullptr}}, {{"baz", "qux"}}}};

    REQUIRE(node.is_mapping());
    REQUIRE(node.size() == 5);

    REQUIRE(node.contains("foo"));
    REQUIRE(node["foo"].is_float_number());
    REQUIRE(node["foo"].get_value_ref<fkyaml::node::float_number_type&>() == 3.14);

    REQUIRE(node.contains(true));
    REQUIRE(node[true].is_integer());
    REQUIRE(node[true].get_value_ref<fkyaml::node::integer_type&>() == 123);

    REQUIRE(node.contains(567));
    REQUIRE(node[567].is_sequence());
    REQUIRE(node[567].size() == 2);
    REQUIRE(node[567][0].is_boolean());
    REQUIRE(node[567][0].get_value_ref<fkyaml::node::boolean_type&>() == true);
    REQUIRE(node[567][1].is_boolean());
    REQUIRE(node[567][1].get_value_ref<fkyaml::node::boolean_type&>() == false);

    REQUIRE(node.contains(fkyaml::node {true, 1.57}));
    REQUIRE(node[fkyaml::node {true, 1.57}].is_null());

    REQUIRE(node.contains(fkyaml::node {{"bar", nullptr}}));
    REQUIRE(node[fkyaml::node {{"bar", nullptr}}].is_mapping());
    REQUIRE(node[fkyaml::node {{"bar", nullptr}}].size() == 1);
    REQUIRE(node[fkyaml::node {{"bar", nullptr}}].contains("baz"));
    REQUIRE(node[fkyaml::node {{"bar", nullptr}}]["baz"].is_string());
    REQUIRE(node[fkyaml::node {{"bar", nullptr}}]["baz"].get_value_ref<fkyaml::node::string_type&>() == "qux");
}

//
// test cases for serialization/deserialization features
//

TEST_CASE("Node_Deserialize") {
    char source[] = "foo: bar";
    std::stringstream ss;
    ss << source;

    fkyaml::node node = GENERATE_REF(
        fkyaml::node::deserialize("foo: bar"),
        fkyaml::node::deserialize(source),
        fkyaml::node::deserialize(&source[0], &source[8]),
        fkyaml::node::deserialize(std::string(source)),
        fkyaml::node::deserialize(ss));

    REQUIRE(node.is_mapping());
    REQUIRE(node.size() == 1);
    REQUIRE(node.contains("foo"));
    REQUIRE(node["foo"].is_string());
    REQUIRE(node["foo"].get_value_ref<std::string&>() == "bar");
}

TEST_CASE("Node_Serialize") {
    fkyaml::node node = fkyaml::node::deserialize("foo: bar");
    REQUIRE(fkyaml::node::serialize(node) == "foo: bar\n");
}

TEST_CASE("Node_InsertionOperator") {
    fkyaml::node node = {{"foo", 123}, {"bar", nullptr}, {"baz", true}};
    std::stringstream ss;
    ss << node;

    REQUIRE(ss.str() == "bar: null\nbaz: true\nfoo: 123\n");
}

TEST_CASE("Node_ExtractionOperator") {
    fkyaml::node node;
    std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/extraction_operator_test_data.yml");
    ifs >> node;

    REQUIRE(node.is_mapping());
    REQUIRE(node.size() == 3);
    REQUIRE(node["foo"].get_value<int>() == 123);
    REQUIRE(node["bar"].is_null());
    REQUIRE(node["baz"].get_value<bool>() == true);
}

TEST_CASE("Node_UserDefinedLiteralYaml") {
    SECTION("char sequences literals with using fkyaml::literals") {
        using namespace fkyaml::literals;
        fkyaml::node node = "en: hello\njp: konnichiwa"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].get_value_ref<std::string&>() == "hello");
        REQUIRE(node["jp"].get_value_ref<std::string&>() == "konnichiwa");
    }

    SECTION("char sequences literals with using fkyaml::yaml_literals") {
        using namespace fkyaml::yaml_literals;
        fkyaml::node node = "en: hello\njp: konnichiwa"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].get_value_ref<std::string&>() == "hello");
        REQUIRE(node["jp"].get_value_ref<std::string&>() == "konnichiwa");
    }

    SECTION("char sequences literals with using fkyaml::literals::yaml_literals") {
        using namespace fkyaml::literals::yaml_literals;
        fkyaml::node node = "en: hello\njp: konnichiwa"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].get_value_ref<std::string&>() == "hello");
        REQUIRE(node["jp"].get_value_ref<std::string&>() == "konnichiwa");
    }

    SECTION("char sequences of u8\"\" literals with using fkyaml::literals") {
        using namespace fkyaml::literals;
        fkyaml::node node = u8"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].get_value_ref<std::string&>() == "hello");
        REQUIRE(node["jp"].get_value_ref<std::string&>() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char sequences with u8 literal literals with using fkyaml::yaml_literals") {
        using namespace fkyaml::yaml_literals;
        fkyaml::node node = u8"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].get_value_ref<std::string&>() == "hello");
        REQUIRE(node["jp"].get_value_ref<std::string&>() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char sequences of u8 literal literals with using fkyaml::literals::yaml_literals") {
        using namespace fkyaml::literals::yaml_literals;
        fkyaml::node node = u8"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].get_value_ref<std::string&>() == "hello");
        REQUIRE(node["jp"].get_value_ref<std::string&>() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char16_t sequences with using fkyaml::literals") {
        using namespace fkyaml::literals;
        fkyaml::node node = u"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].get_value_ref<std::string&>() == "hello");
        REQUIRE(node["jp"].get_value_ref<std::string&>() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char16_t sequences with using fkyaml::yaml_literals") {
        using namespace fkyaml::yaml_literals;
        fkyaml::node node = u"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].get_value_ref<std::string&>() == "hello");
        REQUIRE(node["jp"].get_value_ref<std::string&>() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char16_t sequences with using fkyaml::literals::yaml_literals") {
        using namespace fkyaml::literals::yaml_literals;
        fkyaml::node node = u"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].get_value_ref<std::string&>() == "hello");
        REQUIRE(node["jp"].get_value_ref<std::string&>() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char32_t sequences with using fkyaml::literals") {
        using namespace fkyaml::literals;
        fkyaml::node node = U"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].get_value_ref<std::string&>() == "hello");
        REQUIRE(node["jp"].get_value_ref<std::string&>() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char32_t sequences with using fkyaml::yaml_literals") {
        using namespace fkyaml::yaml_literals;
        fkyaml::node node = U"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].get_value_ref<std::string&>() == "hello");
        REQUIRE(node["jp"].get_value_ref<std::string&>() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char32_t sequences with using fkyaml::literals::yaml_literals") {
        using namespace fkyaml::literals::yaml_literals;
        fkyaml::node node = U"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].get_value_ref<std::string&>() == "hello");
        REQUIRE(node["jp"].get_value_ref<std::string&>() == reinterpret_cast<const char*>(u8"こんにちは"));
    }
}

//
// test cases for factory methods
//

TEST_CASE("Node_SequenceNodeFactory") {
    SECTION("empty sequence") {
        fkyaml::node node = fkyaml::node::sequence();
        REQUIRE(node.is_sequence());
        REQUIRE(node.size() == 0);
    }

    SECTION("non-empty sequence") {
        fkyaml::node::sequence_type seq(3);

        SECTION("lvalue sequence") {
            fkyaml::node node = fkyaml::node::sequence(seq);
            REQUIRE(node.is_sequence());
            REQUIRE(node.size() == 3);
            for (int i = 0; i < 3; ++i) {
                REQUIRE(node[i].is_null());
            }
        }

        SECTION("lvalue sequence") {
            fkyaml::node node = fkyaml::node::sequence(std::move(seq));
            REQUIRE(node.is_sequence());
            REQUIRE(node.size() == 3);
            for (int i = 0; i < 3; ++i) {
                REQUIRE(node[i].is_null());
            }
        }
    }
}

TEST_CASE("Node_MappingNodeFactory") {
    SECTION("empty mapping") {
        fkyaml::node node = fkyaml::node::mapping();
        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 0);
    }

    SECTION("non-empty mapping node factory methods.") {
        fkyaml::node::mapping_type map {{"test", true}};

        SECTION("lvalue mapping") {
            fkyaml::node node = fkyaml::node::mapping(map);
            REQUIRE(node.is_mapping());
            REQUIRE(node.size() == 1);
            REQUIRE(node["test"].is_boolean());
            REQUIRE(node["test"].get_value_ref<fkyaml::node::boolean_type&>() == true);
        }

        SECTION("rvalue mapping") {
            fkyaml::node node = fkyaml::node::mapping(std::move(map));
            REQUIRE(node.is_mapping());
            REQUIRE(node.size() == 1);
            REQUIRE(node["test"].is_boolean());
            REQUIRE(node["test"].get_value_ref<fkyaml::node::boolean_type&>() == true);
        }
    }
}

TEST_CASE("Node_BooleanNodeFactory") {
    auto boolean = GENERATE(true, false);
    fkyaml::node node = boolean;
    REQUIRE(node.is_boolean());
    REQUIRE(node.get_value_ref<fkyaml::node::boolean_type&>() == boolean);
}

TEST_CASE("Node_IntegerNodeFactory") {
    auto integer = GENERATE(
        std::numeric_limits<fkyaml::node::integer_type>::min(),
        0,
        std::numeric_limits<fkyaml::node::integer_type>::max());
    fkyaml::node node = integer;
    REQUIRE(node.is_integer());
    REQUIRE(node.get_value_ref<fkyaml::node::integer_type&>() == integer);
}

TEST_CASE("Node_FloatNumberNodeFactory") {
    auto float_val = GENERATE(
        std::numeric_limits<fkyaml::node::float_number_type>::min(),
        3.141592,
        std::numeric_limits<fkyaml::node::float_number_type>::max());
    fkyaml::node node = float_val;
    REQUIRE(node.is_float_number());
    REQUIRE(node.get_value_ref<fkyaml::node::float_number_type&>() == float_val);
}

TEST_CASE("Node_StringNodeFactory") {
    SECTION("empty string") {
        fkyaml::node node = "";
        REQUIRE(node.is_string());
        REQUIRE(node.size() == 0);
    }

    SECTION("lvalue string") {
        fkyaml::node::string_type str("test");
        fkyaml::node node = str;
        REQUIRE(node.is_string());
        REQUIRE(node.size() == str.size());
        REQUIRE(node.get_value_ref<fkyaml::node::string_type&>() == str);
    }

    SECTION("rvalue string") {
        fkyaml::node node = "test";
        REQUIRE(node.is_string());
        REQUIRE(node.size() == 4);
        REQUIRE(node.get_value_ref<fkyaml::node::string_type&>().compare("test") == 0);
    }
}

TEST_CASE("Node_AliasNodeFactory") {
    fkyaml::node anchor = "alias_test";

    SECTION("without anchor name.") {
        REQUIRE_THROWS_AS(fkyaml::node::alias_of(anchor), fkyaml::exception);
    }

    SECTION("with an empty anchor name") {
        anchor.add_anchor_name("");
        REQUIRE_THROWS_AS(fkyaml::node::alias_of(anchor), fkyaml::exception);
    }

    SECTION("with an alias node") {
        anchor.add_anchor_name("anchor");
        fkyaml::node alias = fkyaml::node::alias_of(anchor);
        REQUIRE_THROWS_AS(fkyaml::node::alias_of(alias), fkyaml::exception);
    }

    SECTION("with an anchor node") {
        anchor.add_anchor_name("anchor_name");
        REQUIRE_NOTHROW(fkyaml::node::alias_of(anchor));
        fkyaml::node alias = fkyaml::node::alias_of(anchor);
        REQUIRE(alias.is_string());
        REQUIRE(alias.get_value_ref<fkyaml::node::string_type&>().compare("alias_test") == 0);
    }
}

//
// test cases for subscript operators
//

TEST_CASE("Node_SubscriptOperator") {
    SECTION("mapping") {
        fkyaml::node::mapping_type map {{"test", fkyaml::node()}};

        SECTION("non-const string value") {
            fkyaml::node node = fkyaml::node::mapping(map);

            SECTION("non-const lvalue string") {
                std::string key = "test";
                REQUIRE_NOTHROW(node[key]);
                REQUIRE(node[key].is_null());
            }

            SECTION("non-const rvalue string") {
                REQUIRE_NOTHROW(node["test"]);
                REQUIRE(node["test"].is_null());
            }
        }

        SECTION("const string value") {
            const fkyaml::node node = fkyaml::node::mapping(map);
            std::string key = "test";

            SECTION("const lvalue string") {
                REQUIRE_NOTHROW(node[key]);
            }

            SECTION("const rvalue string") {
                REQUIRE_NOTHROW(node["test"]);
            }
        }

#ifdef FK_YAML_HAS_CXX_17
        SECTION("string view value") {
            std::string_view key = "test";
            REQUIRE(map[key].is_null());
        }
#endif

        SECTION("non-const string node") {
            fkyaml::node node = fkyaml::node::mapping(map);
            fkyaml::node node_key = "test";

            SECTION("non-const lvalue string node") {
                REQUIRE_NOTHROW(node[node_key]);
            }

            SECTION("non-const rvalue string node") {
                REQUIRE_NOTHROW(node[std::move(node_key)]);
            }
        }

        SECTION("const string node") {
            const fkyaml::node node = fkyaml::node::mapping(map);
            fkyaml::node node_key = "test";

            SECTION("non-const lvalue string node") {
                REQUIRE_NOTHROW(node[node_key]);
            }

            SECTION("non-const rvalue string node") {
                REQUIRE_NOTHROW(node[std::move(node_key)]);
            }
        }
    }

    SECTION("sequence") {
        fkyaml::node node = fkyaml::node::sequence();
        node.get_value_ref<fkyaml::node::sequence_type&>().emplace_back();

        SECTION("non-const integer value") {
            REQUIRE_NOTHROW(node[0]);
        }

        SECTION("const integer value") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node[0]);
        }

        SECTION("non-const integer node") {
            REQUIRE_NOTHROW(node[fkyaml::node(0)]);
        }

        SECTION("const integer node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node[fkyaml::node(0)]);
        }

        SECTION("non-const node with a non-integer value") {
            REQUIRE_THROWS_AS(node[fkyaml::node::sequence_type()], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[fkyaml::node::mapping_type()], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[nullptr], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[false], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[0.0], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[""], fkyaml::type_error);
        }

        SECTION("const node with a non-integer value") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node[fkyaml::node::sequence_type()], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[fkyaml::node::mapping_type()], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[nullptr], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[false], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[0.0], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[""], fkyaml::type_error);
        }

        SECTION("non-const node with a non-integer node") {
            REQUIRE_THROWS_AS(node[fkyaml::node::sequence()], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[fkyaml::node::mapping()], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[fkyaml::node()], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[fkyaml::node(false)], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[fkyaml::node(0.0)], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[fkyaml::node("")], fkyaml::type_error);
        }

        SECTION("const node with a non-integer node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node[fkyaml::node::sequence()], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[fkyaml::node::mapping()], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[fkyaml::node()], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[fkyaml::node(false)], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[fkyaml::node(0.0)], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[fkyaml::node("")], fkyaml::type_error);
        }
    }

    SECTION("scalar") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));
        fkyaml::node node_key = 0;

        SECTION("non-const node with an integer") {
            REQUIRE_THROWS_AS(node[0], fkyaml::type_error);
        }

        SECTION("const node with an integer") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node[0], fkyaml::type_error);
        }

        SECTION("non-const node with an integer node") {
            REQUIRE_THROWS_AS(node[node_key], fkyaml::type_error);
        }

        SECTION("const node with an integer node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node[node_key], fkyaml::type_error);
        }
    }
}

//
// test cases for operators for comparisons between nodes
//

TEST_CASE("Node_EqualToOperator") {
    SECTION("The same type and value") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, nullptr},
            fkyaml::node {true, true},
            fkyaml::node {123, 123},
            fkyaml::node {3.14, 3.14},
            fkyaml::node {"foo", "foo"});
        REQUIRE(params[0] == params[1]);
    }

    SECTION("The same type but different values") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {false, 456, "bar"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"baz", 456}, {"qux", false}}},
            fkyaml::node {true, false},
            fkyaml::node {123, 456},
            fkyaml::node {3.14, 1.41},
            fkyaml::node {"foo", "bar"});
        REQUIRE_FALSE(params[0] == params[1]);
    }

    SECTION("Different types") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {{true, 123, "foo"}, nullptr},
            fkyaml::node {{true, 123, "foo"}, true},
            fkyaml::node {{true, 123, "foo"}, 123},
            fkyaml::node {{true, 123, "foo"}, 3.14},
            fkyaml::node {{true, 123, "foo"}, "foo"},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, nullptr},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, true},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 123},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 3.14},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, "foo"},
            fkyaml::node {nullptr, {true, 123, "foo"}},
            fkyaml::node {nullptr, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, true},
            fkyaml::node {nullptr, 123},
            fkyaml::node {nullptr, 3.14},
            fkyaml::node {nullptr, "foo"},
            fkyaml::node {true, {true, 123, "foo"}},
            fkyaml::node {true, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, nullptr},
            fkyaml::node {true, 123},
            fkyaml::node {true, 3.14},
            fkyaml::node {true, "foo"},
            fkyaml::node {123, {true, 123, "foo"}},
            fkyaml::node {123, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {123, nullptr},
            fkyaml::node {123, true},
            fkyaml::node {123, 3.14},
            fkyaml::node {123, "foo"},
            fkyaml::node {3.14, {true, 123, "foo"}},
            fkyaml::node {3.14, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {3.14, nullptr},
            fkyaml::node {3.14, true},
            fkyaml::node {3.14, 123},
            fkyaml::node {3.14, "foo"},
            fkyaml::node {"foo", {true, 123, "foo"}},
            fkyaml::node {"foo", {{"foo", 123}, {"bar", true}}},
            fkyaml::node {"foo", nullptr},
            fkyaml::node {"foo", true},
            fkyaml::node {"foo", 123},
            fkyaml::node {"foo", 3.14});
        REQUIRE_FALSE(params[0] == params[1]);
    }
}

TEST_CASE("Node_NotEqualToOperator") {
    SECTION("The same type and value") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, nullptr},
            fkyaml::node {true, true},
            fkyaml::node {123, 123},
            fkyaml::node {3.14, 3.14},
            fkyaml::node {"foo", "foo"});
        REQUIRE_FALSE(params[0] != params[1]);
    }

    SECTION("The same type but different values") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {false, 456, "bar"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"baz", 456}, {"qux", false}}},
            fkyaml::node {true, false},
            fkyaml::node {123, 456},
            fkyaml::node {3.14, 1.41},
            fkyaml::node {"foo", "bar"});
        REQUIRE(params[0] != params[1]);
    }

    SECTION("Different types") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {{true, 123, "foo"}, nullptr},
            fkyaml::node {{true, 123, "foo"}, true},
            fkyaml::node {{true, 123, "foo"}, 123},
            fkyaml::node {{true, 123, "foo"}, 3.14},
            fkyaml::node {{true, 123, "foo"}, "foo"},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, nullptr},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, true},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 123},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 3.14},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, "foo"},
            fkyaml::node {nullptr, {true, 123, "foo"}},
            fkyaml::node {nullptr, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, true},
            fkyaml::node {nullptr, 123},
            fkyaml::node {nullptr, 3.14},
            fkyaml::node {nullptr, "foo"},
            fkyaml::node {true, {true, 123, "foo"}},
            fkyaml::node {true, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, nullptr},
            fkyaml::node {true, 123},
            fkyaml::node {true, 3.14},
            fkyaml::node {true, "foo"},
            fkyaml::node {123, {true, 123, "foo"}},
            fkyaml::node {123, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {123, nullptr},
            fkyaml::node {123, true},
            fkyaml::node {123, 3.14},
            fkyaml::node {123, "foo"},
            fkyaml::node {3.14, {true, 123, "foo"}},
            fkyaml::node {3.14, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {3.14, nullptr},
            fkyaml::node {3.14, true},
            fkyaml::node {3.14, 123},
            fkyaml::node {3.14, "foo"},
            fkyaml::node {"foo", {true, 123, "foo"}},
            fkyaml::node {"foo", {{"foo", 123}, {"bar", true}}},
            fkyaml::node {"foo", nullptr},
            fkyaml::node {"foo", true},
            fkyaml::node {"foo", 123},
            fkyaml::node {"foo", 3.14});
        REQUIRE(params[0] != params[1]);
    }
}

TEST_CASE("Node_LessThanOperator") {
    SECTION("The same type and value") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, nullptr},
            fkyaml::node {true, true},
            fkyaml::node {123, 123},
            fkyaml::node {3.14, 3.14},
            fkyaml::node {"foo", "foo"});
        REQUIRE_FALSE(params[0] < params[1]);
    }

    SECTION("The same type and the target value is less than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123}, {true, 123, "foo"}},
            fkyaml::node {{{"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {false, true},
            fkyaml::node {123, 456},
            fkyaml::node {3.14, 4.25},
            fkyaml::node {"foo", "qux"});
        REQUIRE(params[0] < params[1]);
    }

    SECTION("The same type but the target value is greater than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"bar", true}}},
            fkyaml::node {true, false},
            fkyaml::node {456, 123},
            fkyaml::node {4.25, 3.14},
            fkyaml::node {"qux", "foo"});
        REQUIRE_FALSE(params[0] < params[1]);
    }

    SECTION("The numeric value of the target type is less than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {{true, 123, "foo"}, nullptr},
            fkyaml::node {{true, 123, "foo"}, true},
            fkyaml::node {{true, 123, "foo"}, 123},
            fkyaml::node {{true, 123, "foo"}, 3.14},
            fkyaml::node {{true, 123, "foo"}, "foo"},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, nullptr},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, true},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 123},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 3.14},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, "foo"},
            fkyaml::node {nullptr, true},
            fkyaml::node {nullptr, 123},
            fkyaml::node {nullptr, 3.14},
            fkyaml::node {nullptr, "foo"},
            fkyaml::node {true, 123},
            fkyaml::node {true, 3.14},
            fkyaml::node {true, "foo"},
            fkyaml::node {123, 3.14},
            fkyaml::node {123, "foo"},
            fkyaml::node {3.14, "foo"});
        REQUIRE(params[0] < params[1]);
    }

    SECTION("The numeric value of the target type is greater than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {true, 123, "foo"}},
            fkyaml::node {nullptr, {true, 123, "foo"}},
            fkyaml::node {nullptr, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, {true, 123, "foo"}},
            fkyaml::node {true, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, nullptr},
            fkyaml::node {123, {true, 123, "foo"}},
            fkyaml::node {123, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {123, nullptr},
            fkyaml::node {123, true},
            fkyaml::node {3.14, {true, 123, "foo"}},
            fkyaml::node {3.14, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {3.14, nullptr},
            fkyaml::node {3.14, true},
            fkyaml::node {3.14, 123},
            fkyaml::node {"foo", {true, 123, "foo"}},
            fkyaml::node {"foo", {{"foo", 123}, {"bar", true}}},
            fkyaml::node {"foo", nullptr},
            fkyaml::node {"foo", true},
            fkyaml::node {"foo", 123},
            fkyaml::node {"foo", 3.14});
        REQUIRE_FALSE(params[0] < params[1]);
    }
}

TEST_CASE("Node_LessThanOrEqualToOperator") {
    SECTION("The same type and value") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, nullptr},
            fkyaml::node {true, true},
            fkyaml::node {123, 123},
            fkyaml::node {3.14, 3.14},
            fkyaml::node {"foo", "foo"});
        REQUIRE(params[0] <= params[1]);
    }

    SECTION("The same type and the target value is less than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123}, {true, 123, "foo"}},
            fkyaml::node {{{"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {false, true},
            fkyaml::node {123, 456},
            fkyaml::node {3.14, 4.25},
            fkyaml::node {"foo", "qux"});
        REQUIRE(params[0] <= params[1]);
    }

    SECTION("The same type but the target value is greater than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"bar", true}}},
            fkyaml::node {true, false},
            fkyaml::node {456, 123},
            fkyaml::node {4.25, 3.14},
            fkyaml::node {"qux", "foo"});
        REQUIRE_FALSE(params[0] <= params[1]);
    }

    SECTION("The numeric value of the target type is less than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {{true, 123, "foo"}, nullptr},
            fkyaml::node {{true, 123, "foo"}, true},
            fkyaml::node {{true, 123, "foo"}, 123},
            fkyaml::node {{true, 123, "foo"}, 3.14},
            fkyaml::node {{true, 123, "foo"}, "foo"},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, nullptr},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, true},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 123},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 3.14},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, "foo"},
            fkyaml::node {nullptr, true},
            fkyaml::node {nullptr, 123},
            fkyaml::node {nullptr, 3.14},
            fkyaml::node {nullptr, "foo"},
            fkyaml::node {true, 123},
            fkyaml::node {true, 3.14},
            fkyaml::node {true, "foo"},
            fkyaml::node {123, 3.14},
            fkyaml::node {123, "foo"},
            fkyaml::node {3.14, "foo"});
        REQUIRE(params[0] <= params[1]);
    }

    SECTION("The numeric value of the target type is greater than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {true, 123, "foo"}},
            fkyaml::node {nullptr, {true, 123, "foo"}},
            fkyaml::node {nullptr, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, {true, 123, "foo"}},
            fkyaml::node {true, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, nullptr},
            fkyaml::node {123, {true, 123, "foo"}},
            fkyaml::node {123, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {123, nullptr},
            fkyaml::node {123, true},
            fkyaml::node {3.14, {true, 123, "foo"}},
            fkyaml::node {3.14, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {3.14, nullptr},
            fkyaml::node {3.14, true},
            fkyaml::node {3.14, 123},
            fkyaml::node {"foo", {true, 123, "foo"}},
            fkyaml::node {"foo", {{"foo", 123}, {"bar", true}}},
            fkyaml::node {"foo", nullptr},
            fkyaml::node {"foo", true},
            fkyaml::node {"foo", 123},
            fkyaml::node {"foo", 3.14});
        REQUIRE_FALSE(params[0] <= params[1]);
    }
}

TEST_CASE("Node_GreaterThanOperator") {
    SECTION("The same type and value") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, nullptr},
            fkyaml::node {true, true},
            fkyaml::node {123, 123},
            fkyaml::node {3.14, 3.14},
            fkyaml::node {"foo", "foo"});
        REQUIRE_FALSE(params[0] > params[1]);
    }

    SECTION("The same type and the target value is less than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123}, {true, 123, "foo"}},
            fkyaml::node {{{"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {false, true},
            fkyaml::node {123, 456},
            fkyaml::node {3.14, 4.25},
            fkyaml::node {"foo", "qux"});
        REQUIRE_FALSE(params[0] > params[1]);
    }

    SECTION("The same type but the target value is greater than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"bar", true}}},
            fkyaml::node {true, false},
            fkyaml::node {456, 123},
            fkyaml::node {4.25, 3.14},
            fkyaml::node {"qux", "foo"});
        REQUIRE(params[0] > params[1]);
    }

    SECTION("The numeric value of the target type is less than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {{true, 123, "foo"}, nullptr},
            fkyaml::node {{true, 123, "foo"}, true},
            fkyaml::node {{true, 123, "foo"}, 123},
            fkyaml::node {{true, 123, "foo"}, 3.14},
            fkyaml::node {{true, 123, "foo"}, "foo"},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, nullptr},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, true},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 123},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 3.14},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, "foo"},
            fkyaml::node {nullptr, true},
            fkyaml::node {nullptr, 123},
            fkyaml::node {nullptr, 3.14},
            fkyaml::node {nullptr, "foo"},
            fkyaml::node {true, 123},
            fkyaml::node {true, 3.14},
            fkyaml::node {true, "foo"},
            fkyaml::node {123, 3.14},
            fkyaml::node {123, "foo"},
            fkyaml::node {3.14, "foo"});
        REQUIRE_FALSE(params[0] > params[1]);
    }

    SECTION("The numeric value of the target type is greater than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {true, 123, "foo"}},
            fkyaml::node {nullptr, {true, 123, "foo"}},
            fkyaml::node {nullptr, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, {true, 123, "foo"}},
            fkyaml::node {true, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, nullptr},
            fkyaml::node {123, {true, 123, "foo"}},
            fkyaml::node {123, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {123, nullptr},
            fkyaml::node {123, true},
            fkyaml::node {3.14, {true, 123, "foo"}},
            fkyaml::node {3.14, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {3.14, nullptr},
            fkyaml::node {3.14, true},
            fkyaml::node {3.14, 123},
            fkyaml::node {"foo", {true, 123, "foo"}},
            fkyaml::node {"foo", {{"foo", 123}, {"bar", true}}},
            fkyaml::node {"foo", nullptr},
            fkyaml::node {"foo", true},
            fkyaml::node {"foo", 123},
            fkyaml::node {"foo", 3.14});
        REQUIRE(params[0] > params[1]);
    }
}

TEST_CASE("Node_GreaterThanOrEqualToOperator") {
    SECTION("The same type and value") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, nullptr},
            fkyaml::node {true, true},
            fkyaml::node {123, 123},
            fkyaml::node {3.14, 3.14},
            fkyaml::node {"foo", "foo"});
        REQUIRE(params[0] >= params[1]);
    }

    SECTION("The same type and the target value is less than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123}, {true, 123, "foo"}},
            fkyaml::node {{{"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {false, true},
            fkyaml::node {123, 456},
            fkyaml::node {3.14, 4.25},
            fkyaml::node {"foo", "qux"});
        REQUIRE_FALSE(params[0] >= params[1]);
    }

    SECTION("The same type but the target value is greater than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"bar", true}}},
            fkyaml::node {true, false},
            fkyaml::node {456, 123},
            fkyaml::node {4.25, 3.14},
            fkyaml::node {"qux", "foo"});
        REQUIRE(params[0] >= params[1]);
    }

    SECTION("The numeric value of the target type is less than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {{true, 123, "foo"}, nullptr},
            fkyaml::node {{true, 123, "foo"}, true},
            fkyaml::node {{true, 123, "foo"}, 123},
            fkyaml::node {{true, 123, "foo"}, 3.14},
            fkyaml::node {{true, 123, "foo"}, "foo"},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, nullptr},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, true},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 123},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 3.14},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, "foo"},
            fkyaml::node {nullptr, true},
            fkyaml::node {nullptr, 123},
            fkyaml::node {nullptr, 3.14},
            fkyaml::node {nullptr, "foo"},
            fkyaml::node {true, 123},
            fkyaml::node {true, 3.14},
            fkyaml::node {true, "foo"},
            fkyaml::node {123, 3.14},
            fkyaml::node {123, "foo"},
            fkyaml::node {3.14, "foo"});
        REQUIRE_FALSE(params[0] >= params[1]);
    }

    SECTION("The numeric value of the target type is greater than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {true, 123, "foo"}},
            fkyaml::node {nullptr, {true, 123, "foo"}},
            fkyaml::node {nullptr, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, {true, 123, "foo"}},
            fkyaml::node {true, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, nullptr},
            fkyaml::node {123, {true, 123, "foo"}},
            fkyaml::node {123, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {123, nullptr},
            fkyaml::node {123, true},
            fkyaml::node {3.14, {true, 123, "foo"}},
            fkyaml::node {3.14, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {3.14, nullptr},
            fkyaml::node {3.14, true},
            fkyaml::node {3.14, 123},
            fkyaml::node {"foo", {true, 123, "foo"}},
            fkyaml::node {"foo", {{"foo", 123}, {"bar", true}}},
            fkyaml::node {"foo", nullptr},
            fkyaml::node {"foo", true},
            fkyaml::node {"foo", 123},
            fkyaml::node {"foo", 3.14});
        REQUIRE(params[0] >= params[1]);
    }
}

//
// test cases for type property getter/checkers
//

TEST_CASE("Node_Type") {
    using NodeTypePair = std::pair<fkyaml::node, fkyaml::node::node_t>;
    auto type_pair = GENERATE(
        NodeTypePair(fkyaml::node::sequence(), fkyaml::node::node_t::SEQUENCE),
        NodeTypePair(fkyaml::node::mapping(), fkyaml::node::node_t::MAPPING),
        NodeTypePair(fkyaml::node(), fkyaml::node::node_t::NULL_OBJECT),
        NodeTypePair(fkyaml::node(false), fkyaml::node::node_t::BOOLEAN),
        NodeTypePair(fkyaml::node(0), fkyaml::node::node_t::INTEGER),
        NodeTypePair(fkyaml::node(0.0), fkyaml::node::node_t::FLOAT_NUMBER),
        NodeTypePair(fkyaml::node(""), fkyaml::node::node_t::STRING));

    SECTION("non-alias node types") {
        REQUIRE(type_pair.first.type() == type_pair.second);
    }

    SECTION("alias node types") {
        type_pair.first.add_anchor_name("anchor_name");
        fkyaml::node alias = fkyaml::node::alias_of(type_pair.first);
        REQUIRE(alias.type() == type_pair.second);
    }
}

TEST_CASE("Node_IsSequence") {
    SECTION("sequence node type") {
        fkyaml::node node = fkyaml::node::sequence();

        SECTION("non-alias sequence node type") {
            REQUIRE(node.is_sequence());
        }

        SECTION("alias sequence node type") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_sequence());
        }
    }

    SECTION("non-sequence node types") {
        auto node = GENERATE(
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-sequence node types") {
            REQUIRE_FALSE(node.is_sequence());
        }

        SECTION("alias non-sequence node types.") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_sequence());
        }
    }
}

TEST_CASE("Node_IsMapping") {
    SECTION("mapping node type") {
        fkyaml::node node = fkyaml::node::mapping();

        SECTION("non-alias mapping node type") {
            REQUIRE(node.is_mapping());
        }

        SECTION("alias mapping node type") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_mapping());
        }
    }

    SECTION("non-mapping node types") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-mapping node types") {
            REQUIRE_FALSE(node.is_mapping());
        }

        SECTION("alias non-mapping node types.") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_mapping());
        }
    }
}

TEST_CASE("Node_IsNull") {
    SECTION("null node type") {
        fkyaml::node node = fkyaml::node();

        SECTION("non-alias null node type") {
            REQUIRE(node.is_null());
        }

        SECTION("alias null node type") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_null());
        }
    }

    SECTION("non-null node types") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-null node types") {
            REQUIRE_FALSE(node.is_null());
        }

        SECTION("alias non-null node types") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_null());
        }
    }
}

TEST_CASE("Node_IsBoolean") {
    SECTION("boolean node type") {
        fkyaml::node node = false;

        SECTION("non-alias boolean node type") {
            REQUIRE(node.is_boolean());
        }

        SECTION("alias boolean node type") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_boolean());
        }
    }

    SECTION("non-boolean node types") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-boolean node types") {
            REQUIRE_FALSE(node.is_boolean());
        }

        SECTION("alias non-boolean node types") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_boolean());
        }
    }
}

TEST_CASE("Node_IsInteger") {
    SECTION("integer node type.") {
        fkyaml::node node = 0;

        SECTION("non-alias integer node type.") {
            REQUIRE(node.is_integer());
        }

        SECTION("alias integer node type.") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_integer());
        }
    }

    SECTION("non-integer node types.") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-integer node types") {
            REQUIRE_FALSE(node.is_integer());
        }

        SECTION("alias non-integer node types") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_integer());
        }
    }
}

TEST_CASE("Node_IsFloatNumber") {
    SECTION("float number node type") {
        fkyaml::node node = 0.0;

        SECTION("non-alias float number node type") {
            REQUIRE(node.is_float_number());
        }

        SECTION("alias float number node type") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_float_number());
        }
    }

    SECTION("non-float-number node types") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(""));

        SECTION("non-alias non-float-number node types") {
            REQUIRE_FALSE(node.is_float_number());
        }

        SECTION("alias non-float-number node types") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_float_number());
        }
    }
}

TEST_CASE("Node_IsString") {
    SECTION("string node type") {
        fkyaml::node node = "";

        SECTION("non-alias string node type") {
            REQUIRE(node.is_string());
        }

        SECTION("alias string node type") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_string());
        }
    }

    SECTION("non-string node types") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0));

        SECTION("non-alias non-string node types") {
            REQUIRE_FALSE(node.is_string());
        }

        SECTION("alias non-string node types") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_string());
        }
    }
}

TEST_CASE("Node_IsScalar") {
    SECTION("scalar node types") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));

        SECTION("non-alias scalar node types") {
            REQUIRE(node.is_scalar());
        }

        SECTION("alias scalar node types") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_scalar());
        }
    }

    SECTION("non-scalar node types") {
        auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping());

        SECTION("non-alias non-scalar node types") {
            REQUIRE_FALSE(node.is_scalar());
        }

        SECTION("alias non-scalar node types") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_scalar());
        }
    }
}

//
// test cases for emptiness checker
//

TEST_CASE("Node_Empty") {
    SECTION("container and string scalar nodes") {
        SECTION("empty node") {
            auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping(), fkyaml::node(""));

            SECTION("empty non-alias node") {
                REQUIRE_NOTHROW(node.empty());
                REQUIRE(node.empty());
            }

            SECTION("empty alias node") {
                node.add_anchor_name("anchor_name");
                fkyaml::node alias = fkyaml::node::alias_of(node);
                REQUIRE_NOTHROW(alias.empty());
                REQUIRE(alias.empty());
            }
        }

        SECTION("non-empty node") {
            auto node = GENERATE(
                fkyaml::node::sequence(fkyaml::node::sequence_type(3)),
                fkyaml::node::mapping(fkyaml::node::mapping_type {{"test", fkyaml::node()}}),
                fkyaml::node("test"));

            SECTION("non-empty non-alias node") {
                REQUIRE_NOTHROW(node.empty());
                REQUIRE_FALSE(node.empty());
            }

            SECTION("non-empty alias node") {
                node.add_anchor_name("anchor_name");
                fkyaml::node alias = fkyaml::node::alias_of(node);
                REQUIRE_NOTHROW(alias.empty());
                REQUIRE_FALSE(alias.empty());
            }
        }
    }

    SECTION("non-string scalar nodes") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0));

        SECTION("non-const non-alias node") {
            REQUIRE_THROWS_AS(node.empty(), fkyaml::type_error);
        }

        SECTION("const non-alias node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.empty(), fkyaml::type_error);
        }

        SECTION("non-const alias node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.empty(), fkyaml::type_error);
        }

        SECTION("const alias node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.empty(), fkyaml::type_error);
        }
    }
}

//
// test cases for mapping key existence checker
//

TEST_CASE("Node_Contains") {
    SECTION("mapping") {
        fkyaml::node node = fkyaml::node::mapping({{"test", fkyaml::node()}});

        SECTION("mapping node with a string key") {
            REQUIRE(node.contains("test"));
        }

#ifdef FK_YAML_HAS_CXX_17
        SECTION("mapping node with a string view key") {
            using namespace std::string_view_literals;
            REQUIRE(node.contains("test"sv));
        }
#endif

        SECTION("mapping node with a string node key") {
            fkyaml::node node_key = "test";
            REQUIRE(node.contains(node_key));
        }
    }

    SECTION("non-mapping") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-mapping node with a key") {
            REQUIRE_FALSE(node.contains("test"));
        }

        SECTION("non-mapping node with a node key") {
            fkyaml::node node_key = "test";
            REQUIRE_FALSE(node.contains(node_key));
        }
    }
}

//
// test cases for container size getter
//

TEST_CASE("Node_Size") {
    SECTION("container and string scalar nodes") {
        auto node = GENERATE(
            fkyaml::node::sequence({fkyaml::node(), fkyaml::node(), fkyaml::node()}),
            fkyaml::node::mapping({{"test0", fkyaml::node()}, {"test1", fkyaml::node()}, {"test2", fkyaml::node()}}),
            fkyaml::node("tmp"));

        SECTION("non-const non-alias node") {
            REQUIRE_NOTHROW(node.size());
            REQUIRE(node.size() == 3);
        }

        SECTION("const node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(node.size());
            REQUIRE(const_node.size() == 3);
        }

        SECTION("alias node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.size());
            REQUIRE(alias.size() == 3);
        }

        SECTION("const alias node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.size());
            REQUIRE(alias.size() == 3);
        }
    }

    SECTION("non-string scalar nodes") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0));

        SECTION("non-const non-alias node") {
            REQUIRE_THROWS_AS(node.size(), fkyaml::type_error);
        }

        SECTION("const non-alias node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.size(), fkyaml::type_error);
        }

        SECTION("non-const alias node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.size(), fkyaml::type_error);
        }

        SECTION("const alias node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.size(), fkyaml::type_error);
        }
    }
}

//
// test cases for container element accessor with bounds checks
//

TEST_CASE("Node_At") {
    SECTION("mapping with existing key argument") {
        fkyaml::node::mapping_type map {{"test", fkyaml::node()}};

        SECTION("non-const string value") {
            fkyaml::node node = fkyaml::node::mapping(map);

            SECTION("non-const lvalue string value") {
                std::string key = "test";
                REQUIRE_NOTHROW(node.at(key));
                REQUIRE(node[key].is_null());
            }

            SECTION("non-const rvalue string value") {
                REQUIRE_NOTHROW(node.at("test"));
                REQUIRE(node.at("test").is_null());
            }
        }

        SECTION("const string value") {
            const fkyaml::node node = fkyaml::node::mapping(map);
            std::string key = "test";

            SECTION("const lvalue string node") {
                REQUIRE_NOTHROW(node.at(key));
            }

            SECTION("const rvalue string node") {
                REQUIRE_NOTHROW(node.at("test"));
            }
        }

#ifdef FK_YAML_HAS_CXX_17
        SECTION("string view value") {
            std::string_view key = "test";
            REQUIRE(map.at(key).is_null());
        }
#endif

        SECTION("non-const string node") {
            fkyaml::node node = fkyaml::node::mapping(map);
            fkyaml::node node_key = "test";

            SECTION("non-const lvalue string node") {
                REQUIRE_NOTHROW(node.at(node_key));
            }

            SECTION("non-const rvalue string node") {
                REQUIRE_NOTHROW(node.at(std::move(node_key)));
            }
        }

        SECTION("const string node") {
            const fkyaml::node node = fkyaml::node::mapping(map);
            fkyaml::node node_key = "test";

            SECTION("non-const lvalue string node") {
                REQUIRE_NOTHROW(node.at(node_key));
            }

            SECTION("non-const rvalue string node") {
                REQUIRE_NOTHROW(node.at(std::move(node_key)));
            }
        }
    }

    SECTION("mapping with non-existing key argument") {
        fkyaml::node node = {{"foo", 123}};

        SECTION("compatible type objects") {
            REQUIRE_THROWS_AS(node.at(fkyaml::node::sequence_type()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node::mapping_type()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(nullptr), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(true), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(123), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(3.14), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at("bar"), fkyaml::out_of_range);
        }

        SECTION("compatible type objects (const)") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::sequence_type()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::mapping_type()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(nullptr), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(true), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(123), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(3.14), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at("bar"), fkyaml::out_of_range);
        }

        SECTION("basic_node objects") {
            REQUIRE_THROWS_AS(node.at(fkyaml::node::sequence()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node::mapping()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node(true)), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node(123)), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node(3.14)), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node("bar")), fkyaml::out_of_range);
        }

        SECTION("basic_node objects (const)") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::sequence()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::mapping()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node(true)), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node(123)), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node(3.14)), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node("bar")), fkyaml::out_of_range);
        }
    }

    SECTION("sequence with integer argument") {
        fkyaml::node node = fkyaml::node::sequence();
        node.get_value_ref<fkyaml::node::sequence_type&>().emplace_back();

        SECTION("non-const integer value") {
            REQUIRE_NOTHROW(node.at(0));
        }

        SECTION("const integer value") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.at(0));
        }

        SECTION("non-const integer node") {
            REQUIRE_NOTHROW(node.at(fkyaml::node(0)));
        }

        SECTION("const integer node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.at(fkyaml::node(0)));
        }
    }

    SECTION("sequence with non-integer argument") {
        fkyaml::node node = fkyaml::node::sequence();
        node.get_value_ref<fkyaml::node::sequence_type&>().emplace_back();

        SECTION("non-const node with a non-integer value") {
            REQUIRE_THROWS_AS(node.at(fkyaml::node::sequence_type()), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(fkyaml::node::mapping_type()), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(nullptr), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(false), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(0.0), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(""), fkyaml::type_error);
        }

        SECTION("const node with a non-integer value") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::sequence_type()), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::mapping_type()), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(nullptr), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(false), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(0.0), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(""), fkyaml::type_error);
        }

        SECTION("non-const node with a non-integer node") {
            REQUIRE_THROWS_AS(node.at(fkyaml::node::mapping()), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(fkyaml::node::sequence()), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(fkyaml::node()), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(fkyaml::node(false)), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(fkyaml::node("")), fkyaml::type_error);
        }

        SECTION("const node with a non-integer node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::mapping()), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::sequence()), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node()), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node(false)), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node("")), fkyaml::type_error);
        }
    }

    SECTION("sequence with out-of-bounds integer argument") {
        fkyaml::node node = fkyaml::node::sequence();
        node.get_value_ref<fkyaml::node::sequence_type&>().emplace_back();

        SECTION("non-const argument") {
            REQUIRE_THROWS_AS(node.at(1), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node(1)), fkyaml::out_of_range);
        }

        SECTION("const argument") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.at(1), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node(1)), fkyaml::out_of_range);
        }
    }

    SECTION("scalar") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));
        fkyaml::node node_key = 0;

        SECTION("non-const node with an integer.") {
            REQUIRE_THROWS_AS(node.at(0), fkyaml::type_error);
        }

        SECTION("const node with an integer") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.at(0), fkyaml::type_error);
        }

        SECTION("non-const node with an integer node") {
            REQUIRE_THROWS_AS(node.at(node_key), fkyaml::type_error);
        }

        SECTION("const node with an integer node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.at(node_key), fkyaml::type_error);
        }
    }
}

//
// test cases for YAML version property getter/setter
//

TEST_CASE("Node_SetYamlVersion") {
    fkyaml::node node;
    node.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_1);
    REQUIRE(node.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_1);

    node.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_2);
    REQUIRE(node.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_2);
}

TEST_CASE("Node_GetYamlVersion") {
    fkyaml::node node;
    REQUIRE(node.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_2);

    node.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_1);
    REQUIRE(node.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_1);
}

//
// test cases for anchor related APIs
//

TEST_CASE("Node_IsAlias") {
    fkyaml::node node;

    SECTION("without anchor name") {
        REQUIRE_FALSE(node.is_alias());
    }

    SECTION("an anchor node") {
        node.add_anchor_name("anchor");
        REQUIRE_FALSE(node.is_alias());
    }

    SECTION("an alias node") {
        fkyaml::node anchor;
        anchor.add_anchor_name("anchor");
        node = fkyaml::node::alias_of(anchor);
        REQUIRE(node.is_alias());
    }
}

TEST_CASE("Node_IsAnchor") {
    fkyaml::node node;

    SECTION("without anchor name") {
        REQUIRE_FALSE(node.is_anchor());
    }

    SECTION("an anchor node") {
        node.add_anchor_name("anchor");
        REQUIRE(node.is_anchor());
    }

    SECTION("an alias node") {
        fkyaml::node anchor;
        anchor.add_anchor_name("anchor");
        node = fkyaml::node::alias_of(anchor);
        REQUIRE_FALSE(node.is_anchor());
    }
}

TEST_CASE("Node_HasAnchorName") {
    fkyaml::node node;

    SECTION("without anchor name") {
        REQUIRE_FALSE(node.has_anchor_name());
    }

    SECTION("with anchor name") {
        node.add_anchor_name("anchor_name");
        REQUIRE(node.has_anchor_name());
    }
}

TEST_CASE("Node_GetAnchorName") {
    fkyaml::node node;

    SECTION("without anchor name") {
        REQUIRE_THROWS_AS(node.get_anchor_name(), fkyaml::exception);
    }

    SECTION("with anchor name") {
        node.add_anchor_name("anchor_name");
        REQUIRE_NOTHROW(node.get_anchor_name());
        REQUIRE(node.get_anchor_name().compare("anchor_name") == 0);
    }
}

TEST_CASE("Node_AddAnchorName") {
    fkyaml::node node;
    std::string anchor_name = "anchor_name";

    SECTION("lvalue anchor name") {
        node.add_anchor_name(anchor_name);
        REQUIRE_NOTHROW(node.get_anchor_name());
        REQUIRE(node.get_anchor_name().compare("anchor_name") == 0);
    }

    SECTION("rvalue anchor name") {
        node.add_anchor_name(std::move(anchor_name));
        REQUIRE_NOTHROW(node.get_anchor_name());
        REQUIRE(node.get_anchor_name().compare("anchor_name") == 0);
    }

    SECTION("overwrite an existing anchor name") {
        node.add_anchor_name(anchor_name);
        node.add_anchor_name("overwritten_name");
        REQUIRE_NOTHROW(node.get_anchor_name());
        REQUIRE_FALSE(node.get_anchor_name().compare("anchor_name") == 0);
        REQUIRE(node.get_anchor_name().compare("overwritten_name") == 0);
    }
}

//
// test cases for tag related APIs
//

TEST_CASE("Node_HasTagName") {
    fkyaml::node node;

    SECTION("node with tag name") {
        node.add_tag_name("tag_name");
        REQUIRE(node.has_tag_name());
    }

    SECTION("node without tag name") {
        REQUIRE_FALSE(node.has_tag_name());
    }
}

TEST_CASE("Node_GetTagName") {
    fkyaml::node node;

    SECTION("node without tag name.") {
        REQUIRE_THROWS_AS(node.get_tag_name(), fkyaml::exception);
    }

    SECTION("node with tag name.") {
        node.add_tag_name("tag_name");
        REQUIRE_NOTHROW(node.get_tag_name());
        REQUIRE(node.get_tag_name() == "tag_name");
    }
}

TEST_CASE("Node_AddTagName") {
    fkyaml::node node;
    std::string tag_name = "tag_name";

    SECTION("lvalue tag name") {
        node.add_tag_name(tag_name);
        REQUIRE_NOTHROW(node.get_tag_name());
        REQUIRE(node.get_tag_name().compare("tag_name") == 0);
    }

    SECTION("rvalue tag name") {
        node.add_tag_name(std::move(tag_name));
        REQUIRE_NOTHROW(node.get_tag_name());
        REQUIRE(node.get_tag_name().compare("tag_name") == 0);
    }

    SECTION("overwrite an existing tag name") {
        node.add_tag_name(tag_name);
        node.add_tag_name("overwritten_name");
        REQUIRE_NOTHROW(node.get_tag_name());
        REQUIRE_FALSE(node.get_tag_name().compare("tag_name") == 0);
        REQUIRE(node.get_tag_name().compare("overwritten_name") == 0);
    }
}

//
// test cases for value getters (copy)
//

struct string_wrap {
    string_wrap() = default;
    string_wrap& operator=(const std::string& _str) {
        str = _str;
        return *this;
    }
    std::string str;
};

TEST_CASE("Node_GetValue") {

    SECTION("sequence") {
        fkyaml::node node(fkyaml::node::sequence_type {fkyaml::node(true), fkyaml::node(false)});

        SECTION("sequence value") {
            auto seq = node.get_value<fkyaml::node::sequence_type>();
            REQUIRE(seq.size() == 2);
            REQUIRE(seq[0].is_boolean());
            REQUIRE(seq[0].get_value<bool>() == true);
            REQUIRE(seq[1].is_boolean());
            REQUIRE(seq[1].get_value<bool>() == false);
        }

        SECTION("non-sequence value") {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::mapping_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::boolean_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::integer_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::float_number_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::string_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<string_wrap>(), fkyaml::type_error);
        }
    }

    SECTION("mapping") {
        fkyaml::node node(fkyaml::node::mapping_type {{"test", fkyaml::node(3.14)}, {"foo", fkyaml::node("bar")}});

        SECTION("mapping value") {
            auto map = node.get_value<fkyaml::node::mapping_type>();
            REQUIRE(map.size() == 2);
            REQUIRE(map.find("test") != map.end());
            REQUIRE(map.at("test").is_float_number());
            REQUIRE(map.at("test").get_value<double>() == 3.14);
            REQUIRE(map.find("foo") != map.end());
            REQUIRE(map.at("foo").is_string());
            REQUIRE(map.at("foo").get_value_ref<std::string&>() == "bar");
        }

        SECTION("non-mapping values") {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::sequence_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::boolean_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::integer_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::float_number_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::string_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<string_wrap>(), fkyaml::type_error);
        }
    }

    SECTION("null node value") {
        fkyaml::node node(nullptr);

        SECTION("null value") {
            auto null = node.get_value<std::nullptr_t>();
            REQUIRE(null == nullptr);
        }

        SECTION("non-null values") {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::sequence_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::mapping_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::boolean_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::integer_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::float_number_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::string_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<string_wrap>(), fkyaml::type_error);
        }
    }

    SECTION("boolean node value") {
        fkyaml::node node(true);

        SECTION("boolean value") {
            REQUIRE(node.get_value<bool>() == true);
        }

        SECTION("non-boolean values") {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::sequence_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::mapping_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::integer_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::float_number_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::string_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<string_wrap>(), fkyaml::type_error);
        }
    }

    SECTION("integer node value") {
        fkyaml::node node(123);

        SECTION("integer values") {
            REQUIRE(node.get_value<int8_t>() == 123);
            REQUIRE(node.get_value<int16_t>() == 123);
            REQUIRE(node.get_value<int32_t>() == 123);
            REQUIRE(node.get_value<int64_t>() == 123);
            REQUIRE(node.get_value<uint8_t>() == 123);
            REQUIRE(node.get_value<uint16_t>() == 123);
            REQUIRE(node.get_value<uint32_t>() == 123);
            // TODO: REQUIRE(node.get_value<uint64_t>() == 123);
        }

        SECTION("non-integer values") {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::sequence_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::mapping_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::boolean_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::float_number_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::string_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<string_wrap>(), fkyaml::type_error);
        }

        SECTION("non-integer node value") {
            fkyaml::node non_int_node(true);
            REQUIRE_THROWS_AS(non_int_node.get_value<int32_t>(), fkyaml::type_error);
        }

        SECTION("underflowable integer type") {
            fkyaml::node negative_int_node(std::numeric_limits<fkyaml::node::integer_type>::min());
            REQUIRE_THROWS_AS(negative_int_node.get_value<int8_t>(), fkyaml::exception);
        }

        SECTION("overflowable integer type") {
            fkyaml::node large_int_node(std::numeric_limits<fkyaml::node::integer_type>::max());
            REQUIRE_THROWS_AS(large_int_node.get_value<int8_t>(), fkyaml::exception);
        }
    }

    SECTION("float number node value") {
        fkyaml::node node(3.14);

        SECTION("float number values") {
            REQUIRE(std::abs(node.get_value<float>() - 3.14) < std::numeric_limits<float>::epsilon());
            REQUIRE(std::abs(node.get_value<double>() - 3.14) < std::numeric_limits<double>::epsilon());
            REQUIRE(std::abs(node.get_value<long double>() - 3.14) < std::numeric_limits<long double>::epsilon());
        }

        SECTION("non-float-number values") {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::sequence_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::mapping_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::boolean_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::integer_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::string_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<string_wrap>(), fkyaml::type_error);
        }

        SECTION("non-float-number node value") {
            fkyaml::node non_float_num_node(true);
            REQUIRE_THROWS_AS(non_float_num_node.get_value<float>(), fkyaml::type_error);
        }

        SECTION("underflowable float number type") {
            fkyaml::node negative_float_node(std::numeric_limits<fkyaml::node::float_number_type>::min());
            REQUIRE_THROWS_AS(negative_float_node.get_value<float>(), fkyaml::exception);
        }

        SECTION("overflowable float number type") {
            fkyaml::node large_float_node(std::numeric_limits<fkyaml::node::float_number_type>::max());
            REQUIRE_THROWS_AS(large_float_node.get_value<float>(), fkyaml::exception);
        }
    }

    SECTION("string node value") {
        fkyaml::node node("test");

        SECTION("string value") {
            auto str = node.get_value<fkyaml::node::string_type>();
            REQUIRE(str.size() == 4);
            REQUIRE(str == "test");
        }

        SECTION("compatible string value") {
            auto str_wrap = node.get_value<string_wrap>();
            REQUIRE(str_wrap.str.size() == 4);
            REQUIRE(str_wrap.str == "test");
        }

#ifdef FK_YAML_HAS_CXX_17
        SECTION("string view") {
            auto str_view = node.get_value<std::string_view>();
            REQUIRE(str_view.size() == 4);
            REQUIRE(str_view == "test");
        }
#endif

        SECTION("non-string values") {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::sequence_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::mapping_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::boolean_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::integer_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::float_number_type>(), fkyaml::type_error);
        }
    }
}

//
// test cases for value reference getters
//

TEST_CASE("Node_GetValueRefForSequence") {
    SECTION("sequence") {
        fkyaml::node node =
            fkyaml::node::sequence(fkyaml::node::sequence_type {fkyaml::node(), fkyaml::node(), fkyaml::node()});

        SECTION("non-alias sequence node") {
            REQUIRE_NOTHROW(node.get_value_ref<fkyaml::node::sequence_type&>());
            REQUIRE(node.get_value_ref<fkyaml::node::sequence_type&>().size() == 3);
            for (int i = 0; i < 3; ++i) {
                REQUIRE(node.get_value_ref<fkyaml::node::sequence_type&>()[i].is_null());
            }
        }

        SECTION("const non-alias sequence node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.get_value_ref<const fkyaml::node::sequence_type&>());
            REQUIRE(const_node.get_value_ref<const fkyaml::node::sequence_type&>().size() == 3);
            for (int i = 0; i < 3; ++i) {
                REQUIRE(node.get_value_ref<const fkyaml::node::sequence_type&>()[i].is_null());
            }
        }

        SECTION("alias sequence node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.get_value_ref<fkyaml::node::sequence_type&>());
            REQUIRE(alias.get_value_ref<fkyaml::node::sequence_type&>().size() == 3);
            for (int i = 0; i < 3; ++i) {
                REQUIRE(alias.get_value_ref<fkyaml::node::sequence_type&>()[i].is_null());
            }
        }

        SECTION("const alias sequence node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.get_value_ref<const fkyaml::node::sequence_type&>());
            REQUIRE(alias.get_value_ref<const fkyaml::node::sequence_type&>().size() == 3);
            for (int i = 0; i < 3; ++i) {
                REQUIRE(alias.get_value_ref<const fkyaml::node::sequence_type&>()[i].is_null());
            }
        }
    }

    SECTION("non-sequence") {
        auto node = GENERATE(
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-sequence nodes") {
            REQUIRE_THROWS_AS(node.get_value_ref<fkyaml::node::sequence_type&>(), fkyaml::type_error);
        }

        SECTION("const non-alias non-sequence nodes") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.get_value_ref<const fkyaml::node::sequence_type&>(), fkyaml::type_error);
        }

        SECTION("alias non-sequence nodes") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.get_value_ref<fkyaml::node::sequence_type&>(), fkyaml::type_error);
        }

        SECTION("const alias non-sequence nodes") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.get_value_ref<const fkyaml::node::sequence_type&>(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_GetValueRefForMapping") {
    SECTION("mapping") {
        fkyaml::node node = fkyaml::node::mapping(fkyaml::node::mapping_type {
            {"test0", fkyaml::node()}, {"test1", fkyaml::node()}, {"test2", fkyaml::node()}});

        SECTION("non-alias mapping node") {
            REQUIRE_NOTHROW(node.get_value_ref<fkyaml::node::mapping_type&>());
            REQUIRE(node.get_value_ref<fkyaml::node::mapping_type&>().size() == 3);
            REQUIRE(node["test0"].is_null());
            REQUIRE(node["test1"].is_null());
            REQUIRE(node["test2"].is_null());
        }

        SECTION("const non-alias mapping node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.get_value_ref<const fkyaml::node::mapping_type&>());
            REQUIRE(const_node.get_value_ref<const fkyaml::node::mapping_type&>().size() == 3);
            REQUIRE(const_node["test0"].is_null());
            REQUIRE(const_node["test1"].is_null());
            REQUIRE(const_node["test2"].is_null());
        }

        SECTION("alias mapping node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.get_value_ref<fkyaml::node::mapping_type&>());
            REQUIRE(alias.get_value_ref<fkyaml::node::mapping_type&>().size() == 3);
            REQUIRE(alias["test0"].is_null());
            REQUIRE(alias["test1"].is_null());
            REQUIRE(alias["test2"].is_null());
        }

        SECTION("const alias mapping node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.get_value_ref<const fkyaml::node::mapping_type&>());
            REQUIRE(alias["test0"].is_null());
            REQUIRE(alias["test1"].is_null());
            REQUIRE(alias["test2"].is_null());
        }
    }

    SECTION("non-mapping") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-mapping nodes") {
            REQUIRE_THROWS_AS(node.get_value_ref<fkyaml::node::mapping_type&>(), fkyaml::type_error);
        }

        SECTION("const non-alias non-mapping nodes") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.get_value_ref<const fkyaml::node::mapping_type&>(), fkyaml::type_error);
        }

        SECTION("alias non-mapping nodes") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.get_value_ref<fkyaml::node::mapping_type&>(), fkyaml::type_error);
        }

        SECTION("const alias non-mapping nodes") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.get_value_ref<const fkyaml::node::mapping_type&>(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_GetValueRefForBoolean") {
    SECTION("boolean") {
        fkyaml::node node = true;

        SECTION("non-alias boolean node") {
            REQUIRE_NOTHROW(node.get_value_ref<fkyaml::node::boolean_type&>());
            REQUIRE(node.get_value_ref<fkyaml::node::boolean_type&>() == true);
        }

        SECTION("const non-alias boolean node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.get_value_ref<const fkyaml::node::boolean_type&>());
            REQUIRE(const_node.get_value_ref<const fkyaml::node::boolean_type&>() == true);
        }

        SECTION("alias boolean node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.get_value_ref<fkyaml::node::boolean_type&>());
            REQUIRE(alias.get_value_ref<fkyaml::node::boolean_type&>() == true);
        }

        SECTION("const alias boolean node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.get_value_ref<const fkyaml::node::boolean_type&>());
            REQUIRE(alias.get_value_ref<const fkyaml::node::boolean_type&>() == true);
        }
    }

    SECTION("non-boolean") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-boolean nodes") {
            REQUIRE_THROWS_AS(node.get_value_ref<fkyaml::node::boolean_type&>(), fkyaml::type_error);
        }

        SECTION("const non-alias non-boolean nodes") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.get_value_ref<const fkyaml::node::boolean_type&>(), fkyaml::type_error);
        }

        SECTION("alias non-boolean nodes") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.get_value_ref<fkyaml::node::boolean_type&>(), fkyaml::type_error);
        }

        SECTION("const alias non-boolean nodes") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.get_value_ref<const fkyaml::node::boolean_type&>(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_GetValueRefForInteger") {
    SECTION("integer") {
        fkyaml::node::integer_type integer = -123;
        fkyaml::node node = integer;

        SECTION("non-alias integer node") {
            REQUIRE_NOTHROW(node.get_value_ref<fkyaml::node::integer_type&>());
            REQUIRE(node.get_value_ref<fkyaml::node::integer_type&>() == integer);
        }

        SECTION("const non-alias integer node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.get_value_ref<const fkyaml::node::integer_type&>());
            REQUIRE(const_node.get_value_ref<const fkyaml::node::integer_type&>() == integer);
        }

        SECTION("alias  integer node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.get_value_ref<fkyaml::node::integer_type&>());
            REQUIRE(alias.get_value_ref<fkyaml::node::integer_type&>() == integer);
        }

        SECTION("const alias  integer node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.get_value_ref<const fkyaml::node::integer_type&>());
            REQUIRE(alias.get_value_ref<const fkyaml::node::integer_type&>() == integer);
        }
    }

    SECTION("non-integer") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-integer nodes") {
            REQUIRE_THROWS_AS(node.get_value_ref<fkyaml::node::integer_type&>(), fkyaml::type_error);
        }

        SECTION("const non-alias non-integer nodes") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.get_value_ref<const fkyaml::node::integer_type&>(), fkyaml::type_error);
        }

        SECTION("alias non-integer nodes") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.get_value_ref<fkyaml::node::integer_type&>(), fkyaml::type_error);
        }

        SECTION("const alias non-integer nodes") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.get_value_ref<const fkyaml::node::integer_type&>(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_GetValueRefForFloatNumber") {
    SECTION("floating point number") {
        fkyaml::node::float_number_type float_val = 123.45;
        fkyaml::node node = float_val;

        SECTION("non-alias float number node") {
            REQUIRE_NOTHROW(node.get_value_ref<fkyaml::node::float_number_type&>());
            REQUIRE(node.get_value_ref<fkyaml::node::float_number_type&>() == float_val);
        }

        SECTION("const non-alias float number node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.get_value_ref<const fkyaml::node::float_number_type&>());
            REQUIRE(const_node.get_value_ref<const fkyaml::node::float_number_type&>() == float_val);
        }

        SECTION("alias float number node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.get_value_ref<fkyaml::node::float_number_type&>());
            REQUIRE(alias.get_value_ref<fkyaml::node::float_number_type&>() == float_val);
        }

        SECTION("const alias float number node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.get_value_ref<const fkyaml::node::float_number_type&>());
            REQUIRE(alias.get_value_ref<const fkyaml::node::float_number_type&>() == float_val);
        }
    }

    SECTION("not floating point number") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(""));

        SECTION("non-alias non-float-number nodes") {
            REQUIRE_THROWS_AS(node.get_value_ref<fkyaml::node::float_number_type&>(), fkyaml::type_error);
        }

        SECTION("const non-alias non-float-number nodes") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.get_value_ref<const fkyaml::node::float_number_type&>(), fkyaml::type_error);
        }

        SECTION("alias non-float-number nodes") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.get_value_ref<fkyaml::node::float_number_type&>(), fkyaml::type_error);
        }

        SECTION("const alias non-float-number nodes") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.get_value_ref<const fkyaml::node::float_number_type&>(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_GetValueRefForString") {
    SECTION("string") {
        fkyaml::node::string_type str = "test";
        fkyaml::node node = str;

        SECTION("non-alias string node") {
            REQUIRE_NOTHROW(node.get_value_ref<fkyaml::node::string_type&>());
            REQUIRE(node.get_value_ref<fkyaml::node::string_type&>() == str);
        }

        SECTION("const non-alias string node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.get_value_ref<const fkyaml::node::string_type&>());
            REQUIRE(const_node.get_value_ref<const fkyaml::node::string_type&>() == str);
        }

        SECTION("alias string node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.get_value_ref<fkyaml::node::string_type&>());
            REQUIRE(alias.get_value_ref<fkyaml::node::string_type&>() == str);
        }

        SECTION("const alias string node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.get_value_ref<const fkyaml::node::string_type&>());
            REQUIRE(alias.get_value_ref<const fkyaml::node::string_type&>() == str);
        }
    }

    SECTION("non-string") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0));

        SECTION("non-alias non-string nodes") {
            REQUIRE_THROWS_AS(node.get_value_ref<fkyaml::node::string_type&>(), fkyaml::type_error);
        }

        SECTION("const non-alias non-string nodes") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.get_value_ref<const fkyaml::node::string_type&>(), fkyaml::type_error);
        }

        SECTION("alias non-string nodes") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.get_value_ref<fkyaml::node::string_type&>(), fkyaml::type_error);
        }

        SECTION("const alias non-string nodes") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.get_value_ref<const fkyaml::node::string_type&>(), fkyaml::type_error);
        }
    }
}

//
// test cases for iterator getters
//

TEST_CASE("Node_Begin") {
    SECTION("container nodes") {
        auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping());

        SECTION("non-const non-alias container node") {
            REQUIRE_NOTHROW(node.begin());
        }

        SECTION("const non-alias container node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.begin());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.begin());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.begin());
        }
    }

    SECTION("scalar nodes") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));

        SECTION("non-const node") {
            REQUIRE_THROWS_AS(node.begin(), fkyaml::type_error);
        }

        SECTION("const node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.begin(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_End") {
    SECTION("container nodes") {
        auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping());

        SECTION("non-const non-alias container node") {
            REQUIRE_NOTHROW(node.end());
        }

        SECTION("const non-alias container node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.end());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.end());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.end());
        }

        SECTION("non-const range-based for-loop compatibility") {
            REQUIRE_NOTHROW(node.end());
        }

        SECTION("const range-based for-loop compatibility") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.end());
        }
    }

    SECTION("scalar nodes") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));

        SECTION("non-const throwing node") {
            REQUIRE_THROWS_AS(node.end(), fkyaml::type_error);
        }

        SECTION("const throwing node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.end(), fkyaml::type_error);
        }
    }
}

//
// test cases for swap
//

TEST_CASE("Node_Swap") {
    fkyaml::node lhs_node = true;
    fkyaml::node rhs_node = 123;
    lhs_node.swap(rhs_node);
    REQUIRE(lhs_node.is_integer());
    REQUIRE(lhs_node.get_value_ref<fkyaml::node::integer_type&>() == 123);
    REQUIRE(rhs_node.is_boolean());
    REQUIRE(rhs_node.get_value_ref<fkyaml::node::boolean_type&>() == true);
}

TEST_CASE("Node_ADLSwap") {
    fkyaml::node lhs_node = true;
    fkyaml::node rhs_node = 123;

    using std::swap;
    swap(lhs_node, rhs_node);

    REQUIRE(lhs_node.is_integer());
    REQUIRE(lhs_node.get_value_ref<fkyaml::node::integer_type&>() == 123);
    REQUIRE(rhs_node.is_boolean());
    REQUIRE(rhs_node.get_value_ref<fkyaml::node::boolean_type&>() == true);
}
