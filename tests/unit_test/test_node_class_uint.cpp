//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <cstdint>
#include <limits>
#include <string>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("Node_IsUint_ReturnsFalseForNonIntegerTypes") {
    SECTION("null") {
        fkyaml::node n;
        REQUIRE(n.is_uint() == false);
    }

    SECTION("boolean") {
        REQUIRE(fkyaml::node::deserialize("v: true")["v"].is_uint() == false);
    }

    SECTION("float") {
        REQUIRE(fkyaml::node::deserialize("v: 3.14")["v"].is_uint() == false);
    }

    SECTION("string") {
        REQUIRE(fkyaml::node::deserialize("v: hello")["v"].is_uint() == false);
    }
}

TEST_CASE("Node_IsUint_ReturnsFalseWithinSignedRange") {
    auto input = GENERATE(
        std::string("-9223372036854775808"), // INT64_MIN
        std::string("-42"),
        std::string("-1"));
    REQUIRE(fkyaml::node::deserialize("v: " + input)["v"].is_uint() == false);
}

TEST_CASE("Node_IsUint_ReturnsTrueAboveSignedRange") {
    auto input = GENERATE(
        std::string("0"),
        std::string("1"),
        std::string("9223372036854775807"),   // INT64_MAX
        std::string("9223372036854775808"),   // INT64_MAX + 1
        std::string("15745692345339290292"),  // xxHash value from the bug report
        std::string("18446744073709551615")); // UINT64_MAX
    REQUIRE(fkyaml::node::deserialize("v: " + input)["v"].is_uint() == true);
}

TEST_CASE("Node_AsUint_ReturnsCorrectValueForUInt64RangeNodes") {
    using test_data_t = std::pair<std::string, uint64_t>;
    auto test_data = GENERATE(
        test_data_t {"9223372036854775808", UINT64_C(9223372036854775808)},
        test_data_t {"15745692345339290292", UINT64_C(15745692345339290292)},
        test_data_t {"18446744073709551615", UINT64_C(18446744073709551615)});
    REQUIRE(fkyaml::node::deserialize("v: " + test_data.first)["v"].as_uint() == test_data.second);
}

TEST_CASE("Node_AsUint_SucceedsForNonNegativeSignedIntegers") {
    using test_data_t = std::pair<std::string, uint64_t>;
    auto test_data = GENERATE(
        test_data_t {"0", 0ULL},
        test_data_t {"100", 100ULL},
        test_data_t {"9223372036854775807", static_cast<uint64_t>(INT64_MAX)});
    REQUIRE(fkyaml::node::deserialize("v: " + test_data.first)["v"].as_uint() == test_data.second);
}

TEST_CASE("Node_AsUint_ThrowsForNegativeIntegers") {
    auto input = GENERATE(std::string("-1"), std::string("-42"));
    REQUIRE_THROWS_AS(fkyaml::node::deserialize("v: " + input)["v"].as_uint(), fkyaml::type_error);
}

TEST_CASE("Node_AsUint_ThrowsForNonIntegerNodes") {
    SECTION("null") {
        REQUIRE_THROWS_AS(fkyaml::node().as_uint(), fkyaml::type_error);
    }

    SECTION("boolean") {
        REQUIRE_THROWS_AS(fkyaml::node::deserialize("v: true")["v"].as_uint(), fkyaml::type_error);
    }

    SECTION("float") {
        REQUIRE_THROWS_AS(fkyaml::node::deserialize("v: 3.14")["v"].as_uint(), fkyaml::type_error);
    }

    SECTION("string") {
        REQUIRE_THROWS_AS(fkyaml::node::deserialize("v: hello")["v"].as_uint(), fkyaml::type_error);
    }
}

TEST_CASE("Node_GetValueUInt64_RoundTrips") {
    using test_data_t = std::pair<std::string, uint64_t>;
    auto test_data = GENERATE(
        test_data_t {"0", 0ULL},
        test_data_t {"15745692345339290292", UINT64_C(15745692345339290292)},
        test_data_t {"18446744073709551615", UINT64_MAX});
    REQUIRE(fkyaml::node::deserialize("v: " + test_data.first)["v"].get_value<uint64_t>() == test_data.second);
}

TEST_CASE("Node_GetValueUInt64_ThrowsForNegativeValue") {
    REQUIRE_THROWS_AS(fkyaml::node::deserialize("v: -5")["v"].get_value<uint64_t>(), fkyaml::exception);
}

TEST_CASE("Node_AsInt_UnaffectedByUint64Change") {
    // Regression: existing signed-integer behaviour must be preserved.
    using test_data_t = std::pair<std::string, int64_t>;
    auto test_data = GENERATE(
        test_data_t {"-9223372036854775808", INT64_MIN},
        test_data_t {"-42", -42},
        test_data_t {"0", 0},
        test_data_t {"1234", 1234},
        test_data_t {"9223372036854775807", INT64_MAX});
    REQUIRE(fkyaml::node::deserialize("v: " + test_data.first)["v"].as_int() == test_data.second);
}

TEST_CASE("Node_AsInt_ThrowsForUintFlaggedNode") {
    // as_int() must throw type_error when uint_bit is set, because the stored
    // bit-pattern is not representable as a signed int64_t.
    auto input = GENERATE(
        std::string("9223372036854775808"),   // INT64_MAX + 1
        std::string("15745692345339290292"),  // xxHash value from the bug report
        std::string("18446744073709551615")); // UINT64_MAX
    auto node = fkyaml::node::deserialize("v: " + input);
    auto& int_node = node["v"];
    REQUIRE_THROWS_AS(int_node.as_int(), fkyaml::type_error);
    const auto& const_int_node = int_node;
    REQUIRE_THROWS_AS(const_int_node.as_int(), fkyaml::type_error);
}

TEST_CASE("Node_UintBit_ClearedOnReassignment") {
    // After assigning a new signed integer value the uint_bit must be cleared.
    fkyaml::node n = fkyaml::node::deserialize("v: 15745692345339290292")["v"];
    REQUIRE(n.is_uint() == true);

    n = fkyaml::node(static_cast<fkyaml::node::integer_type>(-42));
    REQUIRE(n.is_integer() == true);
    REQUIRE(n.is_uint() == false);
    REQUIRE(n.as_int() == -42);
}
