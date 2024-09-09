//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

using lexer_t = fkyaml::detail::lexical_analyzer<fkyaml::node>;

TEST_CASE("LexicalAnalyzer_YamlVersionDirective") {
    fkyaml::detail::lexical_token token;

    SECTION("valid YAML directive") {
        using value_pair_t = std::pair<fkyaml::detail::str_view, std::string>;
        auto value_pair = GENERATE(
            value_pair_t("%YAML 1.1 ", std::string("1.1")),
            value_pair_t("%YAML\t1.1\t", std::string("1.1")),
            value_pair_t("%YAML 1.1\n", std::string("1.1")),
            value_pair_t("%YAML 1.1", std::string("1.1")),
            value_pair_t("%YAML 1.2 ", std::string("1.2")),
            value_pair_t("%YAML\t1.2\t", std::string("1.2")),
            value_pair_t("%YAML 1.2\n", std::string("1.2")),
            value_pair_t("%YAML 1.2", std::string("1.2")));

        lexer_t lexer(value_pair.first);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::YAML_VER_DIRECTIVE);
        REQUIRE(lexer.get_yaml_version() == value_pair.second);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("wrong YAML directive") {
        auto buffer = GENERATE(
            fkyaml::detail::str_view("%YUML 1.2"),
            fkyaml::detail::str_view("%YANL 1.2    \n"),
            fkyaml::detail::str_view("%YAML1.2"));

        lexer_t lexer(buffer);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::INVALID_DIRECTIVE);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("invalid YAML directive value") {
        auto buffer = GENERATE(
            fkyaml::detail::str_view("%YAML 1.3\n"),
            fkyaml::detail::str_view("%YAML 2.0\n"),
            fkyaml::detail::str_view("%YAML 12"),
            fkyaml::detail::str_view("%YAML 123"),
            fkyaml::detail::str_view("%YAML 1.23"),
            fkyaml::detail::str_view("%YAML 1.11"),
            fkyaml::detail::str_view("%YAML 1.A"),
            fkyaml::detail::str_view("%YAML AbC"));

        lexer_t lexer(buffer);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_TagDirective") {
    fkyaml::detail::lexical_token token;

    SECTION("primary tag handle") {
        auto input = GENERATE(fkyaml::detail::str_view("%TAG ! foo"), fkyaml::detail::str_view("%TAG\t!\tfoo"));
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG_DIRECTIVE);
        REQUIRE(lexer.get_tag_handle() == "!");
        REQUIRE(lexer.get_tag_prefix() == "foo");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("secondary tag handle") {
        auto input = GENERATE(fkyaml::detail::str_view("%TAG !! foo"), fkyaml::detail::str_view("%TAG\t!!\tfoo"));
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG_DIRECTIVE);
        REQUIRE(lexer.get_tag_handle() == "!!");
        REQUIRE(lexer.get_tag_prefix() == "foo");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("named tag handle") {
        auto input = GENERATE(
            fkyaml::detail::str_view("%TAG !va1id-ta9! foo"), fkyaml::detail::str_view("%TAG\t!va1id-ta9!\tfoo"));
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG_DIRECTIVE);
        REQUIRE(lexer.get_tag_handle() == "!va1id-ta9!");
        REQUIRE(lexer.get_tag_prefix() == "foo");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("invalid TAG directive") {
        auto buffer = GENERATE(
            fkyaml::detail::str_view("%TUB"), fkyaml::detail::str_view("%TAC"), fkyaml::detail::str_view("%TAGE"));

        lexer_t lexer(buffer);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::INVALID_DIRECTIVE);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("invalid tag handle") {
        auto input = GENERATE(
            fkyaml::detail::str_view("%TAG foo bar"),
            fkyaml::detail::str_view("%TAG !!abc bar"),
            fkyaml::detail::str_view("%TAG !"),
            fkyaml::detail::str_view("%TAG !!"),
            fkyaml::detail::str_view("%TAG !valid!"),
            fkyaml::detail::str_view("%TAG !invalid"),
            fkyaml::detail::str_view("%TAG !invalid bar"),
            fkyaml::detail::str_view("%TAG !invalid\tbar"),
            fkyaml::detail::str_view("%TAG !inv@lid! bar"),
            fkyaml::detail::str_view("%TAG !invalid!tag bar"),
            fkyaml::detail::str_view("%TAG !invalid"));

        lexer_t lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("invalid tag prefix") {
        auto input = GENERATE(
            fkyaml::detail::str_view("%TAG ! [invalid"),
            fkyaml::detail::str_view("%TAG !! ]invalid"),
            fkyaml::detail::str_view("%TAG !valid! {invalid"),
            fkyaml::detail::str_view("%TAG !valid! }invalid"),
            fkyaml::detail::str_view("%TAG !valid! ,invalid"),
            fkyaml::detail::str_view("%TAG !valid! %prefix"));

        lexer_t lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_InvalidDirective") {
    auto buffer = GENERATE(fkyaml::detail::str_view("%TAG"), fkyaml::detail::str_view("%YAML"));

    lexer_t lexer(buffer);
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
}

TEST_CASE("LexicalAnalyzer_ReservedDirective") {
    auto buffer = GENERATE(
        fkyaml::detail::str_view("%TEST\n"),
        fkyaml::detail::str_view("%1984\n "),
        fkyaml::detail::str_view("%TEST4LIB\n"),
        fkyaml::detail::str_view("%%ERROR"));

    fkyaml::detail::lexical_token token;
    lexer_t lexer(buffer);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::INVALID_DIRECTIVE);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_EmptyDirective") {
    lexer_t lexer("%");
    REQUIRE(lexer.get_next_token().type == fkyaml::detail::lexical_token_t::INVALID_DIRECTIVE);
}

TEST_CASE("LexicalAnalyzer_EndOfDirectives") {
    lexer_t lexer("%YAML 1.2\n---\nfoo: bar");
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::YAML_VER_DIRECTIVE);
    REQUIRE(lexer.get_yaml_version() == "1.2");
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_DIRECTIVES);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo");
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "bar");
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_EndOfDocuments") {
    lexer_t lexer("%YAML 1.2\n---\n...");
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::YAML_VER_DIRECTIVE);
    REQUIRE(lexer.get_yaml_version() == "1.2");
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_DIRECTIVES);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_DOCUMENT);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_Colon") {
    fkyaml::detail::lexical_token token;

    SECTION("colon with half-width space") {
        lexer_t lexer(": ");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with LF newline code") {
        lexer_t lexer(":\n");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with the end of the buffer") {
        lexer_t lexer(":");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with a comment and a LF newline code") {
        lexer_t lexer(": # comment\n");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with a comment and no newline code") {
        lexer_t lexer(": # comment");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with many spaces and a LF newline code") {
        lexer_t lexer(":                         \n");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with many spaces and no newline code") {
        lexer_t lexer(":                         ");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with an always-safe character") {
        lexer_t lexer(":test");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == ":test");
    }

    SECTION("colon with a flow indicator in a non-flow context") {
        auto input = GENERATE(
            fkyaml::detail::str_view(":,"),
            fkyaml::detail::str_view(":{"),
            fkyaml::detail::str_view(":}"),
            fkyaml::detail::str_view(":["),
            fkyaml::detail::str_view(":]"));
        lexer_t lexer(input);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        fkyaml::detail::str_view token_str(token.token_begin_itr, token.token_end_itr);
        REQUIRE(token_str == input);
    }

    SECTION("colon with a flow indicator in a flow context") {
        auto input = GENERATE(
            fkyaml::detail::str_view("{:,"),
            fkyaml::detail::str_view("{:{"),
            fkyaml::detail::str_view("{:}"),
            fkyaml::detail::str_view("{:["),
            fkyaml::detail::str_view("{:]"));
        lexer_t lexer(input);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }
}

