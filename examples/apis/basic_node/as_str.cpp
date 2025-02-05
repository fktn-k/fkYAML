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
    // create a string node.
    fkyaml::node n = "foo";

    // get reference to the string value from a non-const node.
    // use `auto&` or `fkyaml::node::string_type&` for forward compatibility.
    auto& s = n.as_str();
    // get const reference to the string value from a const node.
    const fkyaml::node cn = n;
    const auto& cs = cn.as_str();

    // modify the string value.
    s = "bar";

    std::cout << s << std::endl;
    std::cout << cs << std::endl;
}
