/**
 * main.cpp - implementation of the entry point of the unit test application.
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#include <cerrno>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>

#include "NodeClassTest.hpp"

static const std::unordered_map<uint32_t, int (*)()> NODE_TEST_CASE_MAP {
    {0x0000, NodeClassTest::DefaultCtorTest},
    {0x0001, NodeClassTest::SequenceNodeFactoryTest},
    {0x0002, NodeClassTest::MappingNodeFactoryTest},
    {0x0003, NodeClassTest::BoolNodeFactoryTest},
    {0x0004, NodeClassTest::SignedIntNodeFactoryTest},
    {0x0005, NodeClassTest::UnsignedIntNodeFactoryTest},
    {0x0006, NodeClassTest::FloatNumberNodeFactoryTest},
    {0x0007, NodeClassTest::StringNodeFactoryTest},
    {0x0008, NodeClassTest::SequenceForLoopTest},
    {0x0009, NodeClassTest::ConstSequenceForLoopTest},
};

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        return -1;
    }

    errno = 0;

    uint32_t test_id = std::strtoul(argv[1], nullptr, 0);
    if ((test_id == 0 || test_id == ULONG_MAX) && errno == ERANGE)
    {
        std::fprintf(stderr, "Failed to retrieve test case id.");
        return -1;
    }

    int ret = 0;
    auto itr = NODE_TEST_CASE_MAP.find(test_id);
    if (itr != NODE_TEST_CASE_MAP.end())
    {
        ret = (*(itr->second))();
    }
    else
    {
        std::fprintf(stderr, "Failed to retrieve test function from test case id.");
        ret = -1;
    }

    return ret;
}