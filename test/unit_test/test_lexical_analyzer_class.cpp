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

        lexer_t lexer(fkyaml::detail::input_adapter(value_pair.first));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::YAML_VER_DIRECTIVE);
        REQUIRE(lexer.get_yaml_version() == value_pair.second);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("wrong YAML directive") {
        auto buffer = GENERATE(std::string("%YUML 1.2"), std::string("%YANL 1.2    \n"), std::string("%YAML1.2"));

        lexer_t lexer(fkyaml::detail::input_adapter(buffer));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::INVALID_DIRECTIVE);

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

        lexer_t lexer(fkyaml::detail::input_adapter(buffer));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_TagDirective") {
    fkyaml::detail::lexical_token token;

    SECTION("primary tag handle") {
        auto input = GENERATE(std::string("%TAG ! foo"), std::string("%TAG\t!\tfoo"));
        lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG_DIRECTIVE);
        REQUIRE(lexer.get_tag_handle() == "!");
        REQUIRE(lexer.get_tag_prefix() == "foo");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("secondary tag handle") {
        auto input = GENERATE(std::string("%TAG !! foo"), std::string("%TAG\t!!\tfoo"));
        lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG_DIRECTIVE);
        REQUIRE(lexer.get_tag_handle() == "!!");
        REQUIRE(lexer.get_tag_prefix() == "foo");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("named tag handle") {
        auto input = GENERATE(std::string("%TAG !va1id-ta9! foo"), std::string("%TAG\t!va1id-ta9!\tfoo"));
        lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG_DIRECTIVE);
        REQUIRE(lexer.get_tag_handle() == "!va1id-ta9!");
        REQUIRE(lexer.get_tag_prefix() == "foo");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("invalid TAG directive") {
        auto buffer = GENERATE(std::string("%TUB"), std::string("%TAC"), std::string("%TAGE"));

        lexer_t lexer(fkyaml::detail::input_adapter(buffer));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::INVALID_DIRECTIVE);
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

        lexer_t lexer(fkyaml::detail::input_adapter(input));
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

        lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_InvalidDirective") {
    auto buffer = GENERATE(std::string("%TAG"), std::string("%YAML"));

    lexer_t lexer(fkyaml::detail::input_adapter(buffer));
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
}

TEST_CASE("LexicalAnalyzer_ReservedDirective") {
    auto buffer =
        GENERATE(std::string("%TEST\n"), std::string("%1984\n "), std::string("%TEST4LIB\n"), std::string("%%ERROR"));

    fkyaml::detail::lexical_token token;
    lexer_t lexer(fkyaml::detail::input_adapter(buffer));
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::INVALID_DIRECTIVE);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_EmptyDirective") {
    lexer_t lexer(fkyaml::detail::input_adapter("%"));
    REQUIRE(lexer.get_next_token().type == fkyaml::detail::lexical_token_t::INVALID_DIRECTIVE);
}

TEST_CASE("LexicalAnalyzer_EndOfDirectives") {
    lexer_t lexer(fkyaml::detail::input_adapter("%YAML 1.2\n---\nfoo: bar"));
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
    lexer_t lexer(fkyaml::detail::input_adapter("%YAML 1.2\n---\n..."));
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
        lexer_t lexer(fkyaml::detail::input_adapter(": "));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with LF newline code") {
        lexer_t lexer(fkyaml::detail::input_adapter(":\n"));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with the end of the buffer") {
        lexer_t lexer(fkyaml::detail::input_adapter(":"));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with a comment and a LF newline code") {
        lexer_t lexer(fkyaml::detail::input_adapter(": # comment\n"));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with a comment and no newline code") {
        lexer_t lexer(fkyaml::detail::input_adapter(": # comment"));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with many spaces and a LF newline code") {
        lexer_t lexer(fkyaml::detail::input_adapter(":                         \n"));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with many spaces and no newline code") {
        lexer_t lexer(fkyaml::detail::input_adapter(":                         "));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with an always-safe character") {
        lexer_t lexer(fkyaml::detail::input_adapter(":test"));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == ":test");
    }

    SECTION("colon with a flow indicator in a non-flow context") {
        auto input =
            GENERATE(std::string(":,"), std::string(":{"), std::string(":}"), std::string(":["), std::string(":]"));
        lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == input);
    }

    SECTION("colon with a flow indicator in a flow context") {
        auto input = GENERATE(
            std::string("{:,"), std::string("{:{"), std::string("{:}"), std::string("{:["), std::string("{:]"));
        lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }
}

