//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.1
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <cmath>
#include <cfloat>
#include <map>

#include "catch2/catch.hpp"

#include "fkYAML/node.hpp"

//
// test cases for constructors
//

TEST_CASE("NodeClassTest_DefaultCtorTest", "[NodeClassTest]")
{
    fkyaml::node node;
    REQUIRE(node.is_null());
}

TEST_CASE("NodeClassTest_SequenceTypeCtorTest", "[NodeClassTest]")
{
    fkyaml::node node(fkyaml::node::node_t::SEQUENCE);
    REQUIRE(node.is_sequence());
    REQUIRE(node.size() == 0);
}

TEST_CASE("NodeClassTest_MappingTypeCtorTest", "[NodeClassTest]")
{
    fkyaml::node node(fkyaml::node::node_t::MAPPING);
    REQUIRE(node.is_mapping());
    REQUIRE(node.size() == 0);
}

TEST_CASE("NodeClassTest_NullTypeCtorTest", "[NodeClassTest]")
{
    fkyaml::node node(fkyaml::node::node_t::NULL_OBJECT);
    REQUIRE(node.is_null());
}

TEST_CASE("NodeClassTest_BooleanTypeCtorTest", "[NodeClassTest]")
{
    fkyaml::node node(fkyaml::node::node_t::BOOLEAN);
    REQUIRE(node.is_boolean());
    REQUIRE(node.to_boolean() == false);
}

TEST_CASE("NodeClassTest_IntegerTypeCtorTest", "[NodeClassTest]")
{
    fkyaml::node node(fkyaml::node::node_t::INTEGER);
    REQUIRE(node.is_integer());
    REQUIRE(node.to_integer() == 0);
}

TEST_CASE("NodeClassTest_FloatNumberTypeCtorTest", "[NodeClassTest]")
{
    fkyaml::node node(fkyaml::node::node_t::FLOAT_NUMBER);
    REQUIRE(node.is_float_number());
    REQUIRE(node.to_float_number() == 0.0);
}

TEST_CASE("NodeClassTest_StringTypeCtorTest", "[NodeClassTest]")
{
    fkyaml::node node(fkyaml::node::node_t::STRING);
    REQUIRE(node.is_string());
    REQUIRE(node.size() == 0);
}

TEST_CASE("NodeClassTest_ThrowingSpecializationTypeCtorTest", "[NodeClassTest]")
{
    struct String
    {
        String()
        {
            throw fkyaml::exception();
        }
    };

    using NodeType = fkyaml::basic_node<std::vector, std::map, bool, int64_t, double, String>;
    REQUIRE_THROWS_AS(NodeType::string_scalar(), fkyaml::exception);
}

TEST_CASE("NodeClassTest_SequenceCtorTest", "[NodeClassTest]")
{
    fkyaml::node node(fkyaml::node_sequence_type {fkyaml::node {true}, fkyaml::node {false}});
    REQUIRE(node.type() == fkyaml::node::node_t::SEQUENCE);
    REQUIRE(node.is_sequence());
    REQUIRE(node.size() == 2);
    REQUIRE(node[0].is_boolean());
    REQUIRE(node[0].to_boolean() == true);
    REQUIRE(node[1].is_boolean());
    REQUIRE(node[1].to_boolean() == false);
}

TEST_CASE("NodeClassTest_MappingCtorTest", "[NodeClassTest]")
{
    fkyaml::node node(fkyaml::node_mapping_type {{"test", fkyaml::node {true}}});
    REQUIRE(node.type() == fkyaml::node::node_t::MAPPING);
    REQUIRE(node.is_mapping());
    REQUIRE(node.size() == 1);
    REQUIRE(node.contains("test"));
    REQUIRE(node["test"].is_boolean());
    REQUIRE(node["test"].to_boolean() == true);
}

TEST_CASE("NodeClassTest_NullCtorTest", "[NodeClassTest]")
{
    fkyaml::node node(nullptr);
    REQUIRE(node.type() == fkyaml::node::node_t::NULL_OBJECT);
    REQUIRE(node.is_null());
}

TEST_CASE("NodeClassTest_BooleanCtorTest", "[NodeClassTest]")
{
    fkyaml::node node(true);
    REQUIRE(node.type() == fkyaml::node::node_t::BOOLEAN);
    REQUIRE(node.is_boolean());
    REQUIRE(node.to_boolean() == true);
}

TEST_CASE("NodeClassTest_IntegerCtorTest", "[NodeClassTest]")
{
    fkyaml::node node(23467);
    REQUIRE(node.type() == fkyaml::node::node_t::INTEGER);
    REQUIRE(node.is_integer());
    REQUIRE(node.to_integer() == 23467);
}

TEST_CASE("NodeClassTest_FloatNumberCtorTest", "[NodeClassTest]")
{
    fkyaml::node node(3.14);
    REQUIRE(node.type() == fkyaml::node::node_t::FLOAT_NUMBER);
    REQUIRE(node.is_float_number());
    REQUIRE(node.to_float_number() == 3.14);
}

TEST_CASE("NodeClassTest_StringCtorTest", "[NodeClassTest]")
{
    auto node = GENERATE(fkyaml::node(fkyaml::node_string_type("test")));
    REQUIRE(node.type() == fkyaml::node::node_t::STRING);
    REQUIRE(node.is_string());
    REQUIRE(node.size() == 4);
    REQUIRE(node.to_string() == "test");
}

TEST_CASE("NodeClassTest_SequenceCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::node copied =
        fkyaml::node::sequence({fkyaml::node::boolean_scalar(true), fkyaml::node::string_scalar("test")});
    fkyaml::node node(copied);
    REQUIRE(node.is_sequence());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 2);
    REQUIRE_NOTHROW(node[0]);
    REQUIRE(node[0].is_boolean());
    REQUIRE_NOTHROW(node[0].to_boolean());
    REQUIRE(node[0].to_boolean() == true);
    REQUIRE_NOTHROW(node[1]);
    REQUIRE(node[1].is_string());
    REQUIRE_NOTHROW(node[1].to_string());
    REQUIRE_NOTHROW(node[1].to_string().size());
    REQUIRE(node[1].to_string().size() == 4);
    REQUIRE(node[1].to_string().compare("test") == 0);
}

TEST_CASE("NodeClassTest_MappingCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::node copied = fkyaml::node::mapping(
        {{"test0", fkyaml::node::integer_scalar(123)}, {"test1", fkyaml::node::float_number_scalar(3.14)}});
    fkyaml::node node(copied);
    REQUIRE(node.is_mapping());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 2);
    REQUIRE_NOTHROW(node["test0"]);
    REQUIRE(node["test0"].is_integer());
    REQUIRE_NOTHROW(node["test0"].to_integer());
    REQUIRE(node["test0"].to_integer() == 123);
    REQUIRE_NOTHROW(node["test1"]);
    REQUIRE(node["test1"].is_float_number());
    REQUIRE_NOTHROW(node["test1"].to_float_number());
    REQUIRE(node["test1"].to_float_number() == 3.14);
}

