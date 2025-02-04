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
    // create a YAML node.
    fkyaml::node anchor_node = "test";

    // add an anchor name to the node.
    anchor_node.add_anchor_name("anchor");

    // create an alias YAML node.
    fkyaml::node alias_node = fkyaml::node::alias_of(anchor_node);

    // print the value in the alias node.
    std::cout << alias_node.as_str() << std::endl;

    return 0;
}
