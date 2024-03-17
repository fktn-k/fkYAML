///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_ENCODINGS_ENCODE_DETECTOR_HPP_
#define FK_YAML_DETAIL_ENCODINGS_ENCODE_DETECTOR_HPP_

#include <cstdint>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/encodings/encode_t.hpp>
#include <fkYAML/exception.hpp>

/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief Detect an encoding type for UTF-8 expected inputs.
/// @note This function doesn't support the case where the first character is null.
/// @param b0 The 1st byte of an input character sequence.
/// @param b1 The 2nd byte of an input character sequence.
/// @param b2 The 3rd byte of an input character sequence.
/// @param b3 The 4th byte of an input character sequence.
/// @return A detected encoding type.
inline encode_t detect_encoding_type(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) noexcept
{
    // Check if a BOM exists.

    if (b0 == uint8_t(0xEFu) && b1 == uint8_t(0xBBu) && b2 == uint8_t(0xBFu))
    {
        return encode_t::UTF_8_BOM;
    }

    if (b0 == 0 && b1 == 0 && b2 == uint8_t(0xFEu) && b3 == uint8_t(0xFFu))
    {
        return encode_t::UTF_32BE_BOM;
    }

    if (b0 == uint8_t(0xFFu) && b1 == uint8_t(0xFEu) && b2 == 0 && b3 == 0)
    {
        return encode_t::UTF_32LE_BOM;
    }

    if (b0 == uint8_t(0xFEu) && b1 == uint8_t(0xFFu))
    {
        return encode_t::UTF_16BE_BOM;
    }

    if (b0 == uint8_t(0xFFu) && b1 == uint8_t(0xFEu))
    {
        return encode_t::UTF_16LE_BOM;
    }

    // Test the first character assuming it's an ASCII character.

    if (b0 == 0 && b1 == 0 && b2 == 0 && 0 < b3 && b3 < uint8_t(0x80u))
    {
        return encode_t::UTF_32BE_N;
    }

    if (0 < b0 && b0 < uint8_t(0x80u) && b1 == 0 && b2 == 0 && b3 == 0)
    {
        return encode_t::UTF_32LE_N;
    }

    if (b0 == 0 && 0 < b1 && b1 < uint8_t(0x80u))
    {
        return encode_t::UTF_16BE_N;
    }

    if (0 < b0 && b0 < uint8_t(0x80u) && b1 == 0)
    {
        return encode_t::UTF_16LE_N;
    }

    return encode_t::UTF_8_N;
}

/// @brief Detects the encoding type of the input, and consumes a BOM if it exists.
/// @tparam ItrType Type of iterators for the input.
/// @tparam ElemSize The size of one input element.
/// @param begin The beginning of input iterators.
/// @param end The end of input iterators.
/// @return A detected encoding type.
template <typename ItrType, size_t ElemSize = sizeof(decltype(*(std::declval<ItrType>())))>
inline encode_t detect_encoding_and_skip_bom(ItrType& begin, const ItrType& end)
{
    uint8_t bytes[4] = {0xFFu, 0xFFu, 0xFFu, 0xFFu};
    switch (ElemSize)
    {
    case sizeof(char): { // this case covers char8_t as well when compiled with C++20 features.
        for (std::size_t i = 0; i < 4 && begin + i != end; i++)
        {
            bytes[i] = uint8_t(begin[i]);
        }

        encode_t encode_type = detect_encoding_type(bytes[0], bytes[1], bytes[2], bytes[3]);
        switch (encode_type)
        {
        case encode_t::UTF_8_BOM:
            std::advance(begin, 3);
            break;
        case encode_t::UTF_16BE_BOM:
        case encode_t::UTF_16LE_BOM:
            std::advance(begin, 2);
            break;
        case encode_t::UTF_32BE_BOM:
        case encode_t::UTF_32LE_BOM:
            std::advance(begin, 4);
            break;
        default:
            // Do nothing if a BOM doesn't exist.
            break;
        }
        return encode_type;
    }
    case sizeof(char16_t): {
        if (begin == end)
        {
            return encode_t::UTF_16BE_N;
        }
        for (int i = 0; i < 2 && begin + i != end; i++)
        {
            bytes[i * 2] = uint8_t((begin[i] & 0xFF00u) >> 8);
            bytes[i * 2 + 1] = uint8_t(begin[i] & 0xFFu);
        }

        encode_t encode_type = detect_encoding_type(bytes[0], bytes[1], bytes[2], bytes[3]);
        switch (encode_type)
        {
        case encode_t::UTF_16BE_BOM:
        case encode_t::UTF_16LE_BOM:
            std::advance(begin, 1);
            break;
        case encode_t::UTF_16BE_N:
        case encode_t::UTF_16LE_N:
            // Do nothing if a BOM doesn't exist.
            break;
        default:
            throw exception("char16_t characters must be encoded in the UTF-16 format.");
        }
        return encode_type;
    }
    case sizeof(char32_t): {
        if (begin == end)
        {
            return encode_t::UTF_32BE_N;
        }
        bytes[0] = uint8_t((*begin & 0xFF000000u) >> 24);
        bytes[1] = uint8_t((*begin & 0x00FF0000u) >> 16);
        bytes[2] = uint8_t((*begin & 0x0000FF00u) >> 8);
        bytes[3] = uint8_t(*begin & 0x000000FFu);
        encode_t encode_type = detect_encoding_type(bytes[0], bytes[1], bytes[2], bytes[3]);
        switch (encode_type)
        {
        case encode_t::UTF_32BE_BOM:
        case encode_t::UTF_32LE_BOM:
            std::advance(begin, 1);
            break;
        case encode_t::UTF_32BE_N:
        case encode_t::UTF_32LE_N:
            // Do nothing if a BOM doesn't exist.
            break;
        default:
            throw exception("char32_t characters must be encoded in the UTF-32 format.");
        }
        return encode_type;
    }
    default:
        throw exception("Unknown char size.");
    }
}