TEST_CASE("LexicalAnalzer_BlockSequenceEntryPrefix") {
    auto input = GENERATE(
        fkyaml::detail::str_view("- foo"), fkyaml::detail::str_view("-\tfoo"), fkyaml::detail::str_view("-\n  foo"));

    fkyaml::detail::lexical_token token;
    lexer_t lexer(input);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    fkyaml::detail::str_view token_str(token.token_begin_itr, token.token_end_itr);
    REQUIRE(token_str == fkyaml::detail::str_view("foo"));
}

TEST_CASE("LexicalAnalyzer_PlainScalar") {
    using value_pair_t = std::pair<fkyaml::detail::str_view, fkyaml::detail::str_view>;
    auto value_pair = GENERATE(
        value_pair_t(fkyaml::detail::str_view("test"), fkyaml::detail::str_view("test")),
        value_pair_t(fkyaml::detail::str_view("nop"), fkyaml::detail::str_view("nop")),
        value_pair_t(fkyaml::detail::str_view("none"), fkyaml::detail::str_view("none")),
        value_pair_t(fkyaml::detail::str_view("?test"), fkyaml::detail::str_view("?test")),
        value_pair_t(fkyaml::detail::str_view(".NET"), fkyaml::detail::str_view(".NET")),
        value_pair_t(fkyaml::detail::str_view(".on"), fkyaml::detail::str_view(".on")),
        value_pair_t(fkyaml::detail::str_view(".n"), fkyaml::detail::str_view(".n")),
        value_pair_t(fkyaml::detail::str_view("-t"), fkyaml::detail::str_view("-t")),
        value_pair_t(fkyaml::detail::str_view("-foo"), fkyaml::detail::str_view("-foo")),
        value_pair_t(fkyaml::detail::str_view("-.test"), fkyaml::detail::str_view("-.test")),
        value_pair_t(fkyaml::detail::str_view("?"), fkyaml::detail::str_view("?")),
        value_pair_t(fkyaml::detail::str_view("--foo"), fkyaml::detail::str_view("--foo")),
        value_pair_t(fkyaml::detail::str_view("+123"), fkyaml::detail::str_view("+123")),
        value_pair_t(fkyaml::detail::str_view("1.2.3"), fkyaml::detail::str_view("1.2.3")),
        value_pair_t(fkyaml::detail::str_view("foo,bar"), fkyaml::detail::str_view("foo,bar")),
        value_pair_t(fkyaml::detail::str_view("foo[bar"), fkyaml::detail::str_view("foo[bar")),
        value_pair_t(fkyaml::detail::str_view("foo]bar"), fkyaml::detail::str_view("foo]bar")),
        value_pair_t(fkyaml::detail::str_view("foo{bar"), fkyaml::detail::str_view("foo{bar")),
        value_pair_t(fkyaml::detail::str_view("foo}bar"), fkyaml::detail::str_view("foo}bar")),
        value_pair_t(fkyaml::detail::str_view("foo:bar"), fkyaml::detail::str_view("foo:bar")),
        value_pair_t(fkyaml::detail::str_view("foo bar"), fkyaml::detail::str_view("foo bar")),
        value_pair_t(fkyaml::detail::str_view("foo\"bar"), fkyaml::detail::str_view("foo\"bar")),
        value_pair_t(fkyaml::detail::str_view("foo\'s bar"), fkyaml::detail::str_view("foo\'s bar")),
        value_pair_t(fkyaml::detail::str_view("foo\\bar"), fkyaml::detail::str_view("foo\\bar")),
        value_pair_t(fkyaml::detail::str_view("nullValue"), fkyaml::detail::str_view("nullValue")),
        value_pair_t(fkyaml::detail::str_view("NullValue"), fkyaml::detail::str_view("NullValue")),
        value_pair_t(fkyaml::detail::str_view("NULL_VALUE"), fkyaml::detail::str_view("NULL_VALUE")),
        value_pair_t(fkyaml::detail::str_view("~Value"), fkyaml::detail::str_view("~Value")),
        value_pair_t(fkyaml::detail::str_view("trueValue"), fkyaml::detail::str_view("trueValue")),
        value_pair_t(fkyaml::detail::str_view("TrueValue"), fkyaml::detail::str_view("TrueValue")),
        value_pair_t(fkyaml::detail::str_view("TRUE_VALUE"), fkyaml::detail::str_view("TRUE_VALUE")),
        value_pair_t(fkyaml::detail::str_view("falseValue"), fkyaml::detail::str_view("falseValue")),
        value_pair_t(fkyaml::detail::str_view("FalseValue"), fkyaml::detail::str_view("FalseValue")),
        value_pair_t(fkyaml::detail::str_view("FALSE_VALUE"), fkyaml::detail::str_view("FALSE_VALUE")),
        value_pair_t(fkyaml::detail::str_view(".infValue"), fkyaml::detail::str_view(".infValue")),
        value_pair_t(fkyaml::detail::str_view(".InfValue"), fkyaml::detail::str_view(".InfValue")),
        value_pair_t(fkyaml::detail::str_view(".INF_VALUE"), fkyaml::detail::str_view(".INF_VALUE")),
        value_pair_t(fkyaml::detail::str_view("-.infValue"), fkyaml::detail::str_view("-.infValue")),
        value_pair_t(fkyaml::detail::str_view("-.InfValue"), fkyaml::detail::str_view("-.InfValue")),
        value_pair_t(fkyaml::detail::str_view("-.INF_VALUE"), fkyaml::detail::str_view("-.INF_VALUE")),
        value_pair_t(fkyaml::detail::str_view(".nanValue"), fkyaml::detail::str_view(".nanValue")),
        value_pair_t(fkyaml::detail::str_view(".NaNValue"), fkyaml::detail::str_view(".NaNValue")),
        value_pair_t(fkyaml::detail::str_view(".NAN_VALUE"), fkyaml::detail::str_view(".NAN_VALUE")));

    lexer_t lexer(value_pair.first);
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    fkyaml::detail::str_view token_str(token.token_begin_itr, token.token_end_itr);
    REQUIRE(token_str == value_pair.second);
}

