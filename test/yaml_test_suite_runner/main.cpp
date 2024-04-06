//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <fkYAML/node.hpp>

#include "skip_test_case_list.hpp"
#include "stream_parse_engine.hpp"

// Test result codes.
static const int TEST_RESULT_OK = 0;
static const int TEST_RESULT_NG = 1;

// Definitions of indexes of command line arguments.
static const int TEST_CASE_ID_INDEX = 1;
static const int TEST_DIR_PREFIX_INDEX = 2;
static const int TEST_MODE_INDEX = 3;
static const int TEST_ARG_COUNT = 4;

bool is_empty_key_case_id(const std::string& test_case_id, const int input_file_type, std::string& reason) {
    reason.clear();

    for (const auto& empty_key_case : EMPTY_KEY_CASES) {
        if (input_file_type != empty_key_case.input_file_type) {
            continue;
        }

        if (test_case_id == empty_key_case.test_case_id) {
            reason = empty_key_case.reason;
            return true;
        }
    }

    return false;
}

bool is_malformed_input_case_id(const std::string& test_case_id, const int input_file_type, std::string& reason) {
    reason.clear();

    for (const auto& skip_test_case : MALFORMED_INPUT_CASES) {
        if (input_file_type != skip_test_case.input_file_type) {
            continue;
        }

        if (test_case_id == skip_test_case.test_case_id) {
            reason = skip_test_case.reason;
            return true;
        }
    }

    return false;
};

bool is_false_error_case_id(const std::string& test_case_id, const int input_file_type, std::string& reason) {
    reason.clear();

    for (const auto& false_error_case : FALSE_ERROR_CASES) {
        if (input_file_type != false_error_case.input_file_type) {
            continue;
        }

        if (test_case_id == false_error_case.test_case_id) {
            reason = false_error_case.reason;
            return true;
        }
    }

    return false;
}

int main(int argc, char* argv[]) {
    if (argc != TEST_ARG_COUNT) {
        std::cout << "Invalid command line arguments.\n";
        std::cout.flush();
        return TEST_RESULT_NG;
    }

    //
    // setup variables for testing
    //

    std::string test_case_id(argv[TEST_CASE_ID_INDEX]);
    std::cout << "test case id: " << test_case_id << "\n";

    std::string test_dir(argv[TEST_DIR_PREFIX_INDEX]);
    std::cout << "test dir: " << test_dir << "\n";

    std::string test_data_path(test_dir);
    std::string test_mode_str(argv[TEST_MODE_INDEX]);
    int input_file_type = INPUT_FILE_TYPE_YAML;
    if (test_mode_str == "0") {
        test_data_path += "/in.yaml";
    }
    else if (test_mode_str == "1") {
        test_data_path += "/in.json";
        input_file_type = INPUT_FILE_TYPE_JSON;
    }
    else {
        std::cout << "Invalid test mode. test_mode: " << test_mode_str << "\n";
        std::cout.flush();
        return TEST_RESULT_NG;
    }
    std::cout << "test data path: " << test_data_path << "\n";

    std::ifstream label_file(test_dir + "/===");
    if (label_file) {
        std::cout << "label: ";
        std::string label;
        std::getline(label_file, label);
        std::cout << label << "\n";
    }

    std::ifstream test_data_file(test_data_path);
    if (!test_data_file) {
        std::cout << "Failed to open yaml data file. path: " << test_data_path << "\n";
        std::cout.flush();
        return TEST_RESULT_NG;
    }

    std::ifstream error_file(test_dir + "/error");
    bool is_valid = !error_file.is_open();

    {
        std::string reason {};

        //
        // check if the targe test case has empty keys (deliberately unsupported).
        //
        if (is_empty_key_case_id(test_case_id, input_file_type, reason)) {
            std::cout << "Skipped. reason: " << reason << "\n";
            return TEST_RESULT_OK;
        }

        //
        // check if the target test case should be skipped.
        //
        if (is_malformed_input_case_id(test_case_id, input_file_type, reason)) {
            std::cout << "Skipped. reason: " << reason << "\n";
            return TEST_RESULT_OK;
        }

        //
        // check if the target test case is a false error case.
        //
        if (!is_valid && is_false_error_case_id(test_case_id, input_file_type, reason)) {
            is_valid = true;
        }
    }

    //
    // build expected YAML data from the event file.
    //

    // std::vector<fkyaml::node> expected {};
    // if (is_valid) {
    //     std::string yaml_event_filename = test_dir + "/test.event";
    //     stream_event_source source(yaml_event_filename.c_str());
    //     stream_event_callback callback(expected);
    //     stream_parse_engine parse_engine(callback);
    //     parse_engine.parse(source);
    // }

    //
    // run the target test case.
    //

    int result = TEST_RESULT_OK;
    try {
        fkyaml::node actual_from_yaml = fkyaml::node::deserialize_docs(test_data_file);

        if (!is_valid) {
            std::cout << "[YAML] Parsing invalid test data succeeded mistakenly.\n";
            result = TEST_RESULT_NG;
        }

        // if (actual_from_yaml != expected) {
        //     std::cout << "[YAML] Wrong parse result!\n";
        //     result = TEST_RESULT_NG;
        // }
    }
    catch (const fkyaml::exception& e) {
        if (is_valid) {
            std::cout << "[YAML] fkYAML error: " << e.what() << "\n";
            result = TEST_RESULT_NG;
        }
    }
    catch (const std::exception& e) {
        std::cout << "[YAML] Unexpected error: " << e.what() << "\n";
        result = TEST_RESULT_NG;
    }

    std::cout.flush();
    return result;
}
