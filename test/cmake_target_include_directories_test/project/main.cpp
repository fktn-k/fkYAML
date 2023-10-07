//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.1
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iostream>

#include "fkYAML/node.hpp"
#include "fkYAML/deserializer.hpp"

int main()
{
    fkyaml::deserializer deserializer;
    fkyaml::node node = deserializer.deserialize("test: true");

    std::cout << "test: " << node["test"].to_string() << std::endl;

    return 0;
}
