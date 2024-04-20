//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <fstream>
#include <string>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

// generated in test/unit_test/CMakeLists.txt
#include <test_data.hpp>

#ifdef FK_YAML_HAS_CXX_17
    #include <string_view>
#endif

#ifdef _MSC_VER
    #define DISABLE_C4996 __pragma(warning(push)) __pragma(warning(disable : 4996))
    #define ENABLE_C4996 __pragma(warning(pop))
#else
    #define DISABLE_C4996
    #define ENABLE_C4996
#endif

namespace /* file-scoped global variable for test cases */
{

constexpr char input_file_path[] = FK_YAML_TEST_DATA_DIR "/input_adapter_test_data.txt";

} // namespace

TEST_CASE("InputAdapter_IteratorInputAdapterProvider")
{
    char input[] = "test";

    SECTION("C-style char array")
    {
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);
    }

    SECTION("char pointers for beginning/end")
    {
        auto input_adapter = fkyaml::detail::input_adapter(&input[0], &input[sizeof(input) - 1]);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);
    }

    SECTION("C-style char16_t array")
    {
        char16_t input16[] = u"test";
        auto input_adapter = fkyaml::detail::input_adapter(input16);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char16_t*>>::value);
    }

    SECTION("C-style char32_t array")
    {
        char32_t intput32[] = U"test";
        auto input_adapter = fkyaml::detail::input_adapter(intput32);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char32_t*>>::value);
    }

    SECTION("std::string")
    {
        std::string input_str(input);
        auto input_adapter = fkyaml::detail::input_adapter(input_str);
        using iterator_type = typename std::string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }

    SECTION("std::u16string")
    {
        std::u16string input_str(u"test");
        auto input_adapter = fkyaml::detail::input_adapter(input_str);
        using iterator_type = typename std::u16string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }

    SECTION("std::u32string")
    {
        std::u32string input_str(U"test");
        auto input_adapter = fkyaml::detail::input_adapter(input_str);
        using iterator_type = typename std::u32string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }

#ifdef FK_YAML_HAS_CXX_17
    SECTION("std::string_view")
    {
        std::string_view input_str_view(input);
        auto input_adapter = fkyaml::detail::input_adapter(input_str_view);
        using iterator_type = typename std::string_view::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }

    SECTION("std::u16string_view")
    {
        using namespace std::string_view_literals;
        std::u16string_view input_str_view = u"test"sv;
        auto input_adapter = fkyaml::detail::input_adapter(input_str_view);
        using iterator_type = typename std::u16string_view::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }

    SECTION("std::u32string_view")
    {
        using namespace std::string_view_literals;
        std::u32string_view input_str_view = U"test"sv;
        auto input_adapter = fkyaml::detail::input_adapter(input_str_view);
        using iterator_type = typename std::u32string_view::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }
#endif

#ifdef FK_YAML_HAS_CXX_20
    SECTION("std::u32string")
    {
        std::u8string input_str(u8"test");
        auto input_adapter = fkyaml::detail::input_adapter(input_str);
        using iterator_type = typename std::u8string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }

    SECTION("std::u8string_view")
    {
        using namespace std::string_view_literals;
        std::u8string_view input_str_view = u8"test"sv;
        auto input_adapter = fkyaml::detail::input_adapter(input_str_view);
        using iterator_type = typename std::u8string_view::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }
#endif
}

TEST_CASE("InputAdapter_FileInputAdapterProvider")
{
    SECTION("invalid FILE object pointer")
    {
        FILE* p_file = nullptr;
        REQUIRE_THROWS_AS(fkyaml::detail::input_adapter(p_file), fkyaml::exception);
    }

    SECTION("valid FILE object pointer")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(input_file_path, "r");
        ENABLE_C4996

        REQUIRE(p_file != nullptr);
        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::fclose(p_file);
    }
}

TEST_CASE("InputAdapter_StreamInputAdapterProvider")
{
    std::ifstream ifs(input_file_path);
    REQUIRE(ifs);
    auto input_adapter = fkyaml::detail::input_adapter(ifs);
    REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);
}

