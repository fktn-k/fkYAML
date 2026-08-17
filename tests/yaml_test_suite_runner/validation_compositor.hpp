//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_VALIDATION_COMPOSITOR_HPP
#define FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_VALIDATION_COMPOSITOR_HPP

#include <deque>
#include <memory>
#include <stdexcept>

#include "event.hpp"
#include "validator.hpp"

namespace yaml_test_suite_runner {

class validation_compositor;

template <typename T>
class composition_state {
public:
    composition_state() = default;
    virtual ~composition_state() = default;

    virtual void on_enter() = 0;
    virtual void on_event(event&& evt) = 0;
    virtual void on_exit() = 0;
};

template <typename T>
class root_composition_state : public composition_state<T> {
public:
    root_composition_state() = default;
    ~root_composition_state() override = default;

    void on_enter() override {
        // nothing to do for root state
    }

    void on_event(event&& event) override {
        switch (event.type) {
        case event_type::STREAM_START:
            // Handle stream start event
            break;
        default:
            throw std::runtime_error("Unexpected event type at root level.");
            break;
        }
    }

    void on_exit() override {
        // nothing to do for root state
    }
};

class validation_compositor {
public:
    validation_compositor() = default;
    virtual ~validation_compositor() = default;

    virtual void handle_event(event&& evt) {
        // Process the event based on the current state
    }

private:
};

} // namespace yaml_test_suite_runner

#endif // FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_VALIDATION_COMPOSITOR_HPP
