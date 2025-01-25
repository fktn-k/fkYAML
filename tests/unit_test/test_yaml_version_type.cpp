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

TEST_CASE("YamlVersionType_ToString") {
    using test_data_t = std::pair<fkyaml::yaml_version_type, std::string>;
    auto test_data = GENERATE(
        test_data_t {fkyaml::yaml_version_type::VERSION_1_1, "VERSION_1_1"},
        test_data_t {fkyaml::yaml_version_type::VERSION_1_2, "VERSION_1_2"});
    REQUIRE(test_data.second == fkyaml::to_string(test_data.first));
}
