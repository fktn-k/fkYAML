///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_NODE_HPP_
#define FK_YAML_NODE_HPP_

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

// #include <fkYAML/detail/macros/version_macros.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

// Check version definitions if already defined.
#if defined(FK_YAML_MAJOR_VERSION) && defined(FK_YAML_MINOR_VERSION) && defined(FK_YAML_PATCH_VERSION)
    #if FK_YAML_MAJOR_VERSION != 0 || FK_YAML_MINOR_VERSION != 3 || FK_YAML_PATCH_VERSION != 2
        #warning Already included a different version of the fkYAML library!
    #else
        // define macros to skip defining macros down below.
        #define FK_YAML_VERCHECK_SUCCEEDED
    #endif
#endif

#ifndef FK_YAML_VERCHECK_SUCCEEDED

    #define FK_YAML_MAJOR_VERSION 0
    #define FK_YAML_MINOR_VERSION 3
    #define FK_YAML_PATCH_VERSION 2

    #define FK_YAML_NAMESPACE_VERSION_CONCAT_IMPL(major, minor, patch) v##major##_##minor##_##patch

    #define FK_YAML_NAMESPACE_VERSION_CONCAT(major, minor, patch)                                                      \
        FK_YAML_NAMESPACE_VERSION_CONCAT_IMPL(major, minor, patch)

    #define FK_YAML_NAMESPACE_VERSION                                                                                  \
        FK_YAML_NAMESPACE_VERSION_CONCAT(FK_YAML_MAJOR_VERSION, FK_YAML_MINOR_VERSION, FK_YAML_PATCH_VERSION)

    #define FK_YAML_NAMESPACE_BEGIN                                                                                    \
        namespace fkyaml                                                                                               \
        {                                                                                                              \
        inline namespace FK_YAML_NAMESPACE_VERSION                                                                     \
        {

    #define FK_YAML_NAMESPACE_END                                                                                      \
        } /* namespace (inline namespace) */                                                                           \
        } // namespace fkyaml

    // #include <fkYAML/detail/macros/cpp_config_macros.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
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


#endif // !defined(FK_YAML_VERCHECK_SUCCEEDED)

// #include <fkYAML/detail/assert.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_ASSERT_HPP_
#define FK_YAML_DETAIL_ASSERT_HPP_

// if FK_YAML_ASSERT is not user-defined. apply the default assert impl.
#ifndef FK_YAML_ASSERT
    #ifndef NDEBUG
        #include <cassert>
        #define FK_YAML_ASSERT(x) assert(x) // NOLINT(cppcoreguidelines-macro-usage)
    #else
        #define FK_YAML_ASSERT(x) // NOLINT(cppcoreguidelines-macro-usage)
    #endif
#endif

#endif /* FK_YAML_DETAIL_ASSERT_HPP_ */

// #include <fkYAML/detail/input/deserializer.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_INPUT_DESERIALIZER_HPP_
#define FK_YAML_DETAIL_INPUT_DESERIALIZER_HPP_

#include <algorithm>
#include <cstdint>
#include <unordered_map>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/input/lexical_analyzer.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_INPUT_LEXICAL_ANALIZER_HPP_
#define FK_YAML_DETAIL_INPUT_LEXICAL_ANALIZER_HPP_

#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <type_traits>
#include <vector>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/assert.hpp>

// #include <fkYAML/detail/conversions/from_string.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_CONVERSIONS_FROM_STRING_HPP_
#define FK_YAML_DETAIL_CONVERSIONS_FROM_STRING_HPP_

#include <cmath>
#include <cstdlib>
#include <limits>
#include <string>
#include <type_traits>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/meta/stl_supplement.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_META_STL_SUPPLEMENT_HPP_
#define FK_YAML_DETAIL_META_STL_SUPPLEMENT_HPP_

#include <cstddef>
#include <type_traits>

// #include <fkYAML/detail/macros/version_macros.hpp>


/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//   For contributors:
//     This file is for supplementing future C++ STL implementations to utilize some useful features
//     implemented in C++14 or later.
//     This file is needed to keep the fkYAML library requirement to C++11.
//     **DO NOT** implement features which are not included any version of STL in this file.
//     Such implementations must be in the type_traits.hpp file.
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FK_YAML_HAS_CXX_14

/// @brief An alias template for std::add_pointer::type with C++11.
/// @note std::add_pointer_t is available since C++14.
/// @sa https://en.cppreference.com/w/cpp/types/add_pointer
/// @tparam T A type to be added a pointer.
template <typename T>
using add_pointer_t = typename std::add_pointer<T>::type;

/// @brief An alias template for std::enable_if::type with C++11.
/// @note std::enable_if_t is available since C++14.
/// @sa https://en.cppreference.com/w/cpp/types/enable_if
/// @tparam Condition A condition tested at compile time.
/// @tparam T The type defined only if Condition is true.
template <bool Condition, typename T = void>
using enable_if_t = typename std::enable_if<Condition, T>::type;

/// @brief A simple implementation to use std::is_null_pointer with C++11.
/// @note std::is_null_pointer is available since C++14.
/// @sa https://en.cppreference.com/w/cpp/types/is_null_pointer
/// @tparam T The type to be checked if it's equal to std::nullptr_t.
template <typename T>
struct is_null_pointer : std::is_same<std::nullptr_t, typename std::remove_cv<T>::type>
{
};

/// @brief An alias template for std::remove_cv::type with C++11.
/// @note std::remove_cv_t is available since C++14.
/// @sa https://en.cppreference.com/w/cpp/types/remove_cv
/// @tparam T A type from which const-volatile qualifiers are removed.
template <typename T>
using remove_cv_t = typename std::remove_cv<T>::type;

/// @brief An alias template for std::remove_pointer::type with C++11.
/// @note std::remove_pointer_t is available since C++14.
/// @sa https://en.cppreference.com/w/cpp/types/remove_pointer
/// @tparam T A type from which a pointer is removed.
template <typename T>
using remove_pointer_t = typename std::remove_pointer<T>::type;

/// @brief An alias template for std::remove_reference::type with C++11.
/// @note std::remove_reference_t is available since C++14.
/// @sa https://en.cppreference.com/w/cpp/types/remove_reference
/// @tparam T A type from which a reference is removed.
template <typename T>
using remove_reference_t = typename std::remove_reference<T>::type;

#else

using std::add_pointer_t;
using std::enable_if_t;
using std::is_null_pointer;
using std::remove_cv_t;
using std::remove_pointer_t;
using std::remove_reference_t;

#endif

#ifndef FK_YAML_HAS_CXX_17

/// @brief A simple implementation to use std::bool_constant with C++11/C++14.
/// @tparam Val
template <bool Val>
using bool_constant = std::integral_constant<bool, Val>;

/// @brief A simple implementation to use std::void_t with C++11/C++14.
/// @note
/// std::conjunction is available since C++17.
/// This is applied when no traits are specified as inputs.
/// @sa https://en.cppreference.com/w/cpp/types/conjunction
/// @tparam Traits Type traits to be checked if their ::value are all true.
template <typename... Traits>
struct conjunction : std::true_type
{
};

/// @brief A partial specialization of conjunction if only one Trait is given.
/// @tparam Trait Type trait to be checked if its ::value is true.
template <typename Trait>
struct conjunction<Trait> : Trait
{
};

/// @brief A partial specialization of conjunction if more than one traits are given.
/// @tparam First The first type trait to be checked if its ::value is true.
/// @tparam Rest The rest of traits passed as another conjunction template arguments if First::value is true.
template <typename First, typename... Rest>
struct conjunction<First, Rest...> : std::conditional<First::value, conjunction<Rest...>, First>::type
{
};

/// @brief A simple implementation to use std::disjunction with C++11/C++14.
/// @note
/// std::disjunction is available since C++17.
/// This is applied when no traits are specified as inputs.
/// @sa https://en.cppreference.com/w/cpp/types/disjunction
/// @tparam Traits Type traits to be checked if at least one of their ::value is true.
template <typename... Traits>
struct disjunction : std::false_type
{
};

/// @brief A partial specialization of disjunction if only one Trait is given.
/// @tparam Trait Type trait to be checked if its ::value is true.
template <typename Trait>
struct disjunction<Trait> : Trait
{
};

/// @brief A partial specialization of disjunction if more than one traits are given.
/// @tparam First The first type trait to be checked if its ::value is true.
/// @tparam Rest The rest of traits passed as another conjunction template arguments if First::value is false.
template <typename First, typename... Rest>
struct disjunction<First, Rest...> : std::conditional<First::value, First, disjunction<Rest...>>::type
{
};

/// @brief A simple implementation to use std::negation with C++11/C++14.
/// @note std::negation is available since C++17.
/// @sa https://en.cppreference.com/w/cpp/types/negation
/// @tparam Trait Type trait whose ::value is negated.
template <typename Trait>
struct negation : std::integral_constant<bool, !Trait::value>
{
};

/// @brief A helper for void_t.
/// @tparam Types Any types to be transformed to void type.
template <typename... Types>
struct make_void
{
    using type = void;
};

/// @brief A simple implementation to use std::void_t with C++11/C++14.
/// @note std::void_t is available since C++17.
/// @sa https://en.cppreference.com/w/cpp/types/void_t
/// @tparam Types Any types to be transformed to void type.
template <typename... Types>
using void_t = typename make_void<Types...>::type;

#else

using std::bool_constant;
using std::conjunction;
using std::disjunction;
using std::negation;
using std::void_t;

#endif

#ifndef FK_YAML_HAS_CXX_20

/// @brief A simple implementation to use std::remove_cvref_t with C++11/C++14/C++17.
/// @note std::remove_cvref & std::remove_cvref_t are available since C++20.
/// @sa https://en.cppreference.com/w/cpp/types/remove_cvref
/// @tparam T A type from which cv-qualifiers and reference are removed.
template <typename T>
using remove_cvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

#else

using std::remove_cvref_t;

#endif

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_META_STL_SUPPLEMENT_HPP_ */

// #include <fkYAML/detail/meta/type_traits.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_META_TYPE_TRAITS_HPP_
#define FK_YAML_DETAIL_META_TYPE_TRAITS_HPP_

#include <limits>
#include <type_traits>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/meta/detect.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_META_DETECT_HPP_
#define FK_YAML_DETAIL_META_DETECT_HPP_

#include <type_traits>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/meta/stl_supplement.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief A dummy struct to represent detection failure.
struct nonesuch
{
    nonesuch() = delete;
    ~nonesuch() = delete;
    nonesuch(const nonesuch&) = delete;
    nonesuch(nonesuch&&) = delete;
    nonesuch& operator=(const nonesuch&) = delete;
    nonesuch& operator=(nonesuch&&) = delete;
};

/// @brief namespace to implement detector type traits
namespace detector_impl
{

/// @brief A helper for general type detection.
/// @tparam Default A type to represent detection failure.
/// @tparam AlwaysVoid This must be void type.
/// @tparam Op A type for desired operation type.
/// @tparam Args Argument types passed to desired operation.
template <typename Default, typename AlwaysVoid, template <typename...> class Op, typename... Args>
struct detector : std::false_type
{
    /// @brief A type which represents detection failure.
    using type = Default;
};

/// @brief A partial specialization of detector if desired operation type is found.
/// @tparam Default A type to represent detection failure.
/// @tparam Op A type for desired operation type.
/// @tparam Args Argument types passed to desired operation.
template <typename Default, template <typename...> class Op, typename... Args>
struct detector<Default, void_t<Op<Args...>>, Op, Args...> : std::true_type
{
    /// @brief A detected type.
    using type = Op<Args...>;
};

} // namespace detector_impl

/// @brief Type traits to detect Op operation with Args argument types
/// @tparam Op A desired operation type.
/// @tparam Args Argument types passed to desired operation.
template <template <typename...> class Op, typename... Args>
using is_detected = detector_impl::detector<nonesuch, void, Op, Args...>;

/// @brief Type traits to represent a detected type.
/// @tparam Op A type for desired operation type.
/// @tparam Args Argument types passed to desired operation.
template <template <typename...> class Op, typename... Args>
using detected_t = typename detector_impl::detector<nonesuch, void, Op, Args...>::type;

/// @brief Type traits to check if Expected and a detected type are exactly the same.
/// @tparam Expected An expected detection result type.
/// @tparam Op A type for desired operation.
/// @tparam Args Argument types passed to desired operation.
template <typename Expected, template <typename...> class Op, typename... Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_META_DETECT_HPP_ */

// #include <fkYAML/detail/meta/stl_supplement.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief Type trait to check if T and U are comparable types.
/// @tparam Comparator An object type to compare T and U objects.
/// @tparam T A type for comparison.
/// @tparam U The other type for comparison.
/// @tparam typename Placeholder for determining T and U are comparable types.
template <typename Comparator, typename T, typename U, typename = void>
struct is_comparable : std::false_type
{
};

/// @brief A partial specialization of is_comparable if T and U are comparable types.
/// @tparam Comparator An object type to compare T and U objects.
/// @tparam T A type for comparison.
/// @tparam U Ther other type for comparison.
template <typename Comparator, typename T, typename U>
struct is_comparable<
    Comparator, T, U,
    void_t<
        decltype(std::declval<Comparator>()(std::declval<T>(), std::declval<U>())),
        decltype(std::declval<Comparator>()(std::declval<U>(), std::declval<T>()))>> : std::true_type
{
};

/// @brief Type trait to check if KeyType can be used as key type.
/// @tparam Comparator An object type to compare T and U objects.
/// @tparam ObjectKeyType The original key type.
/// @tparam KeyType A type to be used as key type.
template <typename Comparator, typename ObjectKeyType, typename KeyType>
using is_usable_as_key_type = typename std::conditional<
    is_comparable<Comparator, ObjectKeyType, KeyType>::value, std::true_type, std::false_type>::type;

/// @brief Type trait to check if IntegralType is of non-boolean integral types.
/// @tparam IntegralType A type to be checked.
/// @tparam typename N/A
template <typename IntegralType, typename = void>
struct is_non_bool_integral : std::false_type
{
};

/// @brief A partial specialization of is_non_bool_integral if IntegralType is of non-boolean integral types.
/// @tparam IntegralType A type to be checked.
template <typename IntegralType>
struct is_non_bool_integral<
    IntegralType,
    enable_if_t<conjunction<std::is_integral<IntegralType>, negation<std::is_same<bool, IntegralType>>>::value>>
    : std::true_type
{
};

/// @brief Type traits to check if Types are all signed arithmetic types.
/// @tparam Types Types to check if they are all signed arithmetic types.
template <typename... Types>
using is_all_signed = conjunction<std::is_signed<Types>...>;

/// @brief Type traits to check if Types are all unsigned arithmetic types.
/// @tparam Types Types to check if they are all unsigned arithmetic types.
template <typename... Types>
using is_all_unsigned = conjunction<std::is_unsigned<Types>...>;

/// @brief Type trait implementation to check if TargetIntegerType and CompatibleIntegerType are compatible integer
/// types.
/// @tparam TargetIntegerType A target integer type.
/// @tparam CompatibleIntegerType A compatible integer type.
/// @tparam typename N/A
template <typename TargetIntegerType, typename CompatibleIntegerType, typename = void>
struct is_compatible_integer_type_impl : std::false_type
{
};

/// @brief A partial specialization of is_compatible_integer_type_impl if TargetIntegerType and CompatibleIntegerType
/// are compatible integer types.
/// @tparam TargetIntegerType A target integer type.
/// @tparam CompatibleIntegerType A compatible integer type.
template <typename TargetIntegerType, typename CompatibleIntegerType>
struct is_compatible_integer_type_impl<
    TargetIntegerType, CompatibleIntegerType, enable_if_t<is_non_bool_integral<CompatibleIntegerType>::value>>
    : std::true_type
{
};

/// @brief Type traits to check if TargetIntegerType and CompatibleIntegerType are compatible integer types.
/// @tparam TargetIntegerType A target integer type.
/// @tparam CompatibleIntegerType A compatible integer type.
template <typename TargetIntegerType, typename CompatibleIntegerType>
struct is_compatible_integer_type : is_compatible_integer_type_impl<TargetIntegerType, CompatibleIntegerType>
{
};

/// @brief Type traits to check if T is a complete type.
/// @tparam T A type to be checked if a complete type.
/// @tparam typename N/A
template <typename T, typename = void>
struct is_complete_type : std::false_type
{
};

/// @brief A partial specialization of is_complete_type if T is a complete type.
/// @tparam T
template <typename T>
struct is_complete_type<T, decltype(void(sizeof(T)))> : std::true_type
{
};

/// @brief A utility struct to generate static constant instance.
/// @tparam T A target type for the resulting static constant instance.
template <typename T>
struct static_const
{
    static FK_YAML_INLINE_VAR constexpr T value {}; // NOLINT(readability-identifier-naming)
};

#ifndef FK_YAML_HAS_CXX_17
/// @brief A instantiation of static_const::value instance.
/// @note This is required if inline variables are not available. C++11-14 do not provide such a feature yet.
/// @tparam T A target type for the resulting static constant instance.
template <typename T>
constexpr T static_const<T>::value;
#endif

/// @brief A helper structure for tag dispatch.
/// @tparam T A tag type.
template <typename T>
struct type_tag
{
    /// @brief A tagged type.
    using type = T;
};

/// @brief A utility struct to retrieve the first type in variadic template arguments.
/// @tparam Types Types of variadic template arguments.
template <typename... Types>
struct get_head_type;

/// @brief A specialization of get_head_type if variadic template has no arguments.
/// @tparam  N/A
template <>
struct get_head_type<>
{
    /// @brief A head type
    using type = void;
};

/// @brief A partial specialization of get_head_type if variadic template has one or more argument(s).
/// @tparam First The first type in the arguments
/// @tparam Rest The rest of the types in the arguments.
template <typename First, typename... Rest>
struct get_head_type<First, Rest...>
{
    /// @brief A head type.
    using type = First;
};

/// @brief An alias template to retrieve the first type in variadic template arguments.
/// @tparam Types Types of variadic template arguments.
template <typename... Types>
using head_type = typename get_head_type<Types...>::type;

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_META_TYPE_TRAITS_HPP_ */

// #include <fkYAML/exception.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_EXCEPTION_HPP_
#define FK_YAML_EXCEPTION_HPP_

#include <array>
#include <stdexcept>
#include <string>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/string_formatter.hpp>
#ifndef FK_YAML_DETAIL_STRING_FORMATTER_HPP_
#define FK_YAML_DETAIL_STRING_FORMATTER_HPP_

#include <cstdarg>
#include <cstdio>
#include <memory>
#include <string>

// #include <fkYAML/detail/macros/version_macros.hpp>


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

}; // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_STRING_FORMATTER_HPP_ */

// #include <fkYAML/detail/types/node_t.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_TYPES_NODE_T_HPP_
#define FK_YAML_DETAIL_TYPES_NODE_T_HPP_

#include <cstdint>
#include <string>

// #include <fkYAML/detail/macros/version_macros.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief Definition of node value types.
enum class node_t : std::uint32_t
{
    SEQUENCE,     //!< sequence value type
    MAPPING,      //!< mapping value type
    NULL_OBJECT,  //!< null value type
    BOOLEAN,      //!< boolean value type
    INTEGER,      //!< integer value type
    FLOAT_NUMBER, //!< float number value type
    STRING,       //!< string value type
};

inline const char* to_string(node_t t) noexcept
{
    switch (t)
    {
    case node_t::SEQUENCE:
        return "sequence";
    case node_t::MAPPING:
        return "mapping";
    case node_t::NULL_OBJECT:
        return "null";
    case node_t::BOOLEAN:
        return "boolean";
    case node_t::INTEGER:
        return "integer";
    case node_t::FLOAT_NUMBER:
        return "float";
    case node_t::STRING:
        return "string";
    default:       // LCOV_EXCL_LINE
        return ""; // LCOV_EXCL_LINE
    }
}

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_TYPES_NODE_T_HPP_ */


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief A base exception class used in fkYAML library.
/// @sa https://fktn-k.github.io/fkYAML/api/exception/
class exception : public std::exception
{
public:
    /// @brief Construct a new exception object without any error messages.
    /// @sa https://fktn-k.github.io/fkYAML/api/exception/constructor/
    exception() = default;

    /// @brief Construct a new exception object with an error message.
    /// @param[in] msg An error message.
    /// @sa https://fktn-k.github.io/fkYAML/api/exception/constructor/
    explicit exception(const char* msg) noexcept
    {
        if (msg)
        {
            m_error_msg = msg;
        }
    }

public:
    /// @brief Returns an error message internally held. If nothing, a non-null, empty string will be returned.
    /// @return An error message internally held. The message might be empty.
    /// @sa https://fktn-k.github.io/fkYAML/api/exception/what/
    const char* what() const noexcept override
    {
        return m_error_msg.c_str();
    }

private:
    /// An error message holder.
    std::string m_error_msg {};
};

/// @brief An exception class indicating an encoding error.
/// @sa https://fktn-k.github.io/fkYAML/api/exception/invalid_encoding/
class invalid_encoding : public exception
{
public:
    template <std::size_t N>
    explicit invalid_encoding(const char* msg, std::array<int, N> u8) noexcept
        : exception(generate_error_message(msg, u8).c_str())
    {
    }

    /// @brief Construct a new invalid_encoding object for UTF-16 related errors.
    /// @param msg An error message.
    /// @param u16_h The first UTF-16 encoded element used for the UTF-8 encoding.
    /// @param u16_l The second UTF-16 encoded element used for the UTF-8 encoding.
    explicit invalid_encoding(const char* msg, std::array<char16_t, 2> u16) noexcept
        : exception(generate_error_message(msg, u16).c_str())
    {
    }

    /// @brief Construct a new invalid_encoding object for UTF-32 related errors.
    /// @param msg An error message.
    /// @param u32 The UTF-32 encoded element used for the UTF-8 encoding.
    explicit invalid_encoding(const char* msg, char32_t u32) noexcept
        : exception(generate_error_message(msg, u32).c_str())
    {
    }

private:
    template <std::size_t N>
    std::string generate_error_message(const char* msg, std::array<int, N> u8) const noexcept
    {
        std::string formatted = detail::format("invalid_encoding: %s in=[ 0x%02x", msg, u8[0]);
        for (std::size_t i = 1; i < N; i++)
        {
            formatted += detail::format(", 0x%02x", u8[i]);
        }
        formatted += " ]";
        return formatted;
    }

    /// @brief Generate an error message from the given parameters for the UTF-16 encoding.
    /// @param msg An error message.
    /// @param h The first UTF-16 encoded element used for the UTF-8 encoding.
    /// @param l The second UTF-16 encoded element used for the UTF-8 encoding.
    /// @return A generated error message.
    std::string generate_error_message(const char* msg, std::array<char16_t, 2> u16) const noexcept
    {
        // uint16_t is large enough for UTF-16 encoded elements.
        return detail::format("invalid_encoding: %s in=[ 0x%04x, 0x%04x ]", msg, uint16_t(u16[0]), uint16_t(u16[1]));
    }

    /// @brief Generate an error message from the given parameters for the UTF-32 encoding.
    /// @param msg An error message.
    /// @param u32 The UTF-32 encoded element used for the UTF-8 encoding.
    /// @return A genereated error message.
    std::string generate_error_message(const char* msg, char32_t u32) const noexcept
    {
        // uint32_t is large enough for UTF-32 encoded elements.
        return detail::format("invalid_encoding: %s in=0x%08x", msg, uint32_t(u32));
    }
};

/// @brief An exception class indicating an error in parsing.
class parse_error : public exception
{
public:
    explicit parse_error(const char* msg, std::size_t lines, std::size_t cols_in_line) noexcept
        : exception(generate_error_message(msg, lines, cols_in_line).c_str())
    {
    }

private:
    std::string generate_error_message(const char* msg, std::size_t lines, std::size_t cols_in_line) const noexcept
    {
        return detail::format("parse_error: %s (at line %zu, column %zu)", msg, lines, cols_in_line);
    }
};

/// @brief An exception class indicating an invalid type conversion.
/// @sa https://fktn-k.github.io/fkYAML/api/exception/type_error/
class type_error : public exception
{
public:
    /// @brief Construct a new type_error object with an error message and a node type.
    /// @param[in] msg An error message.
    /// @param[in] type The type of a source node value.
    explicit type_error(const char* msg, detail::node_t type) noexcept
        : exception(generate_error_message(msg, type).c_str())
    {
    }

private:
    /// @brief Generate an error message from given parameters.
    /// @param msg An error message.
    /// @param type The type of a source node value.
    /// @return A generated error message.
    std::string generate_error_message(const char* msg, detail::node_t type) const noexcept
    {
        return detail::format("type_error: %s type=%s", msg, detail::to_string(type));
    }
};

class out_of_range : public exception
{
public:
    explicit out_of_range(int index) noexcept
        : exception(generate_error_message(index).c_str())
    {
    }

    explicit out_of_range(const char* key) noexcept
        : exception(generate_error_message(key).c_str())
    {
    }

private:
    std::string generate_error_message(int index)
    {
        return detail::format("out_of_range: index %d is out of range", index);
    }

    std::string generate_error_message(const char* key)
    {
        return detail::format("out_of_range: key \'%s\' is not found.", key);
    }
};

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_EXCEPTION_HPP_ */


FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief Convert a string YAML token to a ValueType object.
/// @tparam ValueType A target value type.
/// @tparam CharType The type of characters in a source string.
template <typename ValueType, typename CharType>
inline ValueType from_string(const std::basic_string<CharType>& s, type_tag<ValueType> /*unused*/);

/// @brief Specialization of from_string() for null values with std::string
/// @tparam  N/A
template <>
inline std::nullptr_t from_string(const std::string& s, type_tag<std::nullptr_t> /*unused*/)
{
    if (s == "null" || s == "Null" || s == "NULL" || s == "~")
    {
        return nullptr;
    }

    throw exception("Cannot convert a string into a null value.");
}

/// @brief Specialization of from_string() for boolean values with std::string.
/// @tparam  N/A
template <>
inline bool from_string(const std::string& s, type_tag<bool> /*unused*/)
{
    if (s == "true" || s == "True" || s == "TRUE")
    {
        return true;
    }

    if (s == "false" || s == "False" || s == "FALSE")
    {
        return false;
    }

    throw exception("Cannot convert a string into a boolean value.");
}

/// @brief Specialization of from_string() for int values with std::string.
/// @tparam  N/A
template <>
inline int from_string(const std::string& s, type_tag<int> /*unused*/)
{
    std::size_t idx = 0;
    long ret = 0;

    try
    {
        ret = std::stoi(s, &idx, 0);
    }
    catch (const std::exception& /*unused*/)
    {
        throw exception("Failed to convert a string into an int value.");
    }

    return ret;
}

/// @brief Specialization of from_string() for long values with std::string.
/// @tparam  N/A
template <>
inline long from_string(const std::string& s, type_tag<long> /*unused*/)
{
    std::size_t idx = 0;
    long ret = 0;

    try
    {
        ret = std::stol(s, &idx, 0);
    }
    catch (const std::exception& /*unused*/)
    {
        throw exception("Failed to convert a string into a long value.");
    }

    return ret;
}

/// @brief Specialization of from_string() for long long values with std::string.
/// @tparam  N/A
template <>
inline long long from_string(const std::string& s, type_tag<long long> /*unused*/)
{
    std::size_t idx = 0;
    long long ret = 0;

    try
    {
        ret = std::stoll(s, &idx, 0);
    }
    catch (const std::exception& /*unused*/)
    {
        throw exception("Failed to convert a string into a long long value.");
    }

    return ret;
}

/// @brief Partial specialization of from_string() for other signed integer types with std::string.
/// @tparam SignedIntType A signed integer type other than long long.
template <typename SignedIntType>
inline enable_if_t<
    conjunction<
        is_non_bool_integral<SignedIntType>, std::is_signed<SignedIntType>, negation<std::is_same<SignedIntType, int>>,
        negation<std::is_same<SignedIntType, long>>, negation<std::is_same<SignedIntType, long long>>>::value,
    SignedIntType>
from_string(const std::string& s, type_tag<SignedIntType> /*unused*/)
{
    const auto tmp_ret = from_string(s, type_tag<int> {});
    if (static_cast<long long>(std::numeric_limits<SignedIntType>::max()) < tmp_ret)
    {
        throw exception("Failed to convert a long long value into a SignedIntegerType value.");
    }

    return static_cast<SignedIntType>(tmp_ret);
}

/// @brief Specialization of from_string() for unsigned long values with std::string.
/// @tparam  N/A
template <>
inline unsigned long from_string(const std::string& s, type_tag<unsigned long> /*unused*/)
{
    std::size_t idx = 0;
    unsigned long ret = 0;

    try
    {
        ret = std::stoul(s, &idx, 0);
    }
    catch (const std::exception& /*unused*/)
    {
        throw exception("Failed to convert a string into an unsigned long value.");
    }

    return ret;
}

/// @brief Specialization of from_string() for unsigned long long values with std::string.
/// @tparam  N/A
template <>
inline unsigned long long from_string(const std::string& s, type_tag<unsigned long long> /*unused*/)
{
    std::size_t idx = 0;
    unsigned long long ret = 0;

    try
    {
        ret = std::stoull(s, &idx, 0);
    }
    catch (const std::exception& /*unused*/)
    {
        throw exception("Failed to convert a string into an unsigned long long value.");
    }

    return ret;
}

/// @brief Partial specialization of from_string() for other unsigned integer types with std::string.
/// @tparam UnsignedIntType An unsigned integer type other than unsigned long long.
template <typename UnsignedIntType>
inline enable_if_t<
    conjunction<
        is_non_bool_integral<UnsignedIntType>, std::is_unsigned<UnsignedIntType>,
        negation<std::is_same<UnsignedIntType, unsigned long>>,
        negation<std::is_same<UnsignedIntType, unsigned long long>>>::value,
    UnsignedIntType>
from_string(const std::string& s, type_tag<UnsignedIntType> /*unused*/)
{
    const auto tmp_ret = from_string(s, type_tag<unsigned long> {});
    if (static_cast<long long>(std::numeric_limits<UnsignedIntType>::max()) < tmp_ret)
    {
        throw exception("Failed to convert an unsigned long long into an UnsignedInteger value.");
    }

    return static_cast<UnsignedIntType>(tmp_ret);
}

/// @brief Specialization of from_string() for float values with std::string.
/// @tparam  N/A
template <>
inline float from_string(const std::string& s, type_tag<float> /*unused*/)
{
    if (s == ".inf" || s == ".Inf" || s == ".INF")
    {
        return std::numeric_limits<float>::infinity();
    }

    if (s == "-.inf" || s == "-.Inf" || s == "-.INF")
    {
        static_assert(std::numeric_limits<float>::is_iec559, "IEEE 754 required.");
        return -1 * std::numeric_limits<float>::infinity();
    }

    if (s == ".nan" || s == ".NaN" || s == ".NAN")
    {
        return std::nanf("");
    }

    std::size_t idx = 0;
    float ret = 0.0f;

    try
    {
        ret = std::stof(s, &idx);
    }
    catch (const std::exception& /*unused*/)
    {
        throw exception("Failed to a string into a float value.");
    }

    return ret;
}

/// @brief Specialization of from_string() for double values with std::string.
/// @tparam  N/A
template <>
inline double from_string(const std::string& s, type_tag<double> /*unused*/)
{
    if (s == ".inf" || s == ".Inf" || s == ".INF")
    {
        return std::numeric_limits<double>::infinity();
    }

    if (s == "-.inf" || s == "-.Inf" || s == "-.INF")
    {
        static_assert(std::numeric_limits<double>::is_iec559, "IEEE 754 required.");
        return -1 * std::numeric_limits<double>::infinity();
    }

    if (s == ".nan" || s == ".NaN" || s == ".NAN")
    {
        return std::nan("");
    }

    std::size_t idx = 0;
    double ret = 0.0;

    try
    {
        ret = std::stod(s, &idx);
    }
    catch (const std::exception& /*unused*/)
    {
        throw exception("Failed to a string into a double value.");
    }

    return ret;
}

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_CONVERSIONS_FROM_STRING_HPP_ */

// #include <fkYAML/detail/encodings/utf8_encoding.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_ENCODINGS_UTF_ENCODING_HPP_
#define FK_YAML_DETAIL_ENCODINGS_UTF_ENCODING_HPP_

#include <array>
#include <cstdint>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/exception.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

template <typename CharType>
class utf_encoding;

/////////////////////////
//   UTF-8 Encoding   ///
/////////////////////////

class utf8_encoding
{
    using int_type = std::char_traits<char>::int_type;

public:
    /// @brief Validates the encoding of a given byte array whose length is 1.
    /// @param[in] byte_array The byte array to be validated.
    /// @return true if a given byte array is valid, false otherwise.
    static bool validate(std::array<int_type, 1> byte_array) noexcept
    {
        // U+0000..U+007F
        return (0x00 <= byte_array[0] && byte_array[0] <= 0x7F);
    }

    /// @brief Validates the encoding of a given byte array whose length is 2.
    /// @param[in] byte_array The byte array to be validated.
    /// @return true if a given byte array is valid, false otherwise.
    static bool validate(std::array<int_type, 2> byte_array) noexcept
    {
        // U+0080..U+07FF
        //   1st Byte: 0xC2..0xDF
        //   2nd Byte: 0x80..0xBF
        if (0xC2 <= byte_array[0] && byte_array[0] <= 0xDF)
        {
            if (0x80 <= byte_array[1] && byte_array[1] <= 0xBF)
            {
                return true;
            }
        }

        // The rest of byte combinations are invalid.
        return false;
    }

