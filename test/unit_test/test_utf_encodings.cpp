//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.5
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <array>
#include <cstdint>
#include <tuple>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("UTF8_GetNumBytes") {
    SECTION("valid bytes") {
        using test_value_pair_t = std::pair<uint8_t, uint32_t>;
        auto pair = GENERATE(
            test_value_pair_t(uint8_t(0u), 1u),
            test_value_pair_t(uint8_t(0x7Fu), 1u),
            test_value_pair_t(uint8_t(0xC0u), 2u),
            test_value_pair_t(uint8_t(0xC1u), 2u),
            test_value_pair_t(uint8_t(0xDFu), 2u),
            test_value_pair_t(uint8_t(0xE0u), 3u),
            test_value_pair_t(uint8_t(0xE1u), 3u),
            test_value_pair_t(uint8_t(0xEFu), 3u),
            test_value_pair_t(uint8_t(0xF0u), 4u),
            test_value_pair_t(uint8_t(0xF1u), 4u));

        REQUIRE(fkyaml::detail::utf8::get_num_bytes(pair.first) == pair.second);
    }

    SECTION("invalid bytes") {
        uint8_t byte = GENERATE(uint8_t(0x80u), uint8_t(0xF8u));
        REQUIRE_THROWS_AS(fkyaml::detail::utf8::get_num_bytes(byte), fkyaml::invalid_encoding);
    }
}

