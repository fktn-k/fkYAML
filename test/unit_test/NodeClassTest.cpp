//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.1
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <map>

#include "catch2/catch.hpp"

#include "fkYAML/Node.hpp"

//
// test cases for constructors
//

TEST_CASE("NodeClassTest_DefaultCtorTest", "[NodeClassTest]")
{
    fkyaml::Node node;
    REQUIRE(node.IsNull());
}

TEST_CASE("NodeClassTest_SequenceTypeCtorTest", "[NodeClassTest]")
{
    fkyaml::Node node(fkyaml::NodeType::SEQUENCE);
    REQUIRE(node.IsSequence());
    REQUIRE(node.Size() == 0);
}

TEST_CASE("NodeClassTest_MappingTypeCtorTest", "[NodeClassTest]")
{
    fkyaml::Node node(fkyaml::NodeType::MAPPING);
    REQUIRE(node.IsMapping());
    REQUIRE(node.Size() == 0);
}

TEST_CASE("NodeClassTest_NullTypeCtorTest", "[NodeClassTest]")
{
    fkyaml::Node node(fkyaml::NodeType::NULL_OBJECT);
    REQUIRE(node.IsNull());
}

TEST_CASE("NodeClassTest_BooleanTypeCtorTest", "[NodeClassTest]")
{
    fkyaml::Node node(fkyaml::NodeType::BOOLEAN);
    REQUIRE(node.IsBoolean());
    REQUIRE(node.ToBoolean() == false);
}

TEST_CASE("NodeClassTest_IntegerTypeCtorTest", "[NodeClassTest]")
{
    fkyaml::Node node(fkyaml::NodeType::INTEGER);
    REQUIRE(node.IsInteger());
    REQUIRE(node.ToInteger() == 0);
}

TEST_CASE("NodeClassTest_FloatNumberTypeCtorTest", "[NodeClassTest]")
{
    fkyaml::Node node(fkyaml::NodeType::FLOAT_NUMBER);
    REQUIRE(node.IsFloatNumber());
    REQUIRE(node.ToFloatNumber() == 0.0);
}

TEST_CASE("NodeClassTest_StringTypeCtorTest", "[NodeClassTest]")
{
    fkyaml::Node node(fkyaml::NodeType::STRING);
    REQUIRE(node.IsString());
    REQUIRE(node.Size() == 0);
}

TEST_CASE("NodeClassTest_ThrowingSpecializationTypeCtorTest", "[NodeClassTest]")
{
    struct String
    {
        String()
        {
            throw fkyaml::Exception();
        }
    };

    using NodeType = fkyaml::BasicNode<std::vector, std::map, bool, int64_t, double, String>;
    REQUIRE_THROWS_AS(NodeType::StringScalar(), fkyaml::Exception);
}

TEST_CASE("NodeClassTest_SequenceCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::Node copied =
        fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(true), fkyaml::Node::StringScalar("test")});
    fkyaml::Node node(copied);
    REQUIRE(node.IsSequence());
    REQUIRE_NOTHROW(node.Size());
    REQUIRE(node.Size() == 2);
    REQUIRE_NOTHROW(node[0]);
    REQUIRE(node[0].IsBoolean());
    REQUIRE_NOTHROW(node[0].ToBoolean());
    REQUIRE(node[0].ToBoolean() == true);
    REQUIRE_NOTHROW(node[1]);
    REQUIRE(node[1].IsString());
    REQUIRE_NOTHROW(node[1].ToString());
    REQUIRE_NOTHROW(node[1].ToString().size());
    REQUIRE(node[1].ToString().size() == 4);
    REQUIRE(node[1].ToString().compare("test") == 0);
}

TEST_CASE("NodeClassTest_MappingCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::Node copied = fkyaml::Node::Mapping(
        {{"test0", fkyaml::Node::IntegerScalar(123)}, {"test1", fkyaml::Node::FloatNumberScalar(3.14)}});
    fkyaml::Node node(copied);
    REQUIRE(node.IsMapping());
    REQUIRE_NOTHROW(node.Size());
    REQUIRE(node.Size() == 2);
    REQUIRE_NOTHROW(node["test0"]);
    REQUIRE(node["test0"].IsInteger());
    REQUIRE_NOTHROW(node["test0"].ToInteger());
    REQUIRE(node["test0"].ToInteger() == 123);
    REQUIRE_NOTHROW(node["test1"]);
    REQUIRE(node["test1"].IsFloatNumber());
    REQUIRE_NOTHROW(node["test1"].ToFloatNumber());
    REQUIRE(node["test1"].ToFloatNumber() == 3.14);
}

TEST_CASE("NodeClassTest_NullCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::Node copied;
    fkyaml::Node node(copied);
    REQUIRE(node.IsNull());
}

TEST_CASE("NodeClassTest_BooleanCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::Node copied = fkyaml::Node::BooleanScalar(true);
    fkyaml::Node node(copied);
    REQUIRE(node.IsBoolean());
    REQUIRE_NOTHROW(node.ToBoolean());
    REQUIRE(node.ToBoolean() == true);
}

TEST_CASE("NodeClassTest_IntegerCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::Node copied = fkyaml::Node::IntegerScalar(123);
    fkyaml::Node node(copied);
    REQUIRE(node.IsInteger());
    REQUIRE_NOTHROW(node.ToInteger());
    REQUIRE(node.ToInteger() == 123);
}

TEST_CASE("NodeClassTest_FloatNumberCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::Node copied = fkyaml::Node::FloatNumberScalar(3.14);
    fkyaml::Node node(copied);
    REQUIRE(node.IsFloatNumber());
    REQUIRE_NOTHROW(node.ToFloatNumber());
    REQUIRE(node.ToFloatNumber() == 3.14);
}

TEST_CASE("NodeClassTest_StringCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::Node copied = fkyaml::Node::StringScalar("test");
    fkyaml::Node node(copied);
    REQUIRE(node.IsString());
    REQUIRE_NOTHROW(node.Size());
    REQUIRE(node.Size() == 4);
    REQUIRE_NOTHROW(node.ToString());
    REQUIRE(node.ToString().compare("test") == 0);
}

