//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.12
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("LexicalAnalyzer_YamlVersionDirective") {
    fkyaml::detail::lexical_token token;

    SECTION("valid YAML directive") {
        using value_pair_t = std::pair<fkyaml::detail::str_view, fkyaml::detail::str_view>;
        auto value_pair = GENERATE(
            value_pair_t("%YAML 1.1 ", "1.1"),
            value_pair_t("%YAML\t1.1\t", "1.1"),
            value_pair_t("%YAML 1.1\n", "1.1"),
            value_pair_t("%YAML 1.1", "1.1"),
            value_pair_t("%YAML 1.2 ", "1.2"),
            value_pair_t("%YAML\t1.2\t", "1.2"),
            value_pair_t("%YAML 1.2\n", "1.2"),
            value_pair_t("%YAML 1.2", "1.2"));

        fkyaml::detail::lexical_analyzer lexer(value_pair.first);
        lexer.set_document_state(true);

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

        fkyaml::detail::lexical_analyzer lexer(buffer);
        lexer.set_document_state(true);

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

        fkyaml::detail::lexical_analyzer lexer(buffer);
        lexer.set_document_state(true);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_TagDirective") {
    fkyaml::detail::lexical_token token;

    SECTION("primary tag handle") {
        auto input = GENERATE(fkyaml::detail::str_view("%TAG ! foo"), fkyaml::detail::str_view("%TAG\t!\tfoo"));
        fkyaml::detail::lexical_analyzer lexer(input);
        lexer.set_document_state(true);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG_DIRECTIVE);
        REQUIRE(lexer.get_tag_handle().compare("!") == 0);
        REQUIRE(lexer.get_tag_prefix().compare("foo") == 0);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("secondary tag handle") {
        auto input = GENERATE(fkyaml::detail::str_view("%TAG !! foo"), fkyaml::detail::str_view("%TAG\t!!\tfoo"));
        fkyaml::detail::lexical_analyzer lexer(input);
        lexer.set_document_state(true);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG_DIRECTIVE);
        REQUIRE(lexer.get_tag_handle().compare("!!") == 0);
        REQUIRE(lexer.get_tag_prefix().compare("foo") == 0);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("named tag handle") {
        auto input = GENERATE(
            fkyaml::detail::str_view("%TAG !va1id-ta9! foo"), fkyaml::detail::str_view("%TAG\t!va1id-ta9!\tfoo"));
        fkyaml::detail::lexical_analyzer lexer(input);
        lexer.set_document_state(true);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG_DIRECTIVE);
        REQUIRE(lexer.get_tag_handle().compare("!va1id-ta9!") == 0);
        REQUIRE(lexer.get_tag_prefix().compare("foo") == 0);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("invalid TAG directive") {
        auto buffer = GENERATE(
            fkyaml::detail::str_view("%TUB"), fkyaml::detail::str_view("%TAC"), fkyaml::detail::str_view("%TAGE"));

        fkyaml::detail::lexical_analyzer lexer(buffer);
        lexer.set_document_state(true);

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

        fkyaml::detail::lexical_analyzer lexer(input);
        lexer.set_document_state(true);
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

        fkyaml::detail::lexical_analyzer lexer(input);
        lexer.set_document_state(true);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_InvalidDirective") {
    auto buffer = GENERATE(fkyaml::detail::str_view("%TAG"), fkyaml::detail::str_view("%YAML"));

    fkyaml::detail::lexical_analyzer lexer(buffer);
    lexer.set_document_state(true);
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
}

TEST_CASE("LexicalAnalyzer_ReservedDirective") {
    auto buffer = GENERATE(
        fkyaml::detail::str_view("%TEST\n"),
        fkyaml::detail::str_view("%1984\n "),
        fkyaml::detail::str_view("%TEST4LIB\n"),
        fkyaml::detail::str_view("%%ERROR"));

    fkyaml::detail::lexical_token token;
    fkyaml::detail::lexical_analyzer lexer(buffer);
    lexer.set_document_state(true);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::INVALID_DIRECTIVE);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_EmptyDirective") {
    fkyaml::detail::lexical_analyzer lexer("%");
    lexer.set_document_state(true);
    REQUIRE(lexer.get_next_token().type == fkyaml::detail::lexical_token_t::INVALID_DIRECTIVE);
}

TEST_CASE("LexicalAnalyzer_EndOfDirectives") {
    fkyaml::detail::lexical_token token;
    fkyaml::detail::lexical_analyzer lexer("%YAML 1.2\n---\nfoo: bar");
    lexer.set_document_state(true);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::YAML_VER_DIRECTIVE);
    REQUIRE(lexer.get_yaml_version() == fkyaml::detail::str_view("1.2"));
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_DIRECTIVES);
    REQUIRE_NOTHROW(token = lexer.get_next_token());

    lexer.set_document_state(false);

    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(token.str == "foo");
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(token.str == "bar");
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_EndOfDocuments") {
    fkyaml::detail::lexical_token token;
    fkyaml::detail::lexical_analyzer lexer("%YAML 1.2\n---\n...");
    lexer.set_document_state(true);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::YAML_VER_DIRECTIVE);
    REQUIRE(lexer.get_yaml_version() == fkyaml::detail::str_view("1.2"));
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_DIRECTIVES);

    lexer.set_document_state(false);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_DOCUMENT);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_Colon") {
    fkyaml::detail::lexical_token token;

    SECTION("colon with half-width space") {
        fkyaml::detail::lexical_analyzer lexer(": ");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with LF newline code") {
        fkyaml::detail::lexical_analyzer lexer(":\n");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with the end of the buffer") {
        fkyaml::detail::lexical_analyzer lexer(":");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with a comment and a LF newline code") {
        fkyaml::detail::lexical_analyzer lexer(": # comment\n");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with a comment and no newline code") {
        fkyaml::detail::lexical_analyzer lexer(": # comment");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with many spaces and a LF newline code") {
        fkyaml::detail::lexical_analyzer lexer(":                         \n");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with many spaces and no newline code") {
        fkyaml::detail::lexical_analyzer lexer(":                         ");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }

    SECTION("colon with an always-safe character") {
        fkyaml::detail::lexical_analyzer lexer(":test");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == ":test");
    }

    SECTION("colon with a flow indicator in a non-flow context") {
        auto input = GENERATE(
            fkyaml::detail::str_view(":,"),
            fkyaml::detail::str_view(":{"),
            fkyaml::detail::str_view(":}"),
            fkyaml::detail::str_view(":["),
            fkyaml::detail::str_view(":]"));
        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == input);
    }

    SECTION("colon with a flow indicator in a flow context") {
        auto input = GENERATE(
            fkyaml::detail::str_view("{:,"),
            fkyaml::detail::str_view("{:{"),
            fkyaml::detail::str_view("{:}"),
            fkyaml::detail::str_view("{:["),
            fkyaml::detail::str_view("{:]"));
        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);
        lexer.set_context_state(true);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);
    }
}

