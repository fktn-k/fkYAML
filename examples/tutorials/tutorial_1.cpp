//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <fstream>
#include <iostream>
#include <fkYAML/node.hpp>

int main() {
    // open a YAML file. Other streams or strings are also usable as an input.
    std::ifstream ifs("example.yaml");

    // deserialize the loaded file contents.
    fkyaml::node root = fkyaml::node::deserialize(ifs);

    // print the deserialized YAML nodes by serializing them back.
    std::cout << root << std::endl;
    return 0;
}
