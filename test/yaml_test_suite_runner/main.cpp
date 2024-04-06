//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.9
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <fkYAML/node.hpp>

// Test result codes.
static const int TEST_RESULT_OK = 0;
static const int TEST_RESULT_NG = 1;

// Definitions of indexes of command line arguments.
static const int TEST_DIR_PREFIX_INDEX = 1;
static const int TEST_MODE_INDEX = 2;

void scan_event_tag(const std::string& input, std::string& event_tag, std::size_t& read_counts) {
    std::size_t event_tag_end_pos = input.find_first_not_of(' ');
    // allows the return value to be `npos` for "+STR", "-MAP" and so forth.
    event_tag = input.substr(0, event_tag_end_pos);
    read_counts = event_tag_end_pos;
}

void scan_tag(const std::string& input, std::string& tag, fkyaml::node::node_t& type, std::size_t& read_counts) {
    std::size_t tag_end_pos = input.find_first_of('>');
    if (tag_end_pos == std::string::npos) {
        throw std::runtime_error("failed to find the end of tag.");
    }
    tag = input.substr(0, tag_end_pos + 1);
    read_counts = tag_end_pos + 1;

    if (input.rfind("<tag:yaml.org,2002:", 0) == std::string::npos) {
        // user-defined tags are used for mappings in the test suite.
        type = fkyaml::node::node_t::MAPPING;
        return;
    }

    const std::string suffix = input.substr(sizeof("<tag:yaml.org,2002:") - 1);

    if (suffix == "seq>") {
        type = fkyaml::node::node_t::SEQUENCE;
    }
    else if (suffix == "map>") {
        type = fkyaml::node::node_t::MAPPING;
    }
    else if (suffix == "null>") {
        type = fkyaml::node::node_t::NULL_OBJECT;
    }
    else if (suffix == "bool>") {
        type = fkyaml::node::node_t::BOOLEAN;
    }
    else if (suffix == "int>") {
        type = fkyaml::node::node_t::INTEGER;
    }
    else if (suffix == "float>") {
        type = fkyaml::node::node_t::FLOAT_NUMBER;
    }
    else if (suffix == "str>") {
        type = fkyaml::node::node_t::STRING;
    }
    else if (suffix == "omap>") {
        // FIXME: modify the type when "omap" gets supported.
        type = fkyaml::node::node_t::MAPPING;
    }
    else if (suffix == "pairs>") {
        // FIXME: modify the type when "pairs" gets supported.
        type = fkyaml::node::node_t::SEQUENCE;
    }
    else if (suffix == "set>") {
        // FIXME: modify the type when "set" gets supported.
        type = fkyaml::node::node_t::MAPPING;
    }
    else if (suffix == "binary>") {
        // FIXME: modify the type when "binary" gets supported.
        type = fkyaml::node::node_t::STRING;
    }
    else {
        throw std::runtime_error("invalid YAML schema suffix found.");
    }
}

