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
    // create YAML nodes.
    fkyaml::node sequence_node = {1, 2, 3};
    fkyaml::node mapping_node = {{"foo", true}, {"bar", false}};
    fkyaml::node null_node;
    fkyaml::node boolean_node = true;
    fkyaml::node integer_node = 256;
    fkyaml::node float_node = 3.14;
    fkyaml::node string_node = "Hello, world!";

    // call type()
    std::cout << std::boolalpha;
    std::cout << (sequence_node.type() == fkyaml::node::node_t::SEQUENCE) << std::endl;
    std::cout << (mapping_node.type() == fkyaml::node::node_t::MAPPING) << std::endl;
    std::cout << (null_node.type() == fkyaml::node::node_t::NULL_OBJECT) << std::endl;
    std::cout << (boolean_node.type() == fkyaml::node::node_t::BOOLEAN) << std::endl;
    std::cout << (integer_node.type() == fkyaml::node::node_t::INTEGER) << std::endl;
    std::cout << (float_node.type() == fkyaml::node::node_t::FLOAT_NUMBER) << std::endl;
    std::cout << (string_node.type() == fkyaml::node::node_t::STRING) << std::endl;

    return 0;
}
