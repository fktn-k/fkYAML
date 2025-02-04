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
    // create a float node.
    fkyaml::node n = 3.14;

    // get reference to the float value from a non-const node.
    // use `auto&` or `fkyaml::node::float_number_type&` for forward compatibility.
    auto& f = n.as_float();
    // get const reference to the float value from a const node.
    const fkyaml::node cn = n;
    const auto& cf = cn.as_float();

    // modify the float value.
    f = 1.41;

    std::cout << f << std::endl;
    std::cout << cf << std::endl;
}
