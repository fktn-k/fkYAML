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

    // try to get an anchor name before any anchor name has been set.
    try {
        std::cout << n.get_anchor_name() << std::endl;
    }
    catch (const fkyaml::exception& e) {
        std::cout << e.what() << std::endl;
    }

    // set an anchor name to the node.
    n.add_anchor_name("anchor");
    std::cout << n.get_anchor_name() << std::endl;

    return 0;
}
