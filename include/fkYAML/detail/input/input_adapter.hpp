///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_INPUT_INPUT_ADAPTER_HPP_
#define FK_YAML_DETAIL_INPUT_INPUT_ADAPTER_HPP_

#include <array>
#include <cstdio>
#include <cstring>
#include <istream>
#include <iterator>
#include <string>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/assert.hpp>
#include <fkYAML/detail/encodings/utf_encode_detector.hpp>
#include <fkYAML/detail/encodings/utf_encode_t.hpp>
#include <fkYAML/detail/encodings/utf_encodings.hpp>
#include <fkYAML/detail/meta/input_adapter_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/str_view.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

///////////////////////
//   input_adapter   //
///////////////////////

template <typename IterType, typename = void>
class iterator_input_adapter;

/// @brief An input adapter for iterators of type char.
/// @tparam IterType An iterator type.
template <typename IterType>
class iterator_input_adapter<IterType, enable_if_t<is_iterator_of<IterType, char>::value>> {
public:
    /// @brief Construct a new iterator_input_adapter object.
    iterator_input_adapter() = default;

    /// @brief Construct a new iterator_input_adapter object.
    /// @param begin The beginning of iteraters.
    /// @param end The end of iterators.
    /// @param encode_type The encoding type for this input adapter.
    /// @param is_contiguous Whether iterators are contiguous or not.
    iterator_input_adapter(IterType begin, IterType end, utf_encode_t encode_type, bool is_contiguous) noexcept
        : m_begin(begin),
          m_end(end),
          m_encode_type(encode_type),
          m_is_contiguous(is_contiguous) {
    }

    // allow only move construct/assignment like other input adapters.
    iterator_input_adapter(const iterator_input_adapter&) = delete;
    iterator_input_adapter(iterator_input_adapter&& rhs) = default;
    iterator_input_adapter& operator=(const iterator_input_adapter&) = delete;
    iterator_input_adapter& operator=(iterator_input_adapter&&) = default;
    ~iterator_input_adapter() = default;

    /// @brief Get view into the input buffer contents.
    /// @return View into the input buffer contents.
    str_view get_buffer_view() {
        m_buffer.clear();

        switch (m_encode_type) {
        case utf_encode_t::UTF_8:
            return get_buffer_view_utf8();
        case utf_encode_t::UTF_16BE:
        case utf_encode_t::UTF_16LE:
            return get_buffer_view_utf16();
        case utf_encode_t::UTF_32BE:
        case utf_encode_t::UTF_32LE:
            return get_buffer_view_utf32();
        default:       // LCOV_EXCL_LINE
            return {}; // LCOV_EXCL_LINE
        }
    }

private:
    /// @brief The concrete implementation of get_buffer_view() for UTF-8 encoded inputs.
    /// @return View into the UTF-8 encoded input buffer contents.
    str_view get_buffer_view_utf8() {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_8);

        IterType current = m_begin;
        while (current != m_end) {
            uint8_t first = uint8_t(*current++);
            uint32_t num_bytes = utf8::get_num_bytes(first);

            switch (num_bytes) {
            case 2: {
                std::initializer_list<uint8_t> bytes {first, uint8_t(*current++)};
                bool is_valid = utf8::validate(bytes);
                if (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", bytes);
                }
                break;
            }
            case 3: {
                std::initializer_list<uint8_t> bytes {first, uint8_t(*current++), uint8_t(*current++)};
                bool is_valid = utf8::validate(bytes);
                if (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", bytes);
                }
                break;
            }
            case 4: {
                std::initializer_list<uint8_t> bytes {
                    first, uint8_t(*current++), uint8_t(*current++), uint8_t(*current++)};
                bool is_valid = utf8::validate(bytes);
                if (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", bytes);
                }
                break;
            }
            case 1:
            default:
                break;
            }
        }

        IterType cr_or_end_itr = std::find(m_begin, m_end, '\r');
        if (cr_or_end_itr == m_end && m_is_contiguous) {
            // The input iterators (begin, end) can be used as-is during parsing.
            return str_view {m_begin, m_end};
        }

        m_buffer.reserve(std::distance(m_begin, m_end));

        current = m_begin;
        do {
            m_buffer.append(current, cr_or_end_itr);
            if (cr_or_end_itr == m_end) {
                break;
            }
            current = std::next(cr_or_end_itr);
            cr_or_end_itr = std::find(current, m_end, '\r');
        } while (current != m_end);

