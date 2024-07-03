///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.9
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_TYPES_LEXICAL_TOKEN_T_HPP_
#define FK_YAML_DETAIL_TYPES_LEXICAL_TOKEN_T_HPP_

#include <fkYAML/detail/macros/version_macros.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Definition of lexical token types.
enum class lexical_token_t {
    END_OF_BUFFER,              //!< the end of an input buffer.
    END_OF_DIRECTIVES,          //!< the end of directives, specified by `---`.
    END_OF_DOCUMENT,            //!< the end of a YAML document, specified by `...`.
    BLOCK_SEQUENCE_ITEM_PREFIX, //!< the beginning of a block sequence item, specified by `- `.
    FLOW_SEQUENCE_BEGIN,        //!< the beginning of a flow sequence, specified by `[`.
    FLOW_SEQUENCE_END,          //!< the end of a flow sequence, specified by `]`.
    FLOW_MAPPING_BEGIN,         //!< the beginning of a flow mapping, specified by `{`.
    FLOW_MAPPING_END,           //!< the end of a flow mapping, specified by `}`.
    FLOW_SINGLE_QUOTED_SCALAR,  //!< a flow single quoted scalar.
    FLOW_DOUBLE_QUOTED_SCALAR,  //!< a flow double quoted scalar.
    FLOW_PLAIN_SCALAR,          //!< a flow plain scalar.
    BLOCK_SCALAR,               //!< a block scalar (either literal or folded).
    EXPLICIT_KEY_PREFIX,        //!< the prefix of an explicit mapping key, specified by `?`.
    KEY_SEPARATOR,              //!< the key separater, specified by `: `.
    VALUE_SEPARATOR,            //!< the value separater, specified by `,`.
    ANCHOR,                     //!< an anchor, beginning with the prefix `&`.
    ALIAS,                      //!< an alias, beginning with the prefix `*`.
    TAG,                        //!< a tag, beginning with the prefix `!`.
    DIRECTIVE,                  //!< a directive line, beginning with the prefix `%`.
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_TYPES_LEXICAL_TOKEN_T_HPP_ */
