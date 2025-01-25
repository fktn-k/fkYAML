//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <string>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("PositionTracker_InitialState") {
    fkyaml::detail::str_view input = "test";
    fkyaml::detail::position_tracker pos_tracker {};
    pos_tracker.set_target_buffer(input);

    REQUIRE(pos_tracker.get_cur_pos() == 0);
    REQUIRE(pos_tracker.get_cur_pos_in_line() == 0);
    REQUIRE(pos_tracker.get_lines_read() == 0);
}

TEST_CASE("PositionTracker_MultipleLines") {
    fkyaml::detail::position_tracker pos_tracker {};

    SECTION("first character is not a newline code") {
        fkyaml::detail::str_view input = "test\nfoo";
        pos_tracker.set_target_buffer(input);

        REQUIRE(pos_tracker.get_cur_pos() == 0);
        REQUIRE(pos_tracker.get_cur_pos_in_line() == 0);
        REQUIRE(pos_tracker.get_lines_read() == 0);

        pos_tracker.update_position(input.begin() + 1);
        REQUIRE(pos_tracker.get_cur_pos() == 1);
        REQUIRE(pos_tracker.get_cur_pos_in_line() == 1);
        REQUIRE(pos_tracker.get_lines_read() == 0);

        pos_tracker.update_position(input.begin() + 4);
        REQUIRE(pos_tracker.get_cur_pos() == 4);
        REQUIRE(pos_tracker.get_cur_pos_in_line() == 4);
        REQUIRE(pos_tracker.get_lines_read() == 0);

        pos_tracker.update_position(input.begin() + 5);
        REQUIRE(pos_tracker.get_cur_pos() == 5);
        REQUIRE(pos_tracker.get_cur_pos_in_line() == 0);
        REQUIRE(pos_tracker.get_lines_read() == 1);

        pos_tracker.update_position(input.begin() + 7);
        REQUIRE(pos_tracker.get_cur_pos() == 7);
        REQUIRE(pos_tracker.get_cur_pos_in_line() == 2);
        REQUIRE(pos_tracker.get_lines_read() == 1);

        pos_tracker.update_position(input.end());
        REQUIRE(pos_tracker.get_cur_pos() == 8);
        REQUIRE(pos_tracker.get_cur_pos_in_line() == 3);
        REQUIRE(pos_tracker.get_lines_read() == 1);
    }

    SECTION("first character is a newline code") {
        fkyaml::detail::str_view input = "\ntest\nfoo";
        pos_tracker.set_target_buffer(input);

        REQUIRE(pos_tracker.get_cur_pos() == 0);
        REQUIRE(pos_tracker.get_cur_pos_in_line() == 0);
        REQUIRE(pos_tracker.get_lines_read() == 0);

        pos_tracker.update_position(input.begin() + 1);
        REQUIRE(pos_tracker.get_cur_pos() == 1);
        REQUIRE(pos_tracker.get_cur_pos_in_line() == 0);
        REQUIRE(pos_tracker.get_lines_read() == 1);

        pos_tracker.update_position(input.begin() + 4);
        REQUIRE(pos_tracker.get_cur_pos() == 4);
        REQUIRE(pos_tracker.get_cur_pos_in_line() == 3);
        REQUIRE(pos_tracker.get_lines_read() == 1);

        pos_tracker.update_position(input.begin() + 5);
        REQUIRE(pos_tracker.get_cur_pos() == 5);
        REQUIRE(pos_tracker.get_cur_pos_in_line() == 4);
        REQUIRE(pos_tracker.get_lines_read() == 1);

        pos_tracker.update_position(input.begin() + 6);
        REQUIRE(pos_tracker.get_cur_pos() == 6);
        REQUIRE(pos_tracker.get_cur_pos_in_line() == 0);
        REQUIRE(pos_tracker.get_lines_read() == 2);

        pos_tracker.update_position(input.begin() + 8);
        REQUIRE(pos_tracker.get_cur_pos() == 8);
        REQUIRE(pos_tracker.get_cur_pos_in_line() == 2);
        REQUIRE(pos_tracker.get_lines_read() == 2);

        pos_tracker.update_position(input.end());
        REQUIRE(pos_tracker.get_cur_pos() == 9);
        REQUIRE(pos_tracker.get_cur_pos_in_line() == 3);
        REQUIRE(pos_tracker.get_lines_read() == 2);
    }
}
