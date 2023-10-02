//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <cstring>

#include "catch2/catch.hpp"

#include "fkYAML/Exception.hpp"

TEST_CASE("ExceptionClassTest_DefaultCtorTest", "[ExceptionClassTest]")
{
    fkyaml::Exception exception;
    REQUIRE(std::string(exception.what()).empty());
}

TEST_CASE("ExceptionClassTest_CtorWithMessageTest", "[ExceptionClassTest]")
{
    SECTION("Test non-null message.")
    {
        const char* message = "test error message.";
        fkyaml::Exception exception(message);
        REQUIRE(std::string(exception.what()).compare(message) == 0);
    }

    SECTION("Test null message.")
    {
        const char* message = nullptr;
        fkyaml::Exception exception(message);
        REQUIRE(std::string(exception.what()).empty());
    }
}
