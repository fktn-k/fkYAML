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
    // create a sequence node.
    fkyaml::node n = {{"foo", true}, {"bar", 123}};

    // get reference to the sequence value from a non-const node.
    // use `auto&` or `fkyaml::node::sequence_type&` for forward compatibility.
    auto& m = n.as_map();
    // get const reference to the sequence value from a const node.
    const fkyaml::node cn = n;
    const auto& cm = cn.as_map();

    // modify the sequence value.
    m.emplace("baz", 3.14);

    for (auto& pair : m) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    std::cout << std::endl;
    for (const auto& cpair : cm) {
        std::cout << cpair.first << ": " << cpair.second << std::endl;
    }
}
