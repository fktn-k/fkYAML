/**
 *   __ _  __     __      __  __ _
 *  / _| | \ \   / //\   |  \/  | |
 * | |_| | _\ \_/ //  \  | \  / | |
 * |  _| |/ /\   // /\ \ | |\/| | |
 * | | |   <  | |/ ____ \| |  | | |____
 * |_| |_|\_\ |_/_/    \_\_|  |_|______|
 *
 * @file ExceptionClassTest.cpp
 * @brief Implementation of test functions for the Exception class.
 * @version 0.0.0
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

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
