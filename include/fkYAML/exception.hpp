///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.2.1
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_EXCEPTION_HPP_
#define FK_YAML_EXCEPTION_HPP_

#include <stdexcept>
#include <string>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/types/node_t.hpp>

/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief A base exception class used in fkYAML library.
/// @sa https://fktn-k.github.io/fkYAML/api/exception/
class exception : public std::exception
{
public:
    /// @brief Construct a new exception object without any error messages.
    /// @sa https://fktn-k.github.io/fkYAML/api/exception/constructor/
    exception() = default;

    /// @brief Construct a new exception object with an error message.
    /// @param[in] msg An error message.
    /// @sa https://fktn-k.github.io/fkYAML/api/exception/constructor/
    explicit exception(const char* msg)
    {
        if (msg)
        {
            m_error_msg = msg;
        }
    }

public:
    /// @brief Returns an error message internally held. If nothing, a non-null, empty string will be returned.
    /// @return An error message internally held. The message might be empty.
    /// @sa https://fktn-k.github.io/fkYAML/api/exception/what/
    const char* what() const noexcept override
    {
        return m_error_msg.c_str();
    }

private:
    /// An error message holder.
    std::string m_error_msg {};
};

/// @brief A exception class indicating an invalid type conversion.
/// @sa https://fktn-k.github.io/fkYAML/api/exception/
class type_error : public exception
{
public:
    /// @brief Construct a new type_error object with an error message and a node type.
    /// @param[in] msg An error message.
    /// @param[in] type The type of a source node value.
    explicit type_error(const char* msg, detail::node_t type)
        : exception(std::string(std::string("type_error: ") + std::string(msg) + " type=" + detail::to_string(type)).c_str())
    {
    }
};

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_EXCEPTION_HPP_ */
