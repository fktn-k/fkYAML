//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <fkYAML/node.hpp>

int main() {
    // deserialize a YAML string.
    std::string s = R"(
    foo: true
    bar: 123
    baz: 3.14
    )";
    fkyaml::node n = fkyaml::node::deserialize(s);

    // check the deserialization result.
    std::cout << n["foo"].get_value<bool>() << std::endl;
    std::cout << n["bar"].get_value<std::int64_t>() << std::endl;
    std::cout << std::setprecision(3) << n["baz"].get_value<double>() << std::endl;

    return 0;
}
