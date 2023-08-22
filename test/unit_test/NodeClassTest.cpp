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
