//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iomanip>
#include <iostream>
#include <type_traits>
#include <fkYAML/node.hpp>

int main() {
    std::cout << std::boolalpha
              << std::is_same<
                     fkyaml::node_value_converter<double, void>,
                     fkyaml::node::value_converter_type<double, void>>::value
              << std::endl;

    fkyaml::node n = 3.14;
    double d = 0.0;
    // This leads to the same result as `d = n.get_value<double>()`.
    fkyaml::node::value_converter_type<double, void>::from_node(n, d);
    std::cout << std::setprecision(3) << d << std::endl;

    return 0;
}