    /// @brief Validates the encoding of a given byte array whose length is 3.
    /// @param[in] byte_array The byte array to be validated.
    /// @return true if a given byte array is valid, false otherwise.
    static bool validate(std::array<int_type, 3> byte_array) noexcept
    {
        // U+1000..U+CFFF:
        //   1st Byte: 0xE0..0xEC
        //   2nd Byte: 0x80..0xBF
        //   3rd Byte: 0x80..0xBF
        if (0xE0 <= byte_array[0] && byte_array[0] <= 0xEC)
        {
            if (0x80 <= byte_array[1] && byte_array[1] <= 0xBF)
            {
                if (0x80 <= byte_array[2] && byte_array[2] <= 0xBF)
                {
                    return true;
                }
            }
            return false;
        }

        // U+D000..U+D7FF:
        //   1st Byte: 0xED
        //   2nd Byte: 0x80..0x9F
        //   3rd Byte: 0x80..0xBF
        if (byte_array[0] == 0xED)
        {
            if (0x80 <= byte_array[1] && byte_array[1] <= 0x9F)
            {
                if (0x80 <= byte_array[2] && byte_array[2] <= 0xBF)
                {
                    return true;
                }
            }
            return false;
        }

        // U+E000..U+FFFF:
        //   1st Byte: 0xEE..0xEF
        //   2nd Byte: 0x80..0xBF
        //   3rd Byte: 0x80..0xBF
        if (byte_array[0] == 0xEE || byte_array[0] == 0xEF)
        {
            if (0x80 <= byte_array[1] && byte_array[1] <= 0xBF)
            {
                if (0x80 <= byte_array[2] && byte_array[2] <= 0xBF)
                {
                    return true;
                }
            }
            return false;
        }

        // The rest of byte combinations are invalid.
        return false;
    }

    /// @brief Validates the encoding of a given byte array whose length is 4.
    /// @param[in] byte_array The byte array to be validated.
    /// @return true if a given byte array is valid, false otherwise.
    static bool validate(std::array<int_type, 4> byte_array) noexcept
    {
        // U+10000..U+3FFFF:
        //   1st Byte: 0xF0
        //   2nd Byte: 0x90..0xBF
        //   3rd Byte: 0x80..0xBF
        //   4th Byte: 0x80..0xBF
        if (byte_array[0] == 0xF0)
        {
            if (0x90 <= byte_array[1] && byte_array[1] <= 0xBF)
            {
                if (0x80 <= byte_array[2] && byte_array[2] <= 0xBF)
                {
                    if (0x80 <= byte_array[3] && byte_array[3] <= 0xBF)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        // U+40000..U+FFFFF:
        //   1st Byte: 0xF1..0xF3
        //   2nd Byte: 0x80..0xBF
        //   3rd Byte: 0x80..0xBF
        //   4th Byte: 0x80..0xBF
        if (0xF1 <= byte_array[0] && byte_array[0] <= 0xF3)
        {
            if (0x80 <= byte_array[1] && byte_array[1] <= 0xBF)
            {
                if (0x80 <= byte_array[2] && byte_array[2] <= 0xBF)
                {
                    if (0x80 <= byte_array[3] && byte_array[3] <= 0xBF)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        // U+100000..U+10FFFF:
        //   1st Byte: 0xF4
        //   2nd Byte: 0x80..0x8F
        //   3rd Byte: 0x80..0xBF
        //   4th Byte: 0x80..0xBF
        if (byte_array[0] == 0xF4)
        {
            if (0x80 <= byte_array[1] && byte_array[1] <= 0x8F)
            {
                if (0x80 <= byte_array[2] && byte_array[2] <= 0xBF)
                {
                    if (0x80 <= byte_array[3] && byte_array[3] <= 0xBF)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        // The rest of byte combinations are invalid.
        return false;
    }

    /// @brief Converts UTF-16 encoded characters to UTF-8 encoded bytes.
    /// @param[in] utf16 UTF-16 encoded character(s).
    /// @param[out] utf8_bytes UTF-8 encoded bytes.
    /// @param[out] consumed_size The number of UTF-16 encoded characters used for the conversion.
    /// @param[out] encoded_size The size of UTF-encoded bytes.
    static void from_utf16(
        std::array<char16_t, 2> utf16, std::array<char, 4>& utf8_bytes, std::size_t& consumed_size,
        std::size_t& encoded_size)
    {
        utf8_bytes.fill(0);
        consumed_size = 0;
        encoded_size = 0;
        bool is_valid = false;

        if (utf16[0] < char16_t(0x80u))
        {
            utf8_bytes[0] = static_cast<char>(utf16[0] & 0x7Fu);
            consumed_size = 1;
            encoded_size = 1;
            is_valid = true;
        }
        else if (utf16[0] <= char16_t(0x7FFu))
        {
            uint16_t utf8_encoded = 0xC080u;
            utf8_encoded |= static_cast<uint16_t>((utf16[0] & 0x07C0u) << 2);
            utf8_encoded |= static_cast<uint16_t>(utf16[0] & 0x003Fu);
            utf8_bytes[0] = static_cast<char>((utf8_encoded & 0xFF00u) >> 8);
            utf8_bytes[1] = static_cast<char>(utf8_encoded & 0x00FFu);
            consumed_size = 1;
            encoded_size = 2;
            is_valid = true;
        }
        else if (utf16[0] < char16_t(0xD800u) || char16_t(0xE000u) <= utf16[0])
        {
            uint32_t utf8_encoded = 0xE08080u;
            utf8_encoded |= static_cast<uint32_t>((utf16[0] & 0xF000u) << 4);
            utf8_encoded |= static_cast<uint32_t>((utf16[0] & 0x0FC0u) << 2);
            utf8_encoded |= static_cast<uint32_t>(utf16[0] & 0x003Fu);
            utf8_bytes[0] = static_cast<char>((utf8_encoded & 0xFF0000u) >> 16);
            utf8_bytes[1] = static_cast<char>((utf8_encoded & 0x00FF00u) >> 8);
            utf8_bytes[2] = static_cast<char>(utf8_encoded & 0x0000FFu);
            consumed_size = 1;
            encoded_size = 3;
            is_valid = true;
        }
        else if (utf16[0] <= char16_t(0xDBFFu) && char16_t(0xDC00u) <= utf16[1] && utf16[1] <= char16_t(0xDFFFu))
        {
            // for surrogate pairs
            uint32_t code_point = 0x10000u + ((utf16[0] & 0x03FFu) << 10) + (utf16[1] & 0x03FFu);
            uint32_t utf8_encoded = 0xF0808080u;
            utf8_encoded |= static_cast<uint32_t>((code_point & 0x1C0000u) << 6);
            utf8_encoded |= static_cast<uint32_t>((code_point & 0x03F000u) << 4);
            utf8_encoded |= static_cast<uint32_t>((code_point & 0x000FC0u) << 2);
            utf8_encoded |= static_cast<uint32_t>(code_point & 0x00003Fu);
            utf8_bytes[0] = static_cast<char>((utf8_encoded & 0xFF000000u) >> 24);
            utf8_bytes[1] = static_cast<char>((utf8_encoded & 0x00FF0000u) >> 16);
            utf8_bytes[2] = static_cast<char>((utf8_encoded & 0x0000FF00u) >> 8);
            utf8_bytes[3] = static_cast<char>(utf8_encoded & 0x000000FFu);
            consumed_size = 2;
            encoded_size = 4;
            is_valid = true;
        }

        if (!is_valid)
        {
            throw invalid_encoding("Invalid UTF-16 encoding detected.", utf16);
        }
    }

    /// @brief Converts a UTF-32 encoded character to UTF-8 encoded bytes.
    /// @param[in] utf32 A UTF-32 encoded character.
    /// @param[out] utf8_bytes UTF-8 encoded bytes.
    /// @param[in] encoded_size The size of UTF-encoded bytes.
    static void from_utf32(const char32_t utf32, std::array<char, 4>& utf8_bytes, std::size_t& encoded_size)
    {
        utf8_bytes.fill(0);
        encoded_size = 0;
        bool is_valid = false;

        if (utf32 < char32_t(0x80u))
        {
            utf8_bytes[0] = static_cast<char>(utf32 & 0x007F);
            encoded_size = 1;
            is_valid = true;
        }
        else if (utf32 <= char32_t(0x7FFu))
        {
            uint16_t utf8_encoded = 0xC080u;
            utf8_encoded |= static_cast<uint16_t>((utf32 & 0x07C0u) << 2);
            utf8_encoded |= static_cast<uint16_t>(utf32 & 0x003Fu);
            utf8_bytes[0] = static_cast<char>((utf8_encoded & 0xFF00u) >> 8);
            utf8_bytes[1] = static_cast<char>(utf8_encoded & 0x00FFu);
            encoded_size = 2;
            is_valid = true;
        }
        else if (utf32 <= char32_t(0xFFFFu))
        {
            uint32_t utf8_encoded = 0xE08080u;
            utf8_encoded |= static_cast<uint32_t>((utf32 & 0xF000u) << 4);
            utf8_encoded |= static_cast<uint32_t>((utf32 & 0x0FC0u) << 2);
            utf8_encoded |= static_cast<uint32_t>(utf32 & 0x003F);
            utf8_bytes[0] = static_cast<char>((utf8_encoded & 0xFF0000u) >> 16);
            utf8_bytes[1] = static_cast<char>((utf8_encoded & 0x00FF00u) >> 8);
            utf8_bytes[2] = static_cast<char>(utf8_encoded & 0x0000FFu);
            encoded_size = 3;
            is_valid = true;
        }
        else if (utf32 <= char32_t(0x10FFFFu))
        {
            uint32_t utf8_encoded = 0xF0808080u;
            utf8_encoded |= static_cast<uint32_t>((utf32 & 0x1C0000u) << 6);
            utf8_encoded |= static_cast<uint32_t>((utf32 & 0x03F000u) << 4);
            utf8_encoded |= static_cast<uint32_t>((utf32 & 0x000FC0u) << 2);
            utf8_encoded |= static_cast<uint32_t>(utf32 & 0x00003Fu);
            utf8_bytes[0] = static_cast<char>((utf8_encoded & 0xFF000000u) >> 24);
            utf8_bytes[1] = static_cast<char>((utf8_encoded & 0x00FF0000u) >> 16);
            utf8_bytes[2] = static_cast<char>((utf8_encoded & 0x0000FF00u) >> 8);
            utf8_bytes[3] = static_cast<char>(utf8_encoded & 0x000000FFu);
            encoded_size = 4;
            is_valid = true;
        }

        if (!is_valid)
        {
            throw invalid_encoding("Invalid UTF-32 encoding detected.", utf32);
        }
    }
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ENCODINGS_UTF_ENCODING_HPP_ */

// #include <fkYAML/detail/input/input_handler.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_INPUT_INPUT_HANDLER_HPP_
#define FK_YAML_DETAIL_INPUT_INPUT_HANDLER_HPP_

#include <string>
#include <utility>
#include <vector>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/meta/input_adapter_traits.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_META_INPUT_ADAPTER_TRAITS_HPP_
#define FK_YAML_DETAIL_META_INPUT_ADAPTER_TRAITS_HPP_

#include <type_traits>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/meta/detect.hpp>

// #include <fkYAML/detail/meta/stl_supplement.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/////////////////////////////////
//   API representative types
/////////////////////////////////

/// @brief A type which represents T::char_type;
/// @tparam T A target type to check if it has char_type;
template <typename T>
using detect_char_type_helper_t = typename T::char_type;

/// @brief A type which represents get_character function.
/// @tparam T A target type.
template <typename T>
using get_character_fn_t = decltype(std::declval<T>().get_character());

/// @brief Type traits to check if T has char_type as its member.
/// @tparam T A target type.
/// @tparam typename N/A
template <typename T, typename = void>
struct has_char_type : std::false_type
{
};

///////////////////////////////////////////
//   Input Adapter API detection traits
///////////////////////////////////////////

/// @brief A partial specialization of has_char_type if T has char_type as its member.
/// @tparam T A target type.
template <typename T>
struct has_char_type<T, enable_if_t<is_detected<detect_char_type_helper_t, T>::value>> : std::true_type
{
};

/// @brief Type traits to check if InputAdapterType has get_character member function.
/// @tparam InputAdapterType An input adapter type to check if it has get_character function.
/// @tparam typename N/A
template <typename InputAdapterType, typename = void>
struct has_get_character : std::false_type
{
};

/// @brief A partial specialization of has_get_character if InputAdapterType has get_character member function.
/// @tparam InputAdapterType A type of a target input adapter.
template <typename InputAdapterType>
struct has_get_character<InputAdapterType, enable_if_t<is_detected<get_character_fn_t, InputAdapterType>::value>>
    : std::true_type
{
};

////////////////////////////////
//   is_input_adapter traits
////////////////////////////////

/// @brief Type traits to check if T is an input adapter type.
/// @tparam T A target type.
/// @tparam typename N/A
template <typename T, typename = void>
struct is_input_adapter : std::false_type
{
};

/// @brief A partial specialization of is_input_adapter if T is an input adapter type.
/// @tparam InputAdapterType
template <typename InputAdapterType>
struct is_input_adapter<
    InputAdapterType,
    enable_if_t<conjunction<has_char_type<InputAdapterType>, has_get_character<InputAdapterType>>::value>>
    : std::true_type
{
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_META_INPUT_ADAPTER_TRAITS_HPP_ */

// #include <fkYAML/detail/meta/stl_supplement.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief An input buffer handler.
class input_handler
{
private:
    /// The type of character traits of the input buffer.
    using char_traits_type = std::char_traits<char>;

    /// @brief A set of information on the current position in an input buffer.
    struct position
    {
        /// The current position from the beginning of an input buffer.
        std::size_t cur_pos {0};
        /// The current position in the current line.
        std::size_t cur_pos_in_line {0};
        /// The number of lines which have already been read.
        std::size_t lines_read {0};
    };

public:
    /// @brief Construct a new input_handler object.
    /// @tparam InputAdapterType The type of the input adapter.
    /// @param input_adapter An input adapter object
    template <typename InputAdapterType, enable_if_t<is_input_adapter<InputAdapterType>::value, int> = 0>
    explicit input_handler(InputAdapterType&& input_adapter)
        : m_buffer_size(0)
    {
        m_buffer.clear();

        int ch = s_end_of_input;
        while ((ch = input_adapter.get_character()) != s_end_of_input)
        {
            m_buffer.push_back(char_traits_type::to_char_type(ch));
            m_buffer_size++;
        }
    }

    /// @brief Get the character at the current position.
    /// @return int A character or EOF.
    int get_current() const noexcept
    {
        if (m_position.cur_pos == m_buffer_size)
        {
            return s_end_of_input;
        }
        return char_traits_type::to_int_type(m_buffer[m_position.cur_pos]);
    }

    /// @brief Get the character at next position.
    /// @return int A character or EOF.
    int get_next()
    {
        // if all the input has already been consumed, return the EOF.
        if (m_position.cur_pos == m_buffer_size - 1)
        {
            m_position.cur_pos++;
            m_position.cur_pos_in_line++;
            return s_end_of_input;
        }

        if (m_position.cur_pos == m_buffer_size)
        {
            return s_end_of_input;
        }

        if (m_buffer[m_position.cur_pos] == '\n')
        {
            m_position.cur_pos_in_line = 0;
            ++m_position.lines_read;
        }
        else
        {
            m_position.cur_pos_in_line++;
        }

        return char_traits_type::to_int_type(m_buffer[++m_position.cur_pos]);
    }

    /// @brief Get the characters in the given range.
    /// @param length The length of characters retrieved from the current position.
    /// @param str A string which will contain the resulting characters.
    /// @return int 0 (for success) or EOF (for error).
    int get_range(std::size_t length, std::string& str)
    {
        str.clear();

        if (length == 0)
        {
            // regard this case as successful in getting zero characters.
            return 0;
        }

        if (m_position.cur_pos + length - 1 >= m_buffer_size)
        {
            return s_end_of_input;
        }

        str += m_buffer[m_position.cur_pos];

        for (std::size_t i = 1; i < length; i++)
        {
            str += char_traits_type::to_char_type(get_next());
        }

        return 0;
    }

    /// @brief Move backward the current position.
    void unget()
    {
        if (m_position.cur_pos > 0)
        {
            --m_position.cur_pos;
            --m_position.cur_pos_in_line;
            if (m_buffer[m_position.cur_pos] == '\n')
            {
                --m_position.lines_read;
                m_position.cur_pos_in_line = 0;
                if (m_position.cur_pos > 0)
                {
                    for (std::size_t i = m_position.cur_pos - 1; m_buffer[i] != '\n'; i--)
                    {
                        if (i == 0)
                        {
                            m_position.cur_pos_in_line = m_position.cur_pos;
                            break;
                        }
                        ++m_position.cur_pos_in_line;
                    }
                }
            }
        }
    }

    /// @brief Move backward the current position to the given range.
    /// @param length The length of moving backward.
    void unget_range(std::size_t length)
    {
        size_t unget_num = (m_position.cur_pos < length) ? m_position.cur_pos : length;
        for (std::size_t i = 0; i < unget_num; i++)
        {
            unget();
        }
    }

    /// @brief Check if the next character is the expected one.
    /// @param expected An expected next character.
    /// @return true The next character is the expected one.
    /// @return false The next character is not the expected one.
    bool test_next_char(char expected)
    {
        if (m_position.cur_pos >= m_buffer_size - 1)
        {
            // there is no input character left.
            return false;
        }

        return char_traits_type::eq(m_buffer[m_position.cur_pos + 1], expected);
    }

    /// @brief Get the current position in the current line.
    /// @return std::size_t The current position in the current line.
    std::size_t get_cur_pos_in_line() const noexcept
    {
        return m_position.cur_pos_in_line;
    }

    /// @brief Get the number of lines which have already been read.
    /// @return std::size_t The number of lines which have already been read.
    std::size_t get_lines_read() const noexcept
    {
        return m_position.lines_read;
    }

private:
    /// The value of EOF for the target character type.
    static constexpr int s_end_of_input = char_traits_type::eof();

    /// The input buffer retrieved from an input adapter object.
    std::string m_buffer {};
    /// The size of the buffer.
    std::size_t m_buffer_size {0};
    /// The current position in an input buffer.
    position m_position {};
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_INPUT_HANDLER_HPP_ */

// #include <fkYAML/detail/meta/input_adapter_traits.hpp>

// #include <fkYAML/detail/meta/node_traits.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_META_NODE_TRAITS_HPP_
#define FK_YAML_DETAIL_META_NODE_TRAITS_HPP_

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/meta/detect.hpp>

// #include <fkYAML/detail/meta/stl_supplement.hpp>

// #include <fkYAML/detail/meta/type_traits.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

// forward declaration for basic_node<...>
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType,
    template <typename, typename> class ConverterType>
class basic_node;

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/////////////////////////////
//   is_basic_node traits
/////////////////////////////

/// @brief A struct to check the template parameter class is a kind of basic_node template class.
/// @tparam T A class to be checked if it's a kind of basic_node template class.
template <typename T>
struct is_basic_node : std::false_type
{
};

/// @brief A partial specialization of is_basic_node for basic_node template class.
/// @tparam SequenceType A type for sequence node value containers.
/// @tparam MappingType A type for mapping node value containers.
/// @tparam BooleanType A type for boolean node values.
/// @tparam IntegerType A type for integer node values.
/// @tparam FloatNumberType A type for float number node values.
/// @tparam StringType A type for string node values.
/// @tparam Converter A type for
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType,
    template <typename, typename> class Converter>
struct is_basic_node<
    basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, Converter>>
    : std::true_type
{
};

///////////////////////////////////
//   is_node_ref_storage traits
///////////////////////////////////

// forward declaration for node_ref_storage<...>
template <typename>
class node_ref_storage;

/// @brief A struct to check the template parameter class is a kind of node_ref_storage_template class.
/// @tparam T A type to be checked if it's a kind of node_ref_storage template class.
template <typename T>
struct is_node_ref_storage : std::false_type
{
};

/// @brief A partial specialization for node_ref_storage template class.
/// @tparam T A template parameter type of node_ref_storage template class.
template <typename T>
struct is_node_ref_storage<node_ref_storage<T>> : std::true_type
{
};

///////////////////////////////////////////////////////
//   basic_node conversion API representative types
///////////////////////////////////////////////////////

/// @brief A type represent from_node function.
/// @tparam T A type which provides from_node function.
/// @tparam Args Argument types passed to from_node function.
template <typename T, typename... Args>
using from_node_function_t = decltype(T::from_node(std::declval<Args>()...));

/// @brief A type which represent to_node function.
/// @tparam T A type which provides to_node function.
/// @tparam Args Argument types passed to to_node function.
template <typename T, typename... Args>
using to_node_funcion_t = decltype(T::to_node(std::declval<Args>()...));

///////////////////////////////////////////////////
//   basic_node conversion API detection traits
///////////////////////////////////////////////////

/// @brief Type traits to check if T is a compatible type for BasicNodeType in terms of from_node function.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A target type passed to from_node function.
/// @tparam typename N/A
template <typename BasicNodeType, typename T, typename = void>
struct has_from_node : std::false_type
{
};

/// @brief A partial specialization of has_from_node if T is not a basic_node template instance type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A target type passed to from_node function.
template <typename BasicNodeType, typename T>
struct has_from_node<BasicNodeType, T, enable_if_t<!is_basic_node<T>::value>>
{
    using converter = typename BasicNodeType::template value_converter_type<T, void>;

    // NOLINTNEXTLINE(readability-identifier-naming)
    static constexpr bool value =
        is_detected_exact<void, from_node_function_t, converter, const BasicNodeType&, T&>::value;
};

/// @brief Type traits to check if T is a compatible type for BasicNodeType in terms of to_node function.
/// @warning Do not pass basic_node type as BasicNodeType to avoid infinite type instantiation.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A target type passed to to_node function.
/// @tparam typename N/A
template <typename BasicNodeType, typename T, typename = void>
struct has_to_node : std::false_type
{
};

/// @brief A partial specialization of has_to_node if T is not a basic_node template instance type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A target type passed to to_node function.
template <typename BasicNodeType, typename T>
struct has_to_node<BasicNodeType, T, enable_if_t<!is_basic_node<T>::value>>
{
    using converter = typename BasicNodeType::template value_converter_type<T, void>;

    // NOLINTNEXTLINE(readability-identifier-naming)
    static constexpr bool value = is_detected_exact<void, to_node_funcion_t, converter, BasicNodeType&, T>::value;
};

///////////////////////////////////////
//   is_node_compatible_type traits
///////////////////////////////////////

/// @brief Type traits implementation of is_node_compatible_type to check if CompatibleType is a compatible type for
/// BasicNodeType.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam CompatibleType A target type for compatibility check.
/// @tparam typename N/A
template <typename BasicNodeType, typename CompatibleType, typename = void>
struct is_node_compatible_type_impl : std::false_type
{
};

/// @brief A partial specialization of is_node_compatible_type_impl if CompatibleType is a complete type and is
/// compatible for BasicNodeType.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam CompatibleType A target type for compatibility check.
template <typename BasicNodeType, typename CompatibleType>
struct is_node_compatible_type_impl<
    BasicNodeType, CompatibleType,
    enable_if_t<conjunction<is_complete_type<CompatibleType>, has_to_node<BasicNodeType, CompatibleType>>::value>>
    : std::true_type
{
};

/// @brief Type traits to check if CompatibleType is a compatible type for BasicNodeType.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam CompatibleType A target type for compatibility check.
template <typename BasicNodeType, typename CompatibleType>
struct is_node_compatible_type : is_node_compatible_type_impl<BasicNodeType, CompatibleType>
{
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_META_NODE_TRAITS_HPP_ */

// #include <fkYAML/detail/meta/stl_supplement.hpp>

// #include <fkYAML/detail/types/lexical_token_t.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_TYPES_LEXICAL_TOKEN_T_HPP_
#define FK_YAML_DETAIL_TYPES_LEXICAL_TOKEN_T_HPP_

// #include <fkYAML/detail/macros/version_macros.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief Definition of lexical token types.
enum class lexical_token_t
{
    END_OF_BUFFER,         //!< the end of input buffer.
    EXPLICIT_KEY_PREFIX,   //!< the character for explicit mapping key prefix `?`.
    KEY_SEPARATOR,         //!< the key separater `:`
    VALUE_SEPARATOR,       //!< the value separater `,`
    ANCHOR_PREFIX,         //!< the character for anchor prefix `&`
    ALIAS_PREFIX,          //!< the character for alias prefix `*`
    COMMENT_PREFIX,        //!< the character for comment prefix `#`
    YAML_VER_DIRECTIVE,    //!< a YAML version directive found. use get_yaml_version() to get a value.
    TAG_DIRECTIVE,         //!< a TAG directive found. use GetTagInfo() to get the tag information.
    INVALID_DIRECTIVE,     //!< an invalid directive found. do not try to get the value.
    SEQUENCE_BLOCK_PREFIX, //!< the character for sequence block prefix `- `
    SEQUENCE_FLOW_BEGIN,   //!< the character for sequence flow begin `[`
    SEQUENCE_FLOW_END,     //!< the character for sequence flow end `]`
    MAPPING_FLOW_BEGIN,    //!< the character for mapping begin `{`
    MAPPING_FLOW_END,      //!< the character for mapping end `}`
    NULL_VALUE,            //!< a null value found. use get_null() to get a value.
    BOOLEAN_VALUE,         //!< a boolean value found. use get_boolean() to get a value.
    INTEGER_VALUE,         //!< an integer value found. use get_integer() to get a value.
    FLOAT_NUMBER_VALUE,    //!< a float number value found. use get_float_number() to get a value.
    STRING_VALUE,          //!< the character for string begin `"` or any character except the above ones
    END_OF_DIRECTIVES,     //!< the end of declaration of directives specified by `---`.
    END_OF_DOCUMENT,       //!< the end of a YAML document specified by `...`.
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_TYPES_LEXICAL_TOKEN_T_HPP_ */

// #include <fkYAML/exception.hpp>


/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

/**
 * @namespace detail
 * @brief namespace for internal implementations of fkYAML library.
 */
namespace detail
{

/// @brief A class which lexically analizes YAML formatted inputs.
/// @tparam BasicNodeType A type of the container for YAML values.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
class lexical_analyzer
{
private:
    using char_traits_type = typename std::char_traits<char>;

    enum class block_style_indicator_t
    {
        LITERAL, //!< keeps newlines inside the block as they are indicated by a pipe `|`.
        FOLDED,  //!< replaces newlines inside the block with spaces indicated by a right angle bracket `>`.
    };

    enum class chomping_indicator_t
    {
        STRIP, //!< excludes final line breaks and traiing empty lines indicated by `-`.
        KEEP,  //!< preserves final line breaks but excludes trailing empty lines. no indicator means this type.
        CLIP,  //!< preserves final line breaks and trailing empty lines indicated by `+`.
    };

public:
    using boolean_type = typename BasicNodeType::boolean_type;
    using integer_type = typename BasicNodeType::integer_type;
    using float_number_type = typename BasicNodeType::float_number_type;
    using string_type = typename BasicNodeType::string_type;

    /// @brief Construct a new lexical_analyzer object.
    /// @tparam InputAdapterType The type of the input adapter.
    /// @param input_adapter An input adapter object.
    template <typename InputAdapterType, enable_if_t<is_input_adapter<InputAdapterType>::value, int> = 0>
    explicit lexical_analyzer(InputAdapterType&& input_adapter)
        : m_input_handler(std::move(input_adapter))
    {
    }

    /// @brief Get the next lexical token type by scanning the left of the input buffer.
    /// @return lexical_token_t The next lexical token type.
    lexical_token_t get_next_token()
    {
        skip_white_spaces_and_newline_codes();

        int current = m_input_handler.get_current();
        m_last_token_begin_pos = m_input_handler.get_cur_pos_in_line();
        m_last_token_begin_line = m_input_handler.get_lines_read();

        if (0x00 <= current && current <= 0x7F && isdigit(current))
        {
            return m_last_token_type = scan_number();
        }

        switch (current)
        {
        case s_end_of_input: // end of input buffer
            return m_last_token_type = lexical_token_t::END_OF_BUFFER;
        case '?':
            switch (m_input_handler.get_next())
            {
            case ' ':
                return m_last_token_type = lexical_token_t::EXPLICIT_KEY_PREFIX;
            default:
                m_value_buffer = "?";
                return m_last_token_type = scan_string(false);
            }
        case ':': { // key separater
            current = m_input_handler.get_next();
            switch (current)
            {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
            case s_end_of_input:
                break;
            case ',':
            case '[':
            case ']':
            case '{':
            case '}':
                if (m_flow_context_depth > 0)
                {
                    // the above characters are not "safe" to be followed in a flow context.
                    // See https://yaml.org/spec/1.2.2/#733-plain-style for more details.
                    break;
                }
                m_value_buffer = ":";
                return scan_string(false);
            default:
                m_value_buffer = ":";
                return scan_string(false);
            }

            return m_last_token_type = lexical_token_t::KEY_SEPARATOR;
        }
        case ',': // value separater
            m_input_handler.get_next();
            return m_last_token_type = lexical_token_t::VALUE_SEPARATOR;
        case '&': { // anchor prefix
            m_value_buffer.clear();
            while (true)
            {
                int next = m_input_handler.get_next();
                if (next == s_end_of_input || next == '\r' || next == '\n')
                {
                    emit_error("An anchor label must be followed by some value.");
                }
                if (next == ' ')
                {
                    m_input_handler.get_next();
                    break;
                }
                m_value_buffer.push_back(char_traits_type::to_char_type(next));
            }
            return m_last_token_type = lexical_token_t::ANCHOR_PREFIX;
        }
        case '*': { // alias prefix
            m_value_buffer.clear();
            while (true)
            {
                int next = m_input_handler.get_next();
                if (next == ' ' || next == '\r' || next == '\n' || next == s_end_of_input)
                {
                    if (m_value_buffer.empty())
                    {
                        emit_error("An alias prefix must be followed by some anchor name.");
                    }
                    m_input_handler.get_next();
                    break;
                }
                m_value_buffer.push_back(char_traits_type::to_char_type(next));
            }
            return m_last_token_type = lexical_token_t::ALIAS_PREFIX;
        }
        case '#': // comment prefix
            scan_comment();
            return m_last_token_type = lexical_token_t::COMMENT_PREFIX;
        case '%': // directive prefix
            return m_last_token_type = scan_directive();
        case '-': {
            int next = m_input_handler.get_next();
            if (next == ' ')
            {
                // Move a cursor to the beginning of the next token.
                m_input_handler.get_next();
                return m_last_token_type = lexical_token_t::SEQUENCE_BLOCK_PREFIX;
            }

            m_input_handler.unget();
            if (std::isdigit(next))
            {
                return m_last_token_type = scan_number();
            }

            int ret = m_input_handler.get_range(3, m_value_buffer);
            if (ret != s_end_of_input)
            {
                if (m_value_buffer == "---")
                {
                    m_input_handler.get_next();
                    return m_last_token_type = lexical_token_t::END_OF_DIRECTIVES;
                }

                m_input_handler.unget_range(2);
            }

            return m_last_token_type = scan_string();
        }
        case '[': // sequence flow begin
            m_flow_context_depth++;
            m_input_handler.get_next();
            return m_last_token_type = lexical_token_t::SEQUENCE_FLOW_BEGIN;
        case ']': // sequence flow end
            if (m_flow_context_depth == 0)
            {
                emit_error("An invalid flow sequence ending.");
            }
            m_flow_context_depth--;
            m_input_handler.get_next();
            return m_last_token_type = lexical_token_t::SEQUENCE_FLOW_END;
        case '{': // mapping flow begin
            m_flow_context_depth++;
            m_input_handler.get_next();
            return m_last_token_type = lexical_token_t::MAPPING_FLOW_BEGIN;
        case '}': // mapping flow end
            if (m_flow_context_depth == 0)
            {
                emit_error("An invalid flow mapping ending.");
            }
            m_flow_context_depth--;
            m_input_handler.get_next();
            return m_last_token_type = lexical_token_t::MAPPING_FLOW_END;
        case '@':
            emit_error("Any token cannot start with at(@). It is a reserved indicator for YAML.");
        case '`':
            emit_error("Any token cannot start with grave accent(`). It is a reserved indicator for YAML.");
        case '\"':
        case '\'':
            return m_last_token_type = scan_string();
        case '+':
            return m_last_token_type = scan_number();
        case '.': {
            int ret = m_input_handler.get_range(3, m_value_buffer);
            if (ret != s_end_of_input)
            {
                if (m_value_buffer == "...")
                {
                    m_input_handler.get_next();
                    return m_last_token_type = lexical_token_t::END_OF_DOCUMENT;
                }

                // revert change in the position to the one before comparison above.
                m_input_handler.unget_range(2);
            }

            return m_last_token_type = scan_string();
        }
        case '|': {
            chomping_indicator_t chomp_type = chomping_indicator_t::KEEP;
            std::size_t indent = 0;
            get_block_style_metadata(chomp_type, indent);
            return m_last_token_type =
                       scan_block_style_string_token(block_style_indicator_t::LITERAL, chomp_type, indent);
        }
        case '>': {
            chomping_indicator_t chomp_type = chomping_indicator_t::KEEP;
            std::size_t indent = 0;
            get_block_style_metadata(chomp_type, indent);
            return m_last_token_type =
                       scan_block_style_string_token(block_style_indicator_t::FOLDED, chomp_type, indent);
        }
        default:
            return m_last_token_type = scan_string();
        }
    }

    /// @brief Get the beginning position of a last token.
    /// @return std::size_t The beginning position of a last token.
    std::size_t get_last_token_begin_pos() const noexcept
    {
        return m_last_token_begin_pos;
    }

    /// @brief Get the number of lines already processed.
    /// @return std::size_t The number of lines already processed.
    std::size_t get_lines_processed() const noexcept
    {
        return m_last_token_begin_line;
    }

    /// @brief Convert from string to null and get the converted value.
    /// @return std::nullptr_t A null value converted from one of the followings: "null", "Null", "NULL", "~".
    std::nullptr_t get_null() const
    {
        if (m_last_token_type == lexical_token_t::NULL_VALUE)
        {
            return nullptr;
        }
        emit_error("Invalid request for a null value.");
    }

    /// @brief Convert from string to boolean and get the converted value.
    /// @return true  A string token is one of the followings: "true", "True", "TRUE".
    /// @return false A string token is one of the followings: "false", "False", "FALSE".
    boolean_type get_boolean() const
    {
        if (m_last_token_type == lexical_token_t::BOOLEAN_VALUE)
        {
            return m_boolean_val;
        }
        emit_error("Invalid request for a boolean value.");
    }

    /// @brief Convert from string to integer and get the converted value.
    /// @return integer_type An integer value converted from the source string.
    integer_type get_integer() const
    {
        if (m_last_token_type == lexical_token_t::INTEGER_VALUE)
        {
            return m_integer_val;
        }
        emit_error("Invalid request for an integer value.");
    }

    /// @brief Convert from string to float number and get the converted value.
    /// @return float_number_type A float number value converted from the source string.
    float_number_type get_float_number() const
    {
        if (m_last_token_type == lexical_token_t::FLOAT_NUMBER_VALUE)
        {
            return m_float_val;
        }
        emit_error("Invalid request for a float number value.");
    }

    /// @brief Get a scanned string value.
    /// @return const string_type& Constant reference to a scanned string.
    const string_type& get_string() const noexcept
    {
        // TODO: Provide support for different string types between nodes & inputs.
        static_assert(std::is_same<string_type, std::string>::value, "Unsupported, different string types.");
        return m_value_buffer;
    }

    /// @brief Get the YAML version specification.
    /// @return const string_type& A YAML version specification.
    const string_type& get_yaml_version() const
    {
        FK_YAML_ASSERT(!m_value_buffer.empty() && m_value_buffer.size() == 3);
        FK_YAML_ASSERT(m_value_buffer == "1.1" || m_value_buffer == "1.2");

        return m_value_buffer;
    }

private:
    /// @brief A utility function to convert a hexadecimal character to an integer.
    /// @param source A hexadecimal character ('0'~'9', 'A'~'F', 'a'~'f')
    /// @return char A integer converted from @a source.
    char convert_hex_char_to_byte(int source) const
    {
        if ('0' <= source && source <= '9')
        {
            // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
            return static_cast<char>(source - '0');
        }

        if ('A' <= source && source <= 'F')
        {
            // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
            return static_cast<char>(source - 'A' + 10);
        }

        if ('a' <= source && source <= 'f')
        {
            // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
            return static_cast<char>(source - 'a' + 10);
        }

        emit_error("Non-hexadecimal character has been given.");
    }

    /// @brief Skip until a newline code or a null character is found.
    /// @return lexical_token_t The lexical token type for comments
    lexical_token_t scan_comment()
    {
        FK_YAML_ASSERT(m_input_handler.get_current() == '#');

        skip_until_line_end();
        return lexical_token_t::COMMENT_PREFIX;
    }

    /// @brief Scan directives starting with the prefix '%'
    /// @note Currently, only %YAML directive is supported. If not, returns invalid or throws an exception.
    /// @return lexical_token_t The lexical token type for directives.
    lexical_token_t scan_directive()
    {
        FK_YAML_ASSERT(m_input_handler.get_current() == '%');

        switch (m_input_handler.get_next())
        {
        case s_end_of_input:
            emit_error("invalid eof in a directive.");
        case 'T': {
            if (m_input_handler.get_next() != 'A' || m_input_handler.get_next() != 'G')
            {
                skip_until_line_end();
                return lexical_token_t::INVALID_DIRECTIVE;
            }
            if (m_input_handler.get_next() != ' ')
            {
                emit_error("There must be a half-width space between \"%TAG\" and tag info.");
            }
            // TODO: parse tag directives' information
            return lexical_token_t::TAG_DIRECTIVE;
        }
        case 'Y':
            if (m_input_handler.get_next() != 'A' || m_input_handler.get_next() != 'M' ||
                m_input_handler.get_next() != 'L')
            {
                skip_until_line_end();
                return lexical_token_t::INVALID_DIRECTIVE;
            }
            if (m_input_handler.get_next() != ' ')
            {
                emit_error("There must be a half-width space between \"%YAML\" and a version number.");
            }
            return scan_yaml_version_directive();
        default:
            skip_until_line_end();
            return lexical_token_t::INVALID_DIRECTIVE;
        }
    }

    /// @brief Scan a YAML version directive.
    /// @note Only 1.1 and 1.2 are supported. If not, throws an exception.
    /// @return lexical_token_t The lexical token type for YAML version directives.
    lexical_token_t scan_yaml_version_directive()
    {
        m_value_buffer.clear();

        if (m_input_handler.get_next() != '1')
        {
            emit_error("Invalid YAML major version found.");
        }
        m_value_buffer.push_back(char_traits_type::to_char_type(m_input_handler.get_current()));

        if (m_input_handler.get_next() != '.')
        {
            emit_error("A period must be followed after the YAML major version.");
        }
        m_value_buffer.push_back(char_traits_type::to_char_type(m_input_handler.get_current()));

        switch (m_input_handler.get_next())
        {
        case '1':
        case '2':
            m_value_buffer.push_back(char_traits_type::to_char_type(m_input_handler.get_current()));
            break;
        case '0':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            emit_error("Unsupported YAML version.");
        default:
            emit_error("YAML version must be specified with digits and periods.");
        }

        if (m_input_handler.get_next() != ' ' && m_input_handler.get_current() != '\r' &&
            m_input_handler.get_current() != '\n')
        {
            emit_error("Only YAML version 1.1/1.2 are supported.");
        }

        return lexical_token_t::YAML_VER_DIRECTIVE;
    }

    /// @brief Scan and determine a number type(integer/float). This method is the entrypoint for all number
    /// tokens.
    /// @return lexical_token_t A lexical token type for a determined number type.
    lexical_token_t scan_number()
    {
        m_value_buffer.clear();

        int current = m_input_handler.get_current();
        FK_YAML_ASSERT(std::isdigit(current) || current == '-' || current == '+');

        lexical_token_t ret = lexical_token_t::END_OF_BUFFER;
        switch (current)
        {
        case '-':
            m_value_buffer.push_back(char_traits_type::to_char_type(current));
            ret = scan_negative_number();
            break;
        case '+':
            ret = scan_decimal_number();
            break;
        case '0':
            m_value_buffer.push_back(char_traits_type::to_char_type(current));
            ret = scan_number_after_zero_at_first();
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            m_value_buffer.push_back(char_traits_type::to_char_type(current));
            ret = scan_decimal_number();
            break;
        default:                                                      // LCOV_EXCL_LINE
            emit_error("Invalid character found in a number token."); // LCOV_EXCL_LINE
        }

        switch (ret)
        {
        case lexical_token_t::INTEGER_VALUE:
            m_integer_val = from_string(m_value_buffer, type_tag<integer_type> {});
            break;
        case lexical_token_t::FLOAT_NUMBER_VALUE:
            m_float_val = from_string(m_value_buffer, type_tag<float_number_type> {});
            break;
        default:   // LCOV_EXCL_LINE
            break; // LCOV_EXCL_LINE
        }

        return ret;
    }

    /// @brief Scan a next character after the negative sign(-).
    /// @return lexical_token_t The lexical token type for either integer or float numbers.
    lexical_token_t scan_negative_number()
    {
        int next = m_input_handler.get_next();

        // The value of `next` must be guranteed to be a digit in the get_next_token() function.
        FK_YAML_ASSERT(std::isdigit(next));
        m_value_buffer.push_back(char_traits_type::to_char_type(next));
        return scan_decimal_number();
    }

    /// @brief Scan a next character after '0' at the beginning of a token.
    /// @return lexical_token_t The lexical token type for one of number types(integer/float).
    lexical_token_t scan_number_after_zero_at_first()
    {
        int next = m_input_handler.get_next();
        switch (next)
        {
        case '.':
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            return scan_decimal_number_after_decimal_point();
        case 'o':
            // Do not store 'o' since std::stoXXX does not support "0o" but "0" as the prefix for octal numbers.
            // YAML specifies octal values start with the prefix "0o".
            // See https://yaml.org/spec/1.2.2/#1032-tag-resolution for more details.
            return scan_octal_number();
        case 'x':
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            return scan_hexadecimal_number();
        default:
            return lexical_token_t::INTEGER_VALUE;
        }
    }

    /// @brief Scan a next character after a decimal point.
    /// @return lexical_token_t The lexical token type for float numbers.
    lexical_token_t scan_decimal_number_after_decimal_point()
    {
        int next = m_input_handler.get_next();

        if (std::isdigit(next))
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            lexical_token_t token = scan_decimal_number();
            return token == lexical_token_t::STRING_VALUE ? token : lexical_token_t::FLOAT_NUMBER_VALUE;
        }

        emit_error("Invalid character found after a decimal point."); // LCOV_EXCL_LINE
    }

    /// @brief Scan a next character after exponent(e/E).
    /// @return lexical_token_t The lexical token type for float numbers.
    lexical_token_t scan_decimal_number_after_exponent()
    {
        int next = m_input_handler.get_next();
        if (next == '+' || next == '-')
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            scan_decimal_number_after_sign();
        }
        else if (std::isdigit(next))
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            scan_decimal_number();
        }
        else
        {
            emit_error("unexpected character found after exponent.");
        }
        return lexical_token_t::FLOAT_NUMBER_VALUE;
    }

    /// @brief Scan a next character after a sign(+/-) after exponent(e/E).
    /// @return lexical_token_t The lexical token type for one of number types(integer/float)
    lexical_token_t scan_decimal_number_after_sign()
    {
        int next = m_input_handler.get_next();

        if (std::isdigit(next))
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            return scan_decimal_number();
        }

        emit_error("Non-numeric character found after a sign(+/-) after exponent(e/E)."); // LCOV_EXCL_LINE
    }

    /// @brief Scan a next character for decimal numbers.
    /// @return lexical_token_t The lexical token type for one of number types(integer/float)
    lexical_token_t scan_decimal_number()
    {
        int next = m_input_handler.get_next();

        if (std::isdigit(next))
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            return scan_decimal_number();
        }

        if (next == '.')
        {
            // NOLINTNEXTLINE(abseil-string-find-str-contains)
            if (m_value_buffer.find('.') != string_type::npos)
            {
                // This path is for strings like 1.2.3
                return scan_string(false);
            }
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            return scan_decimal_number_after_decimal_point();
        }

        if (next == 'e' || next == 'E')
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            return scan_decimal_number_after_exponent();
        }

        return lexical_token_t::INTEGER_VALUE;
    }

    /// @brief Scan a next character for octal numbers.
    /// @return lexical_token_t The lexical token type for integers.
    lexical_token_t scan_octal_number()
    {
        int next = m_input_handler.get_next();
        if ('0' <= next && next <= '7')
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            scan_octal_number();
        }
        return lexical_token_t::INTEGER_VALUE;
    }

    /// @brief Scan a next character for hexadecimal numbers.
    /// @return lexical_token_t The lexical token type for integers.
    lexical_token_t scan_hexadecimal_number()
    {
        int next = m_input_handler.get_next();
        if (std::isxdigit(next))
        {
            m_value_buffer.push_back(char_traits_type::to_char_type(next));
            scan_hexadecimal_number();
        }
        return lexical_token_t::INTEGER_VALUE;
    }

    /// @brief Scan a string token(unquoted/single-quoted/double-quoted).
    /// @return lexical_token_t The lexical token type for strings.
    lexical_token_t scan_string(bool needs_clear = true)
    {
        bool needs_last_single_quote = false;
        bool needs_last_double_quote = false;

        if (needs_clear)
        {
            m_value_buffer.clear();

            needs_last_single_quote = (m_input_handler.get_current() == '\'');
            needs_last_double_quote = (m_input_handler.get_current() == '\"');
            if (needs_last_double_quote || needs_last_single_quote)
            {
                m_input_handler.get_next();
            }
        }

        lexical_token_t type = extract_string_token(needs_last_single_quote, needs_last_double_quote);
        FK_YAML_ASSERT(type == lexical_token_t::STRING_VALUE);

        if (needs_last_single_quote || needs_last_double_quote)
        {
            // just returned the extracted string value if quoted.
            return type;
        }

        if (m_value_buffer == "~")
        {
            return lexical_token_t::NULL_VALUE;
        }

        size_t val_size = m_value_buffer.size();
        if (val_size == 4)
        {
            if (m_value_buffer == "null" || m_value_buffer == "Null" || m_value_buffer == "NULL")
            {
                from_string(m_value_buffer, type_tag<std::nullptr_t> {});
                return lexical_token_t::NULL_VALUE;
            }

            if (m_value_buffer == "true" || m_value_buffer == "True" || m_value_buffer == "TRUE")
            {
                m_boolean_val = from_string(m_value_buffer, type_tag<boolean_type> {});
                return lexical_token_t::BOOLEAN_VALUE;
            }

            if (m_value_buffer == ".inf" || m_value_buffer == ".Inf" || m_value_buffer == ".INF" ||
                m_value_buffer == ".nan" || m_value_buffer == ".NaN" || m_value_buffer == ".NAN")
            {
                m_float_val = from_string(m_value_buffer, type_tag<float_number_type> {});
                return lexical_token_t::FLOAT_NUMBER_VALUE;
            }
        }
        else if (val_size == 5)
        {
            if (m_value_buffer == "false" || m_value_buffer == "False" || m_value_buffer == "FALSE")
            {
                m_boolean_val = from_string(m_value_buffer, type_tag<boolean_type> {});
                return lexical_token_t::BOOLEAN_VALUE;
            }

            if (m_value_buffer == "-.inf" || m_value_buffer == "-.Inf" || m_value_buffer == "-.INF")
            {
                m_float_val = from_string(m_value_buffer, type_tag<float_number_type> {});
                return lexical_token_t::FLOAT_NUMBER_VALUE;
            }
        }

        return type;
    }

    /// @brief Scan a string token(unquoted/single-quoted/double-quoted).
    /// @return lexical_token_t The lexical token type for strings.
    lexical_token_t extract_string_token(bool needs_last_single_quote, bool needs_last_double_quote)
    {
        int current = m_input_handler.get_current();

        for (;; current = m_input_handler.get_next())
        {
            // Handle the end of input buffer.
            if (current == s_end_of_input)
            {
                if (needs_last_double_quote)
                {
                    emit_error("Invalid end of input buffer in a double-quoted string token.");
                }

                if (needs_last_single_quote)
                {
                    emit_error("Invalid end of input buffer in a single-quoted string token.");
                }

                return lexical_token_t::STRING_VALUE;
            }

            if (current == ' ')
            {
                if (!needs_last_double_quote && !needs_last_single_quote)
                {
                    // Allow a space in an unquoted string only if the space is surrounded by non-space characters.
                    // See https://yaml.org/spec/1.2.2/#733-plain-style for more details.
                    current = m_input_handler.get_next();

                    // These characters are permitted when not inside a flow collection, and not inside an implicit key.
                    // TODO: Support detection of implicit key context for this check.
                    if (m_flow_context_depth > 0)
                    {
                        switch (current)
                        {
                        case '{':
                        case '}':
                        case '[':
                        case ']':
                        case ',':
                            return lexical_token_t::STRING_VALUE;
                        }
                    }

                    // " :" is permitted in a plain style string token, but not when followed by a space.
                    if (current == ':')
                    {
                        int next = m_input_handler.get_next();
                        m_input_handler.unget();
                        if (next == ' ')
                        {
                            return lexical_token_t::STRING_VALUE;
                        }
                    }

                    switch (current)
                    {
                    case ' ':
                    case '\r':
                    case '\n':
                    case '#':
                    case '\\':
                        return lexical_token_t::STRING_VALUE;
                    }
                    m_input_handler.unget();
                    current = m_input_handler.get_current();
                }
                m_value_buffer.push_back(char_traits_type::to_char_type(current));
                continue;
            }

            if (current == '\"')
            {
                if (needs_last_double_quote)
                {
                    m_input_handler.get_next();
                    return lexical_token_t::STRING_VALUE;
                }

                // if the target is a plain/single-quoted string token.
                m_value_buffer.push_back(char_traits_type::to_char_type(current));
                continue;
            }

            // Handle single quotation marks.
            if (current == '\'')
            {
                if (needs_last_single_quote)
                {
                    // If single quotation marks are repeated twice in a single-quoted string token, they are considered
                    // as an escaped single quotation mark.
                    current = m_input_handler.get_next();
                    if (current != '\'')
                    {
                        return lexical_token_t::STRING_VALUE;
                    }
                }

                m_value_buffer.push_back(char_traits_type::to_char_type(current));
                continue;
            }

            // Handle colons.
            if (current == ':')
            {
                // Just regard a colon as a character if surrounded by quotation marks.
                if (needs_last_double_quote || needs_last_single_quote)
                {
                    m_value_buffer.push_back(char_traits_type::to_char_type(current));
                    continue;
                }

                int next = m_input_handler.get_next();
                m_input_handler.unget();

                // A colon as a key separator must be followed by a space or a newline code.
                if (next != ' ' && next != '\r' && next != '\n')
                {
                    m_value_buffer.push_back(char_traits_type::to_char_type(current));
                    continue;
                }

                return lexical_token_t::STRING_VALUE;
            }

            // Handle flow indicators.
            {
                bool flow_indicator_appended = false;
                switch (current)
                {
                case '{':
                case '}':
                case '[':
                case ']':
                case ',':
                    // just regard the flow indicators as a normal character:
                    // - if single or double quotated,
                    // - or if not inside flow context.

                    if (needs_last_single_quote || needs_last_double_quote)
                    {
                        m_value_buffer.push_back(char_traits_type::to_char_type(current));
                    }
                    else if (m_flow_context_depth == 0)
                    {
                        m_value_buffer.push_back(char_traits_type::to_char_type(current));
                    }
                    else
                    {
                        return lexical_token_t::STRING_VALUE;
                    }

                    flow_indicator_appended = true;
                    break;
                default:
                    break;
                }

                if (flow_indicator_appended)
                {
                    continue;
                }
            }

            // Handle newline codes.
            if (current == '\r' || current == '\n')
            {
                if (!(needs_last_double_quote || needs_last_single_quote))
                {
                    return lexical_token_t::STRING_VALUE;
                }

                // TODO: Support multi-line string tokens.
                emit_error("multi-line string tokens are unsupported.");
            }

            // Handle escaped characters.
            // See https://yaml.org/spec/1.2.2/#57-escaped-characters for more details.
            if (current == '\\')
            {
                if (!needs_last_double_quote)
                {
                    emit_error("Escaped characters are only available in a double-quoted string token.");
                }

                current = m_input_handler.get_next();
                switch (current)
                {
                case 'a':
                    m_value_buffer.push_back('\a');
                    break;
                case 'b':
                    m_value_buffer.push_back('\b');
                    break;
                case 't':
                    m_value_buffer.push_back('\t');
                    break;
                case 'n':
                    m_value_buffer.push_back('\n');
                    break;
                case 'v':
                    m_value_buffer.push_back('\v');
                    break;
                case 'f':
                    m_value_buffer.push_back('\f');
                    break;
                case 'r':
                    m_value_buffer.push_back('\r');
                    break;
                case 'e':
                    m_value_buffer.push_back(char(0x1B));
                    break;
                case ' ':
                    m_value_buffer.push_back(' ');
                    break;
                case '\"':
                    m_value_buffer.push_back('\"');
                    break;
                case '/':
                    m_value_buffer.push_back('/');
                    break;
                case '\\':
                    m_value_buffer.push_back('\\');
                    break;
                case 'N': // next line
                    utf8_encoding::from_utf32(0x85u, m_encode_buffer, m_encoded_size);
                    for (size_t i = 0; i < m_encoded_size; i++)
                    {
                        m_value_buffer.push_back(m_encode_buffer[i]);
                    }
                    break;
                case '_': // non-breaking space
                    utf8_encoding::from_utf32(0xA0u, m_encode_buffer, m_encoded_size);
                    for (size_t i = 0; i < m_encoded_size; i++)
                    {
                        m_value_buffer.push_back(m_encode_buffer[i]);
                    }
                    break;
                case 'L': // line separator
                    utf8_encoding::from_utf32(0x2028u, m_encode_buffer, m_encoded_size);
                    for (size_t i = 0; i < m_encoded_size; i++)
                    {
                        m_value_buffer.push_back(m_encode_buffer[i]);
                    }
                    break;
                case 'P': // paragraph separator
                    utf8_encoding::from_utf32(0x2029u, m_encode_buffer, m_encoded_size);
                    for (size_t i = 0; i < m_encoded_size; i++)
                    {
                        m_value_buffer.push_back(m_encode_buffer[i]);
                    }
                    break;
                case 'x':
                    handle_escaped_unicode(1);
                    break;
                case 'u':
                    handle_escaped_unicode(2);
                    break;
                case 'U':
                    handle_escaped_unicode(4);
                    break;
                default:
                    emit_error("Unsupported escape sequence found in a string token.");
                }
                continue;
            }

            // Handle unescaped control characters.
            if (current <= 0x1F)
            {
                handle_unescaped_control_char(current);
                continue;
            }

            // Handle ASCII characters except control characters.
            if (current <= 0x7E)
            {
                m_value_buffer.push_back(char_traits_type::to_char_type(current));
                continue;
            }

            // Handle 2-byte characters encoded in UTF-8. (U+0080..U+07FF)
            if (current <= 0xDF)
            {
                std::array<int, 2> byte_array = {{current, m_input_handler.get_next()}};
                if (!utf8_encoding::validate(byte_array))
                {
                    throw fkyaml::invalid_encoding("ill-formed UTF-8 encoded character found", byte_array);
                }

                m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[0]));
                m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[1]));
                continue;
            }

            // Handle 3-byte characters encoded in UTF-8. (U+1000..U+D7FF,U+E000..U+FFFF)
            if (current <= 0xEF)
            {
                std::array<int, 3> byte_array = {{current, m_input_handler.get_next(), m_input_handler.get_next()}};
                if (!utf8_encoding::validate(byte_array))
                {
                    throw fkyaml::invalid_encoding("ill-formed UTF-8 encoded character found", byte_array);
                }

                m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[0]));
                m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[1]));
                m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[2]));

                continue;
            }

