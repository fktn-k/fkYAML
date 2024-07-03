//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.9
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

using lexer_t = fkyaml::detail::lexical_analyzer;

TEST_CASE("LexicalAnalyzer_YamlVersionDirective") {
    lexer_t::lexical_token token;
    std::string input_buffer {};

    SECTION("valid YAML directive") {
        using value_pair_t = std::pair<std::string, std::string>;
        auto value_pair = GENERATE(
            value_pair_t(std::string("%YAML 1.1 "), std::string("1.1")),
            value_pair_t(std::string("%YAML\t1.1\t"), std::string("1.1")),
            value_pair_t(std::string("%YAML 1.1\n"), std::string("1.1")),
            value_pair_t(std::string("%YAML 1.1"), std::string("1.1")),
            value_pair_t(std::string("%YAML 1.2 "), std::string("1.2")),
            value_pair_t(std::string("%YAML\t1.2\t"), std::string("1.2")),
            value_pair_t(std::string("%YAML 1.2\n"), std::string("1.2")),
            value_pair_t(std::string("%YAML 1.2"), std::string("1.2")));

        auto input_adapter = fkyaml::detail::input_adapter(value_pair.first);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::DIRECTIVE);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("wrong YAML directive") {
        auto buffer = GENERATE(std::string("%YUML 1.2"), std::string("%YANL 1.2    \n"), std::string("%YAML1.2"));

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::DIRECTIVE);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("invalid YAML directive value") {
        auto buffer = GENERATE(
            std::string("%YAML 1.3\n"),
            std::string("%YAML 2.0\n"),
            std::string("%YAML 12"),
            std::string("%YAML 123"),
            std::string("%YAML 1.23"),
            std::string("%YAML 1.11"),
            std::string("%YAML 1.A"),
            std::string("%YAML AbC"));

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_TagDirective") {
    lexer_t::lexical_token token;
    std::string input_buffer {};

    SECTION("primary tag handle") {
        auto input = GENERATE(std::string("%TAG ! foo"), std::string("%TAG\t!\tfoo"));
        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::DIRECTIVE);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("secondary tag handle") {
        auto input = GENERATE(std::string("%TAG !! foo"), std::string("%TAG\t!!\tfoo"));
        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::DIRECTIVE);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("named tag handle") {
        auto input = GENERATE(std::string("%TAG !va1id-ta9! foo"), std::string("%TAG\t!va1id-ta9!\tfoo"));
        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::DIRECTIVE);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("invalid TAG directive") {
        auto buffer = GENERATE(std::string("%TUB"), std::string("%TAC"), std::string("%TAGE"));

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::DIRECTIVE);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("invalid tag handle") {
        auto input = GENERATE(
            std::string("%TAG foo bar"),
            std::string("%TAG !!abc bar"),
            std::string("%TAG !"),
            std::string("%TAG !!"),
            std::string("%TAG !valid!"),
            std::string("%TAG !invalid"),
            std::string("%TAG !invalid bar"),
            std::string("%TAG !invalid\tbar"),
            std::string("%TAG !inv@lid! bar"),
            std::string("%TAG !invalid!tag bar"),
            std::string("%TAG !invalid"));

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("invalid tag prefix") {
        auto input = GENERATE(
            std::string("%TAG ! [invalid"),
            std::string("%TAG !! ]invalid"),
            std::string("%TAG !valid! {invalid"),
            std::string("%TAG !valid! }invalid"),
            std::string("%TAG !valid! ,invalid"),
            std::string("%TAG !valid! %prefix"));

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_InvalidDirective") {
    auto buffer = GENERATE(std::string("%TAG"), std::string("%YAML"));

    std::string input_buffer {};
    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
}

TEST_CASE("LexicalAnalyzer_ReservedDirective") {
    auto buffer =
        GENERATE(std::string("%TEST\n"), std::string("%1984\n "), std::string("%TEST4LIB\n"), std::string("%%ERROR"));

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::DIRECTIVE);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_EmptyDirective") {
    std::string buffer = "%";

    std::string input_buffer {};
    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE(lexer.get_next_token().type == fkyaml::detail::lexical_token_t::DIRECTIVE);
}

TEST_CASE("LexicalAnalyzer_EndOfDirectives") {
    std::string buffer = "%YAML 1.2\n---\nfoo: bar";

    std::string input_buffer {};
    lexer_t::lexical_token token;

    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::DIRECTIVE);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_DIRECTIVES);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_EndOfDocuments") {
    std::string buffer = "%YAML 1.2\n---\n...";

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::DIRECTIVE);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_DIRECTIVES);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_DOCUMENT);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_Colon") {
    lexer_t::lexical_token token;
    std::string input_buffer {};

    SECTION("colon with half-width space") {
        std::string buffer = ": ";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with LF newline code") {
        std::string buffer = ":\n";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with the end of the buffer") {
        std::string buffer = ":";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with a comment and a LF newline code") {
        std::string buffer = ": # comment\n";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with a comment and no newline code") {
        std::string buffer = ": # comment";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with many spaces and a LF newline code") {
        std::string buffer = ":                         \n";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with many spaces and no newline code") {
        std::string buffer = ":                         ";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with an always-safe character") {
        std::string buffer = ":test";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
    }

    SECTION("colon with a flow indicator in a non-flow context") {
        auto input =
            GENERATE(std::string(":,"), std::string(":{"), std::string(":}"), std::string(":["), std::string(":]"));

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
    }

    SECTION("colon with a flow indicator in a flow context") {
        auto input = GENERATE(
            std::string("{:,"), std::string("{:{"), std::string("{:}"), std::string("{:["), std::string("{:]"));

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_MAPPING_BEGIN);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }
}

