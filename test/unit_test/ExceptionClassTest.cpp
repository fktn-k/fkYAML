/**
 * ExceptionClassTest.cpp - implementation of test functions for the Exception class
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
    const char* message = "test error message.";
    fkyaml::Exception exception(message);
    REQUIRE(std::string(exception.what()).compare(message) == 0);
}