TEST_CASE("NodeClassTest_AliasCopyCtorTest", "[NodeClassTest]")
{
    fkyaml::Node tmp = fkyaml::Node::BooleanScalar(true);
    tmp.AddAnchorName("anchor_name");
    fkyaml::Node tmp_alias = fkyaml::Node::AliasOf(tmp);
    fkyaml::Node alias(tmp_alias);
    REQUIRE(alias.IsBoolean());
    REQUIRE_NOTHROW(alias.ToBoolean());
    REQUIRE(alias.ToBoolean() == true);
}

TEST_CASE("NodeClassTest_SequenceMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::Node moved =
        fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(true), fkyaml::Node::StringScalar("test")});
    fkyaml::Node node(std::move(moved));
    REQUIRE(node.IsSequence());
    REQUIRE_NOTHROW(node.Size());
    REQUIRE(node.Size() == 2);
    REQUIRE_NOTHROW(node[0]);
    REQUIRE(node[0].IsBoolean());
    REQUIRE_NOTHROW(node[0].ToBoolean());
    REQUIRE(node[0].ToBoolean() == true);
    REQUIRE_NOTHROW(node[1]);
    REQUIRE(node[1].IsString());
    REQUIRE_NOTHROW(node[1].ToString());
    REQUIRE_NOTHROW(node[1].ToString().size());
    REQUIRE(node[1].ToString().size() == 4);
    REQUIRE(node[1].ToString().compare("test") == 0);
}

TEST_CASE("NodeClassTest_MappingMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::Node moved = fkyaml::Node::Mapping(
        {{"test0", fkyaml::Node::IntegerScalar(123)}, {"test1", fkyaml::Node::FloatNumberScalar(3.14)}});
    fkyaml::Node node(std::move(moved));
    REQUIRE(node.IsMapping());
    REQUIRE_NOTHROW(node.Size());
    REQUIRE(node.Size() == 2);
    REQUIRE_NOTHROW(node["test0"]);
    REQUIRE(node["test0"].IsInteger());
    REQUIRE_NOTHROW(node["test0"].ToInteger());
    REQUIRE(node["test0"].ToInteger() == 123);
    REQUIRE_NOTHROW(node["test1"]);
    REQUIRE(node["test1"].IsFloatNumber());
    REQUIRE_NOTHROW(node["test1"].ToFloatNumber());
    REQUIRE(node["test1"].ToFloatNumber() == 3.14);
}

TEST_CASE("NodeClassTest_NullMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::Node moved;
    fkyaml::Node node(std::move(moved));
    REQUIRE(node.IsNull());
}

TEST_CASE("NodeClassTest_BooleanMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::Node moved = fkyaml::Node::BooleanScalar(true);
    fkyaml::Node node(std::move(moved));
    REQUIRE(node.IsBoolean());
    REQUIRE_NOTHROW(node.ToBoolean());
    REQUIRE(node.ToBoolean() == true);
}

TEST_CASE("NodeClassTest_IntegerMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::Node moved = fkyaml::Node::IntegerScalar(123);
    fkyaml::Node node(std::move(moved));
    REQUIRE(node.IsInteger());
    REQUIRE_NOTHROW(node.ToInteger());
    REQUIRE(node.ToInteger() == 123);
}

TEST_CASE("NodeClassTest_FloatNumberMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::Node moved = fkyaml::Node::FloatNumberScalar(3.14);
    fkyaml::Node node(std::move(moved));
    REQUIRE(node.IsFloatNumber());
    REQUIRE_NOTHROW(node.ToFloatNumber());
    REQUIRE(node.ToFloatNumber() == 3.14);
}

TEST_CASE("NodeClassTest_StringMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::Node moved = fkyaml::Node::StringScalar("test");
    fkyaml::Node node(std::move(moved));
    REQUIRE(node.IsString());
    REQUIRE_NOTHROW(node.Size());
    REQUIRE(node.Size() == 4);
    REQUIRE_NOTHROW(node.ToString());
    REQUIRE(node.ToString().compare("test") == 0);
}

TEST_CASE("NodeClassTest_AliasMoveCtorTest", "[NodeClassTest]")
{
    fkyaml::Node tmp = fkyaml::Node::BooleanScalar(true);
    tmp.AddAnchorName("anchor_name");
    fkyaml::Node tmp_alias = fkyaml::Node::AliasOf(tmp);
    fkyaml::Node alias(std::move(tmp_alias));
    REQUIRE(alias.IsBoolean());
    REQUIRE_NOTHROW(alias.ToBoolean());
    REQUIRE(alias.ToBoolean() == true);
}

//
// test cases for factory methods
//

TEST_CASE("NodeClassTest_SequenceNodeFactoryTest", "[NodeClassTest]")
{
    SECTION("Test empty sequence node factory method.")
    {
        fkyaml::Node node = fkyaml::Node::Sequence();
        REQUIRE(node.IsSequence());
        REQUIRE(node.Size() == 0);
    }

    SECTION("Test non-empty sequence node factory methods.")
    {
        fkyaml::NodeSequenceType seq(3);

        SECTION("Test lvalue sequence node factory method.")
        {
            fkyaml::Node node = fkyaml::Node::Sequence(seq);
            REQUIRE(node.IsSequence());
            REQUIRE(node.Size() == 3);
            for (int i = 0; i < 3; ++i)
            {
                REQUIRE(node[i].IsNull());
            }
        }

        SECTION("Test lvalue sequence node factory method.")
        {
            fkyaml::Node node = fkyaml::Node::Sequence(std::move(seq));
            REQUIRE(node.IsSequence());
            REQUIRE(node.Size() == 3);
            for (int i = 0; i < 3; ++i)
            {
                REQUIRE(node[i].IsNull());
            }
        }
    }
}

