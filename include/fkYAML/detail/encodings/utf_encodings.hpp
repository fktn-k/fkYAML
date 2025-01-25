//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_ENCODINGS_UTF_ENCODINGS_HPP
#define FK_YAML_DETAIL_ENCODINGS_UTF_ENCODINGS_HPP

#include <array>
#include <cstdint>

#include <fkYAML/detail/macros/define_macros.hpp>
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
    if FK_YAML_LIKELY (first_byte < 0x80) {
        return 1;
    }
    // The first byte starts with 0b110X'XXXX -> 2-byte character
    if ((first_byte & 0xE0) == 0xC0) {
        return 2;
    }
    // The first byte starts with 0b1110'XXXX -> 3-byte character
    if ((first_byte & 0xF0) == 0xE0) {
        return 3;
    }
    // The first byte starts with 0b1111'0XXX -> 4-byte character
    if ((first_byte & 0xF8) == 0xF0) {
        return 4;
    }

    // The first byte starts with 0b10XX'XXXX or 0b1111'1XXX -> invalid
    throw fkyaml::invalid_encoding("Invalid UTF-8 encoding.", {first_byte});
}

/// @brief Checks if `byte` is a valid 1-byte UTF-8 character.
/// @param[in] byte The byte value.
/// @return true if `byte` is a valid 1-byte UTF-8 character, false otherwise.
inline bool validate(uint8_t byte) noexcept {
    // U+0000..U+007F
    return byte <= 0x7Fu;
}

/// @brief Checks if the given bytes are a valid 2-byte UTF-8 character.
/// @param[in] byte0 The first byte value.
/// @param[in] byte1 The second byte value.
/// @return true if the given bytes a valid 3-byte UTF-8 character, false otherwise.
inline bool validate(uint8_t byte0, uint8_t byte1) noexcept {
    // U+0080..U+07FF
    //   1st Byte: 0xC2..0xDF
    //   2nd Byte: 0x80..0xBF
    if FK_YAML_LIKELY (0xC2u <= byte0 && byte0 <= 0xDFu) {
        if FK_YAML_LIKELY (0x80u <= byte1 && byte1 <= 0xBFu) {
            return true;
        }
    }

    // The rest of byte combinations are invalid.
    return false;
}

/// @brief Checks if the given bytes are a valid 3-byte UTF-8 character.
/// @param[in] byte0 The first byte value.
/// @param[in] byte1 The second byte value.
/// @param[in] byte2 The third byte value.
/// @return true if the given bytes a valid 2-byte UTF-8 character, false otherwise.
inline bool validate(uint8_t byte0, uint8_t byte1, uint8_t byte2) noexcept {
    // U+1000..U+CFFF:
    //   1st Byte: 0xE0..0xEC
    //   2nd Byte: 0x80..0xBF
    //   3rd Byte: 0x80..0xBF
    if (0xE0u <= byte0 && byte0 <= 0xECu) {
        if FK_YAML_LIKELY (0x80u <= byte1 && byte1 <= 0xBFu) {
            if FK_YAML_LIKELY (0x80u <= byte2 && byte2 <= 0xBFu) {
                return true;
            }
        }
        return false;
    }

    // U+D000..U+D7FF:
    //   1st Byte: 0xED
    //   2nd Byte: 0x80..0x9F
    //   3rd Byte: 0x80..0xBF
    if (byte0 == 0xEDu) {
        if FK_YAML_LIKELY (0x80u <= byte1 && byte1 <= 0x9Fu) {
            if FK_YAML_LIKELY (0x80u <= byte2 && byte2 <= 0xBFu) {
                return true;
            }
        }
        return false;
    }

    // U+E000..U+FFFF:
    //   1st Byte: 0xEE..0xEF
    //   2nd Byte: 0x80..0xBF
    //   3rd Byte: 0x80..0xBF
    if FK_YAML_LIKELY (byte0 == 0xEEu || byte0 == 0xEFu) {
        if FK_YAML_LIKELY (0x80u <= byte1 && byte1 <= 0xBFu) {
            if FK_YAML_LIKELY (0x80u <= byte2 && byte2 <= 0xBFu) {
                return true;
            }
        }
    }

    // The rest of byte combinations are invalid.
    return false;
}

