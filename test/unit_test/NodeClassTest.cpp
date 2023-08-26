/**
 * NodeClassTest.cpp - implementation of test functions for the Node class
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#include "NodeClassTest.hpp"

#include <cinttypes>
#include <cstdio>

#include "fkYAML/Node.hpp"

int NodeClassTest::DefaultCtorTest()
{
    fkyaml::Node node;

    if (!node.IsNull())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::NULL_OBJECT),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::SequenceTypeCtorTest()
{
    fkyaml::Node node(fkyaml::NodeType::SEQUENCE);

    if (!node.IsSequence())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::SEQUENCE),
            static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 0)
    {
        std::fprintf(
            stderr,
            "Invalid node size. expect=%d, actual=%zu\n",
            0,
            node.Size());
        return -1;
    }

    return 0;
}

int NodeClassTest::MappingTypeCtorTest()
{
    fkyaml::Node node(fkyaml::NodeType::MAPPING);

    if (!node.IsMapping())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::MAPPING),
            static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 0)
    {
        std::fprintf(
            stderr,
            "Invalid node size. expect=%d, actual=%zu\n",
            0,
            node.Size());
        return -1;
    }

    return 0;
}

int NodeClassTest::NullTypeCtorTest()
{
    fkyaml::Node node(fkyaml::NodeType::NULL_OBJECT);

    if (!node.IsNull())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::NULL_OBJECT),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::BooleanTypeCtorTest()
{
    fkyaml::Node node(fkyaml::NodeType::BOOLEAN);

    if (!node.IsBoolean())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::BOOLEAN),
            static_cast<int>(node.Type()));
        return -1;
    }

    if (node.ToBoolean() != false)
    {
        std::fprintf(
            stderr,
            "Invalid node value. expect=%s, actual=%s\n",
            "false",
            node.ToBoolean() ? "true" : "false");
        return -1;
    }

    return 0;
}

int NodeClassTest::SignedIntTypeCtorTest()
{
    fkyaml::Node node(fkyaml::NodeType::SIGNED_INTEGER);

    if (!node.IsSignedInteger())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::SIGNED_INTEGER),
            static_cast<int>(node.Type()));
        return -1;
    }

    if (node.ToSignedInteger() != 0)
    {
        std::fprintf(
            stderr,
            "Invalid node value. expect=%d, actual=%" PRId64 "\n",
            0,
            node.ToSignedInteger());
        return -1;
    }

    return 0;
}

int NodeClassTest::UnsignedIntTypeCtorTest()
{
    fkyaml::Node node(fkyaml::NodeType::UNSIGNED_INTEGER);

    if (!node.IsUnsignedInteger())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::UNSIGNED_INTEGER),
            static_cast<int>(node.Type()));
        return -1;
    }

    if (node.ToUnsignedInteger() != 0)
    {
        std::fprintf(
            stderr,
            "Invalid node value. expect=%d, actual=%" PRIu64 "\n",
            0,
            node.ToUnsignedInteger());
        return -1;
    }

    return 0;
}

int NodeClassTest::FloatNumberTypeCtorTest()
{
    fkyaml::Node node(fkyaml::NodeType::FLOAT_NUMBER);

    if (!node.IsFloatNumber())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::FLOAT_NUMBER),
            static_cast<int>(node.Type()));
        return -1;
    }

    if (node.ToFloatNumber() != 0)
    {
        std::fprintf(
            stderr,
            "Invalid node value. expect=%lf, actual=%lf\n",
            0.0,
            node.ToFloatNumber());
        return -1;
    }

    return 0;
}

int NodeClassTest::StringTypeCtorTest()
{
    fkyaml::Node node(fkyaml::NodeType::STRING);

    if (!node.IsString())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::STRING),
            static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 0)
    {
        std::fprintf(
            stderr,
            "Invalid node size. expect=%d, actual=%zu\n",
            0,
            node.Size());
        return -1;
    }

    return 0;
}

int NodeClassTest::EmptySequenceNodeFactoryTest()
{

    fkyaml::Node node = fkyaml::Node::Sequence();

    if (!node.IsSequence())
    {
        std::fprintf(stderr, "node type is not the sequence type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 0)
    {
        std::fprintf(stderr, "the size of the sequence node is not 0. actual=%zu\n", node.Size());
        return -1;
    }

    return 0;
}

int NodeClassTest::LvalueSequenceNodeFactoryTest()
{
    fkyaml::NodeSequenceType seq(3);
    fkyaml::Node node = fkyaml::Node::Sequence(seq);

    if (!node.IsSequence())
    {
        std::fprintf(stderr, "node type is not the sequence type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 3)
    {
        std::fprintf(stderr, "the size of the sequence node is not 3. actual=%zu\n", node.Size());
        return -1;
    }

    for (int i = 0; i < 3; ++i)
    {
        if (!node[i].IsNull())
        {
            std::fprintf(
                stderr,
                "type error of the sequence node value. index=%d, expectation=%d, actual=%d\n",
                i,
                static_cast<int>(fkyaml::NodeType::NULL_OBJECT),
                static_cast<int>(node[i].Type()));
            return -1;
        }
    }

    return 0;
}

int NodeClassTest::RvalueSequenceNodeFactoryTest()
{
    fkyaml::Node node = fkyaml::Node::Sequence(fkyaml::NodeSequenceType(3));

    if (!node.IsSequence())
    {
        std::fprintf(stderr, "node type is not the sequence type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 3)
    {
        std::fprintf(stderr, "the size of the sequence node is not 3. actual=%zu\n", node.Size());
        return -1;
    }

    for (int i = 0; i < 3; ++i)
    {
        if (!node[i].IsNull())
        {
            std::fprintf(
                stderr,
                "type error of the sequence node value. index=%d, expectation=%d, actual=%d\n",
                i,
                static_cast<int>(fkyaml::NodeType::NULL_OBJECT),
                static_cast<int>(node[i].Type()));
            return -1;
        }
    }

    return 0;
}

int NodeClassTest::EmptyMappingNodeFactoryTest()
{
    fkyaml::Node node = fkyaml::Node::Mapping();

    if (!node.IsMapping())
    {
        std::fprintf(stderr, "node type is not the mapping type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 0)
    {
        std::fprintf(stderr, "the size of the mapping node is not 0. actual=%zu\n", node.Size());
        return -1;
    }

    return 0;
}

int NodeClassTest::LvalueMappingNodeFactoryTest()
{
    fkyaml::NodeMappingType map {{std::string("test"), fkyaml::Node::BooleanScalar(true)}};
    fkyaml::Node node = fkyaml::Node::Mapping(map);

    if (!node.IsMapping())
    {
        std::fprintf(stderr, "node type is not the mapping type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 1)
    {
        std::fprintf(stderr, "the size of the mapping node is not 1. actual=%zu\n", node.Size());
        return -1;
    }

    if (!node["test"].IsBoolean())
    {
        std::fprintf(
            stderr,
            "type error of the mapping node value. key=%s, expectation=%d, actual=%d\n",
            "test",
            static_cast<int>(fkyaml::NodeType::BOOLEAN),
            static_cast<int>(node.Type()));
        return -1;
    }

    if (node["test"].ToBoolean() != true)
    {
        std::fprintf(
            stderr,
            "value error of the mapping node value. key=%s, expectation=%s, actual=%s\n",
            "test",
            "true",
            node["test"].ToBoolean() ? "true" : "false");
        return -1;
    }

    return 0;
}

int NodeClassTest::RvalueMappingNodeFactoryTest()
{
    fkyaml::Node node =
        fkyaml::Node::Mapping(fkyaml::NodeMappingType {{std::string("test"), fkyaml::Node::BooleanScalar(true)}});

    if (!node.IsMapping())
    {
        std::fprintf(stderr, "node type is not the mapping type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 1)
    {
        std::fprintf(stderr, "the size of the mapping node is not 1. actual=%zu\n", node.Size());
        return -1;
    }

    if (!node["test"].IsBoolean())
    {
        std::fprintf(
            stderr,
            "type error of the mapping node value. key=%s, expectation=%d, actual=%d\n",
            "test",
            static_cast<int>(fkyaml::NodeType::BOOLEAN),
            static_cast<int>(node.Type()));
        return -1;
    }

    if (node["test"].ToBoolean() != true)
    {
        std::fprintf(
            stderr,
            "value error of the mapping node value. key=%s, expectation=%s, actual=%s\n",
            "test",
            "true",
            node["test"].ToBoolean() ? "true" : "false");
        return -1;
    }

    return 0;
}

int NodeClassTest::BoolNodeFactoryTest()
{
    fkyaml::Node node = fkyaml::Node::BooleanScalar(true);

    if (!node.IsBoolean())
    {
        std::fprintf(stderr, "node type is not the boolean type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.ToBoolean() != true)
    {
        std::fprintf(
            stderr, "node value error. expectation=%s, actual=%s\n", "true", node.ToBoolean() ? "true" : "false");
        return -1;
    }

    return 0;
}

int NodeClassTest::SignedIntNodeFactoryTest()
{
    fkyaml::Node node = fkyaml::Node::SignedIntegerScalar(-128);

    if (!node.IsSignedInteger())
    {
        std::fprintf(stderr, "node type is not the signed integer type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.ToSignedInteger() != -128)
    {
        std::fprintf(stderr, "node value error. expectation=%d, actual=%" PRId64 "\n", -128, node.ToSignedInteger());
        return -1;
    }

    return 0;
}

int NodeClassTest::UnsignedIntNodeFactoryTest()
{
    fkyaml::Node node = fkyaml::Node::UnsignedIntegerScalar(255ULL);

    if (!node.IsUnsignedInteger())
    {
        std::fprintf(stderr, "node type is not the unsigned integer type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.ToUnsignedInteger() != 255ULL)
    {
        std::fprintf(
            stderr, "node value error. expectation=%llu, actual=%" PRIu64 "\n", 255ULL, node.ToUnsignedInteger());
        return -1;
    }

    return 0;
}

int NodeClassTest::FloatNumberNodeFactoryTest()
{
    fkyaml::Node node = fkyaml::Node::FloatNumberScalar(3.141952);

    if (!node.IsFloatNumber())
    {
        std::fprintf(stderr, "node type is not the unsigned integer type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.ToFloatNumber() != 3.141952)
    {
        std::fprintf(stderr, "node value error. expectation=%lf, actual=%lf", 3.141952, node.ToFloatNumber());
        return -1;
    }

    return 0;
}

int NodeClassTest::EmptyStringNodeFactoryTest()
{
    fkyaml::Node node = fkyaml::Node::StringScalar();

    if (!node.IsString())
    {
        std::fprintf(stderr, "node type is not the string type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 0)
    {
        std::fprintf(stderr, "the size of the string node is not 0. actual=%zu\n", node.Size());
        return -1;
    }

    return 0;
}

int NodeClassTest::LvalueStringNodeFactoryTest()
{
    fkyaml::NodeStringType str("test");
    fkyaml::Node node = fkyaml::Node::StringScalar(str);

    if (!node.IsString())
    {
        std::fprintf(stderr, "node type is not the string type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 4)
    {
        std::fprintf(stderr, "the size of the string node is not 4. actual=%zu\n", node.Size());
        return -1;
    }

    std::string value = node.ToString();
    if (value.compare("test") != 0)
    {
        std::fprintf(stderr, "the content of the string node is not \"test\". actual=%s\n", value.c_str());
        return -1;
    }

    return 0;
}

int NodeClassTest::RvalueStringNodeFactoryTest()
{
    fkyaml::Node node = fkyaml::Node::StringScalar("test");

    if (!node.IsString())
    {
        std::fprintf(stderr, "node type is not the string type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 4)
    {
        std::fprintf(stderr, "the size of the string node is not 4. actual=%zu\n", node.Size());
        return -1;
    }

    std::string value = node.ToString();
    if (value.compare("test") != 0)
    {
        std::fprintf(stderr, "the content of the string node is not \"test\". actual=%s\n", value.c_str());
        return -1;
    }

    return 0;
}

int NodeClassTest::AliasNodeFactoryTest()
{
    fkyaml::Node anchor = fkyaml::Node::StringScalar("alias_test");
    anchor.AddAnchorName("anchor_name");

    try
    {
        fkyaml::Node alias = fkyaml::Node::AliasOf(anchor);

        if (!alias.HasAnchorName())
        {
            std::fprintf(
                stderr,
                "Invalid alias name existence. expect=%s, actual=%s\n",
                "true",
                alias.HasAnchorName() ? "true" : "false");
            return -1;
        }

        if (alias.GetAnchorName().compare("anchor_name") != 0)
        {
            std::fprintf(
                stderr,
                "Invalid alias name value. expect=\"%s\", actual=\"%s\"\n",
                "anchor_name",
                alias.GetAnchorName().c_str());
            return -1;
        }

        if (!alias.IsString())
        {
            std::fprintf(
                stderr,
                "Invalid alias node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::STRING),
                static_cast<int>(alias.Type()));
            return -1;
        }

        if (alias.ToString().compare("alias_test") != 0)
        {
            std::fprintf(
                stderr,
                "Invalid alias node value. expect=\"%s\", actual=\"%s\"\n",
                "alias_test",
                alias.ToString().c_str());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "Failed to invoke BasicNode::AliasOf() with an anchor node.\n");
        return -1;
    }

    return 0;
}

int NodeClassTest::AliasNodeFactoryThrowExceptionTest()
{
    try
    {
        fkyaml::Node dummy_node;
        fkyaml::Node alias = fkyaml::Node::AliasOf(dummy_node);
    
        std::fprintf(stderr, "Unexpected invocation of BasicNode::AliasOf() with a non-anchor node.\n");
        return -1;
    }
    catch (const fkyaml::Exception&)
    {
    }

    return 0;
}

int NodeClassTest::LvalueStringSubscriptOperatorTest()
{
    fkyaml::Node node = fkyaml::Node::Mapping();
    node.ToMapping().emplace("test", fkyaml::Node());

    try
    {
        std::string key = "test";
        fkyaml::Node test_node = node[key];
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "Failed to access mapping node value with a subscript operator\n.");
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ConstLvalueStringSubscriptOperatorTest()
{
    fkyaml::Node tmp_node = fkyaml::Node::Mapping();
    tmp_node.ToMapping().emplace("test", fkyaml::Node());
    const fkyaml::Node node = std::move(tmp_node);

    try
    {
        std::string key = "test";
        const fkyaml::Node test_node = node[key];
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "Failed to access mapping node value with a subscript operator\n.");
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::RvalueStringSubscriptOperatorTest()
{
    fkyaml::Node node = fkyaml::Node::Mapping();
    node.ToMapping().emplace("test", fkyaml::Node());

    try
    {
        fkyaml::Node test_node = node["test"];
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "Failed to access mapping node value with a subscript operator\n.");
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ConstRvalueStringSubscriptOperatorTest()
{
    fkyaml::Node tmp_node = fkyaml::Node::Mapping();
    tmp_node.ToMapping().emplace("test", fkyaml::Node());
    const fkyaml::Node node = std::move(tmp_node);

    try
    {
        const fkyaml::Node test_node = node["test"];
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "Failed to access mapping node value with a subscript operator\n.");
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::IntegerSubscriptOperatorTest()
{
    fkyaml::Node node = fkyaml::Node::Sequence();
    node.ToSequence().emplace_back();

    try
    {
        fkyaml::Node test_node = node[0];
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "Failed to access sequence node value with a subscript operator\n.");
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ConstIntegerSubscriptOperatorTest()
{
    fkyaml::Node tmp_node = fkyaml::Node::Sequence();
    tmp_node.ToSequence().emplace_back();
    const fkyaml::Node node = std::move(tmp_node);

    try
    {
        const fkyaml::Node test_node = node[0];
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "Failed to access sequence node value with a subscript operator\n.");
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::SequenceTypeTest()
{
    fkyaml::Node node = fkyaml::Node::Sequence();

    if (node.Type() != fkyaml::NodeType::SEQUENCE)
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::SEQUENCE),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::MappingTypeTest()
{
    fkyaml::Node node = fkyaml::Node::Mapping();

    if (node.Type() != fkyaml::NodeType::MAPPING)
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::MAPPING),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::NullObjectTypeTest()
{
    fkyaml::Node node;

    if (node.Type() != fkyaml::NodeType::NULL_OBJECT)
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::NULL_OBJECT),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::BooleanTypeTest()
{
    fkyaml::Node node = fkyaml::Node::BooleanScalar(false);

    if (node.Type() != fkyaml::NodeType::BOOLEAN)
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::BOOLEAN),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::SignedIntegerTypeTest()
{
    fkyaml::Node node = fkyaml::Node::SignedIntegerScalar(0);

    if (node.Type() != fkyaml::NodeType::SIGNED_INTEGER)
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::SIGNED_INTEGER),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::UnsignedIntegerTypeTest()
{
    fkyaml::Node node = fkyaml::Node::UnsignedIntegerScalar(0);

    if (node.Type() != fkyaml::NodeType::UNSIGNED_INTEGER)
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::UNSIGNED_INTEGER),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::FloatNumberTypeTest()
{
    fkyaml::Node node = fkyaml::Node::FloatNumberScalar(0.0);

    if (node.Type() != fkyaml::NodeType::FLOAT_NUMBER)
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::FLOAT_NUMBER),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::StringTypeTest()
{
    fkyaml::Node node = fkyaml::Node::StringScalar();

    if (node.Type() != fkyaml::NodeType::STRING)
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::STRING),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

// FIXME

int NodeClassTest::IsSequenceTest()
{
    fkyaml::Node node = fkyaml::Node::Sequence();

    if (!node.IsSequence())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::SEQUENCE),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::IsMappingTest()
{
    fkyaml::Node node = fkyaml::Node::Mapping();

    if (!node.IsMapping())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::MAPPING),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::IsNullTest()
{
    fkyaml::Node node;

    if (!node.IsNull())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::NULL_OBJECT),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::IsBooleanTest()
{
    fkyaml::Node node = fkyaml::Node::BooleanScalar(false);

    if (!node.IsBoolean())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::BOOLEAN),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::IsSignedIntegerTest()
{
    fkyaml::Node node = fkyaml::Node::SignedIntegerScalar(0);

    if (!node.IsSignedInteger())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::SIGNED_INTEGER),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::IsUnsignedIntegerTest()
{
    fkyaml::Node node = fkyaml::Node::UnsignedIntegerScalar(0);

    if (!node.IsUnsignedInteger())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::UNSIGNED_INTEGER),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::IsFloatNumberTest()
{
    fkyaml::Node node = fkyaml::Node::FloatNumberScalar(0.0);

    if (!node.IsFloatNumber())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::FLOAT_NUMBER),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::IsStringTest()
{
    fkyaml::Node node = fkyaml::Node::StringScalar();

    if (!node.IsString())
    {
        std::fprintf(
            stderr,
            "Invalid node type. expectation=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::STRING),
            static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::IsScalarTest()
{
    std::vector<fkyaml::Node> container_nodes =
    {
        fkyaml::Node::Sequence(),
        fkyaml::Node::Mapping(),
    };
    for (const auto& container_node : container_nodes)
    {
        if (container_node.IsScalar())
        {
            std::fprintf(
                stderr,
                "Invalid IsScalar() result. expectation=%s, actual=%s, type=%d\n",
                "false",
                container_node.IsScalar() ? "true" : "false",
                static_cast<int>(container_node.Type()));
            return -1;
        }
    }

    std::vector<fkyaml::Node> scalar_nodes =
    {
        fkyaml::Node(),
        fkyaml::Node::BooleanScalar(false),
        fkyaml::Node::SignedIntegerScalar(0),
        fkyaml::Node::UnsignedIntegerScalar(0),
        fkyaml::Node::FloatNumberScalar(0.0),
        fkyaml::Node::StringScalar(),
    };
    for (const auto& scalar_node : scalar_nodes)
    {
        if (!scalar_node.IsScalar())
        {
            std::fprintf(
                stderr,
                "Invalid IsScalar() result. expectation=%s, actual=%s, type=%d\n",
                "true",
                scalar_node.IsScalar() ? "true" : "false",
                static_cast<int>(scalar_node.Type()));
            return -1;
        }
    }

    return 0;
}

int NodeClassTest::SequenceIsEmptyTest()
{
    fkyaml::Node node = fkyaml::Node::Sequence();

    try
    {
        bool ret = node.IsEmpty();
        if (!ret)
        {
            std::fprintf(
                stderr,
                "Invalid IsEmpty() result for an empty sequence node. expectation=%s, actual=%s, size=%zu\n",
                "true",
                node.IsEmpty() ? "true" : "false",
                node.Size());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call IsEmpty for a sequence node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    node.ToSequence().emplace_back();

    try
    {
        bool ret = node.IsEmpty();
        if (ret)
        {
            std::fprintf(
                stderr,
                "Invalid IsEmpty() result for a non-empty sequence node. expectation=%s, actual=%s, size=%zu\n",
                "false",
                node.IsEmpty() ? "true" : "false",
                node.Size());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call IsEmpty for a sequence node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::MappingIsEmptyTest()
{
    fkyaml::Node node = fkyaml::Node::Mapping();

    try
    {
        bool ret = node.IsEmpty();
        if (!ret)
        {
            std::fprintf(
                stderr,
                "Invalid IsEmpty() result for an empty mapping node. expectation=%s, actual=%s, size=%zu\n",
                "true",
                node.IsEmpty() ? "true" : "false",
                node.Size());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call IsEmpty for a mapping node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    node.ToMapping().emplace("test", fkyaml::Node());

    try
    {
        bool ret = node.IsEmpty();
        if (ret)
        {
            std::fprintf(
                stderr,
                "Invalid IsEmpty() result for a non-empty mapping node. expectation=%s, actual=%s, size=%zu\n",
                "false",
                node.IsEmpty() ? "true" : "false",
                node.Size());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call IsEmpty for a mapping node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::StringIsEmptyTest()
{
    fkyaml::Node node = fkyaml::Node::StringScalar();

    try
    {
        bool ret = node.IsEmpty();
        if (!ret)
        {
            std::fprintf(
                stderr,
                "Invalid IsEmpty() result for an empty string node. expectation=%s, actual=%s, size=%zu\n",
                "true",
                node.IsEmpty() ? "true" : "false",
                node.Size());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call IsEmpty for a string node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    node.ToString().append("test");

    try
    {
        bool ret = node.IsEmpty();
        if (ret)
        {
            std::fprintf(
                stderr,
                "Invalid IsEmpty() result for a non-empty string node. expectation=%s, actual=%s, size=%zu\n",
                "false",
                node.IsEmpty() ? "true" : "false",
                node.Size());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call IsEmpty for a string node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::IsEmptyThrowExceptionTest()
{
    std::vector<fkyaml::Node> nodes =
    {
        fkyaml::Node(),
        fkyaml::Node::BooleanScalar(false),
        fkyaml::Node::SignedIntegerScalar(0),
        fkyaml::Node::UnsignedIntegerScalar(0),
        fkyaml::Node::FloatNumberScalar(0.0),
    };

    for (const auto& node : nodes)
    {
        try
        {
            bool ret = node.IsEmpty();
            (void)ret; // suppress warning against "unused variable"

            // Should not come here.
            std::fprintf(
                stderr,
                "Unexpected invocation of IsEmpty(). type=%d\n",
                static_cast<int>(node.Type()));
            return -1;
        }
        catch (const fkyaml::Exception&)
        {
        }
    }

    return 0;
}

int NodeClassTest::SequenceSizeTest()
{
    fkyaml::Node node = fkyaml::Node::Sequence();

    try
    {
        size_t size = node.Size();
        if (size != 0)
        {
            std::fprintf(
                stderr,
                "Invalid sequence node size. expect=%d, actual=%zu\n",
                0,
                node.Size());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call Size for a sequence node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::MappingSizeTest()
{
    fkyaml::Node node = fkyaml::Node::Mapping();

    try
    {
        size_t size = node.Size();
        if (size != 0)
        {
            std::fprintf(
                stderr,
                "Invalid mapping node size. expect=%d, actual=%zu\n",
                0,
                node.Size());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call Size for a mapping node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::StringSizeTest()
{
    fkyaml::Node node = fkyaml::Node::StringScalar();

    try
    {
        size_t size = node.Size();
        if (size != 0)
        {
            std::fprintf(
                stderr,
                "Invalid string node size. expect=%d, actual=%zu\n",
                0,
                node.Size());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call Size for a string node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::SizeThrowExceptionTest()
{
    std::vector<fkyaml::Node> nodes =
    {
        fkyaml::Node(),
        fkyaml::Node::BooleanScalar(false),
        fkyaml::Node::SignedIntegerScalar(0),
        fkyaml::Node::UnsignedIntegerScalar(0),
        fkyaml::Node::FloatNumberScalar(0.0),
    };

    for (const auto& node : nodes)
    {
        try
        {
            size_t ret = node.Size();
            (void)ret; // suppress warning against "unused variable"

            // Should not come here.
            std::fprintf(
                stderr,
                "Unexpected invocation of Size(). type=%d\n",
                static_cast<int>(node.Type()));
            return -1;
        }
        catch (const fkyaml::Exception&)
        {
        }
    }

    return 0;
}

int NodeClassTest::HasAnchorNameTest()
{
    fkyaml::Node node;

    if (node.HasAnchorName())
    {
        std::fprintf(
            stderr,
            "Invalid status of anchor name existence. expect=%s, actual=%s\n",
            "false",
            node.HasAnchorName() ? "true" : "false");
        return -1;
    }

    node.AddAnchorName("anchor");

    if (!node.HasAnchorName())
    {
        std::fprintf(
            stderr,
            "Invalid status of anchor name existence. expect=%s, actual=%s\n",
            "true",
            node.HasAnchorName() ? "true" : "false");
        return -1;
    }

    return 0;
}

int NodeClassTest::GetAnchorNameTest()
{
    fkyaml::Node node;

    try
    {
        const std::string& ret = node.GetAnchorName();

        // Should not come here.
        std::fprintf(
            stderr,
            "Unexpected invocation of GetAnchorName() on a Node object without anchor name. ret=\"%s\"\n",
            ret.c_str());
        return -1;
    }
    catch (const fkyaml::Exception&)
    {
    }

    node.AddAnchorName("anchor");

    try
    {
        const std::string& anchor_name = node.GetAnchorName();
        if (anchor_name.compare("anchor") != 0)
        {
            std::fprintf(
                stderr,
                "Invalid anchor name value. expect=\"%s\", actual=\"%s\"\n",
                "anchor",
                anchor_name.c_str());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "Failed to invoke GetAnchorName() on a Node object with anchor name.\n");
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::FirstAddLvalueAnchorNameTest()
{
    fkyaml::Node node;
    std::string anchor_name = "anchor";
    node.AddAnchorName(anchor_name);

    try
    {
        const std::string& anchor_name_ret = node.GetAnchorName();
        if (anchor_name_ret != anchor_name)
        {
            std::fprintf(
                stderr,
                "Invalid anchor name value. expect=\"%s\", actual=\"%s\"\n",
                anchor_name.c_str(),
                anchor_name_ret.c_str());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "Failed to set anchor name.\n");
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::FirstAddRvalueAnchorNameTest()
{
    fkyaml::Node node;
    std::string anchor_name = "anchor";
    node.AddAnchorName(std::move(anchor_name));

    try
    {
        const std::string& anchor_name_ret = node.GetAnchorName();
        if (anchor_name_ret.compare("anchor") != 0)
        {
            std::fprintf(
                stderr,
                "Invalid anchor name value. expect=\"%s\", actual=\"%s\"\n",
                "anchor",
                anchor_name_ret.c_str());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "Failed to set anchor name.\n");
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::TwiceAddLvalueAnchorNameTest()
{
    fkyaml::Node node;
    std::string anchor_name_first = "anchor_name_first";
    node.AddAnchorName(anchor_name_first);
    std::string anchor_name_second = "anchor_name_second";
    node.AddAnchorName(anchor_name_second);

    try
    {
        const std::string& anchor_name_ret = node.GetAnchorName();

        if (anchor_name_ret == anchor_name_first)
        {
            std::fprintf(
                stderr,
                "Failed to overwrite anchor name value. expect=\"%s\", actual=\"%s\"\n",
                anchor_name_second.c_str(),
                anchor_name_ret.c_str());
            return -1;
        }

        if (anchor_name_ret != anchor_name_second)
        {
            std::fprintf(
                stderr,
                "Invalid anchor name value. expect=\"%s\", actual=\"%s\"\n",
                anchor_name_second.c_str(),
                anchor_name_ret.c_str());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "Failed to set anchor name.\n");
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::TwiceAddRvalueAnchorNameTest()
{
    fkyaml::Node node;
    std::string anchor_name_first = "anchor_name_first";
    node.AddAnchorName(std::move(anchor_name_first));
    std::string anchor_name_second = "anchor_name_second";
    node.AddAnchorName(std::move(anchor_name_second));

    try
    {
        const std::string& anchor_name_ret = node.GetAnchorName();

        if (anchor_name_ret.compare("anchor_name_first") == 0)
        {
            std::fprintf(
                stderr,
                "Failed to overwrite anchor name value. expect=\"%s\", actual=\"%s\"\n",
                "anchor_name_second",
                anchor_name_ret.c_str());
            return -1;
        }

        if (anchor_name_ret.compare("anchor_name_second") != 0)
        {
            std::fprintf(
                stderr,
                "Invalid anchor name value. expect=\"%s\", actual=\"%s\"\n",
                "anchor_name_second",
                anchor_name_ret.c_str());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "Failed to set anchor name.\n");
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ToSequenceTest()
{
    fkyaml::Node node = fkyaml::Node::Sequence();

    try
    {
        fkyaml::NodeSequenceType& seq = node.ToSequence();
        if (seq.size() != 0)
        {
            std::fprintf(
                stderr,
                "Invalid sequence size. expect=%d, actual=%zu\n",
                0,
                seq.size());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call ToSequence for a sequence node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ConstToSequenceTest()
{
    const fkyaml::Node node = fkyaml::Node::Sequence();

    try
    {
        const fkyaml::NodeSequenceType& seq = node.ToSequence();
        if (seq.size() != 0)
        {
            std::fprintf(
                stderr,
                "Invalid sequence size. expect=%d, actual=%zu\n",
                0,
                seq.size());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call ToSequence for a sequence node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ToSequenceThrowExceptionTest()
{
    std::vector<fkyaml::Node> nodes =
    {
        fkyaml::Node::Mapping(),
        fkyaml::Node(),
        fkyaml::Node::BooleanScalar(false),
        fkyaml::Node::SignedIntegerScalar(0),
        fkyaml::Node::UnsignedIntegerScalar(0),
        fkyaml::Node::FloatNumberScalar(0.0),
        fkyaml::Node::StringScalar(),
    };

    for (const auto& node : nodes)
    {
        try
        {
            fkyaml::NodeSequenceType ret = node.ToSequence();
            (void)ret; // suppress warning against "unused variable"

            // Should not come here.
            std::fprintf(
                stderr,
                "Unexpected invocation of ToSequence(). type=%d\n",
                static_cast<int>(node.Type()));
            return -1;
        }
        catch (const fkyaml::Exception&)
        {
        }
    }

    return 0;
}

int NodeClassTest::ToMappingTest()
{
    fkyaml::Node node = fkyaml::Node::Mapping();

    try
    {
        fkyaml::NodeMappingType& map = node.ToMapping();
        if (map.size() != 0)
        {
            std::fprintf(
                stderr,
                "Invalid mapping size. expect=%d, actual=%zu\n",
                0,
                map.size());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call ToMapping for a mapping node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ConstToMappingTest()
{
    const fkyaml::Node node = fkyaml::Node::Mapping();

    try
    {
        const fkyaml::NodeMappingType& map = node.ToMapping();
        if (map.size() != 0)
        {
            std::fprintf(
                stderr,
                "Invalid mapping size. expect=%d, actual=%zu\n",
                0,
                map.size());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call ToMapping for a mapping node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ToMappingThrowExceptionTest()
{
    std::vector<fkyaml::Node> nodes =
    {
        fkyaml::Node::Sequence(),
        fkyaml::Node(),
        fkyaml::Node::BooleanScalar(false),
        fkyaml::Node::SignedIntegerScalar(0),
        fkyaml::Node::UnsignedIntegerScalar(0),
        fkyaml::Node::FloatNumberScalar(0.0),
        fkyaml::Node::StringScalar(),
    };

    for (const auto& node : nodes)
    {
        try
        {
            fkyaml::NodeMappingType ret = node.ToMapping();
            (void)ret; // suppress warning against "unused variable"

            // Should not come here.
            std::fprintf(
                stderr,
                "Unexpected invocation of ToMapping(). type=%d\n",
                static_cast<int>(node.Type()));
            return -1;
        }
        catch (const fkyaml::Exception&)
        {
        }
    }

    return 0;
}

int NodeClassTest::ToBooleanTest()
{
    fkyaml::Node node = fkyaml::Node::BooleanScalar(false);

    try
    {
        fkyaml::NodeBooleanType& boolean = node.ToBoolean();
        if (boolean != false)
        {
            std::fprintf(
                stderr,
                "Invalid node value. expect=%s, actual=%s\n",
                "false",
                boolean ? "true" : "false");
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call ToBoolean for a boolean node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ConstToBooleanTest()
{
    const fkyaml::Node node = fkyaml::Node::BooleanScalar(false);

    try
    {
        const fkyaml::NodeBooleanType& boolean = node.ToBoolean();
        if (boolean != false)
        {
            std::fprintf(
                stderr,
                "Invalid node value. expect=%s, actual=%s\n",
                "false",
                boolean ? "true" : "false");
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call ToBoolean for a boolean node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ToBooleanThrowExceptionTest()
{
    std::vector<fkyaml::Node> nodes =
    {
        fkyaml::Node::Sequence(),
        fkyaml::Node::Mapping(),
        fkyaml::Node(),
        fkyaml::Node::SignedIntegerScalar(0),
        fkyaml::Node::UnsignedIntegerScalar(0),
        fkyaml::Node::FloatNumberScalar(0.0),
        fkyaml::Node::StringScalar(),
    };

    for (const auto& node : nodes)
    {
        try
        {
            fkyaml::NodeBooleanType ret = node.ToBoolean();
            (void)ret; // suppress warning against "unused variable"

            // Should not come here.
            std::fprintf(
                stderr,
                "Unexpected invocation of ToBoolean(). type=%d\n",
                static_cast<int>(node.Type()));
            return -1;
        }
        catch (const fkyaml::Exception&)
        {
        }
    }

    return 0;
}

int NodeClassTest::ToSignedIntegerTest()
{
    fkyaml::Node node = fkyaml::Node::SignedIntegerScalar(0);

    try
    {
        fkyaml::NodeSignedIntType& signed_int = node.ToSignedInteger();
        if (signed_int != 0)
        {
            std::fprintf(
                stderr,
                "Invalid node value. expect=%d, actual=%" PRId64 "\n",
                0,
                signed_int);
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call ToSignedInteger for a signed integer node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ConstToSignedIntegerTest()
{
    const fkyaml::Node node = fkyaml::Node::SignedIntegerScalar(0);

    try
    {
        const fkyaml::NodeSignedIntType& signed_int = node.ToSignedInteger();
        if (signed_int != 0)
        {
            std::fprintf(
                stderr,
                "Invalid node value. expect=%d, actual=%" PRId64 "\n",
                0,
                signed_int);
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call ToSignedInteger for a signed integer node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ToSignedIntegerThrowExceptionTest()
{
    std::vector<fkyaml::Node> nodes =
    {
        fkyaml::Node::Sequence(),
        fkyaml::Node::Mapping(),
        fkyaml::Node(),
        fkyaml::Node::BooleanScalar(false),
        fkyaml::Node::UnsignedIntegerScalar(0),
        fkyaml::Node::FloatNumberScalar(0.0),
        fkyaml::Node::StringScalar(),
    };

    for (const auto& node : nodes)
    {
        try
        {
            fkyaml::NodeSignedIntType ret = node.ToSignedInteger();
            (void)ret; // suppress warning against "unused variable"

            // Should not come here.
            std::fprintf(
                stderr,
                "Unexpected invocation of ToSignedInteger(). type=%d\n",
                static_cast<int>(node.Type()));
            return -1;
        }
        catch (const fkyaml::Exception&)
        {
        }
    }

    return 0;
}

int NodeClassTest::ToUnsignedIntegerTest()
{
    fkyaml::Node node = fkyaml::Node::UnsignedIntegerScalar(0);

    try
    {
        fkyaml::NodeUnsignedIntType& unsigned_int = node.ToUnsignedInteger();
        if (unsigned_int != 0)
        {
            std::fprintf(
                stderr,
                "Invalid node value. expect=%d, actual=%" PRIu64 "\n",
                0,
                unsigned_int);
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call ToUnsignedInteger for an unsigned integer node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ConstToUnsignedIntegerTest()
{
    const fkyaml::Node node = fkyaml::Node::UnsignedIntegerScalar(0);

    try
    {
        const fkyaml::NodeUnsignedIntType& signed_int = node.ToUnsignedInteger();
        if (signed_int != 0)
        {
            std::fprintf(
                stderr,
                "Invalid node value. expect=%d, actual=%" PRId64 "\n",
                0,
                signed_int);
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call ToUnsignedInteger for an unsigned integer node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ToUnsignedIntegerThrowExceptionTest()
{
    std::vector<fkyaml::Node> nodes =
    {
        fkyaml::Node::Sequence(),
        fkyaml::Node::Mapping(),
        fkyaml::Node(),
        fkyaml::Node::BooleanScalar(false),
        fkyaml::Node::SignedIntegerScalar(0),
        fkyaml::Node::FloatNumberScalar(0.0),
        fkyaml::Node::StringScalar(),
    };

    for (const auto& node : nodes)
    {
        try
        {
            fkyaml::NodeSignedIntType ret = node.ToUnsignedInteger();
            (void)ret; // suppress warning against "unused variable"

            // Should not come here.
            std::fprintf(
                stderr,
                "Unexpected invocation of ToUnsignedInteger(). type=%d\n",
                static_cast<int>(node.Type()));
            return -1;
        }
        catch (const fkyaml::Exception&)
        {
        }
    }

    return 0;
}

int NodeClassTest::ToFloatNumberTest()
{
    fkyaml::Node node = fkyaml::Node::FloatNumberScalar(0.0);

    try
    {
        fkyaml::NodeFloatNumberType& float_val = node.ToFloatNumber();
        if (float_val != 0.0)
        {
            std::fprintf(
                stderr,
                "Invalid node value. expect=%lf, actual=%lf\n",
                0.0,
                float_val);
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call ToFloatNumber for a float number node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ConstToFloatNumberTest()
{
    const fkyaml::Node node = fkyaml::Node::FloatNumberScalar(0.0);

    try
    {
        const fkyaml::NodeFloatNumberType& float_val = node.ToFloatNumber();
        if (float_val != 0)
        {
            std::fprintf(
                stderr,
                "Invalid node value. expect=%lf, actual=%lf\n",
                0.0,
                float_val);
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call ToFloatNumber for a float number node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ToFloatNumberThrowExceptionTest()
{
    std::vector<fkyaml::Node> nodes =
    {
        fkyaml::Node::Sequence(),
        fkyaml::Node::Mapping(),
        fkyaml::Node(),
        fkyaml::Node::BooleanScalar(false),
        fkyaml::Node::SignedIntegerScalar(0),
        fkyaml::Node::UnsignedIntegerScalar(0),
        fkyaml::Node::StringScalar(),
    };

    for (const auto& node : nodes)
    {
        try
        {
            fkyaml::NodeFloatNumberType ret = node.ToFloatNumber();
            (void)ret; // suppress warning against "unused variable"

            // Should not come here.
            std::fprintf(
                stderr,
                "Unexpected invocation of ToFloatNumber(). type=%d\n",
                static_cast<int>(node.Type()));
            return -1;
        }
        catch (const fkyaml::Exception&)
        {
        }
    }

    return 0;
}

// FIXME

int NodeClassTest::ToStringTest()
{
    fkyaml::Node node = fkyaml::Node::StringScalar();

    try
    {
        fkyaml::NodeStringType& str = node.ToString();
        if (!str.empty())
        {
            std::fprintf(
                stderr,
                "Invalid node value. expect=\"%s\", actual=\"%s\"\n",
                "",
                str.c_str());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call ToString for a string node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ConstToStringTest()
{
    const fkyaml::Node node = fkyaml::Node::StringScalar();

    try
    {
        const fkyaml::NodeStringType& str = node.ToString();
        if (!str.empty())
        {
            std::fprintf(
                stderr,
                "Invalid node value. expect=\"%s\", actual=\"%s\"\n",
                "",
                str.c_str());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(
            stderr,
            "Failed to call ToString for a string node. type=%d\n",
            static_cast<int>(node.Type()));
        std::fprintf(stderr, "%s\n", e.what());
        return -1;
    }

    return 0;
}

int NodeClassTest::ToStringThrowExceptionTest()
{
    std::vector<fkyaml::Node> nodes =
    {
        fkyaml::Node::Sequence(),
        fkyaml::Node::Mapping(),
        fkyaml::Node(),
        fkyaml::Node::BooleanScalar(false),
        fkyaml::Node::SignedIntegerScalar(0),
        fkyaml::Node::UnsignedIntegerScalar(0),
        fkyaml::Node::FloatNumberScalar(0.0),
    };

    for (const auto& node : nodes)
    {
        try
        {
            fkyaml::NodeStringType ret = node.ToString();
            (void)ret; // suppress warning against "unused variable"

            // Should not come here.
            std::fprintf(
                stderr,
                "Unexpected invocation of ToString(). type=%d\n",
                static_cast<int>(node.Type()));
            return -1;
        }
        catch (const fkyaml::Exception&)
        {
        }
    }

    return 0;
}

int NodeClassTest::SwapTest()
{
    fkyaml::Node lhs_node = fkyaml::Node::BooleanScalar(true);
    fkyaml::Node rhs_node = fkyaml::Node::SignedIntegerScalar(123);

    lhs_node.Swap(rhs_node);

    if (!lhs_node.IsSignedInteger())
    {
        std::fprintf(
            stderr,
            "Invalid lhs_node type after swapping. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::SIGNED_INTEGER),
            static_cast<int>(lhs_node.Type()));
        return -1;
    }

    if (lhs_node.ToSignedInteger() != 123)
    {
        std::fprintf(
            stderr,
            "Invalid lhs_node value after swapping. expect=%d, actual=%" PRId64 "\n",
            123,
            lhs_node.ToSignedInteger());
        return -1;
    }

    if (!rhs_node.IsBoolean())
    {
        std::fprintf(
            stderr,
            "Invalid rhs_node type after swapping. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::BOOLEAN),
            static_cast<int>(rhs_node.Type()));
        return -1;
    }

    if (rhs_node.ToBoolean() != true)
    {
        std::fprintf(
            stderr,
            "Invalid rhs_node value after swapping. expect=%s, actual=%s\n",
            "true",
            rhs_node.ToBoolean() ? "true" : "false");
        return -1;
    }

    return 0;
}

int NodeClassTest::StdSwapTest()
{
    fkyaml::Node lhs_node = fkyaml::Node::BooleanScalar(true);
    fkyaml::Node rhs_node = fkyaml::Node::SignedIntegerScalar(123);

    std::swap(lhs_node, rhs_node);

    if (!lhs_node.IsSignedInteger())
    {
        std::fprintf(
            stderr,
            "Invalid lhs_node type after swapping. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::SIGNED_INTEGER),
            static_cast<int>(lhs_node.Type()));
        return -1;
    }

    if (lhs_node.ToSignedInteger() != 123)
    {
        std::fprintf(
            stderr,
            "Invalid lhs_node value after swapping. expect=%d, actual=%" PRId64 "\n",
            123,
            lhs_node.ToSignedInteger());
        return -1;
    }

    if (!rhs_node.IsBoolean())
    {
        std::fprintf(
            stderr,
            "Invalid rhs_node type after swapping. expect=%d, actual=%d\n",
            static_cast<int>(fkyaml::NodeType::BOOLEAN),
            static_cast<int>(rhs_node.Type()));
        return -1;
    }

    if (rhs_node.ToBoolean() != true)
    {
        std::fprintf(
            stderr,
            "Invalid rhs_node value after swapping. expect=%s, actual=%s\n",
            "true",
            rhs_node.ToBoolean() ? "true" : "false");
        return -1;
    }

    return 0;
}

int NodeClassTest::SequenceForLoopTest()
{
    fkyaml::Node node = fkyaml::Node::Sequence(
        {fkyaml::Node::SignedIntegerScalar(0),
         fkyaml::Node::SignedIntegerScalar(1),
         fkyaml::Node::SignedIntegerScalar(2)});

    if (!node.IsSequence() || node.Size() != 3)
    {
        std::fprintf(
            stderr, "node initialization failure. type=%d, size=%zu\n", static_cast<int>(node.Type()), node.Size());
        return -1;
    }

    fkyaml::NodeSignedIntType value = 0;
    for (auto&& item : node.ToSequence())
    {
        if (!item.IsSignedInteger())
        {
            std::fprintf(
                stderr,
                "value type of the target sequence node is invalid. expectation=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::SIGNED_INTEGER),
                static_cast<int>(item.Type()));
            return -1;
        }

        if (item.ToSignedInteger() != value)
        {
            std::fprintf(
                stderr,
                "value of the target sequence node is invalid. expectation=%" PRId64 ", actual=%" PRId64 "\n",
                value,
                item.ToSignedInteger());
            return -1;
        }

        ++value;
    }

    return 0;
}

int NodeClassTest::ConstSequenceForLoopTest()
{
    const fkyaml::Node node = fkyaml::Node::Sequence(
        {fkyaml::Node::SignedIntegerScalar(0),
         fkyaml::Node::SignedIntegerScalar(1),
         fkyaml::Node::SignedIntegerScalar(2)});

    if (!node.IsSequence() || node.Size() != 3)
    {
        std::fprintf(
            stderr, "node initialization failure. type=%d, size=%zu\n", static_cast<int>(node.Type()), node.Size());
        return -1;
    }

    fkyaml::NodeSignedIntType value = 0;
    for (auto&& item : node.ToSequence())
    {
        if (!item.IsSignedInteger())
        {
            std::fprintf(
                stderr,
                "value type of the target sequence node is invalid. expectation=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::SIGNED_INTEGER),
                static_cast<int>(item.Type()));
            return -1;
        }

        if (item.ToSignedInteger() != value)
        {
            std::fprintf(
                stderr,
                "value of the target sequence node is invalid. expectation=%" PRId64 ", actual=%" PRId64 "\n",
                value,
                item.ToSignedInteger());
            return -1;
        }

        ++value;
    }

    return 0;
}
