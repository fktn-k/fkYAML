//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.4
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_CHAR_CLASS_HPP
#define FK_YAML_DETAIL_CHAR_CLASS_HPP

#include <fkYAML/detail/macros/define_macros.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

// The functions below replace their <cctype> counterparts, which must not be used on the characters of
// an input buffer. Those take an int whose value has to be representable as an unsigned char or equal
// to EOF, so passing a char is undefined behavior for any byte of a multi-byte UTF-8 sequence on a
// platform where char is signed. They also depend on the current locale, while both YAML syntax and
// URI syntax allow exactly the ASCII ranges spelled out here.

/// @brief Check if the given character is a digit.
/// @param c A character to be checked.
/// @return true if the given character is a digit, false otherwise.
inline bool is_digit(char c) noexcept {
    return ('0' <= c && c <= '9');
}

/// @brief Check if the given character is a hex-digit.
/// @param c A character to be checked.
/// @return true if the given character is a hex-digit, false otherwise.
inline bool is_xdigit(char c) noexcept {
    return is_digit(c) || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f');
}

/// @brief Check if the given character is an alphabet or a digit.
/// @param c A character to be checked.
/// @return true if the given character is an alphabet or a digit, false otherwise.
inline bool is_alnum(char c) noexcept {
    return is_digit(c) || ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_CHAR_CLASS_HPP */
