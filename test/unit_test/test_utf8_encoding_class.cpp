//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.4
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <array>
#include <cstdint>
#include <tuple>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("UTF8Encoding_GetNumBytes")
{
    SECTION("valid bytes")
    {
        using test_value_pair_t = std::pair<uint8_t, uint32_t>;
        auto pair = GENERATE(
            test_value_pair_t(0u, 1),
            test_value_pair_t(0x7Fu, 1),
            test_value_pair_t(0xC0u, 2),
            test_value_pair_t(0xC1u, 2),
            test_value_pair_t(0xDFu, 2),
            test_value_pair_t(0xE0u, 3),
            test_value_pair_t(0xE1u, 3),
            test_value_pair_t(0xEFu, 3),
            test_value_pair_t(0xF0u, 4),
            test_value_pair_t(0xF1u, 4));

        REQUIRE(fkyaml::detail::utf8_encoding::get_num_bytes(pair.first) == pair.second);
    }

    SECTION("invalid bytes")
    {
        uint8_t byte = GENERATE(uint8_t(0x80u), uint8_t(0xF8u));
        REQUIRE_THROWS_AS(fkyaml::detail::utf8_encoding::get_num_bytes(byte), fkyaml::invalid_encoding);
    }
}

TEST_CASE("UTF8Encoding_Validate")
{
    using int_type = std::char_traits<char>::int_type;

    SECTION("1 byte character encoded in UTF-8")
    {
        using array_ret_pair_t = std::pair<std::array<int_type, 1>, bool>;
        auto pair = GENERATE(
            array_ret_pair_t({{-2}}, false),
            array_ret_pair_t({{-1}}, false),
            array_ret_pair_t({{0x00}}, true),
            array_ret_pair_t({{0x01}}, true),
            array_ret_pair_t({{0x02}}, true),
            array_ret_pair_t({{0x7D}}, true),
            array_ret_pair_t({{0x7E}}, true),
            array_ret_pair_t({{0x7F}}, true),
            array_ret_pair_t({{0x80}}, false),
            array_ret_pair_t({{0x81}}, false));

        REQUIRE(fkyaml::detail::utf8_encoding::validate(pair.first) == pair.second);
    }

    SECTION("2 byte characters encoded in UTF-8")
    {
        using array_ret_pair_t = std::pair<std::array<int_type, 2>, bool>;
        auto pair = GENERATE(
            array_ret_pair_t({{0xC0, 0x80}}, false),
            array_ret_pair_t({{0xC1, 0x80}}, false),
            array_ret_pair_t({{0xC2, 0x7E}}, false),
            array_ret_pair_t({{0xC2, 0x7F}}, false),
            array_ret_pair_t({{0xC2, 0x80}}, true),
            array_ret_pair_t({{0xC3, 0x81}}, true),
            array_ret_pair_t({{0xD0, 0xA0}}, true),
            array_ret_pair_t({{0xDE, 0xBE}}, true),
            array_ret_pair_t({{0xDF, 0xBF}}, true),
            array_ret_pair_t({{0xDF, 0xC0}}, false),
            array_ret_pair_t({{0xDF, 0xC1}}, false),
            array_ret_pair_t({{0xE0, 0xBF}}, false),
            array_ret_pair_t({{0xE1, 0xBF}}, false));

        REQUIRE(fkyaml::detail::utf8_encoding::validate(pair.first) == pair.second);
    }

    SECTION("3 byte characters encoded in UTF-8")
    {
        using array_ret_pair_t = std::pair<std::array<int_type, 3>, bool>;
        auto pair = GENERATE(
            array_ret_pair_t({{0xDE, 0x80, 0x80}}, false),
            array_ret_pair_t({{0xDF, 0x80, 0x80}}, false),
            array_ret_pair_t({{0xE0, 0x7E, 0x80}}, false),
            array_ret_pair_t({{0xE0, 0x7F, 0x80}}, false),
            array_ret_pair_t({{0xE0, 0x80, 0x7E}}, false),
            array_ret_pair_t({{0xE0, 0x80, 0x7F}}, false),

            array_ret_pair_t({{0xE0, 0x80, 0x80}}, true),
            array_ret_pair_t({{0xE6, 0xA0, 0xA0}}, true),
            array_ret_pair_t({{0xEC, 0xBF, 0xBF}}, true),

            array_ret_pair_t({{0xEC, 0xC0, 0xBF}}, false),
            array_ret_pair_t({{0xEC, 0xC1, 0xBF}}, false),
            array_ret_pair_t({{0xEC, 0xBF, 0xC0}}, false),
            array_ret_pair_t({{0xEC, 0xBF, 0xC1}}, false),

            //////////////////////////////////////////////

            array_ret_pair_t({{0xED, 0x7E, 0x80}}, false),
            array_ret_pair_t({{0xED, 0x7F, 0x80}}, false),
            array_ret_pair_t({{0xED, 0x80, 0x7E}}, false),
            array_ret_pair_t({{0xED, 0x80, 0x7F}}, false),

            array_ret_pair_t({{0xED, 0x80, 0x80}}, true),
            array_ret_pair_t({{0xED, 0x90, 0xA0}}, true),
            array_ret_pair_t({{0xED, 0x9F, 0xBF}}, true),

            array_ret_pair_t({{0xED, 0xA0, 0xBF}}, false),
            array_ret_pair_t({{0xED, 0xA1, 0xBF}}, false),
            array_ret_pair_t({{0xED, 0x9F, 0xC0}}, false),
            array_ret_pair_t({{0xED, 0x9F, 0xC1}}, false),

            //////////////////////////////////////////////

            array_ret_pair_t({{0xEE, 0x7E, 0x80}}, false),
            array_ret_pair_t({{0xEE, 0x7F, 0x80}}, false),
            array_ret_pair_t({{0xEE, 0x80, 0x7E}}, false),
            array_ret_pair_t({{0xEE, 0x80, 0x7F}}, false),

            array_ret_pair_t({{0xEE, 0x80, 0x80}}, true),
            array_ret_pair_t({{0xEE, 0xA0, 0xA0}}, true),
            array_ret_pair_t({{0xEF, 0xBF, 0xBF}}, true),

            array_ret_pair_t({{0xEF, 0xC0, 0xBF}}, false),
            array_ret_pair_t({{0xEF, 0xC1, 0xBF}}, false),
            array_ret_pair_t({{0xEF, 0xBF, 0xC0}}, false),
            array_ret_pair_t({{0xEF, 0xBF, 0xC1}}, false),
            array_ret_pair_t({{0xF0, 0xBF, 0xBF}}, false),
            array_ret_pair_t({{0xF1, 0xBF, 0xBF}}, false));

        REQUIRE(fkyaml::detail::utf8_encoding::validate(pair.first) == pair.second);
    }

    SECTION("4 byte characters encoded in UTF-8")
    {
        using array_ret_pair_t = std::pair<std::array<int_type, 4>, bool>;
        auto pair = GENERATE(
            array_ret_pair_t({{0xDE, 0x90, 0x80, 0x80}}, false),
            array_ret_pair_t({{0xDF, 0x90, 0x80, 0x80}}, false),
            array_ret_pair_t({{0xE0, 0x8E, 0x80, 0x80}}, false),
            array_ret_pair_t({{0xE0, 0x8F, 0x80, 0x80}}, false),
            array_ret_pair_t({{0xE0, 0x90, 0x7E, 0x80}}, false),
            array_ret_pair_t({{0xE0, 0x90, 0x7F, 0x80}}, false),
            array_ret_pair_t({{0xE0, 0x90, 0x80, 0x7E}}, false),
            array_ret_pair_t({{0xE0, 0x90, 0x80, 0x7F}}, false),

            array_ret_pair_t({{0xF0, 0x90, 0x80, 0x80}}, true),
            array_ret_pair_t({{0xF0, 0xA8, 0xA0, 0xA0}}, true),
            array_ret_pair_t({{0xF0, 0xBF, 0xBF, 0xBF}}, true),

            array_ret_pair_t({{0xF0, 0xC0, 0xBF, 0xBF}}, false),
            array_ret_pair_t({{0xF0, 0xC1, 0xBF, 0xBF}}, false),
            array_ret_pair_t({{0xF0, 0xBF, 0xC0, 0xBF}}, false),
            array_ret_pair_t({{0xF0, 0xBF, 0xC1, 0xBF}}, false),
            array_ret_pair_t({{0xF0, 0xBF, 0xBF, 0xC0}}, false),
            array_ret_pair_t({{0xF0, 0xBF, 0xBF, 0xC1}}, false),

            ////////////////////////////////////////////////////

            array_ret_pair_t({{0xF1, 0x7E, 0x80, 0x80}}, false),
            array_ret_pair_t({{0xF1, 0x7F, 0x80, 0x80}}, false),
            array_ret_pair_t({{0xF1, 0x80, 0x7E, 0x80}}, false),
            array_ret_pair_t({{0xF1, 0x80, 0x7F, 0x80}}, false),
            array_ret_pair_t({{0xF1, 0x80, 0x80, 0x7E}}, false),
            array_ret_pair_t({{0xF1, 0x80, 0x80, 0x7F}}, false),

            array_ret_pair_t({{0xF1, 0x80, 0x80, 0x80}}, true),
            array_ret_pair_t({{0xF2, 0xA0, 0xA0, 0xA0}}, true),
            array_ret_pair_t({{0xF3, 0xBF, 0xBF, 0xBF}}, true),

            array_ret_pair_t({{0xF3, 0xC0, 0xBF, 0xBF}}, false),
            array_ret_pair_t({{0xF3, 0xC1, 0xBF, 0xBF}}, false),
            array_ret_pair_t({{0xF3, 0xBF, 0xC0, 0xBF}}, false),
            array_ret_pair_t({{0xF3, 0xBF, 0xC1, 0xBF}}, false),
            array_ret_pair_t({{0xF3, 0xBF, 0xBF, 0xC0}}, false),
            array_ret_pair_t({{0xF3, 0xBF, 0xBF, 0xC1}}, false),

            ////////////////////////////////////////////////////

            array_ret_pair_t({{0xF4, 0x7E, 0x80, 0x80}}, false),
            array_ret_pair_t({{0xF4, 0x7F, 0x80, 0x80}}, false),
            array_ret_pair_t({{0xF4, 0x80, 0x7E, 0x80}}, false),
            array_ret_pair_t({{0xF4, 0x80, 0x7F, 0x80}}, false),
            array_ret_pair_t({{0xF4, 0x80, 0x80, 0x7E}}, false),
            array_ret_pair_t({{0xF4, 0x80, 0x80, 0x7F}}, false),

            array_ret_pair_t({{0xF4, 0x80, 0x80, 0x80}}, true),
            array_ret_pair_t({{0xF4, 0x88, 0xA0, 0x80}}, true),
            array_ret_pair_t({{0xF4, 0x8F, 0xBF, 0xBF}}, true),

            array_ret_pair_t({{0xF4, 0x90, 0xBF, 0xBF}}, false),
            array_ret_pair_t({{0xF4, 0x91, 0xBF, 0xBF}}, false),
            array_ret_pair_t({{0xF4, 0x8F, 0xC0, 0xBF}}, false),
            array_ret_pair_t({{0xF4, 0x8F, 0xC1, 0xBF}}, false),
            array_ret_pair_t({{0xF4, 0x8F, 0xBF, 0xC0}}, false),
            array_ret_pair_t({{0xF4, 0x8F, 0xBF, 0xC1}}, false),
            array_ret_pair_t({{0xF5, 0x8F, 0xBF, 0xBF}}, false),
            array_ret_pair_t({{0xF6, 0x8F, 0xBF, 0xBF}}, false));

        REQUIRE(fkyaml::detail::utf8_encoding::validate(pair.first) == pair.second);
    }
}