TEST_CASE("NodeClassTest_NullCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::node copied;
    fkyaml::node node(copied);
    REQUIRE(node.is_null());
}

TEST_CASE("NodeClassTest_BooleanCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::node copied = fkyaml::node::boolean_scalar(true);
    fkyaml::node node(copied);
    REQUIRE(node.is_boolean());
    REQUIRE_NOTHROW(node.to_boolean());
    REQUIRE(node.to_boolean() == true);
}

TEST_CASE("NodeClassTest_IntegerCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::node copied = fkyaml::node::integer_scalar(123);
    fkyaml::node node(copied);
    REQUIRE(node.is_integer());
    REQUIRE_NOTHROW(node.to_integer());
    REQUIRE(node.to_integer() == 123);
}

TEST_CASE("NodeClassTest_FloatNumberCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::node copied = fkyaml::node::float_number_scalar(3.14);
    fkyaml::node node(copied);
    REQUIRE(node.is_float_number());
    REQUIRE_NOTHROW(node.to_float_number());
    REQUIRE(node.to_float_number() == 3.14);
}

TEST_CASE("NodeClassTest_StringCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::node copied = fkyaml::node::string_scalar("test");
    fkyaml::node node(copied);
    REQUIRE(node.is_string());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 4);
    REQUIRE_NOTHROW(node.to_string());
    REQUIRE(node.to_string().compare("test") == 0);
}

TEST_CASE("NodeClassTest_AliasCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::node tmp = fkyaml::node::boolean_scalar(true);
    tmp.add_anchor_name("anchor_name");
    fkyaml::node tmp_alias = fkyaml::node::alias_of(tmp);
    fkyaml::node alias(tmp_alias);
    REQUIRE(alias.is_boolean());
    REQUIRE_NOTHROW(alias.to_boolean());
    REQUIRE(alias.to_boolean() == true);
}

TEST_CASE("NodeClassTest_SequenceMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::node moved =
        fkyaml::node::sequence({fkyaml::node::boolean_scalar(true), fkyaml::node::string_scalar("test")});
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_sequence());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 2);
    REQUIRE_NOTHROW(node[0]);
    REQUIRE(node[0].is_boolean());
    REQUIRE_NOTHROW(node[0].to_boolean());
    REQUIRE(node[0].to_boolean() == true);
    REQUIRE_NOTHROW(node[1]);
    REQUIRE(node[1].is_string());
    REQUIRE_NOTHROW(node[1].to_string());
    REQUIRE_NOTHROW(node[1].to_string().size());
    REQUIRE(node[1].to_string().size() == 4);
    REQUIRE(node[1].to_string().compare("test") == 0);
}

TEST_CASE("NodeClassTest_MappingMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::node moved = fkyaml::node::mapping(
        {{"test0", fkyaml::node::integer_scalar(123)}, {"test1", fkyaml::node::float_number_scalar(3.14)}});
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_mapping());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 2);
    REQUIRE_NOTHROW(node["test0"]);
    REQUIRE(node["test0"].is_integer());
    REQUIRE_NOTHROW(node["test0"].to_integer());
    REQUIRE(node["test0"].to_integer() == 123);
    REQUIRE_NOTHROW(node["test1"]);
    REQUIRE(node["test1"].is_float_number());
    REQUIRE_NOTHROW(node["test1"].to_float_number());
    REQUIRE(node["test1"].to_float_number() == 3.14);
}

TEST_CASE("NodeClassTest_NullMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::node moved;
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_null());
}

TEST_CASE("NodeClassTest_BooleanMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::node moved = fkyaml::node::boolean_scalar(true);
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_boolean());
    REQUIRE_NOTHROW(node.to_boolean());
    REQUIRE(node.to_boolean() == true);
}

TEST_CASE("NodeClassTest_IntegerMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::node moved = fkyaml::node::integer_scalar(123);
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_integer());
    REQUIRE_NOTHROW(node.to_integer());
    REQUIRE(node.to_integer() == 123);
}

TEST_CASE("NodeClassTest_FloatNumberMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::node moved = fkyaml::node::float_number_scalar(3.14);
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_float_number());
    REQUIRE_NOTHROW(node.to_float_number());
    REQUIRE(node.to_float_number() == 3.14);
}

TEST_CASE("NodeClassTest_StringMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::node moved = fkyaml::node::string_scalar("test");
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_string());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 4);
    REQUIRE_NOTHROW(node.to_string());
    REQUIRE(node.to_string().compare("test") == 0);
}

TEST_CASE("NodeClassTest_AliasMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::node tmp = fkyaml::node::boolean_scalar(true);
    tmp.add_anchor_name("anchor_name");
    fkyaml::node tmp_alias = fkyaml::node::alias_of(tmp);
    fkyaml::node alias(std::move(tmp_alias));
    REQUIRE(alias.is_boolean());
    REQUIRE_NOTHROW(alias.to_boolean());
    REQUIRE(alias.to_boolean() == true);
}

//
// test cases for factory methods
//

TEST_CASE("NodeClassTest_SequenceNodeFactoryTest", "[NodeClassTest]")
{
    SECTION("Test empty sequence node factory method.")
    {
        fkyaml::node node = fkyaml::node::sequence();
        REQUIRE(node.is_sequence());
        REQUIRE(node.size() == 0);
    }

    SECTION("Test non-empty sequence node factory methods.")
    {
        fkyaml::node_sequence_type seq(3);

        SECTION("Test lvalue sequence node factory method.")
        {
            fkyaml::node node = fkyaml::node::sequence(seq);
            REQUIRE(node.is_sequence());
            REQUIRE(node.size() == 3);
            for (int i = 0; i < 3; ++i)
            {
                REQUIRE(node[i].is_null());
            }
        }

        SECTION("Test lvalue sequence node factory method.")
        {
            fkyaml::node node = fkyaml::node::sequence(std::move(seq));
            REQUIRE(node.is_sequence());
            REQUIRE(node.size() == 3);
            for (int i = 0; i < 3; ++i)
            {
                REQUIRE(node[i].is_null());
            }
        }
    }
}

