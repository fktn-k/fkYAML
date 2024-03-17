///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_ENCODINGS_UTF_ENCODING_HPP_
#define FK_YAML_DETAIL_ENCODINGS_UTF_ENCODING_HPP_

#include <array>
#include <cstdint>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/exception.hpp>

/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

template <typename CharType>
class utf_encoding;

/////////////////////////
//   UTF-8 Encoding   ///
/////////////////////////

class utf8_encoding
{
    using int_type = std::char_traits<char>::int_type;

public:
    /// @brief Validates the encoding of a given byte array whose length is 1.
    /// @param[in] byte_array The byte array to be validated.
    /// @return true if a given byte array is valid, false otherwise.
    static bool validate(std::array<int_type, 1> byte_array) noexcept
    {
        // U+0000..U+007F
        return (0x00 <= byte_array[0] && byte_array[0] <= 0x7F);
    }

    /// @brief Validates the encoding of a given byte array whose length is 2.
    /// @param[in] byte_array The byte array to be validated.
    /// @return true if a given byte array is valid, false otherwise.
    static bool validate(std::array<int_type, 2> byte_array) noexcept
    {
        // U+0080..U+07FF
        //   1st Byte: 0xC2..0xDF
        //   2nd Byte: 0x80..0xBF
        if (0xC2 <= byte_array[0] && byte_array[0] <= 0xDF)
        {
            if (0x80 <= byte_array[1] && byte_array[1] <= 0xBF)
            {
                return true;
            }
        }

        // The rest of byte combinations are invalid.
        return false;
    }

    /// @brief Validates the encoding of a given byte array whose length is 3.
    /// @param[in] byte_array The byte array to be validated.
    /// @return true if a given byte array is valid, false otherwise.
    static bool validate(std::array<int_type, 3> byte_array) noexcept
    {
        // U+1000..U+CFFF:
        //   1st Byte: 0xE0..0xEC
        //   2nd Byte: 0x80..0xBF
        //   3rd Byte: 0x80..0xBF
        if (0xE0 <= byte_array[0] && byte_array[0] <= 0xEC)
        {
            if (0x80 <= byte_array[1] && byte_array[1] <= 0xBF)
            {
                if (0x80 <= byte_array[2] && byte_array[2] <= 0xBF)
                {
                    return true;
                }
            }
            return false;
        }

        // U+D000..U+D7FF:
        //   1st Byte: 0xED
        //   2nd Byte: 0x80..0x9F
        //   3rd Byte: 0x80..0xBF
        if (byte_array[0] == 0xED)
        {
            if (0x80 <= byte_array[1] && byte_array[1] <= 0x9F)
            {
                if (0x80 <= byte_array[2] && byte_array[2] <= 0xBF)
                {
                    return true;
                }
            }
            return false;
        }

        // U+E000..U+FFFF:
        //   1st Byte: 0xEE..0xEF
        //   2nd Byte: 0x80..0xBF
        //   3rd Byte: 0x80..0xBF
        if (byte_array[0] == 0xEE || byte_array[0] == 0xEF)
        {
            if (0x80 <= byte_array[1] && byte_array[1] <= 0xBF)
            {
                if (0x80 <= byte_array[2] && byte_array[2] <= 0xBF)
                {
                    return true;
                }
            }
            return false;
        }

        // The rest of byte combinations are invalid.
        return false;
    }

