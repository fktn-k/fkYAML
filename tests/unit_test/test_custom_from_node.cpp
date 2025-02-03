//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

namespace test {

struct novel {
    std::string title;
    std::string author;
    int year;
};

void from_node(const fkyaml::node& node, novel& novel) {
    novel.title = node["title"].as_str();
    novel.author = node["author"].as_str();
    novel.year = node["year"].get_value<int>();
}
struct color {
    int value;
};

bool operator<(const color& lhs, const color& rhs) noexcept {
    return lhs.value < rhs.value;
}

bool operator==(const color& lhs, const color& rhs) noexcept {
    return lhs.value == rhs.value;
}

void from_node(const fkyaml::node& node, color& color) {
    color.value = node["color"].get_value<int>();
}

struct rgb {
    int r;
    int g;
    int b;
};

bool operator==(const rgb& lhs, const rgb& rhs) noexcept {
    return (lhs.r == rhs.r) && (lhs.g == rhs.g) && (lhs.b == rhs.b);
}

void from_node(const fkyaml::node& node, rgb& rgb) {
    rgb.r = node["r"].get_value<int>();
    rgb.g = node["g"].get_value<int>();
    rgb.b = node["b"].get_value<int>();
}

} // namespace test

// std::hash specialization for std::unordered_map
namespace std {

template <>
struct hash<test::color> {
    std::size_t operator()(const test::color& c) const noexcept {
        return std::hash<int>()(c.value);
    }
};

template <>
struct hash<test::rgb> {
    std::size_t operator()(const test::rgb& rgb) const noexcept {
        return hash<int>()(rgb.r) ^ (hash<int>()(rgb.g) << 1) ^ (hash<int>()(rgb.b) << 2);
    }
};

} // namespace std

TEST_CASE("FromNode_UserDefinedType") {
    std::string input = "title: Robinson Crusoe\n"
                        "author: Daniel Defoe\n"
                        "year: 1678";
    fkyaml::node node = fkyaml::node::deserialize(input);

    auto novel = node.get_value<test::novel>();
    REQUIRE(novel.title == "Robinson Crusoe");
    REQUIRE(novel.author == "Daniel Defoe");
    REQUIRE(novel.year == 1678);
}

TEST_CASE("FromNode_UserDefinedTypeVector") {
    std::string input = "novels:\n"
                        "  - title: Robinson Crusoe\n"
                        "    author: Daniel Defoe\n"
                        "    year: 1678\n"
                        "  - title: Frankenstein\n"
                        "    author: Mary Shelly\n"
                        "    year: 1818\n";
    fkyaml::node node = fkyaml::node::deserialize(input);

    auto novels = node["novels"].get_value<std::vector<test::novel>>();
    REQUIRE(novels.size() == 2);
    REQUIRE(novels[0].title == "Robinson Crusoe");
    REQUIRE(novels[0].author == "Daniel Defoe");
    REQUIRE(novels[0].year == 1678);
    REQUIRE(novels[1].title == "Frankenstein");
    REQUIRE(novels[1].author == "Mary Shelly");
    REQUIRE(novels[1].year == 1818);
}

TEST_CASE("FromNode_UserDefinedTypeVectorError") {
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

TEST_CASE("FromNode_UserDefinedTypeMap") {
    std::string input = "colors:\n"
                        "  ? color: 0xFFFFFF\n"
                        "  : r: 0xFF\n"
                        "    g: 0xFF\n"
                        "    b: 0xFF\n"
                        "  ? color: 0x808080\n"
                        "  : r: 0x80\n"
                        "    g: 0x80\n"
                        "    b: 0x80\n"
                        "  ? color: 0x586776\n"
                        "  : r: 0x58\n"
                        "    g: 0x67\n"
                        "    b: 0x76\n";
    fkyaml::node node = fkyaml::node::deserialize(input);

    auto colors = node["colors"].get_value<std::map<test::color, test::rgb>>();
    REQUIRE(colors.find(test::color {0xFFFFFF}) != colors.end());
    REQUIRE(colors.at(test::color {0xFFFFFF}).r == 0xFF);
    REQUIRE(colors.at(test::color {0xFFFFFF}).g == 0xFF);
    REQUIRE(colors.at(test::color {0xFFFFFF}).b == 0xFF);
    REQUIRE(colors.find(test::color {0x808080}) != colors.end());
    REQUIRE(colors.at(test::color {0x808080}).r == 0x80);
    REQUIRE(colors.at(test::color {0x808080}).g == 0x80);
    REQUIRE(colors.at(test::color {0x808080}).b == 0x80);
    REQUIRE(colors.find(test::color {0x586776}) != colors.end());
    REQUIRE(colors.at(test::color {0x586776}).r == 0x58);
    REQUIRE(colors.at(test::color {0x586776}).g == 0x67);
    REQUIRE(colors.at(test::color {0x586776}).b == 0x76);

    auto colors_umap = node["colors"].get_value<std::unordered_map<test::color, test::rgb>>();
}

TEST_CASE("FromNode_UserDefinedTypeMapError") {
    std::string input = "colors:\n"
                        "  ? color: 0xFFFFFF\n"
                        "  : r: 0xFF\n"
                        "    g: 0xFF\n"
                        "    b: 0xFF\n"
                        "  ? color: 0x808080\n"
                        "  : r: 0x80\n"
                        "    g: 0x80\n"
                        "    b: 0x80\n"
                        "  ? color: 0x586776\n"
                        "  : r: 0x58\n"
                        "    g: 0x67\n"
                        "    b: 0x76\n";
    fkyaml::node node = fkyaml::node::deserialize(input);
    using compat_map_t = std::map<test::color, test::rgb>;
    REQUIRE_THROWS_AS(node.get_value<compat_map_t>(), fkyaml::exception);

    fkyaml::node int_node = 123;
    REQUIRE_THROWS_AS(int_node.get_value<compat_map_t>(), fkyaml::exception);
}
