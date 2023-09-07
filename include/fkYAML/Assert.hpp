/**
 * @file Assert.hpp
 * @brief Definition of assertion for fkYAML library implementation.
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_ASSERT_HPP_
#define FK_YAML_ASSERT_HPP_

// if FK_YAML_ASSERT is not user-defined. apply the default assert impl.
#ifndef FK_YAML_ASSERT
#ifndef NDEBUG
#include <cassert>
#define FK_YAML_ASSERT(x) assert(x) // NOLINT(cppcoreguidelines-macro-usage)
#else
#define FK_YAML_ASSERT(x) // NOLINT(cppcoreguidelines-macro-usage)
#endif
#endif

#endif /* FK_YAML_ASSERT_HPP_ */