        return str_view {m_buffer.begin(), m_buffer.end()};
    }

    /// @brief The concrete implementation of get_buffer_view() for UTF-16 encoded inputs.
    /// @return View into the UTF-8 encoded input buffer contents.
    str_view get_buffer_view_utf16() {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_16BE || m_encode_type == utf_encode_t::UTF_16LE);

        // Assume the input characters are all ASCII characters.
        // That's the most probably the case.
        m_buffer.reserve(std::distance(m_begin, m_end) / 2);

        int shift_bits[2] {0, 0};
        if (m_encode_type == utf_encode_t::UTF_16BE) {
            shift_bits[0] = 8;
        }
        else // m_encode_type == utf_encode_t::UTF_16LE
        {
            shift_bits[1] = 8;
        }

        std::array<char16_t, 2> encoded_buffer {{0, 0}};
        uint32_t encoded_buf_size {0};
        std::array<uint8_t, 4> utf8_buffer {{0, 0, 0, 0}};
        uint32_t utf8_buf_size {0};

        IterType current = m_begin;
        while (current != m_end || encoded_buf_size != 0) {
            while (current != m_end && encoded_buf_size < 2) {
                char16_t utf16 = static_cast<char16_t>(uint8_t(*current++) << shift_bits[0]);
                utf16 |= static_cast<char16_t>(uint8_t(*current++) << shift_bits[1]);
                if (utf16 != char16_t(0x000Du)) {
                    encoded_buffer[encoded_buf_size++] = utf16;
                }
            }

            uint32_t consumed_size = 0;
            utf8::from_utf16(encoded_buffer, utf8_buffer, consumed_size, utf8_buf_size);

            if (consumed_size == 1) {
                encoded_buffer[0] = encoded_buffer[1];
            }
            encoded_buf_size -= consumed_size;

            m_buffer.append(reinterpret_cast<const char*>(utf8_buffer.data()), utf8_buf_size);
        }

        return str_view {m_buffer.begin(), m_buffer.end()};
    }

    /// @brief The concrete implementation of get_buffer_view() for UTF-32 encoded inputs.
    /// @return View into the UTF-8 encoded input buffer contents.
    str_view get_buffer_view_utf32() {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_32BE || m_encode_type == utf_encode_t::UTF_32LE);

        // Assume the input characters are all ASCII characters.
        // That's the most probably the case.
        m_buffer.reserve(std::distance(m_begin, m_end) / 4);

        int shift_bits[4] {0, 0, 0, 0};
        if (m_encode_type == utf_encode_t::UTF_32BE) {
            shift_bits[0] = 24;
            shift_bits[1] = 16;
            shift_bits[2] = 8;
        }
        else // m_encode_type == utf_encode_t::UTF_32LE
        {
            shift_bits[1] = 8;
            shift_bits[2] = 16;
            shift_bits[3] = 24;
        }

        std::array<uint8_t, 4> utf8_buffer {{0, 0, 0, 0}};
        uint32_t utf8_buf_size {0};

        IterType current = m_begin;
        while (current != m_end) {
            char32_t utf32 = static_cast<char32_t>(*current++ << shift_bits[0]);
            utf32 |= static_cast<char32_t>(*current++ << shift_bits[1]);
            utf32 |= static_cast<char32_t>(*current++ << shift_bits[2]);
            utf32 |= static_cast<char32_t>(*current++ << shift_bits[3]);

            if (utf32 != char32_t(0x0000000Du)) {
                utf8::from_utf32(utf32, utf8_buffer, utf8_buf_size);
                m_buffer.append(reinterpret_cast<const char*>(utf8_buffer.data()), utf8_buf_size);
            }
        }

        return str_view {m_buffer.begin(), m_buffer.end()};
    }

private:
    /// The iterator at the beginning of input.
    IterType m_begin {};
    /// The iterator at the end of input.
    IterType m_end {};
    /// The encoding type for this input adapter.
    utf_encode_t m_encode_type {utf_encode_t::UTF_8};
    /// The normalized owned buffer.
    std::string m_buffer {};
    /// Whether or not ItrType is a contiguous iterator.
    bool m_is_contiguous {false};
};

#ifdef FK_YAML_HAS_CHAR8_T

/// @brief An input adapter for iterators of type char8_t.
/// @tparam IterType An iterator type.
template <typename IterType>
class iterator_input_adapter<IterType, enable_if_t<is_iterator_of<IterType, char8_t>::value>> {
public:
    /// @brief Construct a new iterator_input_adapter object.
    iterator_input_adapter() = default;

