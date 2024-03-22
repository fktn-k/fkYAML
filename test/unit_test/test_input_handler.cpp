//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <string>

#include <catch2/catch.hpp>

#ifdef FK_YAML_TEST_USE_SINGLE_HEADER
    #include <fkYAML/node.hpp>
#else
    #include <fkYAML/detail/input/input_adapter.hpp>
    #include <fkYAML/detail/input/input_handler.hpp>
#endif

TEST_CASE("InputHandlerTest_InitialStateTest", "[InputHandlerTest]")
{
    char input[] = "test";
    fkyaml::detail::input_handler handler(fkyaml::detail::input_adapter(input));

    REQUIRE(handler.get_current() == 't');
    REQUIRE(handler.get_cur_pos_in_line() == 0);
    REQUIRE(handler.get_lines_read() == 0);
}

TEST_CASE("InputHandlerTest_GetCurrentTest", "[InputHandlerTest]")
{
    char input[] = "test";
    fkyaml::detail::input_handler handler(fkyaml::detail::input_adapter(input));

    REQUIRE(handler.get_current() == 't');
    REQUIRE(handler.get_cur_pos_in_line() == 0);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == 'e');
    REQUIRE(handler.get_current() == 'e');
    REQUIRE(handler.get_cur_pos_in_line() == 1);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == 's');
    REQUIRE(handler.get_current() == 's');
    REQUIRE(handler.get_cur_pos_in_line() == 2);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == 't');
    REQUIRE(handler.get_current() == 't');
    REQUIRE(handler.get_cur_pos_in_line() == 3);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == std::char_traits<char>::eof());
    REQUIRE(handler.get_current() == std::char_traits<char>::eof());
    REQUIRE(handler.get_cur_pos_in_line() == 4);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == std::char_traits<char>::eof());
    REQUIRE(handler.get_current() == std::char_traits<char>::eof());
    REQUIRE(handler.get_cur_pos_in_line() == 4);
    REQUIRE(handler.get_lines_read() == 0);
}

TEST_CASE("InputHandlerTest_GetNextTest", "[InputHandlerTest]")
{
    char input[] = "test";
    fkyaml::detail::input_handler handler(fkyaml::detail::input_adapter(input));

    REQUIRE(handler.get_next() == 'e');
    REQUIRE(handler.get_cur_pos_in_line() == 1);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == 's');
    REQUIRE(handler.get_cur_pos_in_line() == 2);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == 't');
    REQUIRE(handler.get_cur_pos_in_line() == 3);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == std::char_traits<char>::eof());
    REQUIRE(handler.get_cur_pos_in_line() == 4);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == std::char_traits<char>::eof());
    REQUIRE(handler.get_cur_pos_in_line() == 4);
    REQUIRE(handler.get_lines_read() == 0);
}

TEST_CASE("InputHandlerTest_GetRangeTest", "[InputHandlerTest]")
{
    char input[] = "test";
    std::string str;
    fkyaml::detail::input_handler handler(fkyaml::detail::input_adapter(input));

    REQUIRE(handler.get_range(4, str) == 0);
    REQUIRE(str == "test");
    REQUIRE(handler.get_current() == 't');
    REQUIRE(handler.get_cur_pos_in_line() == 3);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_range(2, str) == std::char_traits<char>::eof());
    REQUIRE(handler.get_current() == 't');
    REQUIRE(handler.get_cur_pos_in_line() == 3);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == std::char_traits<char>::eof());
    REQUIRE(handler.get_cur_pos_in_line() == 4);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_range(0, str) == 0);
    REQUIRE(handler.get_current() == std::char_traits<char>::eof());
    REQUIRE(handler.get_cur_pos_in_line() == 4);
    REQUIRE(handler.get_lines_read() == 0);
}

TEST_CASE("InputHandlerTest_UngetTest", "[InputHandlerTest]")
{
    char input[] = "test";
    fkyaml::detail::input_handler handler(fkyaml::detail::input_adapter(input));

    REQUIRE(handler.get_current() == 't');
    REQUIRE(handler.get_cur_pos_in_line() == 0);
    REQUIRE(handler.get_lines_read() == 0);

    handler.unget();
    REQUIRE(handler.get_current() == 't');
    REQUIRE(handler.get_cur_pos_in_line() == 0);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == 'e');
    handler.unget();
    REQUIRE(handler.get_current() == 't');
    REQUIRE(handler.get_cur_pos_in_line() == 0);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == 'e');
    REQUIRE(handler.get_next() == 's');
    REQUIRE(handler.get_next() == 't');
    REQUIRE(handler.get_next() == std::char_traits<char>::eof());
    handler.unget();
    REQUIRE(handler.get_current() == 't');
    REQUIRE(handler.get_cur_pos_in_line() == 3);
    REQUIRE(handler.get_lines_read() == 0);
}

