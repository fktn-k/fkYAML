///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_ENCODINGS_ENCODE_DETECTOR_HPP_
#define FK_YAML_DETAIL_ENCODINGS_ENCODE_DETECTOR_HPP_

#include <cstdint>
#include <istream>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/encodings/utf_encode_t.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Detect an encoding type for UTF-8 expected inputs.
/// @note This function doesn't support the case where the first character is null.
/// @param[in] bytes 4 bytes of an input character sequence.
/// @param[out] has_bom Whether or not the input contains a BOM.
/// @return A detected encoding type.
inline utf_encode_t detect_encoding_type(const std::array<uint8_t, 4>& bytes, bool& has_bom) noexcept {
    has_bom = false;

    // Check if a BOM exists.

    if (bytes[0] == uint8_t(0xEFu) && bytes[1] == uint8_t(0xBBu) && bytes[2] == uint8_t(0xBFu)) {
        has_bom = true;
        return utf_encode_t::UTF_8;
    }

    if (bytes[0] == 0 && bytes[1] == 0 && bytes[2] == uint8_t(0xFEu) && bytes[3] == uint8_t(0xFFu)) {
        has_bom = true;
        return utf_encode_t::UTF_32BE;
    }

    if (bytes[0] == uint8_t(0xFFu) && bytes[1] == uint8_t(0xFEu) && bytes[2] == 0 && bytes[3] == 0) {
        has_bom = true;
        return utf_encode_t::UTF_32LE;
    }

    if (bytes[0] == uint8_t(0xFEu) && bytes[1] == uint8_t(0xFFu)) {
        has_bom = true;
        return utf_encode_t::UTF_16BE;
    }

    if (bytes[0] == uint8_t(0xFFu) && bytes[1] == uint8_t(0xFEu)) {
        has_bom = true;
        return utf_encode_t::UTF_16LE;
    }

    // Test the first character assuming it's an ASCII character.

    if (bytes[0] == 0 && bytes[1] == 0 && bytes[2] == 0 && 0 < bytes[3] && bytes[3] < uint8_t(0x80u)) {
        return utf_encode_t::UTF_32BE;
    }

    if (0 < bytes[0] && bytes[0] < uint8_t(0x80u) && bytes[1] == 0 && bytes[2] == 0 && bytes[3] == 0) {
        return utf_encode_t::UTF_32LE;
    }

    if (bytes[0] == 0 && 0 < bytes[1] && bytes[1] < uint8_t(0x80u)) {
        return utf_encode_t::UTF_16BE;
    }

    if (0 < bytes[0] && bytes[0] < uint8_t(0x80u) && bytes[1] == 0) {
        return utf_encode_t::UTF_16LE;
    }

    return utf_encode_t::UTF_8;
}