    /// @brief Construct a new iterator_input_adapter object.
    /// @param begin The beginning of iteraters.
    /// @param end The end of iterators.
    /// @param encode_type The encoding type for this input adapter.
    /// @param is_contiguous Whether iterators are contiguous or not.
    iterator_input_adapter(IterType begin, IterType end, utf_encode_t encode_type, bool is_contiguous) noexcept
        : m_begin(begin),
          m_end(end),
          m_encode_type(encode_type),
          m_is_contiguous(is_contiguous) {
        // char8_t characters must be encoded in the UTF-8 format.
        // See https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0482r6.html.
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_8);
    }

    // allow only move construct/assignment like other input adapters.
    iterator_input_adapter(const iterator_input_adapter&) = delete;
    iterator_input_adapter(iterator_input_adapter&& rhs) = default;
    iterator_input_adapter& operator=(const iterator_input_adapter&) = delete;
    iterator_input_adapter& operator=(iterator_input_adapter&&) = default;
    ~iterator_input_adapter() = default;

    /// @brief Get view into the input buffer contents.
    /// @return View into the input buffer contents.
    str_view get_buffer_view() {
        IterType current = m_begin;
        while (current != m_end) {
            uint8_t first = static_cast<uint8_t>(*current++);
            uint32_t num_bytes = utf8::get_num_bytes(first);

            switch (num_bytes) {
            case 2: {
                std::initializer_list<uint8_t> bytes {first, uint8_t(*current++)};
                bool is_valid = utf8::validate(bytes);
                if (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", bytes);
                }
                break;
            }
            case 3: {
                std::initializer_list<uint8_t> bytes {first, uint8_t(*current++), uint8_t(*current++)};
                bool is_valid = utf8::validate(bytes);
                if (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", bytes);
                }
                break;
            }
            case 4: {
                std::initializer_list<uint8_t> bytes {
                    first, uint8_t(*current++), uint8_t(*current++), uint8_t(*current++)};
                bool is_valid = utf8::validate(bytes);
                if (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", bytes);
                }
                break;
            }
            case 1:
            default:
                break;
            }
        }

        m_buffer.reserve(std::distance(m_begin, m_end));
        current = m_begin;

        while (current != m_end) {
            char c = char(*current++);
            if (c != '\r') {
                m_buffer.push_back(c);
            }
        }

        return str_view {m_buffer.begin(), m_buffer.end()};
    }

private:
    /// The iterator at the beginning of input.
    IterType m_begin {};
    /// The iterator at the end of input.
    IterType m_end {};
    /// The encoding type for this input adapter.
    utf_encode_t m_encode_type {utf_encode_t::UTF_8};
    /// The normalized owned buffer.
    std::string m_buffer {};
    /// Whether or not ItrType is a contiguous iterator.
    bool m_is_contiguous {false};
};

#endif // defined(FK_YAML_HAS_CHAR8_T)

/// @brief An input adapter for iterators of type char16_t.
/// @tparam IterType An iterator type.
template <typename IterType>
class iterator_input_adapter<IterType, enable_if_t<is_iterator_of<IterType, char16_t>::value>> {
public:
    /// @brief Construct a new iterator_input_adapter object.
    iterator_input_adapter() = default;

    /// @brief Construct a new iterator_input_adapter object.
    /// @param begin The beginning of iteraters.
    /// @param end The end of iterators.
    /// @param encode_type The encoding type for this input adapter.
    /// @param is_contiguous Whether iterators are contiguous or not.
    iterator_input_adapter(IterType begin, IterType end, utf_encode_t encode_type, bool is_contiguous) noexcept
        : m_begin(begin),
          m_end(end),
          m_encode_type(encode_type),
          m_is_contiguous(is_contiguous) {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_16BE || m_encode_type == utf_encode_t::UTF_16LE);
    }

    // allow only move construct/assignment like other input adapters.
    iterator_input_adapter(const iterator_input_adapter&) = delete;
    iterator_input_adapter(iterator_input_adapter&& rhs) = default;
    iterator_input_adapter& operator=(const iterator_input_adapter&) = delete;
    iterator_input_adapter& operator=(iterator_input_adapter&&) = default;
    ~iterator_input_adapter() = default;

    /// @brief Get view into the input buffer contents.
    /// @return View into the input buffer contents.
    str_view get_buffer_view() {
        int shift_bits = (m_encode_type == utf_encode_t::UTF_16BE) ? 0 : 8;

        std::array<char16_t, 2> encoded_buffer {{0, 0}};
        uint32_t encoded_buf_size {0};
        std::array<uint8_t, 4> utf8_buffer {{0, 0, 0, 0}};
        uint32_t utf8_buf_size {0};

        // Assume the input characters are all ASCII characters.
        // That's the most probably the case.
        m_buffer.reserve(std::distance(m_begin, m_end));

        IterType current = m_begin;
        while (current != m_end || encoded_buf_size != 0) {
            while (current != m_end && encoded_buf_size < 2) {
                char16_t utf16 = *current++;
                utf16 = char16_t(
                    static_cast<uint16_t>((utf16 & 0x00FFu) << shift_bits) |
                    static_cast<uint16_t>((utf16 & 0xFF00u) >> shift_bits));

                if (utf16 != char16_t(0x000Du)) {
                    encoded_buffer[encoded_buf_size++] = utf16;
                }
            }

            uint32_t consumed_size = 0;
            utf8::from_utf16(encoded_buffer, utf8_buffer, consumed_size, utf8_buf_size);

            if (consumed_size == 1) {
                encoded_buffer[0] = encoded_buffer[1];
                encoded_buffer[1] = 0;
            }
            encoded_buf_size -= consumed_size;

            m_buffer.append(reinterpret_cast<const char*>(utf8_buffer.data()), utf8_buf_size);
        }

        return str_view {m_buffer.begin(), m_buffer.end()};
    }