            // Handle 4-byte characters encoded in UTF-8. (U+10000..U+FFFFF,U+100000..U+10FFFF)
            std::array<int, 4> byte_array = {
                {current, m_input_handler.get_next(), m_input_handler.get_next(), m_input_handler.get_next()}};
            if (!utf8_encoding::validate(byte_array))
            {
                throw fkyaml::invalid_encoding("ill-formed UTF-8 encoded character found", byte_array);
            }

            m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[0]));
            m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[1]));
            m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[2]));
            m_value_buffer.push_back(char_traits_type::to_char_type(byte_array[3]));
        }
    }

    lexical_token_t scan_block_style_string_token(
        block_style_indicator_t style, chomping_indicator_t chomp, std::size_t indent)
    {
        m_value_buffer.clear();

        // Handle leading all-space lines.
        int current = m_input_handler.get_current();
        for (;; current = m_input_handler.get_next())
        {
            if (current == ' ')
            {
                continue;
            }

            if (current == '\r')
            {
                current = m_input_handler.get_next();
            }
            if (current == '\n')
            {
                m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                continue;
            }

            if (current == s_end_of_input)
            {
                if (chomp != chomping_indicator_t::KEEP)
                {
                    m_value_buffer.clear();
                }
                return lexical_token_t::STRING_VALUE;
            }

            break;
        }

        std::size_t cur_indent = m_input_handler.get_cur_pos_in_line();
        if (indent > 0 && cur_indent < indent)
        {
            emit_error("A block style scalar is less indented than the indicated level.");
        }

        // TODO: preserve and compare the last indentation with `cur_indent`
        if (indent == 0)
        {
            indent = cur_indent;
        }

        int chars_in_line = 0;
        bool is_extra_indented = false;
        if (cur_indent > indent)
        {
            std::size_t diff = cur_indent - indent;
            if (style == block_style_indicator_t::FOLDED)
            {
                m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                is_extra_indented = true;
            }
            while (diff > 0)
            {
                m_value_buffer.push_back(char_traits_type::to_char_type(' '));
                --diff;
                ++chars_in_line;
            }
        }

        for (; current != s_end_of_input; current = m_input_handler.get_next())
        {
            if (current == '\r')
            {
                // Ignore CR assuming the next character is LF.
                continue;
            }

            if (current == '\n')
            {
                if (style == block_style_indicator_t::LITERAL)
                {
                    m_value_buffer.push_back(char_traits_type::to_char_type(current));
                }
                else // block_style_indicator_t::FOLDED
                {
                    if (chars_in_line == 0)
                    {
                        // Just append a newline if the current line is empty.
                        m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                        is_extra_indented = false;
                        continue;
                    }

                    if (is_extra_indented)
                    {
                        // A line being more indented is not folded.
                        m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                        chars_in_line = 0;
                        is_extra_indented = false;
                        continue;
                    }

                    // Append a newline if the next line is an empty line.
                    bool is_end_of_token = false;
                    bool is_next_empty = false;
                    for (std::size_t i = 0; i < indent; i++)
                    {
                        current = m_input_handler.get_next();
                        if (current == ' ')
                        {
                            continue;
                        }

                        if (current == '\r')
                        {
                            current = m_input_handler.get_next();
                        }
                        if (current == '\n')
                        {
                            is_next_empty = true;
                            break;
                        }

                        is_end_of_token = true;
                        break;
                    }

                    if (is_end_of_token)
                    {
                        m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                        break;
                    }

                    if (is_next_empty)
                    {
                        m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                    }
                    else
                    {
                        switch (m_input_handler.get_next())
                        {
                        case '\r': {
                            m_input_handler.get_next();
                            FK_YAML_ASSERT(m_input_handler.get_current() == '\n');
                            m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                            break;
                        }
                        case '\n':
                            m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                            break;
                        case ' ':
                            // The next line is more indented, so a newline will be appended in the next loop.
                            m_input_handler.unget();
                            break;
                        default:
                            m_value_buffer.push_back(char_traits_type::to_char_type(' '));
                            m_input_handler.unget();
                            break;
                        }
                    }
                }

                // Reset the values for the next line.
                chars_in_line = 0;
                is_extra_indented = false;

                continue;
            }

            // Handle indentation
            cur_indent = m_input_handler.get_cur_pos_in_line();
            if (cur_indent < indent)
            {
                if (current != ' ')
                {
                    // Interpret less indented non-space characters as the start of next token.
                    break;
                }
                // skip a space if not yet indented enough
                continue;
            }

            if (style == block_style_indicator_t::FOLDED && chars_in_line == 0 && current == ' ')
            {
                // A line being more indented is not folded.
                m_value_buffer.push_back(char_traits_type::to_char_type('\n'));
                is_extra_indented = true;
            }
            m_value_buffer.push_back(char_traits_type::to_char_type(current));
            ++chars_in_line;
        }

        // Manipulate the trailing line endings chomping indicator type.
        switch (chomp)
        {
        case chomping_indicator_t::STRIP:
            while (true)
            {
                // Empty strings are handled above, so no check for the case.
                auto last_char = m_value_buffer.back();
                if (last_char != '\n')
                {
                    break;
                }
                m_value_buffer.pop_back();
            }
            break;
        case chomping_indicator_t::CLIP: {
            auto last_char = m_value_buffer.back();
            if (last_char != '\n')
            {
                // No need to chomp the trailing newlines.
                break;
            }
            while (true)
            {
                // Strings with only newlines are handled above, so no check for the case.
                auto second_last_char = m_value_buffer[m_value_buffer.size() - 2];
                if (second_last_char != '\n')
                {
                    break;
                }
                m_value_buffer.pop_back();
            }
            break;
        }
        case chomping_indicator_t::KEEP:
            break;
        }

        return lexical_token_t::STRING_VALUE;
    }

    /// @brief Handle unescaped control characters.
    /// @param c A target character.
    void handle_unescaped_control_char(int c)
    {
        FK_YAML_ASSERT(0x00 <= c && c <= 0x1F);

        switch (c)
        {
        // 0x00(NULL) has already been handled above.
        case 0x01:
            emit_error("Control character U+0001 (SOH) must be escaped to \\u0001.");
        case 0x02:
            emit_error("Control character U+0002 (STX) must be escaped to \\u0002.");
        case 0x03:
            emit_error("Control character U+0003 (ETX) must be escaped to \\u0003.");
        case 0x04:
            emit_error("Control character U+0004 (EOT) must be escaped to \\u0004.");
        case 0x05:
            emit_error("Control character U+0005 (ENQ) must be escaped to \\u0005.");
        case 0x06:
            emit_error("Control character U+0006 (ACK) must be escaped to \\u0006.");
        case 0x07:
            emit_error("Control character U+0007 (BEL) must be escaped to \\a or \\u0007.");
        case 0x08:
            emit_error("Control character U+0008 (BS) must be escaped to \\b or \\u0008.");
        case 0x09: // HT
            m_value_buffer.push_back(char_traits_type::to_char_type(c));
            break;
        // 0x0A(LF) has already been handled above.
        case 0x0B:
            emit_error("Control character U+000B (VT) must be escaped to \\v or \\u000B.");
        case 0x0C:
            emit_error("Control character U+000C (FF) must be escaped to \\f or \\u000C.");
        // 0x0D(CR) has already been handled above.
        case 0x0E:
            emit_error("Control character U+000E (SO) must be escaped to \\u000E.");
        case 0x0F:
            emit_error("Control character U+000F (SI) must be escaped to \\u000F.");
        case 0x10:
            emit_error("Control character U+0010 (DLE) must be escaped to \\u0010.");
        case 0x11:
            emit_error("Control character U+0011 (DC1) must be escaped to \\u0011.");
        case 0x12:
            emit_error("Control character U+0012 (DC2) must be escaped to \\u0012.");
        case 0x13:
            emit_error("Control character U+0013 (DC3) must be escaped to \\u0013.");
        case 0x14:
            emit_error("Control character U+0014 (DC4) must be escaped to \\u0014.");
        case 0x15:
            emit_error("Control character U+0015 (NAK) must be escaped to \\u0015.");
        case 0x16:
            emit_error("Control character U+0016 (SYN) must be escaped to \\u0016.");
        case 0x17:
            emit_error("Control character U+0017 (ETB) must be escaped to \\u0017.");
        case 0x18:
            emit_error("Control character U+0018 (CAN) must be escaped to \\u0018.");
        case 0x19:
            emit_error("Control character U+0019 (EM) must be escaped to \\u0019.");
        case 0x1A:
            emit_error("Control character U+001A (SUB) must be escaped to \\u001A.");
        case 0x1B:
            emit_error("Control character U+001B (ESC) must be escaped to \\e or \\u001B.");
        case 0x1C:
            emit_error("Control character U+001C (FS) must be escaped to \\u001C.");
        case 0x1D:
            emit_error("Control character U+001D (GS) must be escaped to \\u001D.");
        case 0x1E:
            emit_error("Control character U+001E (RS) must be escaped to \\u001E.");
        case 0x1F:
            emit_error("Control character U+001F (US) must be escaped to \\u001F.");
        }
    }

    void handle_escaped_unicode(int bytes_to_read)
    {
        int read_size = bytes_to_read * 2;
        char32_t code_point = 0;
        for (int i = read_size - 1; i >= 0; i--)
        {
            char four_bits = convert_hex_char_to_byte(m_input_handler.get_next());
            // NOLINTNEXTLINE(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
            code_point |= static_cast<char32_t>(four_bits << (4 * i));
        }

        // Treats the code point as a UTF-32 encoded character.
        utf8_encoding::from_utf32(code_point, m_encode_buffer, m_encoded_size);
        for (size_t i = 0; i < m_encoded_size; i++)
        {
            m_value_buffer.push_back(m_encode_buffer[i]);
        }
    }

    void get_block_style_metadata(chomping_indicator_t& chomp_type, std::size_t& indent)
    {
        int ch = m_input_handler.get_next();

        chomp_type = chomping_indicator_t::CLIP;
        if (ch == '-')
        {
            chomp_type = chomping_indicator_t::STRIP;
            ch = m_input_handler.get_next();
        }
        else if (ch == '+')
        {
            chomp_type = chomping_indicator_t::KEEP;
            ch = m_input_handler.get_next();
        }

        if (ch == '0')
        {
            emit_error("An indentation level for a block style scalar cannot be \'0\'");
        }

        indent = 0;
        if (std::isdigit(ch))
        {
            indent = convert_hex_char_to_byte(ch);
            ch = m_input_handler.get_next();
        }

        // skip characters including comments.
        skip_until_line_end();
    }

    /// @brief Skip white spaces (half-width spaces and tabs) from the current position.
    void skip_white_spaces()
    {
        do
        {
            switch (m_input_handler.get_current())
            {
            case ' ':
            case '\t':
                break;
            default:
                return;
            }
        } while (m_input_handler.get_next() != s_end_of_input);
    }

    /// @brief Skip white spaces and newline codes (CR/LF) from the current position.
    void skip_white_spaces_and_newline_codes()
    {
        do
        {
            skip_white_spaces();

            switch (m_input_handler.get_current())
            {
            case '\n':
            case '\r':
                break;
            default:
                return;
            }
        } while (m_input_handler.get_next() != s_end_of_input);
    }

    /// @brief Skip white spaces and comments from the current position.
    void skip_white_spaces_and_comments()
    {
        do
        {
            skip_white_spaces();

            switch (m_input_handler.get_current())
            {
            case '#': {
                scan_comment();
                break;
            }
            default:
                return;
            }
        } while (m_input_handler.get_next() != s_end_of_input);
    }

    /// @brief Skip the rest in the current line.
    void skip_until_line_end()
    {
        do
        {
            switch (m_input_handler.get_current())
            {
            case '\r':
                if (m_input_handler.get_next() == '\n')
                {
                    m_input_handler.get_next();
                }
                return;
            case '\n':
                m_input_handler.get_next();
                return;
            default:
                break;
            }
        } while (m_input_handler.get_next() != s_end_of_input);
    }

    [[noreturn]] void emit_error(const char* msg) const
    {
        throw fkyaml::parse_error(msg, m_input_handler.get_lines_read(), m_input_handler.get_cur_pos_in_line());
    }

