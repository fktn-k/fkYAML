//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.2.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <string>
#include <vector>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

namespace test
{

struct novel
{
    std::string title;
    std::string author;
    int year;
};

void from_node(const fkyaml::node& node, novel& novel)
{
    novel.title = node["title"].get_value_ref<const std::string&>();
    novel.author = node["author"].get_value_ref<const std::string&>();
    novel.year = node["year"].get_value<int>();
}

} // namespace test

TEST_CASE("FromNodeTest_UserDefinedTypeTest", "[FromNodeTest]")
{
    std::string input = "title: Robinson Crusoe\n"
                        "author: Daniel Defoe\n"
                        "year: 1678";
    fkyaml::node node = fkyaml::node::deserialize(input);

    auto novel = node.get_value<test::novel>();
    REQUIRE(novel.title == "Robinson Crusoe");
    REQUIRE(novel.author == "Daniel Defoe");
    REQUIRE(novel.year == 1678);
}

TEST_CASE("FromNodeTest_UserDefinedTypeVectorTest", "[FromNodeTest]")
{
    std::string input = "novels:\n"
                        "  - title: Robinson Crusoe\n"
                        "    author: Daniel Defoe\n"
                        "    year: 1678\n"
                        "  - title: Frankenstein\n"
                        "    author: Mary Shelly\n"
                        "    year: 1818\n";
    fkyaml::node node = fkyaml::node::deserialize(input);

    auto novels = node["novels"].get_value<std::vector<test::novel>>();
    REQUIRE(novels[0].title == "Robinson Crusoe");
    REQUIRE(novels[0].author == "Daniel Defoe");
    REQUIRE(novels[0].year == 1678);
    REQUIRE(novels[1].title == "Frankenstein");
    REQUIRE(novels[1].author == "Mary Shelly");
    REQUIRE(novels[1].year == 1818);
}

TEST_CASE("FromNodeTest_UserDefinedTypeVectorErrorTest", "[FromNodeTest]")
{
    std::string input = "novels:\n"
                        "  - title: Robinson Crusoe\n"
                        "    author: Daniel Defoe\n"
                        "    year: 1678\n"
                        "  - title: Frankenstein\n"
                        "    author: Mary Shelly\n"
                        "    year: 1818\n";
    fkyaml::node node = fkyaml::node::deserialize(input);
    REQUIRE_THROWS_AS(node.get_value<std::vector<test::novel>>(), fkyaml::exception);
}