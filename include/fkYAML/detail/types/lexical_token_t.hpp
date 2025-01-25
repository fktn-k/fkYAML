//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_TYPES_LEXICAL_TOKEN_T_HPP
#define FK_YAML_DETAIL_TYPES_LEXICAL_TOKEN_T_HPP

#include <cstdint>

#include <fkYAML/detail/macros/define_macros.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Definition of lexical token types.
enum class lexical_token_t : std::uint8_t {
    END_OF_BUFFER,         //!< the end of input buffer.
    EXPLICIT_KEY_PREFIX,   //!< the character for explicit mapping key prefix `?`.
    KEY_SEPARATOR,         //!< the key separator `:`
    VALUE_SEPARATOR,       //!< the value separator `,`
    ANCHOR_PREFIX,         //!< the character for anchor prefix `&`
    ALIAS_PREFIX,          //!< the character for alias prefix `*`
    YAML_VER_DIRECTIVE,    //!< a YAML version directive found. use get_yaml_version() to get a value.
    TAG_DIRECTIVE,         //!< a TAG directive found. use GetTagInfo() to get the tag information.
    TAG_PREFIX,            //!< the character for tag prefix `!`
    INVALID_DIRECTIVE,     //!< an invalid directive found. do not try to get the value.
    SEQUENCE_BLOCK_PREFIX, //!< the character for sequence block prefix `- `
    SEQUENCE_FLOW_BEGIN,   //!< the character for sequence flow begin `[`
    SEQUENCE_FLOW_END,     //!< the character for sequence flow end `]`
    MAPPING_FLOW_BEGIN,    //!< the character for mapping begin `{`
    MAPPING_FLOW_END,      //!< the character for mapping end `}`
    PLAIN_SCALAR,          //!< plain (unquoted) scalars
    SINGLE_QUOTED_SCALAR,  //!< single-quoted scalars
    DOUBLE_QUOTED_SCALAR,  //!< double-quoted scalars
    BLOCK_LITERAL_SCALAR,  //!< block literal style scalars
    BLOCK_FOLDED_SCALAR,   //!< block folded style scalars
    END_OF_DIRECTIVES,     //!< the end of declaration of directives specified by `---`.
    END_OF_DOCUMENT,       //!< the end of a YAML document specified by `...`.
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_TYPES_LEXICAL_TOKEN_T_HPP */
