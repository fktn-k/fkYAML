//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iostream>
#include <list>
#include <tuple>
#include <unordered_map>
#include <fkYAML/node.hpp>

int main() {
    // create nodes from objects of various types.
    fkyaml::node sequence0 = std::list<bool> {true, false, false};
    fkyaml::node sequence1 = std::make_tuple<std::string, int, bool>("foo", 123, true);
    std::unordered_map<int, float> map_val = {{123, 3.14f}, {-456, 1.41f}};
    fkyaml::node mapping = std::move(map_val);
    fkyaml::node null = nullptr;
    fkyaml::node boolean = false;
    fkyaml::node integer = 12345;
    fkyaml::node floating_point = 3.141592;
    const char str_chars[] = "test";
    fkyaml::node string = str_chars;

    // print node values
    std::cout << sequence0 << std::endl;
    std::cout << sequence1 << std::endl;
    std::cout << mapping << std::endl;
    std::cout << null << std::endl;
    std::cout << boolean << std::endl;
    std::cout << integer << std::endl;
    std::cout << floating_point << std::endl;
    std::cout << string << std::endl;
    return 0;
}