TEST_CASE("LexicalAnalzer_BlockSequenceEntryPrefix") {
    auto input = GENERATE(std::string("- foo"), std::string("-\tfoo"), std::string("-\n  foo"));

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(input);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SEQUENCE_ITEM_PREFIX);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
}

TEST_CASE("LexicalAnalyzer_Null") {
    auto buffer = GENERATE(std::string("null"), std::string("Null"), std::string("NULL"), std::string("~"));

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
}

TEST_CASE("LexicalAnalyzer_BooleanTrue") {
    auto buffer = GENERATE(std::string("true"), std::string("True"), std::string("TRUE"));

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
}

TEST_CASE("LexicalAnalyzer_BooleanFalse") {
    auto buffer = GENERATE(std::string("false"), std::string("False"), std::string("FALSE"));

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
}

TEST_CASE("LexicalAnalyzer_Integer") {
    using value_pair_t = std::pair<std::string, fkyaml::node::integer_type>;
    auto value_pair = GENERATE(
        value_pair_t(std::string("-1234"), -1234),
        value_pair_t(std::string("-853255"), -853255),
        value_pair_t(std::string("-1"), -1),
        value_pair_t(std::string("0"), 0),
        value_pair_t(std::string("643"), 643),
        value_pair_t(std::string("+123"), 123));

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(value_pair.first);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
}

TEST_CASE("LexicalAnalyzer_OctalInteger") {
    using value_pair_t = std::pair<std::string, fkyaml::node::integer_type>;
    auto value_pair = GENERATE(
        value_pair_t(std::string("0o27"), 027),
        value_pair_t(std::string("0o5"), 05),
        value_pair_t(std::string("0o77772"), 077772),
        value_pair_t(std::string("0o672"), 0672));

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(value_pair.first);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
}

TEST_CASE("LexicalAnalyzer_HexadecimalInteger") {
    using value_pair_t = std::pair<std::string, fkyaml::node::integer_type>;
    auto value_pair = GENERATE(
        value_pair_t(std::string("0xA04F"), 0xA04F),
        value_pair_t(std::string("0xa7F3"), 0xa7F3),
        value_pair_t(std::string("0xFf29Bc"), 0xFf29Bc));

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(value_pair.first);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
}

