//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.12
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_INPUT_BLOCK_SCALAR_HEADER_HPP
#define FK_YAML_DETAIL_INPUT_BLOCK_SCALAR_HEADER_HPP

#include <cstdint>

#include <fkYAML/detail/macros/version_macros.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

enum class chomping_indicator_t {
    STRIP, //!< excludes final line breaks and trailing empty lines indicated by `-`.
    CLIP,  //!< preserves final line breaks but excludes trailing empty lines. no indicator means this type.
    KEEP,  //!< preserves final line breaks and trailing empty lines indicated by `+`.
};

struct block_scalar_header {
    chomping_indicator_t chomp {chomping_indicator_t::CLIP};
    uint32_t indent {0};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_BLOCK_SCALAR_HEADER_HPP */