fkyaml::node parse_events(std::istream& is) {
    fkyaml::node root;
    fkyaml::node* p_current = &root;
    std::vector<fkyaml::node*> node_ptr_stack {};

    std::string event_line {};
    // test.event files end with a trailing empty line.
    while (std::getline(is, event_line)) {
        std::size_t counts = 0;
        std::string event_tag {};
        scan_event_tag(event_line, event_tag, counts);

        if (event_tag == "+STR") {
            continue;
        }

        if (event_tag == "-STR") {
            continue;
        }

        if (event_tag == "+DOC") {
            continue;
        }

        if (event_tag == "-DOC") {
            continue;
        }

        if (event_tag == "+MAP") {
            counts += 4;
            *p_current = fkyaml::node::mapping();

            if (event_line[counts] == '\n') {
                counts += 1;
                continue;
            }

            if (std::strncmp(&event_line[counts], " {}", 3) == 0) {
                counts += 3;
            }

            if (std::strncmp(&event_line[counts], " &", 2) == 0) {
                counts += 2;

                std::string anchor {};
                do {
                    anchor += event_line[counts++];
                } while (event_line[counts] != ' ' && event_line[counts] != '\n');

                p_current->add_anchor_name(std::move(anchor));

                counts += anchor.size();
            }

            if (std::strncmp(&event_line[counts], " <", 2) == 0) {
                counts += 2;

                std::string tag {};
                fkyaml::node::node_t type {fkyaml::node::node_t::MAPPING};
                std::size_t tag_size {0};
                scan_tag(event_line.substr(counts), tag, type, tag_size);
                if (type != fkyaml::node::node_t::MAPPING) {
                    throw std::runtime_error("non-mapping type is detected from the tag.");
                }
                counts += tag_size;

                p_current->add_tag_name(std::move(tag));
            }

            continue;
        }

        if (event_tag == "-MAP") {
            continue;
        }

        if (event_tag == "+SEQ") {
            continue;
        }

        if (event_tag == "-SEQ") {
            continue;
        }

        if (event_tag == "=VAL") {
            counts += 5; // "=VAL"(4) + space(1)

            fkyaml::node::node_t type {fkyaml::node::node_t::STRING};
            std::string tag {};
            if (event_line[counts] == '<') {
                std::size_t tag_size {0};
                scan_tag(event_line.substr(counts), tag, type, tag_size);
                counts += tag_size;

                if (counts < event_line.size() - 1) {
                    // skip the delimiter (a white space)
                    counts++;
                }
            }

            if (event_line[counts] == ':') {
                counts += 1;
            }

            std::size_t line_end_pos = event_line.find_first_of('\n', counts);
            std::string scalar = event_line.substr(counts, line_end_pos - counts);

            switch (type) {
            case fkyaml::node::node_t::NULL_OBJECT:
                *p_current = fkyaml::node();
                break;
            case fkyaml::node::node_t::BOOLEAN:
                if (scalar == "true" || scalar == "True" || scalar == "TRUE") {
                    *p_current = fkyaml::node(true);
                }
                else if (scalar == "false" || scalar == "False" || scalar == "FALSE") {
                    *p_current = fkyaml::node(false);
                }
                else {
                    throw std::runtime_error("invalid boolean value string.");
                }
                break;
            case fkyaml::node::node_t::INTEGER: {
                int integer = std::stoi(scalar);
                *p_current = fkyaml::node(integer);
                break;
            }
            case fkyaml::node::node_t::FLOAT_NUMBER:
                if (scalar == ".inf" || scalar == ".Inf" || scalar == ".INF") {
                    *p_current = fkyaml::node(std::numeric_limits<double>::infinity());
                }
                else if (scalar == "-.inf" || scalar == "-.Inf" || scalar == "-.INF") {
                    *p_current = fkyaml::node(-1.0 * std::numeric_limits<double>::infinity());
                }
                else if (scalar == ".nan" || scalar == ".NaN" || scalar == ".NAN") {
                    *p_current = fkyaml::node(std::nan(""));
                }
                else {
                    *p_current = fkyaml::node(std::stod(scalar));
                }
                break;
            case fkyaml::node::node_t::STRING:
                if (scalar == "null" || scalar == "Null" || scalar == "NULL" || scalar == "~") {
                    *p_current = fkyaml::node();
                }
                else if (scalar == "true" || scalar == "True" || scalar == "TRUE") {
                    *p_current = fkyaml::node(true);
                }
                else if (scalar == "false" || scalar == "False" || scalar == "FALSE") {
                    *p_current = fkyaml::node(false);
                }
                else if (scalar == ".inf" || scalar == ".Inf" || scalar == ".INF") {
                    *p_current = fkyaml::node(std::numeric_limits<double>::infinity());
                }
                else if (scalar == "-.inf" || scalar == "-.Inf" || scalar == "-.INF") {
                    *p_current = fkyaml::node(-1.0 * std::numeric_limits<double>::infinity());
                }
                else if (scalar == ".nan" || scalar == ".NaN" || scalar == ".NAN") {
                    *p_current = fkyaml::node(std::nan(""));
                }
                else {
                    *p_current = fkyaml::node(scalar);
                }
                break;
            default:
                break;
            }

            continue;
        }
    }

    return root;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Invalid command line arguments.\n";
        std::cout.flush();
        return TEST_RESULT_NG;
    }

    std::string test_dir(argv[TEST_DIR_PREFIX_INDEX]);
    std::cout << "test dir: " << test_dir << "\n";

    std::string test_data_path(test_dir);
    std::string test_mode_str(argv[TEST_MODE_INDEX]);
    if (test_mode_str == "0") {
        test_data_path += "/in.yaml";
    }
    else if (test_mode_str == "1") {
        test_data_path += "/in.json";
    }
    else {
        std::cout << "Invalid test mode. test_mode: " << test_mode_str << "\n";
        std::cout.flush();
        return TEST_RESULT_NG;
    }

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

    // fkyaml::node expected;
    // if (is_valid) {
    //     std::ifstream yaml_event_file(test_dir + "/test.event");
    //     if (!yaml_event_file) {
    //         std::cout << "Failed to open yaml event file. path: " << test_dir << "/test.event\n";
    //         std::cout.flush();
    //         return TEST_RESULT_NG;
    //     }

    //     expected = parse_events(yaml_event_file);
    // }

    int result = TEST_RESULT_OK;
    try {
        fkyaml::node actual_from_yaml = fkyaml::node::deserialize_docs(test_data_file);

        if (!is_valid) {
            std::cout << "[YAML] Parsing invalid test data succeeded mistakenly.\n";
            result = TEST_RESULT_NG;
        }

        // if (actual_from_yaml != expected)
        // {
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
