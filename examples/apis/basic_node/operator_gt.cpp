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
    fkyaml::node seq_1 = {1, 2, 3};
    fkyaml::node seq_2 = {1, 2, 4};
    fkyaml::node map_1 = {{"foo", true}, {"bar", 123}};
    fkyaml::node map_2 = {{"bar", 123}, {"foo", true}};
    fkyaml::node null_1 = nullptr;
    fkyaml::node null_2 = nullptr;
    fkyaml::node bool_1 = false;
    fkyaml::node bool_2 = true;
    fkyaml::node integer_1 = 321;
    fkyaml::node integer_2 = 123;
    fkyaml::node float_1 = 1.23;
    fkyaml::node float_2 = 3.14;
    fkyaml::node string_1 = "foo";
    fkyaml::node string_2 = "bar";

    // the same type
    std::cout << std::boolalpha;
    std::cout << (seq_1 > seq_2) << std::endl;
    std::cout << (map_1 > map_2) << std::endl;
    std::cout << (null_1 > null_2) << std::endl;
    std::cout << (bool_1 > bool_2) << std::endl;
    std::cout << (integer_1 > integer_2) << std::endl;
    std::cout << (float_1 > float_2) << std::endl;
    std::cout << (string_1 > string_2) << std::endl;

    // different types
    std::cout << (bool_1 > string_1) << std::endl;
    std::cout << (float_2 > seq_2) << std::endl;

    return 0;
}
