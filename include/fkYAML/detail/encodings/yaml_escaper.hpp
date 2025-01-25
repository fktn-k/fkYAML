//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_ENCODINGS_YAML_ESCAPER_HPP
#define FK_YAML_DETAIL_ENCODINGS_YAML_ESCAPER_HPP

#include <string>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/assert.hpp>
#include <fkYAML/detail/encodings/utf_encodings.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

class yaml_escaper {
    using iterator = ::std::string::const_iterator;

public:
    static bool unescape(const char*& begin, const char* end, std::string& buff) {
        FK_YAML_ASSERT(*begin == '\\' && std::distance(begin, end) > 0);
        bool ret = true;

        switch (*++begin) {
        case 'a':
            buff.push_back('\a');
            break;
        case 'b':
            buff.push_back('\b');
            break;
        case 't':
        case '\t':
            buff.push_back('\t');
            break;
        case 'n':
            buff.push_back('\n');
            break;
        case 'v':
            buff.push_back('\v');
            break;
        case 'f':
            buff.push_back('\f');
            break;
        case 'r':
            buff.push_back('\r');
            break;
        case 'e':
            buff.push_back(static_cast<char>(0x1B));
            break;
        case ' ':
            buff.push_back(' ');
            break;
        case '\"':
            buff.push_back('\"');
            break;
        case '/':
            buff.push_back('/');
            break;
        case '\\':
            buff.push_back('\\');
            break;
        case 'N': // next line
            unescape_escaped_unicode(0x85u, buff);
            break;
        case '_': // non-breaking space
            unescape_escaped_unicode(0xA0u, buff);
            break;
        case 'L': // line separator
            unescape_escaped_unicode(0x2028u, buff);
            break;
        case 'P': // paragraph separator
            unescape_escaped_unicode(0x2029u, buff);
            break;
        case 'x': {
            char32_t codepoint {0};
            ret = extract_codepoint(begin, end, 1, codepoint);
            if FK_YAML_LIKELY (ret) {
                unescape_escaped_unicode(codepoint, buff);
            }
            break;
        }
        case 'u': {
            char32_t codepoint {0};
            ret = extract_codepoint(begin, end, 2, codepoint);
            if FK_YAML_LIKELY (ret) {
                unescape_escaped_unicode(codepoint, buff);
            }
            break;
        }
        case 'U': {
            char32_t codepoint {0};
            ret = extract_codepoint(begin, end, 4, codepoint);
            if FK_YAML_LIKELY (ret) {
                unescape_escaped_unicode(codepoint, buff);
            }
            break;
        }
        default:
            // Unsupported escape sequence is found in a string token.
            ret = false;
            break;
        }

        return ret;
    }

