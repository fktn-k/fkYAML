//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <cfloat>
#include <string>
#include <utility>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("ScalarConv_aton") {
    std::nullptr_t null = nullptr;

    SECTION("valid string for the null value") {
        auto input = GENERATE(std::string("null"), std::string("Null"), std::string("NULL"), std::string("~"));
        REQUIRE(fkyaml::detail::aton(input.begin(), input.end(), null) == true);
    }

    SECTION("invalid string for the null value") {
        auto input = GENERATE(std::string("test"), std::string(""));
        REQUIRE(fkyaml::detail::aton(input.begin(), input.end(), null) == false);
    }
}

TEST_CASE("ScalarConv_atob") {
    bool boolean = false;

    SECTION("valid string for the true value") {
        auto input = GENERATE(std::string("true"), std::string("True"), std::string("TRUE"));
        REQUIRE(fkyaml::detail::atob(input.begin(), input.end(), boolean) == true);
        REQUIRE(boolean == true);
    }

    SECTION("valid string for the false value") {
        auto input = GENERATE(std::string("false"), std::string("False"), std::string("FALSE"));
        REQUIRE(fkyaml::detail::atob(input.begin(), input.end(), boolean) == true);
        REQUIRE(boolean == false);
    }

    SECTION("invalid string for the boolean values") {
        auto input = GENERATE(std::string("test"), std::string("test2"), std::string(""));
        REQUIRE(fkyaml::detail::atob(input.begin(), input.end(), boolean) == false);
    }
}

