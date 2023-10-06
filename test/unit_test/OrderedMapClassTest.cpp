//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.1
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include "catch2/catch.hpp"

#include <string>

#include "fkYAML/OrderedMap.hpp"

TEST_CASE("OrderedMapClassTest_DefaultCtorTest", "[OrderedMapClassTest]")
{
    fkyaml::OrderedMap<std::string, bool> map;
    REQUIRE(map.empty());
}

TEST_CASE("OrderedMapClassTest_InitListCtorTest", "[OrderedMapClassTest]")
{
    fkyaml::OrderedMap<std::string, bool> map {{"foo", true}, {"bar", false}};
    REQUIRE_FALSE(map.empty());
    REQUIRE(map.size() == 2);
    REQUIRE(map["foo"] == true);
    REQUIRE(map["bar"] == false);
    REQUIRE_NOTHROW(map.at("foo"));
    REQUIRE_NOTHROW(map.at("bar"));
    REQUIRE_THROWS_AS(map.at("buz"), fkyaml::Exception);
    auto itr = map.begin();
    REQUIRE(itr->first == "foo");
    REQUIRE(itr->second == true);
    ++itr;
    REQUIRE(itr->first == "bar");
    REQUIRE(itr->second == false);
}

TEST_CASE("OrderedMapClassTest_SubscriptOperatorTest", "[OrderedMapClassTest]")
{
    fkyaml::OrderedMap<std::string, bool> map {{"foo", true}, {"buz", false}};
    REQUIRE(map["foo"] == true);
    REQUIRE(map["bar"] == false);
    REQUIRE(map["buz"] == false);
    map["buz"] = true;
    REQUIRE(map["buz"] == true);
}

TEST_CASE("OrderedMapClassTest_EmplaceTest", "[OrderedMapClassTest]")
{
    fkyaml::OrderedMap<std::string, bool> map;
    REQUIRE(map.emplace("foo", true).second == true);
    REQUIRE(map.emplace("foo", false).second == false);
    REQUIRE(map["foo"] == true);
    REQUIRE(map.emplace("bar", false).second == true);
    REQUIRE(map["bar"] == false);
}

TEST_CASE("OrderedMapClassTest_NonConstAtTest", "[OrderedMapClassTest]")
{
    fkyaml::OrderedMap<std::string, bool> map;
    REQUIRE_THROWS_AS(map.at("foo"), fkyaml::Exception);
    map.emplace("foo", true);
    REQUIRE_NOTHROW(map.at("foo"));
    REQUIRE(map.at("foo") == true);
    REQUIRE_THROWS_AS(map.at("bar"), fkyaml::Exception);
}

TEST_CASE("OrderedMapClassTest_ConstAtTest", "[OrderedMapClassTest]")
{
    const fkyaml::OrderedMap<std::string, bool> map;
    REQUIRE_THROWS_AS(map.at("foo"), fkyaml::Exception);
    fkyaml::OrderedMap<std::string, bool> map_ = map;
    map_.emplace("buz", false);
    map_.emplace("foo", true);
    const fkyaml::OrderedMap<std::string, bool> map__ = map_;
    REQUIRE_NOTHROW(map__.at("foo"));
    REQUIRE(map__.at("foo") == true);
    REQUIRE_THROWS_AS(map__.at("bar"), fkyaml::Exception);
}

TEST_CASE("OrderedMapClassTest_NonConstFindTest", "[OrderedMapClassTest]")
{
    fkyaml::OrderedMap<std::string, bool> map;
    REQUIRE(map.find("foo") == map.end());
    map.emplace("foo", true);
    REQUIRE(map.find("foo") != map.end());
    REQUIRE(map.find("foo")->first == "foo");
    REQUIRE(map.find("foo")->second == true);
    REQUIRE(map.find("bar") == map.end());
}

TEST_CASE("OrderedMapClassTest_ConstFindTest", "[OrderedMapClassTest]")
{
    const fkyaml::OrderedMap<std::string, bool> map;
    REQUIRE(map.find("foo") == map.end());
    fkyaml::OrderedMap<std::string, bool> map_ = map;
    map_.emplace("foo", true);
    const fkyaml::OrderedMap<std::string, bool> map__ = map_;
    REQUIRE(map__.find("foo") != map__.end());
    REQUIRE(map__.find("foo")->first == "foo");
    REQUIRE(map__.find("foo")->second == true);
    REQUIRE(map__.find("bar") == map__.end());
}
