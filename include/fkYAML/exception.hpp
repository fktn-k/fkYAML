///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.4
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_EXCEPTION_HPP_
#define FK_YAML_EXCEPTION_HPP_

#include <array>
#include <initializer_list>
#include <stdexcept>
#include <string>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/string_formatter.hpp>
#include <fkYAML/detail/types/node_t.hpp>

FK_YAML_NAMESPACE_BEGIN

/// @brief A base exception class used in fkYAML library.
/// @sa https://fktn-k.github.io/fkYAML/api/exception/
class exception : public std::exception {
public:
    /// @brief Construct a new exception object without any error messages.
    /// @sa https://fktn-k.github.io/fkYAML/api/exception/constructor/
    exception() = default;

    /// @brief Construct a new exception object with an error message.
    /// @param[in] msg An error message.
    /// @sa https://fktn-k.github.io/fkYAML/api/exception/constructor/
    explicit exception(const char* msg) noexcept {
        if (msg) {
            m_error_msg = msg;
        }
    }

public:
    /// @brief Returns an error message internally held. If nothing, a non-null, empty string will be returned.
    /// @return An error message internally held. The message might be empty.
    /// @sa https://fktn-k.github.io/fkYAML/api/exception/what/
    const char* what() const noexcept override {
        return m_error_msg.c_str();
    }

private:
    /// An error message holder.
    std::string m_error_msg {};
};

/// @brief An exception class indicating an encoding error.
/// @sa https://fktn-k.github.io/fkYAML/api/exception/invalid_encoding/
class invalid_encoding : public exception {
public:
    /// @brief Construct a new invalid_encoding object for UTF-8 related errors.
    /// @param msg An error message.
    /// @param u8 The UTF-8 character bytes.
    explicit invalid_encoding(const char* msg, const std::initializer_list<uint8_t>& u8) noexcept
        : exception(generate_error_message(msg, u8).c_str()) {
    }

    /// @brief Construct a new invalid_encoding object for UTF-16 related errors.
    /// @param msg An error message.
    /// @param u16_h The first UTF-16 encoded element used for the UTF-8 encoding.
    /// @param u16_l The second UTF-16 encoded element used for the UTF-8 encoding.
    explicit invalid_encoding(const char* msg, std::array<char16_t, 2> u16) noexcept
        : exception(generate_error_message(msg, u16).c_str()) {
    }

    /// @brief Construct a new invalid_encoding object for UTF-32 related errors.
    /// @param msg An error message.
    /// @param u32 The UTF-32 encoded element used for the UTF-8 encoding.
    explicit invalid_encoding(const char* msg, char32_t u32) noexcept
        : exception(generate_error_message(msg, u32).c_str()) {
    }

private:
    std::string generate_error_message(const char* msg, const std::initializer_list<uint8_t>& u8) const noexcept {
        auto itr = u8.begin();
        auto end_itr = u8.end();
        std::string formatted = detail::format("invalid_encoding: %s in=[ 0x%02x", msg, *itr++);
        while (itr != end_itr) {
            formatted += detail::format(", 0x%02x", *itr++);
        }
        formatted += " ]";
        return formatted;
    }

    /// @brief Generate an error message from the given parameters for the UTF-16 encoding.
    /// @param msg An error message.
    /// @param h The first UTF-16 encoded element used for the UTF-8 encoding.
    /// @param l The second UTF-16 encoded element used for the UTF-8 encoding.
    /// @return A generated error message.
    std::string generate_error_message(const char* msg, std::array<char16_t, 2> u16) const noexcept {
        // uint16_t is large enough for UTF-16 encoded elements.
        return detail::format("invalid_encoding: %s in=[ 0x%04x, 0x%04x ]", msg, uint16_t(u16[0]), uint16_t(u16[1]));
    }

    /// @brief Generate an error message from the given parameters for the UTF-32 encoding.
    /// @param msg An error message.
    /// @param u32 The UTF-32 encoded element used for the UTF-8 encoding.
    /// @return A genereated error message.
    std::string generate_error_message(const char* msg, char32_t u32) const noexcept {
        // uint32_t is large enough for UTF-32 encoded elements.
        return detail::format("invalid_encoding: %s in=0x%08x", msg, uint32_t(u32));
    }
};

/// @brief An exception class indicating an error in parsing.
class parse_error : public exception {
public:
    explicit parse_error(const char* msg, std::size_t lines, std::size_t cols_in_line) noexcept
        : exception(generate_error_message(msg, lines, cols_in_line).c_str()) {
    }

private:
    std::string generate_error_message(const char* msg, std::size_t lines, std::size_t cols_in_line) const noexcept {
        return detail::format("parse_error: %s (at line %zu, column %zu)", msg, lines, cols_in_line);
    }
};

/// @brief An exception class indicating an invalid type conversion.
/// @sa https://fktn-k.github.io/fkYAML/api/exception/type_error/
class type_error : public exception {
public:
    /// @brief Construct a new type_error object with an error message and a node type.
    /// @param[in] msg An error message.
    /// @param[in] type The type of a source node value.
    explicit type_error(const char* msg, detail::node_t type) noexcept
        : exception(generate_error_message(msg, type).c_str()) {
    }

private:
    /// @brief Generate an error message from given parameters.
    /// @param msg An error message.
    /// @param type The type of a source node value.
    /// @return A generated error message.
    std::string generate_error_message(const char* msg, detail::node_t type) const noexcept {
        return detail::format("type_error: %s type=%s", msg, detail::to_string(type));
    }
};

class out_of_range : public exception {
public:
    explicit out_of_range(int index) noexcept
        : exception(generate_error_message(index).c_str()) {
    }

    explicit out_of_range(const char* key) noexcept
        : exception(generate_error_message(key).c_str()) {
    }

private:
    std::string generate_error_message(int index) {
        return detail::format("out_of_range: index %d is out of range", index);
    }

    std::string generate_error_message(const char* key) {
        return detail::format("out_of_range: key \'%s\' is not found.", key);
    }
};

class invalid_tag : public exception {
public:
    explicit invalid_tag(const char* msg, const char* tag)
        : exception(generate_error_message(msg, tag).c_str()) {
    }

private:
    std::string generate_error_message(const char* msg, const char* tag) {
        return detail::format("invalid_tag: %s tag=%s", msg, tag);
    }
};

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_EXCEPTION_HPP_ */
