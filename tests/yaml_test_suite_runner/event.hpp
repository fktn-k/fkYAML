//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_EVENT_HPP
#define FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_EVENT_HPP

#include <string>
#include <utility>
#include <vector>

namespace yaml_test_suite_runner {

enum class event_type {
    STREAM_START,
    STREAM_END,
    DOCUMENT_START,
    DOCUMENT_END,
    SEQUENCE_START,
    SEQUENCE_END,
    MAPPING_START,
    MAPPING_END,
    SCALAR,
    ALIAS,
};

enum class event_param_type {
    VALUE,
    ANCHOR,
    TAG,
};

using event_param = std::pair<event_param_type, std::string>;

struct event {
    event(event_type t) noexcept
        : type(t) {
    }

    event(event_type t, std::vector<event_param>&& p) noexcept
        : type(t),
          params(std::move(p)) {
    }

    event_type type;
    std::vector<event_param> params {};
};

} // namespace yaml_test_suite_runner

#endif // FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_EVENT_HPP