TEST_CASE("LexicalAnalzer_BlockSequenceEntryPrefix") {
    auto input = GENERATE(
        fkyaml::detail::str_view("- foo"), fkyaml::detail::str_view("-\tfoo"), fkyaml::detail::str_view("-\n  foo"));

    fkyaml::detail::lexical_token token;
    fkyaml::detail::lexical_analyzer lexer(input);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);
    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(token.str == fkyaml::detail::str_view("foo"));
}

TEST_CASE("LexicalAnalyzer_PlainScalar") {
    using value_pair_t = std::pair<fkyaml::detail::str_view, fkyaml::detail::str_view>;
    auto value_pair = GENERATE(
        value_pair_t("test", "test"),
        value_pair_t("nop", "nop"),
        value_pair_t("none", "none"),
        value_pair_t("?test", "?test"),
        value_pair_t(".NET", ".NET"),
        value_pair_t(".on", ".on"),
        value_pair_t(".n", ".n"),
        value_pair_t("-t", "-t"),
        value_pair_t("-foo", "-foo"),
        value_pair_t("-.test", "-.test"),
        value_pair_t("?", "?"),
        value_pair_t("--foo", "--foo"),
        value_pair_t("+123", "+123"),
        value_pair_t("1.2.3", "1.2.3"),
        value_pair_t("foo,bar", "foo,bar"),
        value_pair_t("foo[bar", "foo[bar"),
        value_pair_t("foo]bar", "foo]bar"),
        value_pair_t("foo{bar", "foo{bar"),
        value_pair_t("foo}bar", "foo}bar"),
        value_pair_t("foo:bar", "foo:bar"),
        value_pair_t("foo bar", "foo bar"),
        value_pair_t("foo\"bar", "foo\"bar"),
        value_pair_t("foo\'s bar", "foo\'s bar"),
        value_pair_t("foo\\bar", "foo\\bar"),
        value_pair_t("nullValue", "nullValue"),
        value_pair_t("NullValue", "NullValue"),
        value_pair_t("NULL_VALUE", "NULL_VALUE"),
        value_pair_t("~Value", "~Value"),
        value_pair_t("trueValue", "trueValue"),
        value_pair_t("TrueValue", "TrueValue"),
        value_pair_t("TRUE_VALUE", "TRUE_VALUE"),
        value_pair_t("falseValue", "falseValue"),
        value_pair_t("FalseValue", "FalseValue"),
        value_pair_t("FALSE_VALUE", "FALSE_VALUE"),
        value_pair_t(".infValue", ".infValue"),
        value_pair_t(".InfValue", ".InfValue"),
        value_pair_t(".INF_VALUE", ".INF_VALUE"),
        value_pair_t("-.infValue", "-.infValue"),
        value_pair_t("-.InfValue", "-.InfValue"),
        value_pair_t("-.INF_VALUE", "-.INF_VALUE"),
        value_pair_t(".nanValue", ".nanValue"),
        value_pair_t(".NaNValue", ".NaNValue"),
        value_pair_t(".NAN_VALUE", ".NAN_VALUE"));

    fkyaml::detail::lexical_analyzer lexer(value_pair.first);
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(token.str == value_pair.second);
}