TEST_CASE("UTF8_Validate") {
    SECTION("1 byte character encoded in UTF-8") {
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0x00u)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0x01u)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0x02u)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0x7Du)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0x7Eu)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0x7Fu)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0x80u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0x81u)}) == false);
    }

    SECTION("2 byte characters encoded in UTF-8") {
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xC0u), uint8_t(0x80u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xC1u), uint8_t(0x80u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xC2u), uint8_t(0x7Eu)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xC2u), uint8_t(0x7Fu)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xC2u), uint8_t(0x80u)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xC3u), uint8_t(0x81u)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xD0u), uint8_t(0xA0u)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xDEu), uint8_t(0xBEu)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xDFu), uint8_t(0xBFu)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xDFu), uint8_t(0xC0u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xDFu), uint8_t(0xC1u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xE0u), uint8_t(0xBFu)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xE1u), uint8_t(0xBFu)}) == false);
    }

    SECTION("3 byte characters encoded in UTF-8") {
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xDEu), uint8_t(0x80u), uint8_t(0x80u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xDFu), uint8_t(0x80u), uint8_t(0x80u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xE0u), uint8_t(0x7Eu), uint8_t(0x80u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xE0u), uint8_t(0x7Fu), uint8_t(0x80u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xE0u), uint8_t(0x80u), uint8_t(0x7Eu)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xE0u), uint8_t(0x80u), uint8_t(0x7Fu)}) == false);

        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xE0u), uint8_t(0x80u), uint8_t(0x80u)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xE6u), uint8_t(0xA0u), uint8_t(0xA0u)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xECu), uint8_t(0xBFu), uint8_t(0xBFu)}) == true);

        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xECu), uint8_t(0xC0u), uint8_t(0xBFu)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xECu), uint8_t(0xC1u), uint8_t(0xBFu)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xECu), uint8_t(0xBFu), uint8_t(0xC0u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xECu), uint8_t(0xBFu), uint8_t(0xC1u)}) == false);

        //////////////////////////////////////////////

        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEDu), uint8_t(0x7Eu), uint8_t(0x80u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEDu), uint8_t(0x7Fu), uint8_t(0x80u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEDu), uint8_t(0x80u), uint8_t(0x7Eu)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEDu), uint8_t(0x80u), uint8_t(0x7Fu)}) == false);

        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEDu), uint8_t(0x80u), uint8_t(0x80u)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEDu), uint8_t(0x90u), uint8_t(0xA0u)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEDu), uint8_t(0x9Fu), uint8_t(0xBFu)}) == true);

        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEDu), uint8_t(0xA0u), uint8_t(0xBFu)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEDu), uint8_t(0xA1u), uint8_t(0xBFu)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEDu), uint8_t(0x9Fu), uint8_t(0xC0u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEDu), uint8_t(0x9Fu), uint8_t(0xC1u)}) == false);

        //////////////////////////////////////////////

        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEEu), uint8_t(0x7Eu), uint8_t(0x80u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEEu), uint8_t(0x7Fu), uint8_t(0x80u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEEu), uint8_t(0x80u), uint8_t(0x7Eu)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEEu), uint8_t(0x80u), uint8_t(0x7Fu)}) == false);

        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEEu), uint8_t(0x80u), uint8_t(0x80u)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEEu), uint8_t(0xA0u), uint8_t(0xA0u)}) == true);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEFu), uint8_t(0xBFu), uint8_t(0xBFu)}) == true);

        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEFu), uint8_t(0xC0u), uint8_t(0xBFu)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEFu), uint8_t(0xC1u), uint8_t(0xBFu)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEFu), uint8_t(0xBFu), uint8_t(0xC0u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xEFu), uint8_t(0xBFu), uint8_t(0xC1u)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xF0u), uint8_t(0xBFu), uint8_t(0xBFu)}) == false);
        REQUIRE(fkyaml::detail::utf8::validate({uint8_t(0xF1u), uint8_t(0xBFu), uint8_t(0xBFu)}) == false);
    }

    SECTION("4 byte characters encoded in UTF-8") {
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xDEu), uint8_t(0x90u), uint8_t(0x80u), uint8_t(0x80u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xDFu), uint8_t(0x90u), uint8_t(0x80u), uint8_t(0x80u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xE0u), uint8_t(0x8Eu), uint8_t(0x80u), uint8_t(0x80u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xE0u), uint8_t(0x8Fu), uint8_t(0x80u), uint8_t(0x80u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xE0u), uint8_t(0x90u), uint8_t(0x7Eu), uint8_t(0x80u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xE0u), uint8_t(0x90u), uint8_t(0x7Fu), uint8_t(0x80u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xE0u), uint8_t(0x90u), uint8_t(0x80u), uint8_t(0x7Eu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xE0u), uint8_t(0x90u), uint8_t(0x80u), uint8_t(0x7Fu)}) == false);

        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF0u), uint8_t(0x90u), uint8_t(0x80u), uint8_t(0x80u)}) == true);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF0u), uint8_t(0xA8u), uint8_t(0xA0u), uint8_t(0xA0u)}) == true);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF0u), uint8_t(0xBFu), uint8_t(0xBFu), uint8_t(0xBFu)}) == true);

        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF0u), uint8_t(0xC0u), uint8_t(0xBFu), uint8_t(0xBFu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF0u), uint8_t(0xC1u), uint8_t(0xBFu), uint8_t(0xBFu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF0u), uint8_t(0xBFu), uint8_t(0xC0u), uint8_t(0xBFu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF0u), uint8_t(0xBFu), uint8_t(0xC1u), uint8_t(0xBFu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF0u), uint8_t(0xBFu), uint8_t(0xBFu), uint8_t(0xC0u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF0u), uint8_t(0xBFu), uint8_t(0xBFu), uint8_t(0xC1u)}) == false);

        ////////////////////////////////////////////////////

        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF1u), uint8_t(0x7Eu), uint8_t(0x80u), uint8_t(0x80u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF1u), uint8_t(0x7Fu), uint8_t(0x80u), uint8_t(0x80u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF1u), uint8_t(0x80u), uint8_t(0x7Eu), uint8_t(0x80u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF1u), uint8_t(0x80u), uint8_t(0x7Fu), uint8_t(0x80u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF1u), uint8_t(0x80u), uint8_t(0x80u), uint8_t(0x7Eu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF1u), uint8_t(0x80u), uint8_t(0x80u), uint8_t(0x7Fu)}) == false);

        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF1u), uint8_t(0x80u), uint8_t(0x80u), uint8_t(0x80u)}) == true);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF2u), uint8_t(0xA0u), uint8_t(0xA0u), uint8_t(0xA0u)}) == true);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF3u), uint8_t(0xBFu), uint8_t(0xBFu), uint8_t(0xBFu)}) == true);

        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF3u), uint8_t(0xC0u), uint8_t(0xBFu), uint8_t(0xBFu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF3u), uint8_t(0xC1u), uint8_t(0xBFu), uint8_t(0xBFu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF3u), uint8_t(0xBFu), uint8_t(0xC0u), uint8_t(0xBFu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF3u), uint8_t(0xBFu), uint8_t(0xC1u), uint8_t(0xBFu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF3u), uint8_t(0xBFu), uint8_t(0xBFu), uint8_t(0xC0u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF3u), uint8_t(0xBFu), uint8_t(0xBFu), uint8_t(0xC1u)}) == false);

        ////////////////////////////////////////////////////

        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF4u), uint8_t(0x7Eu), uint8_t(0x80u), uint8_t(0x80u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF4u), uint8_t(0x7Fu), uint8_t(0x80u), uint8_t(0x80u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF4u), uint8_t(0x80u), uint8_t(0x7Eu), uint8_t(0x80u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF4u), uint8_t(0x80u), uint8_t(0x7Fu), uint8_t(0x80u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF4u), uint8_t(0x80u), uint8_t(0x80u), uint8_t(0x7Eu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF4u), uint8_t(0x80u), uint8_t(0x80u), uint8_t(0x7Fu)}) == false);

        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF4u), uint8_t(0x80u), uint8_t(0x80u), uint8_t(0x80u)}) == true);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF4u), uint8_t(0x88u), uint8_t(0xA0u), uint8_t(0x80u)}) == true);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF4u), uint8_t(0x8Fu), uint8_t(0xBFu), uint8_t(0xBFu)}) == true);

        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF4u), uint8_t(0x90u), uint8_t(0xBFu), uint8_t(0xBFu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF4u), uint8_t(0x91u), uint8_t(0xBFu), uint8_t(0xBFu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF4u), uint8_t(0x8Fu), uint8_t(0xC0u), uint8_t(0xBFu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF4u), uint8_t(0x8Fu), uint8_t(0xC1u), uint8_t(0xBFu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF4u), uint8_t(0x8Fu), uint8_t(0xBFu), uint8_t(0xC0u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF4u), uint8_t(0x8Fu), uint8_t(0xBFu), uint8_t(0xC1u)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF5u), uint8_t(0x8Fu), uint8_t(0xBFu), uint8_t(0xBFu)}) == false);
        REQUIRE(
            fkyaml::detail::utf8::validate({uint8_t(0xF6u), uint8_t(0x8Fu), uint8_t(0xBFu), uint8_t(0xBFu)}) == false);
    }
}

