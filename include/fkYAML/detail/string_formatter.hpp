#ifndef FK_YAML_DETAIL_STRING_FORMATTER_HPP_
#define FK_YAML_DETAIL_STRING_FORMATTER_HPP_

#include <cstdarg>
#include <cstdio>
#include <memory>
#include <string>

#include <fkYAML/detail/macros/version_macros.hpp>

/// @namespace namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @namespace namespace for internal implementation of fkYAML library.
namespace detail
{

inline std::string format(const char* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    int size = std::vsnprintf(nullptr, 0, fmt, vl);
    va_end(vl);

    // LCOV_EXCL_START
    if (size < 0)
    {
        return "";
    }
    // LCOV_EXCL_STOP

    std::unique_ptr<char[]> buffer {new char[size + 1] {}};

    va_start(vl, fmt);
    size = std::vsnprintf(buffer.get(), size + 1, fmt, vl);
    va_end(vl);

    return std::string(buffer.get(), size);
}

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_STRING_FORMATTER_HPP_ */
