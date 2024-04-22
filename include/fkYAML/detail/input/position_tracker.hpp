///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.4
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
private:
    /// @brief A set of information on the current position in an input buffer.
    struct position {
        /// The current position from the beginning of an input buffer.
        std::size_t cur_pos {0};
        /// The current position in the current line.
        std::size_t cur_pos_in_line {0};
        /// The number of lines which have already been read.
        std::size_t lines_read {0};
    };

public:
    void set_target_buffer(const std::string& buffer) {
        m_begin = m_last = buffer.begin();
        m_end = buffer.end();
        m_position = position {};
    }

    /// @brief Update the set of the current position informations.
    /// @note This function doesn't support cases where cur_pos has moved backward from the last call.
    /// @param cur_pos The iterator to the current element of the buffer.
    void update_position(std::string::const_iterator cur_pos) {
        m_position.cur_pos = static_cast<std::size_t>(std::distance(m_begin, cur_pos));
        m_position.lines_read += std::count(m_last, cur_pos, '\n');
        m_last = cur_pos;

        if (m_position.lines_read == 0) {
            m_position.cur_pos_in_line = m_position.cur_pos;
            return;
        }

        std::size_t count = 0;
        while (--cur_pos != m_begin) {
            if (*cur_pos == '\n') {
                break;
            }
            count++;
        }
        m_position.cur_pos_in_line = count;
    }

    std::size_t get_cur_pos() const noexcept {
        return m_position.cur_pos;
    }

    /// @brief Get the current position in the current line.
    /// @return std::size_t The current position in the current line.
    std::size_t get_cur_pos_in_line() const noexcept {
        return m_position.cur_pos_in_line;
    }

    /// @brief Get the number of lines which have already been read.
    /// @return std::size_t The number of lines which have already been read.
    std::size_t get_lines_read() const noexcept {
        return m_position.lines_read;
    }

private:
    /// The iterator to the beginning element in the target buffer.
    std::string::const_iterator m_begin {};
    /// The iterator to the past-the-end element in the target buffer.
    std::string::const_iterator m_end {};
    /// The iterator to the last updated element in the target buffer.
    std::string::const_iterator m_last {};
    /// The current position in the target buffer.
    position m_position {};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_POSITION_TRACKER_HPP_ */
