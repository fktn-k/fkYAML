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
    // create a YAML node.
    fkyaml::node n = 123;

    // try to get a tag name before any tag name has been set.
    try {
        std::cout << n.get_tag_name() << std::endl;
    }
    catch (const fkyaml::exception& e) {
        std::cout << e.what() << std::endl;
    }

    // set a tag name to the node.
    n.add_tag_name("!!int");
    std::cout << n.get_tag_name() << std::endl;

    return 0;
}
