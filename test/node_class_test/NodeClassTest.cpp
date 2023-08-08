#include "NodeClassTest.hpp"

#include <cstdio>

#include "fkYAML/Node.hpp"

int NodeClassTest::DefaultCtorTest()
{
    fkyaml::Node node;

    if (!node.IsNull())
    {
        std::fprintf(stderr, "node type is not the null type.");
        return -1;
    }

    return 0;
}

int NodeClassTest::SequenceNodeFactoryTest()
{
    fkyaml::Node node = fkyaml::Node::Sequence(fkyaml::NodeSequenceType(3));

    if (!node.IsSequence())
    {
        std::fprintf(stderr, "node type is not the sequence type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 3)
    {
        std::fprintf(stderr, "the size of the sequence node is not 3. actual=%llu\n", node.Size());
        return -1;
    }

    for (int i = 0; i < 3; ++i)
    {
        if (!node[i].IsNull())
        {
            std::fprintf(
                stderr,
                "type error of the sequence node value. index=%d, expectation=%d, actual=%d\n",
                i, static_cast<int>(fkyaml::NodeType::NULL_OBJECT), static_cast<int>(node[i].Type())
            );
            return -1;
        }
    }

    return 0;
}

int NodeClassTest::MappingNodeFactoryTest()
{
    fkyaml::Node node = fkyaml::Node::Mapping(fkyaml::NodeMappingType{{std::string("test"), fkyaml::Node::BooleanScalar(true)}});

    if (!node.IsMapping())
    {
        std::fprintf(stderr, "node type is not the mapping type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 1)
    {
        std::fprintf(stderr, "the size of the mapping node is not 1. actual=%llu\n", node.Size());
        return -1;
    }

    if (!node["test"].IsBoolean())
    {
        std::fprintf(
            stderr,
            "type error of the mapping node value. key=%s, expectation=%d, actual=%d\n",
            "test", static_cast<int>(fkyaml::NodeType::BOOLEAN), static_cast<int>(node.Type())
        );
        return -1;
    }

    if (node["test"].Value<fkyaml::NodeBooleanType>() != true)
    {
        std::fprintf(
            stderr,
            "value error of the mapping node value. key=%s, expectation=%s, actual=%s\n",
            "test", "true", node["test"].Value<fkyaml::NodeBooleanType>() ? "true" : "false"
        );
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

    if (node.Value<fkyaml::NodeBooleanType>() != true)
    {
        std::fprintf(stderr, "node value error. expectation=%s, actual=%s\n", "true", node.Value<fkyaml::NodeBooleanType>() ? "true" : "false");
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

    if (node.Value<fkyaml::NodeSignedIntType>() != -128)
    {
        std::fprintf(stderr, "node value error. expectation=%d, actual=%lld\n", -128, node.Value<fkyaml::NodeSignedIntType>());
        return -1;
    }

    return 0;
}

int NodeClassTest::UnsignedIntNodeFactoryTest()
{
    fkyaml::Node node = fkyaml::Node::UnsignedIntegerScalar(255);

    if (!node.IsUnsignedInteger())
    {
        std::fprintf(stderr, "node type is not the unsigned integer type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Value<fkyaml::NodeUnsignedIntType>() != 255)
    {
        std::fprintf(stderr, "node value error. expectation=%u, actual=%llu", 255, node.Value<fkyaml::NodeUnsignedIntType>());
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

    if (node.Value<fkyaml::NodeFloatNumberType>() != 3.141952)
    {
        std::fprintf(stderr, "node value error. expectation=%lf, actual=%lf", 3.141952, node.Value<fkyaml::NodeFloatNumberType>());
        return -1;
    }

    return 0;
}

int NodeClassTest::StringNodeFactoryTest()
{
    fkyaml::Node node = fkyaml::Node::StringScalar("test");

    if (!node.IsString())
    {
        std::fprintf(stderr, "node type is not the string type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 4)
    {
        std::fprintf(stderr, "the size of the string node is not 4. actual=%llu\n", node.Size());
        return -1;
    }

    std::string value = node.Value<fkyaml::NodeStringType>();
    if (value.compare("test") != 0)
    {
        std::fprintf(stderr, "the content of the string node is not \"test\". actual=%s\n", value.c_str());
        return -1;
    }

    return 0;
}