TEST_CASE("LexicalAnalyzer_FloatingPointNumber") {
    lexer_t::lexical_token token;
    std::string input_buffer {};

    SECTION("valid floating point number scalar token") {
        using value_pair_t = std::pair<std::string, fkyaml::node::float_number_type>;
        auto value_pair = GENERATE(
            value_pair_t(std::string("-1.234"), -1.234),
            value_pair_t(std::string("567.8"), 567.8),
            value_pair_t(std::string("0.24"), 0.24),
            value_pair_t(std::string("9.8e-3"), 9.8e-3),
            value_pair_t(std::string("3.95E3"), 3.95e3),
            value_pair_t(std::string("1.863e+3"), 1.863e+3));

        auto input_adapter = fkyaml::detail::input_adapter(value_pair.first);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
    }

    SECTION("valid floating point number scalar token edge cases") {
        auto input = GENERATE(std::string("0."), std::string("1.23e"), std::string("1.2e-z"));

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE_FALSE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
    }
}

TEST_CASE("LexicalAnalyzer_Infinity") {
    auto buffer = GENERATE(
        std::string(".inf"),
        std::string(".Inf"),
        std::string(".INF"),
        std::string("-.inf"),
        std::string("-.Inf"),
        std::string("-.INF"));

    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE(lexer.get_next_token().type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
}

TEST_CASE("LexicalAnalyzer_NaN") {
    auto buffer = GENERATE(std::string(".nan"), std::string(".NaN"), std::string(".NAN"));

    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE(lexer.get_next_token().type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
}

TEST_CASE("LexicalAnalyzer_PlainString") {
    using value_pair_t = std::pair<std::string, fkyaml::node::string_type>;
    auto value_pair = GENERATE(
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
        value_pair_t(std::string("?"), fkyaml::node::string_type("?")),
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
        value_pair_t(std::string("foo\\bar"), fkyaml::node::string_type("foo\\bar")),
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
        value_pair_t(std::string(".NAN_VALUE"), fkyaml::node::string_type(".NAN_VALUE")));

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(value_pair.first);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
}

TEST_CASE("LexicalAnalyzer_SingleQuotedString") {
    using value_pair_t = std::pair<std::string, fkyaml::node::string_type>;
    auto value_pair = GENERATE(
        value_pair_t(std::string("\'\'"), fkyaml::node::string_type("")),
        value_pair_t(std::string("\'foo\"bar\'"), fkyaml::node::string_type("foo\"bar")),
        value_pair_t(std::string("\'foo bar\'"), fkyaml::node::string_type("foo bar")),
        value_pair_t(std::string("\'foo\'\'bar\'"), fkyaml::node::string_type("foo\'bar")),
        value_pair_t(std::string("\'foo,bar\'"), fkyaml::node::string_type("foo,bar")),
        value_pair_t(std::string("\'foo]bar\'"), fkyaml::node::string_type("foo]bar")),
        value_pair_t(std::string("\'foo}bar\'"), fkyaml::node::string_type("foo}bar")),
        value_pair_t(std::string("\'foo\"bar\'"), fkyaml::node::string_type("foo\"bar")),
        value_pair_t(std::string("\'foo:bar\'"), fkyaml::node::string_type("foo:bar")),
        value_pair_t(std::string("\'foo\\bar\'"), fkyaml::node::string_type("foo\\bar")),

        value_pair_t(std::string("\'foo\nbar\'"), fkyaml::node::string_type("foo bar")),
        value_pair_t(std::string("\'foo \t\n \tbar\'"), fkyaml::node::string_type("foo bar")),
        value_pair_t(std::string("\'foo\n\n \t\nbar\'"), fkyaml::node::string_type("foo\n\nbar")),
        value_pair_t(std::string("\'\nfoo\n\n \t\nbar\'"), fkyaml::node::string_type(" foo\n\nbar")),
        value_pair_t(std::string("\'foo\nbar\n\'"), fkyaml::node::string_type("foo bar ")));

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(value_pair.first);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_SINGLE_QUOTED_SCALAR);
}

TEST_CASE("LexicalAnalyzer_DoubleQuotedString") {
    auto input = GENERATE(
        std::string("\"\""),
        std::string("\"foo bar\""),
        std::string("\"foo\tbar\""),
        std::string("\"foo's bar\""),
        std::string("\"foo:bar\""),
        std::string("\"foo,bar\""),
        std::string("\"foo]bar\""),
        std::string("\"foo}bar\""),
        std::string("\"\\x30\\x2B\\x6d\""),

        std::string("\"foo\nbar\""),
        std::string("\"foo \t\n \tbar\""),
        std::string("\"foo\n\n \t\nbar\""),
        std::string("\"\nfoo\n\n \t\nbar\""),
        std::string("\"foo\nbar\n\""),
        std::string("\"foo\\\nbar\""),
        std::string("\"foo \t\\\nbar\""),
        std::string("\"\\\n  foo \t\\\n\tbar\t  \t\\\n\""));

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(input);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_DOUBLE_QUOTED_SCALAR);
    REQUIRE(token.begin_itr == input.begin());
    REQUIRE(token.end_itr == input.end());
}

