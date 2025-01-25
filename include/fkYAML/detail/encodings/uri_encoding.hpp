//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_ENCODINGS_URI_ENCODING_HPP
#define FK_YAML_DETAIL_ENCODINGS_URI_ENCODING_HPP

#include <cctype>
#include <string>

#include <fkYAML/detail/macros/define_macros.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief A class which handles URI encodings.
class uri_encoding {
public:
    /// @brief Validates the encoding of the given character sequence.
    /// @param begin An iterator to the first element of the character sequence.
    /// @param end An iterator to the past-the-end element of the character sequence.
    /// @return true if all the characters are valid, false otherwise.
    static bool validate(const char* begin, const char* end) noexcept {
        if (begin == end) {
            return true;
        }

        const char* current = begin;

        for (; current != end; ++current) {
            if (*current == '%') {
                const bool are_valid_octets = validate_octets(++current, end);
                if (!are_valid_octets) {
                    return false;
                }

                continue;
            }

            const bool is_allowed_character = validate_character(*current);
            if (!is_allowed_character) {
                return false;
            }
        }

        return true;
    }

private:
    /// @brief Validates the given octets.
    /// @param begin An iterator to the first octet.
    /// @param end An iterator to the past-the-end element of the whole character sequence.
    /// @return true if the octets are valid, false otherwise.
    static bool validate_octets(const char*& begin, const char*& end) {
        for (int i = 0; i < 2; i++, ++begin) {
            if (begin == end) {
                return false;
            }

            // Normalize a character for a-f/A-F comparison
            const int octet = std::tolower(*begin);

            if ('0' <= octet && octet <= '9') {
                continue;
            }

            if ('a' <= octet && octet <= 'f') {
                continue;
            }

            return false;
        }

        return true;
    }

    /// @brief Verify if the given character is allowed as a URI character.
    /// @param c The target character.
    /// @return true if the given character is allowed as a URI character, false otherwise.
    static bool validate_character(const char c) {
        // Check if the current character is one of reserved/unreserved characters which are allowed for
        // use. See the following links for details:
        // * reserved characters:   https://datatracker.ietf.org/doc/html/rfc3986#section-2.2
        // * unreserved characters: https://datatracker.ietf.org/doc/html/rfc3986#section-2.3

        switch (c) {
        // reserved characters (gen-delims)
        case ':':
        case '/':
        case '?':
        case '#':
        case '[':
        case ']':
        case '@':
        // reserved characters (sub-delims)
        case '!':
        case '$':
        case '&':
        case '\'':
        case '(':
        case ')':
        case '*':
        case '+':
        case ',':
        case ';':
        case '=':
        // unreserved characters
        case '-':
        case '.':
        case '_':
        case '~':
            return true;
        default:
            // alphabets and numbers are also allowed.
            return static_cast<bool>(std::isalnum(c));
        }
    }
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ENCODINGS_URI_ENCODING_HPP */