TEST_CASE("InputHandlerTest_UngetRangeTest", "[InputHandlerTest]")
{
    char input[] = "test";
    fkyaml::detail::input_handler handler(fkyaml::detail::input_adapter(input));

    REQUIRE(handler.get_current() == 't');
    handler.unget_range(4);
    REQUIRE(handler.get_current() == 't');
    REQUIRE(handler.get_cur_pos_in_line() == 0);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == 'e');
    REQUIRE(handler.get_next() == 's');
    handler.unget_range(1);
    REQUIRE(handler.get_current() == 'e');
    REQUIRE(handler.get_cur_pos_in_line() == 1);
    REQUIRE(handler.get_lines_read() == 0);

    handler.unget_range(0);
    REQUIRE(handler.get_current() == 'e');
    REQUIRE(handler.get_cur_pos_in_line() == 1);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == 's');
    REQUIRE(handler.get_next() == 't');
    REQUIRE(handler.get_next() == std::char_traits<char>::eof());
    handler.unget_range(2);
    REQUIRE(handler.get_current() == 's');
    REQUIRE(handler.get_cur_pos_in_line() == 2);
    REQUIRE(handler.get_lines_read() == 0);
}

TEST_CASE("InputHandlerTest_TestNextCharTest", "[InputHandlerTest]")
{
    char input[] = "test";
    fkyaml::detail::input_handler handler(fkyaml::detail::input_adapter(input));

    REQUIRE(handler.test_next_char('e') == true);
    REQUIRE(handler.get_cur_pos_in_line() == 0);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == 'e');
    REQUIRE(handler.test_next_char('s') == true);
    REQUIRE(handler.test_next_char('t') == false);
    REQUIRE(handler.get_cur_pos_in_line() == 1);
    REQUIRE(handler.get_lines_read() == 0);

    REQUIRE(handler.get_next() == 's');
    REQUIRE(handler.get_next() == 't');
    REQUIRE(handler.test_next_char('t') == false);

    REQUIRE(handler.get_next() == std::char_traits<char>::eof());
    REQUIRE(handler.test_next_char('t') == false);
    REQUIRE(handler.get_cur_pos_in_line() == 4);
    REQUIRE(handler.get_lines_read() == 0);

    char char_eof = std::char_traits<char>::to_char_type(std::char_traits<char>::eof());
    REQUIRE(handler.test_next_char(char_eof) == false);
    REQUIRE(handler.get_current() == std::char_traits<char>::eof());
}

TEST_CASE("InputHandlerTest_TestMultipleLinesTest", "[InputHandlerTest]")
{
    SECTION("first character is not a newline code.")
    {
        char input[] = "test\nfoo";
        std::string str;
        fkyaml::detail::input_handler handler(fkyaml::detail::input_adapter(input));

        REQUIRE(handler.get_range(4, str) == 0);
        REQUIRE(handler.get_cur_pos_in_line() == 3);
        REQUIRE(handler.get_lines_read() == 0);

        REQUIRE(handler.get_next() == '\n');
        REQUIRE(handler.get_cur_pos_in_line() == 4);
        REQUIRE(handler.get_lines_read() == 0);

        REQUIRE(handler.get_next() == 'f');
        REQUIRE(handler.get_cur_pos_in_line() == 0);
        REQUIRE(handler.get_lines_read() == 1);

        handler.unget();
        REQUIRE(handler.get_cur_pos_in_line() == 4);
        REQUIRE(handler.get_lines_read() == 0);
    }

    SECTION("first character is a newline code.")
    {
        char input[] = "\ntest\nfoo";
        std::string str;
        fkyaml::detail::input_handler handler(fkyaml::detail::input_adapter(input));

        REQUIRE(handler.get_next() == 't');
        REQUIRE(handler.get_cur_pos_in_line() == 0);
        REQUIRE(handler.get_lines_read() == 1);

        handler.unget();
        REQUIRE(handler.get_cur_pos_in_line() == 0);
        REQUIRE(handler.get_lines_read() == 0);

        REQUIRE(handler.get_range(5, str) == 0);
        REQUIRE(handler.get_cur_pos_in_line() == 3);
        REQUIRE(handler.get_lines_read() == 1);

        REQUIRE(handler.get_next() == '\n');
        REQUIRE(handler.get_cur_pos_in_line() == 4);
        REQUIRE(handler.get_lines_read() == 1);

        REQUIRE(handler.get_next() == 'f');
        REQUIRE(handler.get_cur_pos_in_line() == 0);
        REQUIRE(handler.get_lines_read() == 2);

        handler.unget();
        REQUIRE(handler.get_cur_pos_in_line() == 4);
        REQUIRE(handler.get_lines_read() == 1);
    }
}