private:
    /// The value of EOF for the target characters.
    static constexpr int s_end_of_input = char_traits_type::eof();

    /// An input buffer adapter to be analyzed.
    input_handler m_input_handler;
    /// A temporal buffer to store a string to be parsed to an actual datum.
    std::string m_value_buffer {};
    /// A temporal buffer to store a UTF-8 encoded char sequence.
    std::array<char, 4> m_encode_buffer {};
    /// The actual size of a UTF-8 encoded char sequence.
    std::size_t m_encoded_size {0};
    /// The beginning position of the last lexical token. (zero origin)
    std::size_t m_last_token_begin_pos {0};
    /// The beginning line of the last lexical token. (zero origin)
    std::size_t m_last_token_begin_line {0};
    /// The current depth of flow context.
    uint32_t m_flow_context_depth {0};
    /// The last found token type.
    lexical_token_t m_last_token_type {lexical_token_t::END_OF_BUFFER};
    /// A temporal bool holder.
    boolean_type m_boolean_val {false};
    /// A temporal integer holder.
    integer_type m_integer_val {0};
    /// A temporal floating point number holder.
    float_number_type m_float_val {0.0};
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_LEXICAL_ANALIZER_HPP_ */

// #include <fkYAML/detail/meta/input_adapter_traits.hpp>

// #include <fkYAML/detail/meta/node_traits.hpp>

// #include <fkYAML/detail/meta/stl_supplement.hpp>

// #include <fkYAML/detail/types/lexical_token_t.hpp>

// #include <fkYAML/detail/types/yaml_version_t.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_TYPES_YAML_VERSION_T_HPP_
#define FK_YAML_DETAIL_TYPES_YAML_VERSION_T_HPP_

#include <cstdint>

// #include <fkYAML/detail/macros/version_macros.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief Definition of YAML version types.
enum class yaml_version_t : std::uint32_t
{
    VER_1_1, //!< YAML version 1.1
    VER_1_2, //!< YAML version 1.2
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_TYPES_YAML_VERSION_T_HPP_ */

// #include <fkYAML/exception.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief A class which provides the feature of deserializing YAML documents.
/// @tparam BasicNodeType A type of the container for deserialized YAML values.
template <typename BasicNodeType>
class basic_deserializer
{
    static_assert(is_basic_node<BasicNodeType>::value, "basic_deserializer only accepts basic_node<...>");

    /** A type for sequence node value containers. */
    using sequence_type = typename BasicNodeType::sequence_type;
    /** A type for mapping node value containers. */
    using mapping_type = typename BasicNodeType::mapping_type;
    /** A type for boolean node values. */
    using boolean_type = typename BasicNodeType::boolean_type;
    /** A type for integer node values. */
    using integer_type = typename BasicNodeType::integer_type;
    /** A type for float number node values. */
    using float_number_type = typename BasicNodeType::float_number_type;
    /** A type for string node values. */
    using string_type = typename BasicNodeType::string_type;

public:
    /// @brief Construct a new basic_deserializer object.
    basic_deserializer() = default;

public:
    /// @brief Deserialize a YAML-formatted source string into a YAML node.
    /// @param source A YAML-formatted source string.
    /// @return BasicNodeType A root YAML node deserialized from the source string.
    template <typename InputAdapterType, enable_if_t<is_input_adapter<InputAdapterType>::value, int> = 0>
    BasicNodeType deserialize(InputAdapterType&& input_adapter)
    {
        lexical_analyzer<BasicNodeType> lexer(std::forward<InputAdapterType>(input_adapter));

        BasicNodeType root = BasicNodeType::mapping();
        m_current_node = &root;

        lexical_token_t type = lexer.get_next_token();
        std::size_t cur_indent = lexer.get_last_token_begin_pos();
        std::size_t cur_line = lexer.get_lines_processed();

        do
        {
            switch (type)
            {
            case lexical_token_t::END_OF_BUFFER:
                // This handles an empty input.
                break;
            case lexical_token_t::EXPLICIT_KEY_PREFIX: {
                bool needs_to_move_back = !m_indent_stack.empty() && cur_indent < m_indent_stack.back().first;
                if (needs_to_move_back)
                {
                    auto target_itr = std::find_if( // LCOV_EXCL_LINE
                        m_indent_stack.rbegin(),
                        m_indent_stack.rend(),
                        [cur_indent](std::pair<std::size_t, bool> p) { return cur_indent > p.first; });

                    auto pop_num = std::distance(m_indent_stack.rbegin(), target_itr);
                    for (auto i = 0; i < pop_num; i++)
                    {
                        // move back to the previous container node.
                        m_current_node = m_node_stack.back();
                        m_node_stack.pop_back();
                        m_indent_stack.pop_back();
                    }
                }

                if (m_current_node->is_null())
                {
                    *m_current_node = BasicNodeType::mapping();
                }

                m_node_stack.push_back(m_current_node);
                m_indent_stack.emplace_back(cur_indent, true);

                type = lexer.get_next_token();
                if (type == lexical_token_t::SEQUENCE_BLOCK_PREFIX)
                {
                    m_indent_stack.emplace_back(lexer.get_last_token_begin_pos(), false);
                    m_current_node = new BasicNodeType(node_t::SEQUENCE);
                    set_yaml_version(*m_current_node);
                    break;
                }

                m_current_node = new BasicNodeType();
                set_yaml_version(*m_current_node);
                cur_indent = lexer.get_last_token_begin_pos();
                cur_line = lexer.get_lines_processed();

                continue;
            }
            case lexical_token_t::KEY_SEPARATOR: {
                bool is_stack_empty = m_node_stack.empty();
                if (is_stack_empty)
                {
                    throw parse_error("A key separator found without key.", cur_line, cur_indent);
                }

                // hold the line count of the key separator for later use.
                std::size_t old_indent = cur_indent;
                std::size_t old_line = cur_line;

                type = lexer.get_next_token();
                if (type == lexical_token_t::COMMENT_PREFIX)
                {
                    // just skip the comment and get the next token.
                    type = lexer.get_next_token();
                }

                cur_indent = lexer.get_last_token_begin_pos();
                cur_line = lexer.get_lines_processed();

                bool is_implicit_same_line =
                    (cur_line == old_line) && (m_indent_stack.empty() || old_indent > m_indent_stack.back().first);
                if (is_implicit_same_line)
                {
                    // a key separator for an implicit key with its value on the same line.
                    continue;
                }

                if (cur_line > old_line)
                {
                    switch (type)
                    {
                    case lexical_token_t::SEQUENCE_BLOCK_PREFIX:
                        // a key separator preceeding block sequence entries
                        *m_current_node = BasicNodeType::sequence();
                        set_yaml_version(*m_current_node);
                        break;
                    case lexical_token_t::EXPLICIT_KEY_PREFIX:
                        // a key separator for a explicit block mapping key.
                        *m_current_node = BasicNodeType::mapping();
                        set_yaml_version(*m_current_node);
                        break;
                    // defer checking the existence of a key separator after the scalar until a deserialize_scalar()
                    // call.
                    case lexical_token_t::NULL_VALUE:
                    case lexical_token_t::BOOLEAN_VALUE:
                    case lexical_token_t::INTEGER_VALUE:
                    case lexical_token_t::FLOAT_NUMBER_VALUE:
                    case lexical_token_t::STRING_VALUE:
                    // defer handling these tokens until the next loop.
                    case lexical_token_t::MAPPING_FLOW_BEGIN:
                    case lexical_token_t::SEQUENCE_FLOW_BEGIN:
                        break;
                    default:   // LCOV_EXCL_LINE
                        break; // LCOV_EXCL_LINE
                    }

                    continue;
                }

                // handle explicit mapping key separators.

                while (!m_indent_stack.back().second)
                {
                    m_current_node = m_node_stack.back();
                    m_node_stack.pop_back();
                    m_indent_stack.pop_back();
                }

                if (m_node_stack.back()->is_sequence())
                {
                    m_current_node = m_node_stack.back();
                    m_node_stack.pop_back();
                }
                if (m_node_stack.back() == m_current_node)
                {
                    // This path is for nested explicit mapping keys like:
                    // ```yaml
                    // ? ? foo
                    //   : bar
                    // : baz
                    // ```
                    m_node_stack.pop_back();
                }

                BasicNodeType* key_node = m_current_node;
                m_node_stack.back()->template get_value_ref<mapping_type&>().emplace(*key_node, BasicNodeType());
                m_current_node = &(m_node_stack.back()->operator[](*key_node));
                delete key_node;
                key_node = nullptr;
                m_node_stack.push_back(m_node_stack.back());
                m_indent_stack.back().second = false;

                if (type == lexical_token_t::SEQUENCE_BLOCK_PREFIX)
                {
                    *m_current_node = BasicNodeType::sequence();
                    set_yaml_version(*m_current_node);
                }
                cur_indent = lexer.get_last_token_begin_pos();
                cur_line = lexer.get_lines_processed();
                continue;
            }
            case lexical_token_t::VALUE_SEPARATOR:
                break;
            case lexical_token_t::ANCHOR_PREFIX: {
                m_anchor_name = lexer.get_string();
                m_needs_anchor_impl = true;
                break;
            }
            case lexical_token_t::ALIAS_PREFIX: {
                const string_type& alias_name = lexer.get_string();
                auto itr = m_anchor_table.find(alias_name);
                if (itr == m_anchor_table.end())
                {
                    throw parse_error(
                        "The given anchor name must appear prior to the alias node.", cur_line, cur_indent);
                }
                assign_node_value(BasicNodeType::alias_of(m_anchor_table.at(alias_name)));
                break;
            }
            case lexical_token_t::COMMENT_PREFIX:
                break;
            case lexical_token_t::YAML_VER_DIRECTIVE: {
                FK_YAML_ASSERT(m_current_node == &root);
                update_yaml_version_from(lexer.get_yaml_version());
                set_yaml_version(*m_current_node);
                break;
            }
            case lexical_token_t::TAG_DIRECTIVE:
                // TODO: implement tag directive deserialization.
            case lexical_token_t::INVALID_DIRECTIVE:
                // TODO: should output a warning log. Currently just ignore this case.
                break;
            case lexical_token_t::SEQUENCE_BLOCK_PREFIX:
                if (m_current_node->is_sequence())
                {
                    bool is_empty = m_current_node->empty();
                    if (is_empty)
                    {
                        m_indent_stack.emplace_back(cur_indent, false);
                        break;
                    }

                    // move back to the previous sequence if necessary.
                    while (!m_current_node->is_sequence() || cur_indent != m_indent_stack.back().first)
                    {
                        m_current_node = m_node_stack.back();
                        m_node_stack.pop_back();
                        m_indent_stack.pop_back();
                    }
                    break;
                }

                // if the current node is a mapping.
                if (m_node_stack.empty())
                {
                    throw parse_error("Invalid sequence block prefix(- ) found.", cur_line, cur_indent);
                }

                // move back to the previous sequence if necessary.
                while (!m_current_node->is_sequence() || cur_indent != m_indent_stack.back().first)
                {
                    m_current_node = m_node_stack.back();
                    m_node_stack.pop_back();
                    m_indent_stack.pop_back();
                }

                // for mappings in a sequence.
                m_current_node->template get_value_ref<sequence_type&>().emplace_back(BasicNodeType::mapping());
                m_node_stack.push_back(m_current_node);
                m_current_node = &(m_current_node->template get_value_ref<sequence_type&>().back());
                set_yaml_version(*m_current_node);
                break;
            case lexical_token_t::SEQUENCE_FLOW_BEGIN:
                *m_current_node = BasicNodeType::sequence();
                set_yaml_version(*m_current_node);
                break;
            case lexical_token_t::SEQUENCE_FLOW_END:
                m_current_node = m_node_stack.back();
                m_node_stack.pop_back();
                break;
            case lexical_token_t::MAPPING_FLOW_BEGIN:
                *m_current_node = BasicNodeType::mapping();
                set_yaml_version(*m_current_node);
                break;
            case lexical_token_t::MAPPING_FLOW_END:
                m_current_node = m_node_stack.back();
                break;
            case lexical_token_t::NULL_VALUE: {
                bool do_continue =
                    deserialize_scalar(lexer, BasicNodeType(lexer.get_null()), cur_indent, cur_line, type);
                if (do_continue)
                {
                    continue;
                }
                break;
            }
            case lexical_token_t::BOOLEAN_VALUE: {
                bool do_continue =
                    deserialize_scalar(lexer, BasicNodeType(lexer.get_boolean()), cur_indent, cur_line, type);
                if (do_continue)
                {
                    continue;
                }
                break;
            }
            case lexical_token_t::INTEGER_VALUE: {
                bool do_continue =
                    deserialize_scalar(lexer, BasicNodeType(lexer.get_integer()), cur_indent, cur_line, type);
                if (do_continue)
                {
                    continue;
                }
                break;
            }
            case lexical_token_t::FLOAT_NUMBER_VALUE: {
                bool do_continue =
                    deserialize_scalar(lexer, BasicNodeType(lexer.get_float_number()), cur_indent, cur_line, type);
                if (do_continue)
                {
                    continue;
                }
                break;
            }
            case lexical_token_t::STRING_VALUE: {
                bool do_continue =
                    deserialize_scalar(lexer, BasicNodeType(lexer.get_string()), cur_indent, cur_line, type);
                if (do_continue)
                {
                    continue;
                }
                break;
            }
            case lexical_token_t::END_OF_DIRECTIVES:
                break;
            case lexical_token_t::END_OF_DOCUMENT:
                // TODO: This token should be handled to support multiple documents.
                break;
            }

            type = lexer.get_next_token();
            cur_indent = lexer.get_last_token_begin_pos();
            cur_line = lexer.get_lines_processed();
        } while (type != lexical_token_t::END_OF_BUFFER);

        m_current_node = nullptr;
        m_needs_anchor_impl = false;
        m_anchor_table.clear();
        m_node_stack.clear();
        m_indent_stack.clear();

        return root;
    }

private:
    /// @brief Add new key string to the current YAML node.
    /// @param key a key string to be added to the current YAML node.
    void add_new_key(const BasicNodeType& key, const std::size_t indent, const std::size_t line)
    {
        if (!m_indent_stack.empty() && indent < m_indent_stack.back().first)
        {
            auto target_itr =
                std::find_if(m_indent_stack.rbegin(), m_indent_stack.rend(), [indent](std::pair<std::size_t, bool> p) {
                    return indent == p.first;
                });
            bool is_indent_valid = (target_itr != m_indent_stack.rend());
            if (!is_indent_valid)
            {
                throw parse_error("Detected invalid indentaion.", line, indent);
            }

            auto pop_num = std::distance(m_indent_stack.rbegin(), target_itr);
            for (auto i = 0; i < pop_num; i++)
            {
                // move back to the previous container node.
                m_current_node = m_node_stack.back();
                m_node_stack.pop_back();
                m_indent_stack.pop_back();
            }
        }

        if (m_current_node->is_sequence())
        {
            m_current_node->template get_value_ref<sequence_type&>().emplace_back(BasicNodeType::mapping());
            m_node_stack.push_back(m_current_node);
            m_current_node = &(m_current_node->operator[](m_current_node->size() - 1));
        }

        mapping_type& map = m_current_node->template get_value_ref<mapping_type&>();
        bool is_empty = map.empty();
        if (is_empty)
        {
            m_indent_stack.emplace_back(indent, false);
        }
        else
        {
            // check key duplication in the current mapping if not empty.
            auto itr = map.find(key);
            if (itr != map.end())
            {
                throw parse_error("Detected duplication in mapping keys.", line, indent);
            }
        }

        map.emplace(key, BasicNodeType());
        m_node_stack.push_back(m_current_node);
        m_current_node = &(m_current_node->operator[](key));
    }

    /// @brief Assign node value to the current node.
    /// @param node_value A rvalue BasicNodeType object to be assigned to the current node.
    void assign_node_value(BasicNodeType&& node_value) noexcept
    {
        if (m_current_node->is_sequence())
        {
            m_current_node->template get_value_ref<sequence_type&>().emplace_back(std::move(node_value));
            set_yaml_version(m_current_node->template get_value_ref<sequence_type&>().back());
            if (m_needs_anchor_impl)
            {
                m_current_node->template get_value_ref<sequence_type&>().back().add_anchor_name(m_anchor_name);
                m_anchor_table[m_anchor_name] = m_current_node->template get_value_ref<sequence_type&>().back();
                m_needs_anchor_impl = false;
                m_anchor_name.clear();
            }
            return;
        }

        // a scalar node
        *m_current_node = std::move(node_value);
        set_yaml_version(*m_current_node);
        if (m_needs_anchor_impl)
        {
            m_current_node->add_anchor_name(m_anchor_name);
            m_anchor_table[m_anchor_name] = *m_current_node;
            m_needs_anchor_impl = false;
            m_anchor_name.clear();
        }
        if (!m_indent_stack.back().second)
        {
            m_current_node = m_node_stack.back();
            m_node_stack.pop_back();
        }
    }

    /// @brief Deserialize a detected scalar node.
    /// @param node A detected scalar node by a lexer.
    /// @param indent The current indentation width. Can be updated in this function.
    /// @param line The number of processed lines. Can be updated in this function.
    /// @return true if next token has already been got, false otherwise.
    template <typename LexerType>
    bool deserialize_scalar(
        LexerType& lexer, BasicNodeType&& node, std::size_t& indent, std::size_t& line, lexical_token_t& type)
    {
        if (m_current_node->is_mapping())
        {
            add_new_key(node, indent, line);
            return false;
        }

        type = lexer.get_next_token();
        if (type == lexical_token_t::KEY_SEPARATOR)
        {
            if (m_current_node->is_scalar())
            {
                if (line != lexer.get_lines_processed())
                {
                    // This path is for explicit mapping key separator(:)
                    assign_node_value(std::move(node));
                    if (!m_indent_stack.back().second)
                    {
                        m_indent_stack.pop_back();
                    }
                    indent = lexer.get_last_token_begin_pos();
                    line = lexer.get_lines_processed();
                    return true;
                }
                *m_current_node = BasicNodeType::mapping();
                set_yaml_version(*m_current_node);
            }
            add_new_key(node, indent, line);
        }
        else
        {
            assign_node_value(std::move(node));
        }
        indent = lexer.get_last_token_begin_pos();
        line = lexer.get_lines_processed();
        return true;
    }

    /// @brief Set the yaml_version_t object to the given node.
    /// @param node A BasicNodeType object to be set the yaml_version_t object.
    void set_yaml_version(BasicNodeType& node) noexcept
    {
        node.set_yaml_version(m_yaml_version);
    }

    /// @brief Update the target YAML version with an input string.
    /// @param version_str A YAML version string.
    void update_yaml_version_from(const string_type& version_str) noexcept
    {
        if (version_str == "1.1")
        {
            m_yaml_version = yaml_version_t::VER_1_1;
            return;
        }
        m_yaml_version = yaml_version_t::VER_1_2;
    }

private:
    /// The currently focused YAML node.
    BasicNodeType* m_current_node {nullptr};
    /// The stack of YAML nodes.
    std::vector<BasicNodeType*> m_node_stack {};
    /// The stack of indentation widths.
    std::vector<std::pair<std::size_t /*indent*/, bool /*is_explicit_key*/>> m_indent_stack {};
    /// The YAML version specification type.
    yaml_version_t m_yaml_version {yaml_version_t::VER_1_2};
    /// A flag to determine the need for YAML anchor node implementation.
    bool m_needs_anchor_impl {false};
    /// The last YAML anchor name.
    string_type m_anchor_name {};
    /// The table of YAML anchor nodes.
    std::unordered_map<std::string, BasicNodeType> m_anchor_table {};
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_DESERIALIZER_HPP_ */

// #include <fkYAML/detail/input/input_adapter.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_INPUT_INPUT_ADAPTER_HPP_
#define FK_YAML_DETAIL_INPUT_INPUT_ADAPTER_HPP_

#include <cstdio>
#include <cstring>
#include <istream>
#include <iterator>
#include <string>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/assert.hpp>

// #include <fkYAML/detail/encodings/encode_detector.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_ENCODINGS_ENCODE_DETECTOR_HPP_
#define FK_YAML_DETAIL_ENCODINGS_ENCODE_DETECTOR_HPP_

#include <cstdint>
#include <istream>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/encodings/utf_encode_t.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_ENCODINGS_UTF_ENCODE_T_HPP_
#define FK_YAML_DETAIL_ENCODINGS_UTF_ENCODE_T_HPP_

// #include <fkYAML/detail/macros/version_macros.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief Definition of Unicode encoding types
/// @note Since fkYAML doesn't treat UTF-16/UTF-32 encoded characters per byte, endians do not matter.
enum class utf_encode_t
{
    UTF_8,    //!< UTF-8
    UTF_16BE, //!< UTF-16 Big Endian
    UTF_16LE, //!< UTF-16 Little Endian
    UTF_32BE, //!< UTF-32 Big Endian
    UTF_32LE, //!< UTF-32 Little Endian
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ENCODINGS_UTF_ENCODE_T_HPP_ */

// #include <fkYAML/exception.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief Detect an encoding type for UTF-8 expected inputs.
/// @note This function doesn't support the case where the first character is null.
/// @param[in] bytes 4 bytes of an input character sequence.
/// @param[out] has_bom Whether or not the input contains a BOM.
/// @return A detected encoding type.
inline utf_encode_t detect_encoding_type(const std::array<uint8_t, 4>& bytes, bool& has_bom) noexcept
{
    has_bom = false;

    // Check if a BOM exists.

    if (bytes[0] == uint8_t(0xEFu) && bytes[1] == uint8_t(0xBBu) && bytes[2] == uint8_t(0xBFu))
    {
        has_bom = true;
        return utf_encode_t::UTF_8;
    }

    if (bytes[0] == 0 && bytes[1] == 0 && bytes[2] == uint8_t(0xFEu) && bytes[3] == uint8_t(0xFFu))
    {
        has_bom = true;
        return utf_encode_t::UTF_32BE;
    }

    if (bytes[0] == uint8_t(0xFFu) && bytes[1] == uint8_t(0xFEu) && bytes[2] == 0 && bytes[3] == 0)
    {
        has_bom = true;
        return utf_encode_t::UTF_32LE;
    }

    if (bytes[0] == uint8_t(0xFEu) && bytes[1] == uint8_t(0xFFu))
    {
        has_bom = true;
        return utf_encode_t::UTF_16BE;
    }

    if (bytes[0] == uint8_t(0xFFu) && bytes[1] == uint8_t(0xFEu))
    {
        has_bom = true;
        return utf_encode_t::UTF_16LE;
    }

    // Test the first character assuming it's an ASCII character.

    if (bytes[0] == 0 && bytes[1] == 0 && bytes[2] == 0 && 0 < bytes[3] && bytes[3] < uint8_t(0x80u))
    {
        return utf_encode_t::UTF_32BE;
    }

    if (0 < bytes[0] && bytes[0] < uint8_t(0x80u) && bytes[1] == 0 && bytes[2] == 0 && bytes[3] == 0)
    {
        return utf_encode_t::UTF_32LE;
    }

    if (bytes[0] == 0 && 0 < bytes[1] && bytes[1] < uint8_t(0x80u))
    {
        return utf_encode_t::UTF_16BE;
    }

    if (0 < bytes[0] && bytes[0] < uint8_t(0x80u) && bytes[1] == 0)
    {
        return utf_encode_t::UTF_16LE;
    }

    return utf_encode_t::UTF_8;
}

/// @brief Detects the encoding type of the input, and consumes a BOM if it exists.
/// @tparam ItrType Type of iterators for the input.
/// @tparam ElemSize The size of one input element.
/// @param begin The beginning of input iterators.
/// @param end The end of input iterators.
/// @return A detected encoding type.
template <typename ItrType, size_t ElemSize = sizeof(decltype(*(std::declval<ItrType>())))>
inline utf_encode_t detect_encoding_and_skip_bom(ItrType& begin, const ItrType& end)
{
    std::array<uint8_t, 4> bytes = {0xFFu, 0xFFu, 0xFFu, 0xFFu};
    switch (ElemSize)
    {
    case sizeof(char): { // this case covers char8_t as well when compiled with C++20 features.
        for (std::size_t i = 0; i < 4 && begin + i != end; i++)
        {
            bytes[i] = uint8_t(begin[i]);
        }

        bool has_bom = false;
        utf_encode_t encode_type = detect_encoding_type(bytes, has_bom);

        if (has_bom)
        {
            // skip reading the BOM.
            switch (encode_type)
            {
            case utf_encode_t::UTF_8:
                std::advance(begin, 3);
                break;
            case utf_encode_t::UTF_16BE:
            case utf_encode_t::UTF_16LE:
                std::advance(begin, 2);
                break;
            case utf_encode_t::UTF_32BE:
            case utf_encode_t::UTF_32LE:
                std::advance(begin, 4);
                break;
            }
        }

        return encode_type;
    }
    case sizeof(char16_t): {
        if (begin == end)
        {
            return utf_encode_t::UTF_16BE;
        }
        for (int i = 0; i < 2 && begin + i != end; i++)
        {
            bytes[i * 2] = uint8_t((begin[i] & 0xFF00u) >> 8);
            bytes[i * 2 + 1] = uint8_t(begin[i] & 0xFFu);
        }

        bool has_bom = false;
        utf_encode_t encode_type = detect_encoding_type(bytes, has_bom);

        if (encode_type != utf_encode_t::UTF_16BE && encode_type != utf_encode_t::UTF_16LE)
        {
            throw exception("char16_t characters must be encoded in the UTF-16 format.");
        }

        if (has_bom)
        {
            // skip reading the BOM.
            std::advance(begin, 1);
        }

        return encode_type;
    }
    case sizeof(char32_t): {
        if (begin == end)
        {
            return utf_encode_t::UTF_32BE;
        }

        bytes[0] = uint8_t((*begin & 0xFF000000u) >> 24);
        bytes[1] = uint8_t((*begin & 0x00FF0000u) >> 16);
        bytes[2] = uint8_t((*begin & 0x0000FF00u) >> 8);
        bytes[3] = uint8_t(*begin & 0x000000FFu);

        bool has_bom = false;
        utf_encode_t encode_type = detect_encoding_type(bytes, has_bom);

        if (encode_type != utf_encode_t::UTF_32BE && encode_type != utf_encode_t::UTF_32LE)
        {
            throw exception("char32_t characters must be encoded in the UTF-32 format.");
        }

        if (has_bom)
        {
            // skip reading the BOM.
            std::advance(begin, 1);
        }

        return encode_type;
    }
    default:
        throw exception("Unknown char size.");
    }
}

inline utf_encode_t detect_encoding_and_skip_bom(std::FILE* file) noexcept
{
    std::array<uint8_t, 4> bytes = {0xFFu, 0xFFu, 0xFFu, 0xFFu};
    for (std::size_t i = 0; i < 4; i++)
    {
        char byte = 0;
        std::size_t size = std::fread(&byte, sizeof(char), 1, file);
        if (size != sizeof(char))
        {
            break;
        }
        bytes[i] = uint8_t(byte & 0xFF);
    }

    bool has_bom = false;
    utf_encode_t encode_type = detect_encoding_type(bytes, has_bom);

    // move back to the beginning if a BOM doesn't exist.
    long offset = 0;
    if (has_bom)
    {
        switch (encode_type)
        {
        case utf_encode_t::UTF_8:
            offset = 3;
            break;
        case utf_encode_t::UTF_16BE:
        case utf_encode_t::UTF_16LE:
            offset = 2;
            break;
        case utf_encode_t::UTF_32BE:
        case utf_encode_t::UTF_32LE:
            offset = 4;
            break;
        }
    }
    fseek(file, offset, SEEK_SET);

    return encode_type;
}

inline utf_encode_t detect_encoding_and_skip_bom(std::istream& is) noexcept
{
    std::array<uint8_t, 4> bytes = {0xFFu, 0xFFu, 0xFFu, 0xFFu};
    for (std::size_t i = 0; i < 4; i++)
    {
        char ch = 0;
        is.read(&ch, 1);
        std::streamsize size = is.gcount();
        if (size != 1)
        {
            // without this, seekg() fails in the switch-case statement below.
            is.clear();
            break;
        }
        bytes[i] = uint8_t(ch & 0xFF);
    }

    bool has_bom = false;
    utf_encode_t encode_type = detect_encoding_type(bytes, has_bom);

    // move back to the beginning if a BOM doesn't exist.
    std::streamoff offset = 0;
    if (has_bom)
    {
        switch (encode_type)
        {
        case utf_encode_t::UTF_8:
            offset = 3;
            break;
        case utf_encode_t::UTF_16BE:
        case utf_encode_t::UTF_16LE:
            offset = 2;
            break;
        case utf_encode_t::UTF_32BE:
        case utf_encode_t::UTF_32LE:
            offset = 4;
            break;
        }
    }
    is.seekg(offset, std::ios_base::beg);

    return encode_type;
}

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ENCODINGS_ENCODE_DETECTOR_HPP_ */

// #include <fkYAML/detail/encodings/utf_encode_t.hpp>

// #include <fkYAML/detail/encodings/utf8_encoding.hpp>

// #include <fkYAML/detail/meta/stl_supplement.hpp>

// #include <fkYAML/exception.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

///////////////////////
//   input_adapter   //
///////////////////////

template <typename IterType, typename = void>
class iterator_input_adapter;

/// @brief An input adapter for iterators of type char.
/// @tparam IterType An iterator type.
template <typename IterType>
class iterator_input_adapter<
    IterType, enable_if_t<std::is_same<remove_cv_t<typename std::iterator_traits<IterType>::value_type>, char>::value>>
{
public:
    /// A type for characters used in this input adapter.
    using char_type = char;

    /// @brief Construct a new iterator_input_adapter object.
    iterator_input_adapter() = default;

    /// @brief Construct a new iterator_input_adapter object.
    /// @param begin The beginning of iteraters.
    /// @param end The end of iterators.
    /// @param encode_type The encoding type for this input adapter.
    iterator_input_adapter(IterType begin, IterType end, utf_encode_t encode_type) noexcept
        : m_current(begin),
          m_end(end),
          m_encode_type(encode_type)
    {
    }

    // allow only move construct/assignment like other input adapters.
    iterator_input_adapter(const iterator_input_adapter&) = delete;
    iterator_input_adapter(iterator_input_adapter&& rhs) = default;
    iterator_input_adapter& operator=(const iterator_input_adapter&) = delete;
    iterator_input_adapter& operator=(iterator_input_adapter&&) = default;
    ~iterator_input_adapter() = default;

    /// @brief Get a character at the current position and move forward.
    /// @return std::char_traits<char_type>::int_type A character or EOF.
    typename std::char_traits<char_type>::int_type get_character()
    {
        typename std::char_traits<char_type>::int_type ret = 0;
        switch (m_encode_type)
        {
        case utf_encode_t::UTF_8:
            ret = get_character_for_utf8();
            break;
        case utf_encode_t::UTF_16BE:
        case utf_encode_t::UTF_16LE:
            ret = get_character_for_utf16();
            break;
        case utf_encode_t::UTF_32BE:
        case utf_encode_t::UTF_32LE:
            ret = get_character_for_utf32();
            break;
        }
        return ret;
    }

private:
    /// @brief The concrete implementation of get_character() for UTF-8 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf8() noexcept
    {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_8);

        if (m_current != m_end)
        {
            auto ret = std::char_traits<char_type>::to_int_type(*m_current);
            ++m_current;
            return ret;
        }
        return std::char_traits<char_type>::eof();
    }