private:
    /// The iterator at the beginning of input.
    IterType m_begin {};
    /// The iterator at the end of input.
    IterType m_end {};
    /// The encoding type for this input adapter.
    utf_encode_t m_encode_type {utf_encode_t::UTF_16BE};
    /// The normalized owned buffer.
    std::string m_buffer {};
    /// Whether or not ItrType is a contiguous iterator.
    bool m_is_contiguous {false};
};

/// @brief An input adapter for iterators of type char32_t.
/// @tparam IterType An iterator type.
template <typename IterType>
class iterator_input_adapter<IterType, enable_if_t<is_iterator_of<IterType, char32_t>::value>> {
public:
    /// @brief Construct a new iterator_input_adapter object.
    iterator_input_adapter() = default;

    /// @brief Construct a new iterator_input_adapter object.
    /// @param begin The beginning of iteraters.
    /// @param end The end of iterators.
    /// @param encode_type The encoding type for this input adapter.
    /// @param is_contiguous Whether iterators are contiguous or not.
    iterator_input_adapter(IterType begin, IterType end, utf_encode_t encode_type, bool is_contiguous) noexcept
        : m_begin(begin),
          m_end(end),
          m_encode_type(encode_type),
          m_is_contiguous(is_contiguous) {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_32BE || m_encode_type == utf_encode_t::UTF_32LE);
    }

    // allow only move construct/assignment like other input adapters.
    iterator_input_adapter(const iterator_input_adapter&) = delete;
    iterator_input_adapter(iterator_input_adapter&& rhs) = default;
    iterator_input_adapter& operator=(const iterator_input_adapter&) = delete;
    iterator_input_adapter& operator=(iterator_input_adapter&&) = default;
    ~iterator_input_adapter() = default;

    /// @brief Get view into the input buffer contents.
    /// @return View into the input buffer contents.
    str_view get_buffer_view() {
        int shift_bits[4] {0, 0, 0, 0};
        if (m_encode_type == utf_encode_t::UTF_32LE) {
            shift_bits[0] = 24;
            shift_bits[1] = 8;
            shift_bits[2] = 8;
            shift_bits[3] = 24;
        }

        std::array<uint8_t, 4> utf8_buffer {{0, 0, 0, 0}};
        uint32_t utf8_buf_size {0};

        // Assume the input characters are all ASCII characters.
        // That's the most probably the case.
        m_buffer.reserve(std::distance(m_begin, m_end));

        IterType current = m_begin;
        while (current != m_end) {
            char32_t tmp = *current++;
            char32_t utf32 = char32_t(
                static_cast<uint32_t>((tmp & 0xFF000000u) >> shift_bits[0]) |
                static_cast<uint32_t>((tmp & 0x00FF0000u) >> shift_bits[1]) |
                static_cast<uint32_t>((tmp & 0x0000FF00u) << shift_bits[2]) |
                static_cast<uint32_t>((tmp & 0x000000FFu) << shift_bits[3]));

            if (utf32 != char32_t(0x0000000Du)) {
                utf8::from_utf32(utf32, utf8_buffer, utf8_buf_size);
                m_buffer.append(reinterpret_cast<const char*>(utf8_buffer.data()), utf8_buf_size);
            }
        }

        return str_view {m_buffer.begin(), m_buffer.end()};
    }

private:
    /// The iterator at the beginning of input.
    IterType m_begin {};
    /// The iterator at the end of input.
    IterType m_end {};
    /// The encoding type for this input adapter.
    utf_encode_t m_encode_type {utf_encode_t::UTF_32BE};
    /// The normalized owned buffer.
    std::string m_buffer {};
    /// Whether or not ItrType is a contiguous iterator.
    bool m_is_contiguous {false};
};

/// @brief An input adapter for C-style file handles.
class file_input_adapter {
public:
    /// @brief Construct a new file_input_adapter object.
    file_input_adapter() = default;

    /// @brief Construct a new file_input_adapter object.
    /// @note
    /// This class doesn't call fopen() nor fclose().
    /// It's user's responsibility to call those functions.
    /// @param file A file handle for this adapter. (A non-null pointer is assumed.)
    /// @param encode_type The encoding type for this input adapter.
    explicit file_input_adapter(std::FILE* file, utf_encode_t encode_type) noexcept
        : m_file(file),
          m_encode_type(encode_type) {
    }