TEST_CASE("NodeClassTest_MappingNodeFactoryTest", "[NodeClassTest]")
{
    SECTION("Test empty mapping node factory method.")
    {
        fkyaml::node node = fkyaml::node::mapping();
        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 0);
    }

    SECTION("Test non-empty mapping node factory methods.")
    {
        fkyaml::node_mapping_type map {{std::string("test"), fkyaml::node::boolean_scalar(true)}};

        SECTION("Test lvalue mapping node factory method.")
        {
            fkyaml::node node = fkyaml::node::mapping(map);
            REQUIRE(node.is_mapping());
            REQUIRE(node.size() == 1);
            REQUIRE(node["test"].is_boolean());
            REQUIRE(node["test"].to_boolean() == true);
        }

        SECTION("Test rvalue mapping node factory method.")
        {
            fkyaml::node node = fkyaml::node::mapping(std::move(map));
            REQUIRE(node.is_mapping());
            REQUIRE(node.size() == 1);
            REQUIRE(node["test"].is_boolean());
            REQUIRE(node["test"].to_boolean() == true);
        }
    }
}

TEST_CASE("NodeClassTest_BooleanNodeFactoryTest", "[NodeClassTest]")
{
    auto boolean = GENERATE(true, false);
    fkyaml::node node = fkyaml::node::boolean_scalar(boolean);
    REQUIRE(node.is_boolean());
    REQUIRE(node.to_boolean() == boolean);
}

TEST_CASE("NodeClassTest_IntegerNodeFactoryTest", "[NodeClassTest]")
{
    auto integer = GENERATE(
        std::numeric_limits<fkyaml::node_integer_type>::min(),
        0,
        std::numeric_limits<fkyaml::node_integer_type>::max());
    fkyaml::node node = fkyaml::node::integer_scalar(integer);
    REQUIRE(node.is_integer());
    REQUIRE(node.to_integer() == integer);
}

TEST_CASE("NodeClassTest_FloatNumberNodeFactoryTest", "[NodeClassTest]")
{
    auto float_val = GENERATE(
        std::numeric_limits<fkyaml::node_float_number_type>::min(),
        3.141592,
        std::numeric_limits<fkyaml::node_float_number_type>::max());
    fkyaml::node node = fkyaml::node::float_number_scalar(float_val);
    REQUIRE(node.is_float_number());
    REQUIRE(node.to_float_number() == float_val);
}

TEST_CASE("NodeClassTest_StringNodeFactoryTest", "[NodeClassTest]")
{
    SECTION("Test empty string node factory method.")
    {
        fkyaml::node node = fkyaml::node::string_scalar();
        REQUIRE(node.is_string());
        REQUIRE(node.size() == 0);
    }

    SECTION("Test lvalue string node factory method.")
    {
        fkyaml::node_string_type str("test");
        fkyaml::node node = fkyaml::node::string_scalar(str);
        REQUIRE(node.is_string());
        REQUIRE(node.size() == str.size());
        REQUIRE(node.to_string() == str);
    }

    SECTION("Test rvalue string node factory method.")
    {
        fkyaml::node node = fkyaml::node::string_scalar("test");
        REQUIRE(node.is_string());
        REQUIRE(node.size() == 4);
        REQUIRE(node.to_string().compare("test") == 0);
    }
}

TEST_CASE("NodeClassTest_AliasNodeFactoryTest", "[NodeClassTest]")
{
    fkyaml::node anchor = fkyaml::node::string_scalar("alias_test");

    SECTION("Make sure BasicNode::alias_of() throws an exception without anchor name.")
    {
        REQUIRE_THROWS_AS(fkyaml::node::alias_of(anchor), fkyaml::exception);
    }

    SECTION("Make sure BasicNode::alias_of() throws an exception with an empty anchor name.")
    {
        anchor.add_anchor_name("");
        REQUIRE_THROWS_AS(fkyaml::node::alias_of(anchor), fkyaml::exception);
    }

    SECTION("Check if BasicNode::alias_of() does not throw any exception.")
    {
        anchor.add_anchor_name("anchor_name");
        REQUIRE_NOTHROW(fkyaml::node::alias_of(anchor));
        fkyaml::node alias = fkyaml::node::alias_of(anchor);
        REQUIRE(alias.is_string());
        REQUIRE(alias.to_string().compare("alias_test") == 0);
    }
}

//
// test cases for subscript operators
//

TEST_CASE("NodeClassTest_StringSubscriptOperatorTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected string subscript operators.")
    {
        fkyaml::node_mapping_type map {{"test", fkyaml::node()}};

        SECTION("Test the non-const string subscript operators.")
        {
            fkyaml::node node = fkyaml::node::mapping(map);

            SECTION("Test the non-const lvalue string subscript operator.")
            {
                std::string key = "test";
                REQUIRE_NOTHROW(node[key]);
                REQUIRE(node[key].is_null());
            }

            SECTION("Test the non-const alias lvalue string subscript operator.")
            {
                std::string key = "test";
                node.add_anchor_name("anchor_name");
                fkyaml::node alias = fkyaml::node::alias_of(node);
                REQUIRE_NOTHROW(alias[key]);
                REQUIRE(alias[key].is_null());
            }

            SECTION("Test the non-const rvalue string subscript operator.")
            {
                REQUIRE_NOTHROW(node["test"]);
                REQUIRE(node["test"].is_null());
            }

            SECTION("Test the const alias lvalue string subscript operator.")
            {
                std::string key = "test";
                node.add_anchor_name("anchor_name");
                const fkyaml::node alias = fkyaml::node::alias_of(node);
                REQUIRE_NOTHROW(alias[key]);
                REQUIRE(alias[key].is_null());
            }
        }

        SECTION("Test the const string subscript operators.")
        {
            const fkyaml::node node = fkyaml::node::mapping(map);
            std::string key = "test";

            SECTION("Test the const lvalue string subscript operator.")
            {
                REQUIRE_NOTHROW(node[key]);
                REQUIRE(node[key].is_null());
            }

            SECTION("Test the const rvalue string subscript operator.")
            {
                REQUIRE_NOTHROW(node["test"]);
                REQUIRE(node["test"].is_null());
            }
        }
    }

    SECTION("Test throwing expected string subscript operator.")
    {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0),
            fkyaml::node::string_scalar());

        SECTION("Test non-const lvalue throwing invocation.")
        {
            std::string key = "test";
            REQUIRE_THROWS_AS(node[key], fkyaml::exception);
        }

        SECTION("Test const lvalue throwing invocation.")
        {
            std::string key = "test";
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node[key], fkyaml::exception);
        }

        SECTION("Test non-const rvalue throwing invocation.")
        {
            REQUIRE_THROWS_AS(node["test"], fkyaml::exception);
        }

        SECTION("Test const rvalue throwing invocation.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node["test"], fkyaml::exception);
        }
    }
}

