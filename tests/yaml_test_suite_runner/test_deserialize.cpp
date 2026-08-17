//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <doctest/doctest.h>

#include <fkYAML/exception.hpp>
#include <fkYAML/node.hpp>

#include "event_file_parser.hpp"
#include "validation_compositor.hpp"
#include <yaml_test_suite_dir.hpp>

namespace {

enum class input_format {
    YAML,
    JSON,
};

void print_label(const std::string& path) {
    std::ifstream label_file(path.c_str());
    if (label_file) {
        std::string label;
        std::getline(label_file, label);
        std::cout << "label: " << label << std::endl;
    }
}

std::string case_root(const std::string& relative_case_dir) {
    return std::string(FK_YAML_YAML_TEST_SUITE_ROOT_DIR) + "/" + relative_case_dir;
}

const char* to_string(const input_format format) {
    switch (format) {
    case input_format::YAML:
        return "YAML";
    case input_format::JSON:
        return "JSON";
    }

    return "unknown";
}

const char* input_file_name(const input_format format) {
    return (format == input_format::YAML) ? "in.yaml" : "in.json";
}

bool file_exists(const std::string& path) {
    std::ifstream ifs(path.c_str());
    return static_cast<bool>(ifs);
}

std::vector<std::unique_ptr<yaml_test_suite_runner::validator<fkyaml::node>>> load_validators(
    const std::string& relative_case_dir, const input_format format) {
    const std::string event_file_path = case_root(relative_case_dir) + "/test.event";
    std::ifstream event_file(event_file_path.c_str());
    if (!event_file) {
        throw std::runtime_error("Failed to open event file: " + event_file_path);
    }

    yaml_test_suite_runner::event_file_parser parser(event_file);
    yaml_test_suite_runner::validation_compositor<fkyaml::node> compositor(
        (format == input_format::YAML) ? yaml_test_suite_runner::validation_mode::YAML
                                       : yaml_test_suite_runner::validation_mode::JSON);
    while (!parser.empty()) {
        compositor.handle_event(parser.get_next_event());
    }

    return compositor.take_document_validators();
}

bool should_skip_json_case(const std::string& test_id, std::string& reason) {
    static const char* const malformed_input_cases[] = {
        "35KP", "5TYM", "6WLZ", "6XDY", "6ZKB", "7Z25", "9DXL", "9KAX", "9WXW", "JHB9", "KSS4",
        "L383", "M7A3", "PUW8", "RZT7", "U9NS", "UT92", "W4TN",
    };
    static const char* const false_error_cases[] = {
        "DK95-01",
        "DK95-06",
    };

    for (std::size_t i = 0; i < sizeof(malformed_input_cases) / sizeof(malformed_input_cases[0]); ++i) {
        if (test_id == malformed_input_cases[i]) {
            reason = "malformed JSON generated from multiple YAML documents.";
            return true;
        }
    }

    for (std::size_t i = 0; i < sizeof(false_error_cases) / sizeof(false_error_cases[0]); ++i) {
        if (test_id == false_error_cases[i]) {
            reason = "marked as an error case, but seems valid JSON.";
            return true;
        }
    }

    return false;
}

void run_yaml_test_suite_case(const char* relative_case_dir, const char* test_id, input_format format) {
    const std::string relative_case_path(relative_case_dir);
    const std::string suite_case_id(test_id);
    const std::string suite_case_root = case_root(relative_case_path);

    print_label(suite_case_root + "/===");
    INFO("Input format: " << to_string(format));

    if (format == input_format::JSON) {
        std::string reason;
        if (should_skip_json_case(suite_case_id, reason)) {
            INFO("Skipped: " << reason);
            CHECK(true);
            return;
        }
    }

    const std::string input_path = suite_case_root + "/" + input_file_name(format);
    std::ifstream ifs(input_path.c_str());
    REQUIRE(ifs);

    const bool expects_parse_error = file_exists(suite_case_root + "/error");
    if (expects_parse_error) {
        CHECK_THROWS_AS(fkyaml::node::deserialize_docs(ifs), fkyaml::exception);
        return;
    }

    std::vector<std::unique_ptr<yaml_test_suite_runner::validator<fkyaml::node>>> validators;
    REQUIRE_NOTHROW(validators = load_validators(relative_case_path, format));

    std::vector<fkyaml::node> docs;
    REQUIRE_NOTHROW(docs = fkyaml::node::deserialize_docs(ifs));
    REQUIRE(docs.size() == validators.size());

    for (std::size_t index = 0; index < docs.size(); ++index) {
        CHECK_NOTHROW(validators[index]->validate(docs[index]));
    }
}

} // namespace

#include "generated_yaml_test_suite_cases.hpp"
