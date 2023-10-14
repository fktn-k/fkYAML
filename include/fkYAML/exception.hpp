/**
 *  _______   __ __   __  _____   __  __  __
 * |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
 * |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.1
 * |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
 *
 * SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
 * SPDX-License-Identifier: MIT
 *
 * @file
 */

#ifndef FK_YAML_EXCEPTION_HPP_
#define FK_YAML_EXCEPTION_HPP_

#include <stdexcept>
#include <string>

#include <fkYAML/detail/macros/version_macros.hpp>

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

/**
 * @class exception
 * @brief A base exception class used in fkYAML library.
 */
class exception : public std::exception
{
public:
    /**
     * @brief Construct a new exception object without any error messages.
     */
    exception() = default;

    /**
     * @brief Construct a new exception object with an error message.
     *
     * @param msg An error description message.
     */
    explicit exception(const char* msg)
    {
        if (msg)
        {
            m_error_msg = msg;
        }
    }

public:
    /**
     * @brief Returns an error message internally held. If nothing, a non-null, empty string will be returned.
     *
     * @return const char* A pointer to error messages
     */
    const char* what() const noexcept override
    {
        return m_error_msg.c_str();
    }

private:
    /** An error message holder. */
    std::string m_error_msg {};
};

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_EXCEPTION_HPP_ */
