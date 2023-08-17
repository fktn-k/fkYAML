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
    static int ScanSignedFloatNumberTest();
    static int ScanUnsignedFloatNumberTest();
};

#endif /* FK_YAML_TEST_LEXICAL_ANALYZER_TEST_HPP_ */