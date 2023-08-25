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

#include "DeserializerTest.hpp"
#include "LexicalAnalyzerTest.hpp"
#include "NodeClassTest.hpp"

static const std::unordered_map<unsigned long, int (*)()> TEST_CASE_MAP {
    {0x0000, NodeClassTest::DefaultCtorTest},
    {0x0001, NodeClassTest::SequenceTypeCtorTest},
    {0x0002, NodeClassTest::MappingTypeCtorTest},
    {0x0003, NodeClassTest::NullTypeCtorTest},
    {0x0004, NodeClassTest::BooleanTypeCtorTest},
    {0x0005, NodeClassTest::SignedIntTypeCtorTest},
    {0x0006, NodeClassTest::UnsignedIntTypeCtorTest},
    {0x0007, NodeClassTest::FloatNumberTypeCtorTest},
    {0x0008, NodeClassTest::StringTypeCtorTest},
    {0x0009, NodeClassTest::EmptySequenceNodeFactoryTest},
    {0x000A, NodeClassTest::LvalueSequenceNodeFactoryTest},
    {0x000B, NodeClassTest::RvalueSequenceNodeFactoryTest},
    {0x000C, NodeClassTest::EmptyMappingNodeFactoryTest},
    {0x000D, NodeClassTest::LvalueMappingNodeFactoryTest},
    {0x000E, NodeClassTest::RvalueMappingNodeFactoryTest},
    {0x000F, NodeClassTest::BoolNodeFactoryTest},
    {0x0010, NodeClassTest::SignedIntNodeFactoryTest},
    {0x0011, NodeClassTest::UnsignedIntNodeFactoryTest},
    {0x0012, NodeClassTest::FloatNumberNodeFactoryTest},
    {0x0013, NodeClassTest::EmptyStringNodeFactoryTest},
    {0x0014, NodeClassTest::LvalueStringNodeFactoryTest},
    {0x0015, NodeClassTest::RvalueStringNodeFactoryTest},
    {0x0016, NodeClassTest::LvalueStringSubscriptOperatorTest},
    {0x0017, NodeClassTest::ConstLvalueStringSubscriptOperatorTest},
    {0x0018, NodeClassTest::RvalueStringSubscriptOperatorTest},
    {0x0019, NodeClassTest::ConstRvalueStringSubscriptOperatorTest},
    {0x001A, NodeClassTest::IntegerSubscriptOperatorTest},
    {0x001B, NodeClassTest::ConstIntegerSubscriptOperatorTest},
    {0x001C, NodeClassTest::SequenceTypeTest},
    {0x001D, NodeClassTest::MappingTypeTest},
    {0x001E, NodeClassTest::NullObjectTypeTest},
    {0x001F, NodeClassTest::BooleanTypeTest},
    {0x0020, NodeClassTest::SignedIntegerTypeTest},
    {0x0021, NodeClassTest::UnsignedIntegerTypeTest},
    {0x0022, NodeClassTest::FloatNumberTypeTest},
    {0x0023, NodeClassTest::StringTypeTest},
    {0x0024, NodeClassTest::IsSequenceTest},
    {0x0025, NodeClassTest::IsMappingTest},
    {0x0026, NodeClassTest::IsNullTest},
    {0x0027, NodeClassTest::IsBooleanTest},
    {0x0028, NodeClassTest::IsSignedIntegerTest},
    {0x0029, NodeClassTest::IsUnsignedIntegerTest},
    {0x002A, NodeClassTest::IsFloatNumberTest},
    {0x002B, NodeClassTest::IsStringTest},
    {0x002C, NodeClassTest::IsScalarTest},
    {0x002D, NodeClassTest::SequenceIsEmptyTest},
    {0x002E, NodeClassTest::MappingIsEmptyTest},
    {0x002F, NodeClassTest::StringIsEmptyTest},
    {0x0030, NodeClassTest::IsEmptyThrowExceptionTest},
    {0x0031, NodeClassTest::SequenceSizeTest},
    {0x0032, NodeClassTest::MappingSizeTest},
    {0x0033, NodeClassTest::StringSizeTest},
    {0x0034, NodeClassTest::SizeThrowExceptionTest},
    {0x0035, NodeClassTest::ToSequenceTest},
    {0x0036, NodeClassTest::ConstToSequenceTest},
    {0x0037, NodeClassTest::ToSequenceThrowExceptionTest},
    {0x0038, NodeClassTest::ToMappingTest},
    {0x0039, NodeClassTest::ConstToMappingTest},
    {0x003A, NodeClassTest::ToMappingThrowExceptionTest},
    {0x003B, NodeClassTest::ToBooleanTest},
    {0x003C, NodeClassTest::ConstToBooleanTest},
    {0x003D, NodeClassTest::ToBooleanThrowExceptionTest},
    {0x003E, NodeClassTest::ToSignedIntegerTest},
    {0x003F, NodeClassTest::ConstToSignedIntegerTest},
    {0x0040, NodeClassTest::ToSignedIntegerThrowExceptionTest},
    {0x0041, NodeClassTest::ToUnsignedIntegerTest},
    {0x0042, NodeClassTest::ConstToUnsignedIntegerTest},
    {0x0043, NodeClassTest::ToUnsignedIntegerThrowExceptionTest},
    {0x0044, NodeClassTest::ToFloatNumberTest},
    {0x0045, NodeClassTest::ConstToFloatNumberTest},
    {0x0046, NodeClassTest::ToFloatNumberThrowExceptionTest},
    {0x0047, NodeClassTest::ToStringTest},
    {0x0048, NodeClassTest::ConstToStringTest},
    {0x0049, NodeClassTest::ToStringThrowExceptionTest},
    {0x004A, NodeClassTest::SwapTest},
    {0x004B, NodeClassTest::StdSwapTest},
    {0x004C, NodeClassTest::SequenceForLoopTest},
    {0x004D, NodeClassTest::ConstSequenceForLoopTest},

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
    {0x011A, LexicalAnalyzerTest::ScanDoubleQuotedEmptyStringTest},
    {0x011B, LexicalAnalyzerTest::ScanSingleQuotedEmptyStringTest},
    {0x011C, LexicalAnalyzerTest::ScanKeyBooleanValuePairTest},
    {0x011D, LexicalAnalyzerTest::ScanKeySignedIntegerValuePairTest},
    {0x011E, LexicalAnalyzerTest::ScanKeyUnsignedIntegerValuePairTest},
    {0x011F, LexicalAnalyzerTest::ScanKeyFloatNumberValuePairTest},
    {0x0120, LexicalAnalyzerTest::ScanKeyStringValuePairTest},
    {0x0121, LexicalAnalyzerTest::ScanFlowSequenceObject1},
    {0x0122, LexicalAnalyzerTest::ScanFlowSequenceObject2},
    {0x0123, LexicalAnalyzerTest::ScanFlowMappingObject1},
    {0x0124, LexicalAnalyzerTest::ScanBlockSequenceObject1},
    {0x0125, LexicalAnalyzerTest::ScanBlockSequenceObject2},
    {0x0126, LexicalAnalyzerTest::ScanBlockMappingObject1},

    {0x0200, DeserializerTest::DeserializeMappingObject1},
    {0x0201, DeserializerTest::DeserializeNestedFlowSequenceObject1},
    {0x0202, DeserializerTest::DeserializeNestedFlowMappingObject1},
    {0x0203, DeserializerTest::DeserializeNestedBlockSequenceObject1},
    {0x0204, DeserializerTest::DeserializeNestedBlockSequenceObject2},
    {0x0205, DeserializerTest::DeserializeNestedBlockMappingObject1},
};

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        return -1;
    }

    errno = 0;

    unsigned long test_id = std::strtoul(argv[1], nullptr, 0);
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