//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.13
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_ENCODINGS_UTF_ENCODE_DETECTOR_HPP
#define FK_YAML_DETAIL_ENCODINGS_UTF_ENCODE_DETECTOR_HPP

#include <cstdint>
#include <istream>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/encodings/utf_encode_t.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Detect an encoding type for UTF-8 expected inputs.
/// @note This function doesn't support the case where the first character is null.
/// @param[in] bytes 4 bytes of an input character sequence.
/// @param[out] has_bom Whether the input contains a BOM.
/// @return A detected encoding type.
inline utf_encode_t detect_encoding_type(const std::array<uint8_t, 4>& bytes, bool& has_bom) noexcept {
    has_bom = false;

    uint8_t byte0 = bytes[0];
    uint8_t byte1 = bytes[1];
    uint8_t byte2 = bytes[2];
    uint8_t byte3 = bytes[3];

    // Check if a BOM exists.

    if (byte0 == uint8_t(0xEFu) && byte1 == uint8_t(0xBBu) && byte2 == uint8_t(0xBFu)) {
        has_bom = true;
        return utf_encode_t::UTF_8;
    }

    if (byte0 == 0 && byte1 == 0 && byte2 == uint8_t(0xFEu) && byte3 == uint8_t(0xFFu)) {
        has_bom = true;
        return utf_encode_t::UTF_32BE;
    }

    if (byte0 == uint8_t(0xFFu) && byte1 == uint8_t(0xFEu) && byte2 == 0 && byte3 == 0) {
        has_bom = true;
        return utf_encode_t::UTF_32LE;
    }

    if (byte0 == uint8_t(0xFEu) && byte1 == uint8_t(0xFFu)) {
        has_bom = true;
        return utf_encode_t::UTF_16BE;
    }

    if (byte0 == uint8_t(0xFFu) && byte1 == uint8_t(0xFEu)) {
        has_bom = true;
        return utf_encode_t::UTF_16LE;
    }

    // Test the first character assuming it's an ASCII character.

    if (byte0 == 0 && byte1 == 0 && byte2 == 0 && 0 < byte3 && byte3 < uint8_t(0x80u)) {
        return utf_encode_t::UTF_32BE;
    }

    if (0 < byte0 && byte0 < uint8_t(0x80u) && byte1 == 0 && byte2 == 0 && byte3 == 0) {
        return utf_encode_t::UTF_32LE;
    }

    if (byte0 == 0 && 0 < byte1 && byte1 < uint8_t(0x80u)) {
        return utf_encode_t::UTF_16BE;
    }

    if (0 < byte0 && byte0 < uint8_t(0x80u) && byte1 == 0) {
        return utf_encode_t::UTF_16LE;
    }

    return utf_encode_t::UTF_8;
}

/// @brief A class which detects UTF encoding type and the existence of a BOM at the beginning.
/// @tparam ItrType Type of iterators for the input.
template <typename ItrType, typename = void>
struct utf_encode_detector {};

