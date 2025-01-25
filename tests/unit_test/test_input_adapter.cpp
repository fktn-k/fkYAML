//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
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

TEST_CASE("InputAdapter_IteratorInputAdapterProvider") {
    char input[] = "test";

    SECTION("C-style char array") {
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);
    }

    SECTION("char pointers for beginning/end") {
        auto input_adapter = fkyaml::detail::input_adapter(&input[0], &input[sizeof(input) - 1]);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);
    }

#if FK_YAML_HAS_CHAR8_T
    SECTION("C-style char8_t array") {
        char8_t input8[] = u8"test";
        auto input_adapter = fkyaml::detail::input_adapter(input8);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char8_t*>>::value);
    }
#endif

    SECTION("C-style char16_t array") {
        char16_t input16[] = u"test";
        auto input_adapter = fkyaml::detail::input_adapter(input16);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char16_t*>>::value);
    }

    SECTION("C-style char32_t array") {
        char32_t intput32[] = U"test";
        auto input_adapter = fkyaml::detail::input_adapter(intput32);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char32_t*>>::value);
    }

    SECTION("std::string") {
        std::string input_str(input);
        auto input_adapter = fkyaml::detail::input_adapter(input_str);
        using iterator_type = typename std::string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }

    SECTION("std::u16string") {
        std::u16string input_str(u"test");
        auto input_adapter = fkyaml::detail::input_adapter(input_str);
        using iterator_type = typename std::u16string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }

    SECTION("std::u32string") {
        std::u32string input_str(U"test");
        auto input_adapter = fkyaml::detail::input_adapter(input_str);
        using iterator_type = typename std::u32string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }

#ifdef FK_YAML_HAS_CXX_17
    SECTION("std::string_view") {
        std::string_view input_str_view(input);
        auto input_adapter = fkyaml::detail::input_adapter(input_str_view);
        using iterator_type = typename std::string_view::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }

    SECTION("std::u16string_view") {
        using namespace std::string_view_literals;
        std::u16string_view input_str_view = u"test"sv;
        auto input_adapter = fkyaml::detail::input_adapter(input_str_view);
        using iterator_type = typename std::u16string_view::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }

    SECTION("std::u32string_view") {
        using namespace std::string_view_literals;
        std::u32string_view input_str_view = U"test"sv;
        auto input_adapter = fkyaml::detail::input_adapter(input_str_view);
        using iterator_type = typename std::u32string_view::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }
#endif

#if FK_YAML_HAS_CHAR8_T
    SECTION("std::u8string") {
        std::u8string input_str(u8"test");
        auto input_adapter = fkyaml::detail::input_adapter(input_str);
        using iterator_type = typename std::u8string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }

    SECTION("std::u8string_view") {
        using namespace std::string_view_literals;
        std::u8string_view input_str_view = u8"test"sv;
        auto input_adapter = fkyaml::detail::input_adapter(input_str_view);
        using iterator_type = typename std::u8string_view::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }
#endif
}

TEST_CASE("InputAdapter_FileInputAdapterProvider") {
    SECTION("invalid FILE object pointer") {
        FILE* p_file = nullptr;
        REQUIRE_THROWS_AS(fkyaml::detail::input_adapter(p_file), fkyaml::exception);
    }

    SECTION("valid FILE object pointer") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data.txt", "r");
        ENABLE_C4996

        REQUIRE(p_file != nullptr);
        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        std::fclose(p_file);
    }
}

TEST_CASE("InputAdapter_StreamInputAdapterProvider") {
    SECTION("invalid stream") {
        std::ifstream ifs("");
        REQUIRE_THROWS_AS(fkyaml::detail::input_adapter(ifs), fkyaml::exception);
    }

    SECTION("valid stream") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data.txt");
        REQUIRE(ifs);
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);
    }
}