    // allow only move construct/assignment
    file_input_adapter(const file_input_adapter&) = delete;
    file_input_adapter(file_input_adapter&& rhs) = default;
    file_input_adapter& operator=(const file_input_adapter&) = delete;
    file_input_adapter& operator=(file_input_adapter&&) = default;
    ~file_input_adapter() = default;

    /// @brief Get view into the input buffer contents.
    /// @return View into the input buffer contents.
    str_view get_buffer_view() {
        switch (m_encode_type) {
        case utf_encode_t::UTF_8:
            return get_buffer_view_utf8();
        case utf_encode_t::UTF_16BE:
        case utf_encode_t::UTF_16LE:
            return get_buffer_view_utf16();
        case utf_encode_t::UTF_32BE:
        case utf_encode_t::UTF_32LE:
            return get_buffer_view_utf32();
        default:       // LCOV_EXCL_LINE
            return {}; // LCOV_EXCL_LINE
        }
    }

private:
    /// @brief The concrete implementation of get_buffer_view() for UTF-8 encoded inputs.
    /// @return View into the UTF-8 encoded input buffer contents.
    str_view get_buffer_view_utf8() {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_8);

        char tmp_buf[256] {};
        std::size_t buf_size = sizeof(tmp_buf) / sizeof(tmp_buf[0]);
        std::size_t read_size = 0;
        while ((read_size = std::fread(&tmp_buf[0], sizeof(char), buf_size, m_file)) > 0) {
            char* p_current = &tmp_buf[0];
            char* p_end = p_current + read_size;
            do {
                // find CR in `tmp_buf`.
                char* p_cr_or_end = p_current;
                while (p_cr_or_end != p_end) {
                    if (*p_cr_or_end == '\r') {
                        break;
                    }
                    ++p_cr_or_end;
                }

                m_buffer.append(p_current, p_cr_or_end);
                p_current = (p_cr_or_end == p_end) ? p_end : p_cr_or_end + 1;
            } while (p_current != p_end);
        }

        auto current = m_buffer.begin();
        auto end = m_buffer.end();
        while (current != end) {
            uint8_t first = static_cast<uint8_t>(*current++);
            uint32_t num_bytes = utf8::get_num_bytes(first);

            switch (num_bytes) {
            case 2: {
                std::initializer_list<uint8_t> bytes {first, uint8_t(*current++)};
                bool is_valid = utf8::validate(bytes);
                if (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", bytes);
                }
                break;
            }
            case 3: {
                std::initializer_list<uint8_t> bytes {first, uint8_t(*current++), uint8_t(*current++)};
                bool is_valid = utf8::validate(bytes);
                if (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", bytes);
                }
                break;
            }
            case 4: {
                std::initializer_list<uint8_t> bytes {
                    first, uint8_t(*current++), uint8_t(*current++), uint8_t(*current++)};
                bool is_valid = utf8::validate(bytes);
                if (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", bytes);
                }
                break;
            }
            case 1:
            default:
                break;
            }
        }

        return str_view {m_buffer.begin(), m_buffer.end()};
    }

    /// @brief The concrete implementation of get_buffer_view() for UTF-16 encoded inputs.
    /// @return View into the UTF-8 encoded input buffer contents.
    str_view get_buffer_view_utf16() {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_16BE || m_encode_type == utf_encode_t::UTF_16LE);

        int shift_bits[2] {0, 0};
        if (m_encode_type == utf_encode_t::UTF_16BE) {
            shift_bits[0] = 8;
        }
        else { // m_encode_type == utf_encode_t::UTF_16LE
            shift_bits[1] = 8;
        }

        char chars[2] = {0, 0};
        std::array<char16_t, 2> encoded_buffer {{0, 0}};
        uint32_t encoded_buf_size {0};
        std::array<uint8_t, 4> utf8_buffer {{0, 0, 0, 0}};
        uint32_t utf8_buf_size {0};

        while (std::feof(m_file) == 0) {
            while (encoded_buf_size < 2 && std::fread(&chars[0], sizeof(char), 2, m_file) == 2) {
                char16_t utf16 = char16_t(
                    static_cast<uint16_t>(uint8_t(chars[0]) << shift_bits[0]) |
                    static_cast<uint16_t>(uint8_t(chars[1]) << shift_bits[1]));
                if (utf16 != char16_t(0x000Du)) {
                    encoded_buffer[encoded_buf_size++] = utf16;
                }
            }

            uint32_t consumed_size = 0;
            utf8::from_utf16(encoded_buffer, utf8_buffer, consumed_size, utf8_buf_size);

            if (consumed_size == 1) {
                encoded_buffer[0] = encoded_buffer[1];
            }
            encoded_buf_size -= consumed_size;

            m_buffer.append(reinterpret_cast<const char*>(utf8_buffer.data()), utf8_buf_size);
        }

        return str_view {m_buffer.begin(), m_buffer.end()};
    }

    /// @brief The concrete implementation of get_buffer_view() for UTF-32 encoded inputs.
    /// @return View into the UTF-8 encoded input buffer contents.
    str_view get_buffer_view_utf32() {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_32BE || m_encode_type == utf_encode_t::UTF_32LE);

        int shift_bits[4] {0, 0, 0, 0};
        if (m_encode_type == utf_encode_t::UTF_32BE) {
            shift_bits[0] = 24;
            shift_bits[1] = 16;
            shift_bits[2] = 8;
        }
        else { // m_encode_type == utf_encode_t::UTF_32LE
            shift_bits[1] = 8;
            shift_bits[2] = 16;
            shift_bits[3] = 24;
        }

        char chars[4] = {0, 0, 0, 0};
        std::array<uint8_t, 4> utf8_buffer {{0, 0, 0, 0}};
        uint32_t utf8_buf_size {0};

        while (std::feof(m_file) == 0) {
            std::size_t size = std::fread(&chars[0], sizeof(char), 4, m_file);
            if (size != 4) {
                break;
            }

            char32_t utf32 = char32_t(
                static_cast<uint32_t>(uint8_t(chars[0]) << shift_bits[0]) |
                static_cast<uint32_t>(uint8_t(chars[1]) << shift_bits[1]) |
                static_cast<uint32_t>(uint8_t(chars[2]) << shift_bits[2]) |
                static_cast<uint32_t>(uint8_t(chars[3]) << shift_bits[3]));

            if (utf32 != char32_t(0x0000000Du)) {
                utf8::from_utf32(utf32, utf8_buffer, utf8_buf_size);
                m_buffer.append(reinterpret_cast<const char*>(utf8_buffer.data()), utf8_buf_size);
            }
        }

        return str_view {m_buffer.begin(), m_buffer.end()};
    }