/// @brief The partial specialization of utf_encode_detector for char iterators.
/// @tparam ItrType An iterator type.
template <typename ItrType>
struct utf_encode_detector<ItrType, enable_if_t<is_iterator_of<ItrType, char>::value>> {
    /// @brief Detects the encoding type of the input, and consumes a BOM if it exists.
    /// @param begin The iterator to the first element of an input.
    /// @param end The iterator to the past-the end element of an input.
    /// @return A detected encoding type.
    static utf_encode_t detect(ItrType& begin, const ItrType& end) noexcept {
        if FK_YAML_UNLIKELY (begin == end) {
            return utf_encode_t::UTF_8;
        }

        std::array<uint8_t, 4> bytes {};
        bytes.fill(0xFFu);
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
};

#if FK_YAML_HAS_CHAR8_T

/// @brief The partial specialization of utf_encode_detector for char8_t iterators.
/// @tparam ItrType An iterator type.
template <typename ItrType>
struct utf_encode_detector<ItrType, enable_if_t<is_iterator_of<ItrType, char8_t>::value>> {
    /// @brief Detects the encoding type of the input, and consumes a BOM if it exists.
    /// @param begin The iterator to the first element of an input.
    /// @param end The iterator to the past-the end element of an input.
    /// @return A detected encoding type.
    static utf_encode_t detect(ItrType& begin, const ItrType& end) {
        if FK_YAML_UNLIKELY (begin == end) {
            return utf_encode_t::UTF_8;
        }

        std::array<uint8_t, 4> bytes {};
        bytes.fill(0xFFu);
        for (int i = 0; i < 4 && begin + i != end; i++) {
            bytes[i] = uint8_t(begin[i]);
        }

        bool has_bom = false;
        utf_encode_t encode_type = detect_encoding_type(bytes, has_bom);

        if FK_YAML_UNLIKELY (encode_type != utf_encode_t::UTF_8) {
            throw exception("char8_t characters must be encoded in the UTF-8 format.");
        }

        if (has_bom) {
            // skip reading the BOM.
            std::advance(begin, 3);
        }

        return encode_type;
    }
};

#endif // FK_YAML_HAS_CHAR8_T

/// @brief The partial specialization of utf_encode_detector for char16_t iterators.
/// @tparam ItrType An iterator type.
template <typename ItrType>
struct utf_encode_detector<ItrType, enable_if_t<is_iterator_of<ItrType, char16_t>::value>> {
    /// @brief Detects the encoding type of the input, and consumes a BOM if it exists.
    /// @param begin The iterator to the first element of an input.
    /// @param end The iterator to the past-the end element of an input.
    /// @return A detected encoding type.
    static utf_encode_t detect(ItrType& begin, const ItrType& end) {
        if FK_YAML_UNLIKELY (begin == end) {
            return utf_encode_t::UTF_16BE;
        }

        std::array<uint8_t, 4> bytes {};
        bytes.fill(0xFFu);
        for (int i = 0; i < 2 && begin + i != end; i++) {
            char16_t elem = begin[i];
            bytes[i * 2] = uint8_t((elem & 0xFF00u) >> 8);
            bytes[i * 2 + 1] = uint8_t(elem & 0xFFu);
        }

        bool has_bom = false;
        utf_encode_t encode_type = detect_encoding_type(bytes, has_bom);

        if FK_YAML_UNLIKELY (encode_type != utf_encode_t::UTF_16BE && encode_type != utf_encode_t::UTF_16LE) {
            throw exception("char16_t characters must be encoded in the UTF-16 format.");
        }

        if (has_bom) {
            // skip reading the BOM.
            std::advance(begin, 1);
        }

        return encode_type;
    }
};

/// @brief The partial specialization of utf_encode_detector for char32_t iterators.
/// @tparam ItrType An iterator type.
template <typename ItrType>
struct utf_encode_detector<ItrType, enable_if_t<is_iterator_of<ItrType, char32_t>::value>> {
    /// @brief Detects the encoding type of the input, and consumes a BOM if it exists.
    /// @param begin The iterator to the first element of an input.
    /// @param end The iterator to the past-the end element of an input.
    /// @return A detected encoding type.
    static utf_encode_t detect(ItrType& begin, const ItrType& end) {
        if FK_YAML_UNLIKELY (begin == end) {
            return utf_encode_t::UTF_32BE;
        }

        std::array<uint8_t, 4> bytes {};
        char32_t elem = *begin;
        bytes[0] = uint8_t((elem & 0xFF000000u) >> 24);
        bytes[1] = uint8_t((elem & 0x00FF0000u) >> 16);
        bytes[2] = uint8_t((elem & 0x0000FF00u) >> 8);
        bytes[3] = uint8_t(elem & 0x000000FFu);

        bool has_bom = false;
        utf_encode_t encode_type = detect_encoding_type(bytes, has_bom);

        if FK_YAML_UNLIKELY (encode_type != utf_encode_t::UTF_32BE && encode_type != utf_encode_t::UTF_32LE) {
            throw exception("char32_t characters must be encoded in the UTF-32 format.");
        }

        if (has_bom) {
            // skip reading the BOM.
            std::advance(begin, 1);
        }

        return encode_type;
    }
};

/// @brief A class which detects UTF encoding type and the existence of a BOM from the input file.
struct file_utf_encode_detector {
    /// @brief Detects the encoding type of the input, and consumes a BOM if it exists.
    /// @param p_file The input file handle.
    /// @return A detected encoding type.
    static utf_encode_t detect(std::FILE* p_file) noexcept {
        std::array<uint8_t, 4> bytes {};
        bytes.fill(0xFFu);
        for (int i = 0; i < 4; i++) {
            char byte = 0;
            std::size_t size = std::fread(&byte, sizeof(char), 1, p_file);
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
        std::fseek(p_file, offset, SEEK_SET);

        return encode_type;
    }
};

/// @brief A class which detects UTF encoding type and the existence of a BOM from the input file.
struct stream_utf_encode_detector {
    /// @brief Detects the encoding type of the input, and consumes a BOM if it exists.
    /// @param p_file The input file handle.
    /// @return A detected encoding type.
    static utf_encode_t detect(std::istream& is) noexcept {
        std::array<uint8_t, 4> bytes {};
        bytes.fill(0xFFu);
        for (int i = 0; i < 4; i++) {
            char ch = 0;
            is.read(&ch, 1);
            std::streamsize size = is.gcount();
            if (size != 1) {
                // without this, seekg() will fail.
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
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ENCODINGS_UTF_ENCODE_DETECTOR_HPP */
