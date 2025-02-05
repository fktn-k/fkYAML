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
    // create a integer node.
    fkyaml::node n = 123;

    // get reference to the integer value from a non-const node.
    // use `auto&` or `fkyaml::node::integer_type&` for forward compatibility.
    auto& i = n.as_int();
    // get const reference to the integer value from a const node.
    const fkyaml::node cn = n;
    const auto& ci = cn.as_int();

    // modify the integer value.
    i = 321;

    std::cout << i << std::endl;
    std::cout << ci << std::endl;
}
