//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.12
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("ScalarParser_FlowPlainScalar_string") {
    fkyaml::node node {};
    fkyaml::detail::scalar_parser<fkyaml::node> scalar_parser {0, 0};
    fkyaml::detail::tag_t tag_type {fkyaml::detail::tag_t::NONE};

    SECTION("plain: normal contents") {
        fkyaml::detail::lexical_token_t lex_type {fkyaml::detail::lexical_token_t::PLAIN_SCALAR};

        auto token = GENERATE(
            fkyaml::detail::str_view("test"),
            fkyaml::detail::str_view("nop"),
            fkyaml::detail::str_view("none"),
            fkyaml::detail::str_view("?test"),
            fkyaml::detail::str_view(".NET"),
            fkyaml::detail::str_view(".on"),
            fkyaml::detail::str_view(".n"),
            fkyaml::detail::str_view("-t"),
            fkyaml::detail::str_view("-foo"),
            fkyaml::detail::str_view("-.test"),
            fkyaml::detail::str_view("?"),
            fkyaml::detail::str_view("--foo"),
            fkyaml::detail::str_view("1.2.3"),
            fkyaml::detail::str_view("foo,bar"),
            fkyaml::detail::str_view("foo[bar"),
            fkyaml::detail::str_view("foo]bar"),
            fkyaml::detail::str_view("foo{bar"),
            fkyaml::detail::str_view("foo}bar"),
            fkyaml::detail::str_view("foo:bar"),
            fkyaml::detail::str_view("foo bar"),
            fkyaml::detail::str_view("foo\"bar"),
            fkyaml::detail::str_view("foo\'s bar"),
            fkyaml::detail::str_view("foo\\bar"),
            fkyaml::detail::str_view("nullValue"),
            fkyaml::detail::str_view("NullValue"),
            fkyaml::detail::str_view("NULL_VALUE"),
            fkyaml::detail::str_view("~Value"),
            fkyaml::detail::str_view("trueValue"),
            fkyaml::detail::str_view("TrueValue"),
            fkyaml::detail::str_view("TRUE_VALUE"),
            fkyaml::detail::str_view("falseValue"),
            fkyaml::detail::str_view("FalseValue"),
            fkyaml::detail::str_view("FALSE_VALUE"),
            fkyaml::detail::str_view(".infValue"),
            fkyaml::detail::str_view(".InfValue"),
            fkyaml::detail::str_view(".INF_VALUE"),
            fkyaml::detail::str_view("-.infValue"),
            fkyaml::detail::str_view("-.InfValue"),
            fkyaml::detail::str_view("-.INF_VALUE"),
            fkyaml::detail::str_view(".nanValue"),
            fkyaml::detail::str_view(".NaNValue"),
            fkyaml::detail::str_view(".NAN_VALUE"));

        REQUIRE_NOTHROW(node = scalar_parser.parse_flow(lex_type, tag_type, token));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == token);
    }

    SECTION("single quoted: single line contents") {
        fkyaml::detail::lexical_token_t lex_type {fkyaml::detail::lexical_token_t::SINGLE_QUOTED_SCALAR};
        using test_data_t = std::pair<fkyaml::detail::str_view, std::string>;
        auto test_data = GENERATE(
            test_data_t("", ""),
            test_data_t("foo\"bar", "foo\"bar"),
            test_data_t("foo bar", "foo bar"),
            test_data_t("foo\'\'bar", "foo\'bar"),
            test_data_t("foo,bar", "foo,bar"),
            test_data_t("foo]bar", "foo]bar"),
            test_data_t("foo}bar", "foo}bar"),
            test_data_t("foo\"bar", "foo\"bar"),
            test_data_t("foo:bar", "foo:bar"),
            test_data_t("foo\\bar", "foo\\bar"));

        REQUIRE_NOTHROW(node = scalar_parser.parse_flow(lex_type, tag_type, test_data.first));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == test_data.second);
    }

    SECTION("single quoted: multiline contents") {
        fkyaml::detail::lexical_token_t lex_type {fkyaml::detail::lexical_token_t::SINGLE_QUOTED_SCALAR};
        using test_data_t = std::pair<fkyaml::detail::str_view, std::string>;
        auto test_data = GENERATE(
            test_data_t("foo\nbar", "foo bar"),
            test_data_t("foo \t\n \tbar", "foo bar"),
            test_data_t("foo\n\n \t\nbar", "foo\n\nbar"),
            test_data_t("\nfoo\n\n \t\nbar", " foo\n\nbar"),
            test_data_t("foo\nbar\n", "foo bar "));

        REQUIRE_NOTHROW(node = scalar_parser.parse_flow(lex_type, tag_type, test_data.first));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == test_data.second);
    }

    SECTION("double quoted: single line contents") {
        fkyaml::detail::lexical_token_t lex_type {fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR};
        using test_data_t = std::pair<fkyaml::detail::str_view, std::string>;
        auto test_data = GENERATE(
            test_data_t("", ""),
            test_data_t("foo bar", "foo bar"),
            test_data_t("foo\tbar", "foo\tbar"),
            test_data_t("foo's bar", "foo's bar"),
            test_data_t("foo:bar", "foo:bar"),
            test_data_t("foo,bar", "foo,bar"),
            test_data_t("foo]bar", "foo]bar"),
            test_data_t("foo}bar", "foo}bar"),
            test_data_t("\\x30\\x2B\\x6d", "0+m"));

        REQUIRE_NOTHROW(node = scalar_parser.parse_flow(lex_type, tag_type, test_data.first));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == test_data.second);
    }

    SECTION("double quoted: multiline contents") {
        fkyaml::detail::lexical_token_t lex_type {fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR};
        using test_data_t = std::pair<fkyaml::detail::str_view, std::string>;
        auto test_data = GENERATE(
            test_data_t("foo\nbar", "foo bar"),
            test_data_t("foo \t\n \tbar", "foo bar"),
            test_data_t("foo\n\n \t\nbar", "foo\n\nbar"),
            test_data_t("\nfoo\n\n \t\nbar", " foo\n\nbar"),
            test_data_t("foo\nbar\n", "foo bar "),
            test_data_t("foo\\\nbar", "foobar"),
            test_data_t("foo \t\\\nbar", "foo \tbar"),
            test_data_t("\\\n  foo \t\\\n\tbar\t  \t\\\n", "foo \tbar\t  \t"));

        REQUIRE_NOTHROW(node = scalar_parser.parse_flow(lex_type, tag_type, test_data.first));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == test_data.second);
    }

    SECTION("double quoted: escaped unicode characters") {
        fkyaml::detail::lexical_token_t lex_type {fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR};
        using test_data_t = std::pair<fkyaml::detail::str_view, std::string>;
        auto to_char = [](int c) { return std::char_traits<char>::to_char_type(c); };

        auto test_data = GENERATE_REF(
            test_data_t("\\x00", {to_char(0x00)}),
            test_data_t("\\x40", {to_char(0x40)}),
            test_data_t("\\x7F", {to_char(0x7F)}),
            test_data_t("\\u0000", {to_char(0x00)}),
            test_data_t("\\u0040", {to_char(0x40)}),
            test_data_t("\\u007F", {to_char(0x7F)}),
            test_data_t("\\u0080", {to_char(0xC2), to_char(0x80)}),
            test_data_t("\\u0400", {to_char(0xD0), to_char(0x80)}),
            test_data_t("\\u07FF", {to_char(0xDF), to_char(0xBF)}),
            test_data_t("\\u0800", {to_char(0xE0), to_char(0xA0), to_char(0x80)}),
            test_data_t("\\u8000", {to_char(0xE8), to_char(0x80), to_char(0x80)}),
            test_data_t("\\uFFFF", {to_char(0xEF), to_char(0xBF), to_char(0xBF)}),
            test_data_t("\\U00000000", {to_char(0x00)}),
            test_data_t("\\U00000040", {to_char(0x40)}),
            test_data_t("\\U0000007F", {to_char(0x7F)}),
            test_data_t("\\U00000080", {to_char(0xC2), to_char(0x80)}),
            test_data_t("\\U00000400", {to_char(0xD0), to_char(0x80)}),
            test_data_t("\\U000007FF", {to_char(0xDF), to_char(0xBF)}),
            test_data_t("\\U00000800", {to_char(0xE0), to_char(0xA0), to_char(0x80)}),
            test_data_t("\\U00008000", {to_char(0xE8), to_char(0x80), to_char(0x80)}),
            test_data_t("\\U0000FFFF", {to_char(0xEF), to_char(0xBF), to_char(0xBF)}),
            test_data_t("\\U00010000", {to_char(0xF0), to_char(0x90), to_char(0x80), to_char(0x80)}),
            test_data_t("\\U00080000", {to_char(0xF2), to_char(0x80), to_char(0x80), to_char(0x80)}),
            test_data_t("\\U0010FFFF", {to_char(0xF4), to_char(0x8F), to_char(0xBF), to_char(0xBF)}));

        REQUIRE_NOTHROW(node = scalar_parser.parse_flow(lex_type, tag_type, test_data.first));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == test_data.second);
    }

    SECTION("double quoted: invalid unicode escapings") {
        fkyaml::detail::lexical_token_t lex_type {fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR};
        auto token = GENERATE(
            fkyaml::detail::str_view("\\xw"),
            fkyaml::detail::str_view("\\x+"),
            fkyaml::detail::str_view("\\x="),
            fkyaml::detail::str_view("\\x^"),
            fkyaml::detail::str_view("\\x{"),
            fkyaml::detail::str_view("\\Q"));

        REQUIRE_THROWS_AS(scalar_parser.parse_flow(lex_type, tag_type, token), fkyaml::parse_error);
    }
}

