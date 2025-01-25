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
    std::cout << n1[0] << std::endl;
    std::cout << n1[1] << std::endl;
    std::cout << n1[2] << std::endl;
    std::cout << n1[3] << std::endl;

    // this will cause an undefined behavior!
    // std::cout << n1[4] << std::endl;

    // create a YAML mapping node.
    fkyaml::node n2 = {{"foo", true}, {"bar", 123}};

    // print YAML nodes associated with the following keys.
    std::cout << std::boolalpha << n2["foo"] << std::endl;
    std::cout << n2["bar"] << std::endl;

    // try to access a YAML node with a key which does not exist.
    std::cout << n2[true] << std::endl;

    return 0;
}