TEST_CASE("NodeClassTest_MappingNodeFactoryTest", "[NodeClassTest]")
{
    SECTION("Test empty mapping node factory method.")
    {
        fkyaml::Node node = fkyaml::Node::Mapping();
        REQUIRE(node.IsMapping());
        REQUIRE(node.Size() == 0);
    }

    SECTION("Test non-empty mapping node factory methods.")
    {
        fkyaml::NodeMappingType map {{std::string("test"), fkyaml::Node::BooleanScalar(true)}};

        SECTION("Test lvalue mapping node factory method.")
        {
            fkyaml::Node node = fkyaml::Node::Mapping(map);
            REQUIRE(node.IsMapping());
            REQUIRE(node.Size() == 1);
            REQUIRE(node["test"].IsBoolean());
            REQUIRE(node["test"].ToBoolean() == true);
        }

        SECTION("Test rvalue mapping node factory method.")
        {
            fkyaml::Node node = fkyaml::Node::Mapping(std::move(map));
            REQUIRE(node.IsMapping());
            REQUIRE(node.Size() == 1);
            REQUIRE(node["test"].IsBoolean());
            REQUIRE(node["test"].ToBoolean() == true);
        }
    }
}

TEST_CASE("NodeClassTest_BooleanNodeFactoryTest", "[NodeClassTest]")
{
    auto boolean = GENERATE(true, false);
    fkyaml::Node node = fkyaml::Node::BooleanScalar(boolean);
    REQUIRE(node.IsBoolean());
    REQUIRE(node.ToBoolean() == boolean);
}

TEST_CASE("NodeClassTest_IntegerNodeFactoryTest", "[NodeClassTest]")
{
    auto integer = GENERATE(
        std::numeric_limits<fkyaml::NodeIntegerType>::min(), 0, std::numeric_limits<fkyaml::NodeIntegerType>::max());
    fkyaml::Node node = fkyaml::Node::IntegerScalar(integer);
    REQUIRE(node.IsInteger());
    REQUIRE(node.ToInteger() == integer);
}

TEST_CASE("NodeClassTest_FloatNumberNodeFactoryTest", "[NodeClassTest]")
{
    auto float_val = GENERATE(
        std::numeric_limits<fkyaml::NodeFloatNumberType>::min(),
        3.141592,
        std::numeric_limits<fkyaml::NodeFloatNumberType>::max());
    fkyaml::Node node = fkyaml::Node::FloatNumberScalar(float_val);
    REQUIRE(node.IsFloatNumber());
    REQUIRE(node.ToFloatNumber() == float_val);
}

TEST_CASE("NodeClassTest_StringNodeFactoryTest", "[NodeClassTest]")
{
    SECTION("Test empty string node factory method.")
    {
        fkyaml::Node node = fkyaml::Node::StringScalar();
        REQUIRE(node.IsString());
        REQUIRE(node.Size() == 0);
    }

    SECTION("Test lvalue string node factory method.")
    {
        fkyaml::NodeStringType str("test");
        fkyaml::Node node = fkyaml::Node::StringScalar(str);
        REQUIRE(node.IsString());
        REQUIRE(node.Size() == str.size());
        REQUIRE(node.ToString() == str);
    }

    SECTION("Test rvalue string node factory method.")
    {
        fkyaml::Node node = fkyaml::Node::StringScalar("test");
        REQUIRE(node.IsString());
        REQUIRE(node.Size() == 4);
        REQUIRE(node.ToString().compare("test") == 0);
    }
}

TEST_CASE("NodeClassTest_AliasNodeFactoryTest", "[NodeClassTest]")
{
    fkyaml::Node anchor = fkyaml::Node::StringScalar("alias_test");

    SECTION("Make sure BasicNode::AliasOf() throws an exception without anchor name.")
    {
        REQUIRE_THROWS_AS(fkyaml::Node::AliasOf(anchor), fkyaml::Exception);
    }

    SECTION("Make sure BasicNode::AliasOf() throws an exception with an empty anchor name.")
    {
        anchor.AddAnchorName("");
        REQUIRE_THROWS_AS(fkyaml::Node::AliasOf(anchor), fkyaml::Exception);
    }

    SECTION("Check if BasicNode::AliasOf() does not throw any exception.")
    {
        anchor.AddAnchorName("anchor_name");
        REQUIRE_NOTHROW(fkyaml::Node::AliasOf(anchor));
        fkyaml::Node alias = fkyaml::Node::AliasOf(anchor);
        REQUIRE(alias.IsString());
        REQUIRE(alias.ToString().compare("alias_test") == 0);
    }
}

//
// test cases for subscript operators
//

TEST_CASE("NodeClassTest_StringSubscriptOperatorTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected string subscript operators.")
    {
        fkyaml::NodeMappingType map {{"test", fkyaml::Node()}};

        SECTION("Test the non-const string subscript operators.")
        {
            fkyaml::Node node = fkyaml::Node::Mapping(map);

            SECTION("Test the non-const lvalue string subscript operator.")
            {
                std::string key = "test";
                REQUIRE_NOTHROW(node[key]);
                REQUIRE(node[key].IsNull());
            }

            SECTION("Test the non-const alias lvalue string subscript operator.")
            {
                std::string key = "test";
                node.AddAnchorName("anchor_name");
                fkyaml::Node alias = fkyaml::Node::AliasOf(node);
                REQUIRE_NOTHROW(alias[key]);
                REQUIRE(alias[key].IsNull());
            }

            SECTION("Test the non-const rvalue string subscript operator.")
            {
                REQUIRE_NOTHROW(node["test"]);
                REQUIRE(node["test"].IsNull());
            }

            SECTION("Test the const alias lvalue string subscript operator.")
            {
                std::string key = "test";
                node.AddAnchorName("anchor_name");
                const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
                REQUIRE_NOTHROW(alias[key]);
                REQUIRE(alias[key].IsNull());
            }
        }

        SECTION("Test the const string subscript operators.")
        {
            const fkyaml::Node node = fkyaml::Node::Mapping(map);
            std::string key = "test";

            SECTION("Test the const lvalue string subscript operator.")
            {
                REQUIRE_NOTHROW(node[key]);
                REQUIRE(node[key].IsNull());
            }

            SECTION("Test the const rvalue string subscript operator.")
            {
                REQUIRE_NOTHROW(node["test"]);
                REQUIRE(node["test"].IsNull());
            }
        }
    }

    SECTION("Test throwing expected string subscript operator.")
    {
        auto node = GENERATE(
            fkyaml::Node::Sequence(),
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-const lvalue throwing invocation.")
        {
            std::string key = "test";
            REQUIRE_THROWS_AS(node[key], fkyaml::Exception);
        }

        SECTION("Test const lvalue throwing invocation.")
        {
            std::string key = "test";
            const fkyaml::Node const_node = node;
            REQUIRE_THROWS_AS(const_node[key], fkyaml::Exception);
        }

        SECTION("Test non-const rvalue throwing invocation.")
        {
            REQUIRE_THROWS_AS(node["test"], fkyaml::Exception);
        }

        SECTION("Test const rvalue throwing invocation.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_THROWS_AS(const_node["test"], fkyaml::Exception);
        }
    }
}