TEST_CASE("NodeClassTest_IntegerSubscriptOperatorTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected integer subscript operators.")
    {
        fkyaml::node node = fkyaml::node::sequence();
        node.to_sequence().emplace_back();

        SECTION("Test non-const non-alias integer subscript operators")
        {
            REQUIRE_NOTHROW(node[0]);
        }

        SECTION("Test non-const alias integer subscript operators")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias[0]);
        }

        SECTION("Test const non-alias integer subscript operators")
        {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node[0]);
        }

        SECTION("Test const alias integer subscript operators")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias[0]);
        }
    }

    SECTION("Test throwing expected integer subscript operator.")
    {
        auto node = GENERATE(
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0),
            fkyaml::node::string_scalar());

        SECTION("Test non-const non-sequence nodes.")
        {
            REQUIRE_THROWS_AS(node[0], fkyaml::exception);
        }

        SECTION("Test const non-sequence nodes.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node[0], fkyaml::exception);
        }
    }
}

//
// test cases for type property getter/checkers
//

TEST_CASE("NodeClassTest_TypeGetterTest", "[NodeClassTest]")
{
    using NodeTypePair = std::pair<fkyaml::node, fkyaml::node::node_t>;
    auto type_pair = GENERATE(
        NodeTypePair(fkyaml::node::sequence(), fkyaml::node::node_t::SEQUENCE),
        NodeTypePair(fkyaml::node::mapping(), fkyaml::node::node_t::MAPPING),
        NodeTypePair(fkyaml::node(), fkyaml::node::node_t::NULL_OBJECT),
        NodeTypePair(fkyaml::node::boolean_scalar(false), fkyaml::node::node_t::BOOLEAN),
        NodeTypePair(fkyaml::node::integer_scalar(0), fkyaml::node::node_t::INTEGER),
        NodeTypePair(fkyaml::node::float_number_scalar(0.0), fkyaml::node::node_t::FLOAT_NUMBER),
        NodeTypePair(fkyaml::node::string_scalar(), fkyaml::node::node_t::STRING));

    SECTION("Test non-alias node types.")
    {
        REQUIRE(type_pair.first.type() == type_pair.second);
    }

    SECTION("Test alias node types.")
    {
        type_pair.first.add_anchor_name("anchor_name");
        fkyaml::node alias = fkyaml::node::alias_of(type_pair.first);
        REQUIRE(alias.type() == type_pair.second);
    }
}

TEST_CASE("NodeClassTest_is_sequenceTest", "[NodeClassTest]")
{
    SECTION("Test sequence node type.")
    {
        fkyaml::node node = fkyaml::node::sequence();

        SECTION("Test non-alias sequence node type.")
        {
            REQUIRE(node.is_sequence());
        }

        SECTION("Test alias sequence node type.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_sequence());
        }
    }

    SECTION("Test non-sequence node types.")
    {
        auto node = GENERATE(
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0),
            fkyaml::node::string_scalar());

        SECTION("Test non-alias non-sequence node types")
        {
            REQUIRE_FALSE(node.is_sequence());
        }

        SECTION("Test alias non-sequence node types.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_sequence());
        }
    }
}

TEST_CASE("NodeClassTest_is_mappingTest", "[NodeClassTest]")
{
    SECTION("Test mapping node type.")
    {
        fkyaml::node node = fkyaml::node::mapping();

        SECTION("Test non-alias mapping node type.")
        {
            REQUIRE(node.is_mapping());
        }

        SECTION("Test alias mapping node type.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_mapping());
        }
    }

    SECTION("Test non-mapping node types.")
    {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0),
            fkyaml::node::string_scalar());

        SECTION("Test non-alias non-mapping node types")
        {
            REQUIRE_FALSE(node.is_mapping());
        }

        SECTION("Test alias non-mapping node types.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_mapping());
        }
    }
}

TEST_CASE("NodeClassTest_is_nullTest", "[NodeClassTest]")
{
    SECTION("Test null node type.")
    {
        fkyaml::node node = fkyaml::node();

        SECTION("Test non-alias null node type.")
        {
            REQUIRE(node.is_null());
        }

        SECTION("Test alias null node type.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_null());
        }
    }

    SECTION("Test non-null node types.")
    {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0),
            fkyaml::node::string_scalar());

        SECTION("Test non-alias non-null node types")
        {
            REQUIRE_FALSE(node.is_null());
        }

        SECTION("Test alias non-null node types.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_null());
        }
    }
}

TEST_CASE("NodeClassTest_is_booleanTest", "[NodeClassTest]")
{
    SECTION("Test boolean node type.")
    {
        fkyaml::node node = fkyaml::node::boolean_scalar(false);

        SECTION("Test non-alias boolean node type.")
        {
            REQUIRE(node.is_boolean());
        }

        SECTION("Test alias boolean node type.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_boolean());
        }
    }

    SECTION("Test non-boolean node types.")
    {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0),
            fkyaml::node::string_scalar());

        SECTION("Test non-alias non-boolean node types")
        {
            REQUIRE_FALSE(node.is_boolean());
        }

        SECTION("Test alias non-boolean node types.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_boolean());
        }
    }
}

TEST_CASE("NodeClassTest_is_integerTest", "[NodeClassTest]")
{
    SECTION("Test integer node type.")
    {
        fkyaml::node node = fkyaml::node::integer_scalar(0);

        SECTION("Test non-alias integer node type.")
        {
            REQUIRE(node.is_integer());
        }

        SECTION("Test alias integer node type.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_integer());
        }
    }

    SECTION("Test non-integer node types.")
    {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::float_number_scalar(0.0),
            fkyaml::node::string_scalar());

        SECTION("Test non-alias non-integer node types")
        {
            REQUIRE_FALSE(node.is_integer());
        }

        SECTION("Test alias non-integer node types.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_integer());
        }
    }
}

TEST_CASE("NodeClassTest_IsFloatNumberTest", "[NodeClassTest]")
{
    SECTION("Test float number node type.")
    {
        fkyaml::node node = fkyaml::node::float_number_scalar(0.0);

        SECTION("Test non-alias float number node type.")
        {
            REQUIRE(node.is_float_number());
        }

        SECTION("Test alias float number node type.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_float_number());
        }
    }

    SECTION("Test non-float-number node types.")
    {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::string_scalar());

        SECTION("Test non-alias non-float-number node types")
        {
            REQUIRE_FALSE(node.is_float_number());
        }

        SECTION("Test alias non-float-number node types.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_float_number());
        }
    }
}