TEST_CASE("LexicalAnalyzer_SingleQuotedScalar") {
    using value_pair_t = std::pair<fkyaml::detail::str_view, fkyaml::detail::str_view>;
    auto value_pair = GENERATE(
        value_pair_t(fkyaml::detail::str_view("\'\'"), fkyaml::detail::str_view("")),
        value_pair_t(fkyaml::detail::str_view("\'foo\"bar\'"), fkyaml::detail::str_view("foo\"bar")),
        value_pair_t(fkyaml::detail::str_view("\'foo bar\'"), fkyaml::detail::str_view("foo bar")),
        value_pair_t(fkyaml::detail::str_view("\'foo\'\'bar\'"), fkyaml::detail::str_view("foo\'bar")),
        value_pair_t(fkyaml::detail::str_view("\'foo\'\'bar\' "), fkyaml::detail::str_view("foo\'bar")),
        value_pair_t(fkyaml::detail::str_view("\'foo,bar\'"), fkyaml::detail::str_view("foo,bar")),
        value_pair_t(fkyaml::detail::str_view("\'foo]bar\'"), fkyaml::detail::str_view("foo]bar")),
        value_pair_t(fkyaml::detail::str_view("\'foo}bar\'"), fkyaml::detail::str_view("foo}bar")),
        value_pair_t(fkyaml::detail::str_view("\'foo\"bar\'"), fkyaml::detail::str_view("foo\"bar")),
        value_pair_t(fkyaml::detail::str_view("\'foo:bar\'"), fkyaml::detail::str_view("foo:bar")),
        value_pair_t(fkyaml::detail::str_view("\'foo\\bar\'"), fkyaml::detail::str_view("foo\\bar")),

        value_pair_t(fkyaml::detail::str_view("\'foo\nbar\'"), fkyaml::detail::str_view("foo bar")),
        value_pair_t(fkyaml::detail::str_view("\'foo \t\n \tbar\'"), fkyaml::detail::str_view("foo bar")),
        value_pair_t(fkyaml::detail::str_view("\'foo\n\n \t\nbar\'"), fkyaml::detail::str_view("foo\n\nbar")),
        value_pair_t(fkyaml::detail::str_view("\'\nfoo\n\n \t\nbar\'"), fkyaml::detail::str_view(" foo\n\nbar")),
        value_pair_t(fkyaml::detail::str_view("\'foo\nbar\n\'"), fkyaml::detail::str_view("foo bar ")));

    lexer_t lexer(value_pair.first);
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::SINGLE_QUOTED_SCALAR);
    fkyaml::detail::str_view token_str(token.token_begin_itr, token.token_end_itr);
    REQUIRE(token_str == value_pair.second);
}

