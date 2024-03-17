///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_INPUT_INPUT_ADAPTER_HPP_
#define FK_YAML_DETAIL_INPUT_INPUT_ADAPTER_HPP_

#include <cstdio>
#include <cstring>
#include <istream>
#include <iterator>
#include <string>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/encodings/encode_detector.hpp>
#include <fkYAML/detail/encodings/encode_t.hpp>
#include <fkYAML/detail/encodings/utf8_encoding.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/exception.hpp>

/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

///////////////////////
//   input_adapter   //
///////////////////////

template <typename IterType, typename = void>
class iterator_input_adapter;

/// @brief An input adapter for iterators of type char.
/// @tparam IterType An iterator type.
template <typename IterType>
class iterator_input_adapter<
    IterType, enable_if_t<std::is_same<remove_cv_t<typename std::iterator_traits<IterType>::value_type>, char>::value>>
{
public:
    /// A type for characters used in this input adapter.
    using char_type = char;

    /// @brief Construct a new iterator_input_adapter object.
    iterator_input_adapter() = default;

    /// @brief Construct a new iterator_input_adapter object.
    /// @param begin The beginning of iteraters.
    /// @param end The end of iterators.
    /// @param encode_type The encoding type for this input adapter.
    iterator_input_adapter(IterType begin, IterType end, encode_t encode_type) noexcept
        : m_current(begin),
          m_end(end),
          m_encode_type(encode_type)
    {
    }

    // allow only move construct/assignment like other input adapters.
    iterator_input_adapter(const iterator_input_adapter&) = delete;
    iterator_input_adapter(iterator_input_adapter&& rhs) = default;
    iterator_input_adapter& operator=(const iterator_input_adapter&) = delete;
    iterator_input_adapter& operator=(iterator_input_adapter&&) = default;
    ~iterator_input_adapter() = default;

    /// @brief Get a character at the current position and move forward.
    /// @return std::char_traits<char_type>::int_type A character or EOF.
    typename std::char_traits<char_type>::int_type get_character()
    {
        typename std::char_traits<char_type>::int_type ret = 0;
        switch (m_encode_type)
        {
        case encode_t::UTF_8_N:
        case encode_t::UTF_8_BOM:
            ret = get_character_for_utf8();
            break;
        case encode_t::UTF_16BE_N:
        case encode_t::UTF_16BE_BOM:
        case encode_t::UTF_16LE_N:
        case encode_t::UTF_16LE_BOM:
            ret = get_character_for_utf16();
            break;
        case encode_t::UTF_32BE_N:
        case encode_t::UTF_32BE_BOM:
        case encode_t::UTF_32LE_N:
        case encode_t::UTF_32LE_BOM:
            ret = get_character_for_utf32();
            break;
        }
        return ret;
    }

private:
    /// @brief The concrete implementation of get_character() for UTF-8 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf8() noexcept
    {
        if (m_current != m_end)
        {
            auto ret = std::char_traits<char_type>::to_int_type(*m_current);
            ++m_current;
            return ret;
        }
        return std::char_traits<char_type>::eof();
    }

    /// @brief The concrete implementation of get_character() for UTF-16 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf16()
    {
        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            if (m_current == m_end)
            {
                if (m_encoded_buf_size == 0)
                {
                    return std::char_traits<char_type>::eof();
                }
            }

            while (m_current != m_end && m_encoded_buf_size < 2)
            {
                switch (m_encode_type)
                {
                case encode_t::UTF_16BE_N:
                case encode_t::UTF_16BE_BOM:
                    m_encoded_buffer[m_encoded_buf_size] = char16_t(uint8_t(*m_current) << 8);
                    ++m_current;
                    m_encoded_buffer[m_encoded_buf_size] |= char16_t(*m_current);
                    break;
                case encode_t::UTF_16LE_N:
                case encode_t::UTF_16LE_BOM: {
                    m_encoded_buffer[m_encoded_buf_size] = char16_t(*m_current);
                    ++m_current;
                    m_encoded_buffer[m_encoded_buf_size] |= char16_t(uint8_t(*m_current) << 8);
                    break;
                }
                default: // LCOV_EXCL_LINE
                    // should not come here.
                    break; // LCOV_EXCL_LINE
                }
                ++m_current;
                ++m_encoded_buf_size;
            }

            std::size_t consumed_size = 0;
            utf8_encoding::from_utf16(m_encoded_buffer, m_utf8_buffer, consumed_size, m_utf8_buf_size);

            if (consumed_size == 1)
            {
                m_encoded_buffer[0] = m_encoded_buffer[1];
                m_encoded_buffer[1] = 0;
            }
            m_encoded_buf_size -= consumed_size;

            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

    /// @brief The concrete implementation of get_character() for UTF-32 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf32()
    {
        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            if (m_current == m_end)
            {
                return std::char_traits<char_type>::eof();
            }

            char32_t utf32 = 0;
            switch (m_encode_type)
            {
            case encode_t::UTF_32BE_N:
            case encode_t::UTF_32BE_BOM:
                utf32 = char32_t(*m_current << 24);
                ++m_current;
                utf32 |= char32_t(*m_current << 16);
                ++m_current;
                utf32 |= char32_t(*m_current << 8);
                ++m_current;
                utf32 |= char32_t(*m_current);
                break;
            case encode_t::UTF_32LE_N:
            case encode_t::UTF_32LE_BOM: {
                utf32 = char32_t(*m_current);
                ++m_current;
                utf32 |= char32_t(*m_current << 8);
                ++m_current;
                utf32 |= char32_t(*m_current << 16);
                ++m_current;
                utf32 |= char32_t(*m_current << 24);
                break;
            }
            default: // LCOV_EXCL_LINE
                // should not come here.
                break; // LCOV_EXCL_LINE
            }

            utf8_encoding::from_utf32(utf32, m_utf8_buffer, m_utf8_buf_size);
            ++m_current;
            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

private:
    /// The iterator at the current position.
    IterType m_current {};
    /// The iterator at the end of input.
    IterType m_end {};
    /// The encoding type for this input adapter.
    encode_t m_encode_type {encode_t::UTF_8_N};
    /// The buffer for decoding characters read from the input.
    std::array<char16_t, 2> m_encoded_buffer {{0, 0}};
    /// The number of elements in `m_encoded_buffer`.
    std::size_t m_encoded_buf_size {0};
    /// The buffer for UTF-8 encoded characters.
    std::array<char, 4> m_utf8_buffer {{0, 0, 0, 0}};
    /// The next index in `m_utf8_buffer` to read.
    std::size_t m_utf8_buf_index {0};
    /// The number of bytes in `m_utf8_buffer`.
    std::size_t m_utf8_buf_size {0};
};

#ifdef FK_YAML_HAS_CHAR8_T

/// @brief An input adapter for iterators of type char8_t.
/// @tparam IterType An iterator type.
template <typename IterType>
class iterator_input_adapter<
    IterType,
    enable_if_t<std::is_same<remove_cv_t<typename std::iterator_traits<IterType>::value_type>, char8_t>::value>>
{
public:
    /// A type for characters used in this input adapter.
    using char_type = char;

    /// @brief Construct a new iterator_input_adapter object.
    iterator_input_adapter() = default;

    /// @brief Construct a new iterator_input_adapter object.
    /// @param begin The beginning of iteraters.
    /// @param end The end of iterators.
    /// @param encode_type The encoding type for this input adapter.
    iterator_input_adapter(IterType begin, IterType end, encode_t encode_type) noexcept
        : m_current(begin),
          m_end(end),
          m_encode_type(encode_type)
    {
    }

    // allow only move construct/assignment like other input adapters.
    iterator_input_adapter(const iterator_input_adapter&) = delete;
    iterator_input_adapter(iterator_input_adapter&& rhs) = default;
    iterator_input_adapter& operator=(const iterator_input_adapter&) = delete;
    iterator_input_adapter& operator=(iterator_input_adapter&&) = default;
    ~iterator_input_adapter() = default;

    /// @brief Get a character at the current position and move forward.
    /// @return std::char_traits<char_type>::int_type A character or EOF.
    typename std::char_traits<char_type>::int_type get_character()
    {
        typename std::char_traits<char_type>::int_type ret = 0;
        switch (m_encode_type)
        {
        case encode_t::UTF_8_N:
        case encode_t::UTF_8_BOM:
            ret = get_character_for_utf8();
            break;
        default: // LCOV_EXCL_LINE
            // char8_t characters must be encoded in the UTF-8 format.
            // See https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0482r6.html.
            break; // LCOV_EXCL_LINE
        }
        return ret;
    }

private:
    /// @brief The concrete implementation of get_character() for UTF-8 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf8() noexcept
    {
        if (m_current != m_end)
        {
            auto ret = std::char_traits<char_type>::to_int_type(*m_current);
            ++m_current;
            return ret;
        }
        return std::char_traits<char_type>::eof();
    }

private:
    /// The iterator at the current position.
    IterType m_current {};
    /// The iterator at the end of input.
    IterType m_end {};
    /// The encoding type for this input adapter.
    encode_t m_encode_type {encode_t::UTF_8_N};
};

#endif // defined(FK_YAML_HAS_CHAR8_T)

/// @brief An input adapter for iterators of type char16_t.
/// @tparam IterType An iterator type.
template <typename IterType>
class iterator_input_adapter<
    IterType,
    enable_if_t<std::is_same<remove_cv_t<typename std::iterator_traits<IterType>::value_type>, char16_t>::value>>
{
public:
    /// A type for characters used in this input adapter.
    using char_type = char;

    /// @brief Construct a new iterator_input_adapter object.
    iterator_input_adapter() = default;

    /// @brief Construct a new iterator_input_adapter object.
    /// @param begin The beginning of iteraters.
    /// @param end The end of iterators.
    /// @param encode_type The encoding type for this input adapter.
    iterator_input_adapter(IterType begin, IterType end, encode_t encode_type) noexcept
        : m_current(begin),
          m_end(end),
          m_encode_type(encode_type)
    {
    }

    // allow only move construct/assignment like other input adapters.
    iterator_input_adapter(const iterator_input_adapter&) = delete;
    iterator_input_adapter(iterator_input_adapter&& rhs) = default;
    iterator_input_adapter& operator=(const iterator_input_adapter&) = delete;
    iterator_input_adapter& operator=(iterator_input_adapter&&) = default;
    ~iterator_input_adapter() = default;

    /// @brief Get a character at the current position and move forward.
    /// @return std::char_traits<char_type>::int_type A character or EOF.
    typename std::char_traits<char_type>::int_type get_character()
    {
        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            if (m_current == m_end)
            {
                if (m_encoded_buf_size == 0)
                {
                    return std::char_traits<char_type>::eof();
                }
            }

            while (m_current != m_end && m_encoded_buf_size < 2)
            {
                switch (m_encode_type)
                {
                case encode_t::UTF_16BE_N:
                case encode_t::UTF_16BE_BOM:
                    m_encoded_buffer[m_encoded_buf_size] = *m_current;
                    break;
                case encode_t::UTF_16LE_N:
                case encode_t::UTF_16LE_BOM: {
                    char16_t tmp = *m_current;
                    m_encoded_buffer[m_encoded_buf_size] = char16_t((tmp & 0x00FFu) << 8);
                    m_encoded_buffer[m_encoded_buf_size] |= char16_t((tmp & 0xFF00u) >> 8);
                    break;
                }
                default: // LCOV_EXCL_LINE
                    // should not come here.
                    break; // LCOV_EXCL_LINE
                }
                ++m_current;
                ++m_encoded_buf_size;
            }

            std::size_t consumed_size = 0;
            utf8_encoding::from_utf16(m_encoded_buffer, m_utf8_buffer, consumed_size, m_utf8_buf_size);

            if (consumed_size == 1)
            {
                m_encoded_buffer[0] = m_encoded_buffer[1];
                m_encoded_buffer[1] = 0;
            }
            m_encoded_buf_size -= consumed_size;

            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

private:
    /// The iterator at the current position.
    IterType m_current {};
    /// The iterator at the end of input.
    IterType m_end {};
    /// The encoding type for this input adapter.
    encode_t m_encode_type {encode_t::UTF_16BE_N};
    /// The buffer for decoding characters read from the input.
    std::array<char16_t, 2> m_encoded_buffer {{0, 0}};
    /// The number of elements in `m_encoded_buffer`.
    std::size_t m_encoded_buf_size {0};
    /// The buffer for UTF-8 encoded characters.
    std::array<char, 4> m_utf8_buffer {{0, 0, 0, 0}};
    /// The next index in `m_utf8_buffer` to read.
    std::size_t m_utf8_buf_index {0};
    /// The number of bytes in `m_utf8_buffer`.
    std::size_t m_utf8_buf_size {0};
};

/// @brief An input adapter for iterators of type char32_t.
/// @tparam IterType An iterator type.
template <typename IterType>
class iterator_input_adapter<
    IterType,
    enable_if_t<std::is_same<remove_cv_t<typename std::iterator_traits<IterType>::value_type>, char32_t>::value>>
{
public:
    /// A type for characters used in this input adapter.
    using char_type = char;

    /// @brief Construct a new iterator_input_adapter object.
    iterator_input_adapter() = default;

    /// @brief Construct a new iterator_input_adapter object.
    /// @param begin The beginning of iteraters.
    /// @param end The end of iterators.
    /// @param encode_type The encoding type for this input adapter.
    iterator_input_adapter(IterType begin, IterType end, encode_t encode_type) noexcept
        : m_current(begin),
          m_end(end),
          m_encode_type(encode_type)
    {
    }

    // allow only move construct/assignment like other input adapters.
    iterator_input_adapter(const iterator_input_adapter&) = delete;
    iterator_input_adapter(iterator_input_adapter&& rhs) = default;
    iterator_input_adapter& operator=(const iterator_input_adapter&) = delete;
    iterator_input_adapter& operator=(iterator_input_adapter&&) = default;
    ~iterator_input_adapter() = default;

    /// @brief Get a character at the current position and move forward.
    /// @return std::char_traits<char_type>::int_type A character or EOF.
    typename std::char_traits<char_type>::int_type get_character()
    {
        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            if (m_current == m_end)
            {
                return std::char_traits<char_type>::eof();
            }

            char32_t utf32 = 0;
            switch (m_encode_type)
            {
            case encode_t::UTF_32BE_N:
            case encode_t::UTF_32BE_BOM:
                utf32 = *m_current;
                break;
            case encode_t::UTF_32LE_N:
            case encode_t::UTF_32LE_BOM: {
                char32_t tmp = *m_current;
                utf32 |= char32_t((tmp & 0xFF000000u) >> 24);
                utf32 |= char32_t((tmp & 0x00FF0000u) >> 8);
                utf32 |= char32_t((tmp & 0x0000FF00u) << 8);
                utf32 |= char32_t((tmp & 0x000000FFu) << 24);
                break;
            }
            default: // LCOV_EXCL_LINE
                // should not come here.
                break; // LCOV_EXCL_LINE
            }

            utf8_encoding::from_utf32(utf32, m_utf8_buffer, m_utf8_buf_size);
            ++m_current;
            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

private:
    /// The iterator at the current position.
    IterType m_current {};
    /// The iterator at the end of input.
    IterType m_end {};
    /// The encoding type for this input adapter.
    encode_t m_encode_type {encode_t::UTF_32BE_N};
    /// The buffer for UTF-8 encoded characters.
    std::array<char, 4> m_utf8_buffer {{0, 0, 0, 0}};
    /// The next index in `m_utf8_buffer` to read.
    std::size_t m_utf8_buf_index {0};
    /// The number of bytes in `m_utf8_buffer`.
    std::size_t m_utf8_buf_size {0};
};

/// @brief An input adapter for C-style file handles.
class file_input_adapter
{
public:
    /// A type for characters used in this input adapter.
    using char_type = char;

    /// @brief Construct a new file_input_adapter object.
    file_input_adapter() = default;

    /// @brief Construct a new file_input_adapter object.
    /// @note
    /// This class doesn't call fopen() nor fclose().
    /// It's user's responsibility to call those functions.
    /// @param file A file handle for this adapter. (A non-null pointer is assumed.)
    /// @param encode_type The encoding type for this input adapter.
    explicit file_input_adapter(std::FILE* file, encode_t encode_type) noexcept
        : m_file(file),
          m_encode_type(encode_type)
    {
    }

    // allow only move construct/assignment
    file_input_adapter(const file_input_adapter&) = delete;
    file_input_adapter(file_input_adapter&& rhs) = default;
    file_input_adapter& operator=(const file_input_adapter&) = delete;
    file_input_adapter& operator=(file_input_adapter&&) = default;
    ~file_input_adapter() = default;

    /// @brief Get a character at the current position and move forward.
    /// @return std::char_traits<char_type>::int_type A character or EOF.
    typename std::char_traits<char_type>::int_type get_character()
    {
        typename std::char_traits<char_type>::int_type ret = 0;
        switch (m_encode_type)
        {
        case encode_t::UTF_8_N:
        case encode_t::UTF_8_BOM:
            ret = get_character_for_utf8();
            break;
        case encode_t::UTF_16BE_N:
        case encode_t::UTF_16BE_BOM:
        case encode_t::UTF_16LE_N:
        case encode_t::UTF_16LE_BOM:
            ret = get_character_for_utf16();
            break;
        case encode_t::UTF_32BE_N:
        case encode_t::UTF_32BE_BOM:
        case encode_t::UTF_32LE_N:
        case encode_t::UTF_32LE_BOM:
            ret = get_character_for_utf32();
            break;
        }
        return ret;
    }

private:
    /// @brief The concrete implementation of get_character() for UTF-8 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf8() noexcept
    {
        char ch = 0;
        size_t size = std::fread(&ch, sizeof(char), 1, m_file);
        if (size == 1)
        {
            return std::char_traits<char_type>::to_int_type(ch);
        }
        return std::char_traits<char_type>::eof();
    }

    /// @brief The concrete implementation of get_character() for UTF-16 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf16()
    {
        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            char chars[2] = {0, 0};
            while (m_encoded_buf_size < 2 && std::fread(&chars[0], sizeof(char), 2, m_file) == 2)
            {
                switch (m_encode_type)
                {
                case encode_t::UTF_16BE_N:
                case encode_t::UTF_16BE_BOM:
                    m_encoded_buffer[m_encoded_buf_size] = char16_t(uint8_t(chars[0]) << 8);
                    m_encoded_buffer[m_encoded_buf_size] |= char16_t(uint8_t(chars[1]));
                    break;
                case encode_t::UTF_16LE_N:
                case encode_t::UTF_16LE_BOM: {
                    m_encoded_buffer[m_encoded_buf_size] = char16_t(uint8_t(chars[0]));
                    m_encoded_buffer[m_encoded_buf_size] |= char16_t(uint8_t(chars[1]) << 8);
                    break;
                }
                default: // LCOV_EXCL_LINE
                    // should not come here.
                    break; // LCOV_EXCL_LINE
                }

                ++m_encoded_buf_size;
            }

            if (m_encoded_buf_size == 0)
            {
                return std::char_traits<char_type>::eof();
            }

            std::size_t consumed_size = 0;
            utf8_encoding::from_utf16(m_encoded_buffer, m_utf8_buffer, consumed_size, m_utf8_buf_size);

            if (consumed_size == 1)
            {
                m_encoded_buffer[0] = m_encoded_buffer[1];
                m_encoded_buffer[1] = 0;
            }
            m_encoded_buf_size -= consumed_size;

            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

    /// @brief The concrete implementation of get_character() for UTF-32 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf32()
    {
        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            char chars[4] = {0, 0, 0, 0};
            std::size_t size = std::fread(&chars[0], sizeof(char), 4, m_file);
            if (size != 4)
            {
                return std::char_traits<char_type>::eof();
            }

            char32_t utf32 = 0;
            switch (m_encode_type)
            {
            case encode_t::UTF_32BE_N:
            case encode_t::UTF_32BE_BOM:
                utf32 = char32_t(uint8_t(chars[0]) << 24);
                utf32 |= char32_t(uint8_t(chars[1]) << 16);
                utf32 |= char32_t(uint8_t(chars[2]) << 8);
                utf32 |= char32_t(uint8_t(chars[3]));
                break;
            case encode_t::UTF_32LE_N:
            case encode_t::UTF_32LE_BOM: {
                utf32 = char32_t(uint8_t(chars[0]));
                utf32 |= char32_t(uint8_t(chars[1]) << 8);
                utf32 |= char32_t(uint8_t(chars[2]) << 16);
                utf32 |= char32_t(uint8_t(chars[3]) << 24);
                break;
            }
            default: // LCOV_EXCL_LINE
                // should not come here.
                break; // LCOV_EXCL_LINE
            }

            utf8_encoding::from_utf32(utf32, m_utf8_buffer, m_utf8_buf_size);
            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

private:
    /// A pointer to the input file handle.
    std::FILE* m_file {nullptr};
    /// The encoding type for this input adapter.
    encode_t m_encode_type {encode_t::UTF_8_N};
    /// The buffer for decoding characters read from the input.
    std::array<char16_t, 2> m_encoded_buffer {{0, 0}};
    /// The number of elements in `m_encoded_buffer`.
    std::size_t m_encoded_buf_size {0};
    /// The buffer for UTF-8 encoded characters.
    std::array<char, 4> m_utf8_buffer {{0, 0, 0, 0}};
    /// The next index in `m_utf8_buffer` to read.
    std::size_t m_utf8_buf_index {0};
    /// The number of bytes in `m_utf8_buffer`.
    std::size_t m_utf8_buf_size {0};
};

/// @brief An input adapter for streams
class stream_input_adapter
{
public:
    /// A type for characters used in this input adapter.
    using char_type = char;

    /// @brief Construct a new stream_input_adapter object.
    stream_input_adapter() = default;

    /// @brief Construct a new stream_input_adapter object.
    /// @param is A reference to the target input stream.
    explicit stream_input_adapter(std::istream& is, encode_t encode_type) noexcept
        : m_istream(&is),
          m_encode_type(encode_type)
    {
    }

    // allow only move construct/assignment
    stream_input_adapter(const stream_input_adapter&) = delete;
    stream_input_adapter& operator=(const stream_input_adapter&) = delete;
    stream_input_adapter(stream_input_adapter&&) = default;
    stream_input_adapter& operator=(stream_input_adapter&&) = default;
    ~stream_input_adapter() = default;

    /// @brief Get a character at the current position and move forward.
    /// @return std::char_traits<char_type>::int_type A character or EOF.
    typename std::char_traits<char_type>::int_type get_character()
    {
        typename std::char_traits<char_type>::int_type ret = 0;
        switch (m_encode_type)
        {
        case encode_t::UTF_8_N:
        case encode_t::UTF_8_BOM:
            ret = get_character_for_utf8();
            break;
        case encode_t::UTF_16BE_N:
        case encode_t::UTF_16BE_BOM:
        case encode_t::UTF_16LE_N:
        case encode_t::UTF_16LE_BOM:
            ret = get_character_for_utf16();
            break;
        case encode_t::UTF_32BE_N:
        case encode_t::UTF_32BE_BOM:
        case encode_t::UTF_32LE_N:
        case encode_t::UTF_32LE_BOM:
            ret = get_character_for_utf32();
            break;
        }
        return ret;
    }

private:
    /// @brief The concrete implementation of get_character() for UTF-8 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf8() noexcept
    {
        return m_istream->get();
    }

    /// @brief The concrete implementation of get_character() for UTF-16 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf16()
    {
        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            while (m_encoded_buf_size < 2)
            {
                char chars[2] = {0, 0};
                m_istream->read(&chars[0], 2);
                std::streamsize size = m_istream->gcount();
                if (size != 2)
                {
                    if (m_encoded_buf_size == 0)
                    {
                        return std::char_traits<char_type>::eof();
                    }
                    break;
                }

                switch (m_encode_type)
                {
                case encode_t::UTF_16BE_N:
                case encode_t::UTF_16BE_BOM:
                    m_encoded_buffer[m_encoded_buf_size] = char16_t(uint8_t(chars[0]) << 8);
                    m_encoded_buffer[m_encoded_buf_size] |= char16_t(uint8_t(chars[1]));
                    break;
                case encode_t::UTF_16LE_N:
                case encode_t::UTF_16LE_BOM: {
                    m_encoded_buffer[m_encoded_buf_size] = char16_t(uint8_t(chars[0]));
                    m_encoded_buffer[m_encoded_buf_size] |= char16_t(uint8_t(chars[1]) << 8);
                    break;
                }
                default: // LCOV_EXCL_LINE
                    // should not come here.
                    break; // LCOV_EXCL_LINE
                }

                ++m_encoded_buf_size;
            };

            std::size_t consumed_size = 0;
            utf8_encoding::from_utf16(m_encoded_buffer, m_utf8_buffer, consumed_size, m_utf8_buf_size);

            if (consumed_size == 1)
            {
                m_encoded_buffer[0] = m_encoded_buffer[1];
                m_encoded_buffer[1] = 0;
            }
            m_encoded_buf_size -= consumed_size;

            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

    /// @brief The concrete implementation of get_character() for UTF-32 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf32()
    {
        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            char ch = 0;
            m_istream->read(&ch, 1);
            std::streamsize size = m_istream->gcount();
            if (size != 1)
            {
                return std::char_traits<char_type>::eof();
            }

            char32_t utf32 = 0;
            switch (m_encode_type)
            {
            case encode_t::UTF_32BE_N:
            case encode_t::UTF_32BE_BOM:
                utf32 = char32_t(ch << 24);
                m_istream->read(&ch, 1);
                utf32 |= char32_t(ch << 16);
                m_istream->read(&ch, 1);
                utf32 |= char32_t(ch << 8);
                m_istream->read(&ch, 1);
                utf32 |= char32_t(ch);
                break;
            case encode_t::UTF_32LE_N:
            case encode_t::UTF_32LE_BOM: {
                utf32 = char32_t(ch);
                m_istream->read(&ch, 1);
                utf32 |= char32_t(ch << 8);
                m_istream->read(&ch, 1);
                utf32 |= char32_t(ch << 16);
                m_istream->read(&ch, 1);
                utf32 |= char32_t(ch << 24);
                break;
            }
            default: // LCOV_EXCL_LINE
                // should not come here.
                break; // LCOV_EXCL_LINE
            }

            utf8_encoding::from_utf32(utf32, m_utf8_buffer, m_utf8_buf_size);
            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

private:
    /// A pointer to the input stream object.
    std::istream* m_istream {nullptr};
    /// The encoding type for this input adapter.
    encode_t m_encode_type {encode_t::UTF_8_N};
    /// The buffer for decoding characters read from the input.
    std::array<char16_t, 2> m_encoded_buffer {{0, 0}};
    /// The number of elements in `m_encoded_buffer`.
    std::size_t m_encoded_buf_size {0};
    /// The buffer for UTF-8 encoded characters.
    std::array<char, 4> m_utf8_buffer {{0, 0, 0, 0}};
    /// The next index in `m_utf8_buffer` to read.
    std::size_t m_utf8_buf_index {0};
    /// The number of bytes in `m_utf8_buffer`.
    std::size_t m_utf8_buf_size {0};
};

/////////////////////////////////
//   input_adapter providers   //
/////////////////////////////////

/// @brief A factory method for iterator_input_adapter objects with ieterator values.
/// @tparam ItrType An iterator type.
/// @param begin The beginning of iterators.
/// @param end The end of iterators.
/// @return iterator_input_adapter<ItrType> An iterator_input_adapter object for the target iterator type.
template <typename ItrType, size_t ElemSize = sizeof(decltype(*(std::declval<ItrType>())))>
inline iterator_input_adapter<ItrType> input_adapter(ItrType begin, ItrType end)
{
    encode_t encode_type = detect_encoding_and_skip_bom(begin, end);
    return iterator_input_adapter<ItrType>(begin, end, encode_type);
}

/// @brief A factory method for iterator_input_adapter objects with C-style arrays.
/// @tparam T A type of arrayed objects.
/// @tparam N A size of an array.
/// @return decltype(input_adapter(array, array + N)) An iterator_input_adapter object for the target array.
template <typename T, std::size_t N>
inline auto input_adapter(T (&array)[N]) -> decltype(input_adapter(array, array + (N - 1)))
{
    return input_adapter(array, array + (N - 1));
}

/// @brief A namespace to implement container_input_adapter_factory for internal use.
namespace container_input_adapter_factory_impl
{

using std::begin;
using std::end;

/// @brief A factory of input adapters for containers.
/// @tparam ContainerType A container type.
/// @tparam typename N/A
template <typename ContainerType, typename = void>
struct container_input_adapter_factory
{
};

/// @brief A partial specialization of container_input_adapter_factory if begin()/end() are available for ContainerType.
/// @tparam ContainerType A container type.
template <typename ContainerType>
struct container_input_adapter_factory<
    ContainerType, void_t<decltype(begin(std::declval<ContainerType>()), end(std::declval<ContainerType>()))>>
{
    /// A type for resulting input adapter object.
    using adapter_type =
        decltype(input_adapter(begin(std::declval<ContainerType>()), end(std::declval<ContainerType>())));

    /// @brief A factory method of input adapter objects for the target container objects.
    /// @param container
    /// @return adapter_type
    static adapter_type create(const ContainerType& container)
    {
        return input_adapter(begin(container), end(container));
    }
};

} // namespace container_input_adapter_factory_impl

/// @brief A factory method for iterator_input_adapter objects with containers.
/// @tparam ContainerType A container type.
/// @param container A container object.
/// @return container_input_adapter_factory_impl::container_input_adapter_factory<ContainerType>::adapter_type
template <typename ContainerType>
inline typename container_input_adapter_factory_impl::container_input_adapter_factory<ContainerType>::adapter_type
input_adapter(ContainerType&& container)
{
    return container_input_adapter_factory_impl::container_input_adapter_factory<ContainerType>::create(container);
}

/// @brief A factory method for file_input_adapter objects with C-style file handles.
/// @param file A file handle.
/// @return file_input_adapter A file_input_adapter object.
inline file_input_adapter input_adapter(std::FILE* file)
{
    if (!file)
    {
        throw fkyaml::exception("Invalid FILE object pointer.");
    }
    encode_t encode_type = detect_encoding_and_skip_bom(file);
    return file_input_adapter(file, encode_type);
}

/// @brief
/// @param stream
/// @return stream_input_adapter
inline stream_input_adapter input_adapter(std::istream& stream) noexcept
{
    encode_t encode_type = detect_encoding_and_skip_bom(stream);
    return stream_input_adapter(stream, encode_type);
}

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_INPUT_ADAPTER_HPP_ */