TEST_CASE("LexicalAnalyzer_SingleQuotedScalar") {
    using value_pair_t = std::pair<fkyaml::detail::str_view, uint32_t /*end offset*/>;
    auto value_pair = GENERATE(
        value_pair_t("\'\'", 1),
        value_pair_t("\'foo\"bar\'", 1),
        value_pair_t("\'foo bar\'", 1),
        value_pair_t("\'foo\'\'bar\'", 1),
        value_pair_t("\'foo\'\'bar\' ", 2),
        value_pair_t("\'foo,bar\'", 1),
        value_pair_t("\'foo]bar\'", 1),
        value_pair_t("\'foo}bar\'", 1),
        value_pair_t("\'foo\"bar\'", 1),
        value_pair_t("\'foo:bar\'", 1),
        value_pair_t("\'foo\\bar\'", 1),

        value_pair_t("\'foo\nbar\'", 1),
        value_pair_t("\'foo \t\n \tbar\'", 1),
        value_pair_t("\'foo\n\n \t\nbar\'", 1),
        value_pair_t("\'\nfoo\n\n \t\nbar\'", 1),
        value_pair_t("\'foo\nbar\n\'", 1));

    fkyaml::detail::lexical_analyzer lexer(value_pair.first);
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::SINGLE_QUOTED_SCALAR);
    REQUIRE(token.str.begin() == value_pair.first.begin() + 1);
    REQUIRE(token.str.end() == value_pair.first.end() - value_pair.second);
}