    /// @brief Validates the encoding of a given byte array whose length is 4.
    /// @param[in] byte_array The byte array to be validated.
    /// @return true if a given byte array is valid, false otherwise.
    static bool validate(std::array<int_type, 4> byte_array) noexcept
    {
        // U+10000..U+3FFFF:
        //   1st Byte: 0xF0
        //   2nd Byte: 0x90..0xBF
        //   3rd Byte: 0x80..0xBF
        //   4th Byte: 0x80..0xBF
        if (byte_array[0] == 0xF0)
        {
            if (0x90 <= byte_array[1] && byte_array[1] <= 0xBF)
            {
                if (0x80 <= byte_array[2] && byte_array[2] <= 0xBF)
                {
                    if (0x80 <= byte_array[3] && byte_array[3] <= 0xBF)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        // U+40000..U+FFFFF:
        //   1st Byte: 0xF1..0xF3
        //   2nd Byte: 0x80..0xBF
        //   3rd Byte: 0x80..0xBF
        //   4th Byte: 0x80..0xBF
        if (0xF1 <= byte_array[0] && byte_array[0] <= 0xF3)
        {
            if (0x80 <= byte_array[1] && byte_array[1] <= 0xBF)
            {
                if (0x80 <= byte_array[2] && byte_array[2] <= 0xBF)
                {
                    if (0x80 <= byte_array[3] && byte_array[3] <= 0xBF)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        // U+100000..U+10FFFF:
        //   1st Byte: 0xF4
        //   2nd Byte: 0x80..0x8F
        //   3rd Byte: 0x80..0xBF
        //   4th Byte: 0x80..0xBF
        if (byte_array[0] == 0xF4)
        {
            if (0x80 <= byte_array[1] && byte_array[1] <= 0x8F)
            {
                if (0x80 <= byte_array[2] && byte_array[2] <= 0xBF)
                {
                    if (0x80 <= byte_array[3] && byte_array[3] <= 0xBF)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        // The rest of byte combinations are invalid.
        return false;
    }

    /// @brief Converts UTF-16 encoded characters to UTF-8 encoded bytes.
    /// @param[in] utf16 UTF-16 encoded character(s).
    /// @param[out] utf8_bytes UTF-8 encoded bytes.
    /// @param[out] consumed_size The number of UTF-16 encoded characters used for the conversion.
    /// @param[out] encoded_size The size of UTF-encoded bytes.
    static void from_utf16(
        std::array<char16_t, 2> utf16, std::array<char, 4>& utf8_bytes, std::size_t& consumed_size,
        std::size_t& encoded_size)
    {
        utf8_bytes.fill(0);
        consumed_size = 0;
        encoded_size = 0;
        bool is_valid = false;

        if (utf16[0] < char16_t(0x80u))
        {
            utf8_bytes[0] = static_cast<char>(utf16[0] & 0x7Fu);
            consumed_size = 1;
            encoded_size = 1;
            is_valid = true;
        }
        else if (utf16[0] <= char16_t(0x7FFu))
        {
            uint16_t utf8_encoded = 0xC080u;
            utf8_encoded |= static_cast<uint16_t>((utf16[0] & 0x07C0u) << 2);
            utf8_encoded |= static_cast<uint16_t>(utf16[0] & 0x003Fu);
            utf8_bytes[0] = static_cast<char>((utf8_encoded & 0xFF00u) >> 8);
            utf8_bytes[1] = static_cast<char>(utf8_encoded & 0x00FFu);
            consumed_size = 1;
            encoded_size = 2;
            is_valid = true;
        }
        else if (utf16[0] < char16_t(0xD800u) || char16_t(0xE000u) <= utf16[0])
        {
            uint32_t utf8_encoded = 0xE08080u;
            utf8_encoded |= static_cast<uint32_t>((utf16[0] & 0xF000u) << 4);
            utf8_encoded |= static_cast<uint32_t>((utf16[0] & 0x0FC0u) << 2);
            utf8_encoded |= static_cast<uint32_t>(utf16[0] & 0x003Fu);
            utf8_bytes[0] = static_cast<char>((utf8_encoded & 0xFF0000u) >> 16);
            utf8_bytes[1] = static_cast<char>((utf8_encoded & 0x00FF00u) >> 8);
            utf8_bytes[2] = static_cast<char>(utf8_encoded & 0x0000FFu);
            consumed_size = 1;
            encoded_size = 3;
            is_valid = true;
        }
        else if (utf16[0] <= char16_t(0xDBFFu) && char16_t(0xDC00u) <= utf16[1] && utf16[1] <= char16_t(0xDFFFu))
        {
            // for surrogate pairs
            uint32_t code_point = 0x10000u + ((utf16[0] & 0x03FFu) << 10) + (utf16[1] & 0x03FFu);
            uint32_t utf8_encoded = 0xF0808080u;
            utf8_encoded |= static_cast<uint32_t>((code_point & 0x1C0000u) << 6);
            utf8_encoded |= static_cast<uint32_t>((code_point & 0x03F000u) << 4);
            utf8_encoded |= static_cast<uint32_t>((code_point & 0x000FC0u) << 2);
            utf8_encoded |= static_cast<uint32_t>(code_point & 0x00003Fu);
            utf8_bytes[0] = static_cast<char>((utf8_encoded & 0xFF000000u) >> 24);
            utf8_bytes[1] = static_cast<char>((utf8_encoded & 0x00FF0000u) >> 16);
            utf8_bytes[2] = static_cast<char>((utf8_encoded & 0x0000FF00u) >> 8);
            utf8_bytes[3] = static_cast<char>(utf8_encoded & 0x000000FFu);
            consumed_size = 2;
            encoded_size = 4;
            is_valid = true;
        }

        if (!is_valid)
        {
            throw invalid_encoding("Invalid UTF-16 encoding detected.", utf16);
        }
    }

    /// @brief Converts a UTF-32 encoded character to UTF-8 encoded bytes.
    /// @param[in] utf32 A UTF-32 encoded character.
    /// @param[out] utf8_bytes UTF-8 encoded bytes.
    /// @param[in] encoded_size The size of UTF-encoded bytes.
    static void from_utf32(const char32_t utf32, std::array<char, 4>& utf8_bytes, std::size_t& encoded_size)
    {
        utf8_bytes.fill(0);
        encoded_size = 0;
        bool is_valid = false;

        if (utf32 < char32_t(0x80u))
        {
            utf8_bytes[0] = static_cast<char>(utf32 & 0x007F);
            encoded_size = 1;
            is_valid = true;
        }
        else if (utf32 <= char32_t(0x7FFu))
        {
            uint16_t utf8_encoded = 0xC080u;
            utf8_encoded |= static_cast<uint16_t>((utf32 & 0x07C0u) << 2);
            utf8_encoded |= static_cast<uint16_t>(utf32 & 0x003Fu);
            utf8_bytes[0] = static_cast<char>((utf8_encoded & 0xFF00u) >> 8);
            utf8_bytes[1] = static_cast<char>(utf8_encoded & 0x00FFu);
            encoded_size = 2;
            is_valid = true;
        }
        else if (utf32 <= char32_t(0xFFFFu))
        {
            uint32_t utf8_encoded = 0xE08080u;
            utf8_encoded |= static_cast<uint32_t>((utf32 & 0xF000u) << 4);
            utf8_encoded |= static_cast<uint32_t>((utf32 & 0x0FC0u) << 2);
            utf8_encoded |= static_cast<uint32_t>(utf32 & 0x003F);
            utf8_bytes[0] = static_cast<char>((utf8_encoded & 0xFF0000u) >> 16);
            utf8_bytes[1] = static_cast<char>((utf8_encoded & 0x00FF00u) >> 8);
            utf8_bytes[2] = static_cast<char>(utf8_encoded & 0x0000FFu);
            encoded_size = 3;
            is_valid = true;
        }
        else if (utf32 <= char32_t(0x10FFFFu))
        {
            uint32_t utf8_encoded = 0xF0808080u;
            utf8_encoded |= static_cast<uint32_t>((utf32 & 0x1C0000u) << 6);
            utf8_encoded |= static_cast<uint32_t>((utf32 & 0x03F000u) << 4);
            utf8_encoded |= static_cast<uint32_t>((utf32 & 0x000FC0u) << 2);
            utf8_encoded |= static_cast<uint32_t>(utf32 & 0x00003Fu);
            utf8_bytes[0] = static_cast<char>((utf8_encoded & 0xFF000000u) >> 24);
            utf8_bytes[1] = static_cast<char>((utf8_encoded & 0x00FF0000u) >> 16);
            utf8_bytes[2] = static_cast<char>((utf8_encoded & 0x0000FF00u) >> 8);
            utf8_bytes[3] = static_cast<char>(utf8_encoded & 0x000000FFu);
            encoded_size = 4;
            is_valid = true;
        }

        if (!is_valid)
        {
            throw invalid_encoding("Invalid UTF-32 encoding detected.", utf32);
        }
    }
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ENCODINGS_UTF_ENCODING_HPP_ */