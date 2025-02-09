//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_CONVERSIONS_TO_NODE_HPP
#define FK_YAML_DETAIL_CONVERSIONS_TO_NODE_HPP

#include <utility>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/exception_safe_allocation.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/node_attrs.hpp>
#include <fkYAML/node_type.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

///////////////////////////////////
//   external_node_constructor   //
///////////////////////////////////

/// @brief The external constructor template for basic_node objects.
/// @note All the non-specialized instantiations results in compilation error since such instantiations are not
/// supported.
/// @warning All the specialization must call n.m_value.destroy() first in the construct function to avoid
/// memory leak.
/// @tparam node_type The resulting YAML node value type.
template <typename BasicNodeType>
struct external_node_constructor {
    template <typename... Args>
    static void sequence(BasicNodeType& n, Args&&... args) {
        destroy(n);
        n.m_attrs |= node_attr_bits::seq_bit;
        n.m_value.p_seq = create_object<typename BasicNodeType::sequence_type>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void mapping(BasicNodeType& n, Args&&... args) {
        destroy(n);
        n.m_attrs |= node_attr_bits::map_bit;
        n.m_value.p_map = create_object<typename BasicNodeType::mapping_type>(std::forward<Args>(args)...);
    }

    static void null_scalar(BasicNodeType& n, std::nullptr_t) {
        destroy(n);
        n.m_attrs |= node_attr_bits::null_bit;
        n.m_value.p_map = nullptr;
    }

    static void boolean_scalar(BasicNodeType& n, const typename BasicNodeType::boolean_type b) {
        destroy(n);
        n.m_attrs |= node_attr_bits::bool_bit;
        n.m_value.boolean = b;
    }

    static void integer_scalar(BasicNodeType& n, const typename BasicNodeType::integer_type i) {
        destroy(n);
        n.m_attrs |= node_attr_bits::int_bit;
        n.m_value.integer = i;
    }

    static void float_scalar(BasicNodeType& n, const typename BasicNodeType::float_number_type f) {
        destroy(n);
        n.m_attrs |= node_attr_bits::float_bit;
        n.m_value.float_val = f;
    }

    template <typename... Args>
    static void string_scalar(BasicNodeType& n, Args&&... args) {
        destroy(n);
        n.m_attrs |= node_attr_bits::string_bit;
        n.m_value.p_str = create_object<typename BasicNodeType::string_type>(std::forward<Args>(args)...);
    }

private:
    static void destroy(BasicNodeType& n) {
        n.m_value.destroy(n.m_attrs & node_attr_mask::value);
        n.m_attrs &= ~node_attr_mask::value;
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
inline void to_node(BasicNodeType& n, T&& s) noexcept {
    external_node_constructor<BasicNodeType>::sequence(n, std::forward<T>(s));
}

/// @brief to_node function for compatible sequence types.
/// @note This overload is enabled when
/// * both begin()/end() functions are callable on a `CompatSeqType` object
/// * CompatSeqType doesn't have `mapped_type` (mapping-like type)
/// * BasicNodeType::string_type cannot be constructed from a CompatSeqType object (string-like type)
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam CompatSeqType A container type.
/// @param n A basic_node object.
/// @param s A container object.
template <
    typename BasicNodeType, typename CompatSeqType,
    enable_if_t<
        conjunction<
            is_basic_node<BasicNodeType>,
            negation<std::is_same<typename BasicNodeType::sequence_type, remove_cvref_t<CompatSeqType>>>,
            negation<is_basic_node<CompatSeqType>>, detect::has_begin_end<CompatSeqType>,
            negation<conjunction<detect::has_key_type<CompatSeqType>, detect::has_mapped_type<CompatSeqType>>>,
            negation<std::is_constructible<typename BasicNodeType::string_type, CompatSeqType>>>::value,
        int> = 0>
// NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
inline void to_node(BasicNodeType& n, CompatSeqType&& s) {
    using std::begin;
    using std::end;
    external_node_constructor<BasicNodeType>::sequence(n, begin(s), end(s));
}

/// @brief to_node function for std::pair objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T The first type of std::pair.
/// @tparam U The second type of std::pair.
/// @param n A basic_node object.
/// @param p A std::pair object.
template <typename BasicNodeType, typename T, typename U>
inline void to_node(BasicNodeType& n, const std::pair<T, U>& p) {
    n = {p.first, p.second};
}

/// @brief concrete implementation of to_node function for std::tuple objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam ...Types The value types of std::tuple.
/// @tparam ...Idx Index sequence values for std::tuple value types.
/// @param n A basic_node object.
/// @param t A std::tuple object.
/// @param _ An index sequence. (unused)
template <typename BasicNodeType, typename... Types, std::size_t... Idx>
inline void to_node_tuple_impl(BasicNodeType& n, const std::tuple<Types...>& t, index_sequence<Idx...> /*unused*/) {
    n = {std::get<Idx>(t)...};
}

/// @brief to_node function for std::tuple objects with no value types.
/// @note This implementation is needed since calling `to_node_tuple_impl()` with an empty tuple creates a null node.
/// @tparam BasicNodeType A basic_node template instance type.
/// @param n A basic_node object.
/// @param _ A std::tuple object. (unused)
template <typename BasicNodeType>
inline void to_node(BasicNodeType& n, const std::tuple<>& /*unused*/) {
    n = BasicNodeType::sequence();
}

/// @brief to_node function for std::tuple objects with at least one value type.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam ...FirstType The first value types of std::tuple.
/// @tparam ...RestTypes The rest value types of std::tuple. (maybe empty)
/// @param n A basic_node object.
/// @param t A std::tuple object.
template <typename BasicNodeType, typename FirstType, typename... RestTypes>
inline void to_node(BasicNodeType& n, const std::tuple<FirstType, RestTypes...>& t) {
    to_node_tuple_impl(n, t, index_sequence_for<FirstType, RestTypes...> {});
}

/// @brief to_node function for BasicNodeType::mapping_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A mapping node value type.
/// @param n A basic_node object.
/// @param m A mapping node value object.
template <
    typename BasicNodeType, typename T,
    enable_if_t<
        conjunction<
            is_basic_node<BasicNodeType>, std::is_same<typename BasicNodeType::mapping_type, remove_cvref_t<T>>>::value,
        int> = 0>
inline void to_node(BasicNodeType& n, T&& m) noexcept {
    external_node_constructor<BasicNodeType>::mapping(n, std::forward<T>(m));
}

/// @brief to_node function for compatible mapping types.
/// @note This overload is enabled when
/// * both begin()/end() functions are callable on a `CompatMapType` object
/// * CompatMapType has both `key_type` and `mapped_type`
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam CompatMapType A container type.
/// @param n A basic_node object.
/// @param m A container object.
template <
    typename BasicNodeType, typename CompatMapType,
    enable_if_t<
        conjunction<
            is_basic_node<BasicNodeType>, negation<is_basic_node<CompatMapType>>,
            negation<std::is_same<typename BasicNodeType::mapping_type, remove_cvref_t<CompatMapType>>>,
            detect::has_begin_end<CompatMapType>, detect::has_key_type<CompatMapType>,
            detect::has_mapped_type<CompatMapType>>::value,
        int> = 0>
inline void to_node(BasicNodeType& n, CompatMapType&& m) {
    external_node_constructor<BasicNodeType>::mapping(n);
    auto& map = n.as_map();
    for (const auto& pair : std::forward<CompatMapType>(m)) {
        map.emplace(pair.first, pair.second);
    }
}

/// @brief to_node function for null objects.
/// @tparam BasicNodeType A mapping node value type.
/// @tparam NullType This must be std::nullptr_t type
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void to_node(BasicNodeType& n, std::nullptr_t /*unused*/) {
    external_node_constructor<BasicNodeType>::null_scalar(n, nullptr);
}

/// @brief to_node function for BasicNodeType::boolean_type objects.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A boolean scalar node value type.
/// @param n A basic_node object.
/// @param b A boolean scalar node value object.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void to_node(BasicNodeType& n, typename BasicNodeType::boolean_type b) noexcept {
    external_node_constructor<BasicNodeType>::boolean_scalar(n, b);
}

/// @brief to_node function for integers.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T An integer type.
/// @param n A basic_node object.
/// @param i An integer object.
template <
    typename BasicNodeType, typename T,
    enable_if_t<conjunction<is_basic_node<BasicNodeType>, is_non_bool_integral<T>>::value, int> = 0>
inline void to_node(BasicNodeType& n, T i) noexcept {
    external_node_constructor<BasicNodeType>::integer_scalar(n, i);
}

/// @brief to_node function for floating point numbers.
/// @tparam BasicNodeType A basic_node template instance type.
/// @tparam T A floating point number type.
/// @param n A basic_node object.
/// @param f A floating point number object.
template <
    typename BasicNodeType, typename T,
    enable_if_t<conjunction<is_basic_node<BasicNodeType>, std::is_floating_point<T>>::value, int> = 0>
inline void to_node(BasicNodeType& n, T f) noexcept {
    external_node_constructor<BasicNodeType>::float_scalar(n, f);
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
            std::is_constructible<typename BasicNodeType::string_type, T>>::value,
        int> = 0>
inline void to_node(BasicNodeType& n, T&& s) {
    external_node_constructor<BasicNodeType>::string_scalar(n, std::forward<T>(s));
}

/// @brief A function object to call to_node functions.
/// @note User-defined specialization is available by providing implementation **OUTSIDE** fkyaml namespace.
struct to_node_fn {
    /// @brief Call to_node function suitable for the given T type.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @tparam T A target value type assigned to the basic_node object.
    /// @param n A basic_node object.
    /// @param val A target object assigned to the basic_node object.
    /// @return decltype(to_node(n, std::forward<T>(val))) void by default. User can set it to some other type.
    template <typename BasicNodeType, typename T>
    auto operator()(BasicNodeType& n, T&& val) const
        noexcept(noexcept(to_node(n, std::forward<T>(val)))) -> decltype(to_node(n, std::forward<T>(val))) {
        return to_node(n, std::forward<T>(val));
    }
};

FK_YAML_DETAIL_NAMESPACE_END

FK_YAML_NAMESPACE_BEGIN

#ifndef FK_YAML_HAS_CXX_17
// anonymous namespace to hold `to_node` functor.
// see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html for why it's needed.
namespace // NOLINT(cert-dcl59-cpp,fuchsia-header-anon-namespaces,google-build-namespaces)
{
#endif

/// @brief A global object to represent ADL friendly to_node functor.
// NOLINTNEXTLINE(misc-definitions-in-headers)
FK_YAML_INLINE_VAR constexpr const auto& to_node = detail::static_const<detail::to_node_fn>::value;

#ifndef FK_YAML_HAS_CXX_17
} // namespace
#endif

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_CONVERSIONS_TO_NODE_HPP */