TEST_CASE("InputAdapter_EmptyInput") {
    SECTION("C-style char array") {
        char input[] = "";
        auto input_adapter = fkyaml::detail::input_adapter(input);
        auto view = input_adapter.get_buffer_view();
        REQUIRE(view.empty());
    }

#if FK_YAML_HAS_CHAR8_T
    SECTION("C-style char8_t array") {
        char8_t input[] = u8"";
        auto input_adapter = fkyaml::detail::input_adapter(input);
        auto view = input_adapter.get_buffer_view();
        REQUIRE(view.empty());
    }
#endif

    SECTION("C-style char16_t array") {
        char16_t input[] = u"";
        auto input_adapter = fkyaml::detail::input_adapter(input);
        auto view = input_adapter.get_buffer_view();
        REQUIRE(view.empty());
    }

    SECTION("C-style char32_t array") {
        char32_t input[] = U"";
        auto input_adapter = fkyaml::detail::input_adapter(input);
        auto view = input_adapter.get_buffer_view();
        REQUIRE(view.empty());
    }

    ////////////////////////////////////////////////////////

    SECTION("std::string") {
        std::string input {};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        auto view = input_adapter.get_buffer_view();
        REQUIRE(view.empty());
    }

    SECTION("std::u16string") {
        std::u16string input {};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        auto view = input_adapter.get_buffer_view();
        REQUIRE(view.empty());
    }

    SECTION("std::u32string") {
        std::u32string input {};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        auto view = input_adapter.get_buffer_view();
        REQUIRE(view.empty());
    }

#ifdef FK_YAML_HAS_CXX_17
    SECTION("std::string_view") {
        std::string_view input {};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        auto view = input_adapter.get_buffer_view();
        REQUIRE(view.empty());
    }

    SECTION("std::u16string_view") {
        using namespace std::string_view_literals;
        std::u16string_view input = u""sv;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        auto view = input_adapter.get_buffer_view();
        REQUIRE(view.empty());
    }

    SECTION("std::u32string_view") {
        using namespace std::string_view_literals;
        std::u32string_view input = U""sv;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        auto view = input_adapter.get_buffer_view();
        REQUIRE(view.empty());
    }
#endif

#if FK_YAML_HAS_CHAR8_T
    SECTION("std::u8string") {
        std::u8string input {};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        auto view = input_adapter.get_buffer_view();
        REQUIRE(view.empty());
    }

    SECTION("std::u8string_view") {
        using namespace std::string_view_literals;
        std::u8string_view input = u8""sv;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        auto view = input_adapter.get_buffer_view();
        REQUIRE(view.empty());
    }
#endif

    SECTION("FILE object pointer") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_empty.txt", "r");
        ENABLE_C4996

        REQUIRE(p_file != nullptr);
        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        auto view = input_adapter.get_buffer_view();
        REQUIRE(view.empty());

        std::fclose(p_file);
    }

    SECTION("input stream") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_empty.txt");
        REQUIRE(ifs);
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        auto view = input_adapter.get_buffer_view();
        REQUIRE(view.empty());
    }
}