private:
    /// A pointer to the input file handle.
    std::FILE* m_file {nullptr};
    /// The encoding type for this input adapter.
    utf_encode_t m_encode_type {utf_encode_t::UTF_8};
    /// The normalized owned buffer.
    std::string m_buffer {};
};

/// @brief An input adapter for streams
class stream_input_adapter {
public:
    /// @brief Construct a new stream_input_adapter object.
    stream_input_adapter() = default;

    /// @brief Construct a new stream_input_adapter object.
    /// @param is A reference to the target input stream.
    /// @param encode_type The encoding type for this input adapter.
    explicit stream_input_adapter(std::istream& is, utf_encode_t encode_type) noexcept
        : m_istream(&is),
          m_encode_type(encode_type) {
    }

    // allow only move construct/assignment
    stream_input_adapter(const stream_input_adapter&) = delete;
    stream_input_adapter& operator=(const stream_input_adapter&) = delete;
    stream_input_adapter(stream_input_adapter&&) = default;
    stream_input_adapter& operator=(stream_input_adapter&&) = default;
    ~stream_input_adapter() = default;

    /// @brief Get view into the input buffer contents.
    /// @return View into the input buffer contents.
    str_view get_buffer_view() {
        switch (m_encode_type) {
        case utf_encode_t::UTF_8:
            return get_buffer_view_utf8();
        case utf_encode_t::UTF_16BE:
        case utf_encode_t::UTF_16LE:
            return get_buffer_view_utf16();
        case utf_encode_t::UTF_32BE:
        case utf_encode_t::UTF_32LE:
            return get_buffer_view_utf32();
        default:       // LCOV_EXCL_LINE
            return {}; // LCOV_EXCL_LINE
        }
    }

private:
    /// @brief The concrete implementation of get_buffer_view() for UTF-8 encoded inputs.
    /// @return View into the UTF-8 encoded input buffer contents.
    str_view get_buffer_view_utf8() {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_8);

        char tmp_buf[256] {};
        do {
            m_istream->read(&tmp_buf[0], 256);
            std::size_t read_size = static_cast<std::size_t>(m_istream->gcount());

            char* p_current = &tmp_buf[0];
            char* p_end = p_current + read_size;
            do {
                // find CR in `tmp_buf`.
                char* p_cr_or_end = p_current;
                while (p_cr_or_end != p_end) {
                    if (*p_cr_or_end == '\r') {
                        break;
                    }
                    ++p_cr_or_end;
                }

                m_buffer.append(p_current, p_cr_or_end);
                p_current = (p_cr_or_end == p_end) ? p_end : p_cr_or_end + 1;
            } while (p_current != p_end);
        } while (!m_istream->eof());

