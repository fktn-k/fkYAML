//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main() {
    // create a YAML mapping node.
    fkyaml::node n = {{"foo", true}, {"bar", 123}};

    // erase entries by compatible and node keys.
    std::cout << n.erase("foo") << std::endl;
    std::cout << n.erase(fkyaml::node("baz")) << std::endl;

    // confirm that the erased key no longer exists.
    std::cout << std::boolalpha << n.contains("foo") << std::endl;

    return 0;
}
