//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.13
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_CONVERSIONS_FROM_NODE_HPP
#define FK_YAML_DETAIL_CONVERSIONS_FROM_NODE_HPP

#include <cmath>
#include <limits>
#include <map>
#include <utility>
#include <vector>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>
#include <fkYAML/detail/types/node_t.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

///////////////////
//   from_node   //
///////////////////

/// @brief from_node function for BasicNodeType::sequence_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param s A sequence node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, typename BasicNodeType::sequence_type& s) {
    if FK_YAML_UNLIKELY (!n.is_sequence()) {
        throw type_error("The target node value type is not sequence type.", n.get_type());
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
inline void from_node(const BasicNodeType& n, std::vector<CompatibleValueType>& s) {
    if FK_YAML_UNLIKELY (!n.is_sequence()) {
        throw type_error("The target node value is not sequence type.", n.get_type());
    }

    s.reserve(n.size());

    for (const auto& elem : n) {
        s.emplace_back(elem.template get_value<CompatibleValueType>());
    }
}

/// @brief from_node function for BasicNodeType::mapping_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param m A mapping node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, typename BasicNodeType::mapping_type& m) {
    if FK_YAML_UNLIKELY (!n.is_mapping()) {
        throw type_error("The target node value type is not mapping type.", n.get_type());
    }

    for (auto pair : n.template get_value_ref<const typename BasicNodeType::mapping_type&>()) {
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
inline void from_node(const BasicNodeType& n, std::map<CompatibleKeyType, CompatibleValueType, Compare, Allocator>& m) {
    if FK_YAML_UNLIKELY (!n.is_mapping()) {
        throw type_error("The target node value type is not mapping type.", n.get_type());
    }

    for (auto pair : n.template get_value_ref<const typename BasicNodeType::mapping_type&>()) {
        m.emplace(
            pair.first.template get_value<CompatibleKeyType>(), pair.second.template get_value<CompatibleValueType>());
    }
}

/// @brief from_node function for null node values.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param null A null node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, std::nullptr_t& null) {
    // to ensure the target node value type is null.
    if FK_YAML_UNLIKELY (!n.is_null()) {
        throw type_error("The target node value type is not null type.", n.get_type());
    }
    null = nullptr;
}

/// @brief from_node function for BasicNodeType::boolean_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param b A boolean node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, typename BasicNodeType::boolean_type& b) {
    if FK_YAML_UNLIKELY (!n.is_boolean()) {
        throw type_error("The target node value type is not boolean type.", n.get_type());
    }
    b = n.template get_value_ref<const typename BasicNodeType::boolean_type&>();
}

