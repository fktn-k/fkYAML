/**
 * Exception.hpp - implementation of custom exception classes.
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_EXCEPTION_HPP_
#define FK_YAML_EXCEPTION_HPP_

#include <stdexcept>
#include <string>

namespace fkyaml
{

class Exception : public std::exception
{
public:
    Exception()
        : m_error_msg("")
    {
    }

    Exception(const char* msg)
        : m_error_msg(msg)
    {
    }

public:
    const char* what() const noexcept override
    {
        return m_error_msg.c_str();
    }

private:
    std::string m_error_msg;
};

} // namespace fkyaml

#endif /* FK_YAML_EXCEPTION_HPP_ */