TEST_CASE("NodeClassTest_IsStringTest", "[NodeClassTest]")
{
    SECTION("Test string node type.")
    {
        fkyaml::node node = fkyaml::node::string_scalar();

        SECTION("Test non-alias string node type.")
        {
            REQUIRE(node.is_string());
        }

        SECTION("Test alias string node type.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_string());
        }
    }

    SECTION("Test non-string node types.")
    {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0));

        SECTION("Test non-alias non-string node types")
        {
            REQUIRE_FALSE(node.is_string());
        }

        SECTION("Test alias non-string node types.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_string());
        }
    }
}

TEST_CASE("NodeClassTest_is_scalarTest", "[NodeClassTest]")
{
    SECTION("Test scalar node types.")
    {
        auto node = GENERATE(
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0),
            fkyaml::node::string_scalar());

        SECTION("Test non-alias scalar node types.")
        {
            REQUIRE(node.is_scalar());
        }

        SECTION("Test alias scalar node types.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_scalar());
        }
    }

    SECTION("Test non-scalar node types.")
    {
        auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping());

        SECTION("Test non-alias non-scalar node types")
        {
            REQUIRE_FALSE(node.is_scalar());
        }

        SECTION("Test alias non-scalar node types.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_scalar());
        }
    }
}

TEST_CASE("NodeClassTest_IsAliasTest", "[NodeClassTest]")
{
    auto node = GENERATE(
        fkyaml::node::sequence(),
        fkyaml::node::mapping(),
        fkyaml::node(),
        fkyaml::node::boolean_scalar(false),
        fkyaml::node::integer_scalar(0),
        fkyaml::node::float_number_scalar(0.0),
        fkyaml::node::string_scalar());

    SECTION("Test alias node types.")
    {
        node.add_anchor_name("anchor_name");
        fkyaml::node alias = fkyaml::node::alias_of(node);
    }
}

//
// test cases for emptiness checker
//

TEST_CASE("NodeClassTest_emptyTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected node emptiness.")
    {
        SECTION("Test empty container node emptiness.")
        {
            auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping(), fkyaml::node::string_scalar());

            SECTION("Test empty non-alias container node emptiness.")
            {
                REQUIRE_NOTHROW(node.empty());
                REQUIRE(node.empty());
            }

            SECTION("Test empty alias container node emptiness.")
            {
                node.add_anchor_name("anchor_name");
                fkyaml::node alias = fkyaml::node::alias_of(node);
                REQUIRE_NOTHROW(alias.empty());
                REQUIRE(alias.empty());
            }
        }

        SECTION("Test non-empty container node emptiness.")
        {
            auto node = GENERATE(
                fkyaml::node::sequence(fkyaml::node_sequence_type(3)),
                fkyaml::node::mapping(fkyaml::node_mapping_type {{"test", fkyaml::node()}}),
                fkyaml::node::string_scalar("test"));

            SECTION("Test non-empty non-alias container node emptiness.")
            {
                REQUIRE_NOTHROW(node.empty());
                REQUIRE_FALSE(node.empty());
            }

            SECTION("Test non-empty alias container node emptiness.")
            {
                node.add_anchor_name("anchor_name");
                fkyaml::node alias = fkyaml::node::alias_of(node);
                REQUIRE_NOTHROW(alias.empty());
                REQUIRE_FALSE(alias.empty());
            }
        }
    }

    SECTION("Test nothrow unexpected node emptiness.")
    {
        auto node = GENERATE(
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0));

        SECTION("Test non-const non-alias non-container node emptiness.")
        {
            REQUIRE_THROWS_AS(node.empty(), fkyaml::exception);
        }

        SECTION("Test const non-alias non-container node emptiness.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.empty(), fkyaml::exception);
        }

        SECTION("Test non-const alias non-container node emptiness.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.empty(), fkyaml::exception);
        }

        SECTION("Test const alias non-container node emptiness.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.empty(), fkyaml::exception);
        }
    }
}

//
// test cases for mapping key existence checker
//

TEST_CASE("NodeClassTest_ContainsTest", "[NodeClassTest]")
{
    SECTION("Test mapping node.")
    {
        fkyaml::node node = fkyaml::node::mapping({{"test", fkyaml::node()}});
        std::string key = "test";

        SECTION("Test non-alias mapping node with lvalue key.")
        {
            REQUIRE(node.contains(key));
        }

        SECTION("Test alias mapping node with lvalue key.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(node.contains(key));
        }

        SECTION("Test non-alias mapping node with rvalue key.")
        {
            REQUIRE(node.contains(std::move(key)));
        }

        SECTION("Test alias mapping node with rvalue key.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(node.contains(std::move(key)));
        }
    }

    SECTION("Test non-mapping node.")
    {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0),
            fkyaml::node::string_scalar());
        std::string key = "test";

        SECTION("Test non-alias non-mapping node with lvalue key.")
        {
            REQUIRE_FALSE(node.contains(key));
        }

        SECTION("Test alias non-mapping node with lvalue key.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.contains(key));
        }

        SECTION("Test non-alias non-mapping node with rvalue key.")
        {
            REQUIRE_FALSE(node.contains(std::move(key)));
        }

        SECTION("Test alias non-mapping node with rvalue key.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.contains(std::move(key)));
        }
    }
}

//
// test cases for container size getter
//

TEST_CASE("NodeClassTest_sizeGetterTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected node size.")
    {
        auto node = GENERATE(
            fkyaml::node::sequence({fkyaml::node(), fkyaml::node(), fkyaml::node()}),
            fkyaml::node::mapping({{"test0", fkyaml::node()}, {"test1", fkyaml::node()}, {"test2", fkyaml::node()}}),
            fkyaml::node::string_scalar("tmp"));

        SECTION("Test container node size.")
        {
            REQUIRE_NOTHROW(node.size());
            REQUIRE(node.size() == 3);
        }

        SECTION("Test const container node size.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(node.size());
            REQUIRE(const_node.size() == 3);
        }

        SECTION("Test alias container node size.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.size());
            REQUIRE(alias.size() == 3);
        }

        SECTION("Test const alias container node size.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.size());
            REQUIRE(alias.size() == 3);
        }
    }

    SECTION("Test nothrow unexpected node size.")
    {
        auto node = GENERATE(
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0));

        SECTION("Test non-const non-alias non-container node size.")
        {
            REQUIRE_THROWS_AS(node.size(), fkyaml::exception);
        }

        SECTION("Test const non-alias non-container node size.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.size(), fkyaml::exception);
        }

        SECTION("Test non-const alias non-container node size.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.size(), fkyaml::exception);
        }

        SECTION("Test const alias non-container node size.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.size(), fkyaml::exception);
        }
    }
}

//
// test cases for YAML version property getter/setter
//

TEST_CASE("NodeClassTest_SetVersionTest", "[NodeClassTest]")
{
    fkyaml::node node;
    node.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_1);
    REQUIRE(node.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_1);

    node.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_2);
    REQUIRE(node.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_2);
}

