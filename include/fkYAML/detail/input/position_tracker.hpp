//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_INPUT_POSITION_TRACKER_HPP
#define FK_YAML_DETAIL_INPUT_POSITION_TRACKER_HPP

#include <algorithm>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/str_view.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief A position tracker of the target buffer.
class position_tracker {
public:
    void set_target_buffer(str_view buffer) noexcept {
        m_begin = m_last = buffer.begin();
        m_end = buffer.end();
    }

    /// @brief Update the set of the current position information.
    /// @note This function doesn't support cases where cur_pos has moved backward from the last call.
    /// @param cur_pos The iterator to the current element of the buffer.
    void update_position(const char* p_current) {
        const auto diff = static_cast<uint32_t>(p_current - m_last);
        if (diff == 0) {
            return;
        }

        m_cur_pos += diff;
        const uint32_t prev_lines_read = m_lines_read;
        m_lines_read += static_cast<uint32_t>(std::count(m_last, p_current, '\n'));
        m_last = p_current;

        if (prev_lines_read == m_lines_read) {
            m_cur_pos_in_line += diff;
            return;
        }

        uint32_t count = 0;
        const char* p_begin = m_begin;
        while (--p_current != p_begin) {
            if (*p_current == '\n') {
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
    const char* m_begin {};
    /// The iterator to the past-the-end element in the target buffer.
    const char* m_end {};
    /// The iterator to the last updated element in the target buffer.
    const char* m_last {};
    /// The current position from the beginning of an input buffer.
    uint32_t m_cur_pos {0};
    /// The current position in the current line.
    uint32_t m_cur_pos_in_line {0};
    /// The number of lines which have already been read.
    uint32_t m_lines_read {0};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_POSITION_TRACKER_HPP */
