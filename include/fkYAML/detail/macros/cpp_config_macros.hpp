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

// C++ language standard detection (__cplusplus is not yet defined for C++23)
// Skip detection if the definitions listed below already exist.
#if !defined(FK_YAML_HAS_CXX_20) && !defined(FK_YAML_HAS_CXX_17) && !defined(FK_YAML_HAS_CXX_14) &&                    \
    !defined(FK_YAML_CXX_11)
#if (defined(__cplusplus) && __cplusplus >= 202002L) || (defined(_MSVC_LANG) && MSVC_LANG >= 202002L)
#define FK_YAML_HAS_CXX_20
#define FK_YAML_HAS_CXX_17
#define FK_YAML_HAS_CXX_14
#elif (defined(__cplusplus) && __cplusplus >= 201703L) || (defined(_HAS_CXX17) && _HAS_CXX17 == 1)
#define FK_YAML_HAS_CXX_17
#define FK_YAML_HAS_CXX_14
#elif (defined(__cplusplus) && __cplusplus >= 201402L) || (defined(_HAS_CXX14) && _HAS_CXX14 == 1)
#define FK_YAML_HAS_CXX_14
#endif

// C++11 is the minimum required version of the fkYAML library.
#define FK_YAML_HAS_CXX_11
#endif

// switch usage of inline variables. Inline variables have been introduced since C++17.
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

// switch usage of char8_t. char8_t has been introduced since C++20
#if !defined(FK_YAML_HAS_CHAR8_T)
#if defined(FK_YAML_HAS_CXX_20)
#if defined(__cpp_char8_t) && __cpp_char8_t >= 201811L
#define FK_YAML_HAS_CHAR8_T
#endif
#endif
#endif

#endif /* FK_YAML_DETAIL_MACROS_CPP_CONFIG_MACROS_HPP_ */
