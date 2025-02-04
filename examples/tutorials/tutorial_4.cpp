//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <fstream>
#include <iostream>
#include <utility>
#include <fkYAML/node.hpp>

// creating a namespace is not mandatory.
namespace ns {

struct novel {
    std::string title;
    std::string author;
    int year;
};

struct recommend {
    std::string title;
    std::string author;
};

// overloads must be defined in the same namespace as user-defined types.
void from_node(const fkyaml::node& node, novel& novel) {
    novel.title = node["title"].as_str();
    novel.author = node["author"].as_str();
    novel.year = node["year"].get_value<int>();
}

void to_node(fkyaml::node& node, const recommend& recommend) {
    node = fkyaml::node {{"title", recommend.title}, {"author", recommend.author}};
}

} // namespace ns

int main() {
    // open a YAML file. Other streams or strings are also usable as an input.
    std::ifstream ifs("example.yaml");

    // deserialize the loaded file contents.
    fkyaml::node root = fkyaml::node::deserialize(ifs);

    // create an empty YAML sequence node.
    fkyaml::node response = {{"recommends", fkyaml::node::sequence()}};
    auto& recommends = response["recommends"].as_seq();

    // get novels directly from the node.
    auto novels = root["novels"].get_value<std::vector<ns::novel>>();

    // generate recommendations by extracting "title" & "author" values.
    for (auto& novel : novels) {
        // create a recommendation node directly with a recommend object.
        ns::recommend recommend = {std::move(novel.title), std::move(novel.author)};
        recommends.emplace_back(recommend);
    }

    // print the response YAML nodes.
    std::cout << response << std::endl;

    return 0;
}
