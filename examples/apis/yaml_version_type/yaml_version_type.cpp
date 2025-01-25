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
    char input[] = "%YAML 1.2\n"
                   "---\n"
                   "foo: bar\n";

    // deserialize a YAML formatted string.
    fkyaml::node n = fkyaml::node::deserialize(input);

    // query the YAML version by calling get_yaml_version_type().
    std::cout << std::boolalpha;
    std::cout << (n.get_yaml_version_type() == fkyaml::yaml_version_type::VERSION_1_2) << std::endl;

    // overwrite the YAML version to 1.1.
    n.set_yaml_version_type(fkyaml::yaml_version_type::VERSION_1_1);

    // query the YAML version again.
    std::cout << (n.get_yaml_version_type() == fkyaml::yaml_version_type::VERSION_1_1) << std::endl;

    return 0;
}
