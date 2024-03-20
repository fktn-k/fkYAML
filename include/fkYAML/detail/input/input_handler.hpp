///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_INPUT_INPUT_HANDLER_HPP_
#define FK_YAML_DETAIL_INPUT_INPUT_HANDLER_HPP_

#include <string>
#include <utility>
#include <vector>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/input_adapter_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>

/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief An input buffer handler.
/// @tparam InputAdapterType The type of the input adapter.
template <typename InputAdapterType, enable_if_t<is_input_adapter<InputAdapterType>::value, int> = 0>
class input_handler
{
public:
    /// The type of character traits of the input buffer.
    using char_traits_type = std::char_traits<typename InputAdapterType::char_type>;
    /// The type of characters of the input buffer.
    using char_type = typename char_traits_type::char_type;
    /// The type of integers for the input buffer.
    using int_type = typename char_traits_type::int_type;
    /// The type of strings of the input buffer.
    using string_type = std::basic_string<char_type>;

private:
    /// @brief A set of information on the current position in an input buffer.
    struct position
    {
        /// The current position from the beginning of an input buffer.
        std::size_t cur_pos {0};
        /// The current position in the current line.
        std::size_t cur_pos_in_line {0};
        /// The number of lines which have already been read.
        std::size_t lines_read {0};
    };

public:
    /// @brief Construct a new input_handler object.
    /// @param input_adapter An input adapter object
    explicit input_handler(InputAdapterType&& input_adapter)
    {
        int_type ch = s_end_of_input;
        while ((ch = input_adapter.get_character()) != s_end_of_input)
        {
            m_buffer.push_back(char_traits_type::to_char_type(ch));
            m_buffer_size++;
        }
    }

    /// @brief Get the character at the current position.
    /// @return int_type A character or EOF.
    int_type get_current() const noexcept
    {
        if (m_position.cur_pos >= m_buffer_size)
        {
            return s_end_of_input;
        }
        return char_traits_type::to_int_type(m_buffer[m_position.cur_pos]);
    }

    /// @brief Get the character at next position.
    /// @return int_type A character or EOF.
    int_type get_next()
    {
        // if all the input has already been consumed, return the EOF.
        if (m_position.cur_pos >= m_buffer_size)
        {
            return s_end_of_input;
        }

        int_type ret = char_traits_type::to_int_type(m_buffer[m_position.cur_pos++]);
        m_position.cur_pos_in_line++;

        if (m_buffer[m_position.cur_pos - 1] == '\n')
        {
            m_position.cur_pos_in_line = 0;
            ++m_position.lines_read;
        }

        return ret;
    }

    /// @brief Get the characters in the given range.
    /// @param length The length of characters retrieved from the current position.
    /// @param str A string which will contain the resulting characters.
    /// @return int_type 0 (for success) or EOF (for error).
    int_type get_range(std::size_t length, string_type& str)
    {
        str.clear();

        int_type ch = get_current();
        if (ch == s_end_of_input)
        {
            return s_end_of_input;
        }

        str += char_traits_type::to_char_type(ch);

        for (std::size_t i = 1; i < length; i++)
        {
            ch = get_next();
            if (ch == s_end_of_input)
            {
                for (std::size_t j = i; j > 0; j--)
                {
                    unget();
                }
                str.clear();
                return ch;
            }
            str += char_traits_type::to_char_type(ch);
        }

        return 0;
    }

    /// @brief Move backward the current position.
    void unget()
    {
        if (m_position.cur_pos > 0)
        {
            // just move back the cursor. (no action for adapter)
            --m_position.cur_pos;
            --m_position.cur_pos_in_line;
            if (m_buffer[m_position.cur_pos] == '\n')
            {
                --m_position.lines_read;
                m_position.cur_pos_in_line = 0;
                if (m_position.cur_pos > 0)
                {
                    for (std::size_t i = m_position.cur_pos - 1; m_buffer[i] != '\n'; i--)
                    {
                        if (i == 0)
                        {
                            m_position.cur_pos_in_line = m_position.cur_pos;
                            break;
                        }
                        ++m_position.cur_pos_in_line;
                    }
                }
            }
        }
    }

    /// @brief Move backward the current position to the given range.
    /// @param length The length of moving backward.
    void unget_range(std::size_t length)
    {
        for (std::size_t i = 0; i < length; i++)
        {
            unget();
        }
    }

    /// @brief Check if the next character is the expected one.
    /// @param expected An expected next character.
    /// @return true The next character is the expected one.
    /// @return false The next character is not the expected one.
    bool test_next_char(char_type expected)
    {
        // already at the end of the input.
        if (get_current() == s_end_of_input)
        {
            return false;
        }

        if (m_position.cur_pos == m_buffer_size - 1)
        {
            return false;
        }

        return char_traits_type::eq(char_traits_type::to_char_type(m_buffer[m_position.cur_pos + 1]), expected);
    }

    /// @brief Get the current position in the current line.
    /// @return std::size_t The current position in the current line.
    std::size_t get_cur_pos_in_line() const noexcept
    {
        return m_position.cur_pos_in_line;
    }

    /// @brief Get the number of lines which have already been read.
    /// @return std::size_t The number of lines which have already been read.
    std::size_t get_lines_read() const noexcept
    {
        return m_position.lines_read;
    }

private:
    /// The value of EOF for the target character type.
    static constexpr int_type s_end_of_input = char_traits_type::eof();

    /// Cached characters retrieved from an input adapter object.
    std::string m_buffer {};
    /// The size of the buffer.
    std::size_t m_buffer_size {0};
    /// The current position in an input buffer.
    position m_position {};
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_INPUT_HANDLER_HPP_ */
