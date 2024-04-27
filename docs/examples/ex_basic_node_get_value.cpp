//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.5
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iostream>
#include <fkYAML/node.hpp>

int main() {
    // create a YAML node.
    fkyaml::node n = 123;
    fkyaml::node n2 = "foo";

    // get references to the value.
    auto int_val = n.get_value<int>();
    auto str_val = n2.get_value<std::string>();

    // print the values
    std::cout << int_val << std::endl;
    std::cout << str_val << std::endl;

    // specifying incompatible type throws an exception
    try {
        auto float_val = n2.get_value<float>();
    }
    catch (const fkyaml::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