TEST_CASE("LexicalAnalyzer_DoubleQuotedScalar") {
    using value_pair_t = std::pair<fkyaml::detail::str_view, fkyaml::detail::str_view>;
    auto value_pair = GENERATE(
        value_pair_t(fkyaml::detail::str_view("\"\""), fkyaml::detail::str_view("")),
        value_pair_t(fkyaml::detail::str_view("\"foo bar\""), fkyaml::detail::str_view("foo bar")),
        value_pair_t(fkyaml::detail::str_view("\"foo\tbar\""), fkyaml::detail::str_view("foo\tbar")),
        value_pair_t(fkyaml::detail::str_view("\"foo's bar\""), fkyaml::detail::str_view("foo's bar")),
        value_pair_t(fkyaml::detail::str_view("\"foo:bar\""), fkyaml::detail::str_view("foo:bar")),
        value_pair_t(fkyaml::detail::str_view("\"foo,bar\""), fkyaml::detail::str_view("foo,bar")),
        value_pair_t(fkyaml::detail::str_view("\"foo]bar\""), fkyaml::detail::str_view("foo]bar")),
        value_pair_t(fkyaml::detail::str_view("\"foo}bar\""), fkyaml::detail::str_view("foo}bar")),
        value_pair_t(fkyaml::detail::str_view("\"\\x30\\x2B\\x6d\""), fkyaml::detail::str_view("0+m")),

        value_pair_t(fkyaml::detail::str_view("\"foo\nbar\""), fkyaml::detail::str_view("foo bar")),
        value_pair_t(fkyaml::detail::str_view("\"foo \t\n \tbar\""), fkyaml::detail::str_view("foo bar")),
        value_pair_t(fkyaml::detail::str_view("\"foo\n\n \t\nbar\""), fkyaml::detail::str_view("foo\n\nbar")),
        value_pair_t(fkyaml::detail::str_view("\"\nfoo\n\n \t\nbar\""), fkyaml::detail::str_view(" foo\n\nbar")),
        value_pair_t(fkyaml::detail::str_view("\"foo\nbar\n\""), fkyaml::detail::str_view("foo bar ")),
        value_pair_t(fkyaml::detail::str_view("\"foo\\\nbar\""), fkyaml::detail::str_view("foobar")),
        value_pair_t(fkyaml::detail::str_view("\"foo \t\\\nbar\""), fkyaml::detail::str_view("foo \tbar")),
        value_pair_t(
            fkyaml::detail::str_view("\"\\\n  foo \t\\\n\tbar\t  \t\\\n\""),
            fkyaml::detail::str_view("foo \tbar\t  \t")));

    lexer_t lexer(value_pair.first);
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR);
    fkyaml::detail::str_view token_str(token.token_begin_itr, token.token_end_itr);
    REQUIRE(token_str == value_pair.second);
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

    fkyaml::detail::str_view input(mb_char);
    lexer_t lexer(input);
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    fkyaml::detail::str_view token_str(token.token_begin_itr, token.token_end_itr);
    REQUIRE(token_str == input);
}

