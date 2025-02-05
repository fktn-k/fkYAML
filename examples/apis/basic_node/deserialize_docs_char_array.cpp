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
#include <fkYAML/node.hpp>

int main() {
    // deserialize a YAML string.
    char input[] = R"(
%YAML 1.2
---
foo: true
bar: 123
baz: 3.14
...
%TAG ! tag:test.com,2000:
---
null: one
false: 456
TRUE: 1.414
)";
    std::vector<fkyaml::node> docs = fkyaml::node::deserialize_docs(input);

    // check the deserialization result.
    std::cout << docs[0]["foo"].get_value<bool>() << std::endl;
    std::cout << docs[0]["bar"].get_value<std::int64_t>() << std::endl;
    std::cout << std::setprecision(3) << docs[0]["baz"].get_value<double>() << std::endl;

    std::cout << std::endl;

    std::cout << docs[1][nullptr].as_str() << std::endl;
    std::cout << docs[1][false].get_value<std::int64_t>() << std::endl;
    std::cout << std::setprecision(4) << docs[1][true].get_value<double>() << std::endl;

    return 0;
}