TEST_CASE("LexicalAnalyzer_DoubleQuotedScalar") {
    using value_pair_t = std::pair<fkyaml::detail::str_view, fkyaml::detail::str_view>;
    auto value_pair = GENERATE(
        value_pair_t("\"\"", ""),
        value_pair_t("\"foo bar\"", "foo bar"),
        value_pair_t("\"foo\tbar\"", "foo\tbar"),
        value_pair_t("\"foo's bar\"", "foo's bar"),
        value_pair_t("\"foo:bar\"", "foo:bar"),
        value_pair_t("\"foo,bar\"", "foo,bar"),
        value_pair_t("\"foo]bar\"", "foo]bar"),
        value_pair_t("\"foo}bar\"", "foo}bar"),
        value_pair_t("\"\\x30\\x2B\\x6d\"", "0+m"),

        value_pair_t("\"foo\nbar\"", "foo bar"),
        value_pair_t("\"foo \t\n \tbar\"", "foo bar"),
        value_pair_t("\"foo\n\n \t\nbar\"", "foo\n\nbar"),
        value_pair_t("\"\nfoo\n\n \t\nbar\"", " foo\n\nbar"),
        value_pair_t("\"foo\nbar\n\"", "foo bar "),
        value_pair_t("\"foo\\\nbar\"", "foobar"),
        value_pair_t("\"foo \t\\\nbar\"", "foo \tbar"),
        value_pair_t("\"\\\n  foo \t\\\n\tbar\t  \t\\\n\"", "foo \tbar\t  \t"));

    fkyaml::detail::lexical_analyzer lexer(value_pair.first);
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR);
    REQUIRE(token.str == value_pair.second);
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
    fkyaml::detail::lexical_analyzer lexer(input);
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(token.str == input);
}

