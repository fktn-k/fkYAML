//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.4
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iostream>
#include <fkYAML/node.hpp>

int main() {
    // create YAML nodes.
    fkyaml::node n1 = 123;
    fkyaml::node n2 = "foo";

    // swap the internally stored data between n1 & n2.
    using std::swap;
    swap(n1, n2);

    // print the swapped values.
    std::cout << n1.get_value_ref<std::string&>() << std::endl;
    std::cout << n2.get_value<std::int64_t>() << std::endl;
    return 0;
}
