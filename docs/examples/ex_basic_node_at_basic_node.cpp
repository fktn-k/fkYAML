//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.4
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iostream>
#include <fkYAML/node.hpp>

int main() {
    // create a YAML sequence node.
    fkyaml::node n1 = {123, 234, 345, 456};

    // print YAML nodes at the following indexes.
    fkyaml::node index_zero = 0;
    fkyaml::node index_one = 1;
    fkyaml::node index_two = 2;
    fkyaml::node index_three = 3;
    std::cout << n1[index_zero] << std::endl;
    std::cout << n1[index_one] << std::endl;
    std::cout << n1[index_two] << std::endl;
    std::cout << n1[index_three] << std::endl;

    // try to print a YAML node with an index which exceeds the size.
    try {
        fkyaml::node index_four = 4;
        std::cout << n1.at(index_four) << std::endl;
    }
    catch (const fkyaml::out_of_range& e) {
        std::cout << e.what() << std::endl;
    }

    // create a YAML node.
    fkyaml::node n2 = {{"foo", true}, {"bar", 123}};

    // print YAML nodes associated with the following keys.
    fkyaml::node foo_key = "foo";
    fkyaml::node bar_key = "bar";
    std::cout << std::boolalpha << n2[foo_key] << std::endl;
    std::cout << n2[bar_key] << std::endl;

    // try to print a YAML node with a key which does not exist.
    try {
        fkyaml::node true_key = true;
        std::cout << n2.at(true_key) << std::endl;
    }
    catch (const fkyaml::out_of_range& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
