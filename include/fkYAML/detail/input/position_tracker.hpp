///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_INPUT_POSITION_TRACKER_HPP_
#define FK_YAML_DETAIL_INPUT_POSITION_TRACKER_HPP_

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/input_adapter_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief A position tracker of the target buffer.
class position_tracker {
public:
    void set_target_buffer(const std::string& buffer) {
        m_begin = m_last = buffer.begin();
        m_end = buffer.end();
    }

    /// @brief Update the set of the current position informations.
    /// @note This function doesn't support cases where cur_pos has moved backward from the last call.
    /// @param cur_pos The iterator to the current element of the buffer.
    void update_position(std::string::const_iterator cur_pos) {
        m_cur_pos = static_cast<uint32_t>(std::distance(m_begin, cur_pos));
        m_lines_read += static_cast<uint32_t>(std::count(m_last, cur_pos, '\n'));
        m_last = cur_pos;

        if (m_lines_read == 0) {
            m_cur_pos_in_line = m_cur_pos;
            return;
        }

        uint32_t count = 0;
        while (--cur_pos != m_begin) {
            if (*cur_pos == '\n') {
                break;
            }
            count++;
        }
        m_cur_pos_in_line = count;
    }

    uint32_t get_cur_pos() const noexcept {
        return m_cur_pos;
    }

    /// @brief Get the current position in the current line.
    /// @return uint32_t The current position in the current line.
    uint32_t get_cur_pos_in_line() const noexcept {
        return m_cur_pos_in_line;
    }

    /// @brief Get the number of lines which have already been read.
    /// @return uint32_t The number of lines which have already been read.
    uint32_t get_lines_read() const noexcept {
        return m_lines_read;
    }

private:
    /// The iterator to the beginning element in the target buffer.
    std::string::const_iterator m_begin {};
    /// The iterator to the past-the-end element in the target buffer.
    std::string::const_iterator m_end {};
    /// The iterator to the last updated element in the target buffer.
    std::string::const_iterator m_last {};
    /// The current position from the beginning of an input buffer.
    uint32_t m_cur_pos {0};
    /// The current position in the current line.
    uint32_t m_cur_pos_in_line {0};
    /// The number of lines which have already been read.
    uint32_t m_lines_read {0};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_POSITION_TRACKER_HPP_ */
