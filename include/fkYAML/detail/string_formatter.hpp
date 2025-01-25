//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_STRING_FORMATTER_HPP
#define FK_YAML_DETAIL_STRING_FORMATTER_HPP

#include <cstdarg>
#include <cstdio>
#include <memory>
#include <string>

#include <fkYAML/detail/macros/define_macros.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

// NOLINTNEXTLINE(cert-dcl50-cpp)
inline std::string format(const char* fmt, ...) {
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
    va_list vl;
    va_start(vl, fmt);
    int size = std::vsnprintf(nullptr, 0, fmt, vl);
    va_end(vl);

    // LCOV_EXCL_START
    if (size < 0) {
        return "";
    }
    // LCOV_EXCL_STOP

    const std::unique_ptr<char[]> buffer {new char[size + 1] {}};

    va_start(vl, fmt);
    size = std::vsnprintf(buffer.get(), size + 1, fmt, vl);
    va_end(vl);
    // NOLINTEND(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)

    return {buffer.get(), static_cast<std::size_t>(size)};
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_STRING_FORMATTER_HPP */
