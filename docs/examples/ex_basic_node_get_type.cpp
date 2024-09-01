//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main() {
    // create YAML nodes.
    fkyaml::node sequence_node = {1, 2, 3};
    fkyaml::node mapping_node = {{"foo", true}, {"bar", false}};
    fkyaml::node null_node;
    fkyaml::node boolean_node = true;
    fkyaml::node integer_node = 256;
    fkyaml::node float_node = 3.14;
    fkyaml::node string_node = "Hello, world!";

    // query node value types by calling get_type().
    std::cout << std::boolalpha;
    std::cout << (sequence_node.get_type() == fkyaml::node_type::SEQUENCE) << std::endl;
    std::cout << (mapping_node.get_type() == fkyaml::node_type::MAPPING) << std::endl;
    std::cout << (null_node.get_type() == fkyaml::node_type::NULL_OBJECT) << std::endl;
    std::cout << (boolean_node.get_type() == fkyaml::node_type::BOOLEAN) << std::endl;
    std::cout << (integer_node.get_type() == fkyaml::node_type::INTEGER) << std::endl;
    std::cout << (float_node.get_type() == fkyaml::node_type::FLOAT) << std::endl;
    std::cout << (string_node.get_type() == fkyaml::node_type::STRING) << std::endl;

    return 0;
}
