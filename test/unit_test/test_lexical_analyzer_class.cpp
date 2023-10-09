//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.1
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include "catch2/catch.hpp"

#include "fkYAML/detail/lexical_analyzer.hpp"
#include "fkYAML/node.hpp"

TEST_CASE("LexicalAnalyzerClassTest_set_input_bufferTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;

    SECTION("Test non-null non-empty input buffer.")
    {
        REQUIRE_NOTHROW(lexer.set_input_buffer("test"));
    }

    SECTION("Test non-null empty input buffer.")
    {
        REQUIRE_THROWS_AS(lexer.set_input_buffer(""), fkyaml::exception);
    }

    SECTION("Test null input buffer.")
    {
        REQUIRE_THROWS_AS(lexer.set_input_buffer(nullptr), fkyaml::exception);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanYamlVersionDirectiveTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    fkyaml::detail::lexical_token_t token;

    SECTION("Test nothrow expected tokens.")
    {
        using value_pair_t = std::pair<std::string, std::string>;
        auto value_pair = GENERATE(
            value_pair_t(std::string("%YAML 1.1\r"), std::string("1.1")),
            value_pair_t(std::string("%YAML 1.2\n"), std::string("1.2")),
            value_pair_t(std::string("%YAML 1.2 "), std::string("1.2")));

        lexer.set_input_buffer(value_pair.first.c_str());

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::YAML_VER_DIRECTIVE);
        REQUIRE(lexer.get_yaml_version() == value_pair.second);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("Test nothrow expected tokens with invalid content.")
    {
        auto buffer = GENERATE(
            std::string("%YUML 1.2"),
            std::string("%YANL 1.2  \r"),
            std::string("%YANR 1.2 \r\n"),
            std::string("%YANL 1.2    \n"));

        lexer.set_input_buffer(buffer.c_str());
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::INVALID_DIRECTIVE);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("Test nothrow unexpected tokens.")
    {
        auto buffer = GENERATE(
            std::string("%YAML 1.1"),
            std::string("%YAML 1.2"),
            std::string("%YAML 1.3\n"),
            std::string("%YAML 2.0\n"),
            std::string("%YAML 12"),
            std::string("%YAML 1.A"),
            std::string("%YAML1.2 "),
            std::string("%YAML AbC"));

        lexer.set_input_buffer(buffer.c_str());
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::exception);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanReservedDirectiveTest", "[LexicalAnalyzerClassTest]")
{
    auto buffer =
        GENERATE(std::string("%TEST"), std::string("%1984\n"), std::string("%TEST4LIB\r"), std::string("%%ERROR\r\n"));

    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    fkyaml::detail::lexical_token_t token;
    lexer.set_input_buffer(buffer.c_str());
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::INVALID_DIRECTIVE);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanBeforeInputBufferSetTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::exception);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanColonTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    fkyaml::detail::lexical_token_t token;

    SECTION("Test colon with half-width space.")
    {
        lexer.set_input_buffer(": ");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("Test colon with CR newline code.")
    {
        lexer.set_input_buffer(":\r");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::MAPPING_BLOCK_PREFIX);
    }

    SECTION("Test colon with CRLF newline code.")
    {
        lexer.set_input_buffer(":\r\n");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::MAPPING_BLOCK_PREFIX);
    }

    SECTION("Test colon with LF newline code.")
    {
        lexer.set_input_buffer(":\n");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::MAPPING_BLOCK_PREFIX);
    }

    SECTION("Test colon with non-newline-code character.")
    {
        lexer.set_input_buffer(":test");
        REQUIRE_THROWS_AS(token = lexer.get_next_token(), fkyaml::exception);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanNullTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    fkyaml::detail::lexical_token_t token;

    SECTION("Test nothrow expected tokens.")
    {
        auto buffer = GENERATE(std::string("null"), std::string("Null"), std::string("NULL"), std::string("~"));
        lexer.set_input_buffer(buffer.c_str());

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::NULL_VALUE);
        REQUIRE_NOTHROW(lexer.get_null());
        REQUIRE(lexer.get_null() == nullptr);
    }

    SECTION("Test nothrow unexpected tokens.")
    {
        lexer.set_input_buffer("test");
        REQUIRE_NOTHROW(lexer.get_next_token());
        REQUIRE_THROWS_AS(lexer.get_null(), fkyaml::exception);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanBooleanTrueTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    fkyaml::detail::lexical_token_t token;

    SECTION("Test nothrow expected tokens.")
    {
        auto buffer = GENERATE(std::string("true"), std::string("True"), std::string("TRUE"));
        lexer.set_input_buffer(buffer.c_str());

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.get_boolean());
        REQUIRE(lexer.get_boolean() == true);
    }

    SECTION("Test nothrow unexpected tokens.")
    {
        lexer.set_input_buffer("test");
        REQUIRE_NOTHROW(lexer.get_next_token());
        REQUIRE_THROWS_AS(lexer.get_boolean(), fkyaml::exception);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanBooleanFalseTokenTest", "[LexicalAnalyzerClassTest]")
{
    auto buffer = GENERATE(std::string("false"), std::string("False"), std::string("FALSE"));
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer(buffer.c_str());
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::BOOLEAN_VALUE);
    REQUIRE_NOTHROW(lexer.get_boolean());
    REQUIRE(lexer.get_boolean() == false);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanIntegerTokenTest", "[LexicalAnalyzerClassTest]")
{
    using value_pair_t = std::pair<std::string, fkyaml::node_integer_type>;
    auto value_pair = GENERATE(
        value_pair_t(std::string("-1234"), -1234),
        value_pair_t(std::string("-853255"), -853255),
        value_pair_t(std::string("-1"), -1));

    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer(value_pair.first.c_str());
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::INTEGER_VALUE);
    REQUIRE_NOTHROW(lexer.get_integer());
    REQUIRE(lexer.get_integer() == value_pair.second);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanOctalNumberTokenTest", "[LexicalAnalyzerClassTest]")
{
    using value_pair_t = std::pair<std::string, fkyaml::node_integer_type>;
    auto value_pair = GENERATE(
        value_pair_t(std::string("0o27"), 027),
        value_pair_t(std::string("0o5"), 05),
        value_pair_t(std::string("0o77772"), 077772));

    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer(value_pair.first.c_str());
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::INTEGER_VALUE);
    REQUIRE_NOTHROW(lexer.get_integer());
    REQUIRE(lexer.get_integer() == value_pair.second);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanHexadecimalNumberTokenTest", "[LexicalAnalyzerClassTest]")
{
    using value_pair_t = std::pair<std::string, fkyaml::node_integer_type>;
    auto value_pair = GENERATE(
        value_pair_t(std::string("0xA04F"), 0xA04F),
        value_pair_t(std::string("0xa7F3"), 0xa7F3),
        value_pair_t(std::string("0xFf29Bc"), 0xFf29Bc));

    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer(value_pair.first.c_str());
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::INTEGER_VALUE);
    REQUIRE_NOTHROW(lexer.get_integer());
    REQUIRE(lexer.get_integer() == value_pair.second);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanFloatNumberTokenTest", "[LexicalAnalyzerClassTest]")
{
    using value_pair_t = std::pair<std::string, fkyaml::node_float_number_type>;
    auto value_pair = GENERATE(
        value_pair_t(std::string("-1.234"), -1.234),
        value_pair_t(std::string("567.8"), 567.8),
        value_pair_t(std::string("0.24"), 0.24),
        value_pair_t(std::string("9.8e-3"), 9.8e-3),
        value_pair_t(std::string("3.95e3"), 3.95e3),
        value_pair_t(std::string("1.863e+3"), 1.863e+3));

    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer(value_pair.first.c_str());
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::FLOAT_NUMBER_VALUE);
    REQUIRE_NOTHROW(lexer.get_float_number());
    REQUIRE(lexer.get_float_number() == value_pair.second);
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
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer(buffer.c_str());

    SECTION("Test nothrow expected buffers.")
    {
        REQUIRE_NOTHROW(lexer.get_next_token());
    }

    SECTION("Test result of positive infinity literal tokens.")
    {
        REQUIRE(lexer.get_next_token() == fkyaml::detail::lexical_token_t::FLOAT_NUMBER_VALUE);
        REQUIRE_NOTHROW(lexer.get_float_number());
        REQUIRE(std::isinf(lexer.get_float_number()) == true);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanNaNTokenTest", "[LexicalAnalyzerClassTest]")
{
    auto buffer = GENERATE(std::string(".nan"), std::string(".NaN"), std::string(".NAN"));
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer(buffer.c_str());

    SECTION("Test nothrow expected buffers.")
    {
        REQUIRE_NOTHROW(lexer.get_next_token());
    }

    SECTION("Test result of scanning NaN literal tokens.")
    {
        REQUIRE(lexer.get_next_token() == fkyaml::detail::lexical_token_t::FLOAT_NUMBER_VALUE);
        REQUIRE_NOTHROW(lexer.get_float_number());
        REQUIRE(std::isnan(lexer.get_float_number()) == true);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanInvalidNumberTokenTest", "[LexicalAnalyzerClassTest]")
{
    auto buffer = GENERATE(std::string("-.test"), std::string("1.0.0"));
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer(buffer.c_str());
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::exception);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanStringTokenTest", "[LexicalAnalyzerClassTest]")
{
    using value_pair_t = std::pair<std::string, fkyaml::node_string_type>;
    auto value_pair = GENERATE(
        value_pair_t(std::string("\"\""), fkyaml::node_string_type("")),
        value_pair_t(std::string("\'\'"), fkyaml::node_string_type("")),
        value_pair_t(std::string("test"), fkyaml::node_string_type("test")),
        value_pair_t(std::string("nop"), fkyaml::node_string_type("nop")),
        value_pair_t(std::string(".NET"), fkyaml::node_string_type(".NET")),
        value_pair_t(std::string("foo:bar"), fkyaml::node_string_type("foo:bar")),
        value_pair_t(std::string("\"foo bar\""), fkyaml::node_string_type("foo bar")),
        value_pair_t(std::string("\"foo:bar\""), fkyaml::node_string_type("foo:bar")),
        value_pair_t(std::string("\"foo,bar\""), fkyaml::node_string_type("foo,bar")),
        value_pair_t(std::string("\"foo]bar\""), fkyaml::node_string_type("foo]bar")),
        value_pair_t(std::string("\"foo}bar\""), fkyaml::node_string_type("foo}bar")),
        value_pair_t(std::string("\"foo\\abar\""), fkyaml::node_string_type("foo\abar")),
        value_pair_t(std::string("\"foo\\bbar\""), fkyaml::node_string_type("foo\bbar")),
        value_pair_t(std::string("\"foo\\tbar\""), fkyaml::node_string_type("foo\tbar")),
        value_pair_t(std::string("\"foo\tbar\""), fkyaml::node_string_type("foo\tbar")),
        value_pair_t(std::string("\"foo\\nbar\""), fkyaml::node_string_type("foo\nbar")),
        value_pair_t(std::string("\"foo\\vbar\""), fkyaml::node_string_type("foo\vbar")),
        value_pair_t(std::string("\"foo\\fbar\""), fkyaml::node_string_type("foo\fbar")),
        value_pair_t(std::string("\"foo\\rbar\""), fkyaml::node_string_type("foo\rbar")),
        value_pair_t(std::string("\"foo\\ebar\""), fkyaml::node_string_type("foo\u001Bbar")),
        value_pair_t(std::string("\"foo\\ bar\""), fkyaml::node_string_type("foo bar")),
        value_pair_t(std::string("\"foo\\\"bar\""), fkyaml::node_string_type("foo\"bar")),
        value_pair_t(std::string("\"foo\\/bar\""), fkyaml::node_string_type("foo/bar")),
        value_pair_t(std::string("\"foo\\\\bar\""), fkyaml::node_string_type("foo\\bar")),
        value_pair_t(std::string("\"\\x30\\x2B\\x6d\""), fkyaml::node_string_type("0+m")),
        value_pair_t(std::string("\'foo bar\'"), fkyaml::node_string_type("foo bar")),
        value_pair_t(std::string("\'foo\'\'bar\'"), fkyaml::node_string_type("foo\'bar")),
        value_pair_t(std::string("\'foo,bar\'"), fkyaml::node_string_type("foo,bar")),
        value_pair_t(std::string("\'foo]bar\'"), fkyaml::node_string_type("foo]bar")),
        value_pair_t(std::string("\'foo}bar\'"), fkyaml::node_string_type("foo}bar")),
        value_pair_t(std::string("\'foo\"bar\'"), fkyaml::node_string_type("foo\"bar")),
        value_pair_t(std::string("\'foo:bar\'"), fkyaml::node_string_type("foo:bar")));

    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer(value_pair.first.c_str());
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.get_string());
    REQUIRE(lexer.get_string() == value_pair.second);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanInvalidStringTokenTest", "[LexicalAnalyzerClassTest]")
{
    auto buffer = GENERATE(
        std::string("foo\"bar"),
        std::string("foo\'bar"),
        std::string("foo\\tbar"),
        std::string("\"foo\'bar\""),
        std::string("\"test"),
        std::string("\'test"),
        std::string("\"\\xw\""),
        std::string("\'\\t\'"),
        std::string("\"\\N\""),
        std::string("\u0080"));

    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer(buffer.c_str());
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::exception);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanUnescapedControlCharacter", "[LexicalAnalyzerClassTest]")
{
    auto unescaped_char = GENERATE(
        char(0x01),
        char(0x02),
        char(0x03),
        char(0x04),
        char(0x05),
        char(0x06),
        char(0x07),
        char(0x08),
        char(0x0B),
        char(0x0C),
        char(0x0E),
        char(0x0F),
        char(0x10),
        char(0x11),
        char(0x12),
        char(0x13),
        char(0x14),
        char(0x15),
        char(0x16),
        char(0x17),
        char(0x18),
        char(0x19),
        char(0x1A),
        char(0x1B),
        char(0x1C),
        char(0x1D),
        char(0x1E),
        char(0x1F));
    std::string buffer("test");
    buffer.push_back(unescaped_char);

    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer(buffer.c_str());
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::exception);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanAnchorTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    fkyaml::detail::lexical_token_t token;

    SECTION("Test nothorw expected tokens with an anchor.")
    {
        lexer.set_input_buffer("test: &anchor foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::ANCHOR_PREFIX);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("anchor") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("Test nothrow unexpected tokens with an anchor.")
    {
        auto buffer =
            GENERATE(std::string("test: &anchor"), std::string("test: &anchor\r\n"), std::string("test: &anchor\n"));
        lexer.set_input_buffer(buffer.c_str());

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_THROWS_AS(token = lexer.get_next_token(), fkyaml::exception);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanAliasTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    fkyaml::detail::lexical_token_t token;

    SECTION("Test nothrow expected tokens with an alias.")
    {
        lexer.set_input_buffer("test: *anchor");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::ALIAS_PREFIX);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("anchor") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("Test nothrow unexpected tokens with an anchor.")
    {
        auto buffer = GENERATE(
            std::string("test: *"), std::string("test: *\r\n"), std::string("test: *\n"), std::string("test: * "));
        lexer.set_input_buffer(buffer.c_str());

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_THROWS_AS(token = lexer.get_next_token(), fkyaml::exception);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanCommentTokenTest", "[LexicalAnalyzerClassTest]")
{
    auto buffer = GENERATE(
        std::string("# comment\r"), std::string("# comment\r\n"), std::string("# comment\n"), std::string("# comment"));
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer(buffer.c_str());
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::COMMENT_PREFIX);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanReservedIndicatorTokenTest", "[LexicalAnalyzerClassTest]")
{
    auto buffer = GENERATE(std::string("@invalid"), std::string("`invalid"));
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer(buffer.c_str());
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::exception);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanKeyBooleanvalue_pair_tTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer("test: true");
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.get_string());
    REQUIRE(lexer.get_string().compare("test") == 0);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::BOOLEAN_VALUE);
    REQUIRE_NOTHROW(lexer.get_boolean());
    REQUIRE(lexer.get_boolean() == true);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanKeyIntegervalue_pair_tTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer("test: -5784");
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.get_string());
    REQUIRE(lexer.get_string().compare("test") == 0);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::INTEGER_VALUE);
    REQUIRE_NOTHROW(lexer.get_integer());
    REQUIRE(lexer.get_integer() == -5784);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanKeyFloatNumbervalue_pair_tTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer("test: -5.58e-3");
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.get_string());
    REQUIRE(lexer.get_string().compare("test") == 0);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::FLOAT_NUMBER_VALUE);
    REQUIRE_NOTHROW(lexer.get_float_number());
    REQUIRE(lexer.get_float_number() == -5.58e-3);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanKeyStringvalue_pair_tTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    lexer.set_input_buffer("test: \"some value\"");
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.get_string());
    REQUIRE(lexer.get_string().compare("test") == 0);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.get_string());
    REQUIRE(lexer.get_string().compare("some value") == 0);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanFlowSequenceTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    fkyaml::detail::lexical_token_t token;

    SECTION("Input source No.1.")
    {
        lexer.set_input_buffer("test: [ foo, bar ]");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("Input source No.2.")
    {
        lexer.set_input_buffer("test: [ { foo: one, bar: false }, { foo: two, bar: true } ]");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("one") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.get_boolean());
        REQUIRE(lexer.get_boolean() == false);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::MAPPING_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("two") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.get_boolean());
        REQUIRE(lexer.get_boolean() == true);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::MAPPING_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanFlowMappingTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    fkyaml::detail::lexical_token_t token;

    SECTION("Input source No.1.")
    {
        lexer.set_input_buffer("test: { bool: true, foo: bar, pi: 3.14 }");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("bool") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.get_boolean());
        REQUIRE(lexer.get_boolean() == true);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("pi") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::FLOAT_NUMBER_VALUE);
        REQUIRE_NOTHROW(lexer.get_float_number());
        REQUIRE(lexer.get_float_number() == 3.14);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::MAPPING_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("Input source No.2.")
    {
        lexer.set_input_buffer("test: {foo: bar}");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::MAPPING_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanBlockSequenceTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    fkyaml::detail::lexical_token_t token;

    SECTION("Input source No.1.")
    {
        auto buffer = GENERATE(std::string("test:\n  - foo\n  - bar"), std::string("test:\r\n  - foo\r\n  - bar"));

        lexer.set_input_buffer(buffer.c_str());

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::MAPPING_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("Input source No.2.")
    {
        auto buffer = GENERATE(
            std::string("test:\r\n  - foo: one\r\n    bar: false\r\n  - foo: two\r\n    bar: true"),
            std::string("test:\n  - foo: one\n    bar: false\n  - foo: two\n    bar: true"));

        lexer.set_input_buffer(buffer.c_str());

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::MAPPING_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("one") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.get_boolean());
        REQUIRE(lexer.get_boolean() == false);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("two") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.get_boolean());
        REQUIRE(lexer.get_boolean() == true);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanBlockMappingTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_analyzer<fkyaml::node> lexer;
    fkyaml::detail::lexical_token_t token;

    SECTION("Input source No.1.")
    {
        lexer.set_input_buffer("test:\n  bool: true\n  foo: bar\n  pi: 3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::MAPPING_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("bool") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.get_boolean());
        REQUIRE(lexer.get_boolean() == true);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("foo") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("bar") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("pi") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::FLOAT_NUMBER_VALUE);
        REQUIRE_NOTHROW(lexer.get_float_number());
        REQUIRE(lexer.get_float_number() == 3.14);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}