        auto current = m_buffer.begin();
        auto end = m_buffer.end();
        while (current != end) {
            uint8_t first = static_cast<uint8_t>(*current++);
            uint32_t num_bytes = utf8::get_num_bytes(first);

            switch (num_bytes) {
            case 2: {
                std::initializer_list<uint8_t> bytes {first, uint8_t(*current++)};
                bool is_valid = utf8::validate(bytes);
                if (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", bytes);
                }
                break;
            }
            case 3: {
                std::initializer_list<uint8_t> bytes {first, uint8_t(*current++), uint8_t(*current++)};
                bool is_valid = utf8::validate(bytes);
                if (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", bytes);
                }
                break;
            }
            case 4: {
                std::initializer_list<uint8_t> bytes {
                    first, uint8_t(*current++), uint8_t(*current++), uint8_t(*current++)};
                bool is_valid = utf8::validate(bytes);
                if (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", bytes);
                }
                break;
            }
            case 1:
            default:
                break;
            }
        }

        return str_view {m_buffer.begin(), m_buffer.end()};
    }

    /// @brief The concrete implementation of get_buffer_view() for UTF-16 encoded inputs.
    /// @return View into the UTF-8 encoded input buffer contents.
    str_view get_buffer_view_utf16() {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_16BE || m_encode_type == utf_encode_t::UTF_16LE);

        int shift_bits[2] {0, 0};
        if (m_encode_type == utf_encode_t::UTF_16BE) {
            shift_bits[0] = 8;
        }
        else { // m_encode_type == utf_encode_t::UTF_16LE
            shift_bits[1] = 8;
        }

        char chars[2] = {0, 0};
        std::array<char16_t, 2> encoded_buffer {{0, 0}};
        uint32_t encoded_buf_size {0};
        std::array<uint8_t, 4> utf8_buffer {{0, 0, 0, 0}};
        uint32_t utf8_buf_size {0};

        do {
            while (encoded_buf_size < 2) {
                m_istream->read(&chars[0], 2);
                std::streamsize size = m_istream->gcount();
                if (size != 2) {
                    break;
                }

                char16_t utf16 = char16_t(
                    static_cast<uint16_t>(uint8_t(chars[0]) << shift_bits[0]) |
                    static_cast<uint16_t>(uint8_t(chars[1]) << shift_bits[1]));

                if (utf16 != char16_t(0x000Du)) {
                    encoded_buffer[encoded_buf_size++] = utf16;
                }
            };

            uint32_t consumed_size = 0;
            utf8::from_utf16(encoded_buffer, utf8_buffer, consumed_size, utf8_buf_size);

            if (consumed_size == 1) {
                encoded_buffer[0] = encoded_buffer[1];
            }
            encoded_buf_size -= consumed_size;

            m_buffer.append(reinterpret_cast<const char*>(utf8_buffer.data()), utf8_buf_size);
        } while (!m_istream->eof());

        return str_view {m_buffer.begin(), m_buffer.end()};
    }

    /// @brief The concrete implementation of get_buffer_view() for UTF-32 encoded inputs.
    /// @return View into the UTF-8 encoded input buffer contents.
    str_view get_buffer_view_utf32() {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_32BE || m_encode_type == utf_encode_t::UTF_32LE);

        int shift_bits[4] {0, 0, 0, 0};
        if (m_encode_type == utf_encode_t::UTF_32BE) {
            shift_bits[0] = 24;
            shift_bits[1] = 16;
            shift_bits[2] = 8;
        }
        else { // m_encode_type == utf_encode_t::UTF_32LE
            shift_bits[1] = 8;
            shift_bits[2] = 16;
            shift_bits[3] = 24;
        }

        char chars[4] = {0, 0, 0, 0};
        std::array<uint8_t, 4> utf8_buffer {{0, 0, 0, 0}};
        uint32_t utf8_buf_size {0};

        do {
            m_istream->read(&chars[0], 4);
            std::streamsize size = m_istream->gcount();
            if (size != 4) {
                break;
            }

            char32_t utf32 = char32_t(
                static_cast<uint32_t>(uint8_t(chars[0]) << shift_bits[0]) |
                static_cast<uint32_t>(uint8_t(chars[1]) << shift_bits[1]) |
                static_cast<uint32_t>(uint8_t(chars[2]) << shift_bits[2]) |
                static_cast<uint32_t>(uint8_t(chars[3]) << shift_bits[3]));

            if (utf32 != char32_t(0x0000000Du)) {
                utf8::from_utf32(utf32, utf8_buffer, utf8_buf_size);
                m_buffer.append(reinterpret_cast<const char*>(utf8_buffer.data()), utf8_buf_size);
            }
        } while (!m_istream->eof());

        return str_view {m_buffer.begin(), m_buffer.end()};
    }

private:
    /// A pointer to the input stream object.
    std::istream* m_istream {nullptr};
    /// The encoding type for this input adapter.
    utf_encode_t m_encode_type {utf_encode_t::UTF_8};
    /// The normalized owned buffer.
    std::string m_buffer {};
};

/////////////////////////////////
//   input_adapter providers   //
/////////////////////////////////

