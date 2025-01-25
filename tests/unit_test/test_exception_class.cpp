//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <cstring>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("Exception_DefaultCtor") {
    fkyaml::exception exception;
    REQUIRE(std::string(exception.what()).empty());
}

TEST_CASE("Exception_CtorWithMessage") {
    SECTION("non-null message.") {
        const char* message = "test error message.";
        fkyaml::exception exception(message);
        REQUIRE(std::string(exception.what()).compare(message) == 0);
    }

    SECTION("null message.") {
        const char* message = nullptr;
        fkyaml::exception exception(message);
        REQUIRE(std::string(exception.what()).empty());
    }
}
