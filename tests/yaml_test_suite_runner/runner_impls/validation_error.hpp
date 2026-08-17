//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_VALIDATION_ERROR_HPP
#define FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_VALIDATION_ERROR_HPP

#include <stdexcept>

namespace yaml_test_suite_runner {

class validation_error : public std::runtime_error {
public:
    explicit validation_error(const std::string& message)
        : std::runtime_error(message) {
    }
};

} // namespace yaml_test_suite_runner

#endif // FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_VALIDATION_ERROR_HPP