namespace {

template <typename ItrType>
inline iterator_input_adapter<ItrType> create_iterator_input_adapter(
    ItrType begin, ItrType end, bool is_contiguous) noexcept {
    utf_encode_t encode_type = utf_encode_detector<ItrType>::detect(begin, end);
    return iterator_input_adapter<ItrType>(begin, end, encode_type, is_contiguous);
}

} // anonymous namespace

/// @brief A factory method for iterator_input_adapter objects with ieterator values.
/// @tparam ItrType An iterator type.
/// @param begin The beginning of iterators.
/// @param end The end of iterators.
/// @return iterator_input_adapter<ItrType> An iterator_input_adapter object for the target iterator type.
template <typename ItrType>
inline iterator_input_adapter<ItrType> input_adapter(ItrType begin, ItrType end) {
    constexpr bool is_random_access_itr =
        std::is_same<typename std::iterator_traits<ItrType>::iterator_category, std::random_access_iterator_tag>::value;

    // Check if `begin` & `end` are contiguous iterators.
    // Getting distance between begin and (end - 1) avoids dereferencing an invalid sentinel.
    bool is_contiguous = false;
    if (is_random_access_itr) {
        ptrdiff_t size = static_cast<ptrdiff_t>(std::distance(begin, end - 1));

        using CharPtr = remove_cvref_t<typename std::iterator_traits<ItrType>::pointer>;
        CharPtr p_begin = &*begin;
        CharPtr p_second_last = &*(end - 1);
        is_contiguous = (p_second_last - p_begin == size);
    }
    return create_iterator_input_adapter(begin, end, is_contiguous);
}

/// @brief A factory method for iterator_input_adapter objects with C-style arrays.
/// @tparam T A type of arrayed objects.
/// @tparam N A size of an array.
/// @return decltype(input_adapter(array, array + N)) An iterator_input_adapter object for the target array.
template <typename T, std::size_t N>
inline auto input_adapter(T (&array)[N]) -> decltype(create_iterator_input_adapter(array, array + (N - 1), true)) {
    return create_iterator_input_adapter(array, array + (N - 1), true);
}

/// @brief A namespace to implement container_input_adapter_factory for internal use.
namespace input_adapter_factory {

using std::begin;
using std::end;

/// @brief A factory of input adapters for containers.
/// @tparam ContainerType A container type.
/// @tparam typename N/A
template <typename ContainerType, typename = void>
struct container_input_adapter_factory {};

/// @brief A partial specialization of container_input_adapter_factory if begin()/end() are available for ContainerType.
/// @tparam ContainerType A container type.
template <typename ContainerType>
struct container_input_adapter_factory<
    ContainerType, void_t<decltype(begin(std::declval<ContainerType>()), end(std::declval<ContainerType>()))>> {
    /// Whether or not ContainerType is a contiguous container.
    static constexpr bool is_contiguous = is_contiguous_container<ContainerType>::value;

    /// A type for resulting input adapter object.
    using adapter_type = decltype(create_iterator_input_adapter(
        begin(std::declval<ContainerType>()), end(std::declval<ContainerType>()), is_contiguous));

    /// @brief A factory method of input adapter objects for the target container objects.
    /// @param container A container-like input object.
    /// @return adapter_type An iterator_input_adapter object.
    static adapter_type create(const ContainerType& container) {
        return create_iterator_input_adapter(begin(container), end(container), is_contiguous);
    }
};

} // namespace input_adapter_factory

/// @brief A factory method for iterator_input_adapter objects with containers.
/// @tparam ContainerType A container type.
/// @param container A container object.
/// @return input_adapter_factory::container_input_adapter_factory<ContainerType>::adapter_type
template <typename ContainerType>
inline typename input_adapter_factory::container_input_adapter_factory<ContainerType>::adapter_type input_adapter(
    ContainerType&& container) {
    return input_adapter_factory::container_input_adapter_factory<ContainerType>::create(container);
}

/// @brief A factory method for file_input_adapter objects with C-style file handles.
/// @param file A file handle.
/// @return file_input_adapter A file_input_adapter object.
inline file_input_adapter input_adapter(std::FILE* file) {
    if (!file) {
        throw fkyaml::exception("Invalid FILE object pointer.");
    }
    utf_encode_t encode_type = file_utf_encode_detector::detect(file);
    return file_input_adapter(file, encode_type);
}

/// @brief A factory method for stream_input_adapter objects with std::istream objects.
/// @param stream An input stream.
/// @return stream_input_adapter A stream_input_adapter object.
inline stream_input_adapter input_adapter(std::istream& stream) {
    if (!stream.good()) {
        throw fkyaml::exception("Invalid stream.");
    }
    utf_encode_t encode_type = stream_utf_encode_detector::detect(stream);
    return stream_input_adapter(stream, encode_type);
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_INPUT_ADAPTER_HPP_ */
