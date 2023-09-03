/**
 * LexicalAnalyzerClassTest.cpp - implementation of test functions for the LexicalAnalyzer class
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#include "catch2/catch.hpp"

#include "fkYAML/LexicalAnalyzer.hpp"
#include "fkYAML/Node.hpp"

TEST_CASE("LexicalAnalyzerClassTest_SetInputBufferTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;

    SECTION("Test non-null non-empty input buffer.")
    {
        REQUIRE_NOTHROW(lexer.SetInputBuffer("test"));
    }

    SECTION("Test non-null empty input buffer.")
    {
        REQUIRE_THROWS_AS(lexer.SetInputBuffer(""), fkyaml::Exception);
    }

    SECTION("Test null input buffer.")
    {
        REQUIRE_THROWS_AS(lexer.SetInputBuffer(nullptr), fkyaml::Exception);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanBeforeInputBufferSetTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    REQUIRE_THROWS_AS(lexer.GetNextToken(), fkyaml::Exception);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanColonTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    fkyaml::LexicalTokenType token;

    SECTION("Test colon with half-width space.")
    {
        lexer.SetInputBuffer(": ");
        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);
    }

    SECTION("Test colon with CR newline code.")
    {
        lexer.SetInputBuffer(":\r");
        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::MAPPING_BLOCK_PREFIX);
    }

    SECTION("Test colon with CRLF newline code.")
    {
        lexer.SetInputBuffer(":\r\n");
        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::MAPPING_BLOCK_PREFIX);
    }

    SECTION("Test colon with LF newline code.")
    {
        lexer.SetInputBuffer(":\n");
        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::MAPPING_BLOCK_PREFIX);
    }

    SECTION("Test colon with non-newline-code character.")
    {
        lexer.SetInputBuffer(":test");
        REQUIRE_THROWS_AS(token = lexer.GetNextToken(), fkyaml::Exception);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanNullTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    fkyaml::LexicalTokenType token;

    SECTION("Test nothrow expected tokens.")
    {
        auto buffer = GENERATE(std::string("null"), std::string("Null"), std::string("NULL"), std::string("~"));
        lexer.SetInputBuffer(buffer.c_str());

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::NULL_VALUE);
        REQUIRE_NOTHROW(lexer.GetNull());
        REQUIRE(lexer.GetNull() == nullptr);
    }

    SECTION("Test nothrow unexpected tokens.")
    {
        lexer.SetInputBuffer("test");
        REQUIRE_NOTHROW(lexer.GetNextToken());
        REQUIRE_THROWS_AS(lexer.GetNull(), fkyaml::Exception);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanBooleanTrueTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    fkyaml::LexicalTokenType token;

    SECTION("Test nothrow expected tokens.")
    {
        auto buffer = GENERATE(std::string("true"), std::string("True"), std::string("TRUE"));
        lexer.SetInputBuffer(buffer.c_str());

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.GetBoolean());
        REQUIRE(lexer.GetBoolean() == true);
    }

    SECTION("Test nothrow unexpected tokens.")
    {
        lexer.SetInputBuffer("test");
        REQUIRE_NOTHROW(lexer.GetNextToken());
        REQUIRE_THROWS_AS(lexer.GetBoolean(), fkyaml::Exception);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanBooleanFalseTokenTest", "[LexicalAnalyzerClassTest]")
{
    auto buffer = GENERATE(std::string("false"), std::string("False"), std::string("FALSE"));
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(buffer.c_str());
    fkyaml::LexicalTokenType token;

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::BOOLEAN_VALUE);
    REQUIRE_NOTHROW(lexer.GetBoolean());
    REQUIRE(lexer.GetBoolean() == false);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanSignedDecimalIntegerTokenTest", "[LexicalAnalyzerClassTest]")
{
    using ValuePair = std::pair<std::string, fkyaml::NodeSignedIntType>;
    auto value_pair = GENERATE(
        ValuePair(std::string("-1234"), -1234),
        ValuePair(std::string("-853255"), -853255),
        ValuePair(std::string("-1"), -1));

    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(value_pair.first.c_str());
    fkyaml::LexicalTokenType token;

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::SIGNED_INT_VALUE);
    REQUIRE_NOTHROW(lexer.GetSignedInt());
    REQUIRE(lexer.GetSignedInt() == value_pair.second);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanUnsignedDecimalIntegerTokenTest", "[LexicalAnalyzerClassTest]")
{
    using ValuePair = std::pair<std::string, fkyaml::NodeUnsignedIntType>;
    auto value_pair = GENERATE(
        ValuePair(std::string("1234"), 1234),
        ValuePair(std::string("853255"), 853255),
        ValuePair(std::string("1"), 1),
        ValuePair(std::string("0"), 0));

    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(value_pair.first.c_str());
    fkyaml::LexicalTokenType token;

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::UNSIGNED_INT_VALUE);
    REQUIRE_NOTHROW(lexer.GetUnsignedInt());
    REQUIRE(lexer.GetUnsignedInt() == value_pair.second);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanOctalNumberTokenTest", "[LexicalAnalyzerClassTest]")
{
    using ValuePair = std::pair<std::string, fkyaml::NodeUnsignedIntType>;
    auto value_pair = GENERATE(
        ValuePair(std::string("0o27"), 027),
        ValuePair(std::string("0o5"), 05),
        ValuePair(std::string("0o77772"), 077772));

    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(value_pair.first.c_str());
    fkyaml::LexicalTokenType token;

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::UNSIGNED_INT_VALUE);
    REQUIRE_NOTHROW(lexer.GetUnsignedInt());
    REQUIRE(lexer.GetUnsignedInt() == value_pair.second);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanHexadecimalNumberTokenTest", "[LexicalAnalyzerClassTest]")
{
    using ValuePair = std::pair<std::string, fkyaml::NodeUnsignedIntType>;
    auto value_pair = GENERATE(
        ValuePair(std::string("0xA04F"), 0xA04F),
        ValuePair(std::string("0xa7F3"), 0xa7F3),
        ValuePair(std::string("0xFf29Bc"), 0xFf29Bc));

    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(value_pair.first.c_str());
    fkyaml::LexicalTokenType token;

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::UNSIGNED_INT_VALUE);
    REQUIRE_NOTHROW(lexer.GetUnsignedInt());
    REQUIRE(lexer.GetUnsignedInt() == value_pair.second);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanFloatNumberTokenTest", "[LexicalAnalyzerClassTest]")
{
    using ValuePair = std::pair<std::string, fkyaml::NodeFloatNumberType>;
    auto value_pair = GENERATE(
        ValuePair(std::string("-1.234"), -1.234),
        ValuePair(std::string("567.8"), 567.8),
        ValuePair(std::string("0.24"), 0.24),
        ValuePair(std::string("9.8e-3"), 9.8e-3),
        ValuePair(std::string("3.95e3"), 3.95e3),
        ValuePair(std::string("1.863e+3"), 1.863e+3));

    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(value_pair.first.c_str());
    fkyaml::LexicalTokenType token;

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE);
    REQUIRE_NOTHROW(lexer.GetFloatNumber());
    REQUIRE(lexer.GetFloatNumber() == value_pair.second);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanInfinityTokenTest", "[LexicalAnalyzerClassTest]")
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

    SECTION("Test result of positive infinity literal tokens.")
    {
        REQUIRE(lexer.GetNextToken() == fkyaml::LexicalTokenType::FLOAT_NUMBER_VALUE);
        REQUIRE_NOTHROW(lexer.GetFloatNumber());
        REQUIRE(std::isinf(lexer.GetFloatNumber()) == true);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanNaNTokenTest", "[LexicalAnalyzerClassTest]")
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

TEST_CASE("LexicalAnalyzerClassTest_ScanStringTokenTest", "[LexicalAnalyzerClassTest]")
{
    using ValuePair = std::pair<std::string, fkyaml::NodeStringType>;
    auto value_pair = GENERATE(
        ValuePair(std::string("\"\""), fkyaml::NodeStringType("")),
        ValuePair(std::string("\'\'"), fkyaml::NodeStringType("")),
        ValuePair(std::string("test"), fkyaml::NodeStringType("test")),
        ValuePair(std::string("nop"), fkyaml::NodeStringType("nop")),
        ValuePair(std::string(".NET"), fkyaml::NodeStringType(".NET")),
        ValuePair(std::string("\"foo:bar\""), fkyaml::NodeStringType("foo:bar")),
        ValuePair(std::string("\"foo,bar\""), fkyaml::NodeStringType("foo,bar")),
        ValuePair(std::string("\"foo]bar\""), fkyaml::NodeStringType("foo]bar")),
        ValuePair(std::string("\"foo}bar\""), fkyaml::NodeStringType("foo}bar")),
        ValuePair(std::string("\"foo\\tbar\""), fkyaml::NodeStringType("foo\tbar")),
        ValuePair(std::string("\"foo\tbar\""), fkyaml::NodeStringType("foo\tbar")),
        ValuePair(std::string("\"foo\\nbar\""), fkyaml::NodeStringType("foo\nbar")),
        ValuePair(std::string("\"foo\\ bar\""), fkyaml::NodeStringType("foo bar")),
        ValuePair(std::string("\"foo\\\"bar\""), fkyaml::NodeStringType("foo\"bar")),
        ValuePair(std::string("\"foo\\/bar\""), fkyaml::NodeStringType("foo/bar")),
        ValuePair(std::string("\"foo\\\\bar\""), fkyaml::NodeStringType("foo\\bar")),
        ValuePair(std::string("\'foo\'\'bar\'"), fkyaml::NodeStringType("foo\'bar")),
        ValuePair(std::string("\'foo,bar\'"), fkyaml::NodeStringType("foo,bar")),
        ValuePair(std::string("\'foo]bar\'"), fkyaml::NodeStringType("foo]bar")),
        ValuePair(std::string("\'foo}bar\'"), fkyaml::NodeStringType("foo}bar")));

    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(value_pair.first.c_str());
    fkyaml::LexicalTokenType token;

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.GetString());
    REQUIRE(lexer.GetString() == value_pair.second);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanAnchorTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    fkyaml::LexicalTokenType token;

    SECTION("Test nothorw expected tokens with an anchor.")
    {
        lexer.SetInputBuffer("test: &anchor foo");

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::ANCHOR_PREFIX);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("anchor") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::END_OF_BUFFER);
    }

    SECTION("Test nothrow unexpected tokens with an anchor.")
    {
        auto buffer =
            GENERATE(std::string("test: &anchor"), std::string("test: &anchor\r\n"), std::string("test: &anchor\n"));
        lexer.SetInputBuffer(buffer.c_str());

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_THROWS_AS(token = lexer.GetNextToken(), fkyaml::Exception);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanAliasTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    fkyaml::LexicalTokenType token;

    SECTION("Test nothrow expected tokens with an alias.")
    {
        lexer.SetInputBuffer("test: *anchor");

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::ALIAS_PREFIX);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("anchor") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::END_OF_BUFFER);
    }

    SECTION("Test nothrow unexpected tokens with an anchor.")
    {
        auto buffer = GENERATE(std::string("test: *"), std::string("test: *\r\n"), std::string("test: *\n"), std::string("test: * "));
        lexer.SetInputBuffer(buffer.c_str());

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.GetString());
        REQUIRE(lexer.GetString().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.GetNextToken());
        REQUIRE(token == fkyaml::LexicalTokenType::KEY_SEPARATOR);

        REQUIRE_THROWS_AS(token = lexer.GetNextToken(), fkyaml::Exception);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanCommentTokenTest", "[LexicalAnalyzerClassTest]")
{
    auto buffer = GENERATE(std::string("# comment\r\n"), std::string("# comment\n"), std::string("# comment"));
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(buffer.c_str());
    fkyaml::LexicalTokenType token;

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::COMMENT_PREFIX);

    REQUIRE_NOTHROW(token = lexer.GetNextToken());
    REQUIRE(token == fkyaml::LexicalTokenType::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanReservedIndicatorTokenTest", "[LexicalAnalyzerClassTest]")
{
    auto buffer = GENERATE(std::string("@invalid"), std::string("`invalid"));
    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(buffer.c_str());
    REQUIRE_THROWS_AS(lexer.GetNextToken(), fkyaml::Exception);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanKeyBooleanValuePairTokenTest", "[LexicalAnalyzerClassTest]")
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

TEST_CASE("LexicalAnalyzerClassTest_ScanKeySignedIntegerValuePairTokenTest", "[LexicalAnalyzerClassTest]")
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

TEST_CASE("LexicalAnalyzerClassTest_ScanKeyUnsignedIntegerValuePairTokenTest", "[LexicalAnalyzerClassTest]")
{
    auto buffer = GENERATE(std::string("test: 47239"), std::string("test: +47239"));

    fkyaml::LexicalAnalyzer<fkyaml::Node> lexer;
    lexer.SetInputBuffer(buffer.c_str());
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

TEST_CASE("LexicalAnalyzerClassTest_ScanKeyFloatNumberValuePairTokenTest", "[LexicalAnalyzerClassTest]")
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

TEST_CASE("LexicalAnalyzerClassTest_ScanKeyStringValuePairTokenTest", "[LexicalAnalyzerClassTest]")
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

TEST_CASE("LexicalAnalyzerClassTest_ScanFlowSequenceTokenTest", "[LexicalAnalyzerClassTest]")
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

TEST_CASE("LexicalAnalyzerClassTest_ScanFlowMappingTokenTest", "[LexicalAnalyzerClassTest]")
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

TEST_CASE("LexicalAnalyzerClassTest_ScanBlockSequenceTokenTest", "[LexicalAnalyzerClassTest]")
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

TEST_CASE("LexicalAnalyzerClassTest_ScanBlockMappingTokenTest", "[LexicalAnalyzerClassTest]")
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
