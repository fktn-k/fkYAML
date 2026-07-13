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
    using namespace fkyaml::literals;

    // create a integer node.
    fkyaml::node n = "v: 15745692345339290292"_yaml;
    auto& v = n["v"];

    // get the integer value as an unsingned 64-bit integer from a non-const node.
    auto i = v.as_uint();

    std::cout << i << std::endl;

    // as_uint() throws `fkyaml::type_error` when the node value is not an unsigned integer.
    v = -123;
    try {
        auto j = v.as_uint();
    }
    catch (const fkyaml::type_error& e) {
        std::cout << e.what() << std::endl;
    }
}
