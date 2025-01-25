//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("URIEncoding_Validate") {
    SECTION("valid URI characters") {
        auto input = GENERATE(
            std::string(""),
            std::string("%00%99%AF%af"),
            std::string(":/?#[]@"),
            std::string("!$&\'()*+,;="),
            std::string("-._~"),
            std::string("0123456789"),
            std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
            std::string("abcdefghijklmnopqrstuvwxyz"));
        REQUIRE(fkyaml::detail::uri_encoding::validate(input.c_str(), input.c_str() + input.size()));
    }

    SECTION("invalid URI characters") {
        auto input = GENERATE(
            std::string("%//"),
            std::string("%::"),
            std::string("%@@"),
            std::string("%[["),
            std::string("%``"),
            std::string("%{{"),
            std::string("%"),
            std::string("%0"),
            std::string({'\x00'}),
            std::string("\x01"),
            std::string("\x02"),
            std::string("\x03"),
            std::string("\x04"),
            std::string("\x05"),
            std::string("\x06"),
            std::string("\x07"),
            std::string("\x08"),
            std::string("\x09"),
            std::string("\x0A"),
            std::string("\x0B"),
            std::string("\x0C"),
            std::string("\x0D"),
            std::string("\x0E"),
            std::string("\x0F"),
            std::string("\x10"),
            std::string("\x11"),
            std::string("\x12"),
            std::string("\x13"),
            std::string("\x14"),
            std::string("\x15"),
            std::string("\x16"),
            std::string("\x17"),
            std::string("\x18"),
            std::string("\x19"),
            std::string("\x1A"),
            std::string("\x1B"),
            std::string("\x1C"),
            std::string("\x1D"),
            std::string("\x1E"),
            std::string("\x1F"),
            std::string(" "),
            std::string("\""),
            std::string("%"),
            std::string("<"),
            std::string(">"),
            std::string("\\"),
            std::string("^"),
            std::string("`"),
            std::string("|"),
            std::string("\x7F"));
        REQUIRE_FALSE(fkyaml::detail::uri_encoding::validate(input.c_str(), input.c_str() + input.size()));
    }
}
