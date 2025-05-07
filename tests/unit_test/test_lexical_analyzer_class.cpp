//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
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

    SECTION("valid document end marker") {
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

    SECTION("invalid document end marker") {
        fkyaml::detail::lexical_analyzer lexer("...invalid");
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_Comment") {
    fkyaml::detail::lexical_token token;

    SECTION("valid comments") {
        auto input = GENERATE(
            fkyaml::detail::str_view("# comment"),
            fkyaml::detail::str_view(" # comment"),
            fkyaml::detail::str_view("\t# comment\n"),
            fkyaml::detail::str_view("\n# comment"));
        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("valid tab comments") {
        fkyaml::detail::str_view input("a #comment");
        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.str == "a");

        fkyaml::detail::str_view tab_input("a\t#comment");
        fkyaml::detail::lexical_analyzer tab_lexer(tab_input);
        REQUIRE_NOTHROW(token = tab_lexer.get_next_token());
        REQUIRE(token.str == "a");
    }

    // regression test for https://github.com/fktn-k/fkYAML/pull/469
    SECTION("invalid comments") {
        fkyaml::detail::str_view input("\'foo\'#invalid");
        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SINGLE_QUOTED_SCALAR);
        REQUIRE(token.str == "foo");
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
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

    SECTION("colon with an always-safe character (block)") {
        fkyaml::detail::lexical_analyzer lexer(":test");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == ":test");
    }

    SECTION("colon with an always-safe character (flow)") {
        fkyaml::detail::lexical_analyzer lexer("[:test]");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_BEGIN);
        lexer.set_context_state(true);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == ":test");
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_END);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        lexer.set_context_state(false);
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
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
    fkyaml::detail::lexical_token token;

    SECTION("single line") {
        auto input = GENERATE(
            fkyaml::detail::str_view("test"),
            fkyaml::detail::str_view("test "),
            fkyaml::detail::str_view("test:"),
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
            fkyaml::detail::str_view("+123"),
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
            fkyaml::detail::str_view(".NAN_VALUE"),

            // "---" and "..." not at the beginning of a line is a scalar
            fkyaml::detail::str_view(" ---"),
            fkyaml::detail::str_view(" ..."),
            fkyaml::detail::str_view(" ...this is valid"));

        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + input.find_first_not_of(' '));
        REQUIRE(token.str.end() == input.begin() + input.find_last_not_of(' ') + 1);
    }

    SECTION("multiline without final newline") {
        fkyaml::detail::str_view input = "  foo\n"
                                         "   bar\n"
                                         "     baz";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 2);
        REQUIRE(token.str.end() == input.end());
    }

    SECTION("multiline with final newline") {
        fkyaml::detail::str_view input = "  foo\n"
                                         "   bar\n"
                                         "     baz\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 2);
        REQUIRE(token.str.end() == input.end() - 1);
    }

    SECTION("multiline as an implicit mapping value") {
        fkyaml::detail::str_view input = "  foo: foo\n"
                                         "   bar\n"
                                         "     baz\n"
                                         "  qux";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 2);
        REQUIRE(token.str.end() == input.begin() + 5);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 7);
        REQUIRE(token.str.end() == input.end() - 6);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.end() - 3);
        REQUIRE(token.str.end() == input.end());
    }

    SECTION("multiline as a block sequence item") {
        fkyaml::detail::str_view input = "  - foo\n"
                                         "   bar\n"
                                         "     baz";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 4);
        REQUIRE(token.str.end() == input.end());
    }

    SECTION("multiline as a block sequence item and an implicit mapping value") {
        fkyaml::detail::str_view input = "  - -foo: bar\n"
                                         "     baz\n"
                                         "   baz";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 4);
        REQUIRE(token.str.end() == input.begin() + 8);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 10);
        REQUIRE(token.str.end() == input.end() - 7);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.end() - 3);
        REQUIRE(token.str.end() == input.end());
    }

    SECTION("multiline as an explicit mapping key") {
        fkyaml::detail::str_view input = "  ? foo\n"
                                         "   bar\n"
                                         "     baz";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::EXPLICIT_KEY_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 4);
        REQUIRE(token.str.end() == input.end());
    }

    SECTION("multiline as an explicit mapping key and an implicit mapping value") {
        fkyaml::detail::str_view input = "  ? ?foo: bar\n"
                                         "     baz\n"
                                         "   baz";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::EXPLICIT_KEY_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 4);
        REQUIRE(token.str.end() == input.begin() + 8);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 10);
        REQUIRE(token.str.end() == input.end() - 7);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.end() - 3);
        REQUIRE(token.str.end() == input.end());
    }

    SECTION("multiline as an explicit mapping value") {
        fkyaml::detail::str_view input = "  : foo\n"
                                         "   bar\n"
                                         "     baz";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 4);
        REQUIRE(token.str.end() == input.end());
    }

    SECTION("multiline as an explicit mapping value and an implicit mapping value") {
        fkyaml::detail::str_view input = "  : :foo: bar\n"
                                         "     bar\n"
                                         "   baz";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 4);
        REQUIRE(token.str.end() == input.begin() + 8);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 10);
        REQUIRE(token.str.end() == input.end() - 7);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.end() - 3);
        REQUIRE(token.str.end() == input.end());
    }

    SECTION("multiline as a block sequence item and an explicit mapping value") {
        fkyaml::detail::str_view input = "  ? - foo\n"
                                         "     bar\n"
                                         "     baz";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::EXPLICIT_KEY_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_BLOCK_PREFIX);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 6);
        REQUIRE(token.str.end() == input.end());
    }

    SECTION("multiline with less indented line") {
        fkyaml::detail::str_view input = "  foo\n"
                                         "   bar\n"
                                         " baz";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 2);
        REQUIRE(token.str.end() == input.end() - 5);
    }

    SECTION("multiline with equally indented line") {
        fkyaml::detail::str_view input = "  foo\n"
                                         "   bar\n"
                                         "  baz";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 2);
        REQUIRE(token.str.end() == input.end() - 6);
    }

    SECTION("multiline with empty line") {
        fkyaml::detail::str_view input = "  foo\n"
                                         " \t   \n"
                                         "   bar\n"
                                         "\n"
                                         "   baz";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str.begin() == input.begin() + 2);
        REQUIRE(token.str.end() == input.end());
    }
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
    auto input = GENERATE(
        fkyaml::detail::str_view("\"\""),
        fkyaml::detail::str_view("\"\\\"\""),
        fkyaml::detail::str_view("\"foo bar\""),
        fkyaml::detail::str_view("\"foo\tbar\""),
        fkyaml::detail::str_view("\"foo's bar\""),
        fkyaml::detail::str_view("\"foo:bar\""),
        fkyaml::detail::str_view("\"foo,bar\""),
        fkyaml::detail::str_view("\"foo]bar\""),
        fkyaml::detail::str_view("\"foo}bar\""),
        fkyaml::detail::str_view("\"\\x30\\x2B\\x6d\""),

        fkyaml::detail::str_view("\"foo\nbar\""),
        fkyaml::detail::str_view("\"foo \t\n \tbar\""),
        fkyaml::detail::str_view("\"foo\n\n \t\nbar\""),
        fkyaml::detail::str_view("\"\nfoo\n\n \t\nbar\""),
        fkyaml::detail::str_view("\"foo\nbar\n\""),
        fkyaml::detail::str_view("\"foo\\\nbar\""),
        fkyaml::detail::str_view("\"foo \t\\\nbar\""),
        fkyaml::detail::str_view("\"\\\n  foo \t\\\n\tbar\t  \t\\\n\""));

    fkyaml::detail::lexical_analyzer lexer(input);
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR);
    REQUIRE(token.str.begin() == input.begin() + 1);
    REQUIRE(token.str.end() == input.end() - 1);
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
    auto input = GENERATE(
        fkyaml::detail::str_view("\"\\x00\""),
        fkyaml::detail::str_view("\"\\x40\""),
        fkyaml::detail::str_view("\"\\x7F\""),
        fkyaml::detail::str_view("\"\\u0000\""),
        fkyaml::detail::str_view("\"\\u0040\""),
        fkyaml::detail::str_view("\"\\u007F\""),
        fkyaml::detail::str_view("\"\\u0080\""),
        fkyaml::detail::str_view("\"\\u0400\""),
        fkyaml::detail::str_view("\"\\u07FF\""),
        fkyaml::detail::str_view("\"\\u0800\""),
        fkyaml::detail::str_view("\"\\u8000\""),
        fkyaml::detail::str_view("\"\\uFFFF\""),
        fkyaml::detail::str_view("\"\\U00000000\""),
        fkyaml::detail::str_view("\"\\U00000040\""),
        fkyaml::detail::str_view("\"\\U0000007F\""),
        fkyaml::detail::str_view("\"\\U00000080\""),
        fkyaml::detail::str_view("\"\\U00000400\""),
        fkyaml::detail::str_view("\"\\U000007FF\""),
        fkyaml::detail::str_view("\"\\U00000800\""),
        fkyaml::detail::str_view("\"\\U00008000\""),
        fkyaml::detail::str_view("\"\\U0000FFFF\""),
        fkyaml::detail::str_view("\"\\U00010000\""),
        fkyaml::detail::str_view("\"\\U00080000\""),
        fkyaml::detail::str_view("\"\\U0010FFFF\""));

    fkyaml::detail::lexical_analyzer lexer(input);
    fkyaml::detail::lexical_token token;

    REQUIRE_NOTHROW(token = lexer.get_next_token());
    REQUIRE(token.type == fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR);
    REQUIRE(token.str.begin() == input.begin() + 1);
    REQUIRE(token.str.end() == input.end() - 1);
}