TEST_CASE("LexicalAnalzer_BlockSequenceEntryPrefix") {
    auto input = GENERATE(std::string("- foo"), std::string("-\tfoo"), std::string("-\n  foo"));

    fkyaml::detail::lexical_token token;
    lexer_t lexer(fkyaml::detail::input_adapter(input));
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo");
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

    lexer_t lexer(fkyaml::detail::input_adapter(value_pair.first));
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == value_pair.second);
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

    lexer_t lexer(fkyaml::detail::input_adapter(value_pair.first));
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::SINGLE_QUOTED_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == value_pair.second);
}

TEST_CASE("LexicalAnalyzer_DoubleQuotedString") {
    using value_pair_t = std::pair<std::string, fkyaml::node::string_type>;
    auto value_pair = GENERATE(
        value_pair_t(std::string("\"\""), fkyaml::node::string_type("")),
        value_pair_t(std::string("\"foo bar\""), fkyaml::node::string_type("foo bar")),
        value_pair_t(std::string("\"foo\tbar\""), fkyaml::node::string_type("foo\tbar")),
        value_pair_t(std::string("\"foo's bar\""), fkyaml::node::string_type("foo's bar")),
        value_pair_t(std::string("\"foo:bar\""), fkyaml::node::string_type("foo:bar")),
        value_pair_t(std::string("\"foo,bar\""), fkyaml::node::string_type("foo,bar")),
        value_pair_t(std::string("\"foo]bar\""), fkyaml::node::string_type("foo]bar")),
        value_pair_t(std::string("\"foo}bar\""), fkyaml::node::string_type("foo}bar")),
        value_pair_t(std::string("\"\\x30\\x2B\\x6d\""), fkyaml::node::string_type("0+m")),

        value_pair_t(std::string("\"foo\nbar\""), fkyaml::node::string_type("foo bar")),
        value_pair_t(std::string("\"foo \t\n \tbar\""), fkyaml::node::string_type("foo bar")),
        value_pair_t(std::string("\"foo\n\n \t\nbar\""), fkyaml::node::string_type("foo\n\nbar")),
        value_pair_t(std::string("\"\nfoo\n\n \t\nbar\""), fkyaml::node::string_type(" foo\n\nbar")),
        value_pair_t(std::string("\"foo\nbar\n\""), fkyaml::node::string_type("foo bar ")),
        value_pair_t(std::string("\"foo\\\nbar\""), fkyaml::node::string_type("foobar")),
        value_pair_t(std::string("\"foo \t\\\nbar\""), fkyaml::node::string_type("foo \tbar")),
        value_pair_t(std::string("\"\\\n  foo \t\\\n\tbar\t  \t\\\n\""), fkyaml::node::string_type("foo \tbar\t  \t")));

    lexer_t lexer(fkyaml::detail::input_adapter(value_pair.first));
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == value_pair.second);
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

    lexer_t lexer(fkyaml::detail::input_adapter(mb_char));
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == mb_char);
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

    lexer_t lexer(fkyaml::detail::input_adapter(value_pair.first));
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR);
    REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == value_pair.second);
}

TEST_CASE("LexicalAnalyzer_InvalidString") {
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

        lexer_t lexer(fkyaml::detail::input_adapter(buffer));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("invalid encoding") {
        std::string buffer = "\"\\U00110000\"";
        lexer_t lexer(fkyaml::detail::input_adapter(buffer));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::invalid_encoding);
    }
}

