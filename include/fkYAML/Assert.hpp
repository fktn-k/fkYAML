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
