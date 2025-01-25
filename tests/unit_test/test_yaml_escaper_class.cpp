//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <utility>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("YamlEscaper_Unescape") {
    SECTION("valid escape sequence") {
        using test_data_t = std::pair<fkyaml::detail::str_view, std::string>;
        auto test_data = GENERATE(
            test_data_t {"\\a", "\a"},
            test_data_t {"\\b", "\b"},
            test_data_t {"\\t", "\t"},
            test_data_t {"\\\u0009", "\t"},
            test_data_t {"\\n", "\n"},
            test_data_t {"\\v", "\v"},
            test_data_t {"\\f", "\f"},
            test_data_t {"\\r", "\r"},
            test_data_t {"\\e", "\u001b"},
            test_data_t {"\\ ", " "},
            test_data_t {"\\\"", "\""},
            test_data_t {"\\/", "/"},
            test_data_t {"\\\\", "\\"},
            test_data_t {"\\N", "\u0085"},
            test_data_t {"\\_", "\u00a0"},
            test_data_t {"\\L", "\u2028"},
            test_data_t {"\\P", "\u2029"},
            test_data_t {"\\x00", {char(0)}},
            test_data_t {"\\x40", {char(0x40)}},
            test_data_t {"\\x7F", {char(0x7F)}},
            test_data_t {"\\u0000", {char(0)}},
            test_data_t {"\\u0040", {char(0x40)}},
            test_data_t {"\\u007F", {char(0x7F)}},
            test_data_t {"\\U00000000", {char(0)}},
            test_data_t {"\\U00000040", {char(0x40)}},
            test_data_t {"\\U0000007F", {char(0x7F)}});

        std::string buff {};
        auto begin_itr = test_data.first.begin();
        auto end_itr = test_data.first.end();
        REQUIRE(fkyaml::detail::yaml_escaper::unescape(begin_itr, end_itr, buff));
        REQUIRE(buff == test_data.second);
    }

    SECTION("invalid escape sequence") {
        auto input = GENERATE(
            fkyaml::detail::str_view("\\Q"),
            fkyaml::detail::str_view("\\xw"),
            fkyaml::detail::str_view("\\x+"),
            fkyaml::detail::str_view("\\u="),
            fkyaml::detail::str_view("\\U^"),
            fkyaml::detail::str_view("\\x{"));

        std::string buff {};
        auto begin_itr = input.cbegin();
        auto end_itr = input.cend();
        REQUIRE_FALSE(fkyaml::detail::yaml_escaper::unescape(begin_itr, end_itr, buff));
    }

    SECTION("invalid UTF encoding") {
        fkyaml::detail::str_view input = "\\U00110000";
        auto begin_itr = input.cbegin();
        auto end_itr = input.cend();
        std::string buff {};
        REQUIRE_THROWS_AS(fkyaml::detail::yaml_escaper::unescape(begin_itr, end_itr, buff), fkyaml::invalid_encoding);
    }
}

TEST_CASE("YamlEscaper_Escape") {
    using test_data_t = std::pair<std::string, std::string>;
    auto test_data = GENERATE(
        test_data_t {{char(0x01)}, "\\u0001"},
        test_data_t {{char(0x02)}, "\\u0002"},
        test_data_t {{char(0x03)}, "\\u0003"},
        test_data_t {{char(0x04)}, "\\u0004"},
        test_data_t {{char(0x05)}, "\\u0005"},
        test_data_t {{char(0x06)}, "\\u0006"},
        test_data_t {"\a", "\\a"},
        test_data_t {"\b", "\\b"},
        test_data_t {"\t", "\\t"},
        test_data_t {"\n", "\\n"},
        test_data_t {"\v", "\\v"},
        test_data_t {"\f", "\\f"},
        test_data_t {"\r", "\\r"},
        test_data_t {{char(0x0E)}, "\\u000E"},
        test_data_t {{char(0x0F)}, "\\u000F"},
        test_data_t {{char(0x10)}, "\\u0010"},
        test_data_t {{char(0x11)}, "\\u0011"},
        test_data_t {{char(0x12)}, "\\u0012"},
        test_data_t {{char(0x13)}, "\\u0013"},
        test_data_t {{char(0x14)}, "\\u0014"},
        test_data_t {{char(0x15)}, "\\u0015"},
        test_data_t {{char(0x16)}, "\\u0016"},
        test_data_t {{char(0x17)}, "\\u0017"},
        test_data_t {{char(0x18)}, "\\u0018"},
        test_data_t {{char(0x19)}, "\\u0019"},
        test_data_t {{char(0x1A)}, "\\u001A"},
        test_data_t {{char(0x1B)}, "\\e"},
        test_data_t {{char(0x1C)}, "\\u001C"},
        test_data_t {{char(0x1D)}, "\\u001D"},
        test_data_t {{char(0x1E)}, "\\u001E"},
        test_data_t {{char(0x1F)}, "\\u001F"},
        test_data_t {"\"", "\\\""},
        test_data_t {"\\", "\\\\"},
        test_data_t {{char(0xC2u), char(0x85u)}, "\\N"},
        test_data_t {{char(0xC2u), char(0xA0u)}, "\\_"},
        test_data_t {{char(0xE2u), char(0x80u), char(0xA8u)}, "\\L"},
        test_data_t {{char(0xE2u), char(0x80u), char(0xA9u)}, "\\P"});

    bool is_escaped = false;
    fkyaml::detail::str_view input = test_data.first;
    REQUIRE(fkyaml::detail::yaml_escaper::escape(input.begin(), input.end(), is_escaped) == test_data.second);
    REQUIRE(is_escaped);
}