TEST_CASE("UTF8_FromUTF16") {
    SECTION("valid UTF-16 character(s)") {
        struct test_params {
            std::array<char16_t, 2> utf16;
            std::array<uint8_t, 4> utf8_bytes;
            std::size_t consumed_size;
            std::size_t encoded_size;
        };
        auto params = GENERATE(
            test_params {{{char16_t(0x00u)}}, {{uint8_t(0x00u)}}, 1, 1},
            test_params {{{char16_t(0x01u)}}, {{uint8_t(0x01u)}}, 1, 1},
            test_params {{{char16_t(0x7Eu)}}, {{uint8_t(0x7Eu)}}, 1, 1},
            test_params {{{char16_t(0x7Fu)}}, {{uint8_t(0x7Fu)}}, 1, 1},
            test_params {{{char16_t(0x0080u)}}, {{uint8_t(0xC2u), uint8_t(0x80u)}}, 1, 2},
            test_params {{{char16_t(0x0081u)}}, {{uint8_t(0xC2u), uint8_t(0x81u)}}, 1, 2},
            test_params {{{char16_t(0x07FEu)}}, {{uint8_t(0xDFu), uint8_t(0xBEu)}}, 1, 2},
            test_params {{{char16_t(0x07FFu)}}, {{uint8_t(0xDFu), uint8_t(0xBFu)}}, 1, 2},
            test_params {{{char16_t(0x0800u)}}, {{uint8_t(0xE0u), uint8_t(0xA0u), uint8_t(0x80u)}}, 1, 3},
            test_params {{{char16_t(0x0801u)}}, {{uint8_t(0xE0u), uint8_t(0xA0u), uint8_t(0x81u)}}, 1, 3},
            test_params {{{char16_t(0xD7FEu)}}, {{uint8_t(0xEDu), uint8_t(0x9Fu), uint8_t(0xBEu)}}, 1, 3},
            test_params {{{char16_t(0xD7FFu)}}, {{uint8_t(0xEDu), uint8_t(0x9Fu), uint8_t(0xBFu)}}, 1, 3},
            test_params {{{char16_t(0xE000u)}}, {{uint8_t(0xEEu), uint8_t(0x80u), uint8_t(0x80u)}}, 1, 3},
            test_params {{{char16_t(0xE001u)}}, {{uint8_t(0xEEu), uint8_t(0x80u), uint8_t(0x81u)}}, 1, 3},
            test_params {
                {{char16_t(0xD800u), char16_t(0xDC00u)}},
                {{uint8_t(0xF0u), uint8_t(0x90u), uint8_t(0x80u), uint8_t(0x80u)}},
                2,
                4},
            test_params {
                {{char16_t(0xD801u), char16_t(0xDC00u)}},
                {{uint8_t(0xF0u), uint8_t(0x90u), uint8_t(0x90u), uint8_t(0x80u)}},
                2,
                4},
            test_params {
                {{char16_t(0xD800u), char16_t(0xDC01u)}},
                {{uint8_t(0xF0u), uint8_t(0x90u), uint8_t(0x80u), uint8_t(0x81u)}},
                2,
                4},
            test_params {
                {{char16_t(0xDBFEu), char16_t(0xDFFFu)}},
                {{uint8_t(0xF4u), uint8_t(0x8Fu), uint8_t(0xAFu), uint8_t(0xBFu)}},
                2,
                4},
            test_params {
                {{char16_t(0xDBFFu), char16_t(0xDFFEu)}},
                {{uint8_t(0xF4u), uint8_t(0x8Fu), uint8_t(0xBFu), uint8_t(0xBEu)}},
                2,
                4},
            test_params {
                {{char16_t(0xDBFFu), char16_t(0xDFFFu)}},
                {{uint8_t(0xF4u), uint8_t(0x8Fu), uint8_t(0xBFu), uint8_t(0xBFu)}},
                2,
                4});

        std::array<uint8_t, 4> utf8_bytes;
        utf8_bytes.fill(0);
        uint32_t consumed_size {0};
        uint32_t encoded_size {0};

        fkyaml::detail::utf8::from_utf16(params.utf16, utf8_bytes, consumed_size, encoded_size);

        REQUIRE(utf8_bytes == params.utf8_bytes);
        REQUIRE(consumed_size == params.consumed_size);
        REQUIRE(encoded_size == params.encoded_size);
    }

    SECTION("invalid UTF-16 character(s)") {
        auto utf16 = GENERATE(
            std::array<char16_t, 2> {{char16_t(0xDC00u), char16_t(0xDC00u)}},
            std::array<char16_t, 2> {{char16_t(0xDBFFu), char16_t(0xDBFFu)}},
            std::array<char16_t, 2> {{char16_t(0xDBFFu), char16_t(0xE000u)}});

        std::array<uint8_t, 4> utf8_bytes;
        uint32_t consumed_size;
        uint32_t encoded_size;

        REQUIRE_THROWS_AS(
            fkyaml::detail::utf8::from_utf16(utf16, utf8_bytes, consumed_size, encoded_size), fkyaml::invalid_encoding);
    }
}

