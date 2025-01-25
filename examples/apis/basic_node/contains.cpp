//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main() {
    // create a YAML mapping node.
    fkyaml::node n = {{"foo", true}, {"bar", 123}};

    // check if the node has the following keys.
    std::cout << std::boolalpha;
    std::cout << n.contains("foo") << std::endl;
    std::cout << n.contains(fkyaml::node("baz")) << std::endl;

    // create a YAML node. (not mapping)
    fkyaml::node n2 = "qux";

    // check if the node has the following key.
    std::cout << std::boolalpha << n2.contains("qux") << std::endl;

    return 0;
}
