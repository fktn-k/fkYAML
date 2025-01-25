//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <fkYAML/node.hpp>

int main() {
    // deserialize a YAML string.
    std::array<char, 14> input = {'f', 'o', 'o', ':', ' ', 't', 'r', 'u', 'e', 'd', 'u', 'm', 'm', 'y'};
    fkyaml::node n = fkyaml::node::deserialize(input.begin(), input.begin() + 9);

    // check the deserialization result.
    std::cout << std::boolalpha << n["foo"].get_value<bool>() << std::endl;

    return 0;
}