    /// @brief The concrete implementation of get_character() for UTF-16 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf16()
    {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_16BE || m_encode_type == utf_encode_t::UTF_16LE);

        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            if (m_current == m_end)
            {
                if (m_encoded_buf_size == 0)
                {
                    return std::char_traits<char_type>::eof();
                }
            }

            while (m_current != m_end && m_encoded_buf_size < 2)
            {
                if (m_encode_type == utf_encode_t::UTF_16BE)
                {
                    m_encoded_buffer[m_encoded_buf_size] = char16_t(uint8_t(*m_current) << 8);
                    ++m_current;
                    m_encoded_buffer[m_encoded_buf_size] |= char16_t(*m_current);
                }
                else // m_encode_type == utf_encode_t::UTF_16LE
                {
                    m_encoded_buffer[m_encoded_buf_size] = char16_t(*m_current);
                    ++m_current;
                    m_encoded_buffer[m_encoded_buf_size] |= char16_t(uint8_t(*m_current) << 8);
                }

                ++m_current;
                ++m_encoded_buf_size;
            }

            std::size_t consumed_size = 0;
            utf8_encoding::from_utf16(m_encoded_buffer, m_utf8_buffer, consumed_size, m_utf8_buf_size);

            if (consumed_size == 1)
            {
                m_encoded_buffer[0] = m_encoded_buffer[1];
                m_encoded_buffer[1] = 0;
            }
            m_encoded_buf_size -= consumed_size;

            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

    /// @brief The concrete implementation of get_character() for UTF-32 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf32()
    {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_32BE || m_encode_type == utf_encode_t::UTF_32LE);

        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            if (m_current == m_end)
            {
                return std::char_traits<char_type>::eof();
            }

            char32_t utf32 = 0;
            if (m_encode_type == utf_encode_t::UTF_32BE)
            {
                utf32 = char32_t(*m_current << 24);
                ++m_current;
                utf32 |= char32_t(*m_current << 16);
                ++m_current;
                utf32 |= char32_t(*m_current << 8);
                ++m_current;
                utf32 |= char32_t(*m_current);
            }
            else // m_encode_type == utf_encode_t::UTF_32LE
            {
                utf32 = char32_t(*m_current);
                ++m_current;
                utf32 |= char32_t(*m_current << 8);
                ++m_current;
                utf32 |= char32_t(*m_current << 16);
                ++m_current;
                utf32 |= char32_t(*m_current << 24);
            }

            utf8_encoding::from_utf32(utf32, m_utf8_buffer, m_utf8_buf_size);
            ++m_current;
            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

private:
    /// The iterator at the current position.
    IterType m_current {};
    /// The iterator at the end of input.
    IterType m_end {};
    /// The encoding type for this input adapter.
    utf_encode_t m_encode_type {utf_encode_t::UTF_8};
    /// The buffer for decoding characters read from the input.
    std::array<char16_t, 2> m_encoded_buffer {{0, 0}};
    /// The number of elements in `m_encoded_buffer`.
    std::size_t m_encoded_buf_size {0};
    /// The buffer for UTF-8 encoded characters.
    std::array<char, 4> m_utf8_buffer {{0, 0, 0, 0}};
    /// The next index in `m_utf8_buffer` to read.
    std::size_t m_utf8_buf_index {0};
    /// The number of bytes in `m_utf8_buffer`.
    std::size_t m_utf8_buf_size {0};
};

#ifdef FK_YAML_HAS_CHAR8_T

/// @brief An input adapter for iterators of type char8_t.
/// @tparam IterType An iterator type.
template <typename IterType>
class iterator_input_adapter<
    IterType,
    enable_if_t<std::is_same<remove_cv_t<typename std::iterator_traits<IterType>::value_type>, char8_t>::value>>
{
public:
    /// A type for characters used in this input adapter.
    using char_type = char;

    /// @brief Construct a new iterator_input_adapter object.
    iterator_input_adapter() = default;

    /// @brief Construct a new iterator_input_adapter object.
    /// @param begin The beginning of iteraters.
    /// @param end The end of iterators.
    /// @param encode_type The encoding type for this input adapter.
    iterator_input_adapter(IterType begin, IterType end, utf_encode_t encode_type) noexcept
        : m_current(begin),
          m_end(end),
          m_encode_type(encode_type)
    {
        // char8_t characters must be encoded in the UTF-8 format.
        // See https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0482r6.html.
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_8);
    }

    // allow only move construct/assignment like other input adapters.
    iterator_input_adapter(const iterator_input_adapter&) = delete;
    iterator_input_adapter(iterator_input_adapter&& rhs) = default;
    iterator_input_adapter& operator=(const iterator_input_adapter&) = delete;
    iterator_input_adapter& operator=(iterator_input_adapter&&) = default;
    ~iterator_input_adapter() = default;

    /// @brief Get a character at the current position and move forward.
    /// @return std::char_traits<char_type>::int_type A character or EOF.
    typename std::char_traits<char_type>::int_type get_character()
    {
        if (m_current != m_end)
        {
            auto ret = std::char_traits<char_type>::to_int_type(char(*m_current));
            ++m_current;
            return ret;
        }
        return std::char_traits<char_type>::eof();
    }

private:
    /// The iterator at the current position.
    IterType m_current {};
    /// The iterator at the end of input.
    IterType m_end {};
    /// The encoding type for this input adapter.
    utf_encode_t m_encode_type {utf_encode_t::UTF_8};
};

#endif // defined(FK_YAML_HAS_CHAR8_T)

/// @brief An input adapter for iterators of type char16_t.
/// @tparam IterType An iterator type.
template <typename IterType>
class iterator_input_adapter<
    IterType,
    enable_if_t<std::is_same<remove_cv_t<typename std::iterator_traits<IterType>::value_type>, char16_t>::value>>
{
public:
    /// A type for characters used in this input adapter.
    using char_type = char;

    /// @brief Construct a new iterator_input_adapter object.
    iterator_input_adapter() = default;

    /// @brief Construct a new iterator_input_adapter object.
    /// @param begin The beginning of iteraters.
    /// @param end The end of iterators.
    /// @param encode_type The encoding type for this input adapter.
    iterator_input_adapter(IterType begin, IterType end, utf_encode_t encode_type) noexcept
        : m_current(begin),
          m_end(end),
          m_encode_type(encode_type)
    {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_16BE || m_encode_type == utf_encode_t::UTF_16LE);
    }

    // allow only move construct/assignment like other input adapters.
    iterator_input_adapter(const iterator_input_adapter&) = delete;
    iterator_input_adapter(iterator_input_adapter&& rhs) = default;
    iterator_input_adapter& operator=(const iterator_input_adapter&) = delete;
    iterator_input_adapter& operator=(iterator_input_adapter&&) = default;
    ~iterator_input_adapter() = default;

    /// @brief Get a character at the current position and move forward.
    /// @return std::char_traits<char_type>::int_type A character or EOF.
    typename std::char_traits<char_type>::int_type get_character()
    {
        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            if (m_current == m_end)
            {
                if (m_encoded_buf_size == 0)
                {
                    return std::char_traits<char_type>::eof();
                }
            }

            while (m_current != m_end && m_encoded_buf_size < 2)
            {
                if (m_encode_type == utf_encode_t::UTF_16BE)
                {
                    m_encoded_buffer[m_encoded_buf_size] = *m_current;
                }
                else // utf_encode_t::UTF_16LE
                {
                    char16_t tmp = *m_current;
                    m_encoded_buffer[m_encoded_buf_size] = char16_t((tmp & 0x00FFu) << 8);
                    m_encoded_buffer[m_encoded_buf_size] |= char16_t((tmp & 0xFF00u) >> 8);
                }

                ++m_current;
                ++m_encoded_buf_size;
            }

            std::size_t consumed_size = 0;
            utf8_encoding::from_utf16(m_encoded_buffer, m_utf8_buffer, consumed_size, m_utf8_buf_size);

            if (consumed_size == 1)
            {
                m_encoded_buffer[0] = m_encoded_buffer[1];
                m_encoded_buffer[1] = 0;
            }
            m_encoded_buf_size -= consumed_size;

            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

private:
    /// The iterator at the current position.
    IterType m_current {};
    /// The iterator at the end of input.
    IterType m_end {};
    /// The encoding type for this input adapter.
    utf_encode_t m_encode_type {utf_encode_t::UTF_16BE};
    /// The buffer for decoding characters read from the input.
    std::array<char16_t, 2> m_encoded_buffer {{0, 0}};
    /// The number of elements in `m_encoded_buffer`.
    std::size_t m_encoded_buf_size {0};
    /// The buffer for UTF-8 encoded characters.
    std::array<char, 4> m_utf8_buffer {{0, 0, 0, 0}};
    /// The next index in `m_utf8_buffer` to read.
    std::size_t m_utf8_buf_index {0};
    /// The number of bytes in `m_utf8_buffer`.
    std::size_t m_utf8_buf_size {0};
};

/// @brief An input adapter for iterators of type char32_t.
/// @tparam IterType An iterator type.
template <typename IterType>
class iterator_input_adapter<
    IterType,
    enable_if_t<std::is_same<remove_cv_t<typename std::iterator_traits<IterType>::value_type>, char32_t>::value>>
{
public:
    /// A type for characters used in this input adapter.
    using char_type = char;

    /// @brief Construct a new iterator_input_adapter object.
    iterator_input_adapter() = default;

    /// @brief Construct a new iterator_input_adapter object.
    /// @param begin The beginning of iteraters.
    /// @param end The end of iterators.
    /// @param encode_type The encoding type for this input adapter.
    iterator_input_adapter(IterType begin, IterType end, utf_encode_t encode_type) noexcept
        : m_current(begin),
          m_end(end),
          m_encode_type(encode_type)
    {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_32BE || m_encode_type == utf_encode_t::UTF_32LE);
    }

    // allow only move construct/assignment like other input adapters.
    iterator_input_adapter(const iterator_input_adapter&) = delete;
    iterator_input_adapter(iterator_input_adapter&& rhs) = default;
    iterator_input_adapter& operator=(const iterator_input_adapter&) = delete;
    iterator_input_adapter& operator=(iterator_input_adapter&&) = default;
    ~iterator_input_adapter() = default;

    /// @brief Get a character at the current position and move forward.
    /// @return std::char_traits<char_type>::int_type A character or EOF.
    typename std::char_traits<char_type>::int_type get_character()
    {
        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            if (m_current == m_end)
            {
                return std::char_traits<char_type>::eof();
            }

            char32_t utf32 = 0;
            if (m_encode_type == utf_encode_t::UTF_32BE)
            {
                utf32 = *m_current;
            }
            else // m_encode_type == utf_encode_t::UTF_32LE
            {
                char32_t tmp = *m_current;
                utf32 |= char32_t((tmp & 0xFF000000u) >> 24);
                utf32 |= char32_t((tmp & 0x00FF0000u) >> 8);
                utf32 |= char32_t((tmp & 0x0000FF00u) << 8);
                utf32 |= char32_t((tmp & 0x000000FFu) << 24);
            }

            utf8_encoding::from_utf32(utf32, m_utf8_buffer, m_utf8_buf_size);
            ++m_current;
            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

private:
    /// The iterator at the current position.
    IterType m_current {};
    /// The iterator at the end of input.
    IterType m_end {};
    /// The encoding type for this input adapter.
    utf_encode_t m_encode_type {utf_encode_t::UTF_32BE};
    /// The buffer for UTF-8 encoded characters.
    std::array<char, 4> m_utf8_buffer {{0, 0, 0, 0}};
    /// The next index in `m_utf8_buffer` to read.
    std::size_t m_utf8_buf_index {0};
    /// The number of bytes in `m_utf8_buffer`.
    std::size_t m_utf8_buf_size {0};
};

/// @brief An input adapter for C-style file handles.
class file_input_adapter
{
public:
    /// A type for characters used in this input adapter.
    using char_type = char;

    /// @brief Construct a new file_input_adapter object.
    file_input_adapter() = default;

    /// @brief Construct a new file_input_adapter object.
    /// @note
    /// This class doesn't call fopen() nor fclose().
    /// It's user's responsibility to call those functions.
    /// @param file A file handle for this adapter. (A non-null pointer is assumed.)
    /// @param encode_type The encoding type for this input adapter.
    explicit file_input_adapter(std::FILE* file, utf_encode_t encode_type) noexcept
        : m_file(file),
          m_encode_type(encode_type)
    {
    }

    // allow only move construct/assignment
    file_input_adapter(const file_input_adapter&) = delete;
    file_input_adapter(file_input_adapter&& rhs) = default;
    file_input_adapter& operator=(const file_input_adapter&) = delete;
    file_input_adapter& operator=(file_input_adapter&&) = default;
    ~file_input_adapter() = default;

    /// @brief Get a character at the current position and move forward.
    /// @return std::char_traits<char_type>::int_type A character or EOF.
    typename std::char_traits<char_type>::int_type get_character()
    {
        typename std::char_traits<char_type>::int_type ret = 0;
        switch (m_encode_type)
        {
        case utf_encode_t::UTF_8:
            ret = get_character_for_utf8();
            break;
        case utf_encode_t::UTF_16BE:
        case utf_encode_t::UTF_16LE:
            ret = get_character_for_utf16();
            break;
        case utf_encode_t::UTF_32BE:
        case utf_encode_t::UTF_32LE:
            ret = get_character_for_utf32();
            break;
        }
        return ret;
    }

private:
    /// @brief The concrete implementation of get_character() for UTF-8 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf8() noexcept
    {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_8);

        char ch = 0;
        size_t size = std::fread(&ch, sizeof(char), 1, m_file);
        if (size == 1)
        {
            return std::char_traits<char_type>::to_int_type(ch);
        }
        return std::char_traits<char_type>::eof();
    }

    /// @brief The concrete implementation of get_character() for UTF-16 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf16()
    {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_16BE || m_encode_type == utf_encode_t::UTF_16LE);

        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            char chars[2] = {0, 0};
            while (m_encoded_buf_size < 2 && std::fread(&chars[0], sizeof(char), 2, m_file) == 2)
            {
                if (m_encode_type == utf_encode_t::UTF_16BE)
                {
                    m_encoded_buffer[m_encoded_buf_size] = char16_t(uint8_t(chars[0]) << 8);
                    m_encoded_buffer[m_encoded_buf_size] |= char16_t(uint8_t(chars[1]));
                }
                else // m_encode_type == utf_encode_t::UTF_16LE
                {
                    m_encoded_buffer[m_encoded_buf_size] = char16_t(uint8_t(chars[0]));
                    m_encoded_buffer[m_encoded_buf_size] |= char16_t(uint8_t(chars[1]) << 8);
                }

                ++m_encoded_buf_size;
            }

            if (m_encoded_buf_size == 0)
            {
                return std::char_traits<char_type>::eof();
            }

            std::size_t consumed_size = 0;
            utf8_encoding::from_utf16(m_encoded_buffer, m_utf8_buffer, consumed_size, m_utf8_buf_size);

            if (consumed_size == 1)
            {
                m_encoded_buffer[0] = m_encoded_buffer[1];
                m_encoded_buffer[1] = 0;
            }
            m_encoded_buf_size -= consumed_size;

            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

    /// @brief The concrete implementation of get_character() for UTF-32 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf32()
    {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_32BE || m_encode_type == utf_encode_t::UTF_32LE);

        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            char chars[4] = {0, 0, 0, 0};
            std::size_t size = std::fread(&chars[0], sizeof(char), 4, m_file);
            if (size != 4)
            {
                return std::char_traits<char_type>::eof();
            }

            char32_t utf32 = 0;
            if (m_encode_type == utf_encode_t::UTF_32BE)
            {
                utf32 = char32_t(uint8_t(chars[0]) << 24);
                utf32 |= char32_t(uint8_t(chars[1]) << 16);
                utf32 |= char32_t(uint8_t(chars[2]) << 8);
                utf32 |= char32_t(uint8_t(chars[3]));
            }
            else // m_encode_type == utf_encode_t::UTF_32LE
            {
                utf32 = char32_t(uint8_t(chars[0]));
                utf32 |= char32_t(uint8_t(chars[1]) << 8);
                utf32 |= char32_t(uint8_t(chars[2]) << 16);
                utf32 |= char32_t(uint8_t(chars[3]) << 24);
            }

            utf8_encoding::from_utf32(utf32, m_utf8_buffer, m_utf8_buf_size);
            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

private:
    /// A pointer to the input file handle.
    std::FILE* m_file {nullptr};
    /// The encoding type for this input adapter.
    utf_encode_t m_encode_type {utf_encode_t::UTF_8};
    /// The buffer for decoding characters read from the input.
    std::array<char16_t, 2> m_encoded_buffer {{0, 0}};
    /// The number of elements in `m_encoded_buffer`.
    std::size_t m_encoded_buf_size {0};
    /// The buffer for UTF-8 encoded characters.
    std::array<char, 4> m_utf8_buffer {{0, 0, 0, 0}};
    /// The next index in `m_utf8_buffer` to read.
    std::size_t m_utf8_buf_index {0};
    /// The number of bytes in `m_utf8_buffer`.
    std::size_t m_utf8_buf_size {0};
};

/// @brief An input adapter for streams
class stream_input_adapter
{
public:
    /// A type for characters used in this input adapter.
    using char_type = char;

    /// @brief Construct a new stream_input_adapter object.
    stream_input_adapter() = default;

    /// @brief Construct a new stream_input_adapter object.
    /// @param is A reference to the target input stream.
    explicit stream_input_adapter(std::istream& is, utf_encode_t encode_type) noexcept
        : m_istream(&is),
          m_encode_type(encode_type)
    {
    }

    // allow only move construct/assignment
    stream_input_adapter(const stream_input_adapter&) = delete;
    stream_input_adapter& operator=(const stream_input_adapter&) = delete;
    stream_input_adapter(stream_input_adapter&&) = default;
    stream_input_adapter& operator=(stream_input_adapter&&) = default;
    ~stream_input_adapter() = default;

    /// @brief Get a character at the current position and move forward.
    /// @return std::char_traits<char_type>::int_type A character or EOF.
    typename std::char_traits<char_type>::int_type get_character()
    {
        typename std::char_traits<char_type>::int_type ret = 0;
        switch (m_encode_type)
        {
        case utf_encode_t::UTF_8:
            ret = get_character_for_utf8();
            break;
        case utf_encode_t::UTF_16BE:
        case utf_encode_t::UTF_16LE:
            ret = get_character_for_utf16();
            break;
        case utf_encode_t::UTF_32BE:
        case utf_encode_t::UTF_32LE:
            ret = get_character_for_utf32();
            break;
        }
        return ret;
    }

private:
    /// @brief The concrete implementation of get_character() for UTF-8 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf8() noexcept
    {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_8);
        return m_istream->get();
    }

    /// @brief The concrete implementation of get_character() for UTF-16 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf16()
    {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_16BE || m_encode_type == utf_encode_t::UTF_16LE);

        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            while (m_encoded_buf_size < 2)
            {
                char chars[2] = {0, 0};
                m_istream->read(&chars[0], 2);
                std::streamsize size = m_istream->gcount();
                if (size != 2)
                {
                    if (m_encoded_buf_size == 0)
                    {
                        return std::char_traits<char_type>::eof();
                    }
                    break;
                }

                if (m_encode_type == utf_encode_t::UTF_16BE)
                {
                    m_encoded_buffer[m_encoded_buf_size] = char16_t(uint8_t(chars[0]) << 8);
                    m_encoded_buffer[m_encoded_buf_size] |= char16_t(uint8_t(chars[1]));
                }
                else // m_encode_type == utf_encode_t::UTF_16LE
                {
                    m_encoded_buffer[m_encoded_buf_size] = char16_t(uint8_t(chars[0]));
                    m_encoded_buffer[m_encoded_buf_size] |= char16_t(uint8_t(chars[1]) << 8);
                }

                ++m_encoded_buf_size;
            };

            std::size_t consumed_size = 0;
            utf8_encoding::from_utf16(m_encoded_buffer, m_utf8_buffer, consumed_size, m_utf8_buf_size);

            if (consumed_size == 1)
            {
                m_encoded_buffer[0] = m_encoded_buffer[1];
                m_encoded_buffer[1] = 0;
            }
            m_encoded_buf_size -= consumed_size;

            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

    /// @brief The concrete implementation of get_character() for UTF-32 encoded inputs.
    /// @return A UTF-8 encoded byte at the current position, or EOF.
    typename std::char_traits<char_type>::int_type get_character_for_utf32()
    {
        FK_YAML_ASSERT(m_encode_type == utf_encode_t::UTF_32BE || m_encode_type == utf_encode_t::UTF_32LE);

        if (m_utf8_buf_index == m_utf8_buf_size)
        {
            char ch = 0;
            m_istream->read(&ch, 1);
            std::streamsize size = m_istream->gcount();
            if (size != 1)
            {
                return std::char_traits<char_type>::eof();
            }

            char32_t utf32 = 0;
            if (m_encode_type == utf_encode_t::UTF_32BE)
            {
                utf32 = char32_t(ch << 24);
                m_istream->read(&ch, 1);
                utf32 |= char32_t(ch << 16);
                m_istream->read(&ch, 1);
                utf32 |= char32_t(ch << 8);
                m_istream->read(&ch, 1);
                utf32 |= char32_t(ch);
            }
            else // m_encode_type == utf_encode_t::UTF_32LE
            {
                utf32 = char32_t(ch);
                m_istream->read(&ch, 1);
                utf32 |= char32_t(ch << 8);
                m_istream->read(&ch, 1);
                utf32 |= char32_t(ch << 16);
                m_istream->read(&ch, 1);
                utf32 |= char32_t(ch << 24);
            }

            utf8_encoding::from_utf32(utf32, m_utf8_buffer, m_utf8_buf_size);
            m_utf8_buf_index = 0;
        }

        auto ret = std::char_traits<char_type>::to_int_type(m_utf8_buffer[m_utf8_buf_index]);
        ++m_utf8_buf_index;
        return ret;
    }

private:
    /// A pointer to the input stream object.
    std::istream* m_istream {nullptr};
    /// The encoding type for this input adapter.
    utf_encode_t m_encode_type {utf_encode_t::UTF_8};
    /// The buffer for decoding characters read from the input.
    std::array<char16_t, 2> m_encoded_buffer {{0, 0}};
    /// The number of elements in `m_encoded_buffer`.
    std::size_t m_encoded_buf_size {0};
    /// The buffer for UTF-8 encoded characters.
    std::array<char, 4> m_utf8_buffer {{0, 0, 0, 0}};
    /// The next index in `m_utf8_buffer` to read.
    std::size_t m_utf8_buf_index {0};
    /// The number of bytes in `m_utf8_buffer`.
    std::size_t m_utf8_buf_size {0};
};

/////////////////////////////////
//   input_adapter providers   //
/////////////////////////////////

/// @brief A factory method for iterator_input_adapter objects with ieterator values.
/// @tparam ItrType An iterator type.
/// @param begin The beginning of iterators.
/// @param end The end of iterators.
/// @return iterator_input_adapter<ItrType> An iterator_input_adapter object for the target iterator type.
template <typename ItrType, size_t ElemSize = sizeof(decltype(*(std::declval<ItrType>())))>
inline iterator_input_adapter<ItrType> input_adapter(ItrType begin, ItrType end)
{
    utf_encode_t encode_type = detect_encoding_and_skip_bom(begin, end);
    return iterator_input_adapter<ItrType>(begin, end, encode_type);
}

/// @brief A factory method for iterator_input_adapter objects with C-style arrays.
/// @tparam T A type of arrayed objects.
/// @tparam N A size of an array.
/// @return decltype(input_adapter(array, array + N)) An iterator_input_adapter object for the target array.
template <typename T, std::size_t N>
inline auto input_adapter(T (&array)[N]) -> decltype(input_adapter(array, array + (N - 1)))
{
    return input_adapter(array, array + (N - 1));
}

/// @brief A namespace to implement container_input_adapter_factory for internal use.
namespace container_input_adapter_factory_impl
{

using std::begin;
using std::end;

/// @brief A factory of input adapters for containers.
/// @tparam ContainerType A container type.
/// @tparam typename N/A
template <typename ContainerType, typename = void>
struct container_input_adapter_factory
{
};

/// @brief A partial specialization of container_input_adapter_factory if begin()/end() are available for ContainerType.
/// @tparam ContainerType A container type.
template <typename ContainerType>
struct container_input_adapter_factory<
    ContainerType, void_t<decltype(begin(std::declval<ContainerType>()), end(std::declval<ContainerType>()))>>
{
    /// A type for resulting input adapter object.
    using adapter_type =
        decltype(input_adapter(begin(std::declval<ContainerType>()), end(std::declval<ContainerType>())));

    /// @brief A factory method of input adapter objects for the target container objects.
    /// @param container
    /// @return adapter_type
    static adapter_type create(const ContainerType& container)
    {
        return input_adapter(begin(container), end(container));
    }
};

} // namespace container_input_adapter_factory_impl

/// @brief A factory method for iterator_input_adapter objects with containers.
/// @tparam ContainerType A container type.
/// @param container A container object.
/// @return container_input_adapter_factory_impl::container_input_adapter_factory<ContainerType>::adapter_type
template <typename ContainerType>
inline typename container_input_adapter_factory_impl::container_input_adapter_factory<ContainerType>::adapter_type
input_adapter(ContainerType&& container)
{
    return container_input_adapter_factory_impl::container_input_adapter_factory<ContainerType>::create(container);
}

/// @brief A factory method for file_input_adapter objects with C-style file handles.
/// @param file A file handle.
/// @return file_input_adapter A file_input_adapter object.
inline file_input_adapter input_adapter(std::FILE* file)
{
    if (!file)
    {
        throw fkyaml::exception("Invalid FILE object pointer.");
    }
    utf_encode_t encode_type = detect_encoding_and_skip_bom(file);
    return file_input_adapter(file, encode_type);
}

/// @brief
/// @param stream
/// @return stream_input_adapter
inline stream_input_adapter input_adapter(std::istream& stream) noexcept
{
    utf_encode_t encode_type = detect_encoding_and_skip_bom(stream);
    return stream_input_adapter(stream, encode_type);
}

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_INPUT_ADAPTER_HPP_ */

// #include <fkYAML/detail/iterator.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_ITERATOR_HPP_
#define FK_YAML_DETAIL_ITERATOR_HPP_

#include <cstddef>
#include <iterator>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/meta/node_traits.hpp>

// #include <fkYAML/exception.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief A tag which tells Iterator will contain sequence value iterator.
struct sequence_iterator_tag
{
};

/// @brief A tag which tells Iterator will contain mapping value iterator.
struct mapping_iterator_tag
{
};

/// @brief The template definitions of type informations used in @ref Iterator class
/// @tparam ValueType The type of iterated elements.
template <typename ValueType>
struct iterator_traits
{
    /// A type of iterated elements.
    using value_type = ValueType;
    /// A type to represent difference between iterators.
    using difference_type = std::ptrdiff_t;
    /// A type to represent iterator sizes.
    using size_type = std::size_t;
    /// A type of an element pointer.
    using pointer = value_type*;
    /// A type of reference to an element.
    using reference = value_type&;
};

/// @brief A specialization of @ref iterator_traits for constant value types.
/// @tparam ValueType The type of iterated elements.
template <typename ValueType>
struct iterator_traits<const ValueType>
{
    /// A type of iterated elements.
    using value_type = ValueType;
    /// A type to represent difference between iterators.
    using difference_type = std::ptrdiff_t;
    /// A type to represent iterator sizes.
    using size_type = std::size_t;
    /// A type of a constant element pointer.
    using pointer = const value_type*;
    /// A type of constant reference to an element.
    using reference = const value_type&;
};

/// @brief Definitions of iterator types for iterators internally held.
enum class iterator_t
{
    SEQUENCE, //!< sequence iterator type.
    MAPPING,  //!< mapping iterator type.
};

/// @brief A class which holds iterators either of sequence or mapping type
/// @tparam ValueType The type of iterated elements.
template <typename ValueType>
class iterator
{
public:
    /// A type for iterator traits of instantiated @Iterator template class.
    using ItrTraitsType = iterator_traits<ValueType>;

    /// A type for iterator category tag.
    using iterator_category = std::bidirectional_iterator_tag;
    /// A type of iterated element.
    using value_type = typename ItrTraitsType::value_type;
    /// A type to represent differences between iterators.
    using difference_type = typename ItrTraitsType::difference_type;
    /// A type to represent container sizes.
    using size_type = typename ItrTraitsType::size_type;
    /// A type of an element pointer.
    using pointer = typename ItrTraitsType::pointer;
    /// A type of reference to an element.
    using reference = typename ItrTraitsType::reference;

private:
    /// A type of non-const version of iterated elements.
    using NonConstValueType = typename std::remove_const<ValueType>::type;

    static_assert(is_basic_node<NonConstValueType>::value, "Iterator only accepts basic_node<...>");

    /// @brief The actual storage for iterators internally held in @ref Iterator.
    struct iterator_holder
    {
        /// A sequence iterator object.
        typename NonConstValueType::sequence_type::iterator sequence_iterator {};
        /// A mapping iterator object.
        typename NonConstValueType::mapping_type::iterator mapping_iterator {};
    };

public:
    /// @brief Construct a new iterator object with sequence iterator object.
    /// @param[in] itr An sequence iterator object.
    iterator(sequence_iterator_tag /* unused */, const typename ValueType::sequence_type::iterator& itr) noexcept
        : m_inner_iterator_type(iterator_t::SEQUENCE)
    {
        m_iterator_holder.sequence_iterator = itr;
    }

    /// @brief Construct a new iterator object with mapping iterator object.
    /// @param[in] itr An mapping iterator object.
    iterator(mapping_iterator_tag /* unused */, const typename ValueType::mapping_type::iterator& itr) noexcept
        : m_inner_iterator_type(iterator_t::MAPPING)
    {
        m_iterator_holder.mapping_iterator = itr;
    }

    /// @brief Copy constructor of the iterator class.
    /// @param other An iterator object to be copied with.
    iterator(const iterator& other) noexcept
        : m_inner_iterator_type(other.m_inner_iterator_type)
    {
        switch (m_inner_iterator_type)
        {
        case iterator_t::SEQUENCE:
            m_iterator_holder.sequence_iterator = other.m_iterator_holder.sequence_iterator;
            break;
        case iterator_t::MAPPING:
            m_iterator_holder.mapping_iterator = other.m_iterator_holder.mapping_iterator;
            break;
        }
    }

    /// @brief Move constructor of the iterator class.
    /// @param other An iterator object to be moved from.
    iterator(iterator&& other) noexcept
        : m_inner_iterator_type(other.m_inner_iterator_type)
    {
        switch (m_inner_iterator_type)
        {
        case iterator_t::SEQUENCE:
            m_iterator_holder.sequence_iterator = std::move(other.m_iterator_holder.sequence_iterator);
            break;
        case iterator_t::MAPPING:
            m_iterator_holder.mapping_iterator = std::move(other.m_iterator_holder.mapping_iterator);
            break;
        }
    }

    /// @brief Destroys an iterator.
    ~iterator() = default;

public:
    /// @brief A copy assignment operator of the iterator class.
    /// @param rhs An iterator object to be copied with.
    /// @return iterator& Reference to this iterator object.
    iterator& operator=(const iterator& rhs) noexcept
    {
        if (&rhs == this)
        {
            return *this;
        }

        m_inner_iterator_type = rhs.m_inner_iterator_type;
        switch (m_inner_iterator_type)
        {
        case iterator_t::SEQUENCE:
            m_iterator_holder.sequence_iterator = rhs.m_iterator_holder.sequence_iterator;
            break;
        case iterator_t::MAPPING:
            m_iterator_holder.mapping_iterator = rhs.m_iterator_holder.mapping_iterator;
            break;
        }

        return *this;
    }

    /// @brief A move assignment operator of the iterator class.
    /// @param rhs An iterator object to be moved from.
    /// @return iterator& Reference to this iterator object.
    iterator& operator=(iterator&& rhs) noexcept
    {
        if (&rhs == this)
        {
            return *this;
        }

        m_inner_iterator_type = rhs.m_inner_iterator_type;
        switch (m_inner_iterator_type)
        {
        case iterator_t::SEQUENCE:
            m_iterator_holder.sequence_iterator = std::move(rhs.m_iterator_holder.sequence_iterator);
            break;
        case iterator_t::MAPPING:
            m_iterator_holder.mapping_iterator = std::move(rhs.m_iterator_holder.mapping_iterator);
            break;
        }

        return *this;
    }

    /// @brief An arrow operator of the iterator class.
    /// @return pointer A pointer to the BasicNodeType object internally referenced by the actual iterator object.
    pointer operator->() noexcept
    {
        if (m_inner_iterator_type == iterator_t::SEQUENCE)
        {
            return &(*(m_iterator_holder.sequence_iterator));
        }

        // m_inner_iterator_type == iterator_t::MAPPING:
        return &(m_iterator_holder.mapping_iterator->second);
    }