inline encode_t detect_encoding_and_skip_bom(std::FILE* file) noexcept
{
    uint8_t bytes[4] = {0xFFu, 0xFFu, 0xFFu, 0xFFu};
    for (std::size_t i = 0; i < 4; i++)
    {
        char byte = 0;
        std::size_t size = std::fread(&byte, sizeof(char), 1, file);
        if (size != sizeof(char))
        {
            break;
        }
        bytes[i] = uint8_t(byte & 0xFF);
    }

    encode_t encode_type = detect_encoding_type(bytes[0], bytes[1], bytes[2], bytes[3]);
    switch (encode_type)
    {
    case encode_t::UTF_8_BOM:
        fseek(file, 3, SEEK_SET);
        break;
    case encode_t::UTF_16BE_BOM:
    case encode_t::UTF_16LE_BOM:
        fseek(file, 2, SEEK_SET);
        break;
    case encode_t::UTF_32BE_BOM:
    case encode_t::UTF_32LE_BOM:
        fseek(file, 4, SEEK_SET);
        break;
    default:
        // Move back to the beginning of the file contents if a BOM doesn't exist.
        fseek(file, 0, SEEK_SET);
        break;
    }

    return encode_type;
}

inline encode_t detect_encoding_and_skip_bom(std::istream& is) noexcept
{
    uint8_t bytes[4] = {0xFFu, 0xFFu, 0xFFu, 0xFFu};
    for (std::size_t i = 0; i < 4; i++)
    {
        char ch = 0;
        is.read(&ch, 1);
        std::streamsize size = is.gcount();
        if (size != 1)
        {
            // without this, seekg() fails in the switch-case statement below.
            is.clear();
            break;
        }
        bytes[i] = uint8_t(ch & 0xFF);
    }

    encode_t encode_type = detect_encoding_type(bytes[0], bytes[1], bytes[2], bytes[3]);
    switch (encode_type)
    {
    case encode_t::UTF_8_BOM:
        is.seekg(3, std::ios_base::beg);
        break;
    case encode_t::UTF_16BE_BOM:
    case encode_t::UTF_16LE_BOM:
        is.seekg(2, std::ios_base::beg);
        break;
    case encode_t::UTF_32BE_BOM:
    case encode_t::UTF_32LE_BOM:
        is.seekg(4, std::ios_base::beg);
        break;
    default:
        // Move back to the beginning of the file contents if a BOM doesn't exist.
        is.seekg(0, std::ios_base::beg);
        break;
    }

    return encode_type;
}

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ENCODINGS_ENCODE_DETECTOR_HPP_ */