TEST_CASE("UTF8Encoding_FromUTF16")
{
    SECTION("valid UTF-16 character(s)")
    {
        struct test_params
        {
            std::array<char16_t, 2> utf16;
            std::array<char, 4> utf8_bytes;
            std::size_t consumed_size;
            std::size_t encoded_size;
        };
        auto params = GENERATE(
            test_params {{{char16_t(0x00u)}}, {{char(0x00u)}}, 1, 1},
            test_params {{{char16_t(0x01u)}}, {{char(0x01u)}}, 1, 1},
            test_params {{{char16_t(0x7Eu)}}, {{char(0x7Eu)}}, 1, 1},
            test_params {{{char16_t(0x7Fu)}}, {{char(0x7Fu)}}, 1, 1},
            test_params {{{char16_t(0x0080u)}}, {{char(0xC2u), char(0x80u)}}, 1, 2},
            test_params {{{char16_t(0x0081u)}}, {{char(0xC2u), char(0x81u)}}, 1, 2},
            test_params {{{char16_t(0x07FEu)}}, {{char(0xDFu), char(0xBEu)}}, 1, 2},
            test_params {{{char16_t(0x07FFu)}}, {{char(0xDFu), char(0xBFu)}}, 1, 2},
            test_params {{{char16_t(0x0800u)}}, {{char(0xE0u), char(0xA0u), char(0x80u)}}, 1, 3},
            test_params {{{char16_t(0x0801u)}}, {{char(0xE0u), char(0xA0u), char(0x81u)}}, 1, 3},
            test_params {{{char16_t(0xD7FEu)}}, {{char(0xEDu), char(0x9Fu), char(0xBEu)}}, 1, 3},
            test_params {{{char16_t(0xD7FFu)}}, {{char(0xEDu), char(0x9Fu), char(0xBFu)}}, 1, 3},
            test_params {{{char16_t(0xE000u)}}, {{char(0xEEu), char(0x80u), char(0x80u)}}, 1, 3},
            test_params {{{char16_t(0xE001u)}}, {{char(0xEEu), char(0x80u), char(0x81u)}}, 1, 3},
            test_params {
                {{char16_t(0xD800u), char16_t(0xDC00u)}}, {{char(0xF0u), char(0x90u), char(0x80u), char(0x80u)}}, 2, 4},
            test_params {
                {{char16_t(0xD801u), char16_t(0xDC00u)}}, {{char(0xF0u), char(0x90u), char(0x90u), char(0x80u)}}, 2, 4},
            test_params {
                {{char16_t(0xD800u), char16_t(0xDC01u)}}, {{char(0xF0u), char(0x90u), char(0x80u), char(0x81u)}}, 2, 4},
            test_params {
                {{char16_t(0xDBFEu), char16_t(0xDFFFu)}}, {{char(0xF4u), char(0x8Fu), char(0xAFu), char(0xBFu)}}, 2, 4},
            test_params {
                {{char16_t(0xDBFFu), char16_t(0xDFFEu)}}, {{char(0xF4u), char(0x8Fu), char(0xBFu), char(0xBEu)}}, 2, 4},
            test_params {
                {{char16_t(0xDBFFu), char16_t(0xDFFFu)}},
                {{char(0xF4u), char(0x8Fu), char(0xBFu), char(0xBFu)}},
                2,
                4});

        std::array<char, 4> utf8_bytes;
        std::size_t consumed_size;
        std::size_t encoded_size;

        fkyaml::detail::utf8_encoding::from_utf16(params.utf16, utf8_bytes, consumed_size, encoded_size);

        REQUIRE(utf8_bytes == params.utf8_bytes);
        REQUIRE(consumed_size == params.consumed_size);
        REQUIRE(encoded_size == params.encoded_size);
    }

    SECTION("invalid UTF-16 character(s)")
    {
        auto utf16 = GENERATE(
            std::array<char16_t, 2> {{char16_t(0xDC00u), char16_t(0xDC00u)}},
            std::array<char16_t, 2> {{char16_t(0xDBFFu), char16_t(0xDBFFu)}},
            std::array<char16_t, 2> {{char16_t(0xDBFFu), char16_t(0xE000u)}});

        std::array<char, 4> utf8_bytes;
        std::size_t consumed_size;
        std::size_t encoded_size;

        REQUIRE_THROWS_AS(
            fkyaml::detail::utf8_encoding::from_utf16(utf16, utf8_bytes, consumed_size, encoded_size),
            fkyaml::invalid_encoding);
    }
}

