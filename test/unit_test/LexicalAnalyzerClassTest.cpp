/**
 * LexicalAnalyzerTest.cpp - implementation of test functions for the LexicalAnalyzer class
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#include "catch2/catch.hpp"

#include "fkYAML/LexicalAnalyzer.hpp"
#include "fkYAML/Node.hpp"

TEST_CASE("LexicalAnalyzerTest_ScanNullTokenTest", "[LexicalAnalyzerTest]")
{
    auto buffer = GENERATE(std::string("null"), std::string("Null"), std::string("NULL"), std::string("~"));
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(buffer.c_str());

    SECTION("Test nothrow expected buffers.")
    {
        REQUIRE_NOTHROW(lexer.GetNextToken());
    }

    SECTION("Test result of scanning null tokens.")
    {
        REQUIRE(lexer.GetNextToken() == fkyaml::LexicalTokenType::NULL_VALUE);
        REQUIRE_NOTHROW(lexer.GetNull());
        REQUIRE(lexer.GetNull() == nullptr);
    }
}

TEST_CASE("LexicalAnalyzerTest_ScanBooleanTrueTokenTest", "[LexicalAnalyzerTest]")
{
    auto buffer = GENERATE(std::string("true"), std::string("True"), std::string("TRUE"));
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(buffer.c_str());

    SECTION("Test nothrow expected buffers.")
    {
        REQUIRE_NOTHROW(lexer.GetNextToken());
    }

    SECTION("Test result of scanning boolean true literal tokens.")
    {
        REQUIRE(lexer.GetNextToken() == fkyaml::LexicalTokenType::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.GetBoolean());
        REQUIRE(lexer.GetBoolean() == true);
    }
}

TEST_CASE("LexicalAnalyzerTest_ScanBooleanFalseTokenTest", "[LexicalAnalyzerTest]")
{
    auto buffer = GENERATE(std::string("false"), std::string("False"), std::string("FALSE"));
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(buffer.c_str());

    SECTION("Test nothrow expected buffers.")
    {
        REQUIRE_NOTHROW(lexer.GetNextToken());
    }

    SECTION("Test result of scanning boolean false literal tokens.")
    {
        REQUIRE(lexer.GetNextToken() == fkyaml::LexicalTokenType::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.GetBoolean());
        REQUIRE(lexer.GetBoolean() == false);
    }
}

TEST_CASE("LexicalAnalyzerTest_ScanSignedDecimalIntegerTokenTest", "[LexicalAnalyzerTest]")
{
    using ValuePair = std::pair<std::string, fkyaml::NodeSignedIntType>;
    auto value_pair = GENERATE(
        ValuePair(std::string("-1234"), -1234),
        ValuePair(std::string("-853255"), -853255),
        ValuePair(std::string("-1"), -1));
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(value_pair.first.c_str());

    SECTION("Test nothrow expected buffers.")
    {
        REQUIRE_NOTHROW(lexer.GetNextToken());
    }

    SECTION("Test result of scanning signed decimal integer tokens.")
    {
        REQUIRE(lexer.GetNextToken() == fkyaml::LexicalTokenType::SIGNED_INT_VALUE);
        REQUIRE_NOTHROW(lexer.GetSignedInt());
        REQUIRE(lexer.GetSignedInt() == value_pair.second);
    }
}

TEST_CASE("LexicalAnalyzerTest_ScanUnsignedDecimalIntegerTokenTest", "[LexicalAnalyzerTest]")
{
    using ValuePair = std::pair<std::string, fkyaml::NodeUnsignedIntType>;
    auto value_pair = GENERATE(
        ValuePair(std::string("1234"), 1234), ValuePair(std::string("853255"), 853255), ValuePair(std::string("1"), 1));
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(value_pair.first.c_str());

    SECTION("Test nothrow expected buffers.")
    {
        REQUIRE_NOTHROW(lexer.GetNextToken());
    }

    SECTION("Test result of scanning unsigned decimal integer tokens.")
    {
        REQUIRE(lexer.GetNextToken() == fkyaml::LexicalTokenType::UNSIGNED_INT_VALUE);
        REQUIRE_NOTHROW(lexer.GetUnsignedInt());
        REQUIRE(lexer.GetUnsignedInt() == value_pair.second);
    }
}

TEST_CASE("LexicalAnalyzerTest_ScanOctalNumberTokenTest", "[LexicalAnalyzerTest]")
{
    using ValuePair = std::pair<std::string, fkyaml::NodeUnsignedIntType>;
    auto value_pair = GENERATE(
        ValuePair(std::string("0o27"), 027),
        ValuePair(std::string("0o5"), 05),
        ValuePair(std::string("0o77772"), 077772));
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(value_pair.first.c_str());

    SECTION("Test nothrow expected buffers.")
    {
        REQUIRE_NOTHROW(lexer.GetNextToken());
    }

    SECTION("Test result of scanning octal number tokens.")
    {
        REQUIRE(lexer.GetNextToken() == fkyaml::LexicalTokenType::UNSIGNED_INT_VALUE);
        REQUIRE_NOTHROW(lexer.GetUnsignedInt());
        REQUIRE(lexer.GetUnsignedInt() == value_pair.second);
    }
}

TEST_CASE("LexicalAnalyzerTest_ScanHexadecimalNumberTokenTest", "[LexicalAnalyzerTest]")
{
    using ValuePair = std::pair<std::string, fkyaml::NodeUnsignedIntType>;
    auto value_pair = GENERATE(
        ValuePair(std::string("0xA04F"), 0xA04F),
        ValuePair(std::string("0xa7F3"), 0xa7F3),
        ValuePair(std::string("0xFf29Bc"), 0xFf29Bc));
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(value_pair.first.c_str());

    SECTION("Test nothrow expected buffers.")
    {
        REQUIRE_NOTHROW(lexer.GetNextToken());
    }

    SECTION("Test result of scanning hexadecimal number tokens.")
    {
        REQUIRE(lexer.GetNextToken() == fkyaml::LexicalTokenType::UNSIGNED_INT_VALUE);
        REQUIRE_NOTHROW(lexer.GetUnsignedInt());
        REQUIRE(lexer.GetUnsignedInt() == value_pair.second);
    }
}

TEST_CASE("LexicalAnalyzerTest_ScanFloatNumberTokenTest", "[LexicalAnalyzerTest]")
{
    using ValuePair = std::pair<std::string, fkyaml::NodeFloatNumberType>;
    auto value_pair = GENERATE(
        ValuePair(std::string("-1.234"), -1.234),
        ValuePair(std::string("567.8"), 567.8),
        ValuePair(std::string("9.8e-3"), 9.8e-3),
        ValuePair(std::string("1.863e+3"), 1.863e+3));
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(value_pair.first.c_str());

    SECTION("Test nothrow expected buffers.")
    {
        REQUIRE_NOTHROW(lexer.GetNextToken());
    }

    SECTION("Test result of scanning float number tokens.")
    {
        REQUIRE(lexer.GetNextToken() == fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE);
        REQUIRE_NOTHROW(lexer.GetFloatNumber());
        REQUIRE(lexer.GetFloatNumber() == value_pair.second);
    }
}

TEST_CASE("LexicalAnalyzerTest_ScanInfinityTokenTest", "[LexicalAnalyzerTest]")
{
    auto buffer = GENERATE(
        std::string(".inf"),
        std::string(".Inf"),
        std::string(".INF"),
        std::string("-.inf"),
        std::string("-.Inf"),
        std::string("-.INF"));
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(buffer.c_str());

    SECTION("Test nothrow expected buffers.")
    {
        REQUIRE_NOTHROW(lexer.GetNextToken());
    }

    SECTION("Test result of scanning positive infinity literal tokens.")
    {
        REQUIRE(lexer.GetNextToken() == fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE);
        REQUIRE_NOTHROW(lexer.GetFloatNumber());
        REQUIRE(std::isinf(lexer.GetFloatNumber()) == true);
    }
}

TEST_CASE("LexicalAnalyzerTest_ScanNaNTokenTest", "[LexicalAnalyzerTest]")
{
    auto buffer = GENERATE(std::string(".nan"), std::string(".NaN"), std::string(".NAN"));
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(buffer.c_str());

    SECTION("Test nothrow expected buffers.")
    {
        REQUIRE_NOTHROW(lexer.GetNextToken());
    }

    SECTION("Test result of scanning NaN literal tokens.")
    {
        REQUIRE(lexer.GetNextToken() == fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE);
        REQUIRE_NOTHROW(lexer.GetFloatNumber());
        REQUIRE(std::isnan(lexer.GetFloatNumber()) == true);
    }
}

TEST_CASE("LexicalAnalyzerTest_ScanStringTokenTest", "[LexicalAnalyzerTest]")
{
    using ValuePair = std::pair<std::string, fkyaml::NodeStringType>;
    auto value_pair = GENERATE(
        ValuePair(std::string("\"\""), fkyaml::NodeStringType("")),
        ValuePair(std::string("\'\'"), fkyaml::NodeStringType("")),
        ValuePair(std::string("test"), fkyaml::NodeStringType("test")),
        ValuePair(std::string("\"foo:bar\""), fkyaml::NodeStringType("foo:bar")),
        ValuePair(std::string("\"foo\\tbar\""), fkyaml::NodeStringType("foo\tbar")),
        ValuePair(std::string("\"foo\tbar\""), fkyaml::NodeStringType("foo\tbar")),
        ValuePair(std::string("\"foo\\ bar\""), fkyaml::NodeStringType("foo bar")),
        ValuePair(std::string("\"foo\\/bar\""), fkyaml::NodeStringType("foo/bar")));
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(value_pair.first.c_str());

    SECTION("Test nothrow expected buffers.")
    {
        REQUIRE_NOTHROW(lexer.GetNextToken());
    }

    SECTION("Test result of scanning string tokens.")
    {
        REQUIRE(lexer.GetNextToken() == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString() == value_pair.second);
    }
}

TEST_CASE("LexicalAnalyzerTest_ScanKeyBooleanValuePairTokenTest", "[LexicalAnalyzerTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer("test: true");
    fkyaml::LexicalTokenType token;

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.GetString());
    REQUIRE(lexer.GetString().compare("test") == 0);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::BOOLEAN_VALUE);
    REQUIRE_NOTHROW(lexer.GetBoolean());
    REQUIRE(lexer.GetBoolean() == true);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerTest_ScanKeySignedIntegerValuePairTokenTest", "[LexicalAnalyzerTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer("test: -5784");
    fkyaml::LexicalTokenType token;

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.GetString());
    REQUIRE(lexer.GetString().compare("test") == 0);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::SIGNED_INT_VALUE);
    REQUIRE_NOTHROW(lexer.GetSignedInt());
    REQUIRE(lexer.GetSignedInt() == -5784);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerTest_ScanKeyUnsignedIntegerValuePairTokenTest", "[LexicalAnalyzerTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer("test: 47239");
    fkyaml::LexicalTokenType token;

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.GetString());
    REQUIRE(lexer.GetString().compare("test") == 0);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::UNSIGNED_INT_VALUE);
    REQUIRE_NOTHROW(lexer.GetUnsignedInt());
    REQUIRE(lexer.GetUnsignedInt() == 47239);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerTest_ScanKeyFloatNumberValuePairTokenTest", "[LexicalAnalyzerTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer("test: -5.58e-3");
    fkyaml::LexicalTokenType token;

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.GetString());
    REQUIRE(lexer.GetString().compare("test") == 0);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE);
    REQUIRE_NOTHROW(lexer.GetFloatNumber());
    REQUIRE(lexer.GetFloatNumber() == -5.58e-3);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerTest_ScanKeyStringValuePairTokenTest", "[LexicalAnalyzerTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer("test: some value");
    fkyaml::LexicalTokenType token;

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.GetString());
    REQUIRE(lexer.GetString().compare("test") == 0);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.GetString());
    REQUIRE(lexer.GetString().compare("some value") == 0);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerTest_ScanFlowSequenceTokenTest", "[LexicalAnalyzerTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    fkyaml::LexicalTokenType token;

    SECTION("Input source No.1.")
    {
        lexer.SetInputBuffer("test: [ foo, bar ]");

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::SEQUENCE_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::SEQUENCE_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::END_OF_BUFFER);
    }

    SECTION("Input source No.2.")
    {
        lexer.SetInputBuffer("test: [ { foo: one, bar: false }, { foo: two, bar: true } ]");

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::SEQUENCE_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::MAPPING_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("one") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.GetBoolean());
        REQUIRE(lexer.GetBoolean() == false);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::MAPPING_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::MAPPING_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("two") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.GetBoolean());
        REQUIRE(lexer.GetBoolean() == true);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::MAPPING_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::SEQUENCE_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::END_OF_BUFFER);
    }
}

TEST_CASE("LexicalAnalyzerTest_ScanFlowMappingTokenTest", "[LexicalAnalyzerTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    fkyaml::LexicalTokenType token;

    SECTION("Input source No.1.")
    {
        lexer.SetInputBuffer("test: { bool: true, foo: bar, pi: 3.14 }");

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::MAPPING_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("bool") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.GetBoolean());
        REQUIRE(lexer.GetBoolean() == true);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("pi") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE);
        REQUIRE_NOTHROW(lexer.GetFloatNumber());
        REQUIRE(lexer.GetFloatNumber() == 3.14);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::MAPPING_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::END_OF_BUFFER);
    }
}

TEST_CASE("LexicalAnalyzerTest_ScanBlockSequenceTokenTest", "[LexicalAnalyzerTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    fkyaml::LexicalTokenType token;

    SECTION("Input source No.1.")
    {
        lexer.SetInputBuffer("test:\n  - foo\n  - bar");

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::MAPPING_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::END_OF_BUFFER);
    }

    SECTION("Input source No.2.")
    {
        lexer.SetInputBuffer("test:\n  - foo: one\n    bar: false\n  - foo: two\n    bar: true");

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::MAPPING_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("one") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.GetBoolean());
        REQUIRE(lexer.GetBoolean() == false);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("two") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.GetBoolean());
        REQUIRE(lexer.GetBoolean() == true);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::END_OF_BUFFER);
    }
}

TEST_CASE("LexicalAnalyzerTest_ScanBlockMappingTokenTest", "[LexicalAnalyzerTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    fkyaml::LexicalTokenType token;

    SECTION("Input source No.1.")
    {
        lexer.SetInputBuffer("test:\n  bool: true\n  foo: bar\n  pi: 3.14");

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::MAPPING_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("bool") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.GetBoolean());
        REQUIRE(lexer.GetBoolean() == true);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("pi") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE);
        REQUIRE_NOTHROW(lexer.GetFloatNumber());
        REQUIRE(lexer.GetFloatNumber() == 3.14);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::END_OF_BUFFER);
    }
}