TEST_CASE("NodeClassTest_GetVersionTest", "[NodeClassTest]")
{
    fkyaml::node node;
    REQUIRE(node.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_2);

    node.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_1);
    REQUIRE(node.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_1);
}

//
// test cases for anchor name property checker/getter/setter
//

TEST_CASE("NodeClassTest_HasAnchorNameTest", "[NodeClassTest]")
{
    fkyaml::node node;

    SECTION("Test node without anchor name.")
    {
        REQUIRE_FALSE(node.has_anchor_name());
    }

    SECTION("Test node wityh anchor name.")
    {
        node.add_anchor_name("anchor_name");
        REQUIRE(node.has_anchor_name());
    }
}

TEST_CASE("NodeClassTest_GetAnchorNameTest", "[NodeClassTest]")
{
    fkyaml::node node;

    SECTION("Test node without anchor name.")
    {
        REQUIRE_THROWS_AS(node.get_anchor_name(), fkyaml::exception);
    }

    SECTION("Test node with anchor name.")
    {
        node.add_anchor_name("anchor_name");
        REQUIRE_NOTHROW(node.get_anchor_name());
        REQUIRE(node.get_anchor_name().compare("anchor_name") == 0);
    }
}

TEST_CASE("NodeClassTest_add_anchor_nameTest", "[NodeClassTest]")
{
    fkyaml::node node;
    std::string anchor_name = "anchor_name";

    SECTION("Test lvalue anchor name.")
    {
        node.add_anchor_name(anchor_name);
        REQUIRE_NOTHROW(node.get_anchor_name());
        REQUIRE(node.get_anchor_name().compare("anchor_name") == 0);
    }

    SECTION("Test rvalue anchor name.")
    {
        node.add_anchor_name(std::move(anchor_name));
        REQUIRE_NOTHROW(node.get_anchor_name());
        REQUIRE(node.get_anchor_name().compare("anchor_name") == 0);
    }

    SECTION("Test overwritten anchor name.")
    {
        node.add_anchor_name(anchor_name);
        node.add_anchor_name("overwritten_name");
        REQUIRE_NOTHROW(node.get_anchor_name());
        REQUIRE_FALSE(node.get_anchor_name().compare("anchor_name") == 0);
        REQUIRE(node.get_anchor_name().compare("overwritten_name") == 0);
    }
}

//
// test cases for value getters (copy)
//

TEST_CASE("NodeClassTest_GetValueTest", "[NodeClassTest]")
{
    SECTION("test sequence node value.")
    {
        fkyaml::node node(fkyaml::node_sequence_type {fkyaml::node(true), fkyaml::node(false)});

        SECTION("test for sequence value.")
        {
            auto seq = node.get_value<fkyaml::node_sequence_type>();
            REQUIRE(seq.size() == 2);
            REQUIRE(seq[0].is_boolean());
            REQUIRE(seq[0].get_value<bool>() == true);
            REQUIRE(seq[1].is_boolean());
            REQUIRE(seq[1].get_value<bool>() == false);
        }

        SECTION("test for non-sequence value.")
        {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_mapping_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_boolean_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_integer_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_float_number_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_string_type>(), fkyaml::exception);
        }
    }

    SECTION("test mapping node value.")
    {
        fkyaml::node node(
            fkyaml::node_mapping_type {{"test", fkyaml::node(3.14)}, {"foo", fkyaml::node(std::string("bar"))}});

        SECTION("test for mapping value.")
        {
            auto map = node.get_value<fkyaml::node_mapping_type>();
            REQUIRE(map.size() == 2);
            REQUIRE(map.find("test") != map.end());
            REQUIRE(map.at("test").is_float_number());
            REQUIRE(map.at("test").get_value<double>() == 3.14);
            REQUIRE(map.find("foo") != map.end());
            REQUIRE(map.at("foo").is_string());
            REQUIRE(map.at("foo").get_value<std::string>() == "bar");
        }

        SECTION("test for non-mapping values.")
        {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_sequence_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_boolean_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_integer_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_float_number_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_string_type>(), fkyaml::exception);
        }
    }

    SECTION("test null node value.")
    {
        fkyaml::node node(nullptr);

        SECTION("test for null value.")
        {
            auto null = node.get_value<std::nullptr_t>();
            REQUIRE(null == nullptr);
        }

        SECTION("test for non-null values.")
        {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_sequence_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_mapping_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_boolean_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_integer_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_float_number_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_string_type>(), fkyaml::exception);
        }
    }

    SECTION("test boolean node value.")
    {
        fkyaml::node node(true);

        SECTION("test for boolean value.")
        {
            REQUIRE(node.get_value<bool>() == true);
        }

        SECTION("test for non-boolean values.")
        {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_sequence_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_mapping_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_integer_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_float_number_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_string_type>(), fkyaml::exception);
        }
    }

    SECTION("test integer node value.")
    {
        fkyaml::node node(123);

        SECTION("test for integer values.")
        {
            REQUIRE(node.get_value<int8_t>() == 123);
            REQUIRE(node.get_value<int16_t>() == 123);
            REQUIRE(node.get_value<int32_t>() == 123);
            REQUIRE(node.get_value<int64_t>() == 123);
            REQUIRE(node.get_value<uint8_t>() == 123);
            REQUIRE(node.get_value<uint16_t>() == 123);
            REQUIRE(node.get_value<uint32_t>() == 123);
            // TODO: REQUIRE(node.get_value<uint64_t>() == 123);
        }

        SECTION("test for non-integer values.")
        {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_sequence_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_mapping_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_boolean_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_float_number_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_string_type>(), fkyaml::exception);
        }

        SECTION("test for non-integer node value.")
        {
            fkyaml::node non_int_node(true);
            REQUIRE_THROWS_AS(non_int_node.get_value<int32_t>(), fkyaml::exception);
        }

        SECTION("test underflowable integer type.")
        {
            fkyaml::node negative_int_node(std::numeric_limits<fkyaml::node_integer_type>::min());
            REQUIRE_THROWS_AS(negative_int_node.get_value<int8_t>(), fkyaml::exception);
        }

        SECTION("test overflowable integer type.")
        {
            fkyaml::node large_int_node(std::numeric_limits<fkyaml::node_integer_type>::max());
            REQUIRE_THROWS_AS(large_int_node.get_value<int8_t>(), fkyaml::exception);
        }
    }

    SECTION("test float number node value.")
    {
        fkyaml::node node(3.14);

        SECTION("test for float number values.")
        {
            REQUIRE(fabsf(node.get_value<float>() - 3.14) < FLT_EPSILON);
            REQUIRE(fabsf(node.get_value<double>() - 3.14) < DBL_EPSILON);
            REQUIRE(fabsf(node.get_value<long double>() - 3.14) < LDBL_EPSILON);
        }

        SECTION("test for non-float-number values.")
        {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_sequence_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_mapping_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_boolean_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_integer_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_string_type>(), fkyaml::exception);
        }

        SECTION("test for non-float-number node value.")
        {
            fkyaml::node non_float_num_node(true);
            REQUIRE_THROWS_AS(non_float_num_node.get_value<float>(), fkyaml::exception);
        }

        SECTION("test underflowable float number type.")
        {
            fkyaml::node negative_float_node(std::numeric_limits<fkyaml::node_float_number_type>::min());
            REQUIRE_THROWS_AS(negative_float_node.get_value<float>(), fkyaml::exception);
        }

        SECTION("test overflowable float number type.")
        {
            fkyaml::node large_float_node(std::numeric_limits<fkyaml::node_float_number_type>::max());
            REQUIRE_THROWS_AS(large_float_node.get_value<float>(), fkyaml::exception);
        }
    }

    SECTION("test string node value.")
    {
        fkyaml::node node(std::string("test"));

        SECTION("test for string value.")
        {
            auto str = node.get_value<std::string>();
            REQUIRE(str.size() == 4);
            REQUIRE(str == "test");
        }

        SECTION("test for non-string values.")
        {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_sequence_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_mapping_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_boolean_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_integer_type>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node_float_number_type>(), fkyaml::exception);
        }
    }
}

