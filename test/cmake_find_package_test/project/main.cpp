/**
 *   __ _  __     __      __  __ _
 *  / _| | \ \   / //\   |  \/  | |
 * | |_| | _\ \_/ //  \  | \  / | |
 * |  _| |/ /\   // /\ \ | |\/| | |
 * | | |   <  | |/ ____ \| |  | | |____
 * |_| |_|\_\ |_/_/    \_\_|  |_|______|
 *
 * @file main.cpp
 * @brief main source file for CMake find_package() test project.
 * @version 0.0.0
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

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
