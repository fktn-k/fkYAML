//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <fkYAML/node.hpp>

int main() {
    // create sequence nodes.
    fkyaml::node seq = {true, false};
    fkyaml::node seq2 = {123, 3.14, "foo"};

    // get the node values
    // to std::vector
    auto bool_vec = seq.get_value<std::vector<bool>>();
    for (auto b : bool_vec) {
        std::cout << std::boolalpha << b << " ";
    }
    std::cout << "\n\n";
    // to std::tuple
    auto tpl = seq2.get_value<std::tuple<int, float, std::string>>();
    std::cout << std::get<0>(tpl) << " ";
    std::cout << std::get<1>(tpl) << " ";
    std::cout << std::get<2>(tpl) << "\n\n";

    // create a mapping node.
    fkyaml::node map = {
        {0, "foo"},
        {1, "bar"},
        {2, "baz"},
    };
    // get the node values
    // to std::unordered_map
    auto umap = map.get_value<std::unordered_map<uint32_t, std::string>>();
    for (auto& p : umap) {
        std::cout << p.first << " : " << p.second << std::endl;
    }
    std::cout << std::endl;

    // create scalar nodes.
    fkyaml::node n = 1.23;
    fkyaml::node n2 = "foo";

    // get the node value (value gets copied).
    auto float_val = n.get_value<float>();
    auto str_val = n2.get_value<std::string>();

    std::cout << float_val << std::endl;
    std::cout << str_val << std::endl;

    // Numeric scalar value can be converted to other numeric types inside get_value().
    auto bool_val = n.get_value<bool>();
    auto int_val = n.get_value<int>();

    std::cout << std::boolalpha << bool_val << std::endl;
    std::cout << int_val << std::endl;

    // specifying incompatible type throws an exception
    try {
        auto float_val = n2.get_value<float>();
    }
    catch (const fkyaml::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
