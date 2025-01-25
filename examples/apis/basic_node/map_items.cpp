//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iostream>
#include <fkYAML/node.hpp>

int main() {
    // create a mapping node.
    fkyaml::node map_node = {{"foo", 123}, {"bar", true}};

    // print all the mapping entries.
    for (auto it : map_node.map_items()) {
        std::cout << "key: " << it.key() << ", value: " << it.value() << std::endl;
    }

    fkyaml::node seq_node = {123, false, 3.14};
    try {
        // map_items() cannot be called on a sequence node.
        auto invalid = seq_node.map_items();
    }
    catch (const fkyaml::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
