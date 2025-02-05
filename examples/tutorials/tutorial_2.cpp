//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <fstream>
#include <iostream>
#include <fkYAML/node.hpp>

int main() {
    // open a YAML file. Other streams or strings are also usable as an input.
    std::ifstream ifs("example.yaml");

    // deserialize the loaded file contents.
    fkyaml::node root = fkyaml::node::deserialize(ifs);

    // print only values associated with "title" key.
    for (auto& novel_node : root["novels"]) {
        // get reference to the "title" value with `as_str` function.
        std::cout << novel_node["title"].as_str() << std::endl;
    }

    return 0;
}
