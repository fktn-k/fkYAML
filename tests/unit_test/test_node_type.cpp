//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <utility>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("NodeType_ToString") {
    using test_data_t = std::pair<fkyaml::node_type, std::string>;
    auto test_data = GENERATE(
        test_data_t {fkyaml::node_type::SEQUENCE, "SEQUENCE"},
        test_data_t {fkyaml::node_type::MAPPING, "MAPPING"},
        test_data_t {fkyaml::node_type::NULL_OBJECT, "NULL_OBJECT"},
        test_data_t {fkyaml::node_type::BOOLEAN, "BOOLEAN"},
        test_data_t {fkyaml::node_type::INTEGER, "INTEGER"},
        test_data_t {fkyaml::node_type::FLOAT, "FLOAT"},
        test_data_t {fkyaml::node_type::STRING, "STRING"});
    REQUIRE(test_data.second == fkyaml::to_string(test_data.first));
}
