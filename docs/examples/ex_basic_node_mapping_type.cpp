//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.13
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <fkYAML/node.hpp>

int main() {
    std::cout << std::boolalpha << std::is_same<std::map<std::string, fkyaml::node>, fkyaml::node::mapping_type>::value
              << std::endl;
    return 0;
}
