//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.13
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iostream>
#include <fkYAML/node.hpp>

int main() {
    // create a YAML node.
    fkyaml::node n = 1.23;
    fkyaml::node n2 = "foo";

    // get the node value (value gets copied).
    auto float_val = n.get_value<float>();
    auto str_val = n2.get_value<std::string>();

    std::cout << float_val << std::endl;
    std::cout << str_val << std::endl;

    // Numeric scalar value can be converted to other numeric types inside get_value().
    auto bool_val = n.get_value<bool>();
    auto int_val = n.get_value<int>();

    std::cout << std::boolalpha << bool_val << std::endl;
    std::cout << int_val << std::endl;

    // specifying incompatible type throws an exception
    try {
        auto float_val = n2.get_value<float>();
    }
    catch (const fkyaml::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
