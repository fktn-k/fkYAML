//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.1
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iostream>

#include "fkYAML/Node.hpp"
#include "fkYAML/Deserializer.hpp"

int main()
{
    fkyaml::Deserializer deserializer;
    fkyaml::Node node = deserializer.Deserialize("test: true");

    std::cout << "test: " << node["test"].ToString() << std::endl;

    return 0;
}
