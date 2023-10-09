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

#if defined(FK_YAML_MAJOR_VERSION) && defined(FK_YAML_MINOR_VERSION) && defined(FK_YAML_PATCH_VERSION)
    #if FK_YAML_MAJOR_VERSION != 0 || FK_YAML_MINOR_VERSION != 0 || FK_YAML_PATCH_VERSION != 1
        #warning "Already included a different version of fkYAML library!"
    #endif
#endif

#define FK_YAML_MAJOR_VERSION 0
#define FK_YAML_MINOR_VERSION 0
#define FK_YAML_PATCH_VERSION 1

#define FK_YAML_NAMESPACE_VERSION_CONCAT_EX(major, minor, patch) v##major##_##minor##_##patch

#define FK_YAML_NAMESPACE_VERSION_CONCAT(major, minor, patch) FK_YAML_NAMESPACE_VERSION_CONCAT_EX(major, minor, patch)

#define FK_YAML_NAMESPACE_VERSION                                                                                      \
    FK_YAML_NAMESPACE_VERSION_CONCAT(FK_YAML_MAJOR_VERSION, FK_YAML_MINOR_VERSION, FK_YAML_PATCH_VERSION)

#define FK_YAML_NAMESPACE_BEGIN                                                                                        \
    namespace fkyaml                                                                                                   \
    {                                                                                                                  \
    inline namespace FK_YAML_NAMESPACE_VERSION                                                                         \
    {

#define FK_YAML_NAMESPACE_END                                                                                          \
    } /* namespace (inline namespace) */                                                                               \
    } // namespace fkyaml

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

// switch usage of inline variables. Inline variables are available since C++17.
#if defined(FK_YAML_HAS_CXX_17)
    #define FK_YAML_INLINE_VAR inline
#else
    #define FK_YAML_INLINE_VAR
#endif
