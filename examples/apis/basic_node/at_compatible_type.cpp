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
    // create a YAML sequence node.
    fkyaml::node n1 = {123, 234, 345, 456};

    // print YAML nodes at the following indexes.
    std::cout << n1.at(0) << std::endl;
    std::cout << n1.at(1) << std::endl;
    std::cout << n1.at(2) << std::endl;
    std::cout << n1.at(3) << std::endl;

    // try to print a YAML node with an index which exceeds the size.
    try {
        std::cout << n1.at(4) << std::endl;
    }
    catch (const fkyaml::out_of_range& e) {
        std::cout << e.what() << std::endl;
    }

    // create a YAML mapping node.
    fkyaml::node n2 = {{"foo", true}, {"bar", 123}};

    // print YAML nodes associated with the following keys.
    std::cout << std::boolalpha << n2.at("foo") << std::endl;
    std::cout << n2.at("bar") << std::endl;

    // try to print a YAML node with a key which does not exist.
    try {
        std::cout << n2.at(true) << std::endl;
    }
    catch (const fkyaml::out_of_range& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
