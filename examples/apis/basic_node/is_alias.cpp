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
    // create YAML nodes (both anchor/non-anchor)
    fkyaml::node anchor = true;
    anchor.add_anchor_name("anchor");
    fkyaml::node alias = fkyaml::node::alias_of(anchor);
    fkyaml::node non_alias = 123;

    // print the result of testing whether they are an anchor node or not.
    std::cout << std::boolalpha;
    std::cout << anchor.is_alias() << std::endl;
    std::cout << alias.is_alias() << std::endl;
    std::cout << non_alias.is_alias() << std::endl;

    return 0;
}
