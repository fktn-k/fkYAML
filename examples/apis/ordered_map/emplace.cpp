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
    fkyaml::ordered_map<std::string, fkyaml::node> om = {{"foo", 123}, {"bar", "baz"}};

    // insert a value with a new key.
    auto ret = om.emplace("qux", 3.14);
    if (ret.second) {
        std::cout << "emplacement took place." << std::endl;
    }
    std::cout << ret.first->second << std::endl;

    // insert a value with an existing key.
    auto ret2 = om.emplace("foo", true);
    if (!ret2.second) {
        std::cout << "emplacement did not take place." << std::endl;
    }
    std::cout << ret2.first->second << std::endl;

    return 0;
}