TEST_CASE("LexicalAnalyzer_InvalidMultiByteCharString") {
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

    auto input_adapter = fkyaml::detail::input_adapter(mb_char);
    REQUIRE_THROWS_AS(lexer_t(std::move(input_adapter)), fkyaml::invalid_encoding);
}

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

    lexer_t lexer(fkyaml::detail::input_adapter(buffer));
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
}

TEST_CASE("LexicalAnalyzer_LiteralStringScalar") {
    fkyaml::detail::lexical_token token;

    SECTION("empty literal string scalar with strip chomping") {
        const char input[] = "|-\n"
                             "  \n";
        lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "");
    }

    SECTION("empty literal string scalar with clip chomping") {
        const char input[] = "|\n"
                             "  \n";
        lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "");
    }

    SECTION("empty literal string scalar with keep chomping") {
        const char input[] = "|+\n"
                             "  \n";
        lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "\n");
    }

    SECTION("literal string scalar with 0 indent level.") {
        const char input[] = "|0\n"
                             "foo";

        lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("less indented literal string scalar") {
        const char input[] = "|2\n"
                             " foo";

        lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("literal scalar with the first line being more indented than the indicated level") {
        const char input[] = "|2\n"
                             "    foo\n"
                             "  bar\n";
        lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "  foo\nbar\n");
    }

    SECTION("literal string scalar") {
        const char input[] = "|\n"
                             "  foo\n"
                             "  bar\n";
        lexer_t lexer(fkyaml::detail::input_adapter(input));

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
        lexer_t lexer(fkyaml::detail::input_adapter(input));

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
        lexer_t lexer(fkyaml::detail::input_adapter(input));

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
        lexer_t lexer(fkyaml::detail::input_adapter(input));

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
        lexer_t lexer(fkyaml::detail::input_adapter(input));

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
        lexer_t lexer(fkyaml::detail::input_adapter(input));

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
        lexer_t lexer(fkyaml::detail::input_adapter(input));

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
        lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo\n  bar\n\nbaz\n\n");
    }

    SECTION("literal string scalar with trailing spaces/tabs after the block scalar header.") {
        auto input = GENERATE(
            std::string("|2  \n  foo\n"), std::string("|2\t\t\n  foo\n"), std::string("|2 # comment\n  foo\n"));
        lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo\n");
    }

    SECTION("literal string scalar with invalid block scalar headers") {
        auto input = GENERATE(
            std::string("|++2\n  foo"),
            std::string("|--2\n  foo"),
            std::string("|+-2\n  foo"),
            std::string("|-+2\n  foo"),
            std::string("|+0\n  foo"),
            std::string("|+11\n           foo"),
            std::string("|invalid\n  foo"));

        lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_THROWS_AS(token = lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_FoldedString") {
    fkyaml::detail::lexical_token token;

    SECTION("empty folded string scalar with strip chomping") {
        const char input[] = ">-\n"
                             "  \n";
        lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "");
    }

    SECTION("empty folded string scalar with clip chomping") {
        const char input[] = ">\n"
                             "  \n";
        lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "");
    }

    SECTION("empty folded string scalar with keep chomping") {
        const char input[] = ">+\n"
                             "  \n";
        lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "\n");
    }

    SECTION("folded string scalar with 0 indent level") {
        const char input[] = "|0\n"
                             "foo";

        lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("less indented folded string scalar") {
        const char input[] = ">2\n"
                             " foo";

        lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("folded string scalar with the first line being more indented than the indicated level") {
        const char input[] = ">2\n"
                             "    foo\n"
                             "  bar\n";
        lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "\n  foo\nbar\n");
    }

    SECTION("folded string scalar with the non-first line being more indented than the indicated level") {
        const char input[] = ">2\n"
                             "  foo\n"
                             "    bar\n";
        lexer_t lexer(fkyaml::detail::input_adapter(input));

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
        lexer_t lexer(fkyaml::detail::input_adapter(input));

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
        lexer_t lexer(fkyaml::detail::input_adapter(input));

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
        lexer_t lexer(fkyaml::detail::input_adapter(input));

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
        lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo bar\n\n");
    }

    SECTION("folded string scalar with trailing spaces/tabs/comments after the block scalar header.") {
        auto input = GENERATE(
            std::string(">2  \n  foo\n"), std::string(">2\t\t\n  foo\n"), std::string(">2 # comment\n  foo\n"));
        lexer_t lexer(fkyaml::detail::input_adapter(input));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo\n");
    }

    SECTION("folded string scalar with invalid block scalar headers") {
        auto input = GENERATE(
            std::string(">++2\n  foo"),
            std::string(">--2\n  foo"),
            std::string(">+-2\n  foo"),
            std::string(">-+2\n  foo"),
            std::string(">+0\n  foo"),
            std::string(">+11\n           foo"),
            std::string(">invalid\n  foo"));

        lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_THROWS_AS(token = lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_Anchor") {
    fkyaml::detail::lexical_token token;

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

        lexer_t lexer(fkyaml::detail::input_adapter(test_data.first));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::ANCHOR_PREFIX);
        REQUIRE_NOTHROW(std::string(token.token_begin_itr, token.token_end_itr) == test_data.second);
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

        lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_Alias") {
    fkyaml::detail::lexical_token token;

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

        lexer_t lexer(fkyaml::detail::input_adapter(test_data.first));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::ALIAS_PREFIX);
        REQUIRE_NOTHROW(std::string(token.token_begin_itr, token.token_end_itr) == test_data.second);
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

        lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_Tag") {
    fkyaml::detail::lexical_token token;

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

        lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG_PREFIX);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == input.substr(0, input.size() - 4));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "tag");
    }

    SECTION("valid tag name (not followed by a value)") {
        auto input = GENERATE(std::string("!"), std::string("!!foo"), std::string("!foo!bar"), std::string("!foo"));

        lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG_PREFIX);
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == input);
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

        lexer_t lexer(fkyaml::detail::input_adapter(input));
        REQUIRE_THROWS_AS(token = lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_ReservedIndicator") {
    auto buffer = GENERATE(std::string("@invalid"), std::string("`invalid"));
    lexer_t lexer(fkyaml::detail::input_adapter(buffer));
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
}