TEST_CASE("InputAdapter_GetBufferView_UTF8N") {
    SECTION("iterator_input_adapter with a char array") {
        char input[] = "test source.";
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::string") {
        std::string input = "test source.";
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("file_input_adapter") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("stream_input_adapter") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

TEST_CASE("InputAdapter_GetBufferView_UTF8BOM") {
    SECTION("iterator_input_adapter with a char array") {
        char input[] = {
            char(0xEFu), char(0xBBu), char(0xBFu), 't', 'e', 's', 't', ' ', 's', 'o', 'u', 'r', 'c', 'e', '.', 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::string") {
        char raw_input[] = {
            char(0xEFu), char(0xBBu), char(0xBFu), 't', 'e', 's', 't', ' ', 's', 'o', 'u', 'r', 'c', 'e', '.', 0};
        std::string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("file_input_adapter") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8bom.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("stream_input_adapter") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8bom.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

TEST_CASE("InputAdapter_GetBufferView_UTF16BEN") {
    SECTION("iterator_input_adapter with a char array") {
        char input[] = {0, 0x61, 0x30, 0x42, char(0xD8u), 0x40, char(0xDCu), 0x0B, 0, 0x52, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::string") {
        std::string input {0, 0x61, 0x30, 0x42, char(0xD8u), 0x40, char(0xDCu), 0x0B, 0, 0x52, 0, 0x5A};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with a char16_t array") {
        char16_t input[] = {0x0061u, 0x3042u, 0xD840u, 0xDC0Bu, 0x0052u, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char16_t*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::u16string") {
        char16_t raw_input[] = {0x0061u, 0x3042u, 0xD840u, 0xDC0Bu, 0x0052u, 0x005Au, 0};
        std::u16string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u16string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("file_input_adapter") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16ben.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("stream_input_adapter") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16ben.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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
}

TEST_CASE("InputAdapter_GetBufferView_UTF16BEBOM") {
    SECTION("iterator_input_adapter with a char array") {
        char input[] = {
            char(0xFEu), char(0xFFu), 0, 0x61, 0x30, 0x42, char(0xD8u), 0x40, char(0xDCu), 0x0B, 0, 0x52, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::string") {
        std::string input {
            char(0xFEu), char(0xFFu), 0, 0x61, 0x30, 0x42, char(0xD8u), 0x40, char(0xDCu), 0x0B, 0, 0x52};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with a char16_t array") {
        char16_t input[] = {0xFEFFu, 0x0061u, 0x3042u, 0xD840u, 0xDC0Bu, 0x0052u, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char16_t*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::u16string") {
        char16_t raw_input[] = {0xFEFFu, 0x0061u, 0x3042u, 0xD840u, 0xDC0Bu, 0x0052u, 0};
        std::u16string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u16string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("file_input_adapter") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16bebom.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("stream_input_adapter") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16bebom.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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
}

TEST_CASE("InputAdapter_GetBufferView_UTF16LEN") {
    SECTION("iterator_input_adapter with a char array") {
        char input[] = {0x61, 0, 0x42, 0x30, 0x40, char(0xD8u), 0x0B, char(0xDCu), 0x52, 0, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::string") {
        std::string input {0x61, 0, 0x42, 0x30, 0x40, char(0xD8u), 0x0B, char(0xDCu), 0x52, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with a char16_t array") {
        char16_t input[] = {0x6100u, 0x4230u, 0x40D8u, 0x0BDCu, 0x5200u, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char16_t*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::u16string") {
        char16_t raw_input[] = {0x6100u, 0x4230u, 0x40D8u, 0x0BDCu, 0x5200u, 0};
        std::u16string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u16string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("file_input_adapter") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16len.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("stream_input_adapter") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16len.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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
}

TEST_CASE("InputAdapter_GetBufferView_UTF16LEBOM") {
    SECTION("iterator_input_adapter with a char array") {
        char input[] = {
            char(0xFFu), char(0xFEu), 0x61, 0, 0x42, 0x30, 0x40, char(0xD8u), 0x0B, char(0xDCu), 0x52, 0, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::string") {
        std::string input {
            char(0xFFu), char(0xFEu), 0x61, 0, 0x42, 0x30, 0x40, char(0xD8u), 0x0B, char(0xDCu), 0x52, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with a char16_t array") {
        char16_t input[] = {0xFFFEu, 0x6100u, 0x4230u, 0x40D8u, 0x0BDCu, 0x5200u, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char16_t*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::u16string") {
        char16_t raw_input[] = {0xFFFEu, 0x6100u, 0x4230u, 0x40D8u, 0x0BDCu, 0x5200u, 0};
        std::u16string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u16string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("file_input_adapter") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16lebom.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("stream_input_adapter") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16lebom.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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
}

TEST_CASE("InputAdapter_GetBufferView_UTF32BEN") {
    SECTION("iterator_input_adapter with a char array") {
        char input[] = {0, 0, 0, 0x61, 0, 0, 0x30, 0x42, 0, 0x02, 0, 0x0B, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::string") {
        std::string input {0, 0, 0, 0x61, 0, 0, 0x30, 0x42, 0, 0x02, 0, 0x0B};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with a char32_t array") {
        char32_t input[] = {0x00000061u, 0x00003042u, 0x0002000Bu, 0x00000000};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char32_t*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::u32string") {
        char32_t raw_input[] = {0x00000061u, 0x00003042u, 0x0002000Bu, 0x00000000};
        std::u32string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u32string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("file_input_adapter") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32ben.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("stream_input_adapter") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32ben.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

TEST_CASE("InputAdapter_GetBufferView_UTF32BEBOM") {
    SECTION("iterator_input_adapter with a char array") {
        char input[] = {0, 0, char(0xFEu), char(0xFFu), 0, 0, 0, 0x61, 0, 0, 0x30, 0x42, 0, 0x02, 0, 0x0B, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::string") {
        std::string input {0, 0, char(0xFEu), char(0xFFu), 0, 0, 0, 0x61, 0, 0, 0x30, 0x42, 0, 0x02, 0, 0x0B};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with a char32_t array") {
        char32_t input[] = {0x0000FEFFu, 0x00000061u, 0x00003042u, 0x0002000Bu, 0x00000000};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char32_t*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::u32string") {
        char32_t raw_input[] = {0x0000FEFFu, 0x00000061u, 0x00003042u, 0x0002000Bu, 0x00000000};
        std::u32string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u32string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("file_input_adapter") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32bebom.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("stream_input_adapter") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32bebom.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

TEST_CASE("InputAdapter_GetBufferView_UTF32LEN") {
    SECTION("iterator_input_adapter with a char array") {
        char input[] = {0x61, 0, 0, 0, 0x42, 0x30, 0, 0, 0x0B, 0, 0x02, 0, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::string") {
        std::string input {0x61, 0, 0, 0, 0x42, 0x30, 0, 0, 0x0B, 0, 0x02, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with a char32_t array") {
        char32_t input[] = {0x61000000u, 0x42300000u, 0x0B000200u, 0x00000000};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char32_t*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::u32string") {
        char32_t raw_input[] = {0x61000000u, 0x42300000u, 0x0B000200u, 0x00000000};
        std::u32string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u32string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("file_input_adapter") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32len.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("stream_input_adapter") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32len.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

TEST_CASE("InputAdapter_GetBufferView_UTF32LEBOM") {
    SECTION("iterator_input_adapter with a char array") {
        char input[] = {char(0xFFu), char(0xFEu), 0, 0, 0x61, 0, 0, 0, 0x42, 0x30, 0, 0, 0x0B, 0, 0x02, 0, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::string") {
        std::string input {char(0xFFu), char(0xFEu), 0, 0, 0x61, 0, 0, 0, 0x42, 0x30, 0, 0, 0x0B, 0, 0x02, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with a char32_t array") {
        char32_t input[] = {0xFFFE0000u, 0x61000000u, 0x42300000u, 0x0B000200u, 0x00000000};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char32_t*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with std::u32string") {
        char32_t raw_input[] = {0xFFFE0000u, 0x61000000u, 0x42300000u, 0x0B000200u, 0x00000000};
        std::u32string input = raw_input;
        auto input_adapter = fkyaml::detail::input_adapter(input);
        using itr_type = typename std::u32string::const_iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<itr_type>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("file_input_adapter") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32lebom.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("stream_input_adapter") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32lebom.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

TEST_CASE("InputAdapter_GetBufferView_UTF8CharsValidation") {
    /////////////////////////////////
    //   UTF-8 1-Byte Characters   //
    /////////////////////////////////

    SECTION("iterator_input_adapter with valid 1-byte UTF-8 encodings") {
        char input[] = {0x5A, 0x30, 0x61, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 3);
        REQUIRE(buffer[0] == char(0x5Au));
        REQUIRE(buffer[1] == char(0x30u));
        REQUIRE(buffer[2] == char(0x61u));
    }

    SECTION("iterator_input_adapter with invalid 1-byte UTF-8 encodings") {
        char input[] = {char(0x81u), char(0x82u), char(0x83u), 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        REQUIRE_THROWS_AS(input_adapter.get_buffer_view(), fkyaml::invalid_encoding);
    }

    SECTION("file_input_adapter with valid 1-byte UTF-8 encodings") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_1byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 3);
        REQUIRE(buffer[0] == char(0x5Au));
        REQUIRE(buffer[1] == char(0x30u));
        REQUIRE(buffer[2] == char(0x61u));

        std::fclose(p_file);
    }

    SECTION("file_input_adapter with invalid 1-byte UTF-8 encodings") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_1byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        REQUIRE_THROWS_AS(input_adapter.get_buffer_view(), fkyaml::invalid_encoding);

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter with valid 1-byte UTF-8 encodings") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_1byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 3);
        REQUIRE(buffer[0] == char(0x5Au));
        REQUIRE(buffer[1] == char(0x30u));
        REQUIRE(buffer[2] == char(0x61u));
    }

    SECTION("stream_input_adapter with invalid 1-byte UTF-8 encodings") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_1byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        REQUIRE_THROWS_AS(input_adapter.get_buffer_view(), fkyaml::invalid_encoding);
    }

    /////////////////////////////////
    //   UTF-8 2-Byte Characters   //
    /////////////////////////////////

    SECTION("iterator_input_adapter with valid 2-byte UTF-8 encodings") {
        char input[] = {char(0xC2u), char(0x80u), char(0xDFu), char(0xBFu), 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 4);
        REQUIRE(buffer[0] == char(0xC2u));
        REQUIRE(buffer[1] == char(0x80u));
        REQUIRE(buffer[2] == char(0xDFu));
        REQUIRE(buffer[3] == char(0xBFu));
    }

    SECTION("iterator_input_adapter with invalid 2-byte UTF-8 encodings") {
        char input[] = {char(0xC1u), char(0x80u), char(0xC2u), 0x7F, 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        REQUIRE_THROWS_AS(input_adapter.get_buffer_view(), fkyaml::invalid_encoding);
    }

    SECTION("file_input_adapter with valid 2-byte UTF-8 encodings") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_2byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 4);
        REQUIRE(buffer[0] == char(0xC2u));
        REQUIRE(buffer[1] == char(0x80u));
        REQUIRE(buffer[2] == char(0xDFu));
        REQUIRE(buffer[3] == char(0xBFu));

        std::fclose(p_file);
    }

    SECTION("file_input_adapter with invalid 2-byte UTF-8 encodings") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_2byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        REQUIRE_THROWS_AS(input_adapter.get_buffer_view(), fkyaml::invalid_encoding);

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter with valid 2-byte UTF-8 encodings") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_2byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 4);
        REQUIRE(buffer[0] == char(0xC2u));
        REQUIRE(buffer[1] == char(0x80u));
        REQUIRE(buffer[2] == char(0xDFu));
        REQUIRE(buffer[3] == char(0xBFu));
    }

    SECTION("stream_input_adapter with invalid 2-byte UTF-8 encodings") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_2byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        REQUIRE_THROWS_AS(input_adapter.get_buffer_view(), fkyaml::invalid_encoding);
    }

    /////////////////////////////////
    //   UTF-8 3-Byte Characters   //
    /////////////////////////////////

    SECTION("iterator_input_adapter with valid 3-byte UTF-8 encodings") {
        char input[] = {char(0xE0u), char(0x80u), char(0x80u), char(0xECu), char(0xBFu), char(0xBFu), 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 6);
        REQUIRE(buffer[0] == char(0xE0u));
        REQUIRE(buffer[1] == char(0x80u));
        REQUIRE(buffer[2] == char(0x80u));
        REQUIRE(buffer[3] == char(0xECu));
        REQUIRE(buffer[4] == char(0xBFu));
        REQUIRE(buffer[5] == char(0xBFu));
    }

    SECTION("iterator_input_adapter with invalid 3-byte UTF-8 encodings") {
        char input[] = {char(0xE0u), 0x6A, char(0x80u), char(0xEDu), char(0xA0u), char(0xC0u), 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        REQUIRE_THROWS_AS(input_adapter.get_buffer_view(), fkyaml::invalid_encoding);
    }

    SECTION("file_input_adapter with valid 3-byte UTF-8 encodings") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_3byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 6);
        REQUIRE(buffer[0] == char(0xE0u));
        REQUIRE(buffer[1] == char(0x80u));
        REQUIRE(buffer[2] == char(0x80u));
        REQUIRE(buffer[3] == char(0xECu));
        REQUIRE(buffer[4] == char(0xBFu));
        REQUIRE(buffer[5] == char(0xBFu));

        std::fclose(p_file);
    }

    SECTION("file_input_adapter with invalid 3-byte UTF-8 encodings") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_3byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        REQUIRE_THROWS_AS(input_adapter.get_buffer_view(), fkyaml::invalid_encoding);

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter with valid 3-byte UTF-8 encodings") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_3byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 6);
        REQUIRE(buffer[0] == char(0xE0u));
        REQUIRE(buffer[1] == char(0x80u));
        REQUIRE(buffer[2] == char(0x80u));
        REQUIRE(buffer[3] == char(0xECu));
        REQUIRE(buffer[4] == char(0xBFu));
        REQUIRE(buffer[5] == char(0xBFu));
    }

    SECTION("stream_input_adapter with invalid 3-byte UTF-8 encodings") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_3byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        REQUIRE_THROWS_AS(input_adapter.get_buffer_view(), fkyaml::invalid_encoding);
    }

    /////////////////////////////////
    //   UTF-8 4-Byte Characters   //
    /////////////////////////////////

    SECTION("iterator_input_adapter with valid 4-byte UTF-8 encodings") {
        char input[] = {
            char(0xF0u), char(0x90u), char(0x80u), char(0x80u), char(0xF2u), char(0xBFu), char(0x80u), char(0x80u), 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("iterator_input_adapter with invalid 4-byte UTF-8 encodings") {
        char input[] = {char(0xF0u), char(0x80u), 0x70, 0x70, char(0xF4u), char(0xC0u), char(0xC0u), 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        REQUIRE_THROWS_AS(input_adapter.get_buffer_view(), fkyaml::invalid_encoding);
    }

    SECTION("file_input_adapter with valid 4-byte UTF-8 encodings") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_4byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("file_input_adapter with invalid 4-byte UTF-8 encodings") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_4byte_char.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        REQUIRE_THROWS_AS(input_adapter.get_buffer_view(), fkyaml::invalid_encoding);

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter with valid 4-byte UTF-8 encodings") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_valid_4byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

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

    SECTION("stream_input_adapter with invalid 4-byte UTF-8 encodings") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8n_invalid_4byte_char.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        REQUIRE_THROWS_AS(input_adapter.get_buffer_view(), fkyaml::invalid_encoding);
    }
}

TEST_CASE("InputAdapter_GetBufferView_UTF8NewlineCodeNormalization") {
    SECTION("iterator_input_adapter (char)") {
        char input[] = "test\r\ndata\r\n";
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 10);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == '\n');
        REQUIRE(buffer[5] == 'd');
        REQUIRE(buffer[6] == 'a');
        REQUIRE(buffer[7] == 't');
        REQUIRE(buffer[8] == 'a');
        REQUIRE(buffer[9] == '\n');
    }

#if FK_YAML_HAS_CHAR8_T
    SECTION("iterator_input_adapter (char)") {
        char8_t input[] = u8"test\r\ndata\r\n";
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char8_t*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 10);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == '\n');
        REQUIRE(buffer[5] == 'd');
        REQUIRE(buffer[6] == 'a');
        REQUIRE(buffer[7] == 't');
        REQUIRE(buffer[8] == 'a');
        REQUIRE(buffer[9] == '\n');
    }
#endif

    SECTION("file_input_adapter") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8_crlf.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 10);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == '\n');
        REQUIRE(buffer[5] == 'd');
        REQUIRE(buffer[6] == 'a');
        REQUIRE(buffer[7] == 't');
        REQUIRE(buffer[8] == 'a');
        REQUIRE(buffer[9] == '\n');

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf8_crlf.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 10);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == '\n');
        REQUIRE(buffer[5] == 'd');
        REQUIRE(buffer[6] == 'a');
        REQUIRE(buffer[7] == 't');
        REQUIRE(buffer[8] == 'a');
        REQUIRE(buffer[9] == '\n');
    }
}

TEST_CASE("InputAdapter_GetBufferView_UTF16BENewlineCodeNormalization") {
    SECTION("iterator_input_adapter (char)") {
        char input[] = {0, char(0x74), 0, char(0x65), 0, char(0x73), 0, char(0x74), 0, char(0x0D),
                        0, char(0x0A), 0, char(0x64), 0, char(0x61), 0, char(0x74), 0, char(0x61),
                        0, char(0x0D), 0, char(0x0A), 0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 10);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == '\n');
        REQUIRE(buffer[5] == 'd');
        REQUIRE(buffer[6] == 'a');
        REQUIRE(buffer[7] == 't');
        REQUIRE(buffer[8] == 'a');
        REQUIRE(buffer[9] == '\n');
    }

    SECTION("iterator_input_adapter (char16_t)") {
        char16_t input[] = {
            0x0074u,
            0x0065u,
            0x0073u,
            0x0074u,
            0x000Du,
            0x000Au,
            0x0064u,
            0x0061u,
            0x0074u,
            0x0061u,
            0x000Du,
            0x000Au,
            0x0000};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char16_t*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 10);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == '\n');
        REQUIRE(buffer[5] == 'd');
        REQUIRE(buffer[6] == 'a');
        REQUIRE(buffer[7] == 't');
        REQUIRE(buffer[8] == 'a');
        REQUIRE(buffer[9] == '\n');
    }

    SECTION("file_input_adapter") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16be_crlf.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 10);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == '\n');
        REQUIRE(buffer[5] == 'd');
        REQUIRE(buffer[6] == 'a');
        REQUIRE(buffer[7] == 't');
        REQUIRE(buffer[8] == 'a');
        REQUIRE(buffer[9] == '\n');

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf16be_crlf.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 10);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == '\n');
        REQUIRE(buffer[5] == 'd');
        REQUIRE(buffer[6] == 'a');
        REQUIRE(buffer[7] == 't');
        REQUIRE(buffer[8] == 'a');
        REQUIRE(buffer[9] == '\n');
    }
}

TEST_CASE("InputAdapter_GetBufferView_UTF32BENewlineCodeNormalization") {
    SECTION("iterator_input_adapter (char)") {
        char input[] = {0, 0, 0, char(0x74), 0, 0, 0, char(0x65), 0, 0, 0, char(0x73), 0, 0, 0, char(0x74),
                        0, 0, 0, char(0x0D), 0, 0, 0, char(0x0A), 0, 0, 0, char(0x64), 0, 0, 0, char(0x61),
                        0, 0, 0, char(0x74), 0, 0, 0, char(0x61), 0, 0, 0, char(0x0D), 0, 0, 0, char(0x0A),
                        0};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 10);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == '\n');
        REQUIRE(buffer[5] == 'd');
        REQUIRE(buffer[6] == 'a');
        REQUIRE(buffer[7] == 't');
        REQUIRE(buffer[8] == 'a');
        REQUIRE(buffer[9] == '\n');
    }

    SECTION("iterator_input_adapter (char32_t)") {
        char32_t input[] = {
            0x00000074u,
            0x00000065u,
            0x00000073u,
            0x00000074u,
            0x0000000Du,
            0x0000000Au,
            0x00000064u,
            0x00000061u,
            0x00000074u,
            0x00000061u,
            0x0000000Du,
            0x0000000Au,
            0x00000000};
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char32_t*>>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 10);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == '\n');
        REQUIRE(buffer[5] == 'd');
        REQUIRE(buffer[6] == 'a');
        REQUIRE(buffer[7] == 't');
        REQUIRE(buffer[8] == 'a');
        REQUIRE(buffer[9] == '\n');
    }

    SECTION("file_input_adapter") {
        DISABLE_C4996
        FILE* p_file = std::fopen(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32be_crlf.txt", "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 10);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == '\n');
        REQUIRE(buffer[5] == 'd');
        REQUIRE(buffer[6] == 'a');
        REQUIRE(buffer[7] == 't');
        REQUIRE(buffer[8] == 'a');
        REQUIRE(buffer[9] == '\n');

        std::fclose(p_file);
    }

    SECTION("stream_input_adapter") {
        std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/input_adapter_test_data_utf32be_crlf.txt");
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        fkyaml::detail::str_view buffer = input_adapter.get_buffer_view();

        REQUIRE(buffer.size() == 10);
        REQUIRE(buffer[0] == 't');
        REQUIRE(buffer[1] == 'e');
        REQUIRE(buffer[2] == 's');
        REQUIRE(buffer[3] == 't');
        REQUIRE(buffer[4] == '\n');
        REQUIRE(buffer[5] == 'd');
        REQUIRE(buffer[6] == 'a');
        REQUIRE(buffer[7] == 't');
        REQUIRE(buffer[8] == 'a');
        REQUIRE(buffer[9] == '\n');
    }
}
