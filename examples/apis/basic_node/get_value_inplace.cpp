//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iostream>
#include <fkYAML/node.hpp>

struct not_default_constructible {
    not_default_constructible() = delete;
    explicit not_default_constructible(int v) noexcept
        : value(v) {
    }
    not_default_constructible(const not_default_constructible&) = default;

    int value {0};
};
// Custom implementation of from_node() for not_default_constructible objects.
// This function is called inside get_value_inplace().
// See https://fktn-k.github.io/fkYAML/api/node_value_converter/from_node/ for details.
inline void from_node(const fkyaml::node& n, not_default_constructible& ndc) {
    ndc.value = n.get_value<int>();
}

int main() {
    // create a sequence node.
    fkyaml::node seq = {true, false};

    // fill the node values into a 1D C-style array
    bool bools[2] {};
    seq.get_value_inplace(bools);
    for (auto b : bools) {
        std::cout << std::boolalpha << b << " ";
    }
    std::cout << "\n\n";

    // create an integer scalar node
    fkyaml::node integer = 123;

    // get_value_inplace() accepts a type which is not default constructible.
    not_default_constructible ndc {0};
    integer.get_value_inplace(ndc);
    std::cout << ndc.value << std::endl;

    // of course, you can pass a value of default constructible type as well.
    integer = -123;
    integer.get_value_inplace(ndc.value);
    std::cout << ndc.value << std::endl;

    return 0;
}
