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
    // bring the user-defined literals into this scope.
    using namespace fkyaml::literals::yaml_literals;

    // deserialize a YAML string encoded in the UTF-8 format.
    fkyaml::node n_u8 = u8"foo: bar"_yaml;
    std::cout << n_u8 << std::endl;

    // deserialize a YAML string encoded in the UTF-16 format.
    fkyaml::node n_u16 = u"foo: bar"_yaml;
    std::cout << n_u16 << std::endl;

    // deserialize a YAML string encoded in the UTF-32 format.
    fkyaml::node n_u32 = U"foo: bar"_yaml;
    std::cout << n_u32 << std::endl;

    return 0;
}
