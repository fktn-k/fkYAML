//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_EVENT_FILE_PARSER_HPP
#define FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_EVENT_FILE_PARSER_HPP

#include <cassert>
#include <deque>
#include <fstream>
#include <string>

#include "event.hpp"

namespace yaml_test_suite_runner {

class event_file_parser {
public:
    event_file_parser(std::ifstream& file) {
        prepare_event_queue(file);
    }

    event get_next_event() {
        if (m_event_queue.empty()) {
            throw std::runtime_error("No more events to read.");
        }
        event next_event = std::move(m_event_queue.front());
        m_event_queue.pop_front();
        return next_event;
    }

private:
    void prepare_event_queue(std::ifstream& file) {
        assert(file.is_open());

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) {
                continue; // Skip empty lines
            }

            event_type type = parse_event_type(line);
            switch (type) {
            case event_type::STREAM_START:
            case event_type::STREAM_END:
            case event_type::DOCUMENT_START:
            case event_type::DOCUMENT_END:
            case event_type::SEQUENCE_END:
            case event_type::MAPPING_END:
                m_event_queue.emplace_back(type);
                break;
            case event_type::SEQUENCE_START:
            case event_type::MAPPING_START: {
                auto params = parse_collection_start_params(line);
                m_event_queue.emplace_back(type, std::move(params));
                break;
            }
            case event_type::SCALAR: {
                auto params = parse_scalar_params(line);
                m_event_queue.emplace_back(type, std::move(params));
                break;
            }
            case event_type::ALIAS: {
                auto params = parse_alias_params(line);
                m_event_queue.emplace_back(type, std::move(params));
                break;
            }
            }
        }
    }

    event_type parse_event_type(const std::string& line) {
        if (line.rfind("+STR", 0) == 0) {
            return event_type::STREAM_START;
        }
        if (line.rfind("-STR", 0) == 0) {
            return event_type::STREAM_END;
        }
        if (line.rfind("+DOC", 0) == 0) {
            return event_type::DOCUMENT_START;
        }
        if (line.rfind("-DOC", 0) == 0) {
            return event_type::DOCUMENT_END;
        }
        if (line.rfind("+SEQ", 0) == 0) {
            return event_type::SEQUENCE_START;
        }
        if (line.rfind("-SEQ", 0) == 0) {
            return event_type::SEQUENCE_END;
        }
        if (line.rfind("+MAP", 0) == 0) {
            return event_type::MAPPING_START;
        }
        if (line.rfind("-MAP", 0) == 0) {
            return event_type::MAPPING_END;
        }
        if (line.rfind("=VAL", 0) == 0) {
            return event_type::SCALAR;
        }
        if (line.rfind("=ALI", 0) == 0) {
            return event_type::ALIAS;
        }
        throw std::runtime_error("Unknown event type: " + line);
    }

    std::vector<event_param> parse_collection_start_params(const std::string& line) {
        if (line.size() < 6) {
            return {}; // No parameters
        }

        std::size_t pos = 6; // Start after "+SEQ " or "+MAP "
        std::vector<event_param> params {};

        parse_anchor_param(line, pos, params);
        parse_tag_param(line, pos, params);

        return params;
    }

    std::vector<event_param> parse_scalar_params(const std::string& line) {
        if (line.size() < 6) {
            throw std::runtime_error("Malformed SCALAR event line: " + line);
        }

        std::size_t pos = 6; // Start after "=VAL "
        std::vector<event_param> params {};

        parse_anchor_param(line, pos, params);
        if (pos == std::string::npos) {
            throw std::runtime_error("Malformed SCALAR event line: " + line);
        }
        parse_tag_param(line, pos, params);
        parse_value_param(line, pos, params);

        return params; // Replace with actual parsing logic
    }

    std::vector<event_param> parse_alias_params(const std::string& line) {
        if (line.size() < 6) {
            throw std::runtime_error("Malformed ALIAS event line: " + line);
        }

        std::size_t pos = 6; // Start after "=ALI "
        std::vector<event_param> params {};

        parse_alias_param(line, pos, params);

        return params;
    }

    void parse_anchor_param(const std::string& line, std::size_t& pos, std::vector<event_param>& params) {
        std::size_t anchor_start_pos = line.find('&', pos);
        if (anchor_start_pos != std::string::npos) {
            std::size_t anchor_end_pos = line.find(' ', anchor_start_pos);
            std::string anchor_value = line.substr(anchor_start_pos + 1);
            params.emplace_back(event_param_type::ANCHOR, std::move(anchor_value));
            pos = anchor_end_pos; // Move past the "&anchor" indicator
        }
    }

    void parse_tag_param(const std::string& line, std::size_t& pos, std::vector<event_param>& params) {
        std::size_t tag_start_pos = line.find('<', pos);
        if (tag_start_pos != std::string::npos) {
            std::size_t tag_end_pos = line.find('>', tag_start_pos + 1);
            if (tag_end_pos == std::string::npos) {
                throw std::runtime_error("Malformed tag parameter in event line: " + line);
            }
            std::string tag_value = line.substr(tag_start_pos + 1, tag_end_pos - (tag_start_pos + 1));
            params.emplace_back(event_param_type::TAG, std::move(tag_value));
            pos = tag_end_pos + 1; // Move past the "<tag:...>" indicator
        }
    }

    void parse_value_param(const std::string& line, std::size_t& pos, std::vector<event_param>& params) {
        std::size_t value_start_pos = line.find_first_of(":\'\"|>", pos);
        if (value_start_pos == std::string::npos) {
            throw std::runtime_error("Malformed value parameter in event line: " + line);
        }
        std::string tmp_value = line.substr(value_start_pos);
        std::string value;
        for (std::size_t i = 0; i < tmp_value.size(); ++i) {
            if (tmp_value[i] == '\\') {
                if (i + 1 >= tmp_value.size()) {
                    throw std::runtime_error("Malformed escape sequence in value parameter: " + line);
                }
                ++i;

                switch (tmp_value[i]) {
                case 'b':
                    value += '\b';
                    break;
                case 'n':
                    value += '\n';
                    break;
                case 'r':
                    value += '\r';
                    break;
                case 't':
                    value += '\t';
                    break;
                case '\\':
                    value += '\\';
                    break;
                default:
                    throw std::runtime_error("Unknown escape sequence in value parameter: " + line);
                }
            }
            else {
                value += tmp_value[i];
            }
        }
        params.emplace_back(event_param_type::VALUE, std::move(value));
    }

    void parse_alias_param(const std::string& line, std::size_t& pos, std::vector<event_param>& params) {
        std::size_t alias_start_pos = line.find('*', pos);
        if (alias_start_pos != std::string::npos) {
            std::string alias_value = line.substr(alias_start_pos + 1);
            params.emplace_back(event_param_type::ANCHOR, std::move(alias_value));
        }
    }

    std::deque<event> m_event_queue {};
};

} // namespace yaml_test_suite_runner

#endif // FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_EVENT_FILE_PARSER_HPP
