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
        std::fprintf(stderr, "node type is not the boolean type. type=%d", static_cast<int>(node.Type()));
        return -1;
    }

    return 0;
}