TEST_CASE("ScalarParser_BlockLiteralScalar") {
    fkyaml::node node {};
    fkyaml::detail::scalar_parser<fkyaml::node> scalar_parser {0, 0};
    fkyaml::detail::lexical_token_t lex_type {fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR};
    fkyaml::detail::tag_t tag_type {fkyaml::detail::tag_t::NONE};
    fkyaml::detail::block_scalar_header header {};

    SECTION("empty literal string scalar with strip chomping") {
        fkyaml::detail::str_view token = "  \n";
        header.chomp = fkyaml::detail::chomping_indicator_t::STRIP;
        header.indent = 3;

        REQUIRE_NOTHROW(node = scalar_parser.parse_block(lex_type, tag_type, token, header));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == "");
    }

    SECTION("empty literal string scalar with clip chomping") {
        fkyaml::detail::str_view token = "  \n";
        header.chomp = fkyaml::detail::chomping_indicator_t::CLIP;
        header.indent = 3;

        REQUIRE_NOTHROW(node = scalar_parser.parse_block(lex_type, tag_type, token, header));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == "");
    }

    SECTION("empty literal string scalar with keep chomping") {
        fkyaml::detail::str_view token = "  \n";
        header.chomp = fkyaml::detail::chomping_indicator_t::KEEP;
        header.indent = 3;

        REQUIRE_NOTHROW(node = scalar_parser.parse_block(lex_type, tag_type, token, header));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == "\n");
    }

    SECTION("literal scalar with the first line being more indented than the indicated level") {
        fkyaml::detail::str_view token = "    foo\n"
                                         "  bar\n";
        header.chomp = fkyaml::detail::chomping_indicator_t::CLIP;
        header.indent = 2;

        REQUIRE_NOTHROW(node = scalar_parser.parse_block(lex_type, tag_type, token, header));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == "  foo\nbar\n");
    }

    SECTION("literal string scalar") {
        fkyaml::detail::str_view token = "  foo\n"
                                         "  bar\n";
        header.chomp = fkyaml::detail::chomping_indicator_t::CLIP;
        header.indent = 2;

        REQUIRE_NOTHROW(node = scalar_parser.parse_block(lex_type, tag_type, token, header));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == "foo\nbar\n");
    }

    SECTION("literal string scalar with implicit indentation and strip chomping") {
        fkyaml::detail::str_view token = "\n"
                                         "  foo\n"
                                         "  bar\n"
                                         "\n"
                                         "  baz\n"
                                         "\n";
        header.chomp = fkyaml::detail::chomping_indicator_t::STRIP;
        header.indent = 2;

        REQUIRE_NOTHROW(node = scalar_parser.parse_block(lex_type, tag_type, token, header));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == "\nfoo\nbar\n\nbaz");
    }

    SECTION("literal string scalar with explicit indentation and strip chomping") {
        fkyaml::detail::str_view token = "\n"
                                         "  foo\n"
                                         "    bar\n"
                                         "\n"
                                         "  baz\n"
                                         "\n";
        header.chomp = fkyaml::detail::chomping_indicator_t::STRIP;
        header.indent = 2;

        REQUIRE_NOTHROW(node = scalar_parser.parse_block(lex_type, tag_type, token, header));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == "\nfoo\n  bar\n\nbaz");
    }

    SECTION("literal string scalar with implicit indentation and clip chomping") {
        fkyaml::detail::str_view token = "\n"
                                         "  foo\n"
                                         "  bar\n"
                                         "\n"
                                         "  baz\n"
                                         "\n";
        header.chomp = fkyaml::detail::chomping_indicator_t::CLIP;
        header.indent = 2;

        REQUIRE_NOTHROW(node = scalar_parser.parse_block(lex_type, tag_type, token, header));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == "\nfoo\nbar\n\nbaz\n");
    }

    SECTION("literal string scalar with explicit indentation and clip chomping") {
        fkyaml::detail::str_view token = "\n"
                                         "  foo\n"
                                         "    bar\n"
                                         "\n"
                                         "  baz\n"
                                         "\n";
        header.chomp = fkyaml::detail::chomping_indicator_t::STRIP;
        header.indent = 2;

        REQUIRE_NOTHROW(node = scalar_parser.parse_block(lex_type, tag_type, token, header));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == "\nfoo\n  bar\n\nbaz");
    }

    SECTION("literal string scalar with clip chomping and no trailing newlines") {
        fkyaml::detail::str_view token = "  foo\n"
                                         "  bar\n"
                                         "\n"
                                         "  baz";
        header.chomp = fkyaml::detail::chomping_indicator_t::STRIP;
        header.indent = 2;

        REQUIRE_NOTHROW(node = scalar_parser.parse_block(lex_type, tag_type, token, header));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == "foo\nbar\n\nbaz");
    }

    SECTION("literal string scalar with implicit indentation and keep chomping") {
        fkyaml::detail::str_view token = "\n"
                                         "  foo\n"
                                         "  bar\n"
                                         "\n"
                                         "  baz\n"
                                         "\n";
        header.chomp = fkyaml::detail::chomping_indicator_t::KEEP;
        header.indent = 2;

        REQUIRE_NOTHROW(node = scalar_parser.parse_block(lex_type, tag_type, token, header));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == "\nfoo\nbar\n\nbaz\n\n");
    }

    SECTION("literal string scalar with explicit indentation and keep chomping") {
        fkyaml::detail::str_view token = "  foo\n"
                                         "    bar\n"
                                         "\n"
                                         "  baz\n"
                                         "\n";
        header.chomp = fkyaml::detail::chomping_indicator_t::KEEP;
        header.indent = 2;

        REQUIRE_NOTHROW(node = scalar_parser.parse_block(lex_type, tag_type, token, header));
        REQUIRE(node.is_string());
        REQUIRE(node.get_value_ref<std::string&>() == "foo\n  bar\n\nbaz\n\n");
    }
}
