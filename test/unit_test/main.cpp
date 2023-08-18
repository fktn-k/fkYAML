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

#include "LexicalAnalyzerTest.hpp"
#include "NodeClassTest.hpp"

static const std::unordered_map<uint32_t, int (*)()> TEST_CASE_MAP {
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

    {0x0100, LexicalAnalyzerTest::ScanSignedDecimalIntegerTest},
    {0x0101, LexicalAnalyzerTest::ScanUnsignedDecimalIntegerTest},
    {0x0102, LexicalAnalyzerTest::ScanOctalIntegerTest},
    {0x0103, LexicalAnalyzerTest::ScanHexadecimalIntegerTest},
    {0x0104, LexicalAnalyzerTest::ScanSignedFloatNumberTest},
    {0x0105, LexicalAnalyzerTest::ScanUnsignedFloatNumberTest},
    {0x0106, LexicalAnalyzerTest::ScanFloatNumberWithExponentTest},
    {0x0107, LexicalAnalyzerTest::ScanPositiveInfinityTest1},
    {0x0108, LexicalAnalyzerTest::ScanPositiveInfinityTest2},
    {0x0109, LexicalAnalyzerTest::ScanPositiveInfinityTest3},
    {0x010A, LexicalAnalyzerTest::ScanNegativeInfinityTest1},
    {0x010B, LexicalAnalyzerTest::ScanNegativeInfinityTest2},
    {0x010C, LexicalAnalyzerTest::ScanNegativeInfinityTest3},
    {0x010D, LexicalAnalyzerTest::ScanNaNTest1},
    {0x010E, LexicalAnalyzerTest::ScanNaNTest2},
    {0x010F, LexicalAnalyzerTest::ScanNaNTest3},
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
    auto itr = TEST_CASE_MAP.find(test_id);
    if (itr != TEST_CASE_MAP.end())
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