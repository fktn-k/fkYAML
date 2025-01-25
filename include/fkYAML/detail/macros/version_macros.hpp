//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

// Check version definitions if already defined.
#if defined(FK_YAML_MAJOR_VERSION) && defined(FK_YAML_MINOR_VERSION) && defined(FK_YAML_PATCH_VERSION)
#if FK_YAML_MAJOR_VERSION != 0 || FK_YAML_MINOR_VERSION != 4 || FK_YAML_PATCH_VERSION != 2
#warning Already included a different version of the fkYAML library!
#else
// define macros to skip defining macros down below.
#define FK_YAML_VERCHECK_SUCCEEDED
#endif
#endif

#ifndef FK_YAML_VERCHECK_SUCCEEDED

#define FK_YAML_MAJOR_VERSION 0
#define FK_YAML_MINOR_VERSION 4
#define FK_YAML_PATCH_VERSION 2

#define FK_YAML_NAMESPACE_VERSION_CONCAT_IMPL(major, minor, patch) v##major##_##minor##_##patch

#define FK_YAML_NAMESPACE_VERSION_CONCAT(major, minor, patch) FK_YAML_NAMESPACE_VERSION_CONCAT_IMPL(major, minor, patch)

#define FK_YAML_NAMESPACE_VERSION                                                                                      \
    FK_YAML_NAMESPACE_VERSION_CONCAT(FK_YAML_MAJOR_VERSION, FK_YAML_MINOR_VERSION, FK_YAML_PATCH_VERSION)

#define FK_YAML_NAMESPACE_BEGIN                                                                                        \
    namespace fkyaml {                                                                                                 \
    inline namespace FK_YAML_NAMESPACE_VERSION {

#define FK_YAML_NAMESPACE_END                                                                                          \
    } /* inline namespace FK_YAML_NAMESPACE_VERSION */                                                                 \
    } // namespace fkyaml

#define FK_YAML_DETAIL_NAMESPACE_BEGIN                                                                                 \
    FK_YAML_NAMESPACE_BEGIN                                                                                            \
    namespace detail {

#define FK_YAML_DETAIL_NAMESPACE_END                                                                                   \
    } /* namespace detail */                                                                                           \
    FK_YAML_NAMESPACE_END

#endif // !defined(FK_YAML_VERCHECK_SUCCEEDED)
