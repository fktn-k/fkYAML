///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.9
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
#include <fkYAML/detail/meta/input_adapter_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Detect an encoding type for UTF-8 expected inputs.
/// @note This function doesn't support the case where the first character is null.
/// @param[in] bytes 4 bytes of an input character sequence.
/// @param[out] has_bom Whether or not the input contains a BOM.
/// @return A detected encoding type.
inline utf_encode_t detect_encoding_type(const std::array<uint8_t, 4>& bytes, bool& has_bom) noexcept {
    has_bom = false;

    switch (bytes[0]) {
    case uint8_t(0):
        if (bytes[1] == 0) {
            if (bytes[2] == uint8_t(0xFEu)) {
                if (bytes[3] == uint8_t(0xFFu)) {
                    has_bom = true;
                    return utf_encode_t::UTF_32BE;
                }
            }
            else if (bytes[2] == 0 && 0 < bytes[3] && bytes[3] < uint8_t(0x80u)) {
                return utf_encode_t::UTF_32BE;
            }
        }
        else if (bytes[1] < uint8_t(0x80u)) {
            return utf_encode_t::UTF_16BE;
        }
        break;
    case uint8_t(0xEFu):
        has_bom = (bytes[1] == uint8_t(0xBBu) && bytes[2] == uint8_t(0xBFu));
        break;
    case uint8_t(0xFEu):
        if (bytes[1] == uint8_t(0xFFu)) {
            has_bom = true;
            return utf_encode_t::UTF_16BE;
        }
        break;
    case uint8_t(0xFFu):
        if (bytes[1] == uint8_t(0xFEu)) {
            has_bom = true;
            return (bytes[2] == 0 && bytes[3] == 0) ? utf_encode_t::UTF_32LE : utf_encode_t::UTF_16LE;
        }
        break;
    default:
        if (bytes[0] < uint8_t(0x80u)) {
            if (bytes[1] == 0) {
                return (bytes[2] == 0 && bytes[3] == 0) ? utf_encode_t::UTF_32LE : utf_encode_t::UTF_16LE;
            }
        }
        break;
    }
    return utf_encode_t::UTF_8;
}

/// @brief A class which detects UTF encoding and the existence of a BOM from iterators.
/// @tparam ItrType An iterator type.
template <typename ItrType, typename = void>
struct encode_detector {};

/// @brief The partial specialization of encode_detector for char iterators.
/// @tparam ItrType An iterator type.
template <typename ItrType>
struct encode_detector<ItrType, enable_if_t<is_iterator_of<ItrType, char>::value>> {
    /// @brief Detects UTF encoding type and the existence of a BOM.
    /// @note The detection is based on the list at https://yaml.org/spec/1.2.2/#52-character-encodings.
    /// @param begin The beginning of input iterators.
    /// @param end The end of input iterators.
    /// @return The detected UTF encoding type.
    static utf_encode_t detect(ItrType& begin, const ItrType& end) noexcept {
        if (begin == end) {
            return utf_encode_t::UTF_8;
        }

        std::array<uint8_t, 4> bytes = {{0xFFu, 0xFFu, 0xFFu, 0xFFu}};
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

#ifdef FK_YAML_HAS_CHAR8_T

/// @brief The partial specialization of encode_detector for char8_t iterators.
/// @tparam ItrType An iterator type.
template <typename ItrType>
struct encode_detector<ItrType, enable_if_t<is_iterator_of<ItrType, char8_t>::value>> {
    static utf_encode_t detect(ItrType& begin, const ItrType& end) {
        std::array<uint8_t, 4> bytes = {{0xFFu, 0xFFu, 0xFFu, 0xFFu}};
        for (int i = 0; i < 4 && begin + i != end; i++) {
            bytes[i] = uint8_t(begin[i]);
        }

        bool has_bom = false;
        utf_encode_t encode_type = detect_encoding_type(bytes, has_bom);

        if (encode_type != utf_encode_t::UTF_8) {
            throw exception("char16_t characters must be encoded in the UTF-16 format.");
        }

        if (has_bom) {
            std::advance(begin, 3);
        }

        return encode_type;
    }
};

#endif // defined(FK_YAML_HAS_CHAR8_T)

/// @brief The partial specialization of encode_detector for char16_t iterators.
/// @tparam ItrType An iterator type.
template <typename ItrType>
struct encode_detector<ItrType, enable_if_t<is_iterator_of<ItrType, char16_t>::value>> {
    /// @brief Detects UTF encoding type and the existence of a BOM.
    /// @param begin The beginning of input iterators.
    /// @param end The end of input iterators.
    /// @return The detected UTF encoding type.
    static utf_encode_t detect(ItrType& begin, const ItrType& end) {
        if (begin == end) {
            return utf_encode_t::UTF_16BE;
        }

        std::array<uint8_t, 4> bytes = {{0xFFu, 0xFFu, 0xFFu, 0xFFu}};
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
};

/// @brief The partial specialization of encode_detector for char32_t iterators.
/// @tparam ItrType An iterator type.
template <typename ItrType>
struct encode_detector<ItrType, enable_if_t<is_iterator_of<ItrType, char32_t>::value>> {
    /// @brief Detects UTF encoding type and the existence of a BOM.
    /// @param begin The beginning of input iterators.
    /// @param end The end of input iterators.
    /// @return The detected UTF encoding type.
    static utf_encode_t detect(ItrType& begin, const ItrType& end) {
        if (begin == end) {
            return utf_encode_t::UTF_32BE;
        }

        std::array<uint8_t, 4> bytes = {{0xFFu, 0xFFu, 0xFFu, 0xFFu}};
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
};

/// @brief A class which detects UTF encoding and the existence of a BOM from a C-style file handle.
struct file_encode_detector {
    /// @brief Detects UTF encoding type and the existence of a BOM.
    /// @param file The file handle.
    /// @return The detected UTF encoding type.
    static utf_encode_t detect(std::FILE* file) noexcept {
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
};

/// @brief A class which detects UTF encoding and the existence of a BOM from an input stream.
struct stream_encode_detector {
    /// @brief Detects UTF encoding type and the existence of a BOM.
    /// @param is The input stream.
    /// @return The detected UTF encoding type.
    static utf_encode_t detect(std::istream& is) noexcept {
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
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ENCODINGS_ENCODE_DETECTOR_HPP_ */
