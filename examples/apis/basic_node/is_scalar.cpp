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
    // create YAML nodes.
    fkyaml::node null_node;
    fkyaml::node boolean_node = true;
    fkyaml::node integer_node = 256;
    fkyaml::node float_node = 3.14;
    fkyaml::node string_node = "Hello, world!";

    // check if the nodes are a scalar by calling is_scalar()
    std::cout << std::boolalpha;
    std::cout << null_node.is_scalar() << std::endl;
    std::cout << boolean_node.is_scalar() << std::endl;
    std::cout << integer_node.is_scalar() << std::endl;
    std::cout << float_node.is_scalar() << std::endl;
    std::cout << string_node.is_scalar() << std::endl;

    return 0;
}
