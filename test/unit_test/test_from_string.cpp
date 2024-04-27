//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.5
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <cfloat>
#include <string>
#include <utility>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("FromString_Null") {
    SECTION("valid string for the null value") {
        auto input = GENERATE(std::string("null"), std::string("Null"), std::string("NULL"), std::string("~"));
        REQUIRE(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<std::nullptr_t> {}) == nullptr);
    }

    SECTION("invalid string for the null value") {
        std::string input("test");
        REQUIRE_THROWS_AS(
            fkyaml::detail::from_string(input, fkyaml::detail::type_tag<std::nullptr_t> {}), fkyaml::exception);
    }
}

TEST_CASE("FromString_Bool") {
    SECTION("valid string for the true value") {
        auto input = GENERATE(std::string("true"), std::string("True"), std::string("TRUE"));
        REQUIRE(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<bool> {}) == true);
    }

    SECTION("valid string for the false value") {
        auto input = GENERATE(std::string("false"), std::string("False"), std::string("FALSE"));
        REQUIRE(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<bool> {}) == false);
    }

    SECTION("invalid string for the boolean values") {
        std::string input("test");
        REQUIRE_THROWS_AS(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<bool> {}), fkyaml::exception);
    }
}

TEST_CASE("FromString_Integer") {
    SECTION("char type") {
        std::string input("-64");
        REQUIRE(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<char> {}) == -64);

        input = "256";
        REQUIRE_THROWS_AS(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<char> {}), fkyaml::exception);
    }

    SECTION("unsigned char type") {
        std::string input("64");
        REQUIRE(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<unsigned char> {}) == 64);

        input = "512";
        REQUIRE_THROWS_AS(
            fkyaml::detail::from_string(input, fkyaml::detail::type_tag<unsigned char> {}), fkyaml::exception);
    }

    SECTION("short type") {
        std::string input("-15464");
        REQUIRE(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<short> {}) == -15464);

        input = "45464";
        REQUIRE_THROWS_AS(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<short> {}), fkyaml::exception);
    }

    SECTION("unsigned short type") {
        std::string input("15464");
        REQUIRE(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<unsigned short> {}) == 15464);

        input = "-1";
        REQUIRE_THROWS_AS(
            fkyaml::detail::from_string(input, fkyaml::detail::type_tag<unsigned short> {}), fkyaml::exception);
    }

    SECTION("int type") {
        std::string input("-1154357464");
        REQUIRE(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<int> {}) == -1154357464);

        input = "3154357464";
        REQUIRE_THROWS_AS(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<int> {}), fkyaml::exception);
    }

    SECTION("unsigned int type") {
        std::string input("3154357464");
        REQUIRE(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<unsigned int> {}) == 3154357464u);

        input = "999999999999999999999999";
        REQUIRE_THROWS_AS(
            fkyaml::detail::from_string(input, fkyaml::detail::type_tag<unsigned int> {}), fkyaml::exception);
    }

    SECTION("long type") {
        std::string input("-1154357464");
        REQUIRE(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<long> {}) == -1154357464l);

        input = "9413456789012123456";
        REQUIRE_THROWS_AS(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<long> {}), fkyaml::exception);
    }

    SECTION("unsigned long type") {
        std::string input("317464");
        REQUIRE(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<unsigned long> {}) == 317464ul);

        input = "999999999999999999999999";
        REQUIRE_THROWS_AS(
            fkyaml::detail::from_string(input, fkyaml::detail::type_tag<unsigned long> {}), fkyaml::exception);
    }

    SECTION("long long type") {
        std::string input("-1154357464");
        REQUIRE(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<long long> {}) == -1154357464ll);

        input = "18413456789012123456";
        REQUIRE_THROWS_AS(
            fkyaml::detail::from_string(input, fkyaml::detail::type_tag<long long> {}), fkyaml::exception);
    }

    SECTION("unsigned long long type") {
        std::string input("3154357464");
        REQUIRE(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<unsigned long long> {}) == 3154357464ull);

        input = "999999999999999999999999";
        REQUIRE_THROWS_AS(
            fkyaml::detail::from_string(input, fkyaml::detail::type_tag<unsigned long long> {}), fkyaml::exception);
    }
}

TEST_CASE("FromString_Float") {
    SECTION("positive infinity") {
        auto input = GENERATE(std::string(".inf"), std::string(".Inf"), std::string(".INF"));
        REQUIRE(
            fkyaml::detail::from_string(input, fkyaml::detail::type_tag<float> {}) ==
            std::numeric_limits<float>::infinity());
    }

    SECTION("negative infinity") {
        auto input = GENERATE(std::string("-.inf"), std::string("-.Inf"), std::string("-.INF"));
        REQUIRE(
            fkyaml::detail::from_string(input, fkyaml::detail::type_tag<float> {}) ==
            -1 * std::numeric_limits<float>::infinity());
    }

    SECTION("NaN") {
        auto input = GENERATE(std::string(".nan"), std::string(".NaN"), std::string(".NAN"));
        float ret = 0.0f;
        REQUIRE_NOTHROW(ret = fkyaml::detail::from_string(input, fkyaml::detail::type_tag<float> {}));
        REQUIRE(std::isnan(ret));
    }

    SECTION("valid string for a float value") {
        std::string input("3.14");
        REQUIRE(std::abs(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<float> {}) - 3.14f) < FLT_EPSILON);

        input = "3.40282347e+39";
        REQUIRE_THROWS_AS(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<float> {}), fkyaml::exception);
    }
}

TEST_CASE("FromString_Double") {
    SECTION("positive infinity") {
        auto input = GENERATE(std::string(".inf"), std::string(".Inf"), std::string(".INF"));
        REQUIRE(
            fkyaml::detail::from_string(input, fkyaml::detail::type_tag<double> {}) ==
            std::numeric_limits<double>::infinity());
    }

    SECTION("negative infinity") {
        auto input = GENERATE(std::string("-.inf"), std::string("-.Inf"), std::string("-.INF"));
        REQUIRE(
            fkyaml::detail::from_string(input, fkyaml::detail::type_tag<double> {}) ==
            -1 * std::numeric_limits<double>::infinity());
    }

    SECTION("NaN") {
        auto input = GENERATE(std::string(".nan"), std::string(".NaN"), std::string(".NAN"));
        double ret = 0.0;
        REQUIRE_NOTHROW(ret = fkyaml::detail::from_string(input, fkyaml::detail::type_tag<double> {}));
        REQUIRE(std::isnan(ret));
    }

    SECTION("valid string for a double value") {
        std::string input("3.14");
        REQUIRE(std::abs(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<double> {}) - 3.14) < DBL_EPSILON);

        input = "1.7976931348623157E+309";
        REQUIRE_THROWS_AS(fkyaml::detail::from_string(input, fkyaml::detail::type_tag<double> {}), fkyaml::exception);
    }
}
