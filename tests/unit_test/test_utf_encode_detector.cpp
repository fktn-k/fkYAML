//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <fstream>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

// generated in test/unit_test/CMakeLists.txt
#include <test_data.hpp>

#ifdef _MSC_VER
#define DISABLE_C4996 __pragma(warning(push)) __pragma(warning(disable : 4996))
#define ENABLE_C4996 __pragma(warning(pop))
#else
#define DISABLE_C4996
#define ENABLE_C4996
#endif

struct test_data_t {
    test_data_t(std::array<uint8_t, 4> input_, fkyaml::detail::utf_encode_t encode_type_, bool has_bom_)
        : input(input_),
          encode_type(encode_type_),
          has_bom(has_bom_) {
    }

    std::array<uint8_t, 4> input {{}};
    fkyaml::detail::utf_encode_t encode_type {fkyaml::detail::utf_encode_t::UTF_8};
    bool has_bom {false};
};

TEST_CASE("UTFEncodeDetector_DetectEncodingType") {
    auto d = GENERATE(
        test_data_t {{{0xEFu, 0xBBu, 0xBFu, 0x80u}}, fkyaml::detail::utf_encode_t::UTF_8, true},
        test_data_t {{{0xEFu, 0, 0xBFu, 0x80u}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0xEFu, 0xBBu, 0, 0x80u}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0, 0xBBu, 0xBFu, 0x80u}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0, 0, 0xFEu, 0xFFu}}, fkyaml::detail::utf_encode_t::UTF_32BE, true},
        test_data_t {{{0x80u, 0, 0xFEu, 0xFFu}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0, 0x80u, 0xFEu, 0xFFu}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0, 0, 0x80u, 0xFFu}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0, 0, 0xFEu, 0x80u}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0xFFu, 0xFEu, 0, 0}}, fkyaml::detail::utf_encode_t::UTF_32LE, true},
        test_data_t {{{0x80u, 0xFEu, 0, 0}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0xFFu, 0x80u, 0, 0}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0xFFu, 0xFEu, 0x80u, 0}}, fkyaml::detail::utf_encode_t::UTF_16LE, true},
        test_data_t {{{0xFFu, 0xFEu, 0, 0x80u}}, fkyaml::detail::utf_encode_t::UTF_16LE, true},
        test_data_t {{{0xFEu, 0xFFu, 0x80u, 0x80u}}, fkyaml::detail::utf_encode_t::UTF_16BE, true},
        test_data_t {{{0x80u, 0xFFu, 0x80u, 0x80u}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0xFEu, 0x80u, 0x80u, 0x80u}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0xFFu, 0xFEu, 0x80u, 0x80u}}, fkyaml::detail::utf_encode_t::UTF_16LE, true},
        test_data_t {{{0x80u, 0xFEu, 0x80u, 0x80u}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0xFFu, 0x80u, 0x80u, 0x80u}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0, 0, 0, 1}}, fkyaml::detail::utf_encode_t::UTF_32BE, false},
        test_data_t {{{0, 0, 0, 0x40u}}, fkyaml::detail::utf_encode_t::UTF_32BE, false},
        test_data_t {{{0, 0, 0, 0x7Fu}}, fkyaml::detail::utf_encode_t::UTF_32BE, false},
        test_data_t {{{0x80u, 0, 0, 0x7Fu}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0, 0x80u, 0, 0x7Fu}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0, 0, 0x80u, 0x7Fu}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0, 0, 0, 0}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0, 0, 0, 0x80u}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{1, 0, 0, 0}}, fkyaml::detail::utf_encode_t::UTF_32LE, false},
        test_data_t {{{0x40u, 0, 0, 0}}, fkyaml::detail::utf_encode_t::UTF_32LE, false},
        test_data_t {{{0x7Fu, 0, 0, 0}}, fkyaml::detail::utf_encode_t::UTF_32LE, false},
        test_data_t {{{0, 0, 0, 0}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0x80u, 0, 0, 0}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0x7Fu, 0x80u, 0, 0}}, fkyaml::detail::utf_encode_t::UTF_8, false},
        test_data_t {{{0x7Fu, 0, 0x80u, 0}}, fkyaml::detail::utf_encode_t::UTF_16LE, false},
        test_data_t {{{0x7Fu, 0, 0, 0x80u}}, fkyaml::detail::utf_encode_t::UTF_16LE, false},
        test_data_t {{{0, 1, 1, 1}}, fkyaml::detail::utf_encode_t::UTF_16BE, false},
        test_data_t {{{0, 0x40u, 1, 1}}, fkyaml::detail::utf_encode_t::UTF_16BE, false},
        test_data_t {{{0, 0x7Fu, 1, 1}}, fkyaml::detail::utf_encode_t::UTF_16BE, false},
        test_data_t {{{1, 0, 1, 1}}, fkyaml::detail::utf_encode_t::UTF_16LE, false},
        test_data_t {{{0x40u, 0, 1, 1}}, fkyaml::detail::utf_encode_t::UTF_16LE, false},
        test_data_t {{{0x7Fu, 0, 1, 1}}, fkyaml::detail::utf_encode_t::UTF_16LE, false});

    bool has_bom = false;
    REQUIRE(fkyaml::detail::detect_encoding_type(d.input, has_bom) == d.encode_type);
    REQUIRE(has_bom == d.has_bom);
}

