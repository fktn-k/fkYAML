//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.1.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <string>

#include <catch2/catch.hpp>

#include <fkYAML/detail/input/input_adapter.hpp>
#include <fkYAML/detail/input/input_handler.hpp>

using pchar_input_handler = fkyaml::detail::input_handler<fkyaml::detail::iterator_input_adapter<char*>>;

TEST_CASE("InputHandlerTest_InitialStateTest", "[InputHandlerTest]")
{
    char input[] = "test";
    pchar_input_handler handler(fkyaml::detail::input_adapter(input));

    REQUIRE(handler.get_current() == 't');
}

TEST_CASE("InputHandlerTest_GetCurrentTest", "[InputHandlerTest]")
{
    char input[] = "test";
    pchar_input_handler handler(fkyaml::detail::input_adapter(input));

    REQUIRE(handler.get_current() == 't');
    REQUIRE(handler.get_next() == 'e');
    REQUIRE(handler.get_current() == 'e');
    REQUIRE(handler.get_next() == 's');
    REQUIRE(handler.get_current() == 's');
    REQUIRE(handler.get_next() == 't');
    REQUIRE(handler.get_current() == 't');
    REQUIRE(handler.get_next() == pchar_input_handler::char_traits_type::eof());
    REQUIRE(handler.get_current() == pchar_input_handler::char_traits_type::eof());
    REQUIRE(handler.get_next() == pchar_input_handler::char_traits_type::eof());
    REQUIRE(handler.get_current() == pchar_input_handler::char_traits_type::eof());
}

TEST_CASE("InputHandlerTest_GetNextTest", "[InputHandlerTest]")
{
    char input[] = "test";
    pchar_input_handler handler(fkyaml::detail::input_adapter(input));

    REQUIRE(handler.get_next() == 'e');
    REQUIRE(handler.get_next() == 's');
    REQUIRE(handler.get_next() == 't');
    REQUIRE(handler.get_next() == pchar_input_handler::char_traits_type::eof());
    REQUIRE(handler.get_next() == pchar_input_handler::char_traits_type::eof());
}

TEST_CASE("InputHandlerTest_GetRangeTest", "[InputHandlerTest]")
{
    char input[] = "test";
    pchar_input_handler::string_type str;
    pchar_input_handler handler(fkyaml::detail::input_adapter(input));

    REQUIRE(handler.get_range(4, str) == 0);
    REQUIRE(str == "test");
    REQUIRE(handler.get_current() == 't');

    REQUIRE(handler.get_range(2, str) == pchar_input_handler::char_traits_type::eof());
    REQUIRE(handler.get_current() == 't');

    REQUIRE(handler.get_next() == pchar_input_handler::char_traits_type::eof());
    REQUIRE(handler.get_range(0, str) == pchar_input_handler::char_traits_type::eof());
    REQUIRE(handler.get_current() == pchar_input_handler::char_traits_type::eof());
}

TEST_CASE("InputHandlerTest_UngetTest", "[InputHandlerTest]")
{
    char input[] = "test";
    pchar_input_handler handler(fkyaml::detail::input_adapter(input));

    REQUIRE(handler.get_current() == 't');
    handler.unget();
    REQUIRE(handler.get_current() == 't');

    REQUIRE(handler.get_next() == 'e');
    handler.unget();
    REQUIRE(handler.get_current() == 't');

    REQUIRE(handler.get_next() == 'e');
    REQUIRE(handler.get_next() == 's');
    REQUIRE(handler.get_next() == 't');
    REQUIRE(handler.get_next() == pchar_input_handler::char_traits_type::eof());
    handler.unget();
    REQUIRE(handler.get_current() == 't');
}

TEST_CASE("InputHandlerTest_UngetRangeTest", "[InputHandlerTest]")
{
    char input[] = "test";
    pchar_input_handler handler(fkyaml::detail::input_adapter(input));

    REQUIRE(handler.get_current() == 't');
    handler.unget_range(4);
    REQUIRE(handler.get_current() == 't');

    REQUIRE(handler.get_next() == 'e');
    REQUIRE(handler.get_next() == 's');
    handler.unget_range(1);
    REQUIRE(handler.get_current() == 'e');
    handler.unget_range(0);
    REQUIRE(handler.get_current() == 'e');

    REQUIRE(handler.get_next() == 's');
    REQUIRE(handler.get_next() == 't');
    REQUIRE(handler.get_next() == pchar_input_handler::char_traits_type::eof());
    handler.unget_range(2);
    REQUIRE(handler.get_current() == 's');
}

TEST_CASE("InputHandlerTest_TestNextCharTest", "[InputHandlerTest]")
{
    char input[] = "test";
    pchar_input_handler handler(fkyaml::detail::input_adapter(input));

    REQUIRE(handler.test_next_char('e') == true);

    REQUIRE(handler.get_next() == 'e');
    REQUIRE(handler.test_next_char('s') == true);
    REQUIRE(handler.test_next_char('t') == false);

    REQUIRE(handler.get_next() == 's');
    REQUIRE(handler.get_next() == 't');
    REQUIRE(handler.get_next() == pchar_input_handler::char_traits_type::eof());
    REQUIRE(handler.test_next_char('t') == false);

    pchar_input_handler::char_type char_eof =
        pchar_input_handler::char_traits_type::to_char_type(pchar_input_handler::char_traits_type::eof());
    REQUIRE(handler.test_next_char(char_eof) == false);
    REQUIRE(handler.get_current() == pchar_input_handler::char_traits_type::eof());
}
