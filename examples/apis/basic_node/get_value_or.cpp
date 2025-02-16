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
    // try get sequence node values
    fkyaml::node seq = {true, false};

    // successful case
    std::vector<bool> vec_def {false, true};
    auto bool_vec = seq.get_value_or<std::vector<bool>>(vec_def);
    for (auto b : bool_vec) {
        std::cout << std::boolalpha << b << std::endl;
    }

    // error case
    auto tpl_def = std::make_tuple<int, bool, std::string>(0, false, "default");
    auto tpl = seq.get_value_or<std::tuple<int, bool, std::string>>(std::move(tpl_def));
    std::cout << std::get<0>(tpl) << ", ";
    std::cout << std::get<1>(tpl) << ", ";
    std::cout << std::get<2>(tpl) << "\n\n";

    // try to get mapping node values
    fkyaml::node map = {
        {0, "foo"},
        {1, "bar"},
        {2, "baz"},
    };
    std::unordered_map<uint32_t, std::string> umap_def {{0, "defalt"}};
    auto umap = map.get_value_or<std::unordered_map<uint32_t, std::string>>(std::move(umap_def));
    for (auto& p : umap) {
        std::cout << p.first << " : " << p.second << std::endl;
    }
    std::cout << std::endl;

    // try to get scalar node values.
    fkyaml::node scalar = 1.23;

    // get the node value (value gets copied).
    auto dbl_val = scalar.get_value_or<double>(0.);
    auto str_val = scalar.get_value_or<std::string>("default");

    std::cout << dbl_val << std::endl;
    std::cout << str_val << std::endl;

    // Numeric scalar value will be converted to target numeric types inside get_value_or().
    auto bool_val = scalar.get_value_or<bool>(false); // 1.23 -> true
    auto int_val = scalar.get_value_or<int>(0);       // 1.23 -> 1

    std::cout << std::boolalpha << bool_val << std::endl;
    std::cout << int_val << std::endl;

    return 0;
}