TEST_CASE("UTF8_FromUTF32") {
    SECTION("valid UTF-32 character") {
        struct test_params {
            char32_t utf32;
            std::array<uint8_t, 4> utf8_bytes;
            uint32_t size;
        };
        auto params = GENERATE(
            test_params {0x00u, {{uint8_t(0x00u)}}, 1},
            test_params {0x01u, {{uint8_t(0x01u)}}, 1},
            test_params {0x7Eu, {{uint8_t(0x7Eu)}}, 1},
            test_params {0x7Fu, {{uint8_t(0x7Fu)}}, 1},
            test_params {0x0080u, {{uint8_t(0xC2u), uint8_t(0x80u)}}, 2},
            test_params {0x0081u, {{uint8_t(0xC2u), uint8_t(0x81u)}}, 2},
            test_params {0x07FEu, {{uint8_t(0xDFu), uint8_t(0xBEu)}}, 2},
            test_params {0x07FFu, {{uint8_t(0xDFu), uint8_t(0xBFu)}}, 2},
            test_params {0x0800u, {{uint8_t(0xE0u), uint8_t(0xA0u), uint8_t(0x80u)}}, 3},
            test_params {0x0801u, {{uint8_t(0xE0u), uint8_t(0xA0u), uint8_t(0x81u)}}, 3},
            test_params {0xFFFFu, {{uint8_t(0xEFu), uint8_t(0xBFu), uint8_t(0xBFu)}}, 3},
            test_params {0x010000u, {{uint8_t(0xF0u), uint8_t(0x90u), uint8_t(0x80u), uint8_t(0x80u)}}, 4},
            test_params {0x010001u, {{uint8_t(0xF0u), uint8_t(0x90u), uint8_t(0x80u), uint8_t(0x81u)}}, 4},
            test_params {0x10FFFEu, {{uint8_t(0xF4u), uint8_t(0x8Fu), uint8_t(0xBFu), uint8_t(0xBEu)}}, 4},
            test_params {0x10FFFFu, {{uint8_t(0xF4u), uint8_t(0x8Fu), uint8_t(0xBFu), uint8_t(0xBFu)}}, 4});

        std::array<uint8_t, 4> utf8_bytes;
        utf8_bytes.fill(0);
        uint32_t size {0};
        fkyaml::detail::utf8::from_utf32(params.utf32, utf8_bytes, size);

        REQUIRE(utf8_bytes == params.utf8_bytes);
        REQUIRE(size == params.size);
    }

    SECTION("invalid UTF-32 character") {
        char32_t utf32 = 0x110000u;
        std::array<uint8_t, 4> utf8_bytes;
        uint32_t encoded_size;

        REQUIRE_THROWS_AS(fkyaml::detail::utf8::from_utf32(utf32, utf8_bytes, encoded_size), fkyaml::invalid_encoding);
    }
}
