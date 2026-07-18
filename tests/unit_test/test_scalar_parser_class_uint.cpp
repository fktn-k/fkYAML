//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <string>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("ScalarParser_UInt64_LargeDecimalParsedAsInteger") {
    // Values above INT64_MAX must be stored as INTEGER nodes, not demoted to STRING.
    auto input = GENERATE(
        std::string("9223372036854775808"),   // INT64_MAX + 1
        std::string("15745692345339290292"),  // xxHash value from the bug report
        std::string("18446744073709551615")); // UINT64_MAX
    auto node = fkyaml::node::deserialize("v: " + input)["v"];
    REQUIRE(node.is_integer() == true);
    REQUIRE(node.is_string() == false);
}

TEST_CASE("ScalarParser_UInt64_LargeDecimalSetsUintBit") {
    // The uint_bit flag must be set for any value that overflows int64_t.
    auto input = GENERATE(
        std::string("9223372036854775808"), std::string("15745692345339290292"), std::string("18446744073709551615"));
    auto node = fkyaml::node::deserialize("v: " + input)["v"];
    REQUIRE(node.is_uint() == true);
}

TEST_CASE("ScalarParser_UInt64_AsUintRecoversPreciseValue") {
    using test_data_t = std::pair<std::string, uint64_t>;
    auto test_data = GENERATE(
        test_data_t {"9223372036854775808", UINT64_C(9223372036854775808)},
        test_data_t {"15745692345339290292", UINT64_C(15745692345339290292)},
        test_data_t {"18446744073709551615", UINT64_C(18446744073709551615)});
    auto node = fkyaml::node::deserialize("v: " + test_data.first)["v"];
    REQUIRE(node.as_uint() == test_data.second);
    REQUIRE(node.get_value<uint64_t>() == test_data.second);
}

TEST_CASE("ScalarParser_UInt64_SignedRangeNoUintBit") {
    // Values within the negative integer range must NOT return true for is_uint().
    auto input = GENERATE(
        std::string("-9223372036854775808"), // INT64_MIN
        std::string("-42"),
        std::string("-1"));
    auto node = fkyaml::node::deserialize("v: " + input)["v"];
    REQUIRE(node.is_integer() == true);
    REQUIRE(node.is_uint() == false);
}

TEST_CASE("ScalarParser_UInt64_ExplicitIntTagOnOverflowThrows") {
    // An explicit !!int tag must not silently demote to string on overflow.
    REQUIRE_THROWS_AS(fkyaml::node::deserialize("v: !!int 99999999999999999999999"), fkyaml::parse_error);
}

TEST_CASE("ScalarParser_UInt64_BeyondUint64MaxBecomesString") {
    // Without a tag, a decimal that exceeds UINT64_MAX falls back to string.
    auto node = fkyaml::node::deserialize("v: 99999999999999999999999")["v"];
    REQUIRE(node.is_string() == true);
}
