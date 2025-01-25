//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_ASSERT_HPP
#define FK_YAML_DETAIL_ASSERT_HPP

// if FK_YAML_ASSERT is not user-defined. apply the default assert impl.
#ifndef FK_YAML_ASSERT
#ifndef NDEBUG
#include <cassert>
#define FK_YAML_ASSERT(x) assert(x)
#else
#define FK_YAML_ASSERT(x)
#endif
#endif

#endif /* FK_YAML_DETAIL_ASSERT_HPP */