TEST_CASE("NodeClassTest_IntegerSubscriptOperatorTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected integer subscript operators.")
    {
        fkyaml::Node node = fkyaml::Node::Sequence();
        node.ToSequence().emplace_back();

        SECTION("Test non-const non-alias integer subscript operators")
        {
            REQUIRE_NOTHROW(node[0]);
        }

        SECTION("Test non-const alias integer subscript operators")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias[0]);
        }

        SECTION("Test const non-alias integer subscript operators")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_NOTHROW(const_node[0]);
        }

        SECTION("Test const alias integer subscript operators")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias[0]);
        }
    }

    SECTION("Test throwing expected integer subscript operator.")
    {
        auto node = GENERATE(
            fkyaml::Node::Mapping(),
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-const non-sequence nodes.")
        {
            REQUIRE_THROWS_AS(node[0], fkyaml::Exception);
        }

        SECTION("Test const non-sequence nodes.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_THROWS_AS(const_node[0], fkyaml::Exception);
        }
    }
}

//
// test cases for type property getter/checkers
//

TEST_CASE("NodeClassTest_TypeGetterTest", "[NodeClassTest]")
{
    using NodeTypePair = std::pair<fkyaml::Node, fkyaml::NodeType>;
    auto type_pair = GENERATE(
        NodeTypePair(fkyaml::Node::Sequence(), fkyaml::NodeType::SEQUENCE),
        NodeTypePair(fkyaml::Node::Mapping(), fkyaml::NodeType::MAPPING),
        NodeTypePair(fkyaml::Node(), fkyaml::NodeType::NULL_OBJECT),
        NodeTypePair(fkyaml::Node::BooleanScalar(false), fkyaml::NodeType::BOOLEAN),
        NodeTypePair(fkyaml::Node::IntegerScalar(0), fkyaml::NodeType::INTEGER),
        NodeTypePair(fkyaml::Node::FloatNumberScalar(0.0), fkyaml::NodeType::FLOAT_NUMBER),
        NodeTypePair(fkyaml::Node::StringScalar(), fkyaml::NodeType::STRING));

    SECTION("Test non-alias node types.")
    {
        REQUIRE(type_pair.first.Type() == type_pair.second);
    }

    SECTION("Test alias node types.")
    {
        type_pair.first.AddAnchorName("anchor_name");
        fkyaml::Node alias = fkyaml::Node::AliasOf(type_pair.first);
        REQUIRE(alias.Type() == type_pair.second);
    }
}