TEST_CASE("UTFEncodeDetector_DetectEncodingAndSkipBom") {
    ////////////////////////
    //   char iterators   //
    ////////////////////////

    SECTION("char iterators encoded in the UTF-8") {
        std::string input {char(0x60u), char(0x61u), char(0x62u), char(0x63u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_8);
        REQUIRE(begin == std::begin(input));
    }

    SECTION("char iterators encoded in the UTF-8(BOM)") {
        std::string input {char(0xEFu), char(0xBBu), char(0xBFu), char(0x60u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_8);
        REQUIRE(begin == std::begin(input) + 3);
    }

    SECTION("char iterators encoded in the UTF-16BE") {
        std::string input {0, char(0x60u), 0, char(0x61u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16BE);
        REQUIRE(begin == std::begin(input));
    }

    SECTION("char iterators encoded in the UTF-16BE(BOM)") {
        std::string input {char(0xFEu), char(0xFFu), 0, char(0x60u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16BE);
        REQUIRE(begin == std::begin(input) + 2);
    }

    SECTION("char iterators encoded in the UTF-16LE") {
        std::string input {char(0x60u), 0, char(0x61u), 0};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16LE);
        REQUIRE(begin == std::begin(input));
    }

    SECTION("char iterators encoded in the UTF-16LE(BOM)") {
        std::string input {char(0xFFu), char(0xFEu), char(0x60u), 0};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16LE);
        REQUIRE(begin == std::begin(input) + 2);
    }

    SECTION("char iterators encoded in the UTF-32BE") {
        std::string input {0, 0, 0, char(0x60u), 0, 0, 0, char(0x61u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32BE);
        REQUIRE(begin == std::begin(input));
    }

    SECTION("char iterators encoded in the UTF-32BE(BOM)") {
        std::string input {0, 0, char(0xFEu), char(0xFFu), 0, 0, 0, char(0x60u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32BE);
        REQUIRE(begin == std::begin(input) + 4);
    }

    SECTION("char iterators encoded in the UTF-32LE") {
        std::string input {char(0x60u), 0, 0, 0, char(0x61u), 0, 0, 0};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32LE);
        REQUIRE(begin == std::begin(input));
    }

    SECTION("char iterators encoded in the UTF-32LE(BOM)") {
        std::string input {char(0xFFu), char(0xFEu), 0, 0, char(0x60u), 0, 0, 0};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32LE);
        REQUIRE(begin == std::begin(input) + 4);
    }

    SECTION("empty char iterators") {
        std::string input = "";
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_8);
        REQUIRE(begin == std::begin(input));
    }

    ///////////////////////////
    //   char8_t iterators   //
    ///////////////////////////

#if FK_YAML_HAS_CHAR8_T

    SECTION("char8_t iterators encoded in the UTF-8") {
        std::u8string input {char8_t(0x60u), char8_t(0x61u), char8_t(0x62u), char8_t(0x63u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_8);
        REQUIRE(begin == std::begin(input));
    }

    SECTION("char8_t iterators encoded in the UTF-8(BOM)") {
        std::u8string input {char8_t(0xEFu), char8_t(0xBBu), char8_t(0xBFu), char8_t(0x60u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_8);
        REQUIRE(begin == std::begin(input) + 3);
    }

    SECTION("empty char8_t iterators") {
        std::u8string input = u8"";
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_8);
        REQUIRE(begin == std::begin(input));
    }

    SECTION("char8_t iterators with invalid encoding") {
        std::u8string input {char8_t(0x00u), char8_t(0x00u), char8_t(0xFEu), char8_t(0xFFu)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        REQUIRE_THROWS_AS(fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end), fkyaml::exception);
    }

#endif // FK_YAML_HAS_CHAR8_T

    ////////////////////////////
    //   char16_t iterators   //
    ////////////////////////////

    SECTION("char16_t iterators encoded in the UTF-16BE") {
        std::u16string input {char16_t(0x0060u), char16_t(0x0061u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16BE);
        REQUIRE(begin == std::begin(input));
    }

    SECTION("char16_t iterators encoded in the UTF-16BE(BOM)") {
        std::u16string input {char16_t(0xFEFFu), char16_t(0x0060u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16BE);
        REQUIRE(begin == std::begin(input) + 1);
    }

    SECTION("char16_t iterators encoded in the UTF-16LE") {
        std::u16string input {char16_t(0x6000u), char16_t(0x6100u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16LE);
        REQUIRE(begin == std::begin(input));
    }

    SECTION("char16_t iterators encoded in the UTF-16LE(BOM)") {
        std::u16string input {char16_t(0xFFFEu), char16_t(0x6000u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16LE);
        REQUIRE(begin == std::begin(input) + 1);
    }

    SECTION("empty char16_t iterators") {
        std::u16string input = u"";
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16BE);
        REQUIRE(begin == std::begin(input));
    }

    SECTION("char16_t iterators with invalid encoding") {
        std::u16string input {char16_t(0x0000u), char16_t(0xFEFFu)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        REQUIRE_THROWS_AS(fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end), fkyaml::exception);
    }

    ////////////////////////////
    //   char32_t iterators   //
    ////////////////////////////

    SECTION("char32_t iterators encoded in the UTF-32BE") {
        std::u32string input {char32_t(0x00000060u), char32_t(0x00000061u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32BE);
        REQUIRE(begin == std::begin(input));
    }

    SECTION("char32_t iterators encoded in the UTF-32BE(BOM)") {
        std::u32string input {char32_t(0x0000FEFFu), char32_t(0x00000060u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32BE);
        REQUIRE(begin == std::begin(input) + 1);
    }

    SECTION("char32_t iterators encoded in the UTF-32LE") {
        std::u32string input {char32_t(0x60000000u), char32_t(0x61000000u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32LE);
        REQUIRE(begin == std::begin(input));
    }

    SECTION("char32_t iterators encoded in the UTF-32LE(BOM)") {
        std::u32string input {char32_t(0xFFFE0000u), char32_t(0x60000000u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32LE);
        REQUIRE(begin == std::begin(input) + 1);
    }

    SECTION("empty char32_t iterators") {
        std::u32string input = U"";
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32BE);
        REQUIRE(begin == std::begin(input));
    }

    SECTION("char32_t iterators with invalid encoding") {
        std::u32string input {char32_t(0xFEFF0060u), char32_t(0x00610062u)};
        auto begin = std::begin(input);
        auto end = std::end(input);
        using iterator_type = decltype(begin);
        REQUIRE_THROWS_AS(fkyaml::detail::utf_encode_detector<iterator_type>::detect(begin, end), fkyaml::exception);
    }

    //////////////////////
    //   FILE* object   //
    //////////////////////

    SECTION("FILE* object with UTF-8 encoding") {
        DISABLE_C4996
        std::FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n.txt", "r");
        ENABLE_C4996

        REQUIRE(p_file != nullptr);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::file_utf_encode_detector::detect(p_file);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_8);
        REQUIRE(std::ftell(p_file) == 0);

        std::fclose(p_file);
    }

    SECTION("FILE* object with UTF-8(BOM) encoding") {
        DISABLE_C4996
        std::FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8bom.txt", "r");
        ENABLE_C4996

        REQUIRE(p_file != nullptr);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::file_utf_encode_detector::detect(p_file);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_8);
        REQUIRE(std::ftell(p_file) == 3);

        std::fclose(p_file);
    }

    SECTION("FILE* object with UTF-16BE encoding") {
        DISABLE_C4996
        std::FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16ben.txt", "r");
        ENABLE_C4996

        REQUIRE(p_file != nullptr);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::file_utf_encode_detector::detect(p_file);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16BE);
        REQUIRE(std::ftell(p_file) == 0);

        std::fclose(p_file);
    }

    SECTION("FILE* object with UTF-16BE(BOM) encoding") {
        DISABLE_C4996
        std::FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16bebom.txt", "r");
        ENABLE_C4996

        REQUIRE(p_file != nullptr);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::file_utf_encode_detector::detect(p_file);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16BE);
        REQUIRE(std::ftell(p_file) == 2);

        std::fclose(p_file);
    }

    SECTION("FILE* object with UTF-16LE encoding") {
        DISABLE_C4996
        std::FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16len.txt", "r");
        ENABLE_C4996

        REQUIRE(p_file != nullptr);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::file_utf_encode_detector::detect(p_file);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16LE);
        REQUIRE(std::ftell(p_file) == 0);

        std::fclose(p_file);
    }

    SECTION("FILE* object with UTF-16LE(BOM) encoding") {
        DISABLE_C4996
        std::FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16lebom.txt", "r");
        ENABLE_C4996

        REQUIRE(p_file != nullptr);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::file_utf_encode_detector::detect(p_file);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16LE);
        REQUIRE(std::ftell(p_file) == 2);

        std::fclose(p_file);
    }

    SECTION("FILE* object with UTF-32BE encoding") {
        DISABLE_C4996
        std::FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32ben.txt", "r");
        ENABLE_C4996

        REQUIRE(p_file != nullptr);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::file_utf_encode_detector::detect(p_file);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32BE);
        REQUIRE(std::ftell(p_file) == 0);

        std::fclose(p_file);
    }

    SECTION("FILE* object with UTF-32BE(BOM) encoding") {
        DISABLE_C4996
        std::FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32bebom.txt", "r");
        ENABLE_C4996

        REQUIRE(p_file != nullptr);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::file_utf_encode_detector::detect(p_file);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32BE);
        REQUIRE(std::ftell(p_file) == 4);

        std::fclose(p_file);
    }

    SECTION("FILE* object with UTF-32LE encoding") {
        DISABLE_C4996
        std::FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32len.txt", "r");
        ENABLE_C4996

        REQUIRE(p_file != nullptr);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::file_utf_encode_detector::detect(p_file);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32LE);
        REQUIRE(std::ftell(p_file) == 0);

        std::fclose(p_file);
    }

    SECTION("FILE* object with UTF-32LE(BOM) encoding") {
        DISABLE_C4996
        std::FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32lebom.txt", "r");
        ENABLE_C4996

        REQUIRE(p_file != nullptr);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::file_utf_encode_detector::detect(p_file);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32LE);
        REQUIRE(std::ftell(p_file) == 4);

        std::fclose(p_file);
    }

    SECTION("FILE* object with an empty input file") {
        DISABLE_C4996
        std::FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/single_char_byte_input.txt", "r");
        ENABLE_C4996

        REQUIRE(p_file != nullptr);
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::file_utf_encode_detector::detect(p_file);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_8);
        REQUIRE(std::ftell(p_file) == 0);

        std::fclose(p_file);
    }

    /////////////////////////////
    //   std::istream object   //
    /////////////////////////////

    SECTION("std::istream with UTF-8 encoding") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n.txt");
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::stream_utf_encode_detector::detect(ifs);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_8);
        REQUIRE(ifs.tellg() == 0);
    }

    SECTION("std::istream with UTF-8(BOM) encoding") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8bom.txt");
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::stream_utf_encode_detector::detect(ifs);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_8);
        REQUIRE(ifs.tellg() == 3);
    }

    SECTION("std::istream with UTF-16BE encoding") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16ben.txt");
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::stream_utf_encode_detector::detect(ifs);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16BE);
        REQUIRE(ifs.tellg() == 0);
    }

    SECTION("std::istream with UTF-16BE(BOM) encoding") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16bebom.txt");
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::stream_utf_encode_detector::detect(ifs);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16BE);
        REQUIRE(ifs.tellg() == 2);
    }

    SECTION("std::istream with UTF-16LE encoding") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16len.txt");
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::stream_utf_encode_detector::detect(ifs);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16LE);
        REQUIRE(ifs.tellg() == 0);
    }

    SECTION("std::istream with UTF-16LE(BOM) encoding") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16lebom.txt");
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::stream_utf_encode_detector::detect(ifs);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_16LE);
        REQUIRE(ifs.tellg() == 2);
    }

    SECTION("std::istream with UTF-32BE encoding") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32ben.txt");
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::stream_utf_encode_detector::detect(ifs);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32BE);
        REQUIRE(ifs.tellg() == 0);
    }

    SECTION("std::istream with UTF-32BE(BOM) encoding") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32bebom.txt");
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::stream_utf_encode_detector::detect(ifs);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32BE);
        REQUIRE(ifs.tellg() == 4);
    }

    SECTION("std::istream with UTF-32LE encoding") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32len.txt");
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::stream_utf_encode_detector::detect(ifs);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32LE);
        REQUIRE(ifs.tellg() == 0);
    }

    SECTION("std::istream with UTF-32LE(BOM) encoding") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32lebom.txt");
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::stream_utf_encode_detector::detect(ifs);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_32LE);
        REQUIRE(ifs.tellg() == 4);
    }

    SECTION("std::istream with an empty input file") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/single_char_byte_input.txt");
        fkyaml::detail::utf_encode_t ret = fkyaml::detail::stream_utf_encode_detector::detect(ifs);
        REQUIRE(ret == fkyaml::detail::utf_encode_t::UTF_8);
        REQUIRE(ifs.tellg() == 0);
    }
}
