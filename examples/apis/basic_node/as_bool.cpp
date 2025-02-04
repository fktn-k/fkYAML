//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iostream>
#include <iomanip>
#include <fkYAML/node.hpp>

int main() {
    // create a boolean node.
    fkyaml::node n = true;

    // get reference to the boolean value from a non-const node.
    // use `auto&` or `fkyaml::node::boolean_type&` for forward compatibility.
    auto& b = n.as_bool();
    // get const reference to the boolean value from a const node.
    const fkyaml::node cn = n;
    const auto& cb = cn.as_bool();

    // modify the boolean value.
    b = false;

    std::cout << std::boolalpha << b << std::endl;
    std::cout << std::boolalpha << cb << std::endl;
}
