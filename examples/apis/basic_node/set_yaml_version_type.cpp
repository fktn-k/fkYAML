//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.1
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main() {
    fkyaml::node n;
    n.set_yaml_version_type(fkyaml::yaml_version_type::VERSION_1_1);
    fkyaml::node n2;
    n2.set_yaml_version_type(fkyaml::yaml_version_type::VERSION_1_2);

    std::cout << std::boolalpha;
    std::cout << (n.get_yaml_version_type() == fkyaml::yaml_version_type::VERSION_1_1) << std::endl;
    std::cout << (n2.get_yaml_version_type() == fkyaml::yaml_version_type::VERSION_1_2) << std::endl;

    return 0;
}
