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
    // create a basic_node object.
    fkyaml::node n = {{"foo", true}, {"bar", {1, 2, 3}}, {"baz", {{"qux", 3.14}, {"corge", nullptr}}}};

    // serialize the basic_node object with insertion operator.
    // this is equivalent with:
    //   std::cout << fkyaml::node::serialize(n) << std::endl;
    std::cout << n << std::endl;

    return 0;
}
