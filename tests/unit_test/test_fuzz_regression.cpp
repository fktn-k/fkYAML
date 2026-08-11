//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <doctest/doctest.h>
#include <fkYAML/node.hpp>

TEST_CASE("FuzzRegression") {
    fkyaml::node root;
    const char* p_begin = nullptr;
    const char* p_end = nullptr;

    SUBCASE("malformed UTF-16 input") {
        uint8_t input[] = {0x01, 0x00, 0x0a};
        p_begin = reinterpret_cast<const char*>(input);
        p_end = p_begin + sizeof(input);
        REQUIRE_THROWS_AS(root = fkyaml::node::deserialize(p_begin, p_end), fkyaml::invalid_encoding);
    }

    SUBCASE("invalid context transition") {
        const char input[] = "o- 1\n"
                             "- trueot:\n"
                             "  ch\n"
                             "i";
        p_begin = input;
        p_end = input + sizeof(input) - 1;
        REQUIRE_THROWS_AS(root = fkyaml::node::deserialize(p_begin, p_end), fkyaml::parse_error);
    }

    SUBCASE("invalid block sequence entry") {
        const char input[] = "- 1\n"
                             "- two\n"
                             "- tru 1\n"
                             "- two\n"
                             "-e";
        p_begin = input;
        p_end = input + sizeof(input) - 1;
        REQUIRE_THROWS_AS(root = fkyaml::node::deserialize(p_begin, p_end), fkyaml::parse_error);
    }

    SUBCASE("invalid flow sequence after scalar root") {
        const char input[] = "ke\n[";
        p_begin = input;
        p_end = input + sizeof(input) - 1;
        REQUIRE_THROWS_AS(root = fkyaml::node::deserialize(p_begin, p_end), fkyaml::parse_error);
    }

    SUBCASE("mismatched flow endings") {
        const char input[] = "{a: 1,  [2 b:,3}\n";
        p_begin = input;
        p_end = input + sizeof(input) - 1;
        REQUIRE_THROWS_AS(root = fkyaml::node::deserialize(p_begin, p_end), fkyaml::parse_error);
    }

    SUBCASE("invalid block scalar indentation") {
        const char input[] = "root:\n"
                             "  c]h:di     |- \n"
                             " a  ) - b\n";
        p_begin = input;
        p_end = input + sizeof(input) - 1;
        REQUIRE_THROWS_AS(root = fkyaml::node::deserialize(p_begin, p_end), fkyaml::parse_error);
    }

    SUBCASE("invalid block scalar header") {
        auto input = GENERATE(std::string(">"), std::string("|"));
        p_begin = input.data();
        p_end = input.data() + input.size();
        REQUIRE_THROWS_AS(root = fkyaml::node::deserialize(p_begin, p_end), fkyaml::parse_error);
    }

    SUBCASE("invalid colons") {
        const char input[] = ":\n"
                             ":";
        p_begin = input;
        p_end = input + sizeof(input) - 1;
        REQUIRE_THROWS_AS(root = fkyaml::node::deserialize(p_begin, p_end), fkyaml::parse_error);
    }

    SUBCASE("invalid explicit key indicator") {
        const char input[] = "a\n"
                             "? ";
        p_begin = input;
        p_end = input + sizeof(input) - 1;
        REQUIRE_THROWS_AS(root = fkyaml::node::deserialize(p_begin, p_end), fkyaml::parse_error);
    }

    SUBCASE("explicit key indicator without a parent context") {
        const char input[] = "a  ? a";
        p_begin = input;
        p_end = input + sizeof(input) - 1;
        REQUIRE_THROWS_AS(root = fkyaml::node::deserialize(p_begin, p_end), fkyaml::parse_error);
    }

    SUBCASE("key separator without a parent context") {
        uint8_t input[] = {0x0D, 0x3B, 0x00, 0x0A, 0x0A, 0x4A, 0x0A, 0x0A, 0x26, 0x7C, 0x0A, 0x3A};
        p_begin = reinterpret_cast<const char*>(input);
        p_end = p_begin + sizeof(input);
        REQUIRE_THROWS_AS(root = fkyaml::node::deserialize(p_begin, p_end), fkyaml::parse_error);
    }

    SUBCASE("mapping key without a parent context") {
        uint8_t input[] = {0x26, 0x78, 0x00, 0x61, 0x5B, 0x5D, 0x0A, 0x70, 0x72, 0x6F, 0x7B, 0x64, 0x2B,
                           0x3A, 0x20, 0x3E, 0x72, 0x6F, 0x7B, 0x64, 0x2B, 0x3A, 0x7B, 0x32, 0x6A};
        p_begin = reinterpret_cast<const char*>(input);
        p_end = p_begin + sizeof(input);
        REQUIRE_THROWS_AS(root = fkyaml::node::deserialize(p_begin, p_end), fkyaml::parse_error);
    }
}
