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

    {0x0100, LexicalAnalyzerTest::ScanNullTest1},
    {0x0101, LexicalAnalyzerTest::ScanNullTest2},
    {0x0102, LexicalAnalyzerTest::ScanNullTest3},
    {0x0103, LexicalAnalyzerTest::ScanNullTest4},
    {0x0104, LexicalAnalyzerTest::ScanTrueBooleanTest1},
    {0x0105, LexicalAnalyzerTest::ScanTrueBooleanTest2},
    {0x0106, LexicalAnalyzerTest::ScanTrueBooleanTest3},
    {0x0107, LexicalAnalyzerTest::ScanFalseBooleanTest1},
    {0x0108, LexicalAnalyzerTest::ScanFalseBooleanTest2},
    {0x0109, LexicalAnalyzerTest::ScanFalseBooleanTest3},
    {0x010A, LexicalAnalyzerTest::ScanSignedDecimalIntegerTest},
    {0x010B, LexicalAnalyzerTest::ScanUnsignedDecimalIntegerTest},
    {0x010C, LexicalAnalyzerTest::ScanOctalIntegerTest},
    {0x010D, LexicalAnalyzerTest::ScanHexadecimalIntegerTest},
    {0x010E, LexicalAnalyzerTest::ScanSignedFloatNumberTest},
    {0x010F, LexicalAnalyzerTest::ScanUnsignedFloatNumberTest},
    {0x0110, LexicalAnalyzerTest::ScanFloatNumberWithExponentTest},
    {0x0111, LexicalAnalyzerTest::ScanPositiveInfinityTest1},
    {0x0112, LexicalAnalyzerTest::ScanPositiveInfinityTest2},
    {0x0113, LexicalAnalyzerTest::ScanPositiveInfinityTest3},
    {0x0114, LexicalAnalyzerTest::ScanNegativeInfinityTest1},
    {0x0115, LexicalAnalyzerTest::ScanNegativeInfinityTest2},
    {0x0116, LexicalAnalyzerTest::ScanNegativeInfinityTest3},
    {0x0117, LexicalAnalyzerTest::ScanNaNTest1},
    {0x0118, LexicalAnalyzerTest::ScanNaNTest2},
    {0x0119, LexicalAnalyzerTest::ScanNaNTest3},
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