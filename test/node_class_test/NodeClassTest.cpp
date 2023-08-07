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

int NodeClassTest::CtorWithBoolTest()
{
    fkyaml::Node node(true);

    if (!node.IsBoolean())
    {
        std::fprintf(stderr, "node type is not the boolean type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}

int NodeClassTest::CtorWithSequenceTest()
{
    fkyaml::Node node(fkyaml::NodeSequenceType(3));

    if (!node.IsSequence())
    {
        std::fprintf(stderr, "node type is not the sequence type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 3)
    {
        std::fprintf(stderr, "the size of the sequence node is not 3. actual=%llu", node.Size());
        return -1;
    }

    return 0;
}

int NodeClassTest::CtorWithStringTest()
{
    fkyaml::Node node(fkyaml::NodeStringType("test"));

    if (!node.IsString())
    {
        std::fprintf(stderr, "node type is not the string type. type=%d\n", static_cast<int>(node.Type()));
        return -1;
    }

    if (node.Size() != 4)
    {
        std::fprintf(stderr, "the size of the string node is not 4. actual=%llu", node.Size());
        return -1;
    }

    std::string value = node.Value<fkyaml::NodeStringType>();
    if (value.compare("test") != 0)
    {
        std::fprintf(stderr, "the content of the string node is not \"test\". actual=%s", value.c_str());
        return -1;
    }

    return 0;
}