TEST_CASE("LexicalAnalyzer_MultiByteCharString") {
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

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(mb_char);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
    REQUIRE(token.begin_itr == mb_char.begin());
    REQUIRE(token.end_itr == mb_char.end());
}

TEST_CASE("LexicalAnalyzer_EscapedUnicodeCharacter") {
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

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(value_pair.first);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_DOUBLE_QUOTED_SCALAR);
}

TEST_CASE("LexicalAnalyzer_InvalidString") {
    std::string input_buffer {};

    SECTION("parse error") {
        auto buffer = GENERATE(
            std::string("\"test"),
            std::string("\'test"),
            std::string("\"\\xw\""),
            std::string("\"\\x+\""),
            std::string("\"\\x=\""),
            std::string("\"\\x^\""),
            std::string("\"\\x{\""),
            std::string("\"\\Q\""));

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("invalid encoding") {
        std::string buffer = "\"\\U00110000\"";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::invalid_encoding);
    }
}

// TEST_CASE("LexicalAnalyzer_InvalidMultiByteCharString") {
//     using char_traits_t = std::char_traits<char>;
//     auto mb_char = GENERATE(
//         std::string {char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0x80)},
//         std::string {char_traits_t::to_char_type(0xC1), char_traits_t::to_char_type(0x80)},
//         std::string {char_traits_t::to_char_type(0xC2), char_traits_t::to_char_type(0x7F)},
//         std::string {char_traits_t::to_char_type(0xC2), char_traits_t::to_char_type(0xC0)},
//         std::string {
//             char_traits_t::to_char_type(0xE0), char_traits_t::to_char_type(0x7F), char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xE0), char_traits_t::to_char_type(0xC0), char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xE0), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0x7F)},
//         std::string {
//             char_traits_t::to_char_type(0xE0), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0xC0)},
//         std::string {
//             char_traits_t::to_char_type(0xED), char_traits_t::to_char_type(0x7F), char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xED), char_traits_t::to_char_type(0xA0), char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xED), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0x7F)},
//         std::string {
//             char_traits_t::to_char_type(0xED), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0xC0)},
//         std::string {
//             char_traits_t::to_char_type(0xEE), char_traits_t::to_char_type(0x7F), char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xEE), char_traits_t::to_char_type(0xC0), char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xEF), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0x7F)},
//         std::string {
//             char_traits_t::to_char_type(0xEF), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0xC0)},
//         std::string {
//             char_traits_t::to_char_type(0xF0),
//             char_traits_t::to_char_type(0x8F),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xF0),
//             char_traits_t::to_char_type(0xC0),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xF0),
//             char_traits_t::to_char_type(0x90),
//             char_traits_t::to_char_type(0x7F),
//             char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xF0),
//             char_traits_t::to_char_type(0x90),
//             char_traits_t::to_char_type(0xC0),
//             char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xF0),
//             char_traits_t::to_char_type(0x90),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x7F)},
//         std::string {
//             char_traits_t::to_char_type(0xF0),
//             char_traits_t::to_char_type(0x90),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0xC0)},
//         std::string {
//             char_traits_t::to_char_type(0xF1),
//             char_traits_t::to_char_type(0x7F),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xF1),
//             char_traits_t::to_char_type(0xC0),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xF1),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x7F),
//             char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xF1),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0xC0),
//             char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xF1),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x7F)},
//         std::string {
//             char_traits_t::to_char_type(0xF1),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0xC0)},
//         std::string {
//             char_traits_t::to_char_type(0xF4),
//             char_traits_t::to_char_type(0x7F),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xF4),
//             char_traits_t::to_char_type(0x90),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xF4),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x7F),
//             char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xF4),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0xC0),
//             char_traits_t::to_char_type(0x80)},
//         std::string {
//             char_traits_t::to_char_type(0xF4),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x7F)},
//         std::string {
//             char_traits_t::to_char_type(0xF4),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0xC0)},
//         std::string {
//             char_traits_t::to_char_type(0xF5),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x80),
//             char_traits_t::to_char_type(0x80)});

