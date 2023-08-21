/**
 * LexicalAnalyzerTest.hpp - declaration of test functions for the LexicalAnalyzer class
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_TEST_LEXICAL_ANALYZER_TEST_HPP_
#define FK_YAML_TEST_LEXICAL_ANALYZER_TEST_HPP_

class LexicalAnalyzerTest
{
public:
    static int ScanNullTest1();
    static int ScanNullTest2();
    static int ScanNullTest3();
    static int ScanNullTest4();
    static int ScanTrueBooleanTest1();
    static int ScanTrueBooleanTest2();
    static int ScanTrueBooleanTest3();
    static int ScanFalseBooleanTest1();
    static int ScanFalseBooleanTest2();
    static int ScanFalseBooleanTest3();
    static int ScanSignedDecimalIntegerTest();
    static int ScanUnsignedDecimalIntegerTest();
    static int ScanOctalIntegerTest();
    static int ScanHexadecimalIntegerTest();
    static int ScanSignedFloatNumberTest();
    static int ScanUnsignedFloatNumberTest();
    static int ScanFloatNumberWithExponentTest();
    static int ScanPositiveInfinityTest1();
    static int ScanPositiveInfinityTest2();
    static int ScanPositiveInfinityTest3();
    static int ScanNegativeInfinityTest1();
    static int ScanNegativeInfinityTest2();
    static int ScanNegativeInfinityTest3();
    static int ScanNaNTest1();
    static int ScanNaNTest2();
    static int ScanNaNTest3();
    static int ScanDoubleQuotedEmptyStringTest();
    static int ScanSingleQuotedEmptyStringTest();
    static int ScanKeyBooleanValuePairTest();
    static int ScanKeySignedIntegerValuePairTest();
    static int ScanKeyUnsignedIntegerValuePairTest();
    static int ScanKeyFloatNumberValuePairTest();
    static int ScanKeyStringValuePairTest();
    static int ScanFlowSequenceObject1();
    static int ScanFlowSequenceObject2();
    static int ScanFlowMappingObject1();
};

#endif /* FK_YAML_TEST_LEXICAL_ANALYZER_TEST_HPP_ */