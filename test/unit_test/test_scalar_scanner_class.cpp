//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.8
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("ScalarScanner_Empty") {
    REQUIRE(fkyaml::detail::scalar_scanner::scan("") == fkyaml::detail::lexical_token_t::STRING_VALUE);
}

TEST_CASE("ScalarScanner_NullValue") {
    auto token = GENERATE(std::string("~"), std::string("null"), std::string("Null"), std::string("NULL"));
    REQUIRE(fkyaml::detail::scalar_scanner::scan(token) == fkyaml::detail::lexical_token_t::NULL_VALUE);
}

TEST_CASE("ScalarScanner_BooleanValue") {
    auto token = GENERATE(
        std::string("true"),
        std::string("True"),
        std::string("TRUE"),
        std::string("false"),
        std::string("False"),
        std::string("FALSE"));
    REQUIRE(fkyaml::detail::scalar_scanner::scan(token) == fkyaml::detail::lexical_token_t::BOOLEAN_VALUE);
}

TEST_CASE("ScalarScanner_IntegerNumberValue") {
    auto token = GENERATE(
        std::string("-1234"),
        std::string("-853259"),
        std::string("-1"),
        std::string("0"),
        std::string("643"),
        std::string("+120"),
        std::string("0o27"),
        std::string("0o5"),
        std::string("0o77772"),
        std::string("0o672"),
        std::string("0xA04F"),
        std::string("0xa7F3"),
        std::string("0xFf29Bc"));
    REQUIRE(fkyaml::detail::scalar_scanner::scan(token) == fkyaml::detail::lexical_token_t::INTEGER_VALUE);
}

TEST_CASE("ScalarScanner_FloatNumberValue") {
    auto token = GENERATE(
        std::string(".inf"),
        std::string(".Inf"),
        std::string(".INF"),
        std::string(".nan"),
        std::string(".NaN"),
        std::string(".NAN"),
        std::string("-.inf"),
        std::string("-.Inf"),
        std::string("-.INF"),
        std::string("-1.234"),
        std::string("567.8"),
        std::string("0.24"),
        std::string("9.8e-3"),
        std::string("3.95E3"),
        std::string("1.863e+3"));
    REQUIRE(fkyaml::detail::scalar_scanner::scan(token) == fkyaml::detail::lexical_token_t::FLOAT_NUMBER_VALUE);
}

TEST_CASE("ScalarScanner_StringValue") {
    auto token = GENERATE(
        std::string("abc"),
        std::string("0th"),
        std::string("0123"),
        std::string("1.non-digit"),
        std::string("1exe"),
        std::string("0oabc"),
        std::string("0xyz"));
    REQUIRE(fkyaml::detail::scalar_scanner::scan(token) == fkyaml::detail::lexical_token_t::STRING_VALUE);
}
