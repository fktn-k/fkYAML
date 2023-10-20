/**
 *  _______   __ __   __  _____   __  __  __
 * |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
 * |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.1.2
 * |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
 *
 * SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
 * SPDX-License-Identifier: MIT
 *
 * @file
 */

#ifndef FK_YAML_DETAIL_INPUT_INPUT_HANDLER_HPP_
#define FK_YAML_DETAIL_INPUT_INPUT_HANDLER_HPP_

#include <string>
#include <utility>
#include <vector>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/input_adapter_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

/**
 * @namespace detail
 * @brief namespace for internal implementations of fkYAML library.
 */
namespace detail
{

/**
 * @brief An input buffer handler.
 *
 * @tparam InputAdapterType The type of the input adapter.
 */
template <typename InputAdapterType, enable_if_t<is_input_adapter<InputAdapterType>::value, int> = 0>
class input_handler
{
public:
    //!< The type of character traits of the input buffer.
    using char_traits_type = std::char_traits<typename InputAdapterType::char_type>;
    //!< The type of characters of the input buffer.
    using char_type = typename char_traits_type::char_type;
    //!< The type of integers for the input buffer.
    using int_type = typename char_traits_type::int_type;
    //!< The type of strings of the input buffer.
    using string_type = std::basic_string<char_type>;

    /**
     * @brief Construct a new input_handler object.
     *
     * @param input_adapter An input adapter object
     */
    explicit input_handler(InputAdapterType&& input_adapter)
        : m_input_adapter(std::move(input_adapter))
    {
        get_next();
        m_cur_pos = 0;
    }

    /**
     * @brief Get the character at the current position.
     *
     * @return int_type A character or EOF.
     */
    int_type get_current()
    {
        return m_cache[m_cur_pos];
    }

    /**
     * @brief Get the character at next position.
     *
     * @return int_type A character or EOF.
     */
    int_type get_next()
    {
        // if already cached, return the cached value.
        if (m_cur_pos + 1 < m_cache.size())
        {
            return m_cache[++m_cur_pos];
        }

        int_type ret = m_input_adapter.get_character();
        if (ret != end_of_input || m_cache[m_cur_pos] != end_of_input)
        {
            // cache the return value for possible later use.
            m_cache.push_back(ret);
            ++m_cur_pos;
        }
        return ret;
    }

    /**
     * @brief Get the characters in the given range.
     *
     * @param length The length of characters retrieved from the current position.
     * @param str A string which will contain the resulting characters.
     * @return int_type 0 (for success) or EOF (for error).
     */
    int_type get_range(size_t length, string_type& str)
    {
        str.clear();

        if (get_current() == end_of_input)
        {
            return end_of_input;
        }

        str += char_traits_type::to_char_type(get_current());

        for (size_t i = 1; i < length; i++)
        {
            if (get_next() == end_of_input)
            {
                m_cur_pos -= i;
                str.clear();
                return end_of_input;
            }
            str += char_traits_type::to_char_type(get_current());
        }

        return 0;
    }

    /**
     * @brief Move backward the current position.
     */
    void unget()
    {
        if (m_cur_pos > 0)
        {
            // just move back the cursor. (no action for adapter)
            --m_cur_pos;
        }
    }

    /**
     * @brief Move backward the current position to the given range.
     *
     * @param length The length of moving backward.
     */
    void unget_range(size_t length)
    {
        m_cur_pos = (m_cur_pos > length) ? m_cur_pos - length : 0;
    }

    /**
     * @brief Check if the next character is the expected one.
     *
     * @param expected An expected next character.
     * @return true The next character is the expected one.
     * @return false The next character is not the expected one.
     */
    bool test_next_char(char_type expected)
    {
        if (get_current() == end_of_input)
        {
            return false;
        }

        int_type next = get_next();
        if (next == end_of_input)
        {
            unget();
            return false;
        }

        bool ret = char_traits_type::eq(char_traits_type::to_char_type(next), expected);
        --m_cur_pos;
        return ret;
    }

private:
    //!< The value of EOF for the target character type.
    static constexpr int_type end_of_input = char_traits_type::eof();

    //!< An input adapter object.
    InputAdapterType m_input_adapter {};
    //!< Cached characters retrieved from an input adapter object.
    std::vector<int_type> m_cache {};
    //!< The current position in an input buffer.
    size_t m_cur_pos {0};
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_INPUT_HANDLER_HPP_ */