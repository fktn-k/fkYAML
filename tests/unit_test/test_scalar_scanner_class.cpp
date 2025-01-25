//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("ScalarScanner_Empty") {
    fkyaml::detail::str_view token = "";
    REQUIRE(fkyaml::detail::scalar_scanner::scan(token.begin(), token.end()) == fkyaml::node_type::STRING);
}

TEST_CASE("ScalarScanner_NullValue") {
    auto token = GENERATE(
        fkyaml::detail::str_view("~"),
        fkyaml::detail::str_view("null"),
        fkyaml::detail::str_view("Null"),
        fkyaml::detail::str_view("NULL"));
    REQUIRE(fkyaml::detail::scalar_scanner::scan(token.begin(), token.end()) == fkyaml::node_type::NULL_OBJECT);
}

TEST_CASE("ScalarScanner_BooleanValue") {
    auto token = GENERATE(
        fkyaml::detail::str_view("true"),
        fkyaml::detail::str_view("True"),
        fkyaml::detail::str_view("TRUE"),
        fkyaml::detail::str_view("false"),
        fkyaml::detail::str_view("False"),
        fkyaml::detail::str_view("FALSE"));
    REQUIRE(fkyaml::detail::scalar_scanner::scan(token.begin(), token.end()) == fkyaml::node_type::BOOLEAN);
}

TEST_CASE("ScalarScanner_IntegerNumberValue") {
    auto token = GENERATE(
        fkyaml::detail::str_view("-1234"),
        fkyaml::detail::str_view("-853259"),
        fkyaml::detail::str_view("-1"),
        fkyaml::detail::str_view("0"),
        fkyaml::detail::str_view("643"),
        fkyaml::detail::str_view("+120"),
        fkyaml::detail::str_view("0o27"),
        fkyaml::detail::str_view("0o5"),
        fkyaml::detail::str_view("0o77772"),
        fkyaml::detail::str_view("0o672"),
        fkyaml::detail::str_view("0xA04F"),
        fkyaml::detail::str_view("0xa7F3"),
        fkyaml::detail::str_view("0xFf29Bc"));
    REQUIRE(fkyaml::detail::scalar_scanner::scan(token.begin(), token.end()) == fkyaml::node_type::INTEGER);
}

TEST_CASE("ScalarScanner_FloatNumberValue") {
    auto token = GENERATE(
        fkyaml::detail::str_view(".inf"),
        fkyaml::detail::str_view(".Inf"),
        fkyaml::detail::str_view(".INF"),
        fkyaml::detail::str_view(".nan"),
        fkyaml::detail::str_view(".NaN"),
        fkyaml::detail::str_view(".NAN"),
        fkyaml::detail::str_view("-.inf"),
        fkyaml::detail::str_view("-.Inf"),
        fkyaml::detail::str_view("-.INF"),
        fkyaml::detail::str_view("+.inf"),
        fkyaml::detail::str_view("+.Inf"),
        fkyaml::detail::str_view("+.INF"),
        fkyaml::detail::str_view("-1.234"),
        fkyaml::detail::str_view("-21."),
        fkyaml::detail::str_view("-.123"),
        fkyaml::detail::str_view("+.123"),
        fkyaml::detail::str_view(".123"),
        fkyaml::detail::str_view("567.8"),
        fkyaml::detail::str_view("123."),
        fkyaml::detail::str_view("0.24"),
        fkyaml::detail::str_view("0."),
        fkyaml::detail::str_view("0e1"),
        fkyaml::detail::str_view("0E+1"),
        fkyaml::detail::str_view("0e-1"),
        fkyaml::detail::str_view("1e-1"),
        fkyaml::detail::str_view("2e+2"),
        fkyaml::detail::str_view("3e3"),
        fkyaml::detail::str_view("1.e3"),
        fkyaml::detail::str_view(".1e-3"),
        fkyaml::detail::str_view("-.1e-3"),
        fkyaml::detail::str_view("+.1e-3"),
        fkyaml::detail::str_view("9.8e-3"),
        fkyaml::detail::str_view("3.95E3"),
        fkyaml::detail::str_view("1.863e+3"));
    REQUIRE(fkyaml::detail::scalar_scanner::scan(token.begin(), token.end()) == fkyaml::node_type::FLOAT);
}

TEST_CASE("ScalarScanner_StringValue") {
    auto token = GENERATE(
        fkyaml::detail::str_view("nullValue"),
        fkyaml::detail::str_view("NullValue"),
        fkyaml::detail::str_view("NULL_VALUE"),
        fkyaml::detail::str_view("~Value"),
        fkyaml::detail::str_view("trueValue"),
        fkyaml::detail::str_view("TrueValue"),
        fkyaml::detail::str_view("TRUE_VALUE"),
        fkyaml::detail::str_view("falseValue"),
        fkyaml::detail::str_view("falsy"),
        fkyaml::detail::str_view("FalseValue"),
        fkyaml::detail::str_view("Falsy"),
        fkyaml::detail::str_view("FALSE_VALUE"),
        fkyaml::detail::str_view(".infValue"),
        fkyaml::detail::str_view(".InfValue"),
        fkyaml::detail::str_view(".INFValue"),
        fkyaml::detail::str_view(".nanValue"),
        fkyaml::detail::str_view(".NaNValue"),
        fkyaml::detail::str_view(".NANValue"),
        fkyaml::detail::str_view("-.infValue"),
        fkyaml::detail::str_view("-.InfValue"),
        fkyaml::detail::str_view("-.INFValue"),
        fkyaml::detail::str_view(".foo"),
        fkyaml::detail::str_view("abc"),
        fkyaml::detail::str_view("0th"),
        fkyaml::detail::str_view("1st"),
        fkyaml::detail::str_view("0e"),
        fkyaml::detail::str_view("0e+"),
        fkyaml::detail::str_view("0e-"),
        fkyaml::detail::str_view("0E"),
        fkyaml::detail::str_view("0123"),
        fkyaml::detail::str_view("1.2.3"),
        fkyaml::detail::str_view("1.23e"),
        fkyaml::detail::str_view("1.23eE"),
        fkyaml::detail::str_view("1.23E-e"),
        fkyaml::detail::str_view("1.2e-z"),
        fkyaml::detail::str_view("1.non-digit"),
        fkyaml::detail::str_view("-.foo"),
        fkyaml::detail::str_view("1exe"),
        fkyaml::detail::str_view("0oabc"),
        fkyaml::detail::str_view("0xyz"));
    REQUIRE(fkyaml::detail::scalar_scanner::scan(token.begin(), token.end()) == fkyaml::node_type::STRING);
}