TEST_CASE("LexicalAnalyzer_EscapedUnicodeCharacter") {
    using value_pair_t = std::pair<fkyaml::detail::str_view, std::string>;
    using char_traits_t = std::char_traits<char>;
    auto value_pair = GENERATE(
        value_pair_t("\"\\x00\"", {char_traits_t::to_char_type(0x00)}),
        value_pair_t("\"\\x40\"", {char_traits_t::to_char_type(0x40)}),
        value_pair_t("\"\\x7F\"", {char_traits_t::to_char_type(0x7F)}),
        value_pair_t("\"\\u0000\"", {char_traits_t::to_char_type(0x00)}),
        value_pair_t("\"\\u0040\"", {char_traits_t::to_char_type(0x40)}),
        value_pair_t("\"\\u007F\"", {char_traits_t::to_char_type(0x7F)}),
        value_pair_t("\"\\u0080\"", {char_traits_t::to_char_type(0xC2), char_traits_t::to_char_type(0x80)}),
        value_pair_t("\"\\u0400\"", {char_traits_t::to_char_type(0xD0), char_traits_t::to_char_type(0x80)}),
        value_pair_t("\"\\u07FF\"", {char_traits_t::to_char_type(0xDF), char_traits_t::to_char_type(0xBF)}),
        value_pair_t(
            "\"\\u0800\"",
            {char_traits_t::to_char_type(0xE0), char_traits_t::to_char_type(0xA0), char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            "\"\\u8000\"",
            {char_traits_t::to_char_type(0xE8), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            "\"\\uFFFF\"",
            {char_traits_t::to_char_type(0xEF), char_traits_t::to_char_type(0xBF), char_traits_t::to_char_type(0xBF)}),
        value_pair_t("\"\\U00000000\"", {char_traits_t::to_char_type(0x00)}),
        value_pair_t("\"\\U00000040\"", {char_traits_t::to_char_type(0x40)}),
        value_pair_t("\"\\U0000007F\"", {char_traits_t::to_char_type(0x7F)}),
        value_pair_t("\"\\U00000080\"", {char_traits_t::to_char_type(0xC2), char_traits_t::to_char_type(0x80)}),
        value_pair_t("\"\\U00000400\"", {char_traits_t::to_char_type(0xD0), char_traits_t::to_char_type(0x80)}),
        value_pair_t("\"\\U000007FF\"", {char_traits_t::to_char_type(0xDF), char_traits_t::to_char_type(0xBF)}),
        value_pair_t(
            "\"\\U00000800\"",
            {char_traits_t::to_char_type(0xE0), char_traits_t::to_char_type(0xA0), char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            "\"\\U00008000\"",
            {char_traits_t::to_char_type(0xE8), char_traits_t::to_char_type(0x80), char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            "\"\\U0000FFFF\"",
            {char_traits_t::to_char_type(0xEF), char_traits_t::to_char_type(0xBF), char_traits_t::to_char_type(0xBF)}),
        value_pair_t(
            "\"\\U00010000\"",
            {char_traits_t::to_char_type(0xF0),
             char_traits_t::to_char_type(0x90),
             char_traits_t::to_char_type(0x80),
             char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            "\"\\U00080000\"",
            {char_traits_t::to_char_type(0xF2),
             char_traits_t::to_char_type(0x80),
             char_traits_t::to_char_type(0x80),
             char_traits_t::to_char_type(0x80)}),
        value_pair_t(
            "\"\\U0010FFFF\"",
            {char_traits_t::to_char_type(0xF4),
             char_traits_t::to_char_type(0x8F),
             char_traits_t::to_char_type(0xBF),
             char_traits_t::to_char_type(0xBF)}));

    fkyaml::detail::lexical_analyzer lexer(value_pair.first);
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR);
    REQUIRE(token.str == value_pair.second);
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

        fkyaml::detail::lexical_analyzer lexer(buffer);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("invalid encoding") {
        fkyaml::detail::lexical_analyzer lexer("\"\\U00110000\"");
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::invalid_encoding);
    }
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

    fkyaml::detail::lexical_analyzer lexer(buffer);
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
}

TEST_CASE("LexicalAnalyzer_LiteralStringScalar") {
    fkyaml::detail::lexical_token token;

    SECTION("empty literal string scalar with strip chomping") {
        const char input[] = "|-\n"
                             "  \n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "");
    }

    SECTION("empty literal string scalar with clip chomping") {
        const char input[] = "|\n"
                             "  \n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "");
    }

    SECTION("empty literal string scalar with keep chomping") {
        const char input[] = "|+\n"
                             "  \n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "\n");
    }

    SECTION("literal string scalar with 0 indent level.") {
        const char input[] = "|0\n"
                             "foo";

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("less indented literal string scalar") {
        const char input[] = "|2\n"
                             " foo";

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("literal scalar with the first line being more indented than the indicated level") {
        const char input[] = "|2\n"
                             "    foo\n"
                             "  bar\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "  foo\nbar\n");
    }

    SECTION("literal string scalar") {
        const char input[] = "|\n"
                             "  foo\n"
                             "  bar\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "foo\nbar\n");
    }

    SECTION("literal string scalar with implicit indentation and strip chomping") {
        const char input[] = "|-\n"
                             "\n"
                             "  foo\n"
                             "  bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "\nfoo\nbar\n\nbaz");
    }

    SECTION("literal string scalar with explicit indentation and strip chomping") {
        const char input[] = "|-2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "foo\n  bar\n\nbaz");
    }

    SECTION("literal string scalar with implicit indentation and clip chomping") {
        const char input[] = "|\n"
                             "\n"
                             "  foo\n"
                             "  bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "\nfoo\nbar\n\nbaz\n");
    }

    SECTION("literal string scalar with explicit indentation and clip chomping") {
        const char input[] = "|2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "foo\n  bar\n\nbaz\n");
    }

    SECTION("literal string scalar with clip chomping and no trailing newlines") {
        const char input[] = "|2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "foo\n  bar\n\nbaz");
    }

    SECTION("literal string scalar with implicit indentation and keep chomping") {
        const char input[] = "|+\n"
                             "\n"
                             "  foo\n"
                             "  bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "\nfoo\nbar\n\nbaz\n\n");
    }

    SECTION("literal string scalar with explicit indentation and keep chomping") {
        const char input[] = "|+2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz\n"
                             "\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "foo\n  bar\n\nbaz\n\n");
    }

    SECTION("literal string scalar with trailing spaces/tabs after the block scalar header.") {
        auto input = GENERATE(
            fkyaml::detail::str_view("|2  \n  foo\n"),
            fkyaml::detail::str_view("|2\t\t\n  foo\n"),
            fkyaml::detail::str_view("|2 # comment\n  foo\n"));
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "foo\n");
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

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_THROWS_AS(token = lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_FoldedString") {
    fkyaml::detail::lexical_token token;

    SECTION("empty folded string scalar with strip chomping") {
        const char input[] = ">-\n"
                             "  \n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str.empty());
    }

    SECTION("empty folded string scalar with clip chomping") {
        const char input[] = ">\n"
                             "  \n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "");
    }

    SECTION("empty folded string scalar with keep chomping") {
        const char input[] = ">+\n"
                             "  \n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "\n");
    }

    SECTION("folded string scalar with 0 indent level") {
        const char input[] = "|0\n"
                             "foo";

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("less indented folded string scalar") {
        const char input[] = ">2\n"
                             " foo";

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("folded string scalar with the first line being more indented than the indicated level") {
        const char input[] = ">2\n"
                             "    foo\n"
                             "  bar\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "\n  foo\nbar\n");
    }

    SECTION("folded string scalar with the non-first line being more indented than the indicated level") {
        const char input[] = ">2\n"
                             "  foo\n"
                             "    bar\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "foo\n  bar\n");
    }

    SECTION("folded string scalar") {
        const char input[] = ">\n"
                             "  foo\n"
                             "  \n"
                             "\n"
                             "  bar\n"
                             " \n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "foo\n\nbar\n");
    }

    SECTION("folded string scalar with implicit indentation and strip chomping") {
        const char input[] = ">-\n"
                             "  foo\n"
                             "  bar\n"
                             " \n"
                             "\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "foo bar");
    }

    SECTION("folded string scalar with implicit indentation and clip chomping") {
        const char input[] = ">\n"
                             "  foo\n"
                             "  bar\n"
                             "  \n"
                             "\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "foo bar\n");
    }

    SECTION("folded string scalar with implicit indentation and keep chomping") {
        const char input[] = ">+\n"
                             "  foo\n"
                             "  bar\n"
                             " \n"
                             "\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "foo bar\n\n");
    }

    SECTION("folded string scalar with trailing spaces/tabs/comments after the block scalar header.") {
        auto input = GENERATE(
            fkyaml::detail::str_view(">2  \n  foo\n"),
            fkyaml::detail::str_view(">2\t\t\n  foo\n"),
            fkyaml::detail::str_view(">2 # comment\n  foo\n"));
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "foo\n");
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

        fkyaml::detail::lexical_analyzer lexer(input);
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

        fkyaml::detail::lexical_analyzer lexer(test_data.first);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::ANCHOR_PREFIX);
        REQUIRE(token.str == test_data.second);
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

        fkyaml::detail::lexical_analyzer lexer(input);
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

        fkyaml::detail::lexical_analyzer lexer(test_data.first);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::ALIAS_PREFIX);
        REQUIRE_NOTHROW(token.str == test_data.second);
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

        fkyaml::detail::lexical_analyzer lexer(input);
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

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG_PREFIX);
        REQUIRE(token.str == input.substr(0, input.size() - 4));

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "tag");
    }

    SECTION("valid tag name (not followed by a value)") {
        auto input = GENERATE(
            fkyaml::detail::str_view("!"),
            fkyaml::detail::str_view("!!foo"),
            fkyaml::detail::str_view("!foo!bar"),
            fkyaml::detail::str_view("!foo"));

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::TAG_PREFIX);
        REQUIRE(token.str == input);
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
            fkyaml::detail::str_view("!foo!%f:oo tag"),
            fkyaml::detail::str_view("!foo{ tag"),
            fkyaml::detail::str_view("!foo} tag"),
            fkyaml::detail::str_view("!foo[ tag"),
            fkyaml::detail::str_view("!foo] tag"),
            fkyaml::detail::str_view("!foo, tag"),
            fkyaml::detail::str_view("!!foo{ tag"),
            fkyaml::detail::str_view("!!foo} tag"),
            fkyaml::detail::str_view("!!foo[ tag"),
            fkyaml::detail::str_view("!!foo] tag"),
            fkyaml::detail::str_view("!!foo, tag"),
            fkyaml::detail::str_view("!foo!bar{ tag"),
            fkyaml::detail::str_view("!foo!bar} tag"),
            fkyaml::detail::str_view("!foo!bar[ tag"),
            fkyaml::detail::str_view("!foo!bar] tag"),
            fkyaml::detail::str_view("!foo!bar, tag"));

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_THROWS_AS(token = lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_ReservedIndicator") {
    auto buffer = GENERATE(fkyaml::detail::str_view("@invalid"), fkyaml::detail::str_view("`invalid"));
    fkyaml::detail::lexical_analyzer lexer(buffer);
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
}

TEST_CASE("LexicalAnalyzer_KeyBooleanValuePair") {
    fkyaml::detail::lexical_analyzer lexer("test: true");
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(token.str == "test");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(token.str == "true");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_KeyIntegerValuePair") {
    fkyaml::detail::lexical_analyzer lexer("test: -5784");
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(token.str == "test");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(token.str == "-5784");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_KeyFloatNumberValuePair") {
    fkyaml::detail::lexical_analyzer lexer("test: -5.58e-3");
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(token.str == "test");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(token.str == "-5.58e-3");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_KeyStringValuePair") {
    fkyaml::detail::lexical_analyzer lexer("test: \"some value\"");
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
    REQUIRE(token.str == "test");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR);
    REQUIRE(token.str == "some value");

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
}

TEST_CASE("LexicalAnalyzer_FlowSequence") {
    fkyaml::detail::lexical_token token;

    SECTION("simple flow sequence") {
        fkyaml::detail::lexical_analyzer lexer("test: [ foo, bar ]");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_BEGIN);
        lexer.set_context_state(true);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_END);
        lexer.set_context_state(false);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("flow sequence with flow mapping child nodes") {
        fkyaml::detail::lexical_analyzer lexer("test: [ { foo: one, bar: false }, { foo: two, bar: true } ]");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_BEGIN);
        lexer.set_context_state(true);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "one");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "false");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "two");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "true");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_END);
        lexer.set_context_state(false);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}