TEST_CASE("NodeClassTest_IsSequenceTest", "[NodeClassTest]")
{
    SECTION("Test sequence node type.")
    {
        fkyaml::Node node = fkyaml::Node::Sequence();

        SECTION("Test non-alias sequence node type.")
        {
            REQUIRE(node.IsSequence());
        }

        SECTION("Test alias sequence node type.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE(alias.IsSequence());
        }
    }

    SECTION("Test non-sequence node types.")
    {
        auto node = GENERATE(
            fkyaml::Node::Mapping(),
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-alias non-sequence node types")
        {
            REQUIRE_FALSE(node.IsSequence());
        }

        SECTION("Test alias non-sequence node types.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_FALSE(alias.IsSequence());
        }
    }
}

TEST_CASE("NodeClassTest_IsMappingTest", "[NodeClassTest]")
{
    SECTION("Test mapping node type.")
    {
        fkyaml::Node node = fkyaml::Node::Mapping();

        SECTION("Test non-alias mapping node type.")
        {
            REQUIRE(node.IsMapping());
        }

        SECTION("Test alias mapping node type.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE(alias.IsMapping());
        }
    }

    SECTION("Test non-mapping node types.")
    {
        auto node = GENERATE(
            fkyaml::Node::Sequence(),
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-alias non-mapping node types")
        {
            REQUIRE_FALSE(node.IsMapping());
        }

        SECTION("Test alias non-mapping node types.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_FALSE(alias.IsMapping());
        }
    }
}

TEST_CASE("NodeClassTest_IsNullTest", "[NodeClassTest]")
{
    SECTION("Test null node type.")
    {
        fkyaml::Node node = fkyaml::Node();

        SECTION("Test non-alias null node type.")
        {
            REQUIRE(node.IsNull());
        }

        SECTION("Test alias null node type.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE(alias.IsNull());
        }
    }

    SECTION("Test non-null node types.")
    {
        auto node = GENERATE(
            fkyaml::Node::Sequence(),
            fkyaml::Node::Mapping(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-alias non-null node types")
        {
            REQUIRE_FALSE(node.IsNull());
        }

        SECTION("Test alias non-null node types.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_FALSE(alias.IsNull());
        }
    }
}

TEST_CASE("NodeClassTest_IsBooleanTest", "[NodeClassTest]")
{
    SECTION("Test boolean node type.")
    {
        fkyaml::Node node = fkyaml::Node::BooleanScalar(false);

        SECTION("Test non-alias boolean node type.")
        {
            REQUIRE(node.IsBoolean());
        }

        SECTION("Test alias boolean node type.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE(alias.IsBoolean());
        }
    }

    SECTION("Test non-boolean node types.")
    {
        auto node = GENERATE(
            fkyaml::Node::Sequence(),
            fkyaml::Node::Mapping(),
            fkyaml::Node(),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-alias non-boolean node types")
        {
            REQUIRE_FALSE(node.IsBoolean());
        }

        SECTION("Test alias non-boolean node types.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_FALSE(alias.IsBoolean());
        }
    }
}

TEST_CASE("NodeClassTest_IsIntegerTest", "[NodeClassTest]")
{
    SECTION("Test integer node type.")
    {
        fkyaml::Node node = fkyaml::Node::IntegerScalar(0);

        SECTION("Test non-alias integer node type.")
        {
            REQUIRE(node.IsInteger());
        }

        SECTION("Test alias integer node type.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE(alias.IsInteger());
        }
    }

    SECTION("Test non-integer node types.")
    {
        auto node = GENERATE(
            fkyaml::Node::Sequence(),
            fkyaml::Node::Mapping(),
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::FloatNumberScalar(0.0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-alias non-integer node types")
        {
            REQUIRE_FALSE(node.IsInteger());
        }

        SECTION("Test alias non-integer node types.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_FALSE(alias.IsInteger());
        }
    }
}

TEST_CASE("NodeClassTest_IsFloatNumberTest", "[NodeClassTest]")
{
    SECTION("Test float number node type.")
    {
        fkyaml::Node node = fkyaml::Node::FloatNumberScalar(0.0);

        SECTION("Test non-alias float number node type.")
        {
            REQUIRE(node.IsFloatNumber());
        }

        SECTION("Test alias float number node type.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE(alias.IsFloatNumber());
        }
    }

    SECTION("Test non-float-number node types.")
    {
        auto node = GENERATE(
            fkyaml::Node::Sequence(),
            fkyaml::Node::Mapping(),
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-alias non-float-number node types")
        {
            REQUIRE_FALSE(node.IsFloatNumber());
        }

        SECTION("Test alias non-float-number node types.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_FALSE(alias.IsFloatNumber());
        }
    }
}

TEST_CASE("NodeClassTest_IsStringTest", "[NodeClassTest]")
{
    SECTION("Test string node type.")
    {
        fkyaml::Node node = fkyaml::Node::StringScalar();

        SECTION("Test non-alias string node type.")
        {
            REQUIRE(node.IsString());
        }

        SECTION("Test alias string node type.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE(alias.IsString());
        }
    }

    SECTION("Test non-string node types.")
    {
        auto node = GENERATE(
            fkyaml::Node::Sequence(),
            fkyaml::Node::Mapping(),
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0));

        SECTION("Test non-alias non-string node types")
        {
            REQUIRE_FALSE(node.IsString());
        }

        SECTION("Test alias non-string node types.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_FALSE(alias.IsString());
        }
    }
}

TEST_CASE("NodeClassTest_IsScalarTest", "[NodeClassTest]")
{
    SECTION("Test scalar node types.")
    {
        auto node = GENERATE(
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-alias scalar node types.")
        {
            REQUIRE(node.IsScalar());
        }

        SECTION("Test alias scalar node types.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE(alias.IsScalar());
        }
    }

    SECTION("Test non-scalar node types.")
    {
        auto node = GENERATE(fkyaml::Node::Sequence(), fkyaml::Node::Mapping());

        SECTION("Test non-alias non-scalar node types")
        {
            REQUIRE_FALSE(node.IsScalar());
        }

        SECTION("Test alias non-scalar node types.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_FALSE(alias.IsScalar());
        }
    }
}

TEST_CASE("NodeClassTest_IsAliasTest", "[NodeClassTest]")
{
    auto node = GENERATE(
        fkyaml::Node::Sequence(),
        fkyaml::Node::Mapping(),
        fkyaml::Node(),
        fkyaml::Node::BooleanScalar(false),
        fkyaml::Node::IntegerScalar(0),
        fkyaml::Node::FloatNumberScalar(0.0),
        fkyaml::Node::StringScalar());

    SECTION("Test alias node types.")
    {
        node.AddAnchorName("anchor_name");
        fkyaml::Node alias = fkyaml::Node::AliasOf(node);
    }
}

//
// test cases for emptiness checker
//

TEST_CASE("NodeClassTest_IsEmptyTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected node emptiness.")
    {
        SECTION("Test empty container node emptiness.")
        {
            auto node = GENERATE(fkyaml::Node::Sequence(), fkyaml::Node::Mapping(), fkyaml::Node::StringScalar());

            SECTION("Test empty non-alias container node emptiness.")
            {
                REQUIRE_NOTHROW(node.IsEmpty());
                REQUIRE(node.IsEmpty());
            }

            SECTION("Test empty alias container node emptiness.")
            {
                node.AddAnchorName("anchor_name");
                fkyaml::Node alias = fkyaml::Node::AliasOf(node);
                REQUIRE_NOTHROW(alias.IsEmpty());
                REQUIRE(alias.IsEmpty());
            }
        }

        SECTION("Test non-empty container node emptiness.")
        {
            auto node = GENERATE(
                fkyaml::Node::Sequence(fkyaml::NodeSequenceType(3)),
                fkyaml::Node::Mapping(fkyaml::NodeMappingType {{"test", fkyaml::Node()}}),
                fkyaml::Node::StringScalar("test"));

            SECTION("Test non-empty non-alias container node emptiness.")
            {
                REQUIRE_NOTHROW(node.IsEmpty());
                REQUIRE_FALSE(node.IsEmpty());
            }

            SECTION("Test non-empty alias container node emptiness.")
            {
                node.AddAnchorName("anchor_name");
                fkyaml::Node alias = fkyaml::Node::AliasOf(node);
                REQUIRE_NOTHROW(alias.IsEmpty());
                REQUIRE_FALSE(alias.IsEmpty());
            }
        }
    }

    SECTION("Test nothrow unexpected node emptiness.")
    {
        auto node = GENERATE(
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0));

        SECTION("Test non-const non-alias non-container node emptiness.")
        {
            REQUIRE_THROWS_AS(node.IsEmpty(), fkyaml::Exception);
        }

        SECTION("Test const non-alias non-container node emptiness.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_THROWS_AS(const_node.IsEmpty(), fkyaml::Exception);
        }

        SECTION("Test non-const alias non-container node emptiness.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.IsEmpty(), fkyaml::Exception);
        }

        SECTION("Test const alias non-container node emptiness.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.IsEmpty(), fkyaml::Exception);
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
        fkyaml::Node node = fkyaml::Node::Mapping({{"test", fkyaml::Node()}});
        std::string key = "test";

        SECTION("Test non-alias mapping node with lvalue key.")
        {
            REQUIRE(node.Contains(key));
        }

        SECTION("Test alias mapping node with lvalue key.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE(node.Contains(key));
        }

        SECTION("Test non-alias mapping node with rvalue key.")
        {
            REQUIRE(node.Contains(std::move(key)));
        }

        SECTION("Test alias mapping node with rvalue key.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE(node.Contains(std::move(key)));
        }
    }

    SECTION("Test non-mapping node.")
    {
        auto node = GENERATE(
            fkyaml::Node::Sequence(),
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0),
            fkyaml::Node::StringScalar());
        std::string key = "test";

        SECTION("Test non-alias non-mapping node with lvalue key.")
        {
            REQUIRE_FALSE(node.Contains(key));
        }

        SECTION("Test alias non-mapping node with lvalue key.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_FALSE(alias.Contains(key));
        }

        SECTION("Test non-alias non-mapping node with rvalue key.")
        {
            REQUIRE_FALSE(node.Contains(std::move(key)));
        }

        SECTION("Test alias non-mapping node with rvalue key.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_FALSE(alias.Contains(std::move(key)));
        }
    }
}

//
// test cases for container size getter
//

TEST_CASE("NodeClassTest_SizeGetterTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected node size.")
    {
        auto node = GENERATE(
            fkyaml::Node::Sequence({fkyaml::Node(), fkyaml::Node(), fkyaml::Node()}),
            fkyaml::Node::Mapping({{"test0", fkyaml::Node()}, {"test1", fkyaml::Node()}, {"test2", fkyaml::Node()}}),
            fkyaml::Node::StringScalar("tmp"));

        SECTION("Test container node size.")
        {
            REQUIRE_NOTHROW(node.Size());
            REQUIRE(node.Size() == 3);
        }

        SECTION("Test const container node size.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_NOTHROW(node.Size());
            REQUIRE(const_node.Size() == 3);
        }

        SECTION("Test alias container node size.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.Size());
            REQUIRE(alias.Size() == 3);
        }

        SECTION("Test const alias container node size.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.Size());
            REQUIRE(alias.Size() == 3);
        }
    }

    SECTION("Test nothrow unexpected node size.")
    {
        auto node = GENERATE(
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0));

        SECTION("Test non-const non-alias non-container node size.")
        {
            REQUIRE_THROWS_AS(node.Size(), fkyaml::Exception);
        }

        SECTION("Test const non-alias non-container node size.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_THROWS_AS(const_node.Size(), fkyaml::Exception);
        }

        SECTION("Test non-const alias non-container node size.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.Size(), fkyaml::Exception);
        }

        SECTION("Test const alias non-container node size.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.Size(), fkyaml::Exception);
        }
    }
}

//
// test cases for YAML version property getter/setter
//

TEST_CASE("NodeClassTest_SetVersionTest", "[NodeClassTest]")
{
    fkyaml::Node node;
    node.SetVersion(fkyaml::YamlVersionType::VER_1_1);
    REQUIRE(node.GetVersion() == fkyaml::YamlVersionType::VER_1_1);

    node.SetVersion(fkyaml::YamlVersionType::VER_1_2);
    REQUIRE(node.GetVersion() == fkyaml::YamlVersionType::VER_1_2);
}

TEST_CASE("NodeClassTest_GetVersionTest", "[NodeClassTest]")
{
    fkyaml::Node node;
    REQUIRE(node.GetVersion() == fkyaml::YamlVersionType::VER_1_2);

    node.SetVersion(fkyaml::YamlVersionType::VER_1_1);
    REQUIRE(node.GetVersion() == fkyaml::YamlVersionType::VER_1_1);
}

//
// test cases for anchor name property checker/getter/setter
//

TEST_CASE("NodeClassTest_HasAnchorNameTest", "[NodeClassTest]")
{
    fkyaml::Node node;

    SECTION("Test node without anchor name.")
    {
        REQUIRE_FALSE(node.HasAnchorName());
    }

    SECTION("Test node wityh anchor name.")
    {
        node.AddAnchorName("anchor_name");
        REQUIRE(node.HasAnchorName());
    }
}

TEST_CASE("NodeClassTest_GetAnchorNameTest", "[NodeClassTest]")
{
    fkyaml::Node node;

    SECTION("Test node without anchor name.")
    {
        REQUIRE_THROWS_AS(node.GetAnchorName(), fkyaml::Exception);
    }

    SECTION("Test node with anchor name.")
    {
        node.AddAnchorName("anchor_name");
        REQUIRE_NOTHROW(node.GetAnchorName());
        REQUIRE(node.GetAnchorName().compare("anchor_name") == 0);
    }
}

TEST_CASE("NodeClassTest_AddAnchorNameTest", "[NodeClassTest]")
{
    fkyaml::Node node;
    std::string anchor_name = "anchor_name";

    SECTION("Test lvalue anchor name.")
    {
        node.AddAnchorName(anchor_name);
        REQUIRE_NOTHROW(node.GetAnchorName());
        REQUIRE(node.GetAnchorName().compare("anchor_name") == 0);
    }

    SECTION("Test rvalue anchor name.")
    {
        node.AddAnchorName(std::move(anchor_name));
        REQUIRE_NOTHROW(node.GetAnchorName());
        REQUIRE(node.GetAnchorName().compare("anchor_name") == 0);
    }

    SECTION("Test overwritten anchor name.")
    {
        node.AddAnchorName(anchor_name);
        node.AddAnchorName("overwritten_name");
        REQUIRE_NOTHROW(node.GetAnchorName());
        REQUIRE_FALSE(node.GetAnchorName().compare("anchor_name") == 0);
        REQUIRE(node.GetAnchorName().compare("overwritten_name") == 0);
    }
}

//
// test cases for value reference getters
//

TEST_CASE("NodeClassTest_ToSequenceTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected nodes.")
    {
        fkyaml::Node node =
            fkyaml::Node::Sequence(fkyaml::NodeSequenceType {fkyaml::Node(), fkyaml::Node(), fkyaml::Node()});

        SECTION("Test non-alias sequence node.")
        {
            REQUIRE_NOTHROW(node.ToSequence());
            REQUIRE(node.ToSequence().size() == 3);
            for (int i = 0; i < 3; ++i)
            {
                REQUIRE(node.ToSequence()[i].IsNull());
            }
        }

        SECTION("Test const non-alias sequence node.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_NOTHROW(const_node.ToSequence());
            REQUIRE(const_node.ToSequence().size() == 3);
            for (int i = 0; i < 3; ++i)
            {
                REQUIRE(node.ToSequence()[i].IsNull());
            }
        }

        SECTION("Test alias sequence node.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.ToSequence());
            REQUIRE(alias.ToSequence().size() == 3);
            for (int i = 0; i < 3; ++i)
            {
                REQUIRE(alias.ToSequence()[i].IsNull());
            }
        }

        SECTION("Test const alias sequence node.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.ToSequence());
            REQUIRE(alias.ToSequence().size() == 3);
            for (int i = 0; i < 3; ++i)
            {
                REQUIRE(alias.ToSequence()[i].IsNull());
            }
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::Node::Mapping(),
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-alias non-sequence nodes.")
        {
            REQUIRE_THROWS_AS(node.ToSequence(), fkyaml::Exception);
        }

        SECTION("Test const non-alias non-sequence nodes.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_THROWS_AS(const_node.ToSequence(), fkyaml::Exception);
        }

        SECTION("Test alias non-sequence nodes.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.ToSequence(), fkyaml::Exception);
        }

        SECTION("Test const alias non-sequence nodes.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.ToSequence(), fkyaml::Exception);
        }
    }
}

TEST_CASE("NodeClassTest_ToMappingTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected nodes.")
    {
        fkyaml::Node node = fkyaml::Node::Mapping(
            fkyaml::NodeMappingType {{"test0", fkyaml::Node()}, {"test1", fkyaml::Node()}, {"test2", fkyaml::Node()}});

        SECTION("Test non-alias mapping node.")
        {
            REQUIRE_NOTHROW(node.ToMapping());
            REQUIRE(node.ToMapping().size() == 3);
            REQUIRE(node["test0"].IsNull());
            REQUIRE(node["test1"].IsNull());
            REQUIRE(node["test2"].IsNull());
        }

        SECTION("Test const non-alias mapping node.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_NOTHROW(const_node.ToMapping());
            REQUIRE(const_node.ToMapping().size() == 3);
            REQUIRE(const_node["test0"].IsNull());
            REQUIRE(const_node["test1"].IsNull());
            REQUIRE(const_node["test2"].IsNull());
        }

        SECTION("Test alias mapping node.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.ToMapping());
            REQUIRE(alias.ToMapping().size() == 3);
            REQUIRE(alias["test0"].IsNull());
            REQUIRE(alias["test1"].IsNull());
            REQUIRE(alias["test2"].IsNull());
        }

        SECTION("Test const alias mapping node.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.ToMapping());
            REQUIRE(alias["test0"].IsNull());
            REQUIRE(alias["test1"].IsNull());
            REQUIRE(alias["test2"].IsNull());
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::Node::Sequence(),
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-alias non-mapping nodes.")
        {
            REQUIRE_THROWS_AS(node.ToMapping(), fkyaml::Exception);
        }

        SECTION("Test const non-alias non-mapping nodes.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_THROWS_AS(const_node.ToMapping(), fkyaml::Exception);
        }

        SECTION("Test alias non-mapping nodes.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.ToMapping(), fkyaml::Exception);
        }

        SECTION("Test const alias non-mapping nodes.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.ToMapping(), fkyaml::Exception);
        }
    }
}

TEST_CASE("NodeClassTest_ToBooleanTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected nodes.")
    {
        fkyaml::Node node = fkyaml::Node::BooleanScalar(true);

        SECTION("Test non-alias boolean node.")
        {
            REQUIRE_NOTHROW(node.ToBoolean());
            REQUIRE(node.ToBoolean() == true);
        }

        SECTION("Test const non-alias boolean node.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_NOTHROW(const_node.ToBoolean());
            REQUIRE(const_node.ToBoolean() == true);
        }

        SECTION("Test alias boolean node.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.ToBoolean());
            REQUIRE(alias.ToBoolean() == true);
        }

        SECTION("Test const alias boolean node.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.ToBoolean());
            REQUIRE(alias.ToBoolean() == true);
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::Node::Sequence(),
            fkyaml::Node::Mapping(),
            fkyaml::Node(),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-alias non-boolean nodes.")
        {
            REQUIRE_THROWS_AS(node.ToBoolean(), fkyaml::Exception);
        }

        SECTION("Test const non-alias non-boolean nodes.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_THROWS_AS(const_node.ToBoolean(), fkyaml::Exception);
        }

        SECTION("Test alias non-boolean nodes.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.ToBoolean(), fkyaml::Exception);
        }

        SECTION("Test const alias non-boolean nodes.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.ToBoolean(), fkyaml::Exception);
        }
    }
}

TEST_CASE("NodeClassTest_ToIntegerTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected nodes.")
    {
        fkyaml::NodeIntegerType integer = -123;
        fkyaml::Node node = fkyaml::Node::IntegerScalar(integer);

        SECTION("Test non-alias  integer node.")
        {
            REQUIRE_NOTHROW(node.ToInteger());
            REQUIRE(node.ToInteger() == integer);
        }

        SECTION("Test const non-alias  integer node.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_NOTHROW(const_node.ToInteger());
            REQUIRE(const_node.ToInteger() == integer);
        }

        SECTION("Test alias  integer node.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.ToInteger());
            REQUIRE(alias.ToInteger() == integer);
        }

        SECTION("Test const alias  integer node.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.ToInteger());
            REQUIRE(alias.ToInteger() == integer);
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::Node::Sequence(),
            fkyaml::Node::Mapping(),
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::FloatNumberScalar(0.0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-alias non-integer nodes.")
        {
            REQUIRE_THROWS_AS(node.ToInteger(), fkyaml::Exception);
        }

        SECTION("Test const non-alias non-integer nodes.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_THROWS_AS(const_node.ToInteger(), fkyaml::Exception);
        }

        SECTION("Test alias non-integer nodes.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.ToInteger(), fkyaml::Exception);
        }

        SECTION("Test const alias non-integer nodes.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.ToInteger(), fkyaml::Exception);
        }
    }
}

TEST_CASE("NodeClassTest_ToFloatNumberTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected nodes.")
    {
        fkyaml::NodeFloatNumberType float_val = 123.45;
        fkyaml::Node node = fkyaml::Node::FloatNumberScalar(float_val);

        SECTION("Test non-alias float number node.")
        {
            REQUIRE_NOTHROW(node.ToFloatNumber());
            REQUIRE(node.ToFloatNumber() == float_val);
        }

        SECTION("Test const non-alias float number node.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_NOTHROW(const_node.ToFloatNumber());
            REQUIRE(const_node.ToFloatNumber() == float_val);
        }

        SECTION("Test alias float number node.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.ToFloatNumber());
            REQUIRE(alias.ToFloatNumber() == float_val);
        }

        SECTION("Test const alias float number node.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.ToFloatNumber());
            REQUIRE(alias.ToFloatNumber() == float_val);
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::Node::Sequence(),
            fkyaml::Node::Mapping(),
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-alias non-float-number nodes.")
        {
            REQUIRE_THROWS_AS(node.ToFloatNumber(), fkyaml::Exception);
        }

        SECTION("Test const non-alias non-float-number nodes.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_THROWS_AS(const_node.ToFloatNumber(), fkyaml::Exception);
        }

        SECTION("Test alias non-float-number nodes.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.ToFloatNumber(), fkyaml::Exception);
        }

        SECTION("Test const alias non-float-number nodes.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.ToFloatNumber(), fkyaml::Exception);
        }
    }
}

TEST_CASE("NodeClassTest_ToStringTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected nodes.")
    {
        fkyaml::NodeStringType str = "test";
        fkyaml::Node node = fkyaml::Node::StringScalar(str);

        SECTION("Test non-alias string node.")
        {
            REQUIRE_NOTHROW(node.ToString());
            REQUIRE(node.ToString() == str);
        }

        SECTION("Test const non-alias string node.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_NOTHROW(const_node.ToString());
            REQUIRE(const_node.ToString() == str);
        }

        SECTION("Test alias string node.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.ToString());
            REQUIRE(alias.ToString() == str);
        }

        SECTION("Test const alias string node.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.ToString());
            REQUIRE(alias.ToString() == str);
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::Node::Sequence(),
            fkyaml::Node::Mapping(),
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0));

        SECTION("Test non-alias non-string nodes.")
        {
            REQUIRE_THROWS_AS(node.ToString(), fkyaml::Exception);
        }

        SECTION("Test const non-alias non-string nodes.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_THROWS_AS(const_node.ToString(), fkyaml::Exception);
        }

        SECTION("Test alias non-string nodes.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.ToString(), fkyaml::Exception);
        }

        SECTION("Test const alias non-string nodes.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_THROWS_AS(alias.ToString(), fkyaml::Exception);
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
        auto node = GENERATE(fkyaml::Node::Sequence(), fkyaml::Node::Mapping());

        SECTION("Test non-const non-alias container node.")
        {
            REQUIRE_NOTHROW(node.Begin());
        }

        SECTION("Test const non-alias container node.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_NOTHROW(const_node.Begin());
        }

        SECTION("Test non-const alias container node.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.Begin());
        }

        SECTION("Test non-const alias container node.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.Begin());
        }

        SECTION("Test non-const range-based for-loop compatibility.")
        {
            REQUIRE_NOTHROW(node.begin());
        }

        SECTION("Test const range-based for-loop compatibility.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_NOTHROW(const_node.begin());
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-const throwing node.")
        {
            REQUIRE_THROWS_AS(node.Begin(), fkyaml::Exception);
        }

        SECTION("Test const throwing node.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_THROWS_AS(const_node.Begin(), fkyaml::Exception);
        }
    }
}

TEST_CASE("NodeClassTest_EndTest", "[NodeClassTest]")
{
    SECTION("Test nothrow expected nodes.")
    {
        auto node = GENERATE(fkyaml::Node::Sequence(), fkyaml::Node::Mapping());

        SECTION("Test non-const non-alias container node.")
        {
            REQUIRE_NOTHROW(node.End());
        }

        SECTION("Test const non-alias container node.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_NOTHROW(const_node.End());
        }

        SECTION("Test non-const alias container node.")
        {
            node.AddAnchorName("anchor_name");
            fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.End());
        }

        SECTION("Test non-const alias container node.")
        {
            node.AddAnchorName("anchor_name");
            const fkyaml::Node alias = fkyaml::Node::AliasOf(node);
            REQUIRE_NOTHROW(alias.End());
        }

        SECTION("Test non-const range-based for-loop compatibility.")
        {
            REQUIRE_NOTHROW(node.end());
        }

        SECTION("Test const range-based for-loop compatibility.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_NOTHROW(const_node.end());
        }
    }

    SECTION("Test nothrow unexpected nodes.")
    {
        auto node = GENERATE(
            fkyaml::Node(),
            fkyaml::Node::BooleanScalar(false),
            fkyaml::Node::IntegerScalar(0),
            fkyaml::Node::FloatNumberScalar(0.0),
            fkyaml::Node::StringScalar());

        SECTION("Test non-const throwing node.")
        {
            REQUIRE_THROWS_AS(node.End(), fkyaml::Exception);
        }

        SECTION("Test const throwing node.")
        {
            const fkyaml::Node const_node = node;
            REQUIRE_THROWS_AS(const_node.End(), fkyaml::Exception);
        }
    }
}

//
// test cases for swap
//

TEST_CASE("NodeClassTest_SwapTest", "[NodeClassTest]")
{
    fkyaml::Node lhs_node = fkyaml::Node::BooleanScalar(true);
    fkyaml::Node rhs_node = fkyaml::Node::IntegerScalar(123);
    lhs_node.Swap(rhs_node);
    REQUIRE(lhs_node.IsInteger());
    REQUIRE(lhs_node.ToInteger() == 123);
    REQUIRE(rhs_node.IsBoolean());
    REQUIRE(rhs_node.ToBoolean() == true);
}

TEST_CASE("NodeClassTest_StdSwapTest", "[NodeClassTest]")
{
    fkyaml::Node lhs_node = fkyaml::Node::BooleanScalar(true);
    fkyaml::Node rhs_node = fkyaml::Node::IntegerScalar(123);
    std::swap(lhs_node, rhs_node);
    REQUIRE(lhs_node.IsInteger());
    REQUIRE(lhs_node.ToInteger() == 123);
    REQUIRE(rhs_node.IsBoolean());
    REQUIRE(rhs_node.ToBoolean() == true);
}
