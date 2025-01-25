//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_MACROS_CPP_CONFIG_MACROS_HPP
#define FK_YAML_DETAIL_MACROS_CPP_CONFIG_MACROS_HPP

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

// C++ language standard detection
// Skip detection if the definitions listed below already exist.
#if !defined(FK_YAML_HAS_CXX_23) && !defined(FK_YAML_HAS_CXX_20) && !defined(FK_YAML_HAS_CXX_17) &&                    \
    !defined(FK_YAML_HAS_CXX_14) && !defined(FK_YAML_CXX_11)
#if FK_YAML_CPLUSPLUS >= 202302L
#define FK_YAML_HAS_CXX_23
#define FK_YAML_HAS_CXX_20
#define FK_YAML_HAS_CXX_17
#define FK_YAML_HAS_CXX_14
#elif FK_YAML_CPLUSPLUS >= 202002L
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

// switch usage of constexpr keyward depending on active C++ standard.
#if defined(FK_YAML_HAS_CXX_17)
#define FK_YAML_CXX17_CONSTEXPR constexpr
#else
#define FK_YAML_CXX17_CONSTEXPR
#endif

// Detect __has_* macros.
// The following macros replace redundant `defined(__has_*) && __has_*(...)`.

#ifdef __has_include
#define FK_YAML_HAS_INCLUDE(header) __has_include(header)
#else
#define FK_YAML_HAS_INCLUDE(header) (0)
#endif

#ifdef __has_builtin
#define FK_YAML_HAS_BUILTIN(builtin) __has_builtin(builtin)
#else
#define FK_YAML_HAS_BUILTIN(builtin) (0)
#endif

#ifdef __has_cpp_attribute
#define FK_YAML_HAS_CPP_ATTRIBUTE(attr) __has_cpp_attribute(attr)
#else
#define FK_YAML_HAS_CPP_ATTRIBUTE(attr) (0)
#endif

#ifdef __has_feature
#define FK_YAML_HAS_FEATURE(feat) __has_feature(feat)
#else
#define FK_YAML_HAS_FEATURE(feat) (0)
#endif

// switch usage of the no_sanitize attribute only when Clang sanitizer is active.
#if defined(__clang__) && FK_YAML_HAS_FEATURE(address_sanitizer)
#define FK_YAML_NO_SANITIZE(...) __attribute__((no_sanitize(__VA_ARGS__)))
#else
#define FK_YAML_NO_SANITIZE(...)
#endif

#if FK_YAML_HAS_INCLUDE(<version>)
// <version> is available since C++20
#include <version>
#endif

//
// C++ feature detections
//

// switch usages of the std::to_chars()/std::from_chars() functions which have been available since C++17.
#if defined(FK_YAML_HAS_CXX_17) && defined(__cpp_lib_to_chars) && __cpp_lib_to_chars >= 201611L
#define FK_YAML_HAS_TO_CHARS (1)
#else
#define FK_YAML_HAS_TO_CHARS (0)
#endif

// switch usage of char8_t which has been available since C++20.
#if defined(FK_YAML_HAS_CXX_20) && defined(__cpp_char8_t) && __cpp_char8_t >= 201811L
#define FK_YAML_HAS_CHAR8_T (1)
#else
#define FK_YAML_HAS_CHAR8_T (0)
#endif

//
// utility macros
//

// switch usage of [[likely]] C++ attribute which has been available since C++20.
#if defined(FK_YAML_HAS_CXX_20) && FK_YAML_HAS_CPP_ATTRIBUTE(likely) >= 201803L
#define FK_YAML_LIKELY(expr) (!!(expr)) [[likely]]
#elif FK_YAML_HAS_BUILTIN(__builtin_expect)
#define FK_YAML_LIKELY(expr) (__builtin_expect(!!(expr), 1))
#else
#define FK_YAML_LIKELY(expr) (!!(expr))
#endif

// switch usage of [[unlikely]] C++ attribute which has been available since C++20.
#if defined(FK_YAML_HAS_CXX_20) && FK_YAML_HAS_CPP_ATTRIBUTE(unlikely) >= 201803L
#define FK_YAML_UNLIKELY(expr) (!!(expr)) [[unlikely]]
#elif FK_YAML_HAS_BUILTIN(__builtin_expect)
#define FK_YAML_UNLIKELY(expr) (__builtin_expect(!!(expr), 0))
#else
#define FK_YAML_UNLIKELY(expr) (!!(expr))
#endif

#endif /* FK_YAML_DETAIL_MACROS_CPP_CONFIG_MACROS_HPP */