//
// test cases for value reference getters
//

TEST_CASE("NodeClassTest_ToSequenceTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected nodes.")
    {
        fkyaml::node node =
            fkyaml::node::sequence(fkyaml::node_sequence_type {fkyaml::node(), fkyaml::node(), fkyaml::node()});

        SECTION("Test non-alias sequence node.")
        {
            REQUIRE_NOTHROW(node.to_sequence());
            REQUIRE(node.to_sequence().size() == 3);
            for (int i = 0; i < 3; ++i)
            {
                REQUIRE(node.to_sequence()[i].is_null());
            }
        }

        SECTION("Test const non-alias sequence node.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.to_sequence());
            REQUIRE(const_node.to_sequence().size() == 3);
            for (int i = 0; i < 3; ++i)
            {
                REQUIRE(node.to_sequence()[i].is_null());
            }
        }

        SECTION("Test alias sequence node.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.to_sequence());
            REQUIRE(alias.to_sequence().size() == 3);
            for (int i = 0; i < 3; ++i)
            {
                REQUIRE(alias.to_sequence()[i].is_null());
            }
        }

        SECTION("Test const alias sequence node.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.to_sequence());
            REQUIRE(alias.to_sequence().size() == 3);
            for (int i = 0; i < 3; ++i)
            {
                REQUIRE(alias.to_sequence()[i].is_null());
            }
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0),
            fkyaml::node::string_scalar());

        SECTION("Test non-alias non-sequence nodes.")
        {
            REQUIRE_THROWS_AS(node.to_sequence(), fkyaml::exception);
        }

        SECTION("Test const non-alias non-sequence nodes.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.to_sequence(), fkyaml::exception);
        }

        SECTION("Test alias non-sequence nodes.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.to_sequence(), fkyaml::exception);
        }

        SECTION("Test const alias non-sequence nodes.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.to_sequence(), fkyaml::exception);
        }
    }
}

TEST_CASE("NodeClassTest_ToMappingTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected nodes.")
    {
        fkyaml::node node = fkyaml::node::mapping(fkyaml::node_mapping_type {
            {"test0", fkyaml::node()}, {"test1", fkyaml::node()}, {"test2", fkyaml::node()}});

        SECTION("Test non-alias mapping node.")
        {
            REQUIRE_NOTHROW(node.to_mapping());
            REQUIRE(node.to_mapping().size() == 3);
            REQUIRE(node["test0"].is_null());
            REQUIRE(node["test1"].is_null());
            REQUIRE(node["test2"].is_null());
        }

        SECTION("Test const non-alias mapping node.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.to_mapping());
            REQUIRE(const_node.to_mapping().size() == 3);
            REQUIRE(const_node["test0"].is_null());
            REQUIRE(const_node["test1"].is_null());
            REQUIRE(const_node["test2"].is_null());
        }

        SECTION("Test alias mapping node.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.to_mapping());
            REQUIRE(alias.to_mapping().size() == 3);
            REQUIRE(alias["test0"].is_null());
            REQUIRE(alias["test1"].is_null());
            REQUIRE(alias["test2"].is_null());
        }

        SECTION("Test const alias mapping node.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.to_mapping());
            REQUIRE(alias["test0"].is_null());
            REQUIRE(alias["test1"].is_null());
            REQUIRE(alias["test2"].is_null());
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0),
            fkyaml::node::string_scalar());

        SECTION("Test non-alias non-mapping nodes.")
        {
            REQUIRE_THROWS_AS(node.to_mapping(), fkyaml::exception);
        }

        SECTION("Test const non-alias non-mapping nodes.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.to_mapping(), fkyaml::exception);
        }

        SECTION("Test alias non-mapping nodes.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.to_mapping(), fkyaml::exception);
        }

        SECTION("Test const alias non-mapping nodes.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.to_mapping(), fkyaml::exception);
        }
    }
}

TEST_CASE("NodeClassTest_ToBooleanTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected nodes.")
    {
        fkyaml::node node = fkyaml::node::boolean_scalar(true);

        SECTION("Test non-alias boolean node.")
        {
            REQUIRE_NOTHROW(node.to_boolean());
            REQUIRE(node.to_boolean() == true);
        }

        SECTION("Test const non-alias boolean node.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.to_boolean());
            REQUIRE(const_node.to_boolean() == true);
        }

        SECTION("Test alias boolean node.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.to_boolean());
            REQUIRE(alias.to_boolean() == true);
        }

        SECTION("Test const alias boolean node.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.to_boolean());
            REQUIRE(alias.to_boolean() == true);
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0),
            fkyaml::node::string_scalar());

        SECTION("Test non-alias non-boolean nodes.")
        {
            REQUIRE_THROWS_AS(node.to_boolean(), fkyaml::exception);
        }

        SECTION("Test const non-alias non-boolean nodes.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.to_boolean(), fkyaml::exception);
        }

        SECTION("Test alias non-boolean nodes.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.to_boolean(), fkyaml::exception);
        }

        SECTION("Test const alias non-boolean nodes.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.to_boolean(), fkyaml::exception);
        }
    }
}