    /// @brief A dereference operator of the iterator class.
    /// @return reference Reference to the Node object internally referenced by the actual iterator object.
    reference operator*() noexcept
    {
        if (m_inner_iterator_type == iterator_t::SEQUENCE)
        {
            return *(m_iterator_holder.sequence_iterator);
        }

        // m_inner_iterator_type == iterator_t::MAPPING:
        return m_iterator_holder.mapping_iterator->second;
    }

    /// @brief A compound assignment operator by sum of the Iterator class.
    /// @param i The difference from this Iterator object with which it moves forward.
    /// @return Iterator& Reference to this Iterator object.
    iterator& operator+=(difference_type i) noexcept
    {
        switch (m_inner_iterator_type)
        {
        case iterator_t::SEQUENCE:
            std::advance(m_iterator_holder.sequence_iterator, i);
            break;
        case iterator_t::MAPPING:
            std::advance(m_iterator_holder.mapping_iterator, i);
            break;
        }
        return *this;
    }

    /// @brief A plus operator of the iterator class.
    /// @param i The difference from this iterator object.
    /// @return iterator An iterator object which has been added @a i.
    iterator operator+(difference_type i) const noexcept
    {
        auto result = *this;
        result += i;
        return result;
    }

    /// @brief An pre-increment operator of the iterator class.
    /// @return iterator& Reference to this iterator object.
    iterator& operator++() noexcept
    {
        switch (m_inner_iterator_type)
        {
        case iterator_t::SEQUENCE:
            std::advance(m_iterator_holder.sequence_iterator, 1);
            break;
        case iterator_t::MAPPING:
            std::advance(m_iterator_holder.mapping_iterator, 1);
            break;
        }
        return *this;
    }

    /// @brief A post-increment opretor of the iterator class.
    /// @return iterator An iterator object which has been incremented.
    iterator operator++(int) & noexcept
    {
        auto result = *this;
        ++(*this);
        return result;
    }

    /// @brief A compound assignment operator by difference of the iterator class.
    /// @param i The difference from this iterator object with which it moves backward.
    /// @return iterator& Reference to this iterator object.
    iterator& operator-=(difference_type i) noexcept
    {
        return operator+=(-i);
    }

    /// @brief A minus operator of the iterator class.
    /// @param i The difference from this iterator object.
    /// @return iterator An iterator object from which has been subtracted @ i.
    iterator operator-(difference_type i) noexcept
    {
        auto result = *this;
        result -= i;
        return result;
    }

    /// @brief A pre-decrement operator of the iterator class.
    /// @return iterator& Reference to this iterator object.
    iterator& operator--() noexcept
    {
        switch (m_inner_iterator_type)
        {
        case iterator_t::SEQUENCE:
            std::advance(m_iterator_holder.sequence_iterator, -1);
            break;
        case iterator_t::MAPPING:
            std::advance(m_iterator_holder.mapping_iterator, -1);
            break;
        }
        return *this;
    }

    /// @brief A post-decrement operator of the iterator class
    /// @return iterator An iterator object which has been decremented.
    iterator operator--(int) & noexcept
    {
        auto result = *this;
        --(*this);
        return result;
    }

    /// @brief An equal-to operator of the iterator class.
    /// @param rhs An iterator object to be compared with this iterator object.
    /// @return true  This iterator object is equal to the other.
    /// @return false This iterator object is not equal to the other.
    bool operator==(const iterator& rhs) const
    {
        if (m_inner_iterator_type != rhs.m_inner_iterator_type)
        {
            throw fkyaml::exception("Cannot compare iterators of different container types.");
        }

        if (m_inner_iterator_type == iterator_t::SEQUENCE)
        {
            return (m_iterator_holder.sequence_iterator == rhs.m_iterator_holder.sequence_iterator);
        }

        // m_inner_iterator_type == iterator_t::MAPPING
        return (m_iterator_holder.mapping_iterator == rhs.m_iterator_holder.mapping_iterator);
    }

    /// @brief An not-equal-to operator of the iterator class.
    /// @param rhs An iterator object to be compared with this iterator object.
    /// @return true  This iterator object is not equal to the other.
    /// @return false This iterator object is equal to the other.
    bool operator!=(const iterator& rhs) const
    {
        return !operator==(rhs);
    }

    /// @brief A less-than operator of the iterator class.
    /// @param rhs An iterator object to be compared with this iterator object.
    /// @return true  This iterator object is less than the other.
    /// @return false This iterator object is not less than the other.
    bool operator<(const iterator& rhs) const
    {
        if (m_inner_iterator_type != rhs.m_inner_iterator_type)
        {
            throw fkyaml::exception("Cannot compare iterators of different container types.");
        }

        if (m_inner_iterator_type == iterator_t::MAPPING)
        {
            throw fkyaml::exception("Cannot compare order of iterators of the mapping container type");
        }

        return (m_iterator_holder.sequence_iterator < rhs.m_iterator_holder.sequence_iterator);
    }

    ///  @brief A less-than-or-equal-to operator of the iterator class.
    ///  @param rhs An iterator object to be compared with this iterator object.
    ///  @return true  This iterator object is either less than or equal to the other.
    ///  @return false This iterator object is neither less than nor equal to the other.
    bool operator<=(const iterator& rhs) const
    {
        return !rhs.operator<(*this);
    }

    /// @brief A greater-than operator of the iterator class.
    /// @param rhs An iterator object to be compared with this iterator object.
    /// @return true  This iterator object is greater than the other.
    /// @return false This iterator object is not greater than the other.
    bool operator>(const iterator& rhs) const
    {
        return !operator<=(rhs);
    }

    /// @brief A greater-than-or-equal-to operator of the iterator class.
    /// @param rhs An iterator object to be compared with this iterator object.
    /// @return true  This iterator object is either greater than or equal to the other.
    /// @return false This iterator object is neither greater than nor equal to the other.
    bool operator>=(const iterator& rhs) const
    {
        return !operator<(rhs);
    }

public:
    /// @brief Get the type of the internal iterator implementation.
    /// @return iterator_t The type of the internal iterator implementation.
    iterator_t type() const noexcept
    {
        return m_inner_iterator_type;
    }

    /// @brief Get the key string of the YAML mapping node for the current iterator.
    /// @return const std::string& The key string of the YAML mapping node for the current iterator.
    const typename ValueType::mapping_type::key_type& key() const
    {
        if (m_inner_iterator_type == iterator_t::SEQUENCE)
        {
            throw fkyaml::exception("Cannot retrieve key from non-mapping iterators.");
        }

        return m_iterator_holder.mapping_iterator->first;
    }

    /// @brief Get the reference of the YAML node for the current iterator.
    /// @return reference A reference to the YAML node for the current iterator.
    reference value() noexcept
    {
        return operator*();
    }

private:
    /// A type of the internally-held iterator.
    iterator_t m_inner_iterator_type {iterator_t::SEQUENCE};
    /// A holder of actual iterators.
    mutable iterator_holder m_iterator_holder {};
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ITERATOR_HPP_ */

// #include <fkYAML/detail/meta/node_traits.hpp>

// #include <fkYAML/detail/meta/stl_supplement.hpp>

// #include <fkYAML/detail/meta/type_traits.hpp>

// #include <fkYAML/detail/node_property.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_NODE_PROPERTY_HPP_
#define FK_YAML_DETAIL_NODE_PROPERTY_HPP_

#include <string>

// #include <fkYAML/detail/macros/version_macros.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

enum class anchor_status_t
{
    NONE,
    ANCHOR,
    ALIAS,
};

struct node_property
{
    std::string tag {};
    anchor_status_t anchor_status {anchor_status_t::NONE};
    std::string anchor {};
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_NODE_PROPERTY_HPP_ */

// #include <fkYAML/detail/node_ref_storage.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_NODE_REF_STORAGE_HPP_
#define FK_YAML_DETAIL_NODE_REF_STORAGE_HPP_

#include <initializer_list>
#include <type_traits>
#include <utility>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/meta/node_traits.hpp>

// #include <fkYAML/detail/meta/stl_supplement.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief A temporal storage for basic_node class objects.
/// @note This class makes it easier to handle lvalue basic_node objects in basic_node ctor with std::initializer_list.
/// @tparam BasicNodeType A basic_node template instance type.
template <typename BasicNodeType>
class node_ref_storage
{
    static_assert(is_basic_node<BasicNodeType>::value, "node_ref_storage only accepts basic_node<...>");

    using node_type = BasicNodeType;

public:
    /// @brief Construct a new node ref storage object with an rvalue basic_node object.
    /// @param n An rvalue basic_node object.
    explicit node_ref_storage(node_type&& n) noexcept(std::is_nothrow_move_constructible<node_type>::value)
        : owned_value(std::move(n))
    {
    }

    /// @brief Construct a new node ref storage object with an lvalue basic_node object.
    /// @param n An lvalue basic_node object.
    explicit node_ref_storage(const node_type& n) noexcept
        : value_ref(&n)
    {
    }

    /// @brief Construct a new node ref storage object with a std::initializer_list object.
    /// @param init A std::initializer_list object.
    node_ref_storage(std::initializer_list<node_ref_storage> init)
        : owned_value(init)
    {
    }

    /// @brief Construct a new node ref storage object with variadic template arguments
    /// @tparam Args Types of arguments to construct a basic_node object.
    /// @param args Arguments to construct a basic_node object.
    template <typename... Args, enable_if_t<std::is_constructible<node_type, Args...>::value, int> = 0>
    node_ref_storage(Args&&... args)
        : owned_value(std::forward<Args>(args)...)
    {
    }

    // allow only move construct/assignment
    node_ref_storage(const node_ref_storage&) = delete;
    node_ref_storage(node_ref_storage&&) = default;
    node_ref_storage& operator=(const node_ref_storage&) = delete;
    node_ref_storage& operator=(node_ref_storage&&) = default;

public:
    /// @brief An arrow operator for node_ref_storage objects.
    /// @return const node_type* A constant pointer to a basic_node object.
    const node_type* operator->() const noexcept
    {
        return value_ref ? value_ref : &owned_value;
    }

    /// @brief Releases a basic_node object internally held.
    /// @return node_type A basic_node object internally held.
    node_type release() const noexcept
    {
        return value_ref ? *value_ref : std::move(owned_value);
    }

private:
    /// A storage for a basic_node object given with rvalue reference.
    mutable node_type owned_value = nullptr;
    /// A pointer to a basic_node object given with lvalue reference.
    const node_type* value_ref = nullptr;
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_NODE_REF_STORAGE_HPP_ */

// #include <fkYAML/detail/output/serializer.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP_
#define FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP_

#include <cmath>
#include <sstream>
#include <string>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/conversions/to_string.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef TO__string_HPP_
#define TO__string_HPP_

#include <cmath>
#include <limits>
#include <string>
#include <sstream>
#include <type_traits>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/meta/stl_supplement.hpp>

// #include <fkYAML/detail/meta/type_traits.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief Converts a ValueType object to a string YAML token.
/// @tparam ValueType A source value type.
/// @tparam CharType The type of characters for the conversion result.
/// @param s A resulting output string.
/// @param v A source value.
template <typename ValueType, typename CharType>
inline void to_string(ValueType v, std::basic_string<CharType>& s) noexcept;

/// @brief Specialization of to_string() for null values.
/// @param s A resulting string YAML token.
/// @param (unused) nullptr
template <>
inline void to_string(std::nullptr_t /*unused*/, std::string& s) noexcept
{
    s = "null";
}

/// @brief Specialization of to_string() for booleans.
/// @param s A resulting string YAML token.
/// @param b A boolean source value.
template <>
inline void to_string(bool b, std::string& s) noexcept
{
    s = b ? "true" : "false";
}

/// @brief Specialization of to_string() for integers.
/// @tparam IntegerType An integer type.
/// @param s A resulting string YAML token.
/// @param i An integer source value.
template <typename IntegerType>
inline enable_if_t<is_non_bool_integral<IntegerType>::value> to_string(IntegerType i, std::string& s) noexcept
{
    s = std::to_string(i);
}

/// @brief Specialization of to_string() for floating point numbers.
/// @tparam FloatType A floating point number type.
/// @param s A resulting string YAML token.
/// @param f A floating point number source value.
template <typename FloatType>
inline enable_if_t<std::is_floating_point<FloatType>::value> to_string(FloatType f, std::string& s) noexcept
{
    if (std::isnan(f))
    {
        s = ".nan";
        return;
    }

    if (std::isinf(f))
    {
        if (f == std::numeric_limits<FloatType>::infinity())
        {
            s = ".inf";
        }
        else
        {
            s = "-.inf";
        }
        return;
    }

    std::ostringstream oss;
    oss << f;
    s = oss.str();
}

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* TO__string_HPP_ */

// #include <fkYAML/detail/input/input_adapter.hpp>

// #include <fkYAML/detail/input/lexical_analyzer.hpp>

// #include <fkYAML/detail/meta/node_traits.hpp>

// #include <fkYAML/detail/types/node_t.hpp>

// #include <fkYAML/exception.hpp>


FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief A basic implementation of serialization feature for YAML nodes.
/// @tparam BasicNodeType A BasicNode template class instantiation.
template <typename BasicNodeType>
class basic_serializer
{
    static_assert(detail::is_basic_node<BasicNodeType>::value, "basic_serializer only accepts basic_node<...>");

public:
    /// @brief Construct a new basic_serializer object.
    basic_serializer() = default;

    /// @brief Serialize the given Node value.
    /// @param node A Node object to be serialized.
    /// @return std::string A serialization result of the given Node value.
    std::string serialize(const BasicNodeType& node)
    {
        std::string str {};
        serialize_node(node, 0, str);
        return str;
    } // LCOV_EXCL_LINE

private:
    /// @brief Recursively serialize each Node object.
    /// @param node A Node object to be serialized.
    /// @param cur_indent The current indent width
    /// @param str A string to hold serialization result.
    void serialize_node(const BasicNodeType& node, const uint32_t cur_indent, std::string& str)
    {
        switch (node.type())
        {
        case node_t::SEQUENCE:
            for (const auto& seq_item : node)
            {
                if (cur_indent > 0)
                {
                    insert_indentation(cur_indent, str);
                }
                str += "-";

                bool is_appended = try_append_alias(seq_item, true, str);
                if (is_appended)
                {
                    str += "\n";
                    continue;
                }

                try_append_anchor(seq_item, true, str);

                bool is_scalar = seq_item.is_scalar();
                if (is_scalar)
                {
                    str += " ";
                    serialize_node(seq_item, cur_indent, str);
                    str += "\n";
                }
                else
                {
                    str += "\n";
                    serialize_node(seq_item, cur_indent + 2, str);
                }
            }
            break;
        case node_t::MAPPING:
            for (auto itr = node.begin(); itr != node.end(); ++itr)
            {
                if (cur_indent > 0)
                {
                    insert_indentation(cur_indent, str);
                }

                bool is_appended = try_append_alias(itr.key(), false, str);
                if (!is_appended)
                {
                    is_appended = try_append_anchor(itr.key(), false, str);
                    if (is_appended)
                    {
                        str += " ";
                    }
                    serialize_node(itr.key(), cur_indent, str);
                }

                str += ":";

                is_appended = try_append_alias(*itr, true, str);
                if (is_appended)
                {
                    str += "\n";
                    continue;
                }

                try_append_anchor(*itr, true, str);

                bool is_scalar = itr->is_scalar();
                if (is_scalar)
                {
                    str += " ";
                    serialize_node(*itr, cur_indent, str);
                    str += "\n";
                }
                else
                {
                    str += "\n";
                    serialize_node(*itr, cur_indent + 2, str);
                }
            }
            break;
        case node_t::NULL_OBJECT:
            to_string(nullptr, m_tmp_str_buff);
            str += m_tmp_str_buff;
            break;
        case node_t::BOOLEAN:
            to_string(node.template get_value<typename BasicNodeType::boolean_type>(), m_tmp_str_buff);
            str += m_tmp_str_buff;
            break;
        case node_t::INTEGER:
            to_string(node.template get_value<typename BasicNodeType::integer_type>(), m_tmp_str_buff);
            str += m_tmp_str_buff;
            break;
        case node_t::FLOAT_NUMBER:
            to_string(node.template get_value<typename BasicNodeType::float_number_type>(), m_tmp_str_buff);
            str += m_tmp_str_buff;
            break;
        case node_t::STRING: {
            using string_type = typename BasicNodeType::string_type;

            // Check if the string value contains a character needed to be escaped on output.
            const string_type& s = node.template get_value_ref<const string_type&>();
            string_type escaped;
            bool has_escape = false;
            size_t size = s.size();
            for (size_t i = 0; i < size; i++)
            {
                switch (s[i])
                {
                case 0x01:
                    escaped += "\\u0001";
                    has_escape = true;
                    break;
                case 0x02:
                    escaped += "\\u0002";
                    has_escape = true;
                    break;
                case 0x03:
                    escaped += "\\u0003";
                    has_escape = true;
                    break;
                case 0x04:
                    escaped += "\\u0004";
                    has_escape = true;
                    break;
                case 0x05:
                    escaped += "\\u0005";
                    has_escape = true;
                    break;
                case 0x06:
                    escaped += "\\u0006";
                    has_escape = true;
                    break;
                case '\a':
                    escaped += "\\a";
                    has_escape = true;
                    break;
                case '\b':
                    escaped += "\\b";
                    has_escape = true;
                    break;
                case '\t':
                    escaped += "\\t";
                    has_escape = true;
                    break;
                case '\n':
                    escaped += "\\n";
                    has_escape = true;
                    break;
                case '\v':
                    escaped += "\\v";
                    has_escape = true;
                    break;
                case '\f':
                    escaped += "\\f";
                    has_escape = true;
                    break;
                case '\r':
                    escaped += "\\r";
                    has_escape = true;
                    break;
                case 0x0E:
                    escaped += "\\u000E";
                    has_escape = true;
                    break;
                case 0x0F:
                    escaped += "\\u000F";
                    has_escape = true;
                    break;
                case 0x10:
                    escaped += "\\u0010";
                    has_escape = true;
                    break;
                case 0x11:
                    escaped += "\\u0011";
                    has_escape = true;
                    break;
                case 0x12:
                    escaped += "\\u0012";
                    has_escape = true;
                    break;
                case 0x13:
                    escaped += "\\u0013";
                    has_escape = true;
                    break;
                case 0x14:
                    escaped += "\\u0014";
                    has_escape = true;
                    break;
                case 0x15:
                    escaped += "\\u0015";
                    has_escape = true;
                    break;
                case 0x16:
                    escaped += "\\u0016";
                    has_escape = true;
                    break;
                case 0x17:
                    escaped += "\\u0017";
                    has_escape = true;
                    break;
                case 0x18:
                    escaped += "\\u0018";
                    has_escape = true;
                    break;
                case 0x19:
                    escaped += "\\u0019";
                    has_escape = true;
                    break;
                case 0x1A:
                    escaped += "\\u001A";
                    has_escape = true;
                    break;
                case 0x1B:
                    escaped += "\\e";
                    has_escape = true;
                    break;
                case 0x1C:
                    escaped += "\\u001C";
                    has_escape = true;
                    break;
                case 0x1D:
                    escaped += "\\u001D";
                    has_escape = true;
                    break;
                case 0x1E:
                    escaped += "\\u001E";
                    has_escape = true;
                    break;
                case 0x1F:
                    escaped += "\\u001F";
                    has_escape = true;
                    break;
                case '\"':
                    escaped += "\\\"";
                    has_escape = true;
                    break;
                case '\\':
                    escaped += "\\\\";
                    has_escape = true;
                    break;
                default:
                    if (i + 1 < size && s[i] == char(0xC2u) && s[i + 1] == char(0x85u))
                    {
                        escaped += "\\N";
                        i++;
                        has_escape = true;
                        break;
                    }
                    if (i + 1 < size && s[i] == char(0xC2u) && s[i + 1] == char(0xA0u))
                    {
                        escaped += "\\_";
                        i++;
                        has_escape = true;
                        break;
                    }
                    if (i + 2 < size && s[i] == char(0xE2u) && s[i + 1] == char(0x80u) && s[i + 2] == char(0xA8u))
                    {
                        escaped += "\\L";
                        i += 2;
                        has_escape = true;
                        break;
                    }
                    if (i + 2 < size && s[i] == char(0xE2u) && s[i + 1] == char(0x80u) && s[i + 2] == char(0xA9u))
                    {
                        escaped += "\\P";
                        i += 2;
                        has_escape = true;
                        break;
                    }
                    escaped += s[i];
                    break;
                }
            }

            if (has_escape)
            {
                // There's no other token type with escapes than strings.
                // Also, escapes must be in double-quoted strings.
                str += '\"';
                str += escaped;
                str += '\"';
                break;
            }

            auto adapter = input_adapter(s);
            lexical_analyzer<BasicNodeType> lexer(std::move(adapter));
            lexical_token_t token_type = lexer.get_next_token();

            if (token_type != lexical_token_t::STRING_VALUE)
            {
                // Surround a string value with double quotes to keep semantic equality.
                // Without them, serialized values will become non-string. (e.g., "1" -> 1)
                str += '\"';
                str += s;
                str += '\"';
            }
            else
            {
                str += s;
            }
            break;
        }
        }
    }

    /// @brief Insert indentation to the serialization result.
    /// @param cur_indent The current indent width to be inserted.
    /// @param str A string to hold serialization result.
    void insert_indentation(const uint32_t cur_indent, std::string& str) const noexcept
    {
        str.append(cur_indent, ' ');
    }

    /// @brief Append an anchor property if it's available. Do nothing otherwise.
    /// @param node The target node which is possibly an anchor node.
    /// @param prepends_space Whether or not to prepend a space before an anchor property.
    /// @param str A string to hold serialization result.
    /// @return true if an anchor property has been appended, false otherwise.
    bool try_append_anchor(const BasicNodeType& node, bool prepends_space, std::string& str) const
    {
        if (node.is_anchor())
        {
            if (prepends_space)
            {
                str += " ";
            }
            str += "&" + node.get_anchor_name();
            return true;
        }
        return false;
    }

    /// @brief Append an alias property if it's available. Do nothing otherwise.
    /// @param node The target node which is possibly an alias node.
    /// @param prepends_space Whether or not to prepend a space before an alias property.
    /// @param str A string to hold serialization result.
    /// @return true if an alias property has been appended, false otherwise.
    bool try_append_alias(const BasicNodeType& node, bool prepends_space, std::string& str) const
    {
        if (node.is_alias())
        {
            if (prepends_space)
            {
                str += " ";
            }
            str += "*" + node.get_anchor_name();
            return true;
        }
        return false;
    }

private:
    /// A temporal buffer for conversion from a scalar to a string.
    std::string m_tmp_str_buff;
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP_ */

// #include <fkYAML/detail/types/node_t.hpp>

// #include <fkYAML/detail/types/yaml_version_t.hpp>

// #include <fkYAML/exception.hpp>

// #include <fkYAML/node_value_converter.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_NODE_VALUE_CONVERTER_HPP_
#define FK_YAML_NODE_VALUE_CONVERTER_HPP_

#include <utility>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/conversions/from_node.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_CONVERSIONS_FROM_NODE_HPP_
#define FK_YAML_DETAIL_CONVERSIONS_FROM_NODE_HPP_

#include <limits>
#include <map>
#include <utility>
#include <vector>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/meta/node_traits.hpp>

// #include <fkYAML/detail/meta/stl_supplement.hpp>

// #include <fkYAML/detail/meta/type_traits.hpp>

// #include <fkYAML/detail/types/node_t.hpp>

// #include <fkYAML/exception.hpp>


/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

///////////////////
//   from_node   //
///////////////////

/// @brief from_node function for BasicNodeType::sequence_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param s A sequence node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, typename BasicNodeType::sequence_type& s)
{
    if (!n.is_sequence())
    {
        throw type_error("The target node value type is not sequence type.", n.type());
    }
    s = n.template get_value_ref<const typename BasicNodeType::sequence_type&>();
}

/// @brief from_node function for objects of the std::vector of compatible types.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam CompatibleValueType A compatible type for BasicNodeType.
/// @param n A basic_node object.
/// @param s A vector of compatible type objects.
template <
    typename BasicNodeType, typename CompatibleValueType,
    enable_if_t<
        conjunction<
            is_basic_node<BasicNodeType>, negation<is_basic_node<CompatibleValueType>>,
            has_from_node<BasicNodeType, CompatibleValueType>,
            negation<std::is_same<std::vector<CompatibleValueType>, typename BasicNodeType::sequence_type>>>::value,
        int> = 0>
inline void from_node(const BasicNodeType& n, std::vector<CompatibleValueType>& s)
{
    if (!n.is_sequence())
    {
        throw type_error("The target node value is not sequence type.", n.type());
    }

    s.reserve(n.size());

    for (const auto& elem : n)
    {
        s.emplace_back(elem.template get_value<CompatibleValueType>());
    }
}

/// @brief from_node function for BasicNodeType::mapping_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param m A mapping node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, typename BasicNodeType::mapping_type& m)
{
    if (!n.is_mapping())
    {
        throw type_error("The target node value type is not mapping type.", n.type());
    }

    for (auto pair : n.template get_value_ref<const typename BasicNodeType::mapping_type&>())
    {
        m.emplace(pair.first, pair.second);
    }
}

template <
    typename BasicNodeType, typename CompatibleKeyType, typename CompatibleValueType, typename Compare,
    typename Allocator,
    enable_if_t<
        conjunction<
            is_basic_node<BasicNodeType>, negation<is_basic_node<CompatibleKeyType>>,
            negation<is_basic_node<CompatibleValueType>>, has_from_node<BasicNodeType, CompatibleKeyType>,
            has_from_node<BasicNodeType, CompatibleValueType>>::value,
        int> = 0>
inline void from_node(const BasicNodeType& n, std::map<CompatibleKeyType, CompatibleValueType, Compare, Allocator>& m)
{
    if (!n.is_mapping())
    {
        throw type_error("The target node value type is not mapping type.", n.type());
    }

    for (auto pair : n.template get_value_ref<const typename BasicNodeType::mapping_type&>())
    {
        m.emplace(
            pair.first.template get_value<CompatibleKeyType>(), pair.second.template get_value<CompatibleValueType>());
    }
}

/// @brief from_node function for null node values.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param null A null node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, std::nullptr_t& null)
{
    // to ensure the target node value type is null.
    if (!n.is_null())
    {
        throw type_error("The target node value type is not null type.", n.type());
    }
    null = nullptr;
}

/// @brief from_node function for BasicNodeType::boolean_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param b A boolean node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, typename BasicNodeType::boolean_type& b)
{
    if (!n.is_boolean())
    {
        throw type_error("The target node value type is not boolean type.", n.type());
    }
    b = n.template get_value_ref<const typename BasicNodeType::boolean_type&>();
}

/// @brief from_node function for BasicNodeType::integer_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param i An integer node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, typename BasicNodeType::integer_type& i)
{
    if (!n.is_integer())
    {
        throw type_error("The target node value type is not integer type.", n.type());
    }
    i = n.template get_value_ref<const typename BasicNodeType::integer_type&>();
}

/// @brief from_node function for other integer objects. (i.e., not BasicNodeType::integer_type)
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam IntegerType An integer value type.
/// @param n A basic_node object.
/// @param i An integer node value object.
template <
    typename BasicNodeType, typename IntegerType,
    enable_if_t<
        conjunction<
            is_non_bool_integral<IntegerType>,
            negation<std::is_same<IntegerType, typename BasicNodeType::integer_type>>>::value,
        int> = 0>
inline void from_node(const BasicNodeType& n, IntegerType& i)
{
    if (!n.is_integer())
    {
        throw type_error("The target node value type is not integer type.", n.type());
    }

    // under/overflow check.
    using node_int_type = typename BasicNodeType::integer_type;
    node_int_type tmp_int = n.template get_value_ref<const node_int_type&>();
    if (tmp_int < static_cast<node_int_type>(std::numeric_limits<IntegerType>::min()))
    {
        throw exception("Integer value underflow detected.");
    }
    if (static_cast<node_int_type>(std::numeric_limits<IntegerType>::max()) < tmp_int)
    {
        throw exception("Integer value overflow detected.");
    }

    i = static_cast<IntegerType>(tmp_int);
}

/// @brief from_node function for BasicNodeType::float_number_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param f A float number node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, typename BasicNodeType::float_number_type& f)
{
    if (!n.is_float_number())
    {
        throw type_error("The target node value type is not float number type.", n.type());
    }
    f = n.template get_value_ref<const typename BasicNodeType::float_number_type&>();
}

/// @brief from_node function for other float number objects. (i.e., not BasicNodeType::float_number_type)
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam FloatType A float number value type.
/// @param n A basic_node object.
/// @param f A float number node value object.
template <
    typename BasicNodeType, typename FloatType,
    enable_if_t<
        conjunction<
            std::is_floating_point<FloatType>,
            negation<std::is_same<FloatType, typename BasicNodeType::float_number_type>>>::value,
        int> = 0>
inline void from_node(const BasicNodeType& n, FloatType& f)
{
    if (!n.is_float_number())
    {
        throw type_error("The target node value type is not float number type.", n.type());
    }

    auto tmp_float = n.template get_value_ref<const typename BasicNodeType::float_number_type&>();
    if (tmp_float < std::numeric_limits<FloatType>::min())
    {
        throw exception("Floating point value underflow detected.");
    }
    if (std::numeric_limits<FloatType>::max() < tmp_float)
    {
        throw exception("Floating point value overflow detected.");
    }

    f = static_cast<FloatType>(tmp_float);
}

/// @brief from_node function for BasicNodeType::string_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param s A string node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, typename BasicNodeType::string_type& s)
{
    if (!n.is_string())
    {
        throw type_error("The target node value type is not string type.", n.type());
    }
    s = n.template get_value_ref<const typename BasicNodeType::string_type&>();
}

/// @brief A function object to call from_node functions.
/// @note User-defined specialization is available by providing implementation **OUTSIDE** fkyaml namespace.
struct from_node_fn
{
    /// @brief Call from_node function suitable for the given T type.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @tparam T A target value type assigned from the basic_node object.
    /// @param n A basic_node object.
    /// @param val A target object assigned from the basic_node object.
    /// @return decltype(from_node(n, std::forward<T>(val))) void by default. User can set it to some other type.
    template <typename BasicNodeType, typename T>
    auto operator()(const BasicNodeType& n, T&& val) const noexcept(noexcept(from_node(n, std::forward<T>(val))))
        -> decltype(from_node(n, std::forward<T>(val)))
    {
        return from_node(n, std::forward<T>(val));
    }
};

} // namespace detail

#ifndef FK_YAML_HAS_CXX_17
// anonymous namespace to hold `from_node` functor.
// see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html for why it's needed.
namespace // NOLINT(cert-dcl59-cpp,fuchsia-header-anon-namespaces,google-build-namespaces)
{
#endif

/// @brief A blobal object to represent ADL friendly from_node functor.
// NOLINTNEXTLINE(misc-definitions-in-headers)
FK_YAML_INLINE_VAR constexpr const auto& from_node = detail::static_const<detail::from_node_fn>::value;

#ifndef FK_YAML_HAS_CXX_17
} // namespace
#endif

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_CONVERSIONS_FROM_NODE_HPP_ */

// #include <fkYAML/detail/conversions/to_node.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_CONVERSIONS_TO_NODE_HPP_
#define FK_YAML_DETAIL_CONVERSIONS_TO_NODE_HPP_

#include <utility>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/types/node_t.hpp>

// #include <fkYAML/detail/meta/node_traits.hpp>

// #include <fkYAML/detail/meta/type_traits.hpp>

// #include <fkYAML/detail/meta/stl_supplement.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

///////////////////////////////////
//   external_node_constructor   //
///////////////////////////////////

/// @brief The external constructor template for basic_node objects.
/// @note All the non-specialized instanciations results in compilation error since such instantiations are not
/// supported.
/// @warning All the specialization must call n.m_node_value.destroy(n.m_node_type) first in construct function to avoid
/// memory leak.
/// @tparam node_t The resulting YAMK node value type.
template <node_t>
struct external_node_constructor;

/// @brief The specialization of external_node_constructor for sequence nodes.
template <>
struct external_node_constructor<node_t::SEQUENCE>
{
    /// @brief Constructs a basic_node object with const lvalue sequence.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param s A lvalue sequence value.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, const typename BasicNodeType::sequence_type& s) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::SEQUENCE;
        n.m_node_value.p_sequence = BasicNodeType::template create_object<typename BasicNodeType::sequence_type>(s);
    }

    /// @brief Constructs a basic_node object with rvalue sequence.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param s A rvalue sequence value.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::sequence_type&& s) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::SEQUENCE;
        n.m_node_value.p_sequence =
            BasicNodeType::template create_object<typename BasicNodeType::sequence_type>(std::move(s));
    }
};

/// @brief The specialization of external_node_constructor for mapping nodes.
template <>
struct external_node_constructor<node_t::MAPPING>
{
    /// @brief Constructs a basic_node object with const lvalue mapping.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param m A lvalue mapping value.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, const typename BasicNodeType::mapping_type& m) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::MAPPING;
        n.m_node_value.p_mapping = BasicNodeType::template create_object<typename BasicNodeType::mapping_type>(m);
    }

    /// @brief Constructs a basic_node object with rvalue mapping.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param m A rvalue mapping value.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::mapping_type&& m) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::MAPPING;
        n.m_node_value.p_mapping =
            BasicNodeType::template create_object<typename BasicNodeType::mapping_type>(std::move(m));
    }
};