TEST_CASE("LexicalAnalyzer_EscapedUnicodeCharacter") {
    using value_pair_t = std::pair<fkyaml::detail::str_view, std::string>;
    using char_traits_t = std::char_traits<char>;
    auto value_pair = GENERATE(
        value_pair_t(fkyaml::detail::str_view("\"\\x00\""), std::string {char_traits_t::to_char_type(0x00)}),
        value_pair_t(fkyaml::detail::str_view("\"\\x40\""), std::string {char_traits_t::to_char_type(0x40)}),
        value_pair_t(fkyaml::detail::str_view("\"\\x7F\""), std::string {char_traits_t::to_char_type(0x7F)}),
        value_pair_t(fkyaml::detail::str_view("\"\\u0000\""), std::string {char_traits_t::to_char_type(0x00)}),
        value_pair_t(fkyaml::detail::str_view("\"\\u0040\""), std::string {char_traits_t::to_char_type(0x40)}),
        value_pair_t(fkyaml::detail::str_view("\"\\u007F\""), std::string {char_traits_t::to_char_type(0x7F)}),
        value_pair_t(
            fkyaml::detail::str_view("\"\\u0080\""),
            std::string {char_traits_t::to_char_type(0xC2), char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            fkyaml::detail::str_view("\"\\u0400\""),
            std::string {char_traits_t::to_char_type(0xD0), char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            fkyaml::detail::str_view("\"\\u07FF\""),
            std::string {char_traits_t::to_char_type(0xDF), char_traits_t::to_char_type(0xBF)}),
        value_pair_t(
            fkyaml::detail::str_view("\"\\u0800\""),
            std::string {
                char_traits_t::to_char_type(0xE0),
                char_traits_t::to_char_type(0xA0),
                char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            fkyaml::detail::str_view("\"\\u8000\""),
            std::string {
                char_traits_t::to_char_type(0xE8),
                char_traits_t::to_char_type(0x80),
                char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            fkyaml::detail::str_view("\"\\uFFFF\""),
            std::string {
                char_traits_t::to_char_type(0xEF),
                char_traits_t::to_char_type(0xBF),
                char_traits_t::to_char_type(0xBF)}),
        value_pair_t(fkyaml::detail::str_view("\"\\U00000000\""), std::string {char_traits_t::to_char_type(0x00)}),
        value_pair_t(fkyaml::detail::str_view("\"\\U00000040\""), std::string {char_traits_t::to_char_type(0x40)}),
        value_pair_t(fkyaml::detail::str_view("\"\\U0000007F\""), std::string {char_traits_t::to_char_type(0x7F)}),
        value_pair_t(
            fkyaml::detail::str_view("\"\\U00000080\""),
            std::string {char_traits_t::to_char_type(0xC2), char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            fkyaml::detail::str_view("\"\\U00000400\""),
            std::string {char_traits_t::to_char_type(0xD0), char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            fkyaml::detail::str_view("\"\\U000007FF\""),
            std::string {char_traits_t::to_char_type(0xDF), char_traits_t::to_char_type(0xBF)}),
        value_pair_t(
            fkyaml::detail::str_view("\"\\U00000800\""),
            std::string {
                char_traits_t::to_char_type(0xE0),
                char_traits_t::to_char_type(0xA0),
                char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            fkyaml::detail::str_view("\"\\U00008000\""),
            std::string {
                char_traits_t::to_char_type(0xE8),
                char_traits_t::to_char_type(0x80),
                char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            fkyaml::detail::str_view("\"\\U0000FFFF\""),
            std::string {
                char_traits_t::to_char_type(0xEF),
                char_traits_t::to_char_type(0xBF),
                char_traits_t::to_char_type(0xBF)}),
        value_pair_t(
            fkyaml::detail::str_view("\"\\U00010000\""),
            std::string {
                char_traits_t::to_char_type(0xF0),
                char_traits_t::to_char_type(0x90),
                char_traits_t::to_char_type(0x80),
                char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            fkyaml::detail::str_view("\"\\U00080000\""),
            std::string {
                char_traits_t::to_char_type(0xF2),
                char_traits_t::to_char_type(0x80),
                char_traits_t::to_char_type(0x80),
                char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            fkyaml::detail::str_view("\"\\U0010FFFF\""),
            std::string {
                char_traits_t::to_char_type(0xF4),
                char_traits_t::to_char_type(0x8F),
                char_traits_t::to_char_type(0xBF),
                char_traits_t::to_char_type(0xBF)}));

    fkyaml::detail::str_view input = value_pair.first;
    lexer_t lexer(input);
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR);
    fkyaml::detail::str_view token_str(token.token_begin_itr, token.token_end_itr);
    fkyaml::detail::str_view escaped_str(value_pair.second);
    REQUIRE(token_str == escaped_str);
}

TEST_CASE("LexicalAnalyzer_InvalidString") {
    SECTION("parse error") {
        auto buffer = GENERATE(
            fkyaml::detail::str_view("\"test"),
            fkyaml::detail::str_view("\'test"),
            fkyaml::detail::str_view("\"\\xw\""),
            fkyaml::detail::str_view("\"\\x+\""),
            fkyaml::detail::str_view("\"\\x=\""),
            fkyaml::detail::str_view("\"\\x^\""),
            fkyaml::detail::str_view("\"\\x{\""),
            fkyaml::detail::str_view("\"\\Q\""));

        lexer_t lexer(buffer);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("invalid encoding") {
        lexer_t lexer("\"\\U00110000\"");
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::invalid_encoding);
    }
}

// FIXME: to be deleted
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

//     fkyaml::detail::str_view input = mb_char;
//     REQUIRE_THROWS_AS(lexer_t(input), fkyaml::invalid_encoding);
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

    lexer_t lexer(buffer);
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
}

TEST_CASE("LexicalAnalyzer_LiteralStringScalar") {
    fkyaml::detail::lexical_token token;

    SECTION("empty literal string scalar with strip chomping") {
        const char input[] = "|-\n"
                             "  \n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "");
    }

    SECTION("empty literal string scalar with clip chomping") {
        const char input[] = "|\n"
                             "  \n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "");
    }

    SECTION("empty literal string scalar with keep chomping") {
        const char input[] = "|+\n"
                             "  \n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "\n");
    }

    SECTION("literal string scalar with 0 indent level.") {
        const char input[] = "|0\n"
                             "foo";

        lexer_t lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("less indented literal string scalar") {
        const char input[] = "|2\n"
                             " foo";

        lexer_t lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("literal scalar with the first line being more indented than the indicated level") {
        const char input[] = "|2\n"
                             "    foo\n"
                             "  bar\n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "  foo\nbar\n");
    }

    SECTION("literal string scalar") {
        const char input[] = "|\n"
                             "  foo\n"
                             "  bar\n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo\nbar\n");
    }

    SECTION("literal string scalar with implicit indentation and strip chomping") {
        const char input[] = "|-\n"
                             "\n"
                             "  foo\n"
                             "  bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "\nfoo\nbar\n\nbaz");
    }

    SECTION("literal string scalar with explicit indentation and strip chomping") {
        const char input[] = "|-2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo\n  bar\n\nbaz");
    }

    SECTION("literal string scalar with implicit indentation and clip chomping") {
        const char input[] = "|\n"
                             "\n"
                             "  foo\n"
                             "  bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "\nfoo\nbar\n\nbaz\n");
    }

    SECTION("literal string scalar with explicit indentation and clip chomping") {
        const char input[] = "|2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo\n  bar\n\nbaz\n");
    }

    SECTION("literal string scalar with clip chomping and no trailing newlines") {
        const char input[] = "|2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo\n  bar\n\nbaz");
    }

    SECTION("literal string scalar with implicit indentation and keep chomping") {
        const char input[] = "|+\n"
                             "\n"
                             "  foo\n"
                             "  bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "\nfoo\nbar\n\nbaz\n\n");
    }

    SECTION("literal string scalar with explicit indentation and keep chomping") {
        const char input[] = "|+2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo\n  bar\n\nbaz\n\n");
    }

    SECTION("literal string scalar with trailing spaces/tabs after the block scalar header.") {
        auto input = GENERATE(
            fkyaml::detail::str_view("|2  \n  foo\n"),
            fkyaml::detail::str_view("|2\t\t\n  foo\n"),
            fkyaml::detail::str_view("|2 # comment\n  foo\n"));
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo\n");
    }

    SECTION("literal string scalar with invalid block scalar headers") {
        auto input = GENERATE(
            fkyaml::detail::str_view("|++2\n  foo"),
            fkyaml::detail::str_view("|--2\n  foo"),
            fkyaml::detail::str_view("|+-2\n  foo"),
            fkyaml::detail::str_view("|-+2\n  foo"),
            fkyaml::detail::str_view("|+0\n  foo"),
            fkyaml::detail::str_view("|+11\n           foo"),
            fkyaml::detail::str_view("|invalid\n  foo"));

        lexer_t lexer(input);
        REQUIRE_THROWS_AS(token = lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_FoldedString") {
    fkyaml::detail::lexical_token token;

    SECTION("empty folded string scalar with strip chomping") {
        const char input[] = ">-\n"
                             "  \n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "");
    }

    SECTION("empty folded string scalar with clip chomping") {
        const char input[] = ">\n"
                             "  \n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "");
    }

    SECTION("empty folded string scalar with keep chomping") {
        const char input[] = ">+\n"
                             "  \n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "\n");
    }

    SECTION("folded string scalar with 0 indent level") {
        const char input[] = "|0\n"
                             "foo";

        lexer_t lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("less indented folded string scalar") {
        const char input[] = ">2\n"
                             " foo";

        lexer_t lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("folded string scalar with the first line being more indented than the indicated level") {
        const char input[] = ">2\n"
                             "    foo\n"
                             "  bar\n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "\n  foo\nbar\n");
    }

    SECTION("folded string scalar with the non-first line being more indented than the indicated level") {
        const char input[] = ">2\n"
                             "  foo\n"
                             "    bar\n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo\n  bar\n");
    }

    SECTION("folded string scalar") {
        const char input[] = ">\n"
                             "  foo\n"
                             "  \n"
                             "\n"
                             "  bar\n"
                             " \n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo\n\nbar\n");
    }

    SECTION("folded string scalar with implicit indentation and strip chomping") {
        const char input[] = ">-\n"
                             "  foo\n"
                             "  bar\n"
                             " \n"
                             "\n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo bar");
    }

    SECTION("folded string scalar with implicit indentation and clip chomping") {
        const char input[] = ">\n"
                             "  foo\n"
                             "  bar\n"
                             "  \n"
                             "\n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo bar\n");
    }

    SECTION("folded string scalar with implicit indentation and keep chomping") {
        const char input[] = ">+\n"
                             "  foo\n"
                             "  bar\n"
                             " \n"
                             "\n";
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo bar\n\n");
    }

    SECTION("folded string scalar with trailing spaces/tabs/comments after the block scalar header.") {
        auto input = GENERATE(
            fkyaml::detail::str_view(">2  \n  foo\n"),
            fkyaml::detail::str_view(">2\t\t\n  foo\n"),
            fkyaml::detail::str_view(">2 # comment\n  foo\n"));
        lexer_t lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo\n");
    }

    SECTION("folded string scalar with invalid block scalar headers") {
        auto input = GENERATE(
            fkyaml::detail::str_view(">++2\n  foo"),
            fkyaml::detail::str_view(">--2\n  foo"),
            fkyaml::detail::str_view(">+-2\n  foo"),
            fkyaml::detail::str_view(">-+2\n  foo"),
            fkyaml::detail::str_view(">+0\n  foo"),
            fkyaml::detail::str_view(">+11\n           foo"),
            fkyaml::detail::str_view(">invalid\n  foo"));

        lexer_t lexer(input);
        REQUIRE_THROWS_AS(token = lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_Anchor") {
    fkyaml::detail::lexical_token token;

    SECTION("valid anchor name") {
        using test_data_t = std::pair<fkyaml::detail::str_view, fkyaml::detail::str_view>;
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

        lexer_t lexer(test_data.first);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::ANCHOR_PREFIX);
        fkyaml::detail::str_view token_str(token.token_begin_itr, token.token_end_itr);
        REQUIRE(token_str == test_data.second);
    }

    SECTION("invalid anchor name") {
        auto input = GENERATE(
            fkyaml::detail::str_view("&"),
            fkyaml::detail::str_view("& "),
            fkyaml::detail::str_view("&\t"),
            fkyaml::detail::str_view("&\n"),
            fkyaml::detail::str_view("&{"),
            fkyaml::detail::str_view("&}"),
            fkyaml::detail::str_view("&["),
            fkyaml::detail::str_view("&]"),
            fkyaml::detail::str_view("&,"));

        lexer_t lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_Alias") {
    fkyaml::detail::lexical_token token;

    SECTION("valid anchor name") {
        using test_data_t = std::pair<fkyaml::detail::str_view, fkyaml::detail::str_view>;
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

        lexer_t lexer(test_data.first);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::ALIAS_PREFIX);
        fkyaml::detail::str_view token_str(token.token_begin_itr, token.token_end_itr);
        REQUIRE_NOTHROW(token_str == test_data.second);
    }

    SECTION("invalid anchor name") {
        auto input = GENERATE(
            fkyaml::detail::str_view("*"),
            fkyaml::detail::str_view("* "),
            fkyaml::detail::str_view("*\t"),
            fkyaml::detail::str_view("*\n"),
            fkyaml::detail::str_view("*{"),
            fkyaml::detail::str_view("*}"),
            fkyaml::detail::str_view("*["),
            fkyaml::detail::str_view("*]"),
            fkyaml::detail::str_view("*,"));

        lexer_t lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_Tag") {
    fkyaml::detail::lexical_token token;

    SECTION("valid tag names") {
        auto input = GENERATE(
            fkyaml::detail::str_view("! tag"),
            fkyaml::detail::str_view("!\ntag"),
            fkyaml::detail::str_view("!local tag"),
            fkyaml::detail::str_view("!local%2A%7C tag"),
            fkyaml::detail::str_view("!!foo tag"),
            fkyaml::detail::str_view("!!foo%2A%7C tag"),
            fkyaml::detail::str_view("!<tag:foo.bar> tag"),
            fkyaml::detail::str_view("!<tag:foo.%2A%7C.bar> tag"),
            fkyaml::detail::str_view("!<!foo> tag"),
            fkyaml::detail::str_view("!<!foo%2A%7C> tag"),
            fkyaml::detail::str_view("!foo!bar tag"));

        lexer_t lexer(input);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG_PREFIX);
        REQUIRE(
            fkyaml::detail::str_view(token.token_begin_itr, token.token_end_itr) == input.substr(0, input.size() - 4));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(
            fkyaml::detail::str_view(token.token_begin_itr, token.token_end_itr) == fkyaml::detail::str_view("tag"));
    }

    SECTION("valid tag name (not followed by a value)") {
        auto input = GENERATE(
            fkyaml::detail::str_view("!"),
            fkyaml::detail::str_view("!!foo"),
            fkyaml::detail::str_view("!foo!bar"),
            fkyaml::detail::str_view("!foo"));

        lexer_t lexer(input);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG_PREFIX);
        REQUIRE(fkyaml::detail::str_view(token.token_begin_itr, token.token_end_itr) == input);
    }

    SECTION("invalid tag names") {
        auto input = GENERATE(
            fkyaml::detail::str_view("!!f!oo tag"),
            fkyaml::detail::str_view("!<!f!oo> tag"),
            fkyaml::detail::str_view("!<!foo tag"),
            fkyaml::detail::str_view("!<> tag"),
            fkyaml::detail::str_view("!<%f:oo> tag"),
            fkyaml::detail::str_view("!<!%f:oo> tag"),
            fkyaml::detail::str_view("!foo! tag"),
            fkyaml::detail::str_view("!foo!%f:oo tag"));

        lexer_t lexer(input);
        REQUIRE_THROWS_AS(token = lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_ReservedIndicator") {
    auto buffer = GENERATE(fkyaml::detail::str_view("@invalid"), fkyaml::detail::str_view("`invalid"));
    lexer_t lexer(buffer);
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
}

TEST_CASE("LexicalAnalyzer_KeyBooleanValuePair") {
    lexer_t lexer("test: true");
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "test");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "true");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_KeyIntegerValuePair") {
    lexer_t lexer("test: -5784");
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "test");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "-5784");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_KeyFloatNumberValuePair") {
    lexer_t lexer("test: -5.58e-3");
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "test");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "-5.58e-3");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_KeyStringValuePair") {
    lexer_t lexer("test: \"some value\"");
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "test");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "some value");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_FlowSequence") {
    fkyaml::detail::lexical_token token;

    SECTION("simple flow sequence") {
        lexer_t lexer("test: [ foo, bar ]");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("flow sequence with flow mapping child nodes") {
        lexer_t lexer("test: [ { foo: one, bar: false }, { foo: two, bar: true } ]");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "one");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "false");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "two");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "true");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}

