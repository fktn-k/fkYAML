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
    fkyaml::node n = {{"foo", true}};

    // check if the node has a tag name.
    std::cout << std::boolalpha << n.has_tag_name() << std::endl;

    // set a tag name.
    n.add_tag_name("!!map");

    // check if the node has a tag name again.
    std::cout << std::boolalpha << n.has_tag_name() << std::endl;

    return 0;
}
