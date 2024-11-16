//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.14
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_CONVERSIONS_TO_NODE_HPP
#define FK_YAML_DETAIL_CONVERSIONS_TO_NODE_HPP

#include <utility>

#include <fkYAML/detail/macros/define_macros.hpp>
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
/// @warning All the specialization must call n.m_node_value.destroy() first in the construct function to avoid
/// memory leak.
/// @tparam node_type The resulting YAML node value type.
template <node_type>
struct external_node_constructor;

/// @brief The specialization of external_node_constructor for sequence nodes.
template <>
struct external_node_constructor<node_type::SEQUENCE> {
    /// @brief Constructs a basic_node object with const lvalue sequence.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param s A lvalue sequence value.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, const typename BasicNodeType::sequence_type& s) noexcept {
        n.m_node_value.destroy(n.m_attrs & detail::node_attr_mask::value);
        n.m_attrs = detail::node_attr_bits::seq_bit;
        n.m_node_value.p_sequence = BasicNodeType::template create_object<typename BasicNodeType::sequence_type>(s);
    }

    /// @brief Constructs a basic_node object with rvalue sequence.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param s A rvalue sequence value.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::sequence_type&& s) noexcept {
        n.m_node_value.destroy(n.m_attrs & detail::node_attr_mask::value);
        n.m_attrs = detail::node_attr_bits::seq_bit;
        n.m_node_value.p_sequence =
            BasicNodeType::template create_object<typename BasicNodeType::sequence_type>(std::move(s));
    }
};

/// @brief The specialization of external_node_constructor for mapping nodes.
template <>
struct external_node_constructor<node_type::MAPPING> {
    /// @brief Constructs a basic_node object with const lvalue mapping.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param m A lvalue mapping value.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, const typename BasicNodeType::mapping_type& m) noexcept {
        n.m_node_value.destroy(n.m_attrs & detail::node_attr_mask::value);
        n.m_attrs = detail::node_attr_bits::map_bit;
        n.m_node_value.p_mapping = BasicNodeType::template create_object<typename BasicNodeType::mapping_type>(m);
    }

    /// @brief Constructs a basic_node object with rvalue mapping.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param m A rvalue mapping value.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::mapping_type&& m) noexcept {
        n.m_node_value.destroy(n.m_attrs & detail::node_attr_mask::value);
        n.m_attrs = detail::node_attr_bits::map_bit;
        n.m_node_value.p_mapping =
            BasicNodeType::template create_object<typename BasicNodeType::mapping_type>(std::move(m));
    }
};

/// @brief The specialization of external_node_constructor for null nodes.
template <>
struct external_node_constructor<node_type::NULL_OBJECT> {
    /// @brief Constructs a basic_node object with nullptr.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param (unused) nullptr
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, std::nullptr_t /*unused*/) noexcept {
        n.m_node_value.destroy(n.m_attrs & detail::node_attr_mask::value);
        n.m_attrs = detail::node_attr_bits::null_bit;
        n.m_node_value.p_mapping = nullptr;
    }
};

/// @brief The specialization of external_node_constructor for boolean scalar nodes.
template <>
struct external_node_constructor<node_type::BOOLEAN> {
    /// @brief Constructs a basic_node object with boolean.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param b A boolean value.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::boolean_type b) noexcept {
        n.m_node_value.destroy(n.m_attrs & detail::node_attr_mask::value);
        n.m_attrs = detail::node_attr_bits::bool_bit;
        n.m_node_value.boolean = b;
    }
};

/// @brief The specialization of external_node_constructor for integer scalar nodes.
template <>
struct external_node_constructor<node_type::INTEGER> {
    /// @brief Constructs a basic_node object with integers.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param i An integer value.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::integer_type i) noexcept {
        n.m_node_value.destroy(n.m_attrs & detail::node_attr_mask::value);
        n.m_attrs = detail::node_attr_bits::int_bit;
        n.m_node_value.integer = i;
    }
};

/// @brief The specialization of external_node_constructor for float number scalar nodes.
template <>
struct external_node_constructor<node_type::FLOAT> {
    /// @brief Constructs a basic_node object with floating point numbers.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param f A floating point number.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::float_number_type f) noexcept {
        n.m_node_value.destroy(n.m_attrs & detail::node_attr_mask::value);
        n.m_attrs = detail::node_attr_bits::float_bit;
        n.m_node_value.float_val = f;
    }
};

/// @brief The specialization of external_node_constructor for string scalar nodes.
template <>
struct external_node_constructor<node_type::STRING> {
    /// @brief Constructs a basic_node object with const lvalue strings.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param s A constant lvalue string.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, const typename BasicNodeType::string_type& s) noexcept {
        n.m_node_value.destroy(n.m_attrs & detail::node_attr_mask::value);
        n.m_attrs = detail::node_attr_bits::string_bit;
        n.m_node_value.p_string = BasicNodeType::template create_object<typename BasicNodeType::string_type>(s);
    }

    /// @brief Constructs a basic_node object with rvalue strings.
    /// @tparam BasicNodeType A basic_node template instance type.
    /// @param n A basic_node object.
    /// @param s A rvalue string.
    template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::string_type&& s) noexcept {
        n.m_node_value.destroy(n.m_attrs & detail::node_attr_mask::value);
        n.m_attrs = detail::node_attr_bits::string_bit;
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
    static void construct(BasicNodeType& n, const CompatibleStringType& s) noexcept {
        n.m_node_value.destroy(n.m_attrs & detail::node_attr_mask::value);
        n.m_attrs = detail::node_attr_bits::string_bit;
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
inline void to_node(BasicNodeType& n, T&& s) noexcept {
    external_node_constructor<node_type::SEQUENCE>::construct(n, std::forward<T>(s));
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
    external_node_constructor<node_type::MAPPING>::construct(n, std::forward<T>(m));
}

/// @brief to_node function for null objects.
/// @tparam BasicNodeType A mapping node value type.
/// @tparam NullType This must be std::nullptr_t type
template <
    typename BasicNodeType, typename NullType,
    enable_if_t<conjunction<is_basic_node<BasicNodeType>, std::is_same<NullType, std::nullptr_t>>::value, int> = 0>
inline void to_node(BasicNodeType& n, NullType /*unused*/) {
    external_node_constructor<node_type::NULL_OBJECT>::construct(n, nullptr);
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
inline void to_node(BasicNodeType& n, T b) noexcept {
    external_node_constructor<node_type::BOOLEAN>::construct(n, b);
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
    external_node_constructor<node_type::INTEGER>::construct(n, i);
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
    external_node_constructor<node_type::FLOAT>::construct(n, f);
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
inline void to_node(BasicNodeType& n, const T& s) {
    external_node_constructor<node_type::STRING>::construct(n, s);
}

/// @brief to_node function for rvalue string node values
/// @tparam BasicNodeType A basic_node template instance type
/// @param n A basic_node object.
/// @param s An rvalue string node value.
template <typename BasicNodeType, enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
inline void to_node(BasicNodeType& n, typename BasicNodeType::string_type&& s) noexcept {
    external_node_constructor<node_type::STRING>::construct(n, std::move(s));
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
