#include "DeserializerTest.hpp"

#include <cinttypes>
#include <cstdio>

#include "fkYAML/Deserializer.hpp"

int DeserializerTest::DeserializeFlowSequenceObject1()
{
    fkyaml::Deserializer deserializer;
    try
    {
        fkyaml::Node root = deserializer.Deserialize("test: [ foo, bar ]");

        if (!root.IsMapping())
        {
            std::fprintf(
                stderr,
                "Invalid root node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::MAPPING),
                static_cast<int>(root.Type()));
            return -1;
        }

        if (root.Size() != 1)
        {
            std::fprintf(
                stderr,
                "Invalid root node size. expect=%d, actual=%zu",
                1,
                root.Size());
            return -1;
        }

        fkyaml::Node& test_node = root["test"];
        if (!test_node.IsSequence())
        {
            std::fprintf(
                stderr,
                "Invalid test node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::SEQUENCE),
                static_cast<int>(test_node.Type()));
            return -1;
        }

        if (test_node.Size() != 2)
        {
            std::fprintf(
                stderr,
                "Invalid test node size. expect=%d, actual=%zu",
                2,
                test_node.Size());
            return -1;
        }

        fkyaml::Node& test_node_0 = test_node[0];
        if (!test_node_0.IsString())
        {
            std::fprintf(
                stderr,
                "Invalid test node type(index=0). expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::STRING),
                static_cast<int>(test_node_0.Type()));
            return -1;
        }

        if (test_node_0.ToString().compare("foo") != 0)
        {
            std::fprintf(
                stderr,
                "Invalid test node value(index=0). expect=%s, actual=%s\n",
                "foo",
                test_node_0.ToString().c_str());
            return -1;
        }

        fkyaml::Node& test_node_1 = test_node[1];
        if (!test_node_1.IsString())
        {
            std::fprintf(
                stderr,
                "Invalid test node type(index=1). expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::STRING),
                static_cast<int>(test_node_1.Type()));
            return -1;
        }

        if (test_node_1.ToString().compare("bar") != 0)
        {
            std::fprintf(
                stderr,
                "Invalid foo node value. expect=%s, actual=%s\n",
                "bar",
                test_node_1.ToString().c_str());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    return 0;
}

int DeserializerTest::DeserializeFlowMappingObject1()
{
    fkyaml::Deserializer deserializer;
    try
    {
        fkyaml::Node root = deserializer.Deserialize("test: { bool: true, foo: bar, pi: 3.14 }");

        if (!root.IsMapping())
        {
            std::fprintf(
                stderr,
                "Invalid root node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::MAPPING),
                static_cast<int>(root.Type()));
            return -1;
        }

        if (root.Size() != 1)
        {
            std::fprintf(
                stderr,
                "Invalid root node size. expect=%d, actual=%zu",
                1,
                root.Size());
            return -1;
        }

        fkyaml::Node& test_node = root["test"];
        if (!test_node.IsMapping())
        {
            std::fprintf(
                stderr,
                "Invalid test node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::MAPPING),
                static_cast<int>(test_node.Type()));
            return -1;
        }

        if (test_node.Size() != 3)
        {
            std::fprintf(
                stderr,
                "Invalid test node size. expect=%d, actual=%zu",
                3,
                test_node.Size());
            return -1;
        }

        fkyaml::Node& bool_node = test_node["bool"];
        if (!bool_node.IsBoolean())
        {
            std::fprintf(
                stderr,
                "Invalid bool node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::BOOLEAN),
                static_cast<int>(test_node.Type()));
            return -1;
        }

        if (bool_node.ToBoolean() != true)
        {
            std::fprintf(
                stderr,
                "Invalid bool node value. expect=%s, actual=%s\n",
                "true",
                bool_node.ToBoolean() ? "true" : "false");
            return -1;
        }

        fkyaml::Node& foo_node = test_node["foo"];
        if (!foo_node.IsString())
        {
            std::fprintf(
                stderr,
                "Invalid foo node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::STRING),
                static_cast<int>(test_node.Type()));
            return -1;
        }

        if (foo_node.ToString().compare("bar") != 0)
        {
            std::fprintf(
                stderr,
                "Invalid foo node value. expect=%s, actual=%s\n",
                "bar",
                foo_node.ToString().c_str());
            return -1;
        }

        fkyaml::Node& pi_node = test_node["pi"];
        if (!pi_node.IsFloatNumber())
        {
            std::fprintf(
                stderr,
                "Invalid pi node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::FLOAT_NUMBER),
                static_cast<int>(pi_node.Type()));
            return -1;
        }

        if (pi_node.ToFloatNumber() != 3.14)
        {
            std::fprintf(
                stderr,
                "Invalid foo node value. expect=%lf, actual=%lf\n",
                3.14,
                pi_node.ToFloatNumber());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    return 0;
}

int DeserializerTest::DeserializeBlockSequenceObject1()
{
    fkyaml::Deserializer deserializer;
    try
    {
        fkyaml::Node root = deserializer.Deserialize("test:\n  - foo\n  - bar");

        if (!root.IsMapping())
        {
            std::fprintf(
                stderr,
                "Invalid root node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::MAPPING),
                static_cast<int>(root.Type()));
            return -1;
        }

        if (root.Size() != 1)
        {
            std::fprintf(
                stderr,
                "Invalid root node size. expect=%d, actual=%zu",
                1,
                root.Size());
            return -1;
        }

        fkyaml::Node& test_node = root["test"];
        if (!test_node.IsSequence())
        {
            std::fprintf(
                stderr,
                "Invalid test node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::SEQUENCE),
                static_cast<int>(test_node.Type()));
            return -1;
        }

        if (test_node.Size() != 2)
        {
            std::fprintf(
                stderr,
                "Invalid test node size. expect=%d, actual=%zu",
                2,
                test_node.Size());
            return -1;
        }

        fkyaml::Node& test_node_0 = test_node[0];
        if (!test_node_0.IsString())
        {
            std::fprintf(
                stderr,
                "Invalid test node type(index=0). expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::STRING),
                static_cast<int>(test_node_0.Type()));
            return -1;
        }

        if (test_node_0.ToString().compare("foo") != 0)
        {
            std::fprintf(
                stderr,
                "Invalid test node value(index=0). expect=%s, actual=%s\n",
                "foo",
                test_node_0.ToString().c_str());
            return -1;
        }

        fkyaml::Node& test_node_1 = test_node[1];
        if (!test_node_1.IsString())
        {
            std::fprintf(
                stderr,
                "Invalid test node type(index=1). expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::STRING),
                static_cast<int>(test_node_1.Type()));
            return -1;
        }

        if (test_node_1.ToString().compare("bar") != 0)
        {
            std::fprintf(
                stderr,
                "Invalid foo node value. expect=%s, actual=%s\n",
                "bar",
                test_node_1.ToString().c_str());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    return 0;
}

int DeserializerTest::DeserializeBlockSequenceObject2()
{
    fkyaml::Deserializer deserializer;
    try
    {
        fkyaml::Node root = deserializer.Deserialize("test:\n  - foo: true\n    bar: one\n  - foo: false\n    bar: two");

        if (!root.IsMapping())
        {
            std::fprintf(
                stderr,
                "Invalid root node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::MAPPING),
                static_cast<int>(root.Type()));
            return -1;
        }

        if (root.Size() != 1)
        {
            std::fprintf(
                stderr,
                "Invalid root node size. expect=%d, actual=%zu",
                1,
                root.Size());
            return -1;
        }

        fkyaml::Node& test_node = root["test"];
        if (!test_node.IsSequence())
        {
            std::fprintf(
                stderr,
                "Invalid test node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::SEQUENCE),
                static_cast<int>(test_node.Type()));
            return -1;
        }

        if (test_node.Size() != 2)
        {
            std::fprintf(
                stderr,
                "Invalid test_node size. expect=%d, actual=%zu",
                2,
                test_node.Size());
            return -1;
        }

        fkyaml::Node& test_node_0 = test_node[0];
        if (!test_node_0.IsMapping())
        {
            std::fprintf(
                stderr,
                "Invalid test_node type(index=0). expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::MAPPING),
                static_cast<int>(test_node_0.Type()));
            return -1;
        }

        if (test_node_0.Size() != 2)
        {
            std::fprintf(
                stderr,
                "Invalid test_node[0] size. expect=%d, actual=%zu",
                2,
                test_node_0.Size());
            return -1;
        }

        fkyaml::Node& test_node_0_foo = test_node_0["foo"];
        if (!test_node_0_foo.IsBoolean())
        {
            std::fprintf(
                stderr,
                "Invalid test_node_0_foo type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::BOOLEAN),
                static_cast<int>(test_node_0_foo.Type()));
            return -1;
        }

        if (test_node_0_foo.ToBoolean() != true)
        {
            std::fprintf(
                stderr,
                "Invalid test_node_0_foo value. expect=%s, actual=%s\n",
                "true",
                test_node_0_foo.ToBoolean() ? "true" : "false");
            return -1;
        }

        fkyaml::Node& test_node_0_bar = test_node_0["bar"];
        if (!test_node_0_bar.IsString())
        {
            std::fprintf(
                stderr,
                "Invalid test_node_0_bar type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::STRING),
                static_cast<int>(test_node_0_bar.Type()));
            return -1;
        }

        if (test_node_0_bar.ToString().compare("one") != 0)
        {
            std::fprintf(
                stderr,
                "Invalid test_node_0_bar value. expect=\"%s\", actual=\"%s\"\n",
                "one",
                test_node_0_bar.ToString().c_str());
            return -1;
        }

        fkyaml::Node& test_node_1 = test_node[1];
        if (!test_node_1.IsMapping())
        {
            std::fprintf(
                stderr,
                "Invalid test_node type(index=0). expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::MAPPING),
                static_cast<int>(test_node_1.Type()));
            return -1;
        }

        if (test_node_1.Size() != 2)
        {
            std::fprintf(
                stderr,
                "Invalid test_node[1] size. expect=%d, actual=%zu",
                2,
                test_node_1.Size());
            return -1;
        }

        fkyaml::Node& test_node_1_foo = test_node_1["foo"];
        if (!test_node_1_foo.IsBoolean())
        {
            std::fprintf(
                stderr,
                "Invalid test_node_1_foo type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::BOOLEAN),
                static_cast<int>(test_node_1_foo.Type()));
            return -1;
        }

        if (test_node_1_foo.ToBoolean() != false)
        {
            std::fprintf(
                stderr,
                "Invalid test_node_1_foo value. expect=%s, actual=%s\n",
                "false",
                test_node_1_foo.ToBoolean() ? "true" : "false");
            return -1;
        }

        fkyaml::Node& test_node_1_bar = test_node_1["bar"];
        if (!test_node_1_bar.IsString())
        {
            std::fprintf(
                stderr,
                "Invalid test_node_1_bar type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::STRING),
                static_cast<int>(test_node_1_bar.Type()));
            return -1;
        }

        if (test_node_1_bar.ToString().compare("two") != 0)
        {
            std::fprintf(
                stderr,
                "Invalid test_node_1_bar value. expect=\"%s\", actual=\"%s\"\n",
                "two",
                test_node_1_bar.ToString().c_str());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    return 0;
}

int DeserializerTest::DeserializeBlockMappingObject1()
{
    fkyaml::Deserializer deserializer;
    try
    {
        fkyaml::Node root = deserializer.Deserialize("test:\n  bool: true\n  foo: bar\n  pi: 3.14");

        if (!root.IsMapping())
        {
            std::fprintf(
                stderr,
                "Invalid root node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::MAPPING),
                static_cast<int>(root.Type()));
            return -1;
        }

        if (root.Size() != 1)
        {
            std::fprintf(
                stderr,
                "Invalid root node size. expect=%d, actual=%zu",
                1,
                root.Size());
            return -1;
        }

        fkyaml::Node& test_node = root["test"];
        if (!test_node.IsMapping())
        {
            std::fprintf(
                stderr,
                "Invalid test node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::MAPPING),
                static_cast<int>(test_node.Type()));
            return -1;
        }

        if (test_node.Size() != 3)
        {
            std::fprintf(
                stderr,
                "Invalid test node size. expect=%d, actual=%zu",
                3,
                test_node.Size());
            return -1;
        }

        fkyaml::Node& bool_node = test_node["bool"];
        if (!bool_node.IsBoolean())
        {
            std::fprintf(
                stderr,
                "Invalid bool node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::BOOLEAN),
                static_cast<int>(test_node.Type()));
            return -1;
        }

        if (bool_node.ToBoolean() != true)
        {
            std::fprintf(
                stderr,
                "Invalid bool node value. expect=%s, actual=%s\n",
                "true",
                bool_node.ToBoolean() ? "true" : "false");
            return -1;
        }

        fkyaml::Node& foo_node = test_node["foo"];
        if (!foo_node.IsString())
        {
            std::fprintf(
                stderr,
                "Invalid foo node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::STRING),
                static_cast<int>(test_node.Type()));
            return -1;
        }

        if (foo_node.ToString().compare("bar") != 0)
        {
            std::fprintf(
                stderr,
                "Invalid foo node value. expect=%s, actual=%s\n",
                "bar",
                foo_node.ToString().c_str());
            return -1;
        }

        fkyaml::Node& pi_node = test_node["pi"];
        if (!pi_node.IsFloatNumber())
        {
            std::fprintf(
                stderr,
                "Invalid pi node type. expect=%d, actual=%d\n",
                static_cast<int>(fkyaml::NodeType::FLOAT_NUMBER),
                static_cast<int>(pi_node.Type()));
            return -1;
        }

        if (pi_node.ToFloatNumber() != 3.14)
        {
            std::fprintf(
                stderr,
                "Invalid foo node value. expect=%lf, actual=%lf\n",
                3.14,
                pi_node.ToFloatNumber());
            return -1;
        }
    }
    catch (const fkyaml::Exception& e)
    {
        std::fprintf(stderr, "fkYAML internal error: %s\n", e.what());
        return -1;
    }
    catch (const std::exception& e)
    {
        std::fprintf(stderr, "unexpected error: %s\n", e.what());
        return -1;
    }

    return 0;
}