/// @brief The specialization of external_node_constructor for null nodes.
template <>
struct external_node_constructor<node_t::NULL_OBJECT>
{
    /// @brief Constructs a basic_node object with nullptr.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param (unused) nullptr
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, std::nullptr_t /*unused*/) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::NULL_OBJECT;
        n.m_node_value.p_mapping = nullptr;
    }
};

/// @brief The specialization of external_node_constructor for boolean scalar nodes.
template <>
struct external_node_constructor<node_t::BOOLEAN>
{
    /// @brief Constructs a basic_node object with boolean.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param b A boolean value.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::boolean_type b) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::BOOLEAN;
        n.m_node_value.boolean = b;
    }
};

/// @brief The specialization of external_node_constructor for integer scalar nodes.
template <>
struct external_node_constructor<node_t::INTEGER>
{
    /// @brief Constructs a basic_node object with integers.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param i An integer value.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::integer_type i) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::INTEGER;
        n.m_node_value.integer = i;
    }
};

/// @brief The specialization of external_node_constructor for float number scalar nodes.
template <>
struct external_node_constructor<node_t::FLOAT_NUMBER>
{
    /// @brief Constructs a basic_node object with floating point numbers.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param f A floating point number.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::float_number_type f) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::FLOAT_NUMBER;
        n.m_node_value.float_val = f;
    }
};

/// @brief The specialization of external_node_constructor for string scalar nodes.
template <>
struct external_node_constructor<node_t::STRING>
{
    /// @brief Constructs a basic_node object with const lvalue strings.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param s A constant lvalue string.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, const typename BasicNodeType::string_type& s) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::STRING;
        n.m_node_value.p_string = BasicNodeType::template create_object<typename BasicNodeType::string_type>(s);
    }

    /// @brief Constructs a basic_node object with rvalue strings.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param s A rvalue string.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::string_type&& s) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::STRING;
        n.m_node_value.p_string =
            BasicNodeType::template create_object<typename BasicNodeType::string_type>(std::move(s));
    }

    /// @brief Constructs a basic_node object with compatible strings.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @tparam CompatibleStringType A compatible string type.
    /// @param n A basic_node object.
    /// @param s A compatible string.
    template <
        typename BasicNodeType, typename CompatibleStringType,
        enable_if_t<
            conjunction<
                is_basic_node<BasicNodeType>,
                negation<std::is_same<typename BasicNodeType::string_type, CompatibleStringType>>>::value,
            int> = 0>
    static void construct(BasicNodeType& n, const CompatibleStringType& s) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::STRING;
        n.m_node_value.p_string = BasicNodeType::template create_object<typename BasicNodeType::string_type>(s);
    }
};

/////////////////
//   to_node   //
/////////////////

/// @brief to_node function for BasicNodeType::sequence_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A sequence node value type.
/// @param n A basic_node object.
/// @param s A sequence node value object.
template <
    typename BasicNodeType, typename T,
    enable_if_t<
        conjunction<
            is_basic_node<BasicNodeType>,
            std::is_same<typename BasicNodeType::sequence_type, remove_cvref_t<T>>>::value,
        int> = 0>
inline void to_node(BasicNodeType& n, T&& s) noexcept
{
    external_node_constructor<node_t::SEQUENCE>::construct(n, std::forward<T>(s));
}

/// @brief to_node function for BasicNodeType::mapping_type objects.
/// @tparam BasicNodeType A basid_node template instance type.
/// @tparam T A mapping node value type.
/// @param n A basic_node object.
/// @param m A mapping node value object.
template <
    typename BasicNodeType, typename T,
    enable_if_t<
        conjunction<
            is_basic_node<BasicNodeType>, std::is_same<typename BasicNodeType::mapping_type, remove_cvref_t<T>>>::value,
        int> = 0>
inline void to_node(BasicNodeType& n, T&& m) noexcept
{
    external_node_constructor<node_t::MAPPING>::construct(n, std::forward<T>(m));
}

/// @brief to_node function for null objects.
/// @tparam BasicNodeType A mapping node value type.
/// @tparam NullType This must be std::nullptr_t type
template <
    typename BasicNodeType, typename NullType,
    enable_if_t<conjunction<is_basic_node<BasicNodeType>, std::is_same<NullType, std::nullptr_t>>::value, int> = 0>
inline void to_node(BasicNodeType& n, NullType /*unused*/)
{
    external_node_constructor<node_t::NULL_OBJECT>::construct(n, nullptr);
}

/// @brief to_node function for BasicNodeType::boolean_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A boolean scalar node value type.
/// @param n A basic_node object.
/// @param b A boolean scalar node value object.
template <
    typename BasicNodeType, typename T,
    enable_if_t<
        conjunction<is_basic_node<BasicNodeType>, std::is_same<typename BasicNodeType::boolean_type, T>>::value, int> =
        0>
inline void to_node(BasicNodeType& n, T b) noexcept
{
    external_node_constructor<node_t::BOOLEAN>::construct(n, b);
}

/// @brief to_node function for integers.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T An integer type.
/// @param n A basic_node object.
/// @param i An integer object.
template <
    typename BasicNodeType, typename T,
    enable_if_t<conjunction<is_basic_node<BasicNodeType>, is_non_bool_integral<T>>::value, int> = 0>
inline void to_node(BasicNodeType& n, T i) noexcept
{
    external_node_constructor<node_t::INTEGER>::construct(n, i);
}

/// @brief to_node function for floating point numbers.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A floating point number type.
/// @param n A basic_node object.
/// @param f A floating point number object.
template <
    typename BasicNodeType, typename T,
    enable_if_t<conjunction<is_basic_node<BasicNodeType>, std::is_floating_point<T>>::value, int> = 0>
inline void to_node(BasicNodeType& n, T f) noexcept
{
    external_node_constructor<node_t::FLOAT_NUMBER>::construct(n, f);
}

/// @brief to_node function for compatible strings.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A compatible string type.
/// @param n A basic_node object.
/// @param s A compatible string object.
template <
    typename BasicNodeType, typename T,
    enable_if_t<
        conjunction<
            is_basic_node<BasicNodeType>, negation<is_null_pointer<T>>,
            std::is_constructible<typename BasicNodeType::string_type, const T&>>::value,
        int> = 0>
inline void to_node(BasicNodeType& n, const T& s)
{
    external_node_constructor<node_t::STRING>::construct(n, s);
}

/// @brief to_node function for rvalue string node values
/// @tparam BasicNodeType A basic_node template instance type
/// @param n A basic_node object.
/// @param s An rvalue string node value.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void to_node(BasicNodeType& n, typename BasicNodeType::string_type&& s) noexcept
{
    external_node_constructor<node_t::STRING>::construct(n, std::move(s));
}

/// @brief A function object to call to_node functions.
/// @note User-defined specialization is available by providing implementation **OUTSIDE** fkyaml namespace.
struct to_node_fn
{
    /// @brief Call to_node function suitable for the given T type.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @tparam T A target value type assigned to the basic_node object.
    /// @param n A basic_node object.
    /// @param val A target object assigned to the basic_node object.
    /// @return decltype(to_node(n, std::forward<T>(val))) void by default. User can set it to some other type.
    template <typename BasicNodeType, typename T>
    auto operator()(BasicNodeType& n, T&& val) const noexcept(noexcept(to_node(n, std::forward<T>(val))))
        -> decltype(to_node(n, std::forward<T>(val)))
    {
        return to_node(n, std::forward<T>(val));
    }
};

} // namespace detail

#ifndef FK_YAML_HAS_CXX_17
// anonymous namespace to hold `to_node` functor.
// see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html for why it's needed.
namespace // NOLINT(cert-dcl59-cpp,fuchsia-header-anon-namespaces,google-build-namespaces)
{
#endif

/// @brief A blobal object to represent ADL friendly to_node functor.
// NOLINTNEXTLINE(misc-definitions-in-headers)
FK_YAML_INLINE_VAR constexpr const auto& to_node = detail::static_const<detail::to_node_fn>::value;

#ifndef FK_YAML_HAS_CXX_17
} // namespace
#endif

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_CONVERSIONS_TO_NODE_HPP_ */


FK_YAML_NAMESPACE_BEGIN

/// @brief An ADL friendly converter between basic_node objects and native data objects.
/// @tparam ValueType A default target data type.
/// @sa https://fktn-k.github.io/fkYAML/api/node_value_converter/
template <typename ValueType, typename>
class node_value_converter
{
public:
    /// @brief Convert a YAML node value into compatible native data.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @tparam TargetType A native data type for conversion.
    /// @param n A basic_node object.
    /// @param val A native data object.
    /// @sa https://fktn-k.github.io/fkYAML/api/node_value_converter/from_node/
    template <typename BasicNodeType, typename TargetType = ValueType>
    static auto from_node(BasicNodeType&& n, TargetType& val) noexcept(
        noexcept(::fkyaml::from_node(std::forward<BasicNodeType>(n), val)))
        -> decltype(::fkyaml::from_node(std::forward<BasicNodeType>(n), val), void())
    {
        ::fkyaml::from_node(std::forward<BasicNodeType>(n), val);
    }

    /// @brief Convert compatible native data into a YAML node.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @tparam TargetType A native data type for conversion.
    /// @param n A basic_node object.
    /// @param val A native data object.
    /// @sa https://fktn-k.github.io/fkYAML/api/node_value_converter/to_node/
    template <typename BasicNodeType, typename TargetType = ValueType>
    static auto to_node(BasicNodeType& n, TargetType&& val) noexcept(
        noexcept(::fkyaml::to_node(n, std::forward<TargetType>(val))))
        -> decltype(::fkyaml::to_node(n, std::forward<TargetType>(val)))
    {
        ::fkyaml::to_node(n, std::forward<TargetType>(val));
    }
};

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_NODE_VALUE_CONVERTER_HPP_ */

// #include <fkYAML/ordered_map.hpp>
///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_ORDERED_MAP_HPP_
#define FK_YAML_ORDERED_MAP_HPP_

#include <functional>
#include <initializer_list>
#include <memory>
#include <utility>
#include <vector>

// #include <fkYAML/detail/macros/version_macros.hpp>

// #include <fkYAML/detail/meta/type_traits.hpp>

// #include <fkYAML/exception.hpp>


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief A minimal map-like container which preserves insertion order.
/// @tparam Key A type for keys.
/// @tparam Value A type for values.
/// @tparam IgnoredCompare A placeholder for key comparison. This will be ignored.
/// @tparam Allocator A class for allocators.
/// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/
template <
    typename Key, typename Value, typename IgnoredCompare = std::less<Key>,
    typename Allocator = std::allocator<std::pair<const Key, Value>>>
class ordered_map : public std::vector<std::pair<const Key, Value>, Allocator>
{
public:
    /// A type for keys.
    using key_type = Key;
    /// A type for values.
    using mapped_type = Value;
    /// A type for internal key-value containers.
    using Container = std::vector<std::pair<const Key, Value>, Allocator>;
    /// A type for key-value pairs.
    using value_type = typename Container::value_type;
    /// A type for non-const iterators.
    using iterator = typename Container::iterator;
    /// A type for const iterators.
    using const_iterator = typename Container::const_iterator;
    /// A type for size parameters used in this class.
    using size_type = typename Container::size_type;
    /// A type for comparison between keys.
    using key_compare = std::equal_to<Key>;

public:
    /// @brief Construct a new ordered_map object.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/constructor/
    ordered_map() noexcept(noexcept(Container()))
        : Container()
    {
    }

    /// @brief Construct a new ordered_map object with an initializer list.
    /// @param init An initializer list to construct the inner container object.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/constructor/
    ordered_map(std::initializer_list<value_type> init)
        : Container {init}
    {
    }

public:
    /// @brief A subscript operator for ordered_map objects.
    /// @tparam KeyType A type for the input key.
    /// @param key A key to the target value.
    /// @return mapped_type& Reference to a mapped_type object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/operator[]/
    template <
        typename KeyType,
        detail::enable_if_t<detail::is_usable_as_key_type<key_compare, key_type, KeyType>::value, int> = 0>
    mapped_type& operator[](KeyType&& key) noexcept
    {
        return emplace(std::forward<KeyType>(key), mapped_type()).first->second;
    }

public:
    /// @brief Emplace a new key-value pair if the new key does not exist.
    /// @tparam KeyType A type for the input key.
    /// @param key A key to be emplaced to this ordered_map object.
    /// @param value A value to be emplaced to this ordered_map object.
    /// @return std::pair<iterator, bool> A result of emplacement of the new key-value pair.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/emplace/
    template <
        typename KeyType,
        detail::enable_if_t<detail::is_usable_as_key_type<key_compare, key_type, KeyType>::value, int> = 0>
    std::pair<iterator, bool> emplace(KeyType&& key, const mapped_type& value) noexcept
    {
        for (auto itr = this->begin(); itr != this->end(); ++itr)
        {
            if (m_compare(itr->first, key))
            {
                return {itr, false};
            }
        }
        this->emplace_back(key, value);
        return {std::prev(this->end()), true};
    }

    /// @brief Find a value associated to the given key. Throws an exception if the search fails.
    /// @tparam KeyType A type for the input key.
    /// @param key A key to find a value with.
    /// @return mapped_type& The value associated to the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/at/
    template <
        typename KeyType,
        detail::enable_if_t<detail::is_usable_as_key_type<key_compare, key_type, KeyType>::value, int> = 0>
    mapped_type& at(KeyType&& key)
    {
        for (auto itr = this->begin(); itr != this->end(); ++itr)
        {
            if (m_compare(itr->first, key))
            {
                return itr->second;
            }
        }
        throw fkyaml::exception("key not found.");
    }

    /// @brief Find a value associated to the given key. Throws an exception if the search fails.
    /// @tparam KeyType A type for the input key.
    /// @param key A key to find a value with.
    /// @return const mapped_type& The value associated to the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/at/
    template <
        typename KeyType,
        detail::enable_if_t<detail::is_usable_as_key_type<key_compare, key_type, KeyType>::value, int> = 0>
    const mapped_type& at(KeyType&& key) const
    {
        for (auto itr = this->begin(); itr != this->end(); ++itr)
        {
            if (m_compare(itr->first, key))
            {
                return itr->second;
            }
        }
        throw fkyaml::exception("key not found.");
    }

    /// @brief Find a value with the given key.
    /// @tparam KeyType A type for the input key.
    /// @param key A key to find a value with.
    /// @return iterator The iterator for the found value, or the result of end().
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/find/
    template <
        typename KeyType,
        detail::enable_if_t<detail::is_usable_as_key_type<key_compare, key_type, KeyType>::value, int> = 0>
    iterator find(KeyType&& key) noexcept
    {
        for (auto itr = this->begin(); itr != this->end(); ++itr)
        {
            if (m_compare(itr->first, key))
            {
                return itr;
            }
        }
        return this->end();
    }

    /// @brief Find a value with the given key.
    /// @tparam KeyType A type for the input key.
    /// @param key A key to find a value with.
    /// @return const_iterator The constant iterator for the found value, or the result of end().
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/find/
    template <
        typename KeyType,
        detail::enable_if_t<detail::is_usable_as_key_type<key_compare, key_type, KeyType>::value, int> = 0>
    const_iterator find(KeyType&& key) const noexcept
    {
        for (auto itr = this->begin(); itr != this->end(); ++itr)
        {
            if (m_compare(itr->first, key))
            {
                return itr;
            }
        }
        return this->end();
    }

private:
    /// The object for comparing keys.
    key_compare m_compare {};
};

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_ORDERED_MAP_HPP_ */


/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief A class to store value of YAML nodes.
/// @sa https://fktn-k.github.io/fkYAML/api/basic_node/
template <
    template <typename, typename...> class SequenceType = std::vector,
    template <typename, typename, typename...> class MappingType = std::map, typename BooleanType = bool,
    typename IntegerType = std::int64_t, typename FloatNumberType = double, typename StringType = std::string,
    template <typename, typename = void> class ConverterType = node_value_converter>
class basic_node
{
public:
    /// @brief A type for iterators of basic_node containers.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/iterator/
    using iterator = fkyaml::detail::iterator<basic_node>;

    /// @brief A type for constant iterators of basic_node containers.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/const_iterator/
    using const_iterator = fkyaml::detail::iterator<const basic_node>;

    /// @brief A type for sequence basic_node values.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/sequence_type/
    using sequence_type = SequenceType<basic_node, std::allocator<basic_node>>;

    /// @brief A type for mapping basic_node values.
    /// @note std::unordered_map is not supported since it does not allow incomplete types.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/mapping_type/
    using mapping_type = MappingType<basic_node, basic_node>;

    /// @brief A type for boolean basic_node values.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/boolean_type/
    using boolean_type = BooleanType;

    /// @brief A type for integer basic_node values.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/integer_type/
    using integer_type = IntegerType;

    /// @brief A type for float number basic_node values.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/float_number_type/
    using float_number_type = FloatNumberType;

    /// @brief A type for string basic_node values.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/string_type/
    using string_type = StringType;

    /// @brief A helper alias to determine converter type for the given target native data type.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/value_converter_type/
    template <typename T, typename SFINAE>
    using value_converter_type = ConverterType<T, SFINAE>;

    /// @brief Definition of node value types.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/node_t/
    using node_t = detail::node_t;

    /// @brief Definition of YAML version types.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/yaml_version_t/
    using yaml_version_t = detail::yaml_version_t;

private:
    template <node_t>
    friend struct fkyaml::detail::external_node_constructor;

    /// @brief A type for YAML docs deserializers.
    using deserializer_type = detail::basic_deserializer<basic_node>;
    /// @brief A type for YAML docs serializers.
    using serializer_type = detail::basic_serializer<basic_node>;
    /// @brief A helper type alias for std::initializer_list.
    using initializer_list_t = std::initializer_list<detail::node_ref_storage<basic_node>>;

    /// @brief The actual storage for a YAML node value of the @ref basic_node class.
    /// @details This union combines the different sotrage types for the YAML value types defined in @ref node_t.
    /// @note Container types are stored as pointers so that the size of this union will not exceed 64 bits by
    /// default.
    union node_value
    {
        /// @brief Constructs a new basic_node Value object for null types.
        node_value() = default;

        /// @brief Constructs a new basic_node value object with a node type. The default value for the specified
        /// type will be assigned.
        /// @param[in] type A node type.
        explicit node_value(node_t type)
        {
            switch (type)
            {
            case node_t::SEQUENCE:
                p_sequence = create_object<sequence_type>();
                break;
            case node_t::MAPPING:
                p_mapping = create_object<mapping_type>();
                break;
            case node_t::NULL_OBJECT:
                p_mapping = nullptr;
                break;
            case node_t::BOOLEAN:
                boolean = static_cast<boolean_type>(false);
                break;
            case node_t::INTEGER:
                integer = static_cast<integer_type>(0);
                break;
            case node_t::FLOAT_NUMBER:
                float_val = static_cast<float_number_type>(0.0);
                break;
            case node_t::STRING:
                p_string = create_object<string_type>();
                break;
            }
        }

        /// @brief Destroys the existing Node value. This process is recursive if the specified node type is for
        /// containers.
        /// @param[in] type A Node type to determine the value to be destroyed.
        void destroy(node_t type)
        {
            if (type == node_t::SEQUENCE || type == node_t::MAPPING)
            {
                std::vector<basic_node> stack;

                if (type == node_t::SEQUENCE)
                {
                    stack.reserve(p_sequence->size());
                    std::move(p_sequence->begin(), p_sequence->end(), std::back_inserter(stack));
                }
                else
                {
                    stack.reserve(p_mapping->size());
                    for (auto&& it : *p_mapping)
                    {
                        stack.push_back(std::move(it.second));
                    }
                }

                while (!stack.empty())
                {
                    basic_node current_node(std::move(stack.back()));
                    stack.pop_back();

                    if (current_node.is_sequence())
                    {
                        std::move(
                            current_node.m_node_value.p_sequence->begin(),
                            current_node.m_node_value.p_sequence->end(),
                            std::back_inserter(stack));
                        current_node.m_node_value.p_sequence->clear();
                    }
                    else if (current_node.is_mapping())
                    {
                        for (auto&& it : *current_node.m_node_value.p_mapping)
                        {
                            stack.push_back(std::move(it.second));
                        }
                        current_node.m_node_value.p_mapping->clear();
                    }
                }
            }

            switch (type)
            {
            case node_t::SEQUENCE:
                destroy_object<sequence_type>(p_sequence);
                p_sequence = nullptr;
                break;
            case node_t::MAPPING:
                destroy_object<mapping_type>(p_mapping);
                p_mapping = nullptr;
                break;
            case node_t::STRING:
                destroy_object<string_type>(p_string);
                p_string = nullptr;
                break;
            default:
                break;
            }
        }

        /// A pointer to the value of sequence type.
        sequence_type* p_sequence;
        /// A pointer to the value of mapping type. This pointer is also used when node type is null.
        mapping_type* p_mapping {nullptr};
        /// A value of boolean type.
        boolean_type boolean;
        /// A value of integer type.
        integer_type integer;
        /// A value of float number type.
        float_number_type float_val;
        /// A pointer to the value of string type.
        string_type* p_string;
    };

private:
    /// @brief Allocates and constructs an object with a given type and arguments.
    /// @tparam ObjType The target object type.
    /// @tparam ArgTypes The packed argument types for constructor arguments.
    /// @param[in] args A parameter pack for constructor arguments of the target object type.
    /// @return ObjType* An address of allocated memory on the heap.
    template <typename ObjType, typename... ArgTypes>
    static ObjType* create_object(ArgTypes&&... args)
    {
        using AllocType = std::allocator<ObjType>;
        using AllocTraitsType = std::allocator_traits<AllocType>;

        AllocType alloc;
        auto deleter = [&](ObjType* obj) {
            AllocTraitsType::destroy(alloc, obj);
            AllocTraitsType::deallocate(alloc, obj, 1);
        };

        std::unique_ptr<ObjType, decltype(deleter)> object(AllocTraitsType::allocate(alloc, 1), deleter);
        AllocTraitsType::construct(alloc, object.get(), std::forward<ArgTypes>(args)...);

        FK_YAML_ASSERT(object != nullptr);
        return object.release();
    }

    /// @brief Destroys and deallocates an object with specified type.
    /// @warning Make sure the `obj` parameter is not nullptr before calling this function.
    /// @tparam ObjType The target object type.
    /// @param[in] obj A pointer to the target object to be destroyed.
    template <typename ObjType>
    static void destroy_object(ObjType* obj)
    {
        FK_YAML_ASSERT(obj != nullptr);
        std::allocator<ObjType> alloc;
        std::allocator_traits<decltype(alloc)>::destroy(alloc, obj);
        std::allocator_traits<decltype(alloc)>::deallocate(alloc, obj, 1);
    }

public:
    /// @brief Constructs a new basic_node object of null type.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    basic_node() = default;

    /// @brief Constructs a new basic_node object with a specified type.
    /// @param[in] type A YAML node type.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    explicit basic_node(const node_t type)
        : m_node_type(type),
          m_node_value(type)
    {
    }

    /// @brief Copy constructor of the basic_node class.
    /// @param[in] rhs A basic_node object to be copied with.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    basic_node(const basic_node& rhs)
        : m_node_type(rhs.m_node_type),
          m_yaml_version_type(rhs.m_yaml_version_type),
          m_prop(rhs.m_prop)
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            m_node_value.p_sequence = create_object<sequence_type>(*(rhs.m_node_value.p_sequence));
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            break;
        case node_t::MAPPING:
            m_node_value.p_mapping = create_object<mapping_type>(*(rhs.m_node_value.p_mapping));
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            break;
        case node_t::NULL_OBJECT:
            m_node_value.p_mapping = nullptr;
            break;
        case node_t::BOOLEAN:
            m_node_value.boolean = rhs.m_node_value.boolean;
            break;
        case node_t::INTEGER:
            m_node_value.integer = rhs.m_node_value.integer;
            break;
        case node_t::FLOAT_NUMBER:
            m_node_value.float_val = rhs.m_node_value.float_val;
            break;
        case node_t::STRING:
            m_node_value.p_string = create_object<string_type>(*(rhs.m_node_value.p_string));
            FK_YAML_ASSERT(m_node_value.p_string != nullptr);
            break;
        }
    }

    /// @brief Move constructor of the basic_node class.
    /// @param[in] rhs A basic_node object to be moved from.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    basic_node(basic_node&& rhs) noexcept
        : m_node_type(rhs.m_node_type),
          m_yaml_version_type(rhs.m_yaml_version_type),
          m_prop(std::move(rhs.m_prop))
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            FK_YAML_ASSERT(rhs.m_node_value.p_sequence != nullptr);
            m_node_value.p_sequence = rhs.m_node_value.p_sequence;
            rhs.m_node_value.p_sequence = nullptr;
            break;
        case node_t::MAPPING:
            FK_YAML_ASSERT(rhs.m_node_value.p_mapping != nullptr);
            m_node_value.p_mapping = rhs.m_node_value.p_mapping;
            rhs.m_node_value.p_mapping = nullptr;
            break;
        case node_t::NULL_OBJECT:
            FK_YAML_ASSERT(rhs.m_node_value.p_mapping == nullptr);
            m_node_value.p_mapping = rhs.m_node_value.p_mapping;
            break;
        case node_t::BOOLEAN:
            m_node_value.boolean = rhs.m_node_value.boolean;
            rhs.m_node_value.boolean = static_cast<boolean_type>(false);
            break;
        case node_t::INTEGER:
            m_node_value.integer = rhs.m_node_value.integer;
            rhs.m_node_value.integer = static_cast<integer_type>(0);
            break;
        case node_t::FLOAT_NUMBER:
            m_node_value.float_val = rhs.m_node_value.float_val;
            rhs.m_node_value.float_val = static_cast<float_number_type>(0.0);
            break;
        case node_t::STRING:
            FK_YAML_ASSERT(rhs.m_node_value.p_string != nullptr);
            m_node_value.p_string = rhs.m_node_value.p_string;
            rhs.m_node_value.p_string = nullptr;
            break;
        }

        rhs.m_node_type = node_t::NULL_OBJECT;
        rhs.m_yaml_version_type = yaml_version_t::VER_1_2;
        rhs.m_node_value.p_mapping = nullptr;
        rhs.m_prop.anchor_status = detail::anchor_status_t::NONE;
    }

    /// @brief Construct a new basic_node object from a value of compatible types.
    /// @tparam CompatibleType Type of native data which is compatible with node values.
    /// @tparam U Type of compatible native data without cv-qualifiers and reference.
    /// @param[in] val The value of a compatible type.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    template <
        typename CompatibleType, typename U = detail::remove_cvref_t<CompatibleType>,
        detail::enable_if_t<
            detail::conjunction<
                detail::negation<detail::is_basic_node<U>>,
                detail::disjunction<detail::is_node_compatible_type<basic_node, U>>>::value,
            int> = 0>
    basic_node(CompatibleType&& val) noexcept(
        noexcept(ConverterType<U>::to_node(std::declval<basic_node&>(), std::declval<CompatibleType>())))
    {
        ConverterType<U>::to_node(*this, std::forward<CompatibleType>(val));
    }

    /// @brief Construct a new basic node object with a node_ref_storage object.
    /// @tparam NodeRefStorageType Type of basic_node with reference.
    /// @param[in] node_ref_storage A node_ref_storage template class object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    template <
        typename NodeRefStorageType,
        detail::enable_if_t<detail::is_node_ref_storage<NodeRefStorageType>::value, int> = 0>
    basic_node(const NodeRefStorageType& node_ref_storage) noexcept
        : basic_node(node_ref_storage.release())
    {
    }

    /// @brief Construct a new basic node object with std::initializer_list.
    /// @param[in] init A initializer list of basic_node objects.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    basic_node(initializer_list_t init)
    {
        bool is_mapping =
            std::all_of(init.begin(), init.end(), [](const detail::node_ref_storage<basic_node>& node_ref) {
                return node_ref->is_sequence() && node_ref->size() == 2;
            });

        if (is_mapping)
        {
            m_node_type = node_t::MAPPING;
            m_node_value.p_mapping = create_object<mapping_type>();

            for (auto& elem_ref : init)
            {
                auto elem = elem_ref.release();
                m_node_value.p_mapping->emplace(
                    std::move((*(elem.m_node_value.p_sequence))[0]), std::move((*(elem.m_node_value.p_sequence))[1]));
            }
        }
        else
        {
            m_node_type = node_t::SEQUENCE;
            m_node_value.p_sequence = create_object<sequence_type>(init.begin(), init.end());
        }
    }

    /// @brief Destroy the basic_node object and its value storage.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/destructor/
    ~basic_node() noexcept // NOLINT(bugprone-exception-escape)
    {
        m_node_value.destroy(m_node_type);
        m_node_type = node_t::NULL_OBJECT;
    }