/// @brief Checks if the given bytes are a valid 4-byte UTF-8 character.
/// @param[in] byte0 The first byte value.
/// @param[in] byte1 The second byte value.
/// @param[in] byte2 The third byte value.
/// @param[in] byte3 The fourth byte value.
/// @return true if the given bytes a valid 4-byte UTF-8 character, false otherwise.
inline bool validate(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3) noexcept {
    // U+10000..U+3FFFF:
    //   1st Byte: 0xF0
    //   2nd Byte: 0x90..0xBF
    //   3rd Byte: 0x80..0xBF
    //   4th Byte: 0x80..0xBF
    if (byte0 == 0xF0u) {
        if FK_YAML_LIKELY (0x90u <= byte1 && byte1 <= 0xBFu) {
            if FK_YAML_LIKELY (0x80u <= byte2 && byte2 <= 0xBFu) {
                if FK_YAML_LIKELY (0x80u <= byte3 && byte3 <= 0xBFu) {
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
    if (0xF1u <= byte0 && byte0 <= 0xF3u) {
        if FK_YAML_LIKELY (0x80u <= byte1 && byte1 <= 0xBFu) {
            if FK_YAML_LIKELY (0x80u <= byte2 && byte2 <= 0xBFu) {
                if FK_YAML_LIKELY (0x80u <= byte3 && byte3 <= 0xBFu) {
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
    if FK_YAML_LIKELY (byte0 == 0xF4u) {
        if FK_YAML_LIKELY (0x80u <= byte1 && byte1 <= 0x8Fu) {
            if FK_YAML_LIKELY (0x80u <= byte2 && byte2 <= 0xBFu) {
                if FK_YAML_LIKELY (0x80u <= byte3 && byte3 <= 0xBFu) {
                    return true;
                }
            }
        }
    }

    // The rest of byte combinations are invalid.
    return false;
}

/// @brief Converts UTF-16 encoded characters to UTF-8 encoded bytes.
/// @param[in] utf16 UTF-16 encoded character(s).
/// @param[out] utf8 UTF-8 encoded bytes.
/// @param[out] consumed_size The number of UTF-16 encoded characters used for the conversion.
/// @param[out] encoded_size The size of UTF-encoded bytes.
inline void from_utf16(
    std::array<char16_t, 2> utf16, std::array<uint8_t, 4>& utf8, uint32_t& consumed_size, uint32_t& encoded_size) {
    const auto first = utf16[0];
    const auto second = utf16[1];
    if (first < 0x80u) {
        utf8[0] = static_cast<uint8_t>(first & 0x7Fu);
        consumed_size = 1;
        encoded_size = 1;
    }
    else if (first <= 0x7FFu) {
        const auto utf8_chunk = static_cast<uint16_t>(0xC080u | ((first & 0x07C0u) << 2) | (first & 0x3Fu));
        utf8[0] = static_cast<uint8_t>(utf8_chunk >> 8);
        utf8[1] = static_cast<uint8_t>(utf8_chunk);
        consumed_size = 1;
        encoded_size = 2;
    }
    else if (first < 0xD800u || 0xE000u <= first) {
        const auto utf8_chunk =
            static_cast<uint32_t>(0xE08080u | ((first & 0xF000u) << 4) | ((first & 0x0FC0u) << 2) | (first & 0x3Fu));
        utf8[0] = static_cast<uint8_t>(utf8_chunk >> 16);
        utf8[1] = static_cast<uint8_t>(utf8_chunk >> 8);
        utf8[2] = static_cast<uint8_t>(utf8_chunk);
        consumed_size = 1;
        encoded_size = 3;
    }
    else if (first <= 0xDBFFu && 0xDC00u <= second && second <= 0xDFFFu) {
        // surrogate pair
        const uint32_t code_point = 0x10000u + ((first & 0x03FFu) << 10) + (second & 0x03FFu);
        const auto utf8_chunk = static_cast<uint32_t>(
            0xF0808080u | ((code_point & 0x1C0000u) << 6) | ((code_point & 0x03F000u) << 4) |
            ((code_point & 0x0FC0u) << 2) | (code_point & 0x3Fu));
        utf8[0] = static_cast<uint8_t>(utf8_chunk >> 24);
        utf8[1] = static_cast<uint8_t>(utf8_chunk >> 16);
        utf8[2] = static_cast<uint8_t>(utf8_chunk >> 8);
        utf8[3] = static_cast<uint8_t>(utf8_chunk);
        consumed_size = 2;
        encoded_size = 4;
    }
    else {
        throw invalid_encoding("Invalid UTF-16 encoding detected.", utf16);
    }
}

/// @brief Converts a UTF-32 encoded character to UTF-8 encoded bytes.
/// @param[in] utf32 A UTF-32 encoded character.
/// @param[out] utf8 UTF-8 encoded bytes.
/// @param[in] encoded_size The size of UTF-encoded bytes.
inline void from_utf32(const char32_t utf32, std::array<uint8_t, 4>& utf8, uint32_t& encoded_size) {
    if (utf32 < 0x80u) {
        utf8[0] = static_cast<uint8_t>(utf32 & 0x007F);
        encoded_size = 1;
    }
    else if (utf32 <= 0x7FFu) {
        const auto utf8_chunk = static_cast<uint16_t>(0xC080u | ((utf32 & 0x07C0u) << 2) | (utf32 & 0x3Fu));
        utf8[0] = static_cast<uint8_t>(utf8_chunk >> 8);
        utf8[1] = static_cast<uint8_t>(utf8_chunk);
        encoded_size = 2;
    }
    else if (utf32 <= 0xFFFFu) {
        const auto utf8_chunk =
            static_cast<uint32_t>(0xE08080u | ((utf32 & 0xF000u) << 4) | ((utf32 & 0x0FC0u) << 2) | (utf32 & 0x3F));
        utf8[0] = static_cast<uint8_t>(utf8_chunk >> 16);
        utf8[1] = static_cast<uint8_t>(utf8_chunk >> 8);
        utf8[2] = static_cast<uint8_t>(utf8_chunk);
        encoded_size = 3;
    }
    else if (utf32 <= 0x10FFFFu) {
        const auto utf8_chunk = static_cast<uint32_t>(
            0xF0808080u | ((utf32 & 0x1C0000u) << 6) | ((utf32 & 0x03F000u) << 4) | ((utf32 & 0x0FC0u) << 2) |
            (utf32 & 0x3Fu));
        utf8[0] = static_cast<uint8_t>(utf8_chunk >> 24);
        utf8[1] = static_cast<uint8_t>(utf8_chunk >> 16);
        utf8[2] = static_cast<uint8_t>(utf8_chunk >> 8);
        utf8[3] = static_cast<uint8_t>(utf8_chunk);
        encoded_size = 4;
    }
    else {
        throw invalid_encoding("Invalid UTF-32 encoding detected.", utf32);
    }
}

} // namespace utf8

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ENCODINGS_UTF_ENCODINGS_HPP */
