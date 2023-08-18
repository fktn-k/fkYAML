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
};

#endif /* FK_YAML_TEST_LEXICAL_ANALYZER_TEST_HPP_ */