    static ::std::string escape(const char* begin, const char* end, bool& is_escaped) {
        ::std::string escaped {};
        escaped.reserve(std::distance(begin, end));
        for (; begin != end; ++begin) {
            switch (*begin) {
            case 0x01:
                escaped += "\\u0001";
                is_escaped = true;
                break;
            case 0x02:
                escaped += "\\u0002";
                is_escaped = true;
                break;
            case 0x03:
                escaped += "\\u0003";
                is_escaped = true;
                break;
            case 0x04:
                escaped += "\\u0004";
                is_escaped = true;
                break;
            case 0x05:
                escaped += "\\u0005";
                is_escaped = true;
                break;
            case 0x06:
                escaped += "\\u0006";
                is_escaped = true;
                break;
            case '\a':
                escaped += "\\a";
                is_escaped = true;
                break;
            case '\b':
                escaped += "\\b";
                is_escaped = true;
                break;
            case '\t':
                escaped += "\\t";
                is_escaped = true;
                break;
            case '\n':
                escaped += "\\n";
                is_escaped = true;
                break;
            case '\v':
                escaped += "\\v";
                is_escaped = true;
                break;
            case '\f':
                escaped += "\\f";
                is_escaped = true;
                break;
            case '\r':
                escaped += "\\r";
                is_escaped = true;
                break;
            case 0x0E:
                escaped += "\\u000E";
                is_escaped = true;
                break;
            case 0x0F:
                escaped += "\\u000F";
                is_escaped = true;
                break;
            case 0x10:
                escaped += "\\u0010";
                is_escaped = true;
                break;
            case 0x11:
                escaped += "\\u0011";
                is_escaped = true;
                break;
            case 0x12:
                escaped += "\\u0012";
                is_escaped = true;
                break;
            case 0x13:
                escaped += "\\u0013";
                is_escaped = true;
                break;
            case 0x14:
                escaped += "\\u0014";
                is_escaped = true;
                break;
            case 0x15:
                escaped += "\\u0015";
                is_escaped = true;
                break;
            case 0x16:
                escaped += "\\u0016";
                is_escaped = true;
                break;
            case 0x17:
                escaped += "\\u0017";
                is_escaped = true;
                break;
            case 0x18:
                escaped += "\\u0018";
                is_escaped = true;
                break;
            case 0x19:
                escaped += "\\u0019";
                is_escaped = true;
                break;
            case 0x1A:
                escaped += "\\u001A";
                is_escaped = true;
                break;
            case 0x1B:
                escaped += "\\e";
                is_escaped = true;
                break;
            case 0x1C:
                escaped += "\\u001C";
                is_escaped = true;
                break;
            case 0x1D:
                escaped += "\\u001D";
                is_escaped = true;
                break;
            case 0x1E:
                escaped += "\\u001E";
                is_escaped = true;
                break;
            case 0x1F:
                escaped += "\\u001F";
                is_escaped = true;
                break;
            case '\"':
                escaped += "\\\"";
                is_escaped = true;
                break;
            case '\\':
                escaped += "\\\\";
                is_escaped = true;
                break;
            default:
                const std::ptrdiff_t diff = static_cast<int>(std::distance(begin, end));
                if (diff > 1) {
                    if (*begin == static_cast<char>(0xC2u) && *(begin + 1) == static_cast<char>(0x85u)) {
                        escaped += "\\N";
                        std::advance(begin, 1);
                        is_escaped = true;
                        break;
                    }
                    if (*begin == static_cast<char>(0xC2u) && *(begin + 1) == static_cast<char>(0xA0u)) {
                        escaped += "\\_";
                        std::advance(begin, 1);
                        is_escaped = true;
                        break;
                    }

                    if (diff > 2) {
                        if (*begin == static_cast<char>(0xE2u) && *(begin + 1) == static_cast<char>(0x80u) &&
                            *(begin + 2) == static_cast<char>(0xA8u)) {
                            escaped += "\\L";
                            std::advance(begin, 2);
                            is_escaped = true;
                            break;
                        }
                        if (*begin == static_cast<char>(0xE2u) && *(begin + 1) == static_cast<char>(0x80u) &&
                            *(begin + 2) == static_cast<char>(0xA9u)) {
                            escaped += "\\P";
                            std::advance(begin, 2);
                            is_escaped = true;
                            break;
                        }
                    }
                }
                escaped += *begin;
                break;
            }
        }
        return escaped;
    } // LCOV_EXCL_LINE

private:
    static bool convert_hexchar_to_byte(char source, uint8_t& byte) {
        if ('0' <= source && source <= '9') {
            // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
            byte = static_cast<uint8_t>(source - '0');
            return true;
        }

        if ('A' <= source && source <= 'F') {
            // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
            byte = static_cast<uint8_t>(source - 'A' + 10);
            return true;
        }

        if ('a' <= source && source <= 'f') {
            // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
            byte = static_cast<uint8_t>(source - 'a' + 10);
            return true;
        }

        // The given character is not hexadecimal.
        return false;
    }

    static bool extract_codepoint(const char*& begin, const char* end, int bytes_to_read, char32_t& codepoint) {
        const bool has_enough_room = static_cast<int>(std::distance(begin, end)) >= (bytes_to_read - 1);
        if (!has_enough_room) {
            return false;
        }

        const int read_size = bytes_to_read * 2;
        uint8_t byte {0};
        codepoint = 0;

        for (int i = read_size - 1; i >= 0; i--) {
            const bool is_valid = convert_hexchar_to_byte(*++begin, byte);
            if (!is_valid) {
                return false;
            }
            // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
            codepoint |= static_cast<char32_t>(byte << (4 * i));
        }

        return true;
    }

    static void unescape_escaped_unicode(char32_t codepoint, std::string& buff) {
        // the inner curly braces are necessary to build with older compilers.
        std::array<uint8_t, 4> encode_buff {{}};
        uint32_t encoded_size {0};
        utf8::from_utf32(codepoint, encode_buff, encoded_size);
        buff.append(reinterpret_cast<char*>(encode_buff.data()), encoded_size);
    }
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ENCODINGS_YAML_ESCAPER_HPP */