public:
    /// @brief Deserialize an input source into a basic_node object.
    /// @tparam InputType Type of a compatible input.
    /// @param[in] input An input source in the YAML format.
    /// @return The resulting basic_node object deserialized from the input source.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/deserialize/
    template <typename InputType>
    static basic_node deserialize(InputType&& input)
    {
        return deserializer_type().deserialize(detail::input_adapter(std::forward<InputType>(input)));
    }

    /// @brief Deserialize input iterators into a basic_node object.
    /// @tparam ItrType Type of a compatible iterator.
    /// @param[in] begin An iterator to the first element of an input sequence.
    /// @param[in] end An iterator to the past-the-last element of an input sequence.
    /// @return The resulting basic_node object deserialized from the pair of iterators.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/deserialize/
    template <typename ItrType>
    static basic_node deserialize(ItrType&& begin, ItrType&& end)
    {
        return deserializer_type().deserialize(
            detail::input_adapter(std::forward<ItrType>(begin), std::forward<ItrType>(end)));
    }

    /// @brief Serialize a basic_node object into a string.
    /// @param[in] node A basic_node object to be serialized.
    /// @return The resulting string object from the serialization of the node object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/serialize/
    static std::string serialize(const basic_node& node)
    {
        return serializer_type().serialize(node);
    }

    /// @brief A factory method for sequence basic_node objects without sequence_type objects.
    /// @return A YAML sequence node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/sequence/
    static basic_node sequence()
    {
        basic_node node;
        node.m_node_type = node_t::SEQUENCE;
        node.m_node_value.p_sequence = create_object<sequence_type>();
        FK_YAML_ASSERT(node.m_node_value.p_sequence != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for sequence basic_node objects with lvalue sequence_type objects.
    /// @param[in] seq A lvalue sequence node value.
    /// @return A YAML sequence node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/sequence/
    static basic_node sequence(const sequence_type& seq)
    {
        basic_node node;
        node.m_node_type = node_t::SEQUENCE;
        node.m_node_value.p_sequence = create_object<sequence_type>(seq);
        FK_YAML_ASSERT(node.m_node_value.p_sequence != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for sequence basic_node objects with rvalue sequence_type objects.
    /// @param[in] seq A rvalue sequence node value.
    /// @return A YAML sequence node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/sequence/
    static basic_node sequence(sequence_type&& seq)
    {
        basic_node node;
        node.m_node_type = node_t::SEQUENCE;
        node.m_node_value.p_sequence = create_object<sequence_type>(std::move(seq));
        FK_YAML_ASSERT(node.m_node_value.p_sequence != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for mapping basic_node objects without mapping_type objects.
    /// @return A YAML mapping node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/mapping/
    static basic_node mapping()
    {
        basic_node node;
        node.m_node_type = node_t::MAPPING;
        node.m_node_value.p_mapping = create_object<mapping_type>();
        FK_YAML_ASSERT(node.m_node_value.p_mapping != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for mapping basic_node objects with lvalue mapping_type objects.
    /// @param[in] map A lvalue mapping node value.
    /// @return A YAML mapping node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/mapping/
    static basic_node mapping(const mapping_type& map)
    {
        basic_node node;
        node.m_node_type = node_t::MAPPING;
        node.m_node_value.p_mapping = create_object<mapping_type>(map);
        FK_YAML_ASSERT(node.m_node_value.p_mapping != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for mapping basic_node objects with rvalue mapping_type objects.
    /// @param[in] map A rvalue mapping node value.
    /// @return A YAML mapping node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/mapping/
    static basic_node mapping(mapping_type&& map)
    {
        basic_node node;
        node.m_node_type = node_t::MAPPING;
        node.m_node_value.p_mapping = create_object<mapping_type>(std::move(map));
        FK_YAML_ASSERT(node.m_node_value.p_mapping != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for alias basic_node objects referencing the given anchor basic_node object.
    /// @note The given anchor basic_node must have a non-empty anchor name.
    /// @param[in] anchor_node A basic_node object with an anchor name.
    /// @return An alias YAML node created from the given anchor node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/alias_of/
    static basic_node alias_of(const basic_node& anchor_node)
    {
        if (!anchor_node.has_anchor_name() || anchor_node.m_prop.anchor_status != detail::anchor_status_t::ANCHOR)
        {
            throw fkyaml::exception("Cannot create an alias without anchor name.");
        }

        basic_node node = anchor_node;
        node.m_prop.anchor_status = detail::anchor_status_t::ALIAS;
        node.m_prop.anchor = anchor_node.m_prop.anchor;
        return node;
    } // LCOV_EXCL_LINE

public:
    /// @brief A copy assignment operator of the basic_node class.
    /// @param[in] rhs A lvalue basic_node object to be copied with.
    /// @return Reference to this basic_node object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator=/
    basic_node& operator=(const basic_node& rhs) noexcept
    {
        basic_node(rhs).swap(*this);
        return *this;
    }

    /// @brief A move assignment operator of the basic_node class.
    /// @param[in] rhs A rvalue basic_node object to be moved from.
    /// @return Reference to this basic_node object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator=/
    basic_node& operator=(basic_node&& rhs) noexcept
    {
        basic_node(std::move(rhs)).swap(*this);
        return *this;
    }

    /// @brief A subscript operator of the basic_node class with a key of a compatible type with basic_node.
    /// @tparam KeyType A key type compatible with basic_node
    /// @param key A key to the target value in a sequence/mapping node.
    /// @return The value associated with the given key, or a default basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator[]/
    template <
        typename KeyType, detail::enable_if_t<
                              detail::conjunction<
                                  detail::negation<detail::is_basic_node<KeyType>>,
                                  detail::is_node_compatible_type<basic_node, KeyType>>::value,
                              int> = 0>
    basic_node& operator[](KeyType&& key)
    {
        if (is_scalar())
        {
            throw fkyaml::type_error("operator[] is unavailable for a scalar node.", m_node_type);
        }

        basic_node n = std::forward<KeyType>(key);

        if (is_sequence())
        {
            if (!n.is_integer())
            {
                throw fkyaml::type_error(
                    "An argument of operator[] for sequence nodes must be an integer.", m_node_type);
            }
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return m_node_value.p_sequence->operator[](n.get_value<int>());
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        return m_node_value.p_mapping->operator[](std::move(n));
    }

    /// @brief A subscript operator of the basic_node class with a key of a compatible type with basic_node.
    /// @tparam KeyType A key type compatible with basic_node
    /// @param key A key to the target value in a sequence/mapping node.
    /// @return The value associated with the given key, or a default basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator[]/
    template <
        typename KeyType, detail::enable_if_t<
                              detail::conjunction<
                                  detail::negation<detail::is_basic_node<KeyType>>,
                                  detail::is_node_compatible_type<basic_node, KeyType>>::value,
                              int> = 0>
    const basic_node& operator[](KeyType&& key) const
    {
        if (is_scalar())
        {
            throw fkyaml::type_error("operator[] is unavailable for a scalar node.", m_node_type);
        }

        basic_node node_key = std::forward<KeyType>(key);

        if (is_sequence())
        {
            if (!node_key.is_integer())
            {
                throw fkyaml::type_error(
                    "An argument of operator[] for sequence nodes must be an integer.", m_node_type);
            }
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return m_node_value.p_sequence->operator[](node_key.get_value<int>());
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        return m_node_value.p_mapping->operator[](std::move(node_key));
    }

    /// @brief A subscript operator of the basic_node class with a basic_node key object.
    /// @tparam KeyType A key type which is a kind of the basic_node template class.
    /// @param key A key to the target value in a sequence/mapping node.
    /// @return The value associated with the given key, or a default basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator[]/
    template <
        typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
    basic_node& operator[](KeyType&& key)
    {
        if (is_scalar())
        {
            throw fkyaml::type_error("operator[] is unavailable for a scalar node.", m_node_type);
        }

        if (is_sequence())
        {
            if (!key.is_integer())
            {
                throw fkyaml::type_error(
                    "An argument of operator[] for sequence nodes must be an integer.", m_node_type);
            }
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return m_node_value.p_sequence->operator[](key.template get_value<int>());
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        return m_node_value.p_mapping->operator[](std::forward<KeyType>(key));
    }

    /// @brief A subscript operator of the basic_node class with a basic_node key object.
    /// @tparam KeyType A key type which is a kind of the basic_node template class.
    /// @param key A key to the target value in a sequence/mapping node.
    /// @return The value associated with the given key, or a default basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator[]/
    template <
        typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
    const basic_node& operator[](KeyType&& key) const
    {
        if (is_scalar())
        {
            throw fkyaml::type_error("operator[] is unavailable for a scalar node.", m_node_type);
        }

        if (is_sequence())
        {
            if (!key.is_integer())
            {
                throw fkyaml::type_error(
                    "An argument of operator[] for sequence nodes must be an integer.", m_node_type);
            }
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return m_node_value.p_sequence->operator[](key.template get_value<int>());
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        return m_node_value.p_mapping->operator[](std::forward<KeyType>(key));
    }

    /// @brief An equal-to operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true if both types and values are equal, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_eq/
    bool operator==(const basic_node& rhs) const noexcept
    {
        if (m_node_type != rhs.m_node_type)
        {
            return false;
        }

        bool ret = false;
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            ret = (*(m_node_value.p_sequence) == *(rhs.m_node_value.p_sequence));
            break;
        case node_t::MAPPING:
            ret = (*(m_node_value.p_mapping) == *(rhs.m_node_value.p_mapping));
            break;
        case node_t::NULL_OBJECT:
            // Always true for comparisons between null nodes.
            ret = true;
            break;
        case node_t::BOOLEAN:
            ret = (m_node_value.boolean == rhs.m_node_value.boolean);
            break;
        case node_t::INTEGER:
            ret = (m_node_value.integer == rhs.m_node_value.integer);
            break;
        case node_t::FLOAT_NUMBER:
            ret =
                (std::abs(m_node_value.float_val - rhs.m_node_value.float_val) <
                 std::numeric_limits<float_number_type>::epsilon());
            break;
        case node_t::STRING:
            ret = (*(m_node_value.p_string) == *(rhs.m_node_value.p_string));
            break;
        }

        return ret;
    }

    /// @brief A not-equal-to operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true if either types or values are different, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_ne/
    bool operator!=(const basic_node& rhs) const noexcept
    {
        return !operator==(rhs);
    }

    /// @brief A less-than operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true this basic_node object is less than `rhs`.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_lt/
    bool operator<(const basic_node& rhs) const noexcept
    {
        if (operator==(rhs))
        {
            return false;
        }

        if (uint32_t(m_node_type) < uint32_t(rhs.m_node_type))
        {
            return true;
        }

        if (m_node_type != rhs.m_node_type)
        {
            return false;
        }

        bool ret = false;
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            ret = (*(m_node_value.p_sequence) < *(rhs.m_node_value.p_sequence));
            break;
        case node_t::MAPPING:
            ret = (*(m_node_value.p_mapping) < *(rhs.m_node_value.p_mapping));
            break;
        case node_t::NULL_OBJECT: // LCOV_EXCL_LINE
            // Will not come here since null nodes are alyways the same.
            break; // LCOV_EXCL_LINE
        case node_t::BOOLEAN:
            // false < true
            ret = (!m_node_value.boolean && rhs.m_node_value.boolean);
            break;
        case node_t::INTEGER:
            ret = (m_node_value.integer < rhs.m_node_value.integer);
            break;
        case node_t::FLOAT_NUMBER:
            ret = (m_node_value.float_val < rhs.m_node_value.float_val);
            break;
        case node_t::STRING:
            ret = (*(m_node_value.p_string) < *(rhs.m_node_value.p_string));
            break;
        }

        return ret;
    }

    /// @brief A less-than-or-equal-to operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true this basic_node object is less than or equal to `rhs`.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_le/
    bool operator<=(const basic_node& rhs) const noexcept
    {
        return !rhs.operator<(*this);
    }

    /// @brief A greater-than operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true this basic_node object is greater than `rhs`.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_gt/
    bool operator>(const basic_node& rhs) const noexcept
    {
        return !operator<=(rhs);
    }

    /// @brief A greater-than-or-equal-to operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true this basic_node object is greater than or equal to `rhs`.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_ge/
    bool operator>=(const basic_node& rhs) const noexcept
    {
        return !operator<(rhs);
    }

public:
    /// @brief Returns the type of the current basic_node value.
    /// @return The type of the YAML node value.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/type/
    node_t type() const noexcept
    {
        return m_node_type;
    }

    /// @brief Tests whether the current basic_node value is of sequence type.
    /// @return true if the type is sequence, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_sequence/
    bool is_sequence() const noexcept
    {
        return m_node_type == node_t::SEQUENCE;
    }

    /// @brief Tests whether the current basic_node value is of mapping type.
    /// @return true if the type is mapping, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_mapping/
    bool is_mapping() const noexcept
    {
        return m_node_type == node_t::MAPPING;
    }

    /// @brief Tests whether the current basic_node value is of null type.
    /// @return true if the type is null, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_null/
    bool is_null() const noexcept
    {
        return m_node_type == node_t::NULL_OBJECT;
    }

    /// @brief Tests whether the current basic_node value is of boolean type.
    /// @return true if the type is boolean, false otherwise
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_boolean/
    bool is_boolean() const noexcept
    {
        return m_node_type == node_t::BOOLEAN;
    }

    /// @brief Tests whether the current basic_node value is of integer type.
    /// @return true if the type is integer, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_integer/
    bool is_integer() const noexcept
    {
        return m_node_type == node_t::INTEGER;
    }

    /// @brief Tests whether the current basic_node value is of float number type.
    /// @return true if the type is floating point number, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_float_number/
    bool is_float_number() const noexcept
    {
        return m_node_type == node_t::FLOAT_NUMBER;
    }

    /// @brief Tests whether the current basic_node value is of string type.
    /// @return true if the type is string, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_string/
    bool is_string() const noexcept
    {
        return m_node_type == node_t::STRING;
    }

    /// @brief Tests whether the current basic_node value is of scalar types.
    /// @return true if the type is scalar, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_scalar/
    bool is_scalar() const noexcept
    {
        return !is_sequence() && !is_mapping();
    }

    /// @brief Tests whether the current basic_node is an anchor node.
    /// @return true if the current basic_node is an anchor node, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_anchor/
    bool is_anchor() const noexcept
    {
        return m_prop.anchor_status == detail::anchor_status_t::ANCHOR;
    }

    /// @brief Tests whether the current basic_node is an alias node.
    /// @return true if the current basic_node is an alias node, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_alias/
    bool is_alias() const noexcept
    {
        return m_prop.anchor_status == detail::anchor_status_t::ALIAS;
    }

    /// @brief Tests whether the current basic_node value (sequence, mapping, string) is empty.
    /// @return true if the node value is empty, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/empty/
    bool empty() const
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return m_node_value.p_sequence->empty();
        case node_t::MAPPING:
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            return m_node_value.p_mapping->empty();
        case node_t::STRING:
            FK_YAML_ASSERT(m_node_value.p_string != nullptr);
            return m_node_value.p_string->empty();
        default:
            throw fkyaml::type_error("The target node is not of a container type.", m_node_type);
        }
    }

    /// @brief Returns the size of the current basic_node value (sequence, mapping, string).
    /// @return The size of a node value.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/size/
    std::size_t size() const
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return m_node_value.p_sequence->size();
        case node_t::MAPPING:
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            return m_node_value.p_mapping->size();
        case node_t::STRING:
            FK_YAML_ASSERT(m_node_value.p_string != nullptr);
            return m_node_value.p_string->size();
        default:
            throw fkyaml::type_error("The target node is not of a container type.", m_node_type);
        }
    }

    /// @brief Check whether or not this basic_node object has a given key in its inner mapping node value.
    /// @tparam KeyType A key type compatible with basic_node.
    /// @param key A key to the target value in the mapping node value.
    /// @return true if the target node is a mapping and has the given key, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/contains/
    template <
        typename KeyType, detail::enable_if_t<
                              detail::conjunction<
                                  detail::negation<detail::is_basic_node<detail::remove_cvref_t<KeyType>>>,
                                  detail::is_node_compatible_type<basic_node, detail::remove_cvref_t<KeyType>>>::value,
                              int> = 0>
    bool contains(KeyType&& key) const
    {
        switch (m_node_type)
        {
        case node_t::MAPPING: {
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            mapping_type& map = *m_node_value.p_mapping;
            basic_node node_key = std::forward<KeyType>(key);
            return map.find(node_key) != map.end();
        }
        default:
            return false;
        }
    }

    /// @brief Check whether or not this basic_node object has a given key in its inner mapping Node value.
    /// @tparam KeyType A key type which is a kind of basic_node template class.
    /// @param[in] key A key to the target value in the YAML mapping node value.
    /// @return true if the YAML node is a mapping and has the given key, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/contains/
    template <
        typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
    bool contains(KeyType&& key) const
    {
        switch (m_node_type)
        {
        case node_t::MAPPING: {
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            mapping_type& map = *m_node_value.p_mapping;
            return map.find(std::forward<KeyType>(key)) != map.end();
        }
        default:
            return false;
        }
    }

    /// @brief Get a basic_node object with a key of a compatible type.
    /// @tparam KeyType A key type compatible with basic_node
    /// @param key A key to the target basic_node object in a sequence/mapping node.
    /// @return Reference to the basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/at/
    template <
        typename KeyType, detail::enable_if_t<
                              detail::conjunction<
                                  detail::negation<detail::is_basic_node<KeyType>>,
                                  detail::is_node_compatible_type<basic_node, KeyType>>::value,
                              int> = 0>
    basic_node& at(KeyType&& key)
    {
        if (is_scalar())
        {
            throw fkyaml::type_error("at() is unavailable for a scalar node.", m_node_type);
        }

        basic_node node_key = std::forward<KeyType>(key);

        if (is_sequence())
        {
            if (!node_key.is_integer())
            {
                throw fkyaml::type_error("An argument of at() for sequence nodes must be an integer.", m_node_type);
            }

            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            int index = node_key.template get_value<int>();
            int size = static_cast<int>(m_node_value.p_sequence->size());
            if (index >= size)
            {
                throw fkyaml::out_of_range(index);
            }
            return m_node_value.p_sequence->at(index);
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        bool is_found = m_node_value.p_mapping->find(node_key) != m_node_value.p_mapping->end();
        if (!is_found)
        {
            throw fkyaml::out_of_range(serialize(node_key).c_str());
        }
        return m_node_value.p_mapping->at(node_key);
    }

    /// @brief Get a basic_node object with a key of a compatible type.
    /// @tparam KeyType A key type compatible with basic_node
    /// @param key A key to the target basic_node object in a sequence/mapping node.
    /// @return Constant reference to the basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/at/
    template <
        typename KeyType, detail::enable_if_t<
                              detail::conjunction<
                                  detail::negation<detail::is_basic_node<KeyType>>,
                                  detail::is_node_compatible_type<basic_node, KeyType>>::value,
                              int> = 0>
    const basic_node& at(KeyType&& key) const
    {
        if (is_scalar())
        {
            throw fkyaml::type_error("at() is unavailable for a scalar node.", m_node_type);
        }

        basic_node node_key = std::forward<KeyType>(key);

        if (is_sequence())
        {
            if (!node_key.is_integer())
            {
                throw fkyaml::type_error("An argument of at() for sequence nodes must be an integer.", m_node_type);
            }

            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            int index = node_key.template get_value<int>();
            int size = static_cast<int>(m_node_value.p_sequence->size());
            if (index >= size)
            {
                throw fkyaml::out_of_range(index);
            }
            return m_node_value.p_sequence->at(index);
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        bool is_found = m_node_value.p_mapping->find(node_key) != m_node_value.p_mapping->end();
        if (!is_found)
        {
            throw fkyaml::out_of_range(serialize(node_key).c_str());
        }
        return m_node_value.p_mapping->at(node_key);
    }

    /// @brief Get a basic_node object with a basic_node key object.
    /// @tparam KeyType A key type which is a kind of the basic_node template class.
    /// @param key A key to the target basic_node object in a sequence/mapping node.
    /// @return Reference to the basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/at/
    template <
        typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
    basic_node& at(KeyType&& key)
    {
        if (is_scalar())
        {
            throw fkyaml::type_error("at() is unavailable for a scalar node.", m_node_type);
        }

        if (is_sequence())
        {
            if (!key.is_integer())
            {
                throw fkyaml::type_error("An argument of at() for sequence nodes must be an integer.", m_node_type);
            }

            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            int index = std::forward<KeyType>(key).template get_value<int>();
            int size = static_cast<int>(m_node_value.p_sequence->size());
            if (index >= size)
            {
                throw fkyaml::out_of_range(index);
            }
            return m_node_value.p_sequence->at(index);
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        bool is_found = m_node_value.p_mapping->find(key) != m_node_value.p_mapping->end();
        if (!is_found)
        {
            throw fkyaml::out_of_range(serialize(key).c_str());
        }
        return m_node_value.p_mapping->at(key);
    }

    /// @brief Get a basic_node object with a basic_node key object.
    /// @tparam KeyType A key type which is a kind of the basic_node template class.
    /// @param key A key to the target basic_node object in a sequence/mapping node.
    /// @return Constant reference to the basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/at/
    template <
        typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
    const basic_node& at(KeyType&& key) const
    {
        if (is_scalar())
        {
            throw fkyaml::type_error("at() is unavailable for a scalar node.", m_node_type);
        }

        if (is_sequence())
        {
            if (!key.is_integer())
            {
                throw fkyaml::type_error("An argument of at() for sequence nodes must be an integer.", m_node_type);
            }

            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            int index = std::forward<KeyType>(key).template get_value<int>();
            int size = static_cast<int>(m_node_value.p_sequence->size());
            if (index >= size)
            {
                throw fkyaml::out_of_range(index);
            }
            return m_node_value.p_sequence->at(index);
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        bool is_found = m_node_value.p_mapping->find(key) != m_node_value.p_mapping->end();
        if (!is_found)
        {
            throw fkyaml::out_of_range(serialize(key).c_str());
        }
        return m_node_value.p_mapping->at(key);
    }

    /// @brief Get the YAML version specification for this basic_node object.
    /// @return The version of the YAML format applied to the basic_node object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_yaml_version/
    yaml_version_t get_yaml_version() const noexcept
    {
        return m_yaml_version_type;
    }

    /// @brief Set the YAML version specification for this basic_node object.
    /// @param[in] A version of the YAML format.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/set_yaml_version/
    void set_yaml_version(const yaml_version_t version) noexcept
    {
        m_yaml_version_type = version;
    }

    /// @brief Check whether or not this basic_node object has already had any anchor name.
    /// @return true if ths basic_node has an anchor name, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/has_anchor_name/
    bool has_anchor_name() const noexcept
    {
        return m_prop.anchor_status != detail::anchor_status_t::NONE && !m_prop.anchor.empty();
    }

    /// @brief Get the anchor name associated to this basic_node object.
    /// @note Some anchor name must be set before calling this method. Call basic_node::HasAnchorName() to see if this
    /// basic_node object has any anchor name.
    /// @return The anchor name associated to the node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_anchor_name/
    const std::string& get_anchor_name() const
    {
        if (!has_anchor_name())
        {
            throw fkyaml::exception("No anchor name has been set.");
        }
        return m_prop.anchor;
    }

    /// @brief Add an anchor name to this basic_node object.
    /// @note If this basic_node object has already had any anchor name, the new anchor name will overwrite the old one.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/add_anchor_name/
    void add_anchor_name(const std::string& anchor_name)
    {
        m_prop.anchor_status = detail::anchor_status_t::ANCHOR;
        m_prop.anchor = anchor_name;
    }

    /// @brief Add an anchor name to this basic_node object.
    /// @note If this basic_node object has already had any anchor name, the new anchor name will overwrite the old one.
    /// @param[in] anchor_name An anchor name.This should not be empty.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/add_anchor_name/
    void add_anchor_name(std::string&& anchor_name)
    {
        m_prop.anchor_status = detail::anchor_status_t::ANCHOR;
        m_prop.anchor = std::move(anchor_name);
    }

    /// @brief Get the node value object converted into a given type.
    /// @note This function requires T objects to be default constructible.
    /// @tparam T A compatible value type which might be cv-qualified or a reference type.
    /// @tparam ValueType A compatible value type, without cv-qualifiers and reference by default.
    /// @return A compatible native data value converted from the basic_node object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_value/
    template <
        typename T, typename ValueType = detail::remove_cvref_t<T>,
        detail::enable_if_t<
            detail::conjunction<
                std::is_default_constructible<ValueType>, detail::has_from_node<basic_node, ValueType>>::value,
            int> = 0>
    T get_value() const noexcept(
        noexcept(ConverterType<ValueType>::from_node(std::declval<const basic_node&>(), std::declval<ValueType&>())))
    {
        auto ret = ValueType();
        ConverterType<ValueType>::from_node(*this, ret);
        return ret;
    }

    /// @brief Explicit reference access to the internally stored YAML node value.
    /// @tparam ReferenceType Reference type to the target YAML node value.
    /// @return Reference to the internally stored YAML node value.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_value_ref/
    template <typename ReferenceType, detail::enable_if_t<std::is_reference<ReferenceType>::value, int> = 0>
    ReferenceType get_value_ref()
    {
        return get_value_ref_impl(static_cast<detail::add_pointer_t<ReferenceType>>(nullptr));
    }

    /// @brief Explicit reference access to the internally stored YAML node value.
    /// @tparam ReferenceType Constant reference type to the target YAML node value.
    /// @return Constant reference to the internally stored YAML node value.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_value_ref/
    template <
        typename ReferenceType,
        detail::enable_if_t<
            detail::conjunction<
                std::is_reference<ReferenceType>, std::is_const<detail::remove_reference_t<ReferenceType>>>::value,
            int> = 0>
    ReferenceType get_value_ref() const
    {
        return get_value_ref_impl(static_cast<detail::add_pointer_t<ReferenceType>>(nullptr));
    }

    /// @brief Swaps the internally stored data with the specified basic_node object.
    /// @param[in] rhs A basic_node object to be swapped with.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/swap/
    void swap(basic_node& rhs) noexcept
    {
        using std::swap;
        swap(m_node_type, rhs.m_node_type);
        swap(m_yaml_version_type, rhs.m_yaml_version_type);

        node_value tmp {};
        std::memcpy(&tmp, &m_node_value, sizeof(node_value));
        std::memcpy(&m_node_value, &rhs.m_node_value, sizeof(node_value));
        std::memcpy(&rhs.m_node_value, &tmp, sizeof(node_value));

        swap(m_prop.tag, rhs.m_prop.tag);
        swap(m_prop.anchor_status, rhs.m_prop.anchor_status);
        swap(m_prop.anchor, rhs.m_prop.anchor);
    }

    /// @brief Returns the first iterator of basic_node values of container types (sequence or mapping) from a non-const
    /// basic_node object. Throws exception if the basic_node value is not of container types.
    /// @return An iterator to the first element of a YAML node value (either sequence or mapping).
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/begin/
    iterator begin()
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return {detail::sequence_iterator_tag(), m_node_value.p_sequence->begin()};
        case node_t::MAPPING:
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            return {detail::mapping_iterator_tag(), m_node_value.p_mapping->begin()};
        default:
            throw fkyaml::type_error("The target node is neither of sequence nor mapping types.", m_node_type);
        }
    }

    /// @brief Returns the first iterator of basic_node values of container types (sequence or mapping) from a const
    /// basic_node object. Throws exception if the basic_node value is not of container types.
    /// @return A constant iterator to the first element of a YAML node value (either sequence or mapping).
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/begin/
    const_iterator begin() const
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return {detail::sequence_iterator_tag(), m_node_value.p_sequence->begin()};
        case node_t::MAPPING:
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            return {detail::mapping_iterator_tag(), m_node_value.p_mapping->begin()};
        default:
            throw fkyaml::type_error("The target node is neither of sequence nor mapping types.", m_node_type);
        }
    }

    /// @brief Returns the last iterator of basic_node values of container types (sequence or mapping) from a non-const
    /// basic_node object. Throws exception if the basic_node value is not of container types.
    /// @return An iterator to the past-the end element of a YAML node value (either sequence or mapping).
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/end/
    iterator end()
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return {detail::sequence_iterator_tag(), m_node_value.p_sequence->end()};
        case node_t::MAPPING:
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            return {detail::mapping_iterator_tag(), m_node_value.p_mapping->end()};
        default:
            throw fkyaml::type_error("The target node is neither of sequence nor mapping types.", m_node_type);
        }
    }

    /// @brief Returns the last iterator of basic_node values of container types (sequence or mapping) from a const
    /// basic_node object. Throws exception if the basic_node value is not of container types.
    /// @return A constant iterator to the past-the end element of a YAML node value (either sequence or mapping).
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/end/
    const_iterator end() const
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return {detail::sequence_iterator_tag(), m_node_value.p_sequence->end()};
        case node_t::MAPPING:
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            return {detail::mapping_iterator_tag(), m_node_value.p_mapping->end()};
        default:
            throw fkyaml::type_error("The target node is neither of sequence nor mapping types.", m_node_type);
        }
    }

private:
    /// @brief Returns reference to the sequence node value.
    /// @throw fkyaml::exception The node value is not a sequence.
    /// @return Reference to the sequence node value.
    sequence_type& get_value_ref_impl(sequence_type* /*unused*/)
    {
        if (!is_sequence())
        {
            throw fkyaml::type_error("The node value is not a sequence.", m_node_type);
        }
        return *(m_node_value.p_sequence);
    }

    /// @brief Returns constant reference to the sequence node value.
    /// @throw fkyaml::exception The node value is not a sequence.
    /// @return Constant reference to the sequence node value.
    const sequence_type& get_value_ref_impl(const sequence_type* /*unused*/) const
    {
        if (!is_sequence())
        {
            throw fkyaml::type_error("The node value is not a sequence.", m_node_type);
        }
        return *(m_node_value.p_sequence);
    }

    /// @brief Returns reference to the mapping node value.
    /// @throw fkyaml::exception The node value is not a mapping.
    /// @return Reference to the mapping node value.
    mapping_type& get_value_ref_impl(mapping_type* /*unused*/)
    {
        if (!is_mapping())
        {
            throw fkyaml::type_error("The node value is not a mapping.", m_node_type);
        }
        return *(m_node_value.p_mapping);
    }

    /// @brief Returns constant reference to the mapping node value.
    /// @throw fkyaml::exception The node value is not a mapping.
    /// @return Constant reference to the mapping node value.
    const mapping_type& get_value_ref_impl(const mapping_type* /*unused*/) const
    {
        if (!is_mapping())
        {
            throw fkyaml::type_error("The node value is not a mapping.", m_node_type);
        }
        return *(m_node_value.p_mapping);
    }

    /// @brief Returns reference to the boolean node value.
    /// @throw fkyaml::exception The node value is not a boolean.
    /// @return Reference to the boolean node value.
    boolean_type& get_value_ref_impl(boolean_type* /*unused*/)
    {
        if (!is_boolean())
        {
            throw fkyaml::type_error("The node value is not a boolean.", m_node_type);
        }
        return m_node_value.boolean;
    }

    /// @brief Returns reference to the boolean node value.
    /// @throw fkyaml::exception The node value is not a boolean.
    /// @return Constant reference to the boolean node value.
    const boolean_type& get_value_ref_impl(const boolean_type* /*unused*/) const
    {
        if (!is_boolean())
        {
            throw fkyaml::type_error("The node value is not a boolean.", m_node_type);
        }
        return m_node_value.boolean;
    }

    /// @brief Returns reference to the integer node value.
    /// @throw fkyaml::exception The node value is not an integer.
    /// @return Reference to the integer node value.
    integer_type& get_value_ref_impl(integer_type* /*unused*/)
    {
        if (!is_integer())
        {
            throw fkyaml::type_error("The node value is not an integer.", m_node_type);
        }
        return m_node_value.integer;
    }

    /// @brief Returns reference to the integer node value.
    /// @throw fkyaml::exception The node value is not an integer.
    /// @return Constant reference to the integer node value.
    const integer_type& get_value_ref_impl(const integer_type* /*unused*/) const
    {
        if (!is_integer())
        {
            throw fkyaml::type_error("The node value is not an integer.", m_node_type);
        }
        return m_node_value.integer;
    }

    /// @brief Returns reference to the floating point number node value.
    /// @throw fkyaml::exception The node value is not a floating point number.
    /// @return Reference to the floating point number node value.
    float_number_type& get_value_ref_impl(float_number_type* /*unused*/)
    {
        if (!is_float_number())
        {
            throw fkyaml::type_error("The node value is not a floating point number.", m_node_type);
        }
        return m_node_value.float_val;
    }

    /// @brief Returns reference to the floating point number node value.
    /// @throw fkyaml::exception The node value is not a floating point number.
    /// @return Constant reference to the floating point number node value.
    const float_number_type& get_value_ref_impl(const float_number_type* /*unused*/) const
    {
        if (!is_float_number())
        {
            throw fkyaml::type_error("The node value is not a floating point number.", m_node_type);
        }
        return m_node_value.float_val;
    }

    /// @brief Returns reference to the string node value.
    /// @throw fkyaml::exception The node value is not a string.
    /// @return Reference to the string node value.
    string_type& get_value_ref_impl(string_type* /*unused*/)
    {
        if (!is_string())
        {
            throw fkyaml::type_error("The node value is not a string.", m_node_type);
        }
        return *(m_node_value.p_string);
    }

    /// @brief Returns reference to the string node value.
    /// @throw fkyaml::exception The node value is not a string.
    /// @return Constant reference to the string node value.
    const string_type& get_value_ref_impl(const string_type* /*unused*/) const
    {
        if (!is_string())
        {
            throw fkyaml::type_error("The node value is not a string.", m_node_type);
        }
        return *(m_node_value.p_string);
    }

    /// The current node value type.
    node_t m_node_type {node_t::NULL_OBJECT};
    /// The YAML version specification.
    yaml_version_t m_yaml_version_type {yaml_version_t::VER_1_2};
    /// The current node value.
    node_value m_node_value {};
    /// The property set of this node.
    detail::node_property m_prop {};
};

/// @brief Swap function for basic_node objects.
/// @param[in] lhs A left-side-hand basic_node object to be swapped with.
/// @param[in] rhs A right-side-hand basic_node object to be swapped with.
/// @sa https://fktn-k.github.io/fkYAML/api/swap/
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType,
    template <typename, typename = void> class ConverterType>
inline void swap(
    basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>& lhs,
    basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>&
        rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

/// @brief Insertion operator for basic_node template class. A wrapper for the serialization feature.
/// @param[in] os An output stream object.
/// @param[in] n A basic_node object.
/// @return Reference to the output stream object `os`.
/// @sa https://fktn-k.github.io/fkYAML/api/basic_node/insertion_operator/
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType,
    template <typename, typename = void> class ConverterType>
inline std::ostream& operator<<(
    std::ostream& os,
    const basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>&
        n)
{
    os << basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>::
            serialize(n);
    return os;
}

/// @brief Extraction operator for basic_node template class. A wrapper for the deserialization feature with input
/// streams.
/// @param[in] is An input stream object.
/// @param[in] n A basic_node object.
/// @return Reference to the input stream object `is`.
/// @sa https://fktn-k.github.io/fkYAML/api/basic_node/extraction_operator/
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType,
    template <typename, typename = void> class ConverterType>
inline std::istream& operator>>(
    std::istream& is,
    basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>& n)
{
    n = basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>::
        deserialize(is);
    return is;
}

/// @brief default YAML node value container.
/// @sa https://fktn-k.github.io/fkYAML/api/basic_node/node/
using node = basic_node<>;

/// @brief namespace for user-defined literals for the fkYAML library.
inline namespace literals
{
/// @brief namespace for user-defined literals for YAML node objects.
inline namespace yaml_literals
{

/// @brief The user-defined string literal which deserializes a `char` array into a `node` object.
/// @param s An input `char` array.
/// @param n The size of `s`.
/// @return The resulting `node` object deserialized from `s`.
/// @sa https://fktn-k.github.io/fkYAML/api/operator_literal_yaml/
inline fkyaml::node operator"" _yaml(const char* s, std::size_t n)
{
    return fkyaml::node::deserialize((const char*)s, (const char*)s + n);
}

/// @brief The user-defined string literal which deserializes a `char16_t` array into a `node` object.
/// @param s An input `char16_t` array.
/// @param n The size of `s`.
/// @return The resulting `node` object deserialized from `s`.
/// @sa https://fktn-k.github.io/fkYAML/api/operator_literal_yaml/
inline fkyaml::node operator"" _yaml(const char16_t* s, std::size_t n)
{
    return fkyaml::node::deserialize((const char16_t*)s, (const char16_t*)s + n);
}

/// @brief The user-defined string literal which deserializes a `char32_t` array into a `node` object.
/// @param s An input `char32_t` array.
/// @param n The size of `s`.
/// @return The resulting `node` object deserialized from `s`.
/// @sa https://fktn-k.github.io/fkYAML/api/operator_literal_yaml/
inline fkyaml::node operator"" _yaml(const char32_t* s, std::size_t n)
{
    return fkyaml::node::deserialize((const char32_t*)s, (const char32_t*)s + n);
}

#ifdef FK_YAML_HAS_CHAR8_T
/// @brief The user-defined string literal which deserializes a `char8_t` array into a `node` object.
/// @param s An input `char8_t` array.
/// @param n The size of `s`.
/// @return The resulting `node` object deserialized from `s`.
inline fkyaml::node operator"" _yaml(const char8_t* s, std::size_t n)
{
    return fkyaml::node::deserialize((const char8_t*)s, (const char8_t*)s + n);
}
#endif

} // namespace yaml_literals
} // namespace literals

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_NODE_HPP_ */