TEST_CASE("LexicalAnalyzer_KeyBooleanValuePair") {
    lexer_t lexer(fkyaml::detail::input_adapter("test: true"));
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
    lexer_t lexer(fkyaml::detail::input_adapter("test: -5784"));
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
    lexer_t lexer(fkyaml::detail::input_adapter("test: -5.58e-3"));
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
    lexer_t lexer(fkyaml::detail::input_adapter("test: \"some value\""));
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
        lexer_t lexer(fkyaml::detail::input_adapter("test: [ foo, bar ]"));

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
        lexer_t lexer(fkyaml::detail::input_adapter("test: [ { foo: one, bar: false }, { foo: two, bar: true } ]"));

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
        lexer_t lexer(fkyaml::detail::input_adapter("test: { bool: true, foo: bar, pi: 3.14 }"));

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
        REQUIRE(std::string(token.token_begin_itr, token.token_end_itr) == "foo");

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
        lexer_t lexer(fkyaml::detail::input_adapter("test: {foo: bar baz}"));

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
        std::string buffer = "test:\n  - foo\n  - bar";
        lexer_t lexer(fkyaml::detail::input_adapter(buffer));

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
        std::string buffer = "test:\n  - foo: one\n    bar: false\n  - foo: two\n    bar: true";
        lexer_t lexer(fkyaml::detail::input_adapter(buffer));

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
        lexer_t lexer(fkyaml::detail::input_adapter("test:\n  bool: true\n  foo: \'bar\'\n  pi: 3.14"));

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
        lexer_t lexer(fkyaml::detail::input_adapter("test: |\n  a literal scalar.\nfoo: \'bar\'\npi: 3.14"));

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
        lexer_t lexer(fkyaml::detail::input_adapter("test: >\n  a literal scalar.\nfoo: \'bar\'\npi: 3.14"));

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
