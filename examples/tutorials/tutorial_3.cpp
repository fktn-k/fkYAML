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

int main() {
    // open a YAML file. Other streams or strings are also usable as an input.
    std::ifstream ifs("example.yaml");

    // deserialize the loaded file contents.
    fkyaml::node root = fkyaml::node::deserialize(ifs);

    // create an empty YAML sequence node.
    fkyaml::node response = {{"recommends", fkyaml::node::sequence()}};
    auto& recommends = response["recommends"].as_seq();

    // generate recommendations by extracting "title" & "author" values.
    for (auto& novel_node : root["novels"]) {
        // create a recommendation node with an initializer list.
        fkyaml::node recommend = {{"title", novel_node["title"]}, {"author", novel_node["author"]}};
        recommends.emplace_back(std::move(recommend));
    }

    // print the response YAML nodes.
    std::cout << response << std::endl;

    return 0;
}