TEST_CASE("NodeClassTest_ToIntegerTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected nodes.")
    {
        fkyaml::node_integer_type integer = -123;
        fkyaml::node node = fkyaml::node::integer_scalar(integer);

        SECTION("Test non-alias  integer node.")
        {
            REQUIRE_NOTHROW(node.to_integer());
            REQUIRE(node.to_integer() == integer);
        }

        SECTION("Test const non-alias  integer node.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.to_integer());
            REQUIRE(const_node.to_integer() == integer);
        }

        SECTION("Test alias  integer node.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.to_integer());
            REQUIRE(alias.to_integer() == integer);
        }

        SECTION("Test const alias  integer node.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.to_integer());
            REQUIRE(alias.to_integer() == integer);
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::float_number_scalar(0.0),
            fkyaml::node::string_scalar());

        SECTION("Test non-alias non-integer nodes.")
        {
            REQUIRE_THROWS_AS(node.to_integer(), fkyaml::exception);
        }

        SECTION("Test const non-alias non-integer nodes.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.to_integer(), fkyaml::exception);
        }

        SECTION("Test alias non-integer nodes.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.to_integer(), fkyaml::exception);
        }

        SECTION("Test const alias non-integer nodes.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.to_integer(), fkyaml::exception);
        }
    }
}

TEST_CASE("NodeClassTest_ToFloatNumberTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected nodes.")
    {
        fkyaml::node_float_number_type float_val = 123.45;
        fkyaml::node node = fkyaml::node::float_number_scalar(float_val);

        SECTION("Test non-alias float number node.")
        {
            REQUIRE_NOTHROW(node.to_float_number());
            REQUIRE(node.to_float_number() == float_val);
        }

        SECTION("Test const non-alias float number node.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.to_float_number());
            REQUIRE(const_node.to_float_number() == float_val);
        }

        SECTION("Test alias float number node.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.to_float_number());
            REQUIRE(alias.to_float_number() == float_val);
        }

        SECTION("Test const alias float number node.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.to_float_number());
            REQUIRE(alias.to_float_number() == float_val);
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::string_scalar());

        SECTION("Test non-alias non-float-number nodes.")
        {
            REQUIRE_THROWS_AS(node.to_float_number(), fkyaml::exception);
        }

        SECTION("Test const non-alias non-float-number nodes.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.to_float_number(), fkyaml::exception);
        }

        SECTION("Test alias non-float-number nodes.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.to_float_number(), fkyaml::exception);
        }

        SECTION("Test const alias non-float-number nodes.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.to_float_number(), fkyaml::exception);
        }
    }
}

TEST_CASE("NodeClassTest_ToStringTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected nodes.")
    {
        fkyaml::node_string_type str = "test";
        fkyaml::node node = fkyaml::node::string_scalar(str);

        SECTION("Test non-alias string node.")
        {
            REQUIRE_NOTHROW(node.to_string());
            REQUIRE(node.to_string() == str);
        }

        SECTION("Test const non-alias string node.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.to_string());
            REQUIRE(const_node.to_string() == str);
        }

        SECTION("Test alias string node.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.to_string());
            REQUIRE(alias.to_string() == str);
        }

        SECTION("Test const alias string node.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.to_string());
            REQUIRE(alias.to_string() == str);
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0));

        SECTION("Test non-alias non-string nodes.")
        {
            REQUIRE_THROWS_AS(node.to_string(), fkyaml::exception);
        }

        SECTION("Test const non-alias non-string nodes.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.to_string(), fkyaml::exception);
        }

        SECTION("Test alias non-string nodes.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.to_string(), fkyaml::exception);
        }

        SECTION("Test const alias non-string nodes.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.to_string(), fkyaml::exception);
        }
    }
}

//
// test cases for iterator getters
//

TEST_CASE("NodeClassTest_BeginTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected nodes.")
    {
        auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping());

        SECTION("Test non-const non-alias container node.")
        {
            REQUIRE_NOTHROW(node.begin());
        }

        SECTION("Test const non-alias container node.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.begin());
        }

        SECTION("Test non-const alias container node.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.begin());
        }

        SECTION("Test non-const alias container node.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.begin());
        }

        SECTION("Test non-const range-based for-loop compatibility.")
        {
            REQUIRE_NOTHROW(node.begin());
        }

        SECTION("Test const range-based for-loop compatibility.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.begin());
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0),
            fkyaml::node::string_scalar());

        SECTION("Test non-const throwing node.")
        {
            REQUIRE_THROWS_AS(node.begin(), fkyaml::exception);
        }

        SECTION("Test const throwing node.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.begin(), fkyaml::exception);
        }
    }
}

TEST_CASE("NodeClassTest_EndTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected nodes.")
    {
        auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping());

        SECTION("Test non-const non-alias container node.")
        {
            REQUIRE_NOTHROW(node.end());
        }

        SECTION("Test const non-alias container node.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.end());
        }

        SECTION("Test non-const alias container node.")
        {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.end());
        }

        SECTION("Test non-const alias container node.")
        {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.end());
        }

        SECTION("Test non-const range-based for-loop compatibility.")
        {
            REQUIRE_NOTHROW(node.end());
        }

        SECTION("Test const range-based for-loop compatibility.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.end());
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::node(),
            fkyaml::node::boolean_scalar(false),
            fkyaml::node::integer_scalar(0),
            fkyaml::node::float_number_scalar(0.0),
            fkyaml::node::string_scalar());

        SECTION("Test non-const throwing node.")
        {
            REQUIRE_THROWS_AS(node.end(), fkyaml::exception);
        }

        SECTION("Test const throwing node.")
        {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.end(), fkyaml::exception);
        }
    }
}

//
// test cases for swap
//

TEST_CASE("NodeClassTest_SwapTest", "[NodeClassTest]")
{
    fkyaml::node lhs_node = fkyaml::node::boolean_scalar(true);
    fkyaml::node rhs_node = fkyaml::node::integer_scalar(123);
    lhs_node.swap(rhs_node);
    REQUIRE(lhs_node.is_integer());
    REQUIRE(lhs_node.to_integer() == 123);
    REQUIRE(rhs_node.is_boolean());
    REQUIRE(rhs_node.to_boolean() == true);
}

TEST_CASE("NodeClassTest_StdSwapTest", "[NodeClassTest]")
{
    fkyaml::node lhs_node = fkyaml::node::boolean_scalar(true);
    fkyaml::node rhs_node = fkyaml::node::integer_scalar(123);
    std::swap(lhs_node, rhs_node);
    REQUIRE(lhs_node.is_integer());
    REQUIRE(lhs_node.to_integer() == 123);
    REQUIRE(rhs_node.is_boolean());
    REQUIRE(rhs_node.to_boolean() == true);
}
