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
    fkyaml::ordered_map<std::string, fkyaml::node> om = {{"foo", 123}, {"bar", "baz"}};

    std::cout << om.at("foo") << std::endl;
    std::cout << om.at("bar") << std::endl;

    // accesses with a unknown key will throw an exception.
    try {
        fkyaml::node& n = om.at("baz");
    }
    catch (const fkyaml::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
