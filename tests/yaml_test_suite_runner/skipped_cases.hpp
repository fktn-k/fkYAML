//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_YAML_TEST_SUITE_RUNNER_SKIPPED_CASES_HPP
#define FK_YAML_YAML_TEST_SUITE_RUNNER_SKIPPED_CASES_HPP

//
// This header file is not used but kept as-is for reference.
//

#include <string>

const int INPUT_FILE_TYPE_YAML = 0;
const int INPUT_FILE_TYPE_JSON = 1;

struct skip_test_case {
    skip_test_case(int type, std::string id, std::string reason)
        : input_file_type(type),
          test_case_id(id),
          reason(reason) {
    }

    int input_file_type {INPUT_FILE_TYPE_YAML}; // either INPUT_FILE_TYPE_YAML or INPUT_FILE_TYPE_JSON.
    std::string test_case_id {};
    std::string reason {};
};

// the list of test cases which contains empty keys since fkYAML deliberately doesn't support them.
skip_test_case EMPTY_KEY_CASES[] = {
    {INPUT_FILE_TYPE_YAML, "2JQS", "empty keys are deliberately unsupported."}, // done
    {INPUT_FILE_TYPE_YAML, "6M2F", "empty keys are deliberately unsupported."}, // done
    {INPUT_FILE_TYPE_YAML, "CFD4", "empty keys are deliberately unsupported."},
    {INPUT_FILE_TYPE_YAML, "FRK4", "empty keys are deliberately unsupported."},
    {INPUT_FILE_TYPE_YAML, "NHX8", "empty keys are deliberately unsupported."},
    {INPUT_FILE_TYPE_YAML, "NKF9", "empty keys are deliberately unsupported."},
    {INPUT_FILE_TYPE_YAML, "PW8X", "empty keys are deliberately unsupported."},
    {INPUT_FILE_TYPE_YAML, "S3PD", "empty keys are deliberately unsupported."},
    {INPUT_FILE_TYPE_YAML, "SM9W_01", "empty keys are deliberately unsupported."},
    {INPUT_FILE_TYPE_YAML, "UKK6_00", "empty keys are deliberately unsupported."},
};

// the list of test cases which should be skipped due to issues in the input test data.
skip_test_case MALFORMED_INPUT_CASES[] = {
    {INPUT_FILE_TYPE_JSON, "35KP", "malformed JSON generated from multiple YAML documents."},
    {INPUT_FILE_TYPE_JSON, "5TYM", "malformed JSON generated from multiple YAML documents."}, // done
    {INPUT_FILE_TYPE_JSON, "6WLZ", "malformed JSON generated from multiple YAML documents."}, // done
    {INPUT_FILE_TYPE_JSON, "6XDY", "malformed JSON generated from multiple YAML documents."}, // done
    {INPUT_FILE_TYPE_JSON, "6ZKB", "malformed JSON generated from multiple YAML documents."}, // done
    {INPUT_FILE_TYPE_JSON, "7Z25", "malformed JSON generated from multiple YAML documents."},
    {INPUT_FILE_TYPE_JSON, "9DXL", "malformed JSON generated from multiple YAML documents."},
    {INPUT_FILE_TYPE_JSON, "9KAX", "malformed JSON generated from multiple YAML documents."},
    {INPUT_FILE_TYPE_JSON, "9WXW", "malformed JSON generated from multiple YAML documents."},
    {INPUT_FILE_TYPE_JSON, "JHB9", "malformed JSON generated from multiple YAML documents."},
    {INPUT_FILE_TYPE_JSON, "KSS4", "malformed JSON generated from multiple YAML documents."},
    {INPUT_FILE_TYPE_JSON, "L383", "malformed JSON generated from multiple YAML documents."},
    {INPUT_FILE_TYPE_JSON, "M7A3", "malformed JSON generated from multiple YAML documents."},
    {INPUT_FILE_TYPE_JSON, "PUW8", "malformed JSON generated from multiple YAML documents."},
    {INPUT_FILE_TYPE_JSON, "RZT7", "malformed JSON generated from multiple YAML documents."},
    {INPUT_FILE_TYPE_JSON, "U9NS", "malformed JSON generated from multiple YAML documents."},
    {INPUT_FILE_TYPE_JSON, "UT92", "malformed JSON generated from multiple YAML documents."},
    {INPUT_FILE_TYPE_JSON, "W4TN", "malformed JSON generated from multiple YAML documents."},
};

// the list of test cases which seems to be false error cases.
skip_test_case FALSE_ERROR_CASES[] = {
    {INPUT_FILE_TYPE_JSON, "DK95_01", "marked as an error case, but seems valid JSON."},
    {INPUT_FILE_TYPE_JSON, "DK95_06", "marked as an error case, but seems valid JSON."},
};

#endif /* FK_YAML_YAML_TEST_SUITE_RUNNER_SKIPPED_CASES_HPP */