TEST_CASE("LexicalAnalyzer_FlowMapping") {
    fkyaml::detail::lexical_token token;

    SECTION("simple flow mapping") {
        fkyaml::detail::lexical_analyzer lexer("test: { bool : true, foo :b: bar, pi: 3.14 }");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);
        lexer.set_context_state(true);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "bool");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "true");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "foo :b");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "pi");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_END);
        lexer.set_context_state(false);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("flow maping with a child mapping node") {
        fkyaml::detail::lexical_analyzer lexer("test: {foo: bar baz}");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);
        lexer.set_context_state(true);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "bar baz");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_END);
        lexer.set_context_state(false);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}

TEST_CASE("LexicalAnalyzer_BlockSequence") {
    fkyaml::detail::lexical_token token;

    SECTION("simple block sequence") {
        fkyaml::detail::lexical_analyzer lexer("test:\n  - foo\n  - bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("block sequence with block mapping child nodes") {
        fkyaml::detail::lexical_analyzer lexer("test:\n  - foo: one\n    bar: false\n  - foo: two\n    bar: true");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "one");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "false");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "two");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "true");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}

TEST_CASE("LexicalAnalyzer_BlockMapping") {
    fkyaml::detail::lexical_token token;

    SECTION("simple block mapping") {
        fkyaml::detail::lexical_analyzer lexer("test:\n  bool: true\n  foo: \'bar\'\n  pi: 3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "bool");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "true");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SINGLE_QUOTED_SCALAR);
        REQUIRE(token.str == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "pi");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("block mapping with a literal string scalar value") {
        fkyaml::detail::lexical_analyzer lexer("test: |\n  a literal scalar.\nfoo: \'bar\'\npi: 3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "a literal scalar.\n");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SINGLE_QUOTED_SCALAR);
        REQUIRE(token.str == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "pi");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("block mapping with a folded string scalar value") {
        fkyaml::detail::lexical_analyzer lexer("test: >\n  a literal scalar.\nfoo: \'bar\'\npi: 3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_SCALAR);
        REQUIRE(token.str == "a literal scalar.\n");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SINGLE_QUOTED_SCALAR);
        REQUIRE(token.str == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "pi");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }
}
