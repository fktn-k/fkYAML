///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_ENCODINGS_ENCODE_T_HPP_
#define FK_YAML_DETAIL_ENCODINGS_ENCODE_T_HPP_

#include <fkYAML/detail/macros/version_macros.hpp>

/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief Definition of Unicode encoding types
/// @note Since fkYAML doesn't treat UTF-16/UTF-32 encoded characters per byte, endians do not matter.
enum class encode_t
{
    UTF_8_N,      //!< UTF-8 without BOM
    UTF_8_BOM,    //!< UTF-8 with BOM
    UTF_16BE_N,   //!< UTF-16BE without BOM
    UTF_16BE_BOM, //!< UTF-16BE with BOM
    UTF_16LE_N,   //!< UTF-16LE without BOM
    UTF_16LE_BOM, //!< UTF-16LE with BOM
    UTF_32BE_N,   //!< UTF-32BE without BOM
    UTF_32BE_BOM, //!< UTF-32BE with BOM
    UTF_32LE_N,   //!< UTF-32LE without BOM
    UTF_32LE_BOM, //!< UTF-32LE with BOM
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ENCODINGS_ENCODE_T_HPP_ */