///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_MACROS_CPP_CONFIG_MACROS_HPP_
#define FK_YAML_DETAIL_MACROS_CPP_CONFIG_MACROS_HPP_

// This file is assumed to be included only by version_macros.hpp file.
// To avoid redundant inclusion, do not include version_macros.hpp file as the other files do.

// With the MSVC compilers, the value of __cplusplus is by default always "199611L"(C++98).
// To avoid that, the library instead references _MSVC_LANG which is always set a correct value.
// See https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/ for more details.
#if defined(_MSVC_LANG) && !defined(__clang__)
#define FK_YAML_CPLUSPLUS _MSVC_LANG
#else
#define FK_YAML_CPLUSPLUS __cplusplus
#endif

// C++ language standard detection (__cplusplus is not yet defined for C++23)
// Skip detection if the definitions listed below already exist.
#if !defined(FK_YAML_HAS_CXX_20) && !defined(FK_YAML_HAS_CXX_17) && !defined(FK_YAML_HAS_CXX_14) &&                    \
    !defined(FK_YAML_CXX_11)
#if FK_YAML_CPLUSPLUS >= 202002L
#define FK_YAML_HAS_CXX_20
#define FK_YAML_HAS_CXX_17
#define FK_YAML_HAS_CXX_14
#elif FK_YAML_CPLUSPLUS >= 201703L
#define FK_YAML_HAS_CXX_17
#define FK_YAML_HAS_CXX_14
#elif FK_YAML_CPLUSPLUS >= 201402L
#define FK_YAML_HAS_CXX_14
#endif

// C++11 is the minimum required version of the fkYAML library.
#define FK_YAML_HAS_CXX_11
#endif

// switch usage of the deprecated attribute. [[deprecated]] is available since C++14.
#if defined(FK_YAML_HAS_CXX_14)
#define FK_YAML_DEPRECATED(msg) [[deprecated(msg)]]
#else
#if defined(_MSC_VER)
#define FK_YAML_DEPRECATED(msg) __declspec(deprecated(msg))
#elif defined(__GNUC__) || defined(__clang__)
#define FK_YAML_DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
#define FK_YAML_DEPRECATED(msg)
#endif
#endif

// switch usage of inline variables which have been available since C++17.
#if defined(FK_YAML_HAS_CXX_17)
#define FK_YAML_INLINE_VAR inline
#else
#define FK_YAML_INLINE_VAR
#endif

#ifdef __has_include
#if __has_include(<version>)
// <version> is available since C++20
#include <version>
#endif
#endif

// switch usage of char8_t which has been available since C++20.
#if !defined(FK_YAML_HAS_CHAR8_T)
#if defined(FK_YAML_HAS_CXX_20)
#if defined(__cpp_char8_t) && __cpp_char8_t >= 201811L
#define FK_YAML_HAS_CHAR8_T
#endif
#endif
#endif

// switch usages of the std::to_chars()/std::from_chars() functions which have been available since C++17.
#if defined(FK_YAML_HAS_CXX_17)
#if defined(__cpp_lib_to_chars) && __cpp_lib_to_chars >= 201611L
#define FK_YAML_HAS_TO_CHARS
#endif
#endif

#endif /* FK_YAML_DETAIL_MACROS_CPP_CONFIG_MACROS_HPP_ */
