//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iostream>
#include <fkYAML/node.hpp>
#include <fkYAML/ordered_map.hpp>

int main() {
    fkyaml::ordered_map<std::string, fkyaml::node> om = {{"foo", 123}, {"bar", "baz"}};

    std::cout << om["foo"] << std::endl;
    std::cout << om["bar"] << std::endl;

    // accesses with a unknown key will create a new key-value pair with the default value.
    std::cout << om["qux"] << std::endl;

    return 0;
}
