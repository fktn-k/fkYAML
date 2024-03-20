//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#ifndef FK_YAML_TEST_USE_SINGLE_HEADER
    #include <fkYAML/detail/input/input_adapter.hpp>
    #include <fkYAML/detail/input/lexical_analyzer.hpp>
#endif
#include <fkYAML/node.hpp>

using pchar_lexer_t =
    fkyaml::detail::lexical_analyzer<fkyaml::node, fkyaml::detail::iterator_input_adapter<const char*>>;
using str_lexer_t =
    fkyaml::detail::lexical_analyzer<fkyaml::node, fkyaml::detail::iterator_input_adapter<std::string::iterator>>;

TEST_CASE("LexicalAnalyzerClassTest_ScanYamlVersionDirectiveTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_token_t token;

    SECTION("Test nothrow expected tokens.")
    {
        using value_pair_t = std::pair<std::string, std::string>;
        auto value_pair = GENERATE(
            value_pair_t(std::string("%YAML 1.1\r"), std::string("1.1")),
            value_pair_t(std::string("%YAML 1.2\n"), std::string("1.2")),
            value_pair_t(std::string("%YAML 1.2 "), std::string("1.2")));

        str_lexer_t lexer(fkyaml::detail::input_adapter(value_pair.first));

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
            std::string("%YAMR 1.2 \r\n"),
            std::string("%YANL 1.2    \n"));

        str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));
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

        str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanTagDirectiveTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_token_t token;

    SECTION("Test nothrow expected tokens.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter("%TAG "));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::TAG_DIRECTIVE);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("Test nothrow expected tokens with invalid content.")
    {
        auto buffer = GENERATE(std::string("%TUB"), std::string("%TAC"));

        str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::INVALID_DIRECTIVE);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("Test nothrow expected tokens.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter("%TAGE"));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanReservedDirectiveTest", "[LexicalAnalyzerClassTest]")
{
    auto buffer =
        GENERATE(std::string("%TEST"), std::string("%1984\n"), std::string("%TEST4LIB\r"), std::string("%%ERROR\r\n"));

    fkyaml::detail::lexical_token_t token;
    str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::INVALID_DIRECTIVE);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanEmptyDirectiveTest", "[LexicalAnalyzerClassTest]")
{
    pchar_lexer_t lexer(fkyaml::detail::input_adapter("%"));
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanEndOfDirectivesTest", "[LexicalAnalyzerClassTest]")
{
    pchar_lexer_t lexer(fkyaml::detail::input_adapter("%YAML 1.2\n---\nfoo: bar"));
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::YAML_VER_DIRECTIVE);
    REQUIRE(lexer.get_yaml_version() == "1.2");
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_DIRECTIVES);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
    REQUIRE(lexer.get_string() == "foo");
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
    REQUIRE(lexer.get_string() == "bar");
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanEndOfDocumentsTest", "[LexicalAnalyzerClassTest]")
{
    pchar_lexer_t lexer(fkyaml::detail::input_adapter("%YAML 1.2\n---\n..."));
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::YAML_VER_DIRECTIVE);
    REQUIRE(lexer.get_yaml_version() == "1.2");
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_DIRECTIVES);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_DOCUMENT);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanColonTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_token_t token;

    SECTION("Test colon with half-width space.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(": "));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("Test colon with CR newline code.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(":\r"));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("Test colon with CRLF newline code.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(":\r\n"));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("Test colon with LF newline code.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(":\n"));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("Test colon with a comment and a CRLF newline code.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(": # comment\r\n"));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("Test colon with a comment and a LF newline code.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(": # comment\n"));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("Test colon with a comment and no newline code")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(": # comment"));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("Test colon with many spaces and a CRLF newline code.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(":                         \r\n"));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("Test colon with many spaces and a LF newline code.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(":                         \n"));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("Test colon with many spaces and no newline code.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(":                         "));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("Test colon with an always-safe character.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(":test"));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == ":test");
    }

    SECTION("Test colon with a flow indicator in a non-flow context.")
    {
        auto input =
            GENERATE(std::string(":,"), std::string(":{"), std::string(":}"), std::string(":["), std::string(":]"));
        str_lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == input);
    }

    SECTION("Test colon with a flow indicator in a flow context.")
    {
        auto input = GENERATE(
            std::string("{:,"), std::string("{:{"), std::string("{:}"), std::string("{:["), std::string("{:]"));
        str_lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanNullTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_token_t token;

    SECTION("Test nothrow expected tokens.")
    {
        auto buffer = GENERATE(std::string("null"), std::string("Null"), std::string("NULL"), std::string("~"));
        str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::NULL_VALUE);
        REQUIRE_NOTHROW(lexer.get_null());
        REQUIRE(lexer.get_null() == nullptr);
    }

    SECTION("Test nothrow unexpected tokens.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter("test"));
        REQUIRE_NOTHROW(lexer.get_next_token());
        REQUIRE_THROWS_AS(lexer.get_null(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanBooleanTrueTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_token_t token;

    SECTION("Test nothrow expected tokens.")
    {
        auto buffer = GENERATE(std::string("true"), std::string("True"), std::string("TRUE"));
        str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.get_boolean());
        REQUIRE(lexer.get_boolean() == true);
    }

    SECTION("Test nothrow unexpected tokens.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter("test"));
        REQUIRE_NOTHROW(lexer.get_next_token());
        REQUIRE_THROWS_AS(lexer.get_boolean(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanBooleanFalseTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_token_t token;

    SECTION("Test nothrow expected tokens.")
    {
        auto buffer = GENERATE(std::string("false"), std::string("False"), std::string("FALSE"));
        str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::BOOLEAN_VALUE);
        REQUIRE_NOTHROW(lexer.get_boolean());
        REQUIRE(lexer.get_boolean() == false);
    }

    SECTION("Test nothrow unexpected tokens.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter("test"));
        REQUIRE_NOTHROW(lexer.get_next_token());
        REQUIRE_THROWS_AS(lexer.get_boolean(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanIntegerTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_token_t token;

    SECTION("Test nothrow expected tokens.")
    {
        using value_pair_t = std::pair<std::string, fkyaml::node::integer_type>;
        auto value_pair = GENERATE(
            value_pair_t(std::string("-1234"), -1234),
            value_pair_t(std::string("-853255"), -853255),
            value_pair_t(std::string("-1"), -1),
            value_pair_t(std::string("0"), 0),
            value_pair_t(std::string("643"), 643),
            value_pair_t(std::string("+123"), 123));

        str_lexer_t lexer(fkyaml::detail::input_adapter(value_pair.first));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::INTEGER_VALUE);
        REQUIRE_NOTHROW(lexer.get_integer());
        REQUIRE(lexer.get_integer() == value_pair.second);
    }

    SECTION("Test nothrow unexpected tokens.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter("test"));
        REQUIRE_NOTHROW(lexer.get_next_token());
        REQUIRE_THROWS_AS(lexer.get_integer(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanOctalNumberTokenTest", "[LexicalAnalyzerClassTest]")
{
    using value_pair_t = std::pair<std::string, fkyaml::node::integer_type>;
    auto value_pair = GENERATE(
        value_pair_t(std::string("0o27"), 027),
        value_pair_t(std::string("0o5"), 05),
        value_pair_t(std::string("0o77772"), 077772),
        value_pair_t(std::string("0o672}"), 0672));

    str_lexer_t lexer(fkyaml::detail::input_adapter(value_pair.first));
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::INTEGER_VALUE);
    REQUIRE_NOTHROW(lexer.get_integer());
    REQUIRE(lexer.get_integer() == value_pair.second);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanHexadecimalNumberTokenTest", "[LexicalAnalyzerClassTest]")
{
    using value_pair_t = std::pair<std::string, fkyaml::node::integer_type>;
    auto value_pair = GENERATE(
        value_pair_t(std::string("0xA04F"), 0xA04F),
        value_pair_t(std::string("0xa7F3"), 0xa7F3),
        value_pair_t(std::string("0xFf29Bc"), 0xFf29Bc));

    str_lexer_t lexer(fkyaml::detail::input_adapter(value_pair.first));
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::INTEGER_VALUE);
    REQUIRE_NOTHROW(lexer.get_integer());
    REQUIRE(lexer.get_integer() == value_pair.second);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanFloatNumberTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_token_t token;

    SECTION("Test nothrow expected tokens.")
    {
        using value_pair_t = std::pair<std::string, fkyaml::node::float_number_type>;
        auto value_pair = GENERATE(
            value_pair_t(std::string("-1.234"), -1.234),
            value_pair_t(std::string("567.8"), 567.8),
            value_pair_t(std::string("0.24"), 0.24),
            value_pair_t(std::string("9.8e-3"), 9.8e-3),
            value_pair_t(std::string("3.95E3"), 3.95e3),
            value_pair_t(std::string("1.863e+3"), 1.863e+3));

        str_lexer_t lexer(fkyaml::detail::input_adapter(value_pair.first));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::FLOAT_NUMBER_VALUE);
        REQUIRE_NOTHROW(lexer.get_float_number());
        REQUIRE(lexer.get_float_number() == value_pair.second);
    }

    SECTION("Test nothrow unexpected float tokens.")
    {
        auto input = GENERATE(std::string("0."), std::string("1.23e"), std::string("1.2e-z"));
        str_lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("Test non-float tokens.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter("test"));
        REQUIRE_NOTHROW(lexer.get_next_token());
        REQUIRE_THROWS_AS(lexer.get_float_number(), fkyaml::parse_error);
    }
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
    str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));

    SECTION("Test nothrow expected buffers.")
    {
        REQUIRE_NOTHROW(lexer.get_next_token());
    }

    SECTION("Test result of infinity literal tokens.")
    {
        REQUIRE(lexer.get_next_token() == fkyaml::detail::lexical_token_t::FLOAT_NUMBER_VALUE);
        REQUIRE_NOTHROW(lexer.get_float_number());
        REQUIRE(std::isinf(lexer.get_float_number()) == true);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanNaNTokenTest", "[LexicalAnalyzerClassTest]")
{
    auto buffer = GENERATE(std::string(".nan"), std::string(".NaN"), std::string(".NAN"));
    str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));

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

TEST_CASE("LexicalAnalyzerClassTest_ScanStringTokenTest", "[LexicalAnalyzerClassTest]")
{
    using value_pair_t = std::pair<std::string, fkyaml::node::string_type>;
    auto value_pair = GENERATE(
        value_pair_t(std::string("\"\""), fkyaml::node::string_type("")),
        value_pair_t(std::string("\'\'"), fkyaml::node::string_type("")),

        value_pair_t(std::string("test"), fkyaml::node::string_type("test")),
        value_pair_t(std::string("nop"), fkyaml::node::string_type("nop")),
        value_pair_t(std::string("none"), fkyaml::node::string_type("none")),
        value_pair_t(std::string("?test"), fkyaml::node::string_type("?test")),
        value_pair_t(std::string(".NET"), fkyaml::node::string_type(".NET")),
        value_pair_t(std::string(".on"), fkyaml::node::string_type(".on")),
        value_pair_t(std::string(".n"), fkyaml::node::string_type(".n")),
        value_pair_t(std::string("-t"), fkyaml::node::string_type("-t")),
        value_pair_t(std::string("-foo"), fkyaml::node::string_type("-foo")),
        value_pair_t(std::string("-.test"), fkyaml::node::string_type("-.test")),
        value_pair_t(std::string("1.2.3"), fkyaml::node::string_type("1.2.3")),
        value_pair_t(std::string("foo,bar"), fkyaml::node::string_type("foo,bar")),
        value_pair_t(std::string("foo[bar"), fkyaml::node::string_type("foo[bar")),
        value_pair_t(std::string("foo]bar"), fkyaml::node::string_type("foo]bar")),
        value_pair_t(std::string("foo{bar"), fkyaml::node::string_type("foo{bar")),
        value_pair_t(std::string("foo}bar"), fkyaml::node::string_type("foo}bar")),
        value_pair_t(std::string("foo:bar"), fkyaml::node::string_type("foo:bar")),
        value_pair_t(std::string("foo bar"), fkyaml::node::string_type("foo bar")),
        value_pair_t(std::string("foo\"bar"), fkyaml::node::string_type("foo\"bar")),
        value_pair_t(std::string("foo\'s bar"), fkyaml::node::string_type("foo\'s bar")),
        value_pair_t(std::string("nullValue"), fkyaml::node::string_type("nullValue")),
        value_pair_t(std::string("NullValue"), fkyaml::node::string_type("NullValue")),
        value_pair_t(std::string("NULL_VALUE"), fkyaml::node::string_type("NULL_VALUE")),
        value_pair_t(std::string("~Value"), fkyaml::node::string_type("~Value")),
        value_pair_t(std::string("trueValue"), fkyaml::node::string_type("trueValue")),
        value_pair_t(std::string("TrueValue"), fkyaml::node::string_type("TrueValue")),
        value_pair_t(std::string("TRUE_VALUE"), fkyaml::node::string_type("TRUE_VALUE")),
        value_pair_t(std::string("falseValue"), fkyaml::node::string_type("falseValue")),
        value_pair_t(std::string("FalseValue"), fkyaml::node::string_type("FalseValue")),
        value_pair_t(std::string("FALSE_VALUE"), fkyaml::node::string_type("FALSE_VALUE")),
        value_pair_t(std::string(".infValue"), fkyaml::node::string_type(".infValue")),
        value_pair_t(std::string(".InfValue"), fkyaml::node::string_type(".InfValue")),
        value_pair_t(std::string(".INF_VALUE"), fkyaml::node::string_type(".INF_VALUE")),
        value_pair_t(std::string("-.infValue"), fkyaml::node::string_type("-.infValue")),
        value_pair_t(std::string("-.InfValue"), fkyaml::node::string_type("-.InfValue")),
        value_pair_t(std::string("-.INF_VALUE"), fkyaml::node::string_type("-.INF_VALUE")),
        value_pair_t(std::string(".nanValue"), fkyaml::node::string_type(".nanValue")),
        value_pair_t(std::string(".NaNValue"), fkyaml::node::string_type(".NaNValue")),
        value_pair_t(std::string(".NAN_VALUE"), fkyaml::node::string_type(".NAN_VALUE")),

        value_pair_t(std::string("\'foo\"bar\'"), fkyaml::node::string_type("foo\"bar")),
        value_pair_t(std::string("\'foo bar\'"), fkyaml::node::string_type("foo bar")),
        value_pair_t(std::string("\'foo\'\'bar\'"), fkyaml::node::string_type("foo\'bar")),
        value_pair_t(std::string("\'foo,bar\'"), fkyaml::node::string_type("foo,bar")),
        value_pair_t(std::string("\'foo]bar\'"), fkyaml::node::string_type("foo]bar")),
        value_pair_t(std::string("\'foo}bar\'"), fkyaml::node::string_type("foo}bar")),
        value_pair_t(std::string("\'foo\"bar\'"), fkyaml::node::string_type("foo\"bar")),
        value_pair_t(std::string("\'foo:bar\'"), fkyaml::node::string_type("foo:bar")),

        value_pair_t(std::string("\"foo bar\""), fkyaml::node::string_type("foo bar")),
        value_pair_t(std::string("\"foo's bar\""), fkyaml::node::string_type("foo's bar")),
        value_pair_t(std::string("\"foo:bar\""), fkyaml::node::string_type("foo:bar")),
        value_pair_t(std::string("\"foo,bar\""), fkyaml::node::string_type("foo,bar")),
        value_pair_t(std::string("\"foo]bar\""), fkyaml::node::string_type("foo]bar")),
        value_pair_t(std::string("\"foo}bar\""), fkyaml::node::string_type("foo}bar")),
        value_pair_t(std::string("\"foo\\abar\""), fkyaml::node::string_type("foo\abar")),
        value_pair_t(std::string("\"foo\\bbar\""), fkyaml::node::string_type("foo\bbar")),
        value_pair_t(std::string("\"foo\\tbar\""), fkyaml::node::string_type("foo\tbar")),
        value_pair_t(std::string("\"foo\tbar\""), fkyaml::node::string_type("foo\tbar")),
        value_pair_t(std::string("\"foo\\nbar\""), fkyaml::node::string_type("foo\nbar")),
        value_pair_t(std::string("\"foo\\vbar\""), fkyaml::node::string_type("foo\vbar")),
        value_pair_t(std::string("\"foo\\fbar\""), fkyaml::node::string_type("foo\fbar")),
        value_pair_t(std::string("\"foo\\rbar\""), fkyaml::node::string_type("foo\rbar")),
        value_pair_t(std::string("\"foo\\ebar\""), fkyaml::node::string_type("foo\u001Bbar")),
        value_pair_t(std::string("\"foo\\ bar\""), fkyaml::node::string_type("foo bar")),
        value_pair_t(std::string("\"foo\\\"bar\""), fkyaml::node::string_type("foo\"bar")),
        value_pair_t(std::string("\"foo\\/bar\""), fkyaml::node::string_type("foo/bar")),
        value_pair_t(std::string("\"foo\\\\bar\""), fkyaml::node::string_type("foo\\bar")),
        value_pair_t(std::string("\"foo\\Nbar\""), fkyaml::node::string_type("foo\u0085bar")),
        value_pair_t(std::string("\"foo\\_bar\""), fkyaml::node::string_type("foo\u00A0bar")),
        value_pair_t(std::string("\"foo\\Lbar\""), fkyaml::node::string_type("foo\u2028bar")),
        value_pair_t(std::string("\"foo\\Pbar\""), fkyaml::node::string_type("foo\u2029bar")),
        value_pair_t(std::string("\"\\x30\\x2B\\x6d\""), fkyaml::node::string_type("0+m")));

    str_lexer_t lexer(fkyaml::detail::input_adapter(value_pair.first));
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.get_string());
    REQUIRE(lexer.get_string() == value_pair.second);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanMultiByteCharStringTokenTest", "[LexicalAnalyzerClassTest]")
{
    using char_traits_t = std::char_traits<char>;
    auto mb_char = GENERATE(
        std::string {char_traits_t::to_char_type(0xC2), char_traits_t::to_char_type(0x80)},
        std::string {char_traits_t::to_char_type(0xDF), char_traits_t::to_char_type(0xBF)},
        std::string {
            char_traits_t::to_char_type(0xE0), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xEC), char_traits_t::to_char_type(0xBF), char_traits_t::to_char_type(0xBF)},
        std::string {
            char_traits_t::to_char_type(0xED), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xED), char_traits_t::to_char_type(0x9F), char_traits_t::to_char_type(0xBF)},
        std::string {
            char_traits_t::to_char_type(0xEE), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xEF), char_traits_t::to_char_type(0xBF), char_traits_t::to_char_type(0xBF)},
        std::string {
            char_traits_t::to_char_type(0xF0),
            char_traits_t::to_char_type(0x90),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xF0),
            char_traits_t::to_char_type(0xBF),
            char_traits_t::to_char_type(0xBF),
            char_traits_t::to_char_type(0xBF)},
        std::string {
            char_traits_t::to_char_type(0xF1),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xF3),
            char_traits_t::to_char_type(0xBF),
            char_traits_t::to_char_type(0xBF),
            char_traits_t::to_char_type(0xBF)},
        std::string {
            char_traits_t::to_char_type(0xF4),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xF4),
            char_traits_t::to_char_type(0x8F),
            char_traits_t::to_char_type(0xBF),
            char_traits_t::to_char_type(0xBF)});

    str_lexer_t lexer(fkyaml::detail::input_adapter(mb_char));
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.get_string());
    REQUIRE(lexer.get_string() == mb_char);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanEscapedUnicodeStringTokenTest", "[LexicalAnalyzerClassTest]")
{
    using value_pair_t = std::pair<std::string, std::string>;
    using char_traits_t = std::char_traits<char>;
    auto value_pair = GENERATE(
        value_pair_t(std::string("\"\\x00\""), std::string {char_traits_t::to_char_type(0x00)}),
        value_pair_t(std::string("\"\\x40\""), std::string {char_traits_t::to_char_type(0x40)}),
        value_pair_t(std::string("\"\\x7F\""), std::string {char_traits_t::to_char_type(0x7F)}),
        value_pair_t(std::string("\"\\u0000\""), std::string {char_traits_t::to_char_type(0x00)}),
        value_pair_t(std::string("\"\\u0040\""), std::string {char_traits_t::to_char_type(0x40)}),
        value_pair_t(std::string("\"\\u007F\""), std::string {char_traits_t::to_char_type(0x7F)}),
        value_pair_t(
            std::string("\"\\u0080\""),
            std::string {char_traits_t::to_char_type(0xC2), char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            std::string("\"\\u0400\""),
            std::string {char_traits_t::to_char_type(0xD0), char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            std::string("\"\\u07FF\""),
            std::string {char_traits_t::to_char_type(0xDF), char_traits_t::to_char_type(0xBF)}),
        value_pair_t(
            std::string("\"\\u0800\""),
            std::string {
                char_traits_t::to_char_type(0xE0),
                char_traits_t::to_char_type(0xA0),
                char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            std::string("\"\\u8000\""),
            std::string {
                char_traits_t::to_char_type(0xE8),
                char_traits_t::to_char_type(0x80),
                char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            std::string("\"\\uFFFF\""),
            std::string {
                char_traits_t::to_char_type(0xEF),
                char_traits_t::to_char_type(0xBF),
                char_traits_t::to_char_type(0xBF)}),
        value_pair_t(std::string("\"\\U00000000\""), std::string {char_traits_t::to_char_type(0x00)}),
        value_pair_t(std::string("\"\\U00000040\""), std::string {char_traits_t::to_char_type(0x40)}),
        value_pair_t(std::string("\"\\U0000007F\""), std::string {char_traits_t::to_char_type(0x7F)}),
        value_pair_t(
            std::string("\"\\U00000080\""),
            std::string {char_traits_t::to_char_type(0xC2), char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            std::string("\"\\U00000400\""),
            std::string {char_traits_t::to_char_type(0xD0), char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            std::string("\"\\U000007FF\""),
            std::string {char_traits_t::to_char_type(0xDF), char_traits_t::to_char_type(0xBF)}),
        value_pair_t(
            std::string("\"\\U00000800\""),
            std::string {
                char_traits_t::to_char_type(0xE0),
                char_traits_t::to_char_type(0xA0),
                char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            std::string("\"\\U00008000\""),
            std::string {
                char_traits_t::to_char_type(0xE8),
                char_traits_t::to_char_type(0x80),
                char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            std::string("\"\\U0000FFFF\""),
            std::string {
                char_traits_t::to_char_type(0xEF),
                char_traits_t::to_char_type(0xBF),
                char_traits_t::to_char_type(0xBF)}),
        value_pair_t(
            std::string("\"\\U00010000\""),
            std::string {
                char_traits_t::to_char_type(0xF0),
                char_traits_t::to_char_type(0x90),
                char_traits_t::to_char_type(0x80),
                char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            std::string("\"\\U00080000\""),
            std::string {
                char_traits_t::to_char_type(0xF2),
                char_traits_t::to_char_type(0x80),
                char_traits_t::to_char_type(0x80),
                char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            std::string("\"\\U0010FFFF\""),
            std::string {
                char_traits_t::to_char_type(0xF4),
                char_traits_t::to_char_type(0x8F),
                char_traits_t::to_char_type(0xBF),
                char_traits_t::to_char_type(0xBF)}));

    str_lexer_t lexer(fkyaml::detail::input_adapter(value_pair.first));
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
    REQUIRE_NOTHROW(lexer.get_string());
    REQUIRE(lexer.get_string() == value_pair.second);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanInvalidStringTokenTest", "[LexicalAnalyzerClassTest]")
{
    SECTION("parse_error expected")
    {
        auto buffer = GENERATE(
            std::string("foo\\tbar"),
            std::string("\"test"),
            std::string("\'test"),
            std::string("\'test\n\'"),
            std::string("\"test\n\""),
            std::string("\"\\xw\""),
            std::string("\"\\x+\""),
            std::string("\"\\x=\""),
            std::string("\"\\x^\""),
            std::string("\"\\x{\""),
            std::string("\'\\t\'"),
            std::string("\"\\Q\""));

        str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("invalid_encoding expected")
    {
        std::string buffer = "\"\\U00110000\"";
        str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::invalid_encoding);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanInvalidMultiByteCharStringTokenTest", "[LexicalAnalyzerClassTest]")
{
    using char_traits_t = std::char_traits<char>;
    auto mb_char = GENERATE(
        std::string {char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0x80)},
        std::string {char_traits_t::to_char_type(0xC1), char_traits_t::to_char_type(0x80)},
        std::string {char_traits_t::to_char_type(0xC2), char_traits_t::to_char_type(0x7F)},
        std::string {char_traits_t::to_char_type(0xC2), char_traits_t::to_char_type(0xC0)},
        std::string {
            char_traits_t::to_char_type(0xE0), char_traits_t::to_char_type(0x7F), char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xE0), char_traits_t::to_char_type(0xC0), char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xE0), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0x7F)},
        std::string {
            char_traits_t::to_char_type(0xE0), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0xC0)},
        std::string {
            char_traits_t::to_char_type(0xED), char_traits_t::to_char_type(0x7F), char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xED), char_traits_t::to_char_type(0xA0), char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xED), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0x7F)},
        std::string {
            char_traits_t::to_char_type(0xED), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0xC0)},
        std::string {
            char_traits_t::to_char_type(0xEE), char_traits_t::to_char_type(0x7F), char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xEE), char_traits_t::to_char_type(0xC0), char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xEF), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0x7F)},
        std::string {
            char_traits_t::to_char_type(0xEF), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0xC0)},
        std::string {
            char_traits_t::to_char_type(0xF0),
            char_traits_t::to_char_type(0x8F),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xF0),
            char_traits_t::to_char_type(0xC0),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xF0),
            char_traits_t::to_char_type(0x90),
            char_traits_t::to_char_type(0x7F),
            char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xF0),
            char_traits_t::to_char_type(0x90),
            char_traits_t::to_char_type(0xC0),
            char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xF0),
            char_traits_t::to_char_type(0x90),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x7F)},
        std::string {
            char_traits_t::to_char_type(0xF0),
            char_traits_t::to_char_type(0x90),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0xC0)},
        std::string {
            char_traits_t::to_char_type(0xF1),
            char_traits_t::to_char_type(0x7F),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xF1),
            char_traits_t::to_char_type(0xC0),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xF1),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x7F),
            char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xF1),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0xC0),
            char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xF1),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x7F)},
        std::string {
            char_traits_t::to_char_type(0xF1),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0xC0)},
        std::string {
            char_traits_t::to_char_type(0xF4),
            char_traits_t::to_char_type(0x7F),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xF4),
            char_traits_t::to_char_type(0x90),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xF4),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x7F),
            char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xF4),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0xC0),
            char_traits_t::to_char_type(0x80)},
        std::string {
            char_traits_t::to_char_type(0xF4),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x7F)},
        std::string {
            char_traits_t::to_char_type(0xF4),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0xC0)},
        std::string {
            char_traits_t::to_char_type(0xF5),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80),
            char_traits_t::to_char_type(0x80)});

    str_lexer_t lexer(fkyaml::detail::input_adapter(mb_char));
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::invalid_encoding);
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

    str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanLiteralStringScalar", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_token_t token;

    SECTION("empty literal string scalar with strip chomping")
    {
        const char input[] = "|-\r\n"
                             "  \r\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "");
    }

    SECTION("empty literal string scalar with clip chomping")
    {
        const char input[] = "|\r\n"
                             "  \r\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "");
    }

    SECTION("empty literal string scalar with keep chomping")
    {
        const char input[] = "|+\r\n"
                             "  \r\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "\n");
    }

    SECTION("literal string scalar with 0 indent level.")
    {
        const char input[] = "|0\n"
                             "foo";

        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("less indented literal string scalar")
    {
        const char input[] = "|2\n"
                             " foo";

        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("literal scalar with the first line being more indented than the indicated level")
    {
        const char input[] = "|2\n"
                             "    foo\n"
                             "  bar\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "  foo\nbar\n");
    }

    SECTION("literal string scalar with windows style newlines.")
    {
        const char input[] = "|\r\n"
                             "  foo\r\n"
                             "  bar\r\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "foo\nbar\n");
    }

    SECTION("literal string scalar with implicit indentation and strip chomping.")
    {
        const char input[] = "|-\n"
                             "\n"
                             "  foo\n"
                             "  bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "\nfoo\nbar\n\nbaz");
    }

    SECTION("literal string scalar with explicit indentation and strip chomping.")
    {
        const char input[] = "|-2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "foo\n  bar\n\nbaz");
    }

    SECTION("literal string scalar with implicit indentation and clip chomping.")
    {
        const char input[] = "|\n"
                             "\n"
                             "  foo\n"
                             "  bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "\nfoo\nbar\n\nbaz\n");
    }

    SECTION("literal string scalar with explicit indentation and clip chomping.")
    {
        const char input[] = "|2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "foo\n  bar\n\nbaz\n");
    }

    SECTION("literal string scalar with clip chomping and no trailing newlines")
    {
        const char input[] = "|2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "foo\n  bar\n\nbaz");
    }

    SECTION("literal string scalar with implicit indentation and keep chomping.")
    {
        const char input[] = "|+\n"
                             "\n"
                             "  foo\n"
                             "  bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "\nfoo\nbar\n\nbaz\n\n");
    }

    SECTION("literal string scalar with explicit indentation and keep chomping.")
    {
        const char input[] = "|+2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "foo\n  bar\n\nbaz\n\n");
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanFoldedStringScalar", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_token_t token;

    SECTION("empty folded string scalar with strip chomping")
    {
        const char input[] = ">-\r\n"
                             "  \r\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "");
    }

    SECTION("empty folded string scalar with clip chomping")
    {
        const char input[] = ">\r\n"
                             "  \r\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "");
    }

    SECTION("empty folded string scalar with keep chomping")
    {
        const char input[] = ">+\r\n"
                             "  \r\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "\n");
    }

    SECTION("folded string scalar with 0 indent level.")
    {
        const char input[] = "|0\n"
                             "foo";

        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("less indented folded string scalar")
    {
        const char input[] = ">2\n"
                             " foo";

        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("folded string scalar with the first line being more indented than the indicated level")
    {
        const char input[] = ">2\n"
                             "    foo\n"
                             "  bar\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "\n  foo\nbar\n");
    }

    SECTION("folded string scalar with the non-first line being more indented than the indicated level")
    {
        const char input[] = ">2\n"
                             "  foo\n"
                             "    bar\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "foo\n  bar\n");
    }

    SECTION("folded string scalar with windows style newlines.")
    {
        const char input[] = ">\r\n"
                             "  foo\r\n"
                             "  \r\n"
                             "\r\n"
                             "  bar\r\n"
                             " \r\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "foo\n\nbar\n");
    }

    SECTION("folded string scalar with implicit indentation and strip chomping.")
    {
        const char input[] = ">-\n"
                             "  foo\n"
                             "  bar\n"
                             " \n"
                             "\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "foo bar");
    }

    SECTION("folded string scalar with implicit indentation and clip chomping.")
    {
        const char input[] = ">\n"
                             "  foo\n"
                             "  bar\n"
                             "  \n"
                             "\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "foo bar\n");
    }

    SECTION("folded string scalar with implicit indentation and keep chomping.")
    {
        const char input[] = ">+\n"
                             "  foo\n"
                             "  bar\n"
                             " \n"
                             "\n";
        pchar_lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE(lexer.get_string() == "foo bar\n\n");
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanAnchorTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_token_t token;

    SECTION("Test nothorw expected tokens with an anchor.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter("test: &anchor foo"));

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
        str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_THROWS_AS(token = lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanAliasTokenTest", "[LexicalAnalyzerClassTest]")
{
    fkyaml::detail::lexical_token_t token;

    SECTION("Test nothrow expected tokens with an alias.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter("test: *anchor"));

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
        str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_THROWS_AS(token = lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzerClassTest_ScanCommentTokenTest", "[LexicalAnalyzerClassTest]")
{
    auto buffer = GENERATE(
        std::string("# comment\r"), std::string("# comment\r\n"), std::string("# comment\n"), std::string("# comment"));
    str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));
    fkyaml::detail::lexical_token_t token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::COMMENT_PREFIX);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanReservedIndicatorTokenTest", "[LexicalAnalyzerClassTest]")
{
    auto buffer = GENERATE(std::string("@invalid"), std::string("`invalid"));
    str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
}

TEST_CASE("LexicalAnalyzerClassTest_ScanKeyBooleanValuePairTokenTest", "[LexicalAnalyzerClassTest]")
{
    pchar_lexer_t lexer(fkyaml::detail::input_adapter("test: true"));
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

TEST_CASE("LexicalAnalyzerClassTest_ScanKeyIntegerValuePairTokenTest", "[LexicalAnalyzerClassTest]")
{
    pchar_lexer_t lexer(fkyaml::detail::input_adapter("test: -5784"));
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

TEST_CASE("LexicalAnalyzerClassTest_ScanKeyFloatNumberValuePairTokenTest", "[LexicalAnalyzerClassTest]")
{
    pchar_lexer_t lexer(fkyaml::detail::input_adapter("test: -5.58e-3"));
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

TEST_CASE("LexicalAnalyzerClassTest_ScanKeyStringValuePairTokenTest", "[LexicalAnalyzerClassTest]")
{
    pchar_lexer_t lexer(fkyaml::detail::input_adapter("test: \"some value\""));
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
    fkyaml::detail::lexical_token_t token;

    SECTION("Input source No.1.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter("test: [ foo, bar ]"));

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
        pchar_lexer_t lexer(
            fkyaml::detail::input_adapter("test: [ { foo: one, bar: false }, { foo: two, bar: true } ]"));

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
    fkyaml::detail::lexical_token_t token;

    SECTION("Input source No.1.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter("test: { bool: true, foo: bar, pi: 3.14 }"));

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
        pchar_lexer_t lexer(fkyaml::detail::input_adapter("test: {foo: bar}"));

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
    fkyaml::detail::lexical_token_t token;

    SECTION("Input source No.1.")
    {
        auto buffer = GENERATE(std::string("test:\n  - foo\n  - bar"), std::string("test:\r\n  - foo\r\n  - bar"));

        str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

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

        str_lexer_t lexer(fkyaml::detail::input_adapter(buffer));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

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
    fkyaml::detail::lexical_token_t token;

    SECTION("Input source No.1.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter("test:\n  bool: true\n  foo: \'bar\'\n  pi: 3.14"));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string().compare("test") == 0);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

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

    SECTION("input soure No.2.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter("test: |\n  a literal scalar.\nfoo: \'bar\'\npi: 3.14"));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string() == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string() == "a literal scalar.\n");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string() == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string() == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string() == "pi");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::FLOAT_NUMBER_VALUE);
        REQUIRE_NOTHROW(lexer.get_float_number());
        REQUIRE(lexer.get_float_number() == 3.14);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("input soure No.3.")
    {
        pchar_lexer_t lexer(fkyaml::detail::input_adapter("test: >\n  a literal scalar.\nfoo: \'bar\'\npi: 3.14"));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string() == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string() == "a literal scalar.\n");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string() == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string() == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token == fkyaml::detail::lexical_token_t::STRING_VALUE);
        REQUIRE_NOTHROW(lexer.get_string());
        REQUIRE(lexer.get_string() == "pi");

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
