//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.12
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_YAML_TEST_SUITE_RUNNER_STREAM_EVENT_SOURCE_HPP_
#define FK_YAML_YAML_TEST_SUITE_RUNNER_STREAM_EVENT_SOURCE_HPP_

#include <fstream>
#include <string>

#include <fkYAML/node.hpp>

class stream_event_source {
public:
    using str_view = fkyaml::detail::str_view;

    stream_event_source(const char* p_filename) noexcept {
        assert(p_filename != nullptr);
        read_file_contents(p_filename);

        assert(!m_buffer.empty());
        m_view = m_buffer;
    }

    ~stream_event_source() noexcept = default;

    str_view get_line() {
        std::size_t pos = m_view.find('\n');
        std::size_t rem_count = 0;
        if (pos == str_view::npos) {
            pos = m_view.size();
            rem_count = pos;
        }
        else {
            rem_count = pos + 1;
        }

        auto line = m_view.substr(0, pos);
        m_view.remove_prefix(rem_count);

        return line;
    }

private:
    void read_file_contents(const char* p_filename) {
        std::ifstream ifs(p_filename);
        assert(ifs.good());

        char tmp_buf[256] {};
        do {
            ifs.read(&tmp_buf[0], 256);
            std::size_t read_size = static_cast<std::size_t>(ifs.gcount());
            if (read_size == 0) {
                break;
            }

            m_buffer.append(&tmp_buf[0], &tmp_buf[0] + read_size);
        } while (!ifs.eof());
    }

    std::string m_buffer {};
    str_view m_view {};
};

#endif /* FK_YAML_YAML_TEST_SUITE_RUNNER_STREAM_EVENT_SOURCE_HPP_ */