TEST_CASE("LexicalAnalyzer_UnclosedQuotedString") {
    auto buffer = GENERATE(fkyaml::detail::str_view("\"test"), fkyaml::detail::str_view("\'test"));

    fkyaml::detail::lexical_analyzer lexer(buffer);
    REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
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

    SECTION("plain scalar") {
        fkyaml::detail::lexical_analyzer lexer(buffer);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("single quoted scalar") {
        std::string single_quoted = "\'" + buffer + "\'";
        fkyaml::detail::lexical_analyzer lexer(single_quoted);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("double quoted scalar") {
        std::string double_quoted = "\"" + buffer + "\"";
        fkyaml::detail::lexical_analyzer lexer(double_quoted);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }
}

TEST_CASE("LexicalAnalyzer_LiteralStringScalar") {
    fkyaml::detail::lexical_token token;

    SECTION("empty literal string scalar with strip chomping") {
        const char input[] = "|-\n"
                             "  \n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[3]);
        REQUIRE(token.str.end() == &input[0] + 6);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::STRIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("empty literal string scalar with clip chomping") {
        const char input[] = "|\n"
                             "  \n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[2]);
        REQUIRE(token.str.end() == &input[0] + 5);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("empty literal string scalar with keep chomping") {
        const char input[] = "|+\n"
                             "  \n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[3]);
        REQUIRE(token.str.end() == &input[0] + 6);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::KEEP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("literal string scalar with 0 indent level.") {
        const char input[] = "|0\n"
                             "foo";

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("a leading empty line is more indented") {
        const char input[] = "|\n"
                             "   \n"
                             "  foo";

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("a leading empty line contains a tab") {
        const char input[] = "|\n"
                             "  \t \n"
                             "  foo";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[2]);
        REQUIRE(token.str.end() == &input[0] + 12);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("a following content line is less indented") {
        const char input[] = "|\n"
                             "  foo\n"
                             " bar";

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("less indented literal string scalar") {
        const char input[] = "|2\n"
                             " foo";

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("less indented literal string scalar with preceding empty line") {
        const char input[] = "|2\n"
                             "\n"
                             " foo\n";

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("literal scalar with no newline on first and last content line") {
        const char input[] = "|\n"
                             "  foo";

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[2]);
        REQUIRE(token.str.end() == &input[0] + 7);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("literal scalar with the first line being more indented than the indicated level") {
        const char input[] = "|2\n"
                             "\n"
                             "    foo\n"
                             "  bar\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[3]);
        REQUIRE(token.str.end() == &input[0] + 18);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("literal string scalar") {
        const char input[] = "|\n"
                             "  foo\n"
                             "  bar\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[2]);
        REQUIRE(token.str.end() == &input[0] + 14);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
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
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[3]);
        REQUIRE(token.str.end() == &input[0] + 24);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::STRIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
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
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[4]);
        REQUIRE(token.str.end() == &input[0] + 26);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::STRIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
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
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[2]);
        REQUIRE(token.str.end() == &input[0] + 23);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
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
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[3]);
        REQUIRE(token.str.end() == &input[0] + 25);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("literal string scalar with clip chomping and no trailing newlines") {
        const char input[] = "|2\n"
                             "  foo\n"
                             "    bar\n"
                             "\n"
                             "  baz";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[3]);
        REQUIRE(token.str.end() == &input[0] + 23);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
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
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[3]);
        REQUIRE(token.str.end() == &input[0] + 24);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::KEEP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
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
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[4]);
        REQUIRE(token.str.end() == &input[0] + 26);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::KEEP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("literal string scalar with trailing spaces/tabs after the block scalar header.") {
        auto input = GENERATE(
            fkyaml::detail::str_view("|2  \n  foo\n"),
            fkyaml::detail::str_view("|2\t\t\n  foo\n"),
            fkyaml::detail::str_view("|2 # comment\n  foo\n"));
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[0] + input.find('\n') + 1);
        REQUIRE(token.str.end() == &input[0] + input.size());
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
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
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_FOLDED_SCALAR);
        REQUIRE(token.str.begin() == &input[3]);
        REQUIRE(token.str.end() == &input[0] + 6);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::STRIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("empty folded string scalar with clip chomping") {
        const char input[] = ">\n"
                             "  \n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_FOLDED_SCALAR);
        REQUIRE(token.str.begin() == &input[2]);
        REQUIRE(token.str.end() == &input[0] + 5);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("empty folded string scalar with keep chomping") {
        const char input[] = ">+\n"
                             "  \n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_FOLDED_SCALAR);
        REQUIRE(token.str.begin() == &input[3]);
        REQUIRE(token.str.end() == &input[0] + 6);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::KEEP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("folded string scalar with 0 indent level") {
        const char input[] = ">0\n"
                             "foo";

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("a leading empty line is more indented") {
        const char input[] = ">\n"
                             "   \n"
                             "  foo";

        fkyaml::detail::lexical_analyzer lexer(input);
        REQUIRE_THROWS_AS(lexer.get_next_token(), fkyaml::parse_error);
    }

    SECTION("a leading empty line contains a tab") {
        const char input[] = ">\n"
                             "  \t \n"
                             "  foo";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_FOLDED_SCALAR);
        REQUIRE(token.str.begin() == &input[2]);
        REQUIRE(token.str.end() == &input[0] + 12);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("a following content line is less indented") {
        const char input[] = ">\n"
                             "  foo\n"
                             " bar";

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
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_FOLDED_SCALAR);
        REQUIRE(token.str.begin() == &input[3]);
        REQUIRE(token.str.end() == &input[0] + 17);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("folded string scalar with the non-first line being more indented than the indicated level") {
        const char input[] = ">2\n"
                             "  foo\n"
                             "    bar\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_FOLDED_SCALAR);
        REQUIRE(token.str.begin() == &input[3]);
        REQUIRE(token.str.end() == &input[0] + 17);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
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
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_FOLDED_SCALAR);
        REQUIRE(token.str.begin() == &input[2]);
        REQUIRE(token.str.end() == &input[0] + 20);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("folded string scalar with implicit indentation and strip chomping") {
        const char input[] = ">-\n"
                             "  foo\n"
                             "  bar\n"
                             " \n"
                             "\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_FOLDED_SCALAR);
        REQUIRE(token.str.begin() == &input[3]);
        REQUIRE(token.str.end() == &input[0] + 18);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::STRIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("folded string scalar with implicit indentation and clip chomping") {
        const char input[] = ">\n"
                             "  foo\n"
                             "  bar\n"
                             "  \n"
                             "\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_FOLDED_SCALAR);
        REQUIRE(token.str.begin() == &input[2]);
        REQUIRE(token.str.end() == &input[0] + 18);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("folded string scalar with implicit indentation and keep chomping") {
        const char input[] = ">+\n"
                             "  foo\n"
                             "  bar\n"
                             " \n"
                             "\n";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_FOLDED_SCALAR);
        REQUIRE(token.str.begin() == &input[3]);
        REQUIRE(token.str.end() == &input[0] + 18);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::KEEP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
    }

    SECTION("folded string scalar with trailing spaces/tabs/comments after the block scalar header.") {
        auto input = GENERATE(
            fkyaml::detail::str_view(">2  \n  foo\n"),
            fkyaml::detail::str_view(">2\t\t\n  foo\n"),
            fkyaml::detail::str_view(">2 # comment\n  foo\n"));
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_FOLDED_SCALAR);
        REQUIRE(token.str.begin() == &input[0] + input.find('\n') + 1);
        REQUIRE(token.str.end() == &input[0] + input.size());
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::END_OF_BUFFER);
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

    SECTION("\':\' is preceded by JSON-like keys and followed by values adjacent to it") {
        fkyaml::detail::str_view input = "{\n"
                                         "  \"foo\":123,\n"
                                         "  \'bar\':true,\n"
                                         "  [baz]:3.14,\n"
                                         "  {\"qux\":false}:null\n"
                                         "}";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);

        lexer.set_context_state(true);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR);
        REQUIRE(token.str == "foo");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "123");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SINGLE_QUOTED_SCALAR);
        REQUIRE(token.str == "bar");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "true");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "baz");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::SEQUENCE_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "3.14");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::VALUE_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_BEGIN);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::DOUBLE_QUOTED_SCALAR);
        REQUIRE(token.str == "qux");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "false");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::MAPPING_FLOW_END);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "null");

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
        char input[] = "test: |\n  a block literal scalar.\nfoo: \'bar\'\npi: 3.14";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_LITERAL_SCALAR);
        REQUIRE(token.str.begin() == &input[8]);
        REQUIRE(token.str.end() == &input[34]);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

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
        char input[] = "test: >\n  a block folded scalar.\nfoo: \'bar\'\npi: 3.14";
        fkyaml::detail::lexical_analyzer lexer(input);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::PLAIN_SCALAR);
        REQUIRE(token.str == "test");

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::KEY_SEPARATOR);

        REQUIRE_NOTHROW(token = lexer.get_next_token());
        REQUIRE(token.type == fkyaml::detail::lexical_token_t::BLOCK_FOLDED_SCALAR);
        REQUIRE(token.str.begin() == &input[8]);
        REQUIRE(token.str.end() == &input[33]);
        REQUIRE(lexer.get_block_scalar_header().chomp == fkyaml::detail::chomping_indicator_t::CLIP);
        REQUIRE(lexer.get_block_scalar_header().indent == 2);

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
