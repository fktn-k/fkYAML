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
    const fkyaml::node cn = 123;

    // get references to the value.
    auto& ref = n.as_int();
    auto& cref = cn.as_int();

    // print the referenced values
    std::cout << ref << std::endl;
    std::cout << cref << std::endl;

    // specifying incompatible reference type throws an exception
    try {
        auto iref = n.as_map();
    }
    catch (const fkyaml::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
