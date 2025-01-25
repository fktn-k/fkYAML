//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_INPUT_TAG_T_HPP
#define FK_YAML_DETAIL_INPUT_TAG_T_HPP

#include <cstdint>

#include <fkYAML/detail/macros/define_macros.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Definition of YAML tag types.
enum class tag_t : std::uint8_t {
    NONE,            //!< Represents a non-specific tag "?".
    NON_SPECIFIC,    //!< Represents a non-specific tag "!".
    CUSTOM_TAG,      //!< Represents a custom tag
    SEQUENCE,        //!< Represents a sequence tag.
    MAPPING,         //!< Represents a mapping tag.
    NULL_VALUE,      //!< Represents a null value tag.
    BOOLEAN,         //!< Represents a boolean tag.
    INTEGER,         //!< Represents an integer type
    FLOATING_NUMBER, //!< Represents a floating point number tag.
    STRING,          //!< Represents a string tag.
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_TAG_T_HPP */
