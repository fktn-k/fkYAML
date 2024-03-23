//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
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


void scan_event_tag(const std::string& input, std::string& event_tag, std::size_t& read_counts)
{
    std::size_t event_tag_end_pos = input.find_first_not_of(' ');
    // allows the return value to be `npos` for "+STR", "-MAP" and so forth.
    event_tag = input.substr(0, event_tag_end_pos);
    read_counts = event_tag_end_pos;
}

void scan_tag(const std::string& input, std::string& tag, fkyaml::node::node_t& type, std::size_t& read_counts)
{
    std::size_t tag_end_pos = input.find_first_of('>');
    if (tag_end_pos == std::string::npos)
    {
        throw std::runtime_error("failed to find the end of tag.");
    }
    tag = input.substr(0, tag_end_pos + 1);
    read_counts = tag_end_pos + 1;

    if (input == "<tag:yaml.org,2002:seq>")
    {
        read_counts = 23;
        type = fkyaml::node::node_t::SEQUENCE;
    }
    else if (input == "<tag:yaml.org,2002:map>")
    {
        read_counts = 23;
        type = fkyaml::node::node_t::MAPPING;
    }
    else if (input == "<tag:yaml.org,2002:null>")
    {
        read_counts = 24;
        type = fkyaml::node::node_t::NULL_OBJECT;
    }
    else if (input == "<tag:yaml.org,2002:bool>")
    {
        read_counts = 24;
        type = fkyaml::node::node_t::BOOLEAN;
    }
    else if (input == "<tag:yaml.org,2002:int>")
    {
        read_counts = 23;
        type = fkyaml::node::node_t::INTEGER;
    }
    else if (input == "<tag:yaml.org,2002:float>")
    {
        read_counts = 25;
        type = fkyaml::node::node_t::FLOAT_NUMBER;
    }
    else if (input == "<tag:yaml.org,2002:str>")
    {
        read_counts = 23;
        type = fkyaml::node::node_t::STRING;
    }
    else if (input == "<tag:yaml.org,2002:binary>")
    {
        read_counts = 26;
        // FIXME: modify the type when "binary" gets supported.
        type = fkyaml::node::node_t::STRING;
    }
    else if (input == "<tag:yaml.org,2002:omap>")
    {
        read_counts = 24;
        // FIXME: modify the type when "omap" gets supported.
        type = fkyaml::node::node_t::MAPPING;
    }
    else if (input == "<tag:yaml.org,2002:set>")
    {
        read_counts = 23;
        // FIXME: modify the type when "set" gets supported.
        type = fkyaml::node::node_t::MAPPING;
    }
    else
    {
        // user-defined tags are used for mappings in the test suite.
        type = fkyaml::node::node_t::MAPPING;
    }
}

fkyaml::node parse_events(std::istream& is)
{
    fkyaml::node root {};
    fkyaml::node* p_current = &root;

    std::string event_line {};
    // test.event files end with a trailing empty line.
    while (std::getline(is, event_line))
    {
        std::size_t counts = 0;
        std::string event_tag {};
        scan_event_tag(event_line, event_tag, counts);

        if (event_tag == "+STR")
        {
            continue;
        }

        if (event_tag == "-STR")
        {
            continue;
        }

        if (event_tag == "+DOC")
        {
            continue;
        }

        if (event_tag == "-DOC")
        {
            continue;
        }

        if (event_tag == "+MAP")
        {
            counts += 4;
            *p_current = fkyaml::node::mapping();

            if (event_line[counts] == '\n')
            {
                counts += 1;
                continue;
            }

            if (std::strncmp(&event_line[counts], " {}", 3) == 0)
            {
                counts += 3;
            }

            if (std::strncmp(&event_line[counts], " &", 2) == 0)
            {
                counts += 2;

                std::string anchor {};
                do
                {
                    anchor += event_line[counts++];
                } while (event_line[counts] != ' ' && event_line[counts] != '\n');

                p_current->add_anchor_name(std::move(anchor));

                counts += anchor.size();
            }

            if (std::strncmp(&event_line[counts], " <", 2) == 0)
            {
                counts += 2;

                std::string tag {};
                fkyaml::node::node_t type {fkyaml::node::node_t::MAPPING};
                std::size_t tag_size {0};
                scan_tag(event_line.substr(counts), tag, type, tag_size);
                if (type != fkyaml::node::node_t::MAPPING)
                {
                    throw std::runtime_error("non-mapping type is detected from the tag.");
                }
                counts += tag_size;

                // TODO: set the tag name to the current node.
            }

            continue;
        }

        if (event_tag == "-MAP")
        {
            continue;
        }

        if (event_tag == "+SEQ")
        {
            continue;
        }

        if (event_tag == "-SEQ")
        {
            continue;
        }

        if (event_tag == "=VAL")
        {
            counts += 5; // "=VAL"(4) + space(1)

            fkyaml::node::node_t type {fkyaml::node::node_t::STRING};
            std::string tag {};
            if (event_line[counts] == '<')
            {
                std::size_t tag_size {0};
                scan_tag(event_line.substr(counts), tag, type, tag_size);
                counts += tag_size;

                if (counts < event_line.size() - 1)
                {
                    // skip the delimiter (a white space)
                    counts++;
                }
            }

            if (std::strncmp(&event_line[counts], ":", 1) == 0)
            {
                counts += 1;
            }

            std::size_t line_end_pos = event_line.find_first_of('\n', counts);
            std::string scalar = event_line.substr(counts, line_end_pos - counts);

            // TODO: implement this.

            continue;
        }
    }

    return root;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Invalid command line arguments.\n";
        return TEST_RESULT_NG;
    }

    std::string test_dir(argv[TEST_DIR_PREFIX_INDEX]);
    std::cout << "test dir: " << test_dir << "\n";

    std::ifstream label_file(test_dir + "/===");
    if (label_file)
    {
        std::cout << "label: ";
        std::string label;
        std::getline(label_file, label);
        std::cout << label << "\n";
    }

    std::ifstream yaml_data_file(test_dir + "/in.yaml");
    if (!yaml_data_file)
    {
        std::cout << "Failed to open yaml data file. path: " << test_dir << "/in.yaml\n";
        return TEST_RESULT_NG;
    }

    std::ifstream error_file(test_dir + "/error");
    bool is_valid = !error_file.is_open();

    std::ifstream json_data_file(test_dir + "/in.json");

    int result = TEST_RESULT_OK;
    // fkyaml::node expected;

    try
    {
        fkyaml::node actual_from_yaml;
        yaml_data_file >> actual_from_yaml;
        // if (actual_from_yaml != expected)
        // {
        //     result = TEST_RESULT_NG;
        // }

        if (json_data_file)
        {
            // fkyaml::node actual_from_json;
            // json_data_file >>actual_from_json;

            // if (actual_from_json != expected)
            // {
            //     result = TEST_RESULT_NG;
            // }
        }
    }
    catch (const fkyaml::exception& e)
    {
        if (is_valid)
        {
            std::cout << "fkYAML error: " << e.what() << "\n";
            result = TEST_RESULT_NG;
        }
    }
    catch (const std::exception& e)
    {
        if (is_valid)
        {
            std::cout << "Unexpected error: " << e.what() << "\n";
            result = TEST_RESULT_NG;
        }
    }

    return result;
}
