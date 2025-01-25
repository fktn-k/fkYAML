//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_META_STL_SUPPLEMENT_HPP
#define FK_YAML_DETAIL_META_STL_SUPPLEMENT_HPP

#include <cstddef>
#include <type_traits>

#include <fkYAML/detail/macros/define_macros.hpp>

#ifdef FK_YAML_HAS_CXX_14
#include <utility>
#endif

FK_YAML_DETAIL_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//   For contributors:
//     This file is for supplementing future C++ STL implementations to utilize some useful features
//     implemented in C++14 or better.
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
struct is_null_pointer : std::is_same<std::nullptr_t, typename std::remove_cv<T>::type> {};

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

template <typename T, T... I>
struct integer_sequence {
    using value_type = T;
    static constexpr std::size_t size() noexcept {
        return sizeof...(I);
    }
};

#if !FK_YAML_HAS_BUILTIN(__make_integer_seq) && !FK_YAML_HAS_BUILTIN(__integer_pack)

namespace make_int_seq_impl {

template <typename IntSeq0, typename IntSeq1>
struct merger;

template <typename T, T... Ints0, T... Ints1>
struct merger<integer_sequence<T, Ints0...>, integer_sequence<T, Ints1...>> {
    using type = integer_sequence<T, Ints0..., (sizeof...(Ints0) + Ints1)...>;
};

template <typename T, std::size_t Num>
struct generator {
    using type =
        typename merger<typename generator<T, Num / 2>::type, typename generator<T, Num - Num / 2>::type>::type;
};

template <typename T>
struct generator<T, 0> {
    using type = integer_sequence<T>;
};

template <typename T>
struct generator<T, 1> {
    using type = integer_sequence<T, 0>;
};

} // namespace make_int_seq_impl

#endif

template <typename T, T Num>
using make_integer_sequence
#if FK_YAML_HAS_BUILTIN(__make_integer_seq)
    // clang defines built-in __make_integer_seq to generate an integer sequence.
    = __make_integer_seq<integer_sequence, T, Num>;
#elif FK_YAML_HAS_BUILTIN(__integer_pack)
    // GCC or other compilers may implement built-in __integer_pack to generate an
    // integer sequence.
    = integer_sequence<T, __integer_pack(Num)...>;
#else
    // fallback to the library implementation of make_integer_sequence.
    = typename make_int_seq_impl::generator<T, Num>::type;
#endif

template <std::size_t... Idx>
using index_sequence = integer_sequence<std::size_t, Idx...>;

template <std::size_t Num>
using make_index_sequence = make_integer_sequence<std::size_t, Num>;

template <typename... Types>
using index_sequence_for = make_index_sequence<sizeof...(Types)>;

#else // !defined(FK_YAML_HAS_CXX_14)

using std::add_pointer_t;
using std::enable_if_t;
using std::index_sequence;
using std::index_sequence_for;
using std::integer_sequence;
using std::is_null_pointer;
using std::make_index_sequence;
using std::make_integer_sequence;
using std::remove_cv_t;
using std::remove_pointer_t;
using std::remove_reference_t;

#endif // !defined(FK_YAML_HAS_CXX_14)

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
struct conjunction : std::true_type {};

/// @brief A partial specialization of conjunction if only one Trait is given.
/// @tparam Trait Type trait to be checked if its ::value is true.
template <typename Trait>
struct conjunction<Trait> : Trait {};

/// @brief A partial specialization of conjunction if more than one traits are given.
/// @tparam First The first type trait to be checked if its ::value is true.
/// @tparam Rest The rest of traits passed as another conjunction template arguments if First::value is true.
template <typename First, typename... Rest>
struct conjunction<First, Rest...> : std::conditional<First::value, conjunction<Rest...>, First>::type {};

/// @brief A simple implementation to use std::disjunction with C++11/C++14.
/// @note
/// std::disjunction is available since C++17.
/// This is applied when no traits are specified as inputs.
/// @sa https://en.cppreference.com/w/cpp/types/disjunction
/// @tparam Traits Type traits to be checked if at least one of their ::value is true.
template <typename... Traits>
struct disjunction : std::false_type {};

/// @brief A partial specialization of disjunction if only one Trait is given.
/// @tparam Trait Type trait to be checked if its ::value is true.
template <typename Trait>
struct disjunction<Trait> : Trait {};

/// @brief A partial specialization of disjunction if more than one traits are given.
/// @tparam First The first type trait to be checked if its ::value is true.
/// @tparam Rest The rest of traits passed as another conjunction template arguments if First::value is false.
template <typename First, typename... Rest>
struct disjunction<First, Rest...> : std::conditional<First::value, First, disjunction<Rest...>>::type {};

/// @brief A simple implementation to use std::negation with C++11/C++14.
/// @note std::negation is available since C++17.
/// @sa https://en.cppreference.com/w/cpp/types/negation
/// @tparam Trait Type trait whose ::value is negated.
template <typename Trait>
struct negation : std::integral_constant<bool, !Trait::value> {};

/// @brief A helper for void_t.
/// @tparam Types Any types to be transformed to void type.
template <typename... Types>
struct make_void {
    using type = void;
};

/// @brief A simple implementation to use std::void_t with C++11/C++14.
/// @note std::void_t is available since C++17.
/// @sa https://en.cppreference.com/w/cpp/types/void_t
/// @tparam Types Any types to be transformed to void type.
template <typename... Types>
using void_t = typename make_void<Types...>::type;

#else // !defined(FK_YAML_HAS_CXX_17)

using std::bool_constant;
using std::conjunction;
using std::disjunction;
using std::negation;
using std::void_t;

#endif // !defined(FK_YAML_HAS_CXX_17)

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

/// @brief A wrapper function to call std::unreachable() (since C++23) or similar compiler specific extensions.
/// @note This function is implemented only for better code optimization against dead code and thus excluded from
/// coverage report.
// LCOV_EXCL_START
[[noreturn]] inline void unreachable() {
    // use compiler specific extensions if possible.
    // undefined behavior should be raised by an empty function with noreturn attribute.

#if defined(FK_YAML_HAS_CXX_23) || (defined(__cpp_lib_unreachable) && __cpp_lib_unreachable >= 202202L)
    std::unreachable();
#elif defined(_MSC_VER) && !defined(__clang__) // MSVC
    __assume(false);
#else
    __builtin_unreachable();
#endif
}
// LCOV_EXCL_STOP

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_META_STL_SUPPLEMENT_HPP */
