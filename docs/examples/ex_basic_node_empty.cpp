//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.8
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main() {
    // create YAML nodes.
    std::vector<fkyaml::node> nodes = {
        fkyaml::node::sequence(),
        {1, 2, 3},
        {{"foo", true}, {"bar", false}},
        fkyaml::node(),
        true,
        256,
        3.14,
        "Hello, world!"};

    for (const auto& n : nodes) {
        try {
            // call empty()
            std::cout << std::boolalpha << n.empty() << std::endl;
        }
        catch (const fkyaml::exception& e) {
            std::cout << "The node does not have a container nor string value." << std::endl;
        }
    }

    return 0;
}
