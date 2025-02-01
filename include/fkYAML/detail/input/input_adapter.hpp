//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_INPUT_INPUT_ADAPTER_HPP
#define FK_YAML_DETAIL_INPUT_INPUT_ADAPTER_HPP

#include <array>
#include <cstdio>
#include <cstring>
#include <deque>
#include <istream>
#include <iterator>
#include <string>

#include <fkYAML/detail/macros/define_macros.hpp>
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
    /// @param begin The beginning of iterators.
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
        if FK_YAML_UNLIKELY (m_begin == m_end) {
            return {};
        }

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
        default:                   // LCOV_EXCL_LINE
            detail::unreachable(); // LCOV_EXCL_LINE
        }
    }

private:
    /// @brief The concrete implementation of get_buffer_view() for UTF-8 encoded inputs.
    /// @return View into the UTF-8 encoded input buffer contents.
    str_view get_buffer_view_utf8() {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_8);

        IterType current = m_begin;
        std::deque<IterType> cr_itrs {};
        while (current != m_end) {
            const auto first = static_cast<uint8_t>(*current);
            const uint32_t num_bytes = utf8::get_num_bytes(first);

            switch (num_bytes) {
            case 1:
                if FK_YAML_UNLIKELY (first == 0x0D /*CR*/) {
                    cr_itrs.emplace_back(current);
                }
                break;
            case 2: {
                const auto second = static_cast<uint8_t>(*++current);
                const bool is_valid = utf8::validate(first, second);
                if FK_YAML_UNLIKELY (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", {first, second});
                }
                break;
            }
            case 3: {
                const auto second = static_cast<uint8_t>(*++current);
                const auto third = static_cast<uint8_t>(*++current);
                const bool is_valid = utf8::validate(first, second, third);
                if FK_YAML_UNLIKELY (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", {first, second, third});
                }
                break;
            }
            case 4: {
                const auto second = static_cast<uint8_t>(*++current);
                const auto third = static_cast<uint8_t>(*++current);
                const auto fourth = static_cast<uint8_t>(*++current);
                const bool is_valid = utf8::validate(first, second, third, fourth);
                if FK_YAML_UNLIKELY (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", {first, second, third, fourth});
                }
                break;
            }
            default:           // LCOV_EXCL_LINE
                unreachable(); // LCOV_EXCL_LINE
            }

            ++current;
        }

        const bool is_contiguous_no_cr = cr_itrs.empty() && m_is_contiguous;
        if FK_YAML_LIKELY (is_contiguous_no_cr) {
            // The input iterators (begin, end) can be used as-is during parsing.
            FK_YAML_ASSERT(m_begin != m_end);
            return str_view {&*m_begin, static_cast<std::size_t>(std::distance(m_begin, m_end))};
        }

        m_buffer.reserve(std::distance(m_begin, m_end) - cr_itrs.size());

        current = m_begin;
        for (const auto& cr_itr : cr_itrs) {
            m_buffer.append(current, cr_itr);
            current = std::next(cr_itr);
        }
        m_buffer.append(current, m_end);

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
                auto utf16 = static_cast<char16_t>(static_cast<uint8_t>(*current) << shift_bits[0]);
                utf16 |= static_cast<char16_t>(static_cast<uint8_t>(*++current) << shift_bits[1]);
                ++current;

                // skip appending CRs.
                if FK_YAML_LIKELY (utf16 != char16_t(0x000Du)) {
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    encoded_buffer[encoded_buf_size++] = utf16;
                }
            }

            uint32_t consumed_size = 0;
            utf8::from_utf16(encoded_buffer, utf8_buffer, consumed_size, utf8_buf_size);

            if FK_YAML_LIKELY (consumed_size == 1) {
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
            auto utf32 = static_cast<char32_t>(*current << shift_bits[0]);
            ++current;
            utf32 |= static_cast<char32_t>(*current << shift_bits[1]);
            ++current;
            utf32 |= static_cast<char32_t>(*current << shift_bits[2]);
            ++current;
            utf32 |= static_cast<char32_t>(*current << shift_bits[3]);
            ++current;

            if FK_YAML_LIKELY (utf32 != char32_t(0x0000000Du)) {
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
    std::string m_buffer;
    /// Whether ItrType is a contiguous iterator.
    bool m_is_contiguous {false};
};

#if FK_YAML_HAS_CHAR8_T

/// @brief An input adapter for iterators of type char8_t.
/// @tparam IterType An iterator type.
template <typename IterType>
class iterator_input_adapter<IterType, enable_if_t<is_iterator_of<IterType, char8_t>::value>> {
public:
    /// @brief Construct a new iterator_input_adapter object.
    iterator_input_adapter() = default;

    /// @brief Construct a new iterator_input_adapter object.
    /// @param begin The beginning of iterators.
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
        if FK_YAML_UNLIKELY (m_begin == m_end) {
            return {};
        }

        IterType current = m_begin;
        std::deque<IterType> cr_itrs {};
        while (current != m_end) {
            const auto first = static_cast<uint8_t>(*current);
            const uint32_t num_bytes = utf8::get_num_bytes(first);

            switch (num_bytes) {
            case 1:
                if FK_YAML_UNLIKELY (first == 0x0D /*CR*/) {
                    cr_itrs.emplace_back(current);
                }
                break;
            case 2: {
                const auto second = static_cast<uint8_t>(*++current);
                const bool is_valid = utf8::validate(first, second);
                if FK_YAML_UNLIKELY (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", {first, second});
                }
                break;
            }
            case 3: {
                const auto second = static_cast<uint8_t>(*++current);
                const auto third = static_cast<uint8_t>(*++current);
                const bool is_valid = utf8::validate(first, second, third);
                if FK_YAML_UNLIKELY (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", {first, second, third});
                }
                break;
            }
            case 4: {
                const auto second = static_cast<uint8_t>(*++current);
                const auto third = static_cast<uint8_t>(*++current);
                const auto fourth = static_cast<uint8_t>(*++current);
                const bool is_valid = utf8::validate(first, second, third, fourth);
                if FK_YAML_UNLIKELY (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", {first, second, third, fourth});
                }
                break;
            }
            default:           // LCOV_EXCL_LINE
                unreachable(); // LCOV_EXCL_LINE
            }

            ++current;
        }

        m_buffer.reserve(std::distance(m_begin, m_end) - cr_itrs.size());
        current = m_begin;
        for (const auto& cr_itr : cr_itrs) {
            std::transform(
                current, cr_itr, std::back_inserter(m_buffer), [](char8_t c) { return static_cast<char>(c); });
            current = std::next(cr_itr);
        }
        std::transform(current, m_end, std::back_inserter(m_buffer), [](char8_t c) { return static_cast<char>(c); });

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
    std::string m_buffer;
    /// Whether ItrType is a contiguous iterator.
    bool m_is_contiguous {false};
};

#endif // FK_YAML_HAS_CHAR8_T

/// @brief An input adapter for iterators of type char16_t.
/// @tparam IterType An iterator type.
template <typename IterType>
class iterator_input_adapter<IterType, enable_if_t<is_iterator_of<IterType, char16_t>::value>> {
public:
    /// @brief Construct a new iterator_input_adapter object.
    iterator_input_adapter() = default;

    /// @brief Construct a new iterator_input_adapter object.
    /// @param begin The beginning of iterators.
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
        if FK_YAML_UNLIKELY (m_begin == m_end) {
            return {};
        }

        const int shift_bits = (m_encode_type == utf_encode_t::UTF_16BE) ? 0 : 8;

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
                char16_t utf16 = *current;
                ++current;
                utf16 = static_cast<char16_t>(((utf16 & 0x00FFu) << shift_bits) | ((utf16 & 0xFF00u) >> shift_bits));

                if FK_YAML_LIKELY (utf16 != char16_t(0x000Du)) {
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    encoded_buffer[encoded_buf_size++] = utf16;
                }
            }

            uint32_t consumed_size = 0;
            utf8::from_utf16(encoded_buffer, utf8_buffer, consumed_size, utf8_buf_size);

            if FK_YAML_LIKELY (consumed_size == 1) {
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
    std::string m_buffer;
    /// Whether ItrType is a contiguous iterator.
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
    /// @param begin The beginning of iterators.
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
        if FK_YAML_UNLIKELY (m_begin == m_end) {
            return {};
        }

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
            const char32_t tmp = *current;
            ++current;
            const auto utf32 = static_cast<char32_t>(
                ((tmp & 0xFF000000u) >> shift_bits[0]) | ((tmp & 0x00FF0000u) >> shift_bits[1]) |
                ((tmp & 0x0000FF00u) << shift_bits[2]) | ((tmp & 0x000000FFu) << shift_bits[3]));

            if FK_YAML_UNLIKELY (utf32 != static_cast<char32_t>(0x0000000Du)) {
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
    std::string m_buffer;
    /// Whether ItrType is a contiguous iterator.
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
        default:                   // LCOV_EXCL_LINE
            detail::unreachable(); // LCOV_EXCL_LINE
        }
    }

private:
    /// @brief The concrete implementation of get_buffer_view() for UTF-8 encoded inputs.
    /// @return View into the UTF-8 encoded input buffer contents.
    str_view get_buffer_view_utf8() {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_8);

        m_buffer.clear();
        char tmp_buf[256] {};
        constexpr std::size_t buf_size = sizeof(tmp_buf) / sizeof(tmp_buf[0]);
        std::size_t read_size = 0;
        while ((read_size = std::fread(&tmp_buf[0], sizeof(char), buf_size, m_file)) > 0) {
            char* p_current = &tmp_buf[0];
            char* p_end = p_current + read_size;

            // copy tmp_buf to m_buffer, dropping CRs.
            char* p_cr = p_current;
            do {
                if FK_YAML_UNLIKELY (*p_cr == '\r') {
                    m_buffer.append(p_current, p_cr);
                    p_current = p_cr + 1;
                }
                ++p_cr;
            } while (p_cr != p_end);

            m_buffer.append(p_current, p_end);
        }

        if FK_YAML_UNLIKELY (m_buffer.empty()) {
            return {};
        }

        auto current = m_buffer.begin();
        auto end = m_buffer.end();
        while (current != end) {
            const auto first = static_cast<uint8_t>(*current++);
            const uint32_t num_bytes = utf8::get_num_bytes(first);

            switch (num_bytes) {
            case 1:
                break;
            case 2: {
                const auto second = static_cast<uint8_t>(*current++);
                const bool is_valid = utf8::validate(first, second);
                if FK_YAML_UNLIKELY (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", {first, second});
                }
                break;
            }
            case 3: {
                const auto second = static_cast<uint8_t>(*current++);
                const auto third = static_cast<uint8_t>(*current++);
                const bool is_valid = utf8::validate(first, second, third);
                if FK_YAML_UNLIKELY (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", {first, second, third});
                }
                break;
            }
            case 4: {
                const auto second = static_cast<uint8_t>(*current++);
                const auto third = static_cast<uint8_t>(*current++);
                const auto fourth = static_cast<uint8_t>(*current++);
                const bool is_valid = utf8::validate(first, second, third, fourth);
                if FK_YAML_UNLIKELY (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", {first, second, third, fourth});
                }
                break;
            }
            default:           // LCOV_EXCL_LINE
                unreachable(); // LCOV_EXCL_LINE
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
                const auto utf16 = static_cast<char16_t>(
                    (static_cast<uint8_t>(chars[0]) << shift_bits[0]) |
                    (static_cast<uint8_t>(chars[1]) << shift_bits[1]));
                if FK_YAML_LIKELY (utf16 != static_cast<char16_t>(0x000Du)) {
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    encoded_buffer[encoded_buf_size++] = utf16;
                }
            }

            uint32_t consumed_size = 0;
            utf8::from_utf16(encoded_buffer, utf8_buffer, consumed_size, utf8_buf_size);

            if FK_YAML_LIKELY (consumed_size == 1) {
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
            const std::size_t size = std::fread(&chars[0], sizeof(char), 4, m_file);
            if (size != 4) {
                break;
            }

            const auto utf32 = static_cast<char32_t>(
                (static_cast<uint8_t>(chars[0]) << shift_bits[0]) | (static_cast<uint8_t>(chars[1]) << shift_bits[1]) |
                (static_cast<uint8_t>(chars[2]) << shift_bits[2]) | (static_cast<uint8_t>(chars[3]) << shift_bits[3]));

            if FK_YAML_LIKELY (utf32 != char32_t(0x0000000Du)) {
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
    std::string m_buffer;
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
        default:                   // LCOV_EXCL_LINE
            detail::unreachable(); // LCOV_EXCL_LINE
        }
    }

private:
    /// @brief The concrete implementation of get_buffer_view() for UTF-8 encoded inputs.
    /// @return View into the UTF-8 encoded input buffer contents.
    str_view get_buffer_view_utf8() {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_8);

        m_buffer.clear();
        char tmp_buf[256] {};
        do {
            m_istream->read(&tmp_buf[0], 256);
            const auto read_size = static_cast<std::size_t>(m_istream->gcount());
            if FK_YAML_UNLIKELY (read_size == 0) {
                break;
            }

            char* p_current = &tmp_buf[0];
            char* p_end = p_current + read_size;

            // copy tmp_buf to m_buffer, dropping CRs.
            char* p_cr = p_current;
            do {
                if FK_YAML_UNLIKELY (*p_cr == '\r') {
                    m_buffer.append(p_current, p_cr);
                    p_current = p_cr + 1;
                }
                ++p_cr;
            } while (p_cr != p_end);

            m_buffer.append(p_current, p_end);
        } while (!m_istream->eof());

        if FK_YAML_UNLIKELY (m_buffer.empty()) {
            return {};
        }

        auto current = m_buffer.begin();
        auto end = m_buffer.end();
        while (current != end) {
            const auto first = static_cast<uint8_t>(*current++);
            const uint32_t num_bytes = utf8::get_num_bytes(first);

            switch (num_bytes) {
            case 1:
                break;
            case 2: {
                const auto second = static_cast<uint8_t>(*current++);
                const bool is_valid = utf8::validate(first, second);
                if FK_YAML_UNLIKELY (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", {first, second});
                }
                break;
            }
            case 3: {
                const auto second = static_cast<uint8_t>(*current++);
                const auto third = static_cast<uint8_t>(*current++);
                const bool is_valid = utf8::validate(first, second, third);
                if FK_YAML_UNLIKELY (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", {first, second, third});
                }
                break;
            }
            case 4: {
                const auto second = static_cast<uint8_t>(*current++);
                const auto third = static_cast<uint8_t>(*current++);
                const auto fourth = static_cast<uint8_t>(*current++);
                const bool is_valid = utf8::validate(first, second, third, fourth);
                if FK_YAML_UNLIKELY (!is_valid) {
                    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", {first, second, third, fourth});
                }
                break;
            }
            default:           // LCOV_EXCL_LINE
                unreachable(); // LCOV_EXCL_LINE
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
                const std::streamsize size = m_istream->gcount();
                if FK_YAML_UNLIKELY (size != 2) {
                    break;
                }

                const auto utf16 = static_cast<char16_t>(
                    (static_cast<uint8_t>(chars[0]) << shift_bits[0]) |
                    (static_cast<uint8_t>(chars[1]) << shift_bits[1]));

                if FK_YAML_LIKELY (utf16 != static_cast<char16_t>(0x000Du)) {
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    encoded_buffer[encoded_buf_size++] = utf16;
                }
            }

            uint32_t consumed_size = 0;
            utf8::from_utf16(encoded_buffer, utf8_buffer, consumed_size, utf8_buf_size);

            if FK_YAML_LIKELY (consumed_size == 1) {
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
            const std::streamsize size = m_istream->gcount();
            if FK_YAML_UNLIKELY (size != 4) {
                break;
            }

            const auto utf32 = static_cast<char32_t>(
                (static_cast<uint8_t>(chars[0]) << shift_bits[0]) | (static_cast<uint8_t>(chars[1]) << shift_bits[1]) |
                (static_cast<uint8_t>(chars[2]) << shift_bits[2]) | (static_cast<uint8_t>(chars[3]) << shift_bits[3]));

            if FK_YAML_LIKELY (utf32 != char32_t(0x0000000Du)) {
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
    std::string m_buffer;
};

/////////////////////////////////
//   input_adapter providers   //
/////////////////////////////////

/// @brief A concrete factory method for iterator_input_adapter objects with iterators.
/// @tparam ItrType An iterator type.
/// @param begin The beginning of iterators.
/// @param end The end of iterators.
/// @param is_contiguous Whether iterators refer to a contiguous byte array.
/// @return An iterator_input_adapter object for the target iterator type.
template <typename ItrType>
inline iterator_input_adapter<ItrType> create_iterator_input_adapter(ItrType begin, ItrType end, bool is_contiguous) {
    const utf_encode_t encode_type = utf_encode_detector<ItrType>::detect(begin, end);
    return iterator_input_adapter<ItrType>(begin, end, encode_type, is_contiguous);
}

/// @brief A factory method for iterator_input_adapter objects with iterator values.
/// @tparam ItrType An iterator type.
/// @param begin The beginning of iterators.
/// @param end The end of iterators.
/// @return iterator_input_adapter<ItrType> An iterator_input_adapter object for the target iterator type.
template <typename ItrType>
inline iterator_input_adapter<ItrType> input_adapter(ItrType begin, ItrType end) {
    bool is_contiguous = true;
    const auto size = std::distance(begin, end);

    // Check if `begin` & `end` are contiguous iterators.
    // Getting distance between begin and (end - 1) avoids dereferencing an invalid sentinel.
    if FK_YAML_LIKELY (size > 0) {
        using char_ptr_t = remove_cvref_t<typename std::iterator_traits<ItrType>::pointer>;
        char_ptr_t p_begin = &*begin;
        char_ptr_t p_second_last = &*std::next(begin, size - 1);
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
    /// A type for resulting input adapter object.
    using adapter_type =
        decltype(input_adapter(begin(std::declval<ContainerType>()), end(std::declval<ContainerType>())));

    /// @brief A factory method of input adapter objects for the target container objects.
    /// @param container A container-like input object.
    /// @return adapter_type An iterator_input_adapter object.
    static adapter_type create(const ContainerType& container) {
        return input_adapter(begin(container), end(container));
    }
};

} // namespace input_adapter_factory

/// @brief A factory method for iterator_input_adapter objects with containers.
/// @tparam ContainerType A container type.
/// @param container A container object.
/// @return input_adapter_factory::container_input_adapter_factory<ContainerType>::adapter_type
template <typename ContainerType>
inline typename input_adapter_factory::container_input_adapter_factory<ContainerType>::adapter_type input_adapter(
    const ContainerType& container) {
    return input_adapter_factory::container_input_adapter_factory<ContainerType>::create(container);
}

/// @brief A factory method for file_input_adapter objects with C-style file handles.
/// @param file A file handle.
/// @return file_input_adapter A file_input_adapter object.
inline file_input_adapter input_adapter(std::FILE* file) {
    if FK_YAML_UNLIKELY (!file) {
        throw fkyaml::exception("Invalid FILE object pointer.");
    }

    const utf_encode_t encode_type = file_utf_encode_detector::detect(file);
    return file_input_adapter(file, encode_type);
}

/// @brief A factory method for stream_input_adapter objects with std::istream objects.
/// @param stream An input stream.
/// @return stream_input_adapter A stream_input_adapter object.
inline stream_input_adapter input_adapter(std::istream& stream) {
    if FK_YAML_UNLIKELY (!stream.good()) {
        throw fkyaml::exception("Invalid stream.");
    }

    const utf_encode_t encode_type = stream_utf_encode_detector::detect(stream);
    return stream_input_adapter(stream, encode_type);
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_INPUT_ADAPTER_HPP */