TEST_CASE("UTF8Encoding_FromUTF32")
{
    SECTION("valid UTF-32 character")
    {
        struct test_params
        {
            char32_t utf32;
            std::array<char, 4> utf8_bytes;
            std::size_t size;
        };
        auto params = GENERATE(
            test_params {0x00u, {{char(0x00u)}}, 1},
            test_params {0x01u, {{char(0x01u)}}, 1},
            test_params {0x7Eu, {{char(0x7Eu)}}, 1},
            test_params {0x7Fu, {{char(0x7Fu)}}, 1},
            test_params {0x0080u, {{char(0xC2u), char(0x80u)}}, 2},
            test_params {0x0081u, {{char(0xC2u), char(0x81u)}}, 2},
            test_params {0x07FEu, {{char(0xDFu), char(0xBEu)}}, 2},
            test_params {0x07FFu, {{char(0xDFu), char(0xBFu)}}, 2},
            test_params {0x0800u, {{char(0xE0u), char(0xA0u), char(0x80u)}}, 3},
            test_params {0x0801u, {{char(0xE0u), char(0xA0u), char(0x81u)}}, 3},
            test_params {0xFFFFu, {{char(0xEFu), char(0xBFu), char(0xBFu)}}, 3},
            test_params {0x010000u, {{char(0xF0u), char(0x90u), char(0x80u), char(0x80u)}}, 4},
            test_params {0x010001u, {{char(0xF0u), char(0x90u), char(0x80u), char(0x81u)}}, 4},
            test_params {0x10FFFEu, {{char(0xF4u), char(0x8Fu), char(0xBFu), char(0xBEu)}}, 4},
            test_params {0x10FFFFu, {{char(0xF4u), char(0x8Fu), char(0xBFu), char(0xBFu)}}, 4});

        std::array<char, 4> utf8_bytes;
        std::size_t size;
        fkyaml::detail::utf8_encoding::from_utf32(params.utf32, utf8_bytes, size);

        REQUIRE(utf8_bytes == params.utf8_bytes);
        REQUIRE(size == params.size);
    }

    SECTION("invalid UTF-32 character")
    {
        char32_t utf32 = 0x110000u;
        std::array<char, 4> utf8_bytes;
        std::size_t encoded_size;

        REQUIRE_THROWS_AS(
            fkyaml::detail::utf8_encoding::from_utf32(utf32, utf8_bytes, encoded_size), fkyaml::invalid_encoding);
    }
}