TEST_CASE("LexicalAnalyzer_FlowMapping") {
    fkyaml::detail::lexical_token token;

    SECTION("simple flow mapping") {
        lexer_t lexer("test: { bool : true, foo :b: bar, pi: 3.14 }");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "bool");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "true");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo :b");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "pi");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE_NOTHROW(std::string(token.token_begin_itr, token.token_end_itr));
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("flow maping with a child mapping node") {
        lexer_t lexer("test: {foo: bar baz}");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "bar baz");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}

TEST_CASE("LexicalAnalyzer_BlockSequence") {
    fkyaml::detail::lexical_token token;

    SECTION("simple block sequence") {
        lexer_t lexer("test:\n  - foo\n  - bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("block sequence with block mapping child nodes") {
        lexer_t lexer("test:\n  - foo: one\n    bar: false\n  - foo: two\n    bar: true");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "one");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "false");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "two");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "true");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}

TEST_CASE("LexicalAnalyzer_BlockMapping") {
    fkyaml::detail::lexical_token token;

    SECTION("simple block mapping") {
        lexer_t lexer("test:\n  bool: true\n  foo: \'bar\'\n  pi: 3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "bool");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "true");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SINGLE_QUOTED_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "pi");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE_NOTHROW(std::string(token.token_begin_itr, token.token_end_itr));
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("block mapping with a literal string scalar value") {
        lexer_t lexer("test: |\n  a literal scalar.\nfoo: \'bar\'\npi: 3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "a literal scalar.\n");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SINGLE_QUOTED_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "pi");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE_NOTHROW(std::string(token.token_begin_itr, token.token_end_itr));
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("block mapping with a folded string scalar value") {
        lexer_t lexer("test: >\n  a literal scalar.\nfoo: \'bar\'\npi: 3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "a literal scalar.\n");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SINGLE_QUOTED_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "pi");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE_NOTHROW(std::string(token.token_begin_itr, token.token_end_itr));
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}
