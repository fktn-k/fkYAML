//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <cstring>

#include <catch2/catch.hpp>

#ifdef FK_YAML_TEST_USE_SINGLE_HEADER
    #include <fkYAML/node.hpp>
#else
    #include <fkYAML/exception.hpp>
#endif

TEST_CASE("ExceptionClassTest_DefaultCtorTest", "[ExceptionClassTest]")
{
    fkyaml::exception exception;
    REQUIRE(std::string(exception.what()).empty());
}

TEST_CASE("ExceptionClassTest_CtorWithMessageTest", "[ExceptionClassTest]")
{
    SECTION("Test non-null message.")
    {
        const char* message = "test error message.";
        fkyaml::exception exception(message);
        REQUIRE(std::string(exception.what()).compare(message) == 0);
    }

    SECTION("Test null message.")
    {
        const char* message = nullptr;
        fkyaml::exception exception(message);
        REQUIRE(std::string(exception.what()).empty());
    }
}