TEST_CASE("ScalarConv_atoi") {
    // implement common (not integer type specific) test cases

    int32_t integer = 0;

    SECTION("empty input") {
        std::string input = "";
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == false);
    }

    SECTION("decimal number with an explicit plus sign") {
        std::string input = "+64";
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == int32_t(64));
    }

    SECTION("hexadecimal number with different writings in alphabets") {
        auto input = GENERATE(std::string("0xFA"), std::string("0xfa"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 0xFA);
    }

    SECTION("max digits but within bounds") {
        using test_data_t = std::pair<std::string, int32_t>;
        auto test_data = GENERATE(
            test_data_t(std::string("1147483647"), 1147483647), test_data_t(std::string("-1147483648"), -1147483648));

        REQUIRE(fkyaml::detail::atoi(test_data.first.begin(), test_data.first.end(), integer) == true);
        REQUIRE(integer == test_data.second);
    }

    SECTION("invalid values") {
        auto input = GENERATE(
            std::string("0123"),
            std::string("+"),
            std::string("21474836470"),
            std::string("-"),
            std::string("-21474836480"),
            std::string("1/"),
            std::string("1:"),
            std::string("0o"),
            std::string("0o/"),
            std::string("0o8"),
            std::string("0x"),
            std::string("0x/"),
            std::string("0x:"),
            std::string("0x@"),
            std::string("0xG"),
            std::string("0x`"),
            std::string("0xg"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == false);
    }
}

TEST_CASE("ScalarConv_atoi_int8_t") {
    int8_t integer = 0;

    SECTION("positive values") {
        auto input = GENERATE(std::string("64"), std::string("0o100"), std::string("0x40"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 64);
    }

    SECTION("negative value") {
        std::string input = "-64";
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == -64);
    }

    SECTION("max values") {
        auto input = GENERATE(std::string("127"), std::string("0o177"), std::string("0x7F"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 127);
    }

    SECTION("minimum values") {
        auto input = GENERATE(std::string("-128"), std::string("0o200"), std::string("0x80"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == -128);
    }

    SECTION("invalid values") {
        auto input = GENERATE(std::string("128"), std::string("-129"), std::string("0o400"), std::string("0x100"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == false);
    }
}

TEST_CASE("ScalarConv_atoi_uint8_t") {
    uint8_t integer = 0;

    SECTION("positive values") {
        auto input = GENERATE(std::string("64"), std::string("0o100"), std::string("0x40"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 64u);
    }

    SECTION("max values") {
        auto input = GENERATE(std::string("255"), std::string("0o377"), std::string("0xFF"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 255u);
    }

    SECTION("minimum values") {
        auto input = GENERATE(std::string("0"), std::string("0o0"), std::string("0x0"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 0u);
    }

    SECTION("invalid values") {
        auto input = GENERATE(std::string("256"), std::string("-1"), std::string("0o400"), std::string("0x100"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == false);
    }
}

TEST_CASE("ScalarConv_atoi_int16_t") {
    int16_t integer = 0;

    SECTION("positive values") {
        auto input = GENERATE(std::string("64"), std::string("0o100"), std::string("0x40"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 64);
    }

    SECTION("negative value") {
        std::string input = "-64";
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == -64);
    }

    SECTION("max values") {
        auto input = GENERATE(std::string("32767"), std::string("0o77777"), std::string("0x7FFF"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 32767);
    }

    SECTION("minimum values") {
        auto input = GENERATE(std::string("-32768"), std::string("0o100000"), std::string("0x8000"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == -32768);
    }

    SECTION("invalid values") {
        auto input =
            GENERATE(std::string("32768"), std::string("-32769"), std::string("0o200000"), std::string("0x10000"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == false);
    }
}

TEST_CASE("ScalarConv_atoi_uint16_t") {
    uint16_t integer = 0;

    SECTION("positive values") {
        auto input = GENERATE(std::string("64"), std::string("0o100"), std::string("0x40"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 64u);
    }

    SECTION("max values") {
        auto input = GENERATE(std::string("65535"), std::string("0o177777"), std::string("0xFFFF"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 65535u);
    }

    SECTION("minimum values") {
        auto input = GENERATE(std::string("0"), std::string("0o0"), std::string("0x0"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 0u);
    }

    SECTION("invalid values") {
        auto input = GENERATE(std::string("65536"), std::string("-1"), std::string("0o200000"), std::string("0x10000"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == false);
    }
}

TEST_CASE("ScalarConv_atoi_int32_t") {
    int32_t integer = 0;

    SECTION("positive values") {
        auto input = GENERATE(std::string("64"), std::string("0o100"), std::string("0x40"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 64);
    }

    SECTION("negative value") {
        std::string input = "-64";
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == -64);
    }

    SECTION("max values") {
        auto input = GENERATE(std::string("2147483647"), std::string("0o17777777777"), std::string("0x7FFFFFFF"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 2147483647);
    }

    SECTION("minimum values") {
        auto input = GENERATE(std::string("-2147483648"), std::string("0o20000000000"), std::string("0x80000000"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == -2147483648);
    }

    SECTION("invalid values") {
        auto input = GENERATE(
            std::string("2147483648"),
            std::string("-2147483649"),
            std::string("0o40000000000"),
            std::string("0x100000000"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == false);
    }
}

TEST_CASE("ScalarConv_atoi_uint32_t") {
    uint32_t integer = 0;

    SECTION("positive values") {
        auto input = GENERATE(std::string("64"), std::string("0o100"), std::string("0x40"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 64u);
    }

    SECTION("max values") {
        auto input = GENERATE(std::string("4294967295"), std::string("0o37777777777"), std::string("0xFFFFFFFF"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 4294967295u);
    }

    SECTION("minimum values") {
        auto input = GENERATE(std::string("0"), std::string("0o0"), std::string("0x0"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 0u);
    }

    SECTION("invalid values") {
        auto input = GENERATE(
            std::string("4294967296"), std::string("-1"), std::string("0o40000000000"), std::string("0x100000000"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == false);
    }
}

TEST_CASE("ScalarConv_atoi_int64_t") {
    int64_t integer = 0;

    SECTION("positive values") {
        auto input = GENERATE(std::string("64"), std::string("0o100"), std::string("0x40"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 64);
    }

    SECTION("negative value") {
        std::string input = "-64";
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == -64);
    }

    SECTION("max values") {
        auto input = GENERATE(
            std::string("9223372036854775807"),
            std::string("0o777777777777777777777"),
            std::string("0x7FFFFFFFFFFFFFFF"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 9223372036854775807);
    }

    SECTION("minimum values") {
        auto input = GENERATE(
            std::string("-9223372036854775808"),
            std::string("0o1000000000000000000000"),
            std::string("0x8000000000000000"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == std::numeric_limits<int64_t>::min());
    }

    SECTION("invalid values") {
        auto input = GENERATE(
            std::string("9223372036854775808"),
            std::string("-9223372036854775809"),
            std::string("0o2000000000000000000000"),
            std::string("0x10000000000000000"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == false);
    }
}

TEST_CASE("ScalarConv_atoi_uint64_t") {
    uint64_t integer = 0;

    SECTION("positive values") {
        auto input = GENERATE(std::string("64"), std::string("0o100"), std::string("0x40"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 64u);
    }

    SECTION("max values") {
        auto input = GENERATE(
            std::string("18446744073709551615"),
            std::string("0o1777777777777777777777"),
            std::string("0xFFFFFFFFFFFFFFFF"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 18446744073709551615u);
    }

    SECTION("minimum values") {
        auto input = GENERATE(std::string("0"), std::string("0o0"), std::string("0x0"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == true);
        REQUIRE(integer == 0u);
    }

    SECTION("invalid values") {
        auto input = GENERATE(
            std::string("18446744073709551616"),
            std::string("-1"),
            std::string("0o2000000000000000000000"),
            std::string("0x10000000000000000"));
        REQUIRE(fkyaml::detail::atoi(input.begin(), input.end(), integer) == false);
    }
}

TEST_CASE("ScalarConv_atof_float") {
    float fp = 0.f;
    using limits_type = std::numeric_limits<float>;

    SECTION("positive infinity") {
        auto input = GENERATE(
            std::string(".inf"),
            std::string(".Inf"),
            std::string(".INF"),
            std::string("+.inf"),
            std::string("+.Inf"),
            std::string("+.INF"));
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == true);
        REQUIRE(std::isinf(fp));
    }

    SECTION("negative infinity") {
        auto input = GENERATE(std::string("-.inf"), std::string("-.Inf"), std::string("-.INF"));
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == true);
        REQUIRE(std::isinf(fp));
    }

    SECTION("NaN") {
        auto input = GENERATE(std::string(".nan"), std::string(".NaN"), std::string(".NAN"));
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == true);
        REQUIRE(std::isnan(fp));
    }

    SECTION("values") {
        std::string input("3.14");
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == true);
        REQUIRE(std::abs(fp - 3.14f) < limits_type::epsilon());

        input = "-3.14";
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == true);
        REQUIRE(std::abs(fp + 3.14f) < limits_type::epsilon());

        input = "-0.5";
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == true);
        REQUIRE(std::abs(fp + 0.5f) < limits_type::epsilon());

        input = "";
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == false);

        input = "3.40282347e+39";
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == false);
    }
}

TEST_CASE("ScalarConv_atof_double") {
    double fp = 0.;
    using limits_type = std::numeric_limits<double>;

    SECTION("positive infinity") {
        auto input = GENERATE(
            std::string(".inf"),
            std::string(".Inf"),
            std::string(".INF"),
            std::string("+.inf"),
            std::string("+.Inf"),
            std::string("+.INF"));
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == true);
        REQUIRE(std::isinf(fp));
    }

    SECTION("negative infinity") {
        auto input = GENERATE(std::string("-.inf"), std::string("-.Inf"), std::string("-.INF"));
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == true);
        REQUIRE(std::isinf(fp));
    }

    SECTION("NaN") {
        auto input = GENERATE(std::string(".nan"), std::string(".NaN"), std::string(".NAN"));
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == true);
        REQUIRE(std::isnan(fp));
    }

    SECTION("values") {
        std::string input("3.14");
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == true);
        REQUIRE(std::abs(fp - 3.14) < limits_type::epsilon());

        input = "-3.14";
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == true);
        REQUIRE(std::abs(fp + 3.14) < limits_type::epsilon());

        input = "-0.5";
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == true);
        REQUIRE(std::abs(fp + 0.5) < limits_type::epsilon());

        input = "";
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == false);

        input = "1.7976931348623157E+309";
        REQUIRE(fkyaml::detail::atof(input.begin(), input.end(), fp) == false);
    }
}