//     auto input_adapter = fkyaml::detail::input_adapter(mb_char);
//     REQUIRE_THROWS_AS(lexer_t(std::move(input_adapter)), fkyaml::invalid_encoding);
// }

TEST_CASE("LexicalAnalyzer_UnescapedControlCharacter") {
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

    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
}

TEST_CASE("LexicalAnalyzer_LiteralStringScalar") {
    lexer_t::lexical_token token;
    std::string input_buffer {};

    SECTION("empty literal string scalar with strip chomping") {
        const char input[] = "|-\n"
                             "  \n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("empty literal string scalar with clip chomping") {
        const char input[] = "|\n"
                             "  \n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("empty literal string scalar with keep chomping") {
        const char input[] = "|+\n"
                             "  \n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("literal string scalar with 0 indent level.") {
        const char input[] = "|0\n"
                             "foo";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("less indented literal string scalar") {
        const char input[] = "|2\n"
                             " foo";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("literal scalar with the first line being more indented than the indicated level") {
        const char input[] = "|2\n"
                             "    foo\n"
                             "  bar\n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("literal string scalar") {
        const char input[] = "|\n"
                             "  foo\n"
                             "  bar\n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("literal string scalar with implicit indentation and strip chomping") {
        const char input[] = "|-\n"
                             "\n"
                             "  foo\n"
                             "  bar\n"
                             "\n"
                             "  baz\n"
                             "\n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("literal string scalar with explicit indentation and strip chomping") {
        const char input[] = "|-2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz\n"
                             "\n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("literal string scalar with implicit indentation and clip chomping") {
        const char input[] = "|\n"
                             "\n"
                             "  foo\n"
                             "  bar\n"
                             "\n"
                             "  baz\n"
                             "\n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("literal string scalar with explicit indentation and clip chomping") {
        const char input[] = "|2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz\n"
                             "\n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("literal string scalar with clip chomping and no trailing newlines") {
        const char input[] = "|2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("literal string scalar with implicit indentation and keep chomping") {
        const char input[] = "|+\n"
                             "\n"
                             "  foo\n"
                             "  bar\n"
                             "\n"
                             "  baz\n"
                             "\n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("literal string scalar with explicit indentation and keep chomping") {
        const char input[] = "|+2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz\n"
                             "\n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }
}

TEST_CASE("LexicalAnalyzer_FoldedString") {
    lexer_t::lexical_token token;
    std::string input_buffer {};

    SECTION("empty folded string scalar with strip chomping") {
        const char input[] = ">-\n"
                             "  \n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("empty folded string scalar with clip chomping") {
        const char input[] = ">\n"
                             "  \n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("empty folded string scalar with keep chomping") {
        const char input[] = ">+\n"
                             "  \n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("folded string scalar with 0 indent level") {
        const char input[] = "|0\n"
                             "foo";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("less indented folded string scalar") {
        const char input[] = ">2\n"
                             " foo";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("folded string scalar with the first line being more indented than the indicated level") {
        const char input[] = ">2\n"
                             "    foo\n"
                             "  bar\n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("folded string scalar with the non-first line being more indented than the indicated level") {
        const char input[] = ">2\n"
                             "  foo\n"
                             "    bar\n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("folded string scalar") {
        const char input[] = ">\n"
                             "  foo\n"
                             "  \n"
                             "\n"
                             "  bar\n"
                             " \n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("folded string scalar with implicit indentation and strip chomping") {
        const char input[] = ">-\n"
                             "  foo\n"
                             "  bar\n"
                             " \n"
                             "\n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("folded string scalar with implicit indentation and clip chomping") {
        const char input[] = ">\n"
                             "  foo\n"
                             "  bar\n"
                             "  \n"
                             "\n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }

    SECTION("folded string scalar with implicit indentation and keep chomping") {
        const char input[] = ">+\n"
                             "  foo\n"
                             "  bar\n"
                             " \n"
                             "\n";

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
    }
}

TEST_CASE("LexicalAnalyzer_Anchor") {
    lexer_t::lexical_token token;
    std::string input_buffer {};

    SECTION("valid anchor name") {
        using test_data_t = std::pair<std::string, std::string>;
        auto test_data = GENERATE(
            test_data_t {"&anchor", "anchor"},
            test_data_t {"&anchor name", "anchor"},
            test_data_t {"&anchor\tname", "anchor"},
            test_data_t {"&anchor\nname", "anchor"},
            test_data_t {"&anchor{name", "anchor"},
            test_data_t {"&anchor}name", "anchor"},
            test_data_t {"&anchor[name", "anchor"},
            test_data_t {"&anchor]name", "anchor"},
            test_data_t {"&anchor,name", "anchor"},
            test_data_t {"&anchor: ", "anchor:"},
            test_data_t {"&anchor:", "anchor:"});

        auto input_adapter = fkyaml::detail::input_adapter(test_data.first);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::ANCHOR);
    }

    SECTION("invalid anchor name") {
        auto input = GENERATE(
            std::string("&"),
            std::string("& "),
            std::string("&\t"),
            std::string("&\n"),
            std::string("&{"),
            std::string("&}"),
            std::string("&["),
            std::string("&]"),
            std::string("&,"));

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_Alias") {
    lexer_t::lexical_token token;
    std::string input_buffer {};

    SECTION("valid anchor name") {
        using test_data_t = std::pair<std::string, std::string>;
        auto test_data = GENERATE(
            test_data_t {"*anchor", "anchor"},
            test_data_t {"*anchor name", "anchor"},
            test_data_t {"*anchor\tname", "anchor"},
            test_data_t {"*anchor\nname", "anchor"},
            test_data_t {"*anchor{name", "anchor"},
            test_data_t {"*anchor}name", "anchor"},
            test_data_t {"*anchor[name", "anchor"},
            test_data_t {"*anchor]name", "anchor"},
            test_data_t {"*anchor,name", "anchor"},
            test_data_t {"*anchor: ", "anchor:"},
            test_data_t {"*anchor:", "anchor:"});

        auto input_adapter = fkyaml::detail::input_adapter(test_data.first);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::ALIAS);
    }

    SECTION("invalid anchor name") {
        auto input = GENERATE(
            std::string("*"),
            std::string("* "),
            std::string("*\t"),
            std::string("*\n"),
            std::string("*{"),
            std::string("*}"),
            std::string("*["),
            std::string("*]"),
            std::string("*,"));

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_Tag") {
    lexer_t::lexical_token token;
    std::string input_buffer {};

    SECTION("valid tag names") {
        auto input = GENERATE(
            std::string("! tag"),
            std::string("!\ntag"),
            std::string("!local tag"),
            std::string("!local%2A%7C tag"),
            std::string("!!foo tag"),
            std::string("!!foo%2A%7C tag"),
            std::string("!<tag:foo.bar> tag"),
            std::string("!<tag:foo.%2A%7C.bar> tag"),
            std::string("!<!foo> tag"),
            std::string("!<!foo%2A%7C> tag"),
            std::string("!foo!bar tag"));

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);
    }

    SECTION("valid tag name (not followed by a value)") {
        auto input = GENERATE(std::string("!"), std::string("!!foo"), std::string("!foo!bar"), std::string("!foo"));

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG);
    }

    SECTION("invalid tag names") {
        auto input = GENERATE(
            std::string("!!f!oo tag"),
            std::string("!<!f!oo> tag"),
            std::string("!<!foo tag"),
            std::string("!<> tag"),
            std::string("!<%f:oo> tag"),
            std::string("!<!%f:oo> tag"),
            std::string("!foo! tag"),
            std::string("!foo!%f:oo tag"));

        auto input_adapter = fkyaml::detail::input_adapter(input);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_THROWS_AS(token = lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_ReservedIndicator") {
    auto buffer = GENERATE(std::string("@invalid"), std::string("`invalid"));

    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
}

TEST_CASE("LexicalAnalyzer_KeyBooleanValuePair") {
    std::string buffer = "test: true";

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_KeyIntegerValuePair") {
    std::string buffer = "test: -5784";

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_KeyFloatNumberValuePair") {
    std::string buffer = "test: -5.58e-3";

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_KeyStringValuePair") {
    std::string buffer = "test: \"some value\"";

    lexer_t::lexical_token token;
    std::string input_buffer {};

    auto input_adapter = fkyaml::detail::input_adapter(buffer);
    input_adapter.fill_buffer(input_buffer);
    lexer_t lexer(input_buffer);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_DOUBLE_QUOTED_SCALAR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_FlowSequence") {
    lexer_t::lexical_token token;
    std::string input_buffer {};

    SECTION("simple flow sequence") {
        std::string buffer = "test: [ foo, bar ]";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_SEQUENCE_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_SEQUENCE_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("flow sequence with flow mapping child nodes") {
        std::string buffer = "test: [ { foo: one, bar: false }, { foo: two, bar: true } ]";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_SEQUENCE_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_MAPPING_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_MAPPING_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_MAPPING_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_MAPPING_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_SEQUENCE_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}

TEST_CASE("LexicalAnalyzer_FlowMapping") {
    lexer_t::lexical_token token;
    std::string input_buffer {};

    SECTION("simple flow mapping") {
        std::string buffer = "test: { bool: true, foo: bar, pi: 3.14 }";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_MAPPING_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_MAPPING_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("flow maping with a child mapping node") {
        std::string buffer = "test: {foo: bar baz}";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_MAPPING_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_MAPPING_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}

TEST_CASE("LexicalAnalyzer_BlockSequence") {
    lexer_t::lexical_token token;
    std::string input_buffer {};

    SECTION("simple block sequence") {
        std::string buffer = "test:\n  - foo\n  - bar";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SEQUENCE_ITEM_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SEQUENCE_ITEM_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("block sequence with block mapping child nodes") {
        std::string buffer = "test:\n  - foo: one\n    bar: false\n  - foo: two\n    bar: true";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SEQUENCE_ITEM_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SEQUENCE_ITEM_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}

TEST_CASE("LexicalAnalyzer_BlockMapping") {
    lexer_t::lexical_token token;
    std::string input_buffer {};

    SECTION("simple block mapping") {
        std::string buffer = "test:\n  bool: true\n  foo: \'bar\'\n  pi: 3.14";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_SINGLE_QUOTED_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("block mapping with a literal string scalar value") {
        std::string buffer = "test: |\n  a literal scalar.\nfoo: \'bar\'\npi: 3.14";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_SINGLE_QUOTED_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("block mapping with a folded string scalar value") {
        std::string buffer = "test: >\n  a literal scalar.\nfoo: \'bar\'\npi: 3.14";

        auto input_adapter = fkyaml::detail::input_adapter(buffer);
        input_adapter.fill_buffer(input_buffer);
        lexer_t lexer(input_buffer);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_SINGLE_QUOTED_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::FLOW_PLAIN_SCALAR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}
