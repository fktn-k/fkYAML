//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.1.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <fstream>
#include <string>

#include <catch2/catch.hpp>

#include <fkYAML/detail/input/input_adapter.hpp>

// generated in test/unit_test/CMakeLists.txt
#include <test_data.hpp>

#ifdef _MSC_VER
    #define DISABLE_C4996 __pragma(warning(push)) __pragma(warning(disable : 4996))
    #define ENABLE_C4996 __pragma(warning(pop))
#else
    #define DISABLE_C4996
    #define ENABLE_C4996
#endif

static constexpr char input_file_path[] = FK_YAML_TEST_DATA_DIR "/input_adapter_test_data.txt";

TEST_CASE("InputAdapterTest_IteratorInputAdapterProviderTest", "[InputAdapterTest]")
{
    char input[] = "test";

    SECTION("c-style char array")
    {
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);
    }

    SECTION("char pointers for beginning/end")
    {
        auto input_adapter = fkyaml::detail::input_adapter(&input[0], &input[sizeof(input) - 1]);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);
    }

    SECTION("std::string")
    {
        std::string input_str(input);
        auto input_adapter = fkyaml::detail::input_adapter(input_str);
        using iterator_type = typename std::string::iterator;
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<iterator_type>>::value);
    }
}

TEST_CASE("InputAdapterTest_FileInputAdapterProviderTest", "[InputAdapterTest]")
{
    SECTION("invalid FILE object pointer")
    {
        FILE* p_file = nullptr;
        REQUIRE_THROWS_AS(fkyaml::detail::input_adapter(p_file), fkyaml::exception);
    }

    SECTION("valie FILE object pointer")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(input_file_path, "r");
        ENABLE_C4996

        REQUIRE(p_file != nullptr);
        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);
    }
}

TEST_CASE("InputAdapterTest_StreamInputAdapterProviderTest", "[InputAdapterTest]")
{
    std::ifstream ifs(input_file_path);
    REQUIRE(ifs);
    auto input_adapter = fkyaml::detail::input_adapter(ifs);
    REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);
}

TEST_CASE("InputAdapterTest_GetCharacterTest", "[InputAdapterTest]")
{
    SECTION("iterator_input_adapter")
    {
        char input[] = "test source.";
        auto input_adapter = fkyaml::detail::input_adapter(input);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::iterator_input_adapter<char*>>::value);

        using char_traits_type = std::char_traits<typename decltype(input_adapter)::char_type>;

        REQUIRE(input_adapter.get_character() == 't');
        REQUIRE(input_adapter.get_character() == 'e');
        REQUIRE(input_adapter.get_character() == 's');
        REQUIRE(input_adapter.get_character() == 't');
        REQUIRE(input_adapter.get_character() == ' ');
        REQUIRE(input_adapter.get_character() == 's');
        REQUIRE(input_adapter.get_character() == 'o');
        REQUIRE(input_adapter.get_character() == 'u');
        REQUIRE(input_adapter.get_character() == 'r');
        REQUIRE(input_adapter.get_character() == 'c');
        REQUIRE(input_adapter.get_character() == 'e');
        REQUIRE(input_adapter.get_character() == '.');
        REQUIRE(input_adapter.get_character() == char_traits_type::eof());
    }

    SECTION("file_input_adapter")
    {
        DISABLE_C4996
        FILE* p_file = std::fopen(input_file_path, "r");
        ENABLE_C4996

        auto input_adapter = fkyaml::detail::input_adapter(p_file);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::file_input_adapter>::value);

        using char_traits_type = std::char_traits<typename decltype(input_adapter)::char_type>;

        REQUIRE(input_adapter.get_character() == 't');
        REQUIRE(input_adapter.get_character() == 'e');
        REQUIRE(input_adapter.get_character() == 's');
        REQUIRE(input_adapter.get_character() == 't');
        REQUIRE(input_adapter.get_character() == ' ');
        REQUIRE(input_adapter.get_character() == 's');
        REQUIRE(input_adapter.get_character() == 'o');
        REQUIRE(input_adapter.get_character() == 'u');
        REQUIRE(input_adapter.get_character() == 'r');
        REQUIRE(input_adapter.get_character() == 'c');
        REQUIRE(input_adapter.get_character() == 'e');
        REQUIRE(input_adapter.get_character() == '.');
        REQUIRE(input_adapter.get_character() == char_traits_type::eof());
    }

    SECTION("stream_input_adapter")
    {
        std::ifstream ifs(input_file_path);
        auto input_adapter = fkyaml::detail::input_adapter(ifs);
        REQUIRE(std::is_same<decltype(input_adapter), fkyaml::detail::stream_input_adapter>::value);

        using char_traits_type = std::char_traits<typename decltype(input_adapter)::char_type>;

        REQUIRE(input_adapter.get_character() == 't');
        REQUIRE(input_adapter.get_character() == 'e');
        REQUIRE(input_adapter.get_character() == 's');
        REQUIRE(input_adapter.get_character() == 't');
        REQUIRE(input_adapter.get_character() == ' ');
        REQUIRE(input_adapter.get_character() == 's');
        REQUIRE(input_adapter.get_character() == 'o');
        REQUIRE(input_adapter.get_character() == 'u');
        REQUIRE(input_adapter.get_character() == 'r');
        REQUIRE(input_adapter.get_character() == 'c');
        REQUIRE(input_adapter.get_character() == 'e');
        REQUIRE(input_adapter.get_character() == '.');
        REQUIRE(input_adapter.get_character() == char_traits_type::eof());
    }
}