/// @brief from_node function for BasicNodeType::integer_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param i An integer node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, typename BasicNodeType::integer_type& i) {
    if FK_YAML_UNLIKELY (!n.is_integer()) {
        throw type_error("The target node value type is not integer type.", n.get_type());
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
inline void from_node(const BasicNodeType& n, IntegerType& i) {
    if FK_YAML_UNLIKELY (!n.is_integer()) {
        throw type_error("The target node value type is not integer type.", n.get_type());
    }

    // under/overflow check.
    using node_int_type = typename BasicNodeType::integer_type;
    node_int_type tmp_int = n.template get_value_ref<const node_int_type&>();
    if FK_YAML_UNLIKELY (tmp_int < static_cast<node_int_type>(std::numeric_limits<IntegerType>::min())) {
        throw exception("Integer value underflow detected.");
    }
    if FK_YAML_UNLIKELY (static_cast<node_int_type>(std::numeric_limits<IntegerType>::max()) < tmp_int) {
        throw exception("Integer value overflow detected.");
    }

    i = static_cast<IntegerType>(tmp_int);
}

/// @brief from_node function for BasicNodeType::float_number_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param f A float number node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, typename BasicNodeType::float_number_type& f) {
    if FK_YAML_UNLIKELY (!n.is_float_number()) {
        throw type_error("The target node value type is not float number type.", n.get_type());
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
inline void from_node(const BasicNodeType& n, FloatType& f) {
    if FK_YAML_UNLIKELY (!n.is_float_number()) {
        throw type_error("The target node value type is not float number type.", n.get_type());
    }

    using node_float_type = typename BasicNodeType::float_number_type;
    auto tmp_float = n.template get_value_ref<const node_float_type&>();

    // check if the value is an infinite number (either positive or negative)
    if (std::isinf(tmp_float)) {
        if (tmp_float == std::numeric_limits<node_float_type>::infinity()) {
            f = std::numeric_limits<FloatType>::infinity();
            return;
        }

        f = -1 * std::numeric_limits<FloatType>::infinity();
        return;
    }

    // check if the value is not a number
    if (std::isnan(tmp_float)) {
        f = std::numeric_limits<FloatType>::quiet_NaN();
        return;
    }

    // check if the value is expressible as FloatType.
    if FK_YAML_UNLIKELY (tmp_float < std::numeric_limits<FloatType>::lowest()) {
        throw exception("Floating point value underflow detected.");
    }
    if FK_YAML_UNLIKELY (std::numeric_limits<FloatType>::max() < tmp_float) {
        throw exception("Floating point value overflow detected.");
    }

    f = static_cast<FloatType>(tmp_float);
}

/// @brief from_node function for BasicNodeType::string_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param s A string node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void from_node(const BasicNodeType& n, typename BasicNodeType::string_type& s) {
    if FK_YAML_UNLIKELY (!n.is_string()) {
        throw type_error("The target node value type is not string type.", n.get_type());
    }
    s = n.template get_value_ref<const typename BasicNodeType::string_type&>();
}

/// @brief from_node function for compatible string type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam CompatibleStringType A compatible string type.
/// @param n A basic_node object.
/// @param s A compatible string object.
template <
    typename BasicNodeType, typename CompatibleStringType,
    enable_if_t<
        conjunction<
            is_basic_node<BasicNodeType>,
            negation<std::is_same<CompatibleStringType, typename BasicNodeType::string_type>>,
            disjunction<
                std::is_constructible<CompatibleStringType, const typename BasicNodeType::string_type&>,
                std::is_assignable<CompatibleStringType, const typename BasicNodeType::string_type&>>>::value,
        int> = 0>
inline void from_node(const BasicNodeType& n, CompatibleStringType& s) {
    if FK_YAML_UNLIKELY (!n.is_string()) {
        throw type_error("The target node value type is not string type.", n.get_type());
    }
    s = n.template get_value_ref<const typename BasicNodeType::string_type&>();
}

/// @brief A function object to call from_node functions.
/// @note User-defined specialization is available by providing implementation **OUTSIDE** fkyaml namespace.
struct from_node_fn {
    /// @brief Call from_node function suitable for the given T type.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @tparam T A target value type assigned from the basic_node object.
    /// @param n A basic_node object.
    /// @param val A target object assigned from the basic_node object.
    /// @return decltype(from_node(n, std::forward<T>(val))) void by default. User can set it to some other type.
    template <typename BasicNodeType, typename T>
    auto operator()(const BasicNodeType& n, T&& val) const noexcept(noexcept(from_node(n, std::forward<T>(val))))
        -> decltype(from_node(n, std::forward<T>(val))) {
        return from_node(n, std::forward<T>(val));
    }
};

FK_YAML_DETAIL_NAMESPACE_END

FK_YAML_NAMESPACE_BEGIN

#ifndef FK_YAML_HAS_CXX_17
// anonymous namespace to hold `from_node` functor.
// see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html for why it's needed.
namespace // NOLINT(cert-dcl59-cpp,fuchsia-header-anon-namespaces,google-build-namespaces)
{
#endif

/// @brief A global object to represent ADL friendly from_node functor.
// NOLINTNEXTLINE(misc-definitions-in-headers)
FK_YAML_INLINE_VAR constexpr const auto& from_node = detail::static_const<detail::from_node_fn>::value;

#ifndef FK_YAML_HAS_CXX_17
} // namespace
#endif

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_CONVERSIONS_FROM_NODE_HPP */