TEST_CASE("InputAdapter_FillBuffer")
{
    ///////////////
    //   UTF-8   //
    ///////////////

    SECTION("iterator_input_adapter for UTF-8 with a char array")
    {
        char input[] = "test source.";
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 12);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == ' ');
        REQUIRE(buffer[5] == 's');
        REQUIRE(buffer[6] == 'o');
        REQUIRE(buffer[7] == 'u');
        REQUIRE(buffer[8] == 'r');
        REQUIRE(buffer[9] == 'c');
        REQUIRE(buffer[10] == 'e');
        REQUIRE(buffer[11] == '.');
    }

    SECTION("iterator_input_adapter for UTF-8 with std::string")
    {
        std::string input = "test source.";
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 12);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == ' ');
        REQUIRE(buffer[5] == 's');
        REQUIRE(buffer[6] == 'o');
        REQUIRE(buffer[7] == 'u');
        REQUIRE(buffer[8] == 'r');
        REQUIRE(buffer[9] == 'c');
        REQUIRE(buffer[10] == 'e');
        REQUIRE(buffer[11] == '.');
    }

    SECTION("file_input_adapter for UTF-8")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter for UTF-8")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    ////////////////////
    //   UTF-8(BOM)   //
    ////////////////////

    SECTION("iterator_input_adapter for UTF-8(BOM) with a char array")
    {
        char input[] = {
            char(0xEFu), char(0xBBu), char(0xBFu), 't', 'e', 's', 't', ' ', 's', 'o', 'u', 'r', 'c', 'e', '.', 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 12);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == ' ');
        REQUIRE(buffer[5] == 's');
        REQUIRE(buffer[6] == 'o');
        REQUIRE(buffer[7] == 'u');
        REQUIRE(buffer[8] == 'r');
        REQUIRE(buffer[9] == 'c');
        REQUIRE(buffer[10] == 'e');
        REQUIRE(buffer[11] == '.');
    }

    SECTION("iterator_input_adapter for UTF-8(BOM) with std::string")
    {
        char raw_input[] = {
            char(0xEFu), char(0xBBu), char(0xBFu), 't', 'e', 's', 't', ' ', 's', 'o', 'u', 'r', 'c', 'e', '.', 0};
        std::string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 12);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == ' ');
        REQUIRE(buffer[5] == 's');
        REQUIRE(buffer[6] == 'o');
        REQUIRE(buffer[7] == 'u');
        REQUIRE(buffer[8] == 'r');
        REQUIRE(buffer[9] == 'c');
        REQUIRE(buffer[10] == 'e');
        REQUIRE(buffer[11] == '.');
    }

    SECTION("file_input_adapter for UTF-8(BOM)")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8bom.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter for UTF-8(BOM)")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8bom.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    //////////////////
    //   UTF-16BE   //
    //////////////////

    SECTION("iterator_input_adapter for UTF-16BE with a char array")
    {
        char input[] = {0, 0x61, 0x30, 0x42, char(0xD8u), 0x40, char(0xDCu), 0x0B, 0, 0x52, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    SECTION("iterator_input_adapter for UTF-16BE with std::string")
    {
        std::string input {0, 0x61, 0x30, 0x42, char(0xD8u), 0x40, char(0xDCu), 0x0B, 0, 0x52, 0, 0x5A};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 10);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
        REQUIRE(buffer[9] == char(0x5Au));
    }

    SECTION("iterator_input_adapter for UTF-16BE with a char16_t array")
    {
        char16_t input[] = {0x0061u, 0x3042u, 0xD840u, 0xDC0Bu, 0x0052u, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char16_t*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    SECTION("iterator_input_adapter for UTF-16BE with std::u16string")
    {
        char16_t raw_input[] = {0x0061u, 0x3042u, 0xD840u, 0xDC0Bu, 0x0052u, 0x005Au, 0};
        std::u16string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u16string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 10);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
        REQUIRE(buffer[9] == char(0x5Au));
    }

    SECTION("file_input_adapter for UTF-16BE")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16ben.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter for UTF-16BE")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16ben.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    ///////////////////////
    //   UTF-16BE(BOM)   //
    ///////////////////////

    SECTION("iterator_input_adapter for UTF-16BE(BOM) with a char array")
    {
        char input[] = {
            char(0xFEu), char(0xFFu), 0, 0x61, 0x30, 0x42, char(0xD8u), 0x40, char(0xDCu), 0x0B, 0, 0x52, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    SECTION("iterator_input_adapter for UTF-16BE(BOM) with std::string")
    {
        std::string input {
            char(0xFEu), char(0xFFu), 0, 0x61, 0x30, 0x42, char(0xD8u), 0x40, char(0xDCu), 0x0B, 0, 0x52};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    SECTION("iterator_input_adapter for UTF-16BE(BOM) with a char16_t array")
    {
        char16_t input[] = {0xFEFFu, 0x0061u, 0x3042u, 0xD840u, 0xDC0Bu, 0x0052u, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char16_t*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    SECTION("iterator_input_adapter for UTF-16BE(BOM) with std::u16string")
    {
        char16_t raw_input[] = {0xFEFFu, 0x0061u, 0x3042u, 0xD840u, 0xDC0Bu, 0x0052u, 0};
        std::u16string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u16string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    SECTION("file_input_adapter for UTF-16BE(BOM)")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16bebom.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter for UTF-16BE(BOM)")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16bebom.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    //////////////////
    //   UTF-16LE   //
    //////////////////

    SECTION("iterator_input_adapter for UTF-16LE with a char array")
    {
        char input[] = {0x61, 0, 0x42, 0x30, 0x40, char(0xD8u), 0x0B, char(0xDCu), 0x52, 0, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    SECTION("iterator_input_adapter for UTF-16LE with std::string")
    {
        std::string input {0x61, 0, 0x42, 0x30, 0x40, char(0xD8u), 0x0B, char(0xDCu), 0x52, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    SECTION("iterator_input_adapter for UTF-16LE with a char16_t array")
    {
        char16_t input[] = {0x6100u, 0x4230u, 0x40D8u, 0x0BDCu, 0x5200u, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char16_t*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    SECTION("iterator_input_adapter for UTF-16LE with std::u16string")
    {
        char16_t raw_input[] = {0x6100u, 0x4230u, 0x40D8u, 0x0BDCu, 0x5200u, 0};
        std::u16string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u16string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    SECTION("file_input_adapter for UTF-16LE")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16len.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter for UTF-16LE")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16len.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    ///////////////////////
    //   UTF-16LE(BOM)   //
    ///////////////////////

    SECTION("iterator_input_adapter for UTF-16LE(BOM) with a char array")
    {
        char input[] = {
            char(0xFFu), char(0xFEu), 0x61, 0, 0x42, 0x30, 0x40, char(0xD8u), 0x0B, char(0xDCu), 0x52, 0, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    SECTION("iterator_input_adapter for UTF-16LE(BOM) with std::string")
    {
        std::string input {
            char(0xFFu), char(0xFEu), 0x61, 0, 0x42, 0x30, 0x40, char(0xD8u), 0x0B, char(0xDCu), 0x52, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    SECTION("iterator_input_adapter for UTF-16LE(BOM) with a char16_t array")
    {
        char16_t input[] = {0xFFFEu, 0x6100u, 0x4230u, 0x40D8u, 0x0BDCu, 0x5200u, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char16_t*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    SECTION("iterator_input_adapter for UTF-16LE(BOM) with std::u16string")
    {
        char16_t raw_input[] = {0xFFFEu, 0x6100u, 0x4230u, 0x40D8u, 0x0BDCu, 0x5200u, 0};
        std::u16string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u16string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    SECTION("file_input_adapter for UTF-16LE(BOM)")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16lebom.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter for UTF-16LE(BOM)")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16lebom.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 9);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
        REQUIRE(buffer[8] == char(0x52u));
    }

    //////////////////
    //   UTF-32BE   //
    //////////////////

    SECTION("iterator_input_adapter for UTF-32BE with a char array")
    {
        char input[] = {0, 0, 0, 0x61, 0, 0, 0x30, 0x42, 0, 0x02, 0, 0x0B, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("iterator_input_adapter for UTF-32BE with std::string")
    {
        std::string input {0, 0, 0, 0x61, 0, 0, 0x30, 0x42, 0, 0x02, 0, 0x0B};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("iterator_input_adapter for UTF-32BE with a char32_t array")
    {
        char32_t input[] = {0x00000061u, 0x00003042u, 0x0002000Bu, 0x00000000};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char32_t*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("iterator_input_adapter for UTF-32BE with std::u32string")
    {
        char32_t raw_input[] = {0x00000061u, 0x00003042u, 0x0002000Bu, 0x00000000};
        std::u32string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u32string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("file_input_adapter for UTF-32BE")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32ben.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter for UTF-32BE")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32ben.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    ///////////////////////
    //   UTF-32BE(BOM)   //
    ///////////////////////

    SECTION("iterator_input_adapter for UTF-32BE(BOM) with a char array")
    {
        char input[] = {0, 0, char(0xFEu), char(0xFFu), 0, 0, 0, 0x61, 0, 0, 0x30, 0x42, 0, 0x02, 0, 0x0B, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("iterator_input_adapter for UTF-32BE(BOM) with std::string")
    {
        std::string input {0, 0, char(0xFEu), char(0xFFu), 0, 0, 0, 0x61, 0, 0, 0x30, 0x42, 0, 0x02, 0, 0x0B};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("iterator_input_adapter for UTF-32BE(BOM) with a char32_t array")
    {
        char32_t input[] = {0x0000FEFFu, 0x00000061u, 0x00003042u, 0x0002000Bu, 0x00000000};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char32_t*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("iterator_input_adapter for UTF-32BE(BOM) with std::u32string")
    {
        char32_t raw_input[] = {0x0000FEFFu, 0x00000061u, 0x00003042u, 0x0002000Bu, 0x00000000};
        std::u32string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u32string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("file_input_adapter for UTF-32BE(BOM)")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32bebom.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter for UTF-32BE(BOM)")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32bebom.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    //////////////////
    //   UTF-32LE   //
    //////////////////

    SECTION("iterator_input_adapter for UTF-32LE with a char array")
    {
        char input[] = {0x61, 0, 0, 0, 0x42, 0x30, 0, 0, 0x0B, 0, 0x02, 0, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("iterator_input_adapter for UTF-32LE with std::string")
    {
        std::string input {0x61, 0, 0, 0, 0x42, 0x30, 0, 0, 0x0B, 0, 0x02, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("iterator_input_adapter for UTF-32LE with a char32_t array")
    {
        char32_t input[] = {0x61000000u, 0x42300000u, 0x0B000200u, 0x00000000};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char32_t*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("iterator_input_adapter for UTF-32LE with std::u32string")
    {
        char32_t raw_input[] = {0x61000000u, 0x42300000u, 0x0B000200u, 0x00000000};
        std::u32string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u32string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("file_input_adapter for UTF-32LE")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32len.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter for UTF-32LE")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32len.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    ///////////////////////
    //   UTF-32LE(BOM)   //
    ///////////////////////

    SECTION("iterator_input_adapter for UTF-32LE(BOM) with a char array")
    {
        char input[] = {char(0xFFu), char(0xFEu), 0, 0, 0x61, 0, 0, 0, 0x42, 0x30, 0, 0, 0x0B, 0, 0x02, 0, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("iterator_input_adapter for UTF-32LE(BOM) with std::string")
    {
        std::string input {char(0xFFu), char(0xFEu), 0, 0, 0x61, 0, 0, 0, 0x42, 0x30, 0, 0, 0x0B, 0, 0x02, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("iterator_input_adapter for UTF-32LE(BOM) with a char32_t array")
    {
        char32_t input[] = {0xFFFE0000u, 0x61000000u, 0x42300000u, 0x0B000200u, 0x00000000};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char32_t*>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("iterator_input_adapter for UTF-32LE(BOM) with std::u32string")
    {
        char32_t raw_input[] = {0xFFFE0000u, 0x61000000u, 0x42300000u, 0x0B000200u, 0x00000000};
        std::u32string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u32string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }

    SECTION("file_input_adapter for UTF-32LE(BOM)")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32lebom.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter for UTF-32LE(BOM)")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32lebom.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == 'a');
        REQUIRE(buffer[1] == char(0xE3u));
        REQUIRE(buffer[2] == char(0x81u));
        REQUIRE(buffer[3] == char(0x82u));
        REQUIRE(buffer[4] == char(0xF0u));
        REQUIRE(buffer[5] == char(0xA0u));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x8Bu));
    }
}

TEST_CASE("InputAdapter_FillBufferUTF8CharsValidation")
{
    /////////////////////////////////
    //   UTF-8 1-Byte Characters   //
    /////////////////////////////////

    SECTION("file_input_adapter with valid 1-byte UTF-8 encodings")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_1byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 3);
        REQUIRE(buffer[0] == char(0x5Au));
        REQUIRE(buffer[1] == char(0x30u));
        REQUIRE(buffer[2] == char(0x61u));

        std::fclose(p_file);
    }

    SECTION("file_input_adapter with invalid 1-byte UTF-8 encodings")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_1byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        REQUIRE_THROWS_AS(input_adapter.fill_buffer(buffer), fkyaml::invalid_encoding);

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter with valid 1-byte UTF-8 encodings")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_1byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 3);
        REQUIRE(buffer[0] == char(0x5Au));
        REQUIRE(buffer[1] == char(0x30u));
        REQUIRE(buffer[2] == char(0x61u));
    }

    SECTION("stream_input_adapter with invalid 1-byte UTF-8 encodings")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_1byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        REQUIRE_THROWS_AS(input_adapter.fill_buffer(buffer), fkyaml::invalid_encoding);
    }

    /////////////////////////////////
    //   UTF-8 2-Byte Characters   //
    /////////////////////////////////

    SECTION("file_input_adapter with valid 2-byte UTF-8 encodings")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_2byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 4);
        REQUIRE(buffer[0] == char(0xC2u));
        REQUIRE(buffer[1] == char(0x80u));
        REQUIRE(buffer[2] == char(0xDFu));
        REQUIRE(buffer[3] == char(0xBFu));

        std::fclose(p_file);
    }

    SECTION("file_input_adapter with invalid 2-byte UTF-8 encodings")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_2byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        REQUIRE_THROWS_AS(input_adapter.fill_buffer(buffer), fkyaml::invalid_encoding);

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter with valid 2-byte UTF-8 encodings")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_2byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 4);
        REQUIRE(buffer[0] == char(0xC2u));
        REQUIRE(buffer[1] == char(0x80u));
        REQUIRE(buffer[2] == char(0xDFu));
        REQUIRE(buffer[3] == char(0xBFu));
    }

    SECTION("stream_input_adapter with invalid 2-byte UTF-8 encodings")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_2byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        REQUIRE_THROWS_AS(input_adapter.fill_buffer(buffer), fkyaml::invalid_encoding);
    }

    /////////////////////////////////
    //   UTF-8 3-Byte Characters   //
    /////////////////////////////////

    SECTION("file_input_adapter with valid 3-byte UTF-8 encodings")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_3byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 6);
        REQUIRE(buffer[0] == char(0xE0u));
        REQUIRE(buffer[1] == char(0x80u));
        REQUIRE(buffer[2] == char(0x80u));
        REQUIRE(buffer[3] == char(0xECu));
        REQUIRE(buffer[4] == char(0xBFu));
        REQUIRE(buffer[5] == char(0xBFu));

        std::fclose(p_file);
    }

    SECTION("file_input_adapter with invalid 3-byte UTF-8 encodings")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_3byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        REQUIRE_THROWS_AS(input_adapter.fill_buffer(buffer), fkyaml::invalid_encoding);

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter with valid 3-byte UTF-8 encodings")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_3byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 6);
        REQUIRE(buffer[0] == char(0xE0u));
        REQUIRE(buffer[1] == char(0x80u));
        REQUIRE(buffer[2] == char(0x80u));
        REQUIRE(buffer[3] == char(0xECu));
        REQUIRE(buffer[4] == char(0xBFu));
        REQUIRE(buffer[5] == char(0xBFu));
    }

    SECTION("stream_input_adapter with invalid 3-byte UTF-8 encodings")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_3byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        REQUIRE_THROWS_AS(input_adapter.fill_buffer(buffer), fkyaml::invalid_encoding);
    }

    /////////////////////////////////
    //   UTF-8 4-Byte Characters   //
    /////////////////////////////////

    SECTION("file_input_adapter with valid 4-byte UTF-8 encodings")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_4byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == char(0xF0u));
        REQUIRE(buffer[1] == char(0x90u));
        REQUIRE(buffer[2] == char(0x80u));
        REQUIRE(buffer[3] == char(0x80u));
        REQUIRE(buffer[4] == char(0xF2u));
        REQUIRE(buffer[5] == char(0xBFu));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x80u));

        std::fclose(p_file);
    }

    SECTION("file_input_adapter with invalid 4-byte UTF-8 encodings")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_4byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::string buffer {};
        REQUIRE_THROWS_AS(input_adapter.fill_buffer(buffer), fkyaml::invalid_encoding);

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter with valid 4-byte UTF-8 encodings")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_4byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        input_adapter.fill_buffer(buffer);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer[0] == char(0xF0u));
        REQUIRE(buffer[1] == char(0x90u));
        REQUIRE(buffer[2] == char(0x80u));
        REQUIRE(buffer[3] == char(0x80u));
        REQUIRE(buffer[4] == char(0xF2u));
        REQUIRE(buffer[5] == char(0xBFu));
        REQUIRE(buffer[6] == char(0x80u));
        REQUIRE(buffer[7] == char(0x80u));
    }

    SECTION("stream_input_adapter with invalid 4-byte UTF-8 encodings")
    {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_4byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        std::string buffer {};
        REQUIRE_THROWS_AS(input_adapter.fill_buffer(buffer), fkyaml::invalid_encoding);
    }
}
