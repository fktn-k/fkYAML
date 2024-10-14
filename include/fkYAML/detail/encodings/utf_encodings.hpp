//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.13
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_ENCODINGS_UTF_ENCODINGS_HPP
#define FK_YAML_DETAIL_ENCODINGS_UTF_ENCODINGS_HPP

#include <array>
#include <cstdint>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/////////////////////////
//   UTF-8 Encoding   ///
/////////////////////////

/// @brief A class which handles UTF-8 encodings.
namespace utf8 {

/// @brief Query the number of UTF-8 character bytes with the first byte.
/// @param first_byte The first byte of a UTF-8 character.
/// @return The number of UTF-8 character bytes.
inline uint32_t get_num_bytes(uint8_t first_byte) {
    // The first byte starts with 0b0XXX'XXXX -> 1-byte character
    if (first_byte < 0x80) {
        return 1;
    }
    // The first byte starts with 0b110X'XXXX -> 2-byte character
    else if ((first_byte & 0xE0) == 0xC0) {
        return 2;
    }
    // The first byte starts with 0b1110'XXXX -> 3-byte character
    else if ((first_byte & 0xF0) == 0xE0) {
        return 3;
    }
    // The first byte starts with 0b1111'0XXX -> 4-byte character
    else if ((first_byte & 0xF8) == 0xF0) {
        return 4;
    }

    // The first byte starts with 0b10XX'XXXX or 0b1111'1XXX -> invalid
    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", {first_byte});
}

/// @brief Validates the encoding of a given byte array whose length is 1.
/// @param[in] byte_array The byte array to be validated.
/// @return true if a given byte array is valid, false otherwise.
inline bool validate(const std::initializer_list<uint8_t>& byte_array) noexcept {
    switch (byte_array.size()) {
    case 1:
        // U+0000..U+007F
        return uint8_t(*(byte_array.begin())) <= uint8_t(0x7Fu);
    case 2: {
        auto itr = byte_array.begin();
        uint8_t first = *itr++;
        uint8_t second = *itr;

        // U+0080..U+07FF
        //   1st Byte: 0xC2..0xDF
        //   2nd Byte: 0x80..0xBF
        if (uint8_t(0xC2u) <= first && first <= uint8_t(0xDFu)) {
            if (0x80 <= second && second <= 0xBF) {
                return true;
            }
        }

        // The rest of byte combinations are invalid.
        return false;
    }
    case 3: {
        auto itr = byte_array.begin();
        uint8_t first = *itr++;
        uint8_t second = *itr++;
        uint8_t third = *itr;

        // U+1000..U+CFFF:
        //   1st Byte: 0xE0..0xEC
        //   2nd Byte: 0x80..0xBF
        //   3rd Byte: 0x80..0xBF
        if (0xE0 <= first && first <= 0xEC) {
            if (0x80 <= second && second <= 0xBF) {
                if (0x80 <= third && third <= 0xBF) {
                    return true;
                }
            }
            return false;
        }

        // U+D000..U+D7FF:
        //   1st Byte: 0xED
        //   2nd Byte: 0x80..0x9F
        //   3rd Byte: 0x80..0xBF
        if (first == 0xED) {
            if (0x80 <= second && second <= 0x9F) {
                if (0x80 <= third && third <= 0xBF) {
                    return true;
                }
            }
            return false;
        }

        // U+E000..U+FFFF:
        //   1st Byte: 0xEE..0xEF
        //   2nd Byte: 0x80..0xBF
        //   3rd Byte: 0x80..0xBF
        if (first == 0xEE || first == 0xEF) {
            if (0x80 <= second && second <= 0xBF) {
                if (0x80 <= third && third <= 0xBF) {
                    return true;
                }
            }
            return false;
        }

        // The rest of byte combinations are invalid.
        return false;
    }
    case 4: {
        auto itr = byte_array.begin();
        uint8_t first = *itr++;
        uint8_t second = *itr++;
        uint8_t third = *itr++;
        uint8_t fourth = *itr;

        // U+10000..U+3FFFF:
        //   1st Byte: 0xF0
        //   2nd Byte: 0x90..0xBF
        //   3rd Byte: 0x80..0xBF
        //   4th Byte: 0x80..0xBF
        if (first == 0xF0) {
            if (0x90 <= second && second <= 0xBF) {
                if (0x80 <= third && third <= 0xBF) {
                    if (0x80 <= fourth && fourth <= 0xBF) {
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
        if (0xF1 <= first && first <= 0xF3) {
            if (0x80 <= second && second <= 0xBF) {
                if (0x80 <= third && third <= 0xBF) {
                    if (0x80 <= fourth && fourth <= 0xBF) {
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
        if (first == 0xF4) {
            if (0x80 <= second && second <= 0x8F) {
                if (0x80 <= third && third <= 0xBF) {
                    if (0x80 <= fourth && fourth <= 0xBF) {
                        return true;
                    }
                }
            }
            return false;
        }

        // The rest of byte combinations are invalid.
        return false;
    }
    default:          // LCOV_EXCL_LINE
        return false; // LCOV_EXCL_LINE
    }
}

/// @brief Converts UTF-16 encoded characters to UTF-8 encoded bytes.
/// @param[in] utf16 UTF-16 encoded character(s).
/// @param[out] utf8 UTF-8 encoded bytes.
/// @param[out] consumed_size The number of UTF-16 encoded characters used for the conversion.
/// @param[out] encoded_size The size of UTF-encoded bytes.
inline void from_utf16(
    std::array<char16_t, 2> utf16, std::array<uint8_t, 4>& utf8, uint32_t& consumed_size, uint32_t& encoded_size) {
    if (utf16[0] < char16_t(0x80u)) {
        utf8[0] = static_cast<uint8_t>(utf16[0] & 0x7Fu);
        consumed_size = 1;
        encoded_size = 1;
        return;
    }
    else if (utf16[0] <= char16_t(0x7FFu)) {
        uint16_t utf8_chunk = static_cast<uint16_t>(0xC080u) | static_cast<uint16_t>((utf16[0] & 0x07C0u) << 2) |
                              static_cast<uint16_t>(utf16[0] & 0x003Fu);
        utf8[0] = static_cast<uint8_t>((utf8_chunk & 0xFF00u) >> 8);
        utf8[1] = static_cast<uint8_t>(utf8_chunk & 0x00FFu);
        consumed_size = 1;
        encoded_size = 2;
        return;
    }
    else if (utf16[0] < char16_t(0xD800u) || char16_t(0xE000u) <= utf16[0]) {
        uint32_t utf8_chunk = static_cast<uint32_t>(0xE08080u) | static_cast<uint32_t>((utf16[0] & 0xF000u) << 4) |
                              static_cast<uint32_t>((utf16[0] & 0x0FC0u) << 2) |
                              static_cast<uint32_t>(utf16[0] & 0x003Fu);
        utf8[0] = static_cast<uint8_t>((utf8_chunk & 0xFF0000u) >> 16);
        utf8[1] = static_cast<uint8_t>((utf8_chunk & 0x00FF00u) >> 8);
        utf8[2] = static_cast<uint8_t>(utf8_chunk & 0x0000FFu);
        consumed_size = 1;
        encoded_size = 3;
        return;
    }
    else if (utf16[0] <= char16_t(0xDBFFu) && char16_t(0xDC00u) <= utf16[1] && utf16[1] <= char16_t(0xDFFFu)) {
        // for surrogate pairs
        uint32_t code_point = 0x10000u + ((utf16[0] & 0x03FFu) << 10) + (utf16[1] & 0x03FFu);
        uint32_t utf8_chunk =
            static_cast<uint32_t>(0xF0808080u) | static_cast<uint32_t>((code_point & 0x1C0000u) << 6) |
            static_cast<uint32_t>((code_point & 0x03F000u) << 4) |
            static_cast<uint32_t>((code_point & 0x000FC0u) << 2) | static_cast<uint32_t>(code_point & 0x00003Fu);
        utf8[0] = static_cast<uint8_t>((utf8_chunk & 0xFF000000u) >> 24);
        utf8[1] = static_cast<uint8_t>((utf8_chunk & 0x00FF0000u) >> 16);
        utf8[2] = static_cast<uint8_t>((utf8_chunk & 0x0000FF00u) >> 8);
        utf8[3] = static_cast<uint8_t>(utf8_chunk & 0x000000FFu);
        consumed_size = 2;
        encoded_size = 4;
        return;
    }

    throw invalid_encoding("Invalid UTF-16 encoding detected.", utf16);
}

/// @brief Converts a UTF-32 encoded character to UTF-8 encoded bytes.
/// @param[in] utf32 A UTF-32 encoded character.
/// @param[out] utf8 UTF-8 encoded bytes.
/// @param[in] encoded_size The size of UTF-encoded bytes.
inline void from_utf32(const char32_t utf32, std::array<uint8_t, 4>& utf8, uint32_t& encoded_size) {
    if (utf32 < char32_t(0x80u)) {
        utf8[0] = static_cast<uint8_t>(utf32 & 0x007F);
        encoded_size = 1;
        return;
    }
    else if (utf32 <= char32_t(0x7FFu)) {
        uint16_t utf8_chunk = static_cast<uint16_t>(0xC080u) | static_cast<uint16_t>((utf32 & 0x07C0u) << 2) |
                              static_cast<uint16_t>(utf32 & 0x003Fu);
        utf8[0] = static_cast<uint8_t>((utf8_chunk & 0xFF00u) >> 8);
        utf8[1] = static_cast<uint8_t>(utf8_chunk & 0x00FFu);
        encoded_size = 2;
        return;
    }
    else if (utf32 <= char32_t(0xFFFFu)) {
        uint32_t utf8_chunk = static_cast<uint32_t>(0xE08080u) | static_cast<uint32_t>((utf32 & 0xF000u) << 4) |
                              static_cast<uint32_t>((utf32 & 0x0FC0u) << 2) | static_cast<uint32_t>(utf32 & 0x003F);
        utf8[0] = static_cast<uint8_t>((utf8_chunk & 0xFF0000u) >> 16);
        utf8[1] = static_cast<uint8_t>((utf8_chunk & 0x00FF00u) >> 8);
        utf8[2] = static_cast<uint8_t>(utf8_chunk & 0x0000FFu);
        encoded_size = 3;
        return;
    }
    else if (utf32 <= char32_t(0x10FFFFu)) {
        uint32_t utf8_chunk = static_cast<uint32_t>(0xF0808080u) | static_cast<uint32_t>((utf32 & 0x1C0000u) << 6) |
                              static_cast<uint32_t>((utf32 & 0x03F000u) << 4) |
                              static_cast<uint32_t>((utf32 & 0x000FC0u) << 2) |
                              static_cast<uint32_t>(utf32 & 0x00003Fu);
        utf8[0] = static_cast<uint8_t>((utf8_chunk & 0xFF000000u) >> 24);
        utf8[1] = static_cast<uint8_t>((utf8_chunk & 0x00FF0000u) >> 16);
        utf8[2] = static_cast<uint8_t>((utf8_chunk & 0x0000FF00u) >> 8);
        utf8[3] = static_cast<uint8_t>(utf8_chunk & 0x000000FFu);
        encoded_size = 4;
        return;
    }

    throw invalid_encoding("Invalid UTF-32 encoding detected.", utf32);
}

} // namespace utf8

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ENCODINGS_UTF_ENCODINGS_HPP */
