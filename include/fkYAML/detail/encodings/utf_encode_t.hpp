//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_ENCODINGS_UTF_ENCODE_T_HPP
#define FK_YAML_DETAIL_ENCODINGS_UTF_ENCODE_T_HPP

#include <cstdint>

#include <fkYAML/detail/macros/define_macros.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Definition of Unicode encoding types
/// @note Since fkYAML doesn't treat UTF-16/UTF-32 encoded characters per byte, endians do not matter.
enum class utf_encode_t : std::uint8_t {
    UTF_8,    //!< UTF-8
    UTF_16BE, //!< UTF-16 Big Endian
    UTF_16LE, //!< UTF-16 Little Endian
    UTF_32BE, //!< UTF-32 Big Endian
    UTF_32LE, //!< UTF-32 Little Endian
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ENCODINGS_UTF_ENCODE_T_HPP */
