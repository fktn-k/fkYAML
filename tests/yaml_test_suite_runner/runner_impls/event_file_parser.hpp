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

    bool empty() const noexcept {
        return m_event_queue.empty();
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

        std::size_t pos = 5; // Start after "+SEQ" or "+MAP"
        std::vector<event_param> params {};

        parse_collection_style_param(line, pos, params);
        parse_anchor_param(line, pos, params);
        parse_tag_param(line, pos, params);

        return params;
    }

    void parse_collection_style_param(const std::string& line, std::size_t& pos, std::vector<event_param>& params) {
        skip_spaces(line, pos);
        if (pos < line.size() && (line.rfind("{}", pos) == pos || line.rfind("[]", pos) == pos)) {
            params.emplace_back(event_param_type::STYLE, std::string(&line[pos], 2));
            pos += 2;
        }
    }

    std::vector<event_param> parse_scalar_params(const std::string& line) {
        if (line.size() < 6) {
            throw std::runtime_error("Malformed SCALAR event line: " + line);
        }

        std::size_t pos = 5; // Start after "=VAL"
        std::vector<event_param> params {};

        parse_anchor_param(line, pos, params);
        parse_tag_param(line, pos, params);
        parse_value_param(line, pos, params);

        return params; // Replace with actual parsing logic
    }

    std::vector<event_param> parse_alias_params(const std::string& line) {
        if (line.size() < 6) {
            throw std::runtime_error("Malformed ALIAS event line: " + line);
        }

        std::size_t pos = 5; // Start after "=ALI"
        std::vector<event_param> params {};

        parse_alias_param(line, pos, params);

        return params;
    }

    void skip_spaces(const std::string& line, std::size_t& pos) {
        while (pos < line.size() && line[pos] == ' ') {
            ++pos;
        }
    }

    void parse_anchor_param(const std::string& line, std::size_t& pos, std::vector<event_param>& params) {
        skip_spaces(line, pos);
        if (pos < line.size() && line[pos] == '&') {
            std::size_t anchor_start_pos = pos;
            std::size_t anchor_end_pos = line.find(' ', anchor_start_pos);
            std::string anchor_value = (anchor_end_pos == std::string::npos)
                                           ? line.substr(anchor_start_pos + 1)
                                           : line.substr(anchor_start_pos + 1, anchor_end_pos - anchor_start_pos - 1);
            params.emplace_back(event_param_type::ANCHOR, std::move(anchor_value));
            pos = (anchor_end_pos == std::string::npos) ? line.size() : anchor_end_pos;
        }
    }

    void parse_tag_param(const std::string& line, std::size_t& pos, std::vector<event_param>& params) {
        skip_spaces(line, pos);
        if (pos < line.size() && line[pos] == '<') {
            std::size_t tag_start_pos = pos;
            std::size_t tag_end_pos = line.find('>', tag_start_pos + 1);
            if (tag_end_pos == std::string::npos) {
                throw std::runtime_error("Malformed tag parameter in event line: " + line);
            }
            std::string tag_value = line.substr(tag_start_pos + 1, tag_end_pos - (tag_start_pos + 1));
            params.emplace_back(event_param_type::TAG, std::move(tag_value));
            pos = tag_end_pos + 1;
        }
    }

    void parse_value_param(const std::string& line, std::size_t& pos, std::vector<event_param>& params) {
        skip_spaces(line, pos);
        if (pos >= line.size()) {
            throw std::runtime_error("Malformed value parameter in event line: " + line);
        }
        switch (line[pos]) {
        case ':':
        case '\'':
        case '"':
        case '|':
        case '>':
            params.emplace_back(event_param_type::STYLE, std::string(&line[pos], 1));
            break;
        default:
            throw std::runtime_error("Malformed value parameter in event line: " + line);
        }

        std::string value;
        for (std::size_t i = pos + 1; i < line.size(); ++i) {
            if (line[i] == '\\') {
                if (i + 1 >= line.size()) {
                    throw std::runtime_error("Malformed escape sequence in value parameter: " + line);
                }
                ++i;

                switch (line[i]) {
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
                value += line[i];
            }
        }
        params.emplace_back(event_param_type::VALUE, std::move(value));
    }

    void parse_alias_param(const std::string& line, std::size_t& pos, std::vector<event_param>& params) {
        skip_spaces(line, pos);
        if (pos < line.size() && line[pos] == '*') {
            std::string alias_value = line.substr(pos + 1);
            params.emplace_back(event_param_type::ALIAS, std::move(alias_value));
        }
    }

    std::deque<event> m_event_queue {};
};

} // namespace yaml_test_suite_runner

#endif // FK_YAML_TEST_YAML_TEST_SUITE_RUNNER_EVENT_FILE_PARSER_HPP
