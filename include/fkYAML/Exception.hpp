/**
 * @file Exception.hpp
 * @brief Implementation of custom exception classes.
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_EXCEPTION_HPP_
#define FK_YAML_EXCEPTION_HPP_

#include <stdexcept>
#include <string>

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
namespace fkyaml
{

/**
 * @class Exception
 * @brief A base exception class used in fkYAML library.
 */
class Exception : public std::exception
{
public:
    /**
     * @brief Construct a new Exception object without any error messages.
     */
    Exception() = default;

    /**
     * @brief Construct a new Exception object with an error message.
     *
     * @param msg An error description message.
     */
    explicit Exception(const char* msg)
        : m_error_msg(msg)
    {
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
    std::string m_error_msg;
};

} // namespace fkyaml

#endif /* FK_YAML_EXCEPTION_HPP_ */