/// @brief Detects the encoding type of the input, and consumes a BOM if it exists.
/// @tparam ItrType Type of iterators for the input.
/// @tparam ElemSize The size of one input element.
/// @param begin The beginning of input iterators.
/// @param end The end of input iterators.
/// @return A detected encoding type.
template <typename ItrType, size_t ElemSize = sizeof(decltype(*(std::declval<ItrType>())))>
inline utf_encode_t detect_encoding_and_skip_bom(ItrType& begin, const ItrType& end) {
    std::array<uint8_t, 4> bytes = {{0xFFu, 0xFFu, 0xFFu, 0xFFu}};
    switch (ElemSize) {
    case sizeof(char): { // this case covers char8_t as well when compiled with C++20 or better.
        for (int i = 0; i < 4 && begin + i != end; i++) {
            bytes[i] = uint8_t(begin[i]);
        }

        bool has_bom = false;
        utf_encode_t encode_type = detect_encoding_type(bytes, has_bom);

        if (has_bom) {
            // skip reading the BOM.
            switch (encode_type) {
            case utf_encode_t::UTF_8:
                std::advance(begin, 3);
                break;
            case utf_encode_t::UTF_16BE:
            case utf_encode_t::UTF_16LE:
                std::advance(begin, 2);
                break;
            case utf_encode_t::UTF_32BE:
            case utf_encode_t::UTF_32LE:
                std::advance(begin, 4);
                break;
            }
        }

        return encode_type;
    }
    case sizeof(char16_t): {
        if (begin == end) {
            return utf_encode_t::UTF_16BE;
        }
        for (int i = 0; i < 2 && begin + i != end; i++) {
            bytes[i * 2] = uint8_t((begin[i] & 0xFF00u) >> 8);
            bytes[i * 2 + 1] = uint8_t(begin[i] & 0xFFu);
        }

        bool has_bom = false;
        utf_encode_t encode_type = detect_encoding_type(bytes, has_bom);

        if (encode_type != utf_encode_t::UTF_16BE && encode_type != utf_encode_t::UTF_16LE) {
            throw exception("char16_t characters must be encoded in the UTF-16 format.");
        }

        if (has_bom) {
            // skip reading the BOM.
            std::advance(begin, 1);
        }

        return encode_type;
    }
    case sizeof(char32_t): {
        if (begin == end) {
            return utf_encode_t::UTF_32BE;
        }

        bytes[0] = uint8_t((*begin & 0xFF000000u) >> 24);
        bytes[1] = uint8_t((*begin & 0x00FF0000u) >> 16);
        bytes[2] = uint8_t((*begin & 0x0000FF00u) >> 8);
        bytes[3] = uint8_t(*begin & 0x000000FFu);

        bool has_bom = false;
        utf_encode_t encode_type = detect_encoding_type(bytes, has_bom);

        if (encode_type != utf_encode_t::UTF_32BE && encode_type != utf_encode_t::UTF_32LE) {
            throw exception("char32_t characters must be encoded in the UTF-32 format.");
        }

        if (has_bom) {
            // skip reading the BOM.
            std::advance(begin, 1);
        }

        return encode_type;
    }
    default:
        throw exception("Unknown char size.");
    }
}

inline utf_encode_t detect_encoding_and_skip_bom(std::FILE* file) noexcept {
    std::array<uint8_t, 4> bytes = {{0xFFu, 0xFFu, 0xFFu, 0xFFu}};
    for (int i = 0; i < 4; i++) {
        char byte = 0;
        std::size_t size = std::fread(&byte, sizeof(char), 1, file);
        if (size != sizeof(char)) {
            break;
        }
        bytes[i] = uint8_t(byte & 0xFF);
    }

    bool has_bom = false;
    utf_encode_t encode_type = detect_encoding_type(bytes, has_bom);

    // move back to the beginning if a BOM doesn't exist.
    long offset = 0;
    if (has_bom) {
        switch (encode_type) {
        case utf_encode_t::UTF_8:
            offset = 3;
            break;
        case utf_encode_t::UTF_16BE:
        case utf_encode_t::UTF_16LE:
            offset = 2;
            break;
        case utf_encode_t::UTF_32BE:
        case utf_encode_t::UTF_32LE:
            offset = 4;
            break;
        }
    }
    fseek(file, offset, SEEK_SET);

    return encode_type;
}

inline utf_encode_t detect_encoding_and_skip_bom(std::istream& is) noexcept {
    std::array<uint8_t, 4> bytes = {{0xFFu, 0xFFu, 0xFFu, 0xFFu}};
    for (int i = 0; i < 4; i++) {
        char ch = 0;
        is.read(&ch, 1);
        std::streamsize size = is.gcount();
        if (size != 1) {
            // without this, seekg() fails in the switch-case statement below.
            is.clear();
            break;
        }
        bytes[i] = uint8_t(ch & 0xFF);
    }

    bool has_bom = false;
    utf_encode_t encode_type = detect_encoding_type(bytes, has_bom);

    // move back to the beginning if a BOM doesn't exist.
    std::streamoff offset = 0;
    if (has_bom) {
        switch (encode_type) {
        case utf_encode_t::UTF_8:
            offset = 3;
            break;
        case utf_encode_t::UTF_16BE:
        case utf_encode_t::UTF_16LE:
            offset = 2;
            break;
        case utf_encode_t::UTF_32BE:
        case utf_encode_t::UTF_32LE:
            offset = 4;
            break;
        }
    }
    is.seekg(offset, std::ios_base::beg);

    return encode_type;
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ENCODINGS_ENCODE_DETECTOR_HPP_ */
