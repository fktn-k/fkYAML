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

#ifndef FK_YAML_TO_NODE_HPP_
#define FK_YAML_TO_NODE_HPP_

#include <utility>

#include "fkYAML/version_macros.hpp"
#include "fkYAML/node_t.hpp"
#include "fkYAML/type_traits.hpp"

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

///////////////////////////////////
//   external_node_constructor   //
///////////////////////////////////

/**
 * @struct external_node_constructor
 * @brief The external constructor template for basic_node objects.
 * @note All the non-specialized instanciations results in compilation error since such instantiations are not
 * supported.
 * @warning All the specialization must call n.m_node_value.destroy(n.m_node_type) first in construct function to avoid
 * memory leak.
 *
 * @tparam fkyaml::node_t The resulting YAMK node value type.
 */
template <fkyaml::node_t>
struct external_node_constructor;

/**
 * @brief The specialization of external_node_constructor for sequence nodes.
 *
 * @tparam N/A
 */
template <>
struct external_node_constructor<fkyaml::node_t::SEQUENCE>
{
    template <typename BasicNodeType, fkyaml::enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, const typename BasicNodeType::sequence_type& s) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::SEQUENCE;
        n.m_node_value.p_sequence = BasicNodeType::template create_object<typename BasicNodeType::sequence_type>(s);
    }

    template <typename BasicNodeType, fkyaml::enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::sequence_type&& s) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::SEQUENCE;
        n.m_node_value.p_sequence =
            BasicNodeType::template create_object<typename BasicNodeType::sequence_type>(std::move(s));
    }
};

/**
 * @brief The specialization of external_node_constructor for mapping nodes.
 *
 * @tparam N/A
 */
template <>
struct external_node_constructor<fkyaml::node_t::MAPPING>
{
    template <typename BasicNodeType, fkyaml::enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, const typename BasicNodeType::mapping_type& m) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::MAPPING;
        n.m_node_value.p_mapping = BasicNodeType::template create_object<typename BasicNodeType::mapping_type>(m);
    }

    template <typename BasicNodeType, fkyaml::enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::mapping_type&& m) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::MAPPING;
        n.m_node_value.p_mapping =
            BasicNodeType::template create_object<typename BasicNodeType::mapping_type>(std::move(m));
    }
};

/**
 * @brief The specialization of external_node_constructor for null nodes.
 *
 * @tparam N/A
 */
template <>
struct external_node_constructor<fkyaml::node_t::NULL_OBJECT>
{
    template <typename BasicNodeType, fkyaml::enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, std::nullptr_t /*unused*/) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::NULL_OBJECT;
        n.m_node_value.p_mapping = nullptr;
    }
};

/**
 * @brief The specialization of external_node_constructor for boolean scalar nodes.
 *
 * @tparam N/A
 */
template <>
struct external_node_constructor<fkyaml::node_t::BOOLEAN>
{
    template <typename BasicNodeType, fkyaml::enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::boolean_type b) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::BOOLEAN;
        n.m_node_value.boolean = b;
    }
};

/**
 * @brief The specialization of external_node_constructor for integer scalar nodes.
 *
 * @tparam N/A
 */
template <>
struct external_node_constructor<fkyaml::node_t::INTEGER>
{
    template <typename BasicNodeType, fkyaml::enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::integer_type i) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::INTEGER;
        n.m_node_value.integer = i;
    }
};

/**
 * @brief The specialization of external_node_constructor for float number scalar nodes.
 *
 * @tparam N/A
 */
template <>
struct external_node_constructor<fkyaml::node_t::FLOAT_NUMBER>
{
    template <typename BasicNodeType, fkyaml::enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::float_number_type f) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::FLOAT_NUMBER;
        n.m_node_value.float_val = f;
    }
};

/**
 * @brief The specialization of external_node_constructor for string scalar nodes.
 *
 * @tparam N/A
 */
template <>
struct external_node_constructor<fkyaml::node_t::STRING>
{
    template <typename BasicNodeType, fkyaml::enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, const typename BasicNodeType::string_type& s) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::STRING;
        n.m_node_value.p_mapping = BasicNodeType::template create_object<typename BasicNodeType::string_type>(s);
    }

    template <typename BasicNodeType, fkyaml::enable_if_t<is_basic_node<BasicNodeType>::value, int> = 0>
    static void construct(BasicNodeType& n, typename BasicNodeType::string_type&& s) noexcept
    {
        n.m_node_value.destroy(n.m_node_type);
        n.m_node_type = node_t::STRING;
        n.m_node_value.p_mapping =
            BasicNodeType::template create_object<typename BasicNodeType::string_type>(std::move(s));
    }
};

/////////////////
//   to_node   //
/////////////////

template <
    typename BasicNodeType, typename T,
    fkyaml::enable_if_t<
        fkyaml::is_basic_node<BasicNodeType>::value &&
            std::is_same<typename BasicNodeType::sequence_type, fkyaml::remove_cvref_t<T>>::value,
        int> = 0>
inline void to_node(BasicNodeType& n, T&& s) noexcept
{
    external_node_constructor<fkyaml::node_t::SEQUENCE>::construct(n, std::forward<T>(s));
}

template <
    typename BasicNodeType, typename T,
    fkyaml::enable_if_t<
        fkyaml::is_basic_node<BasicNodeType>::value &&
            std::is_same<typename BasicNodeType::mapping_type, fkyaml::remove_cvref_t<T>>::value,
        int> = 0>
inline void to_node(BasicNodeType& n, T&& m) noexcept
{
    external_node_constructor<fkyaml::node_t::MAPPING>::construct(n, std::forward<T>(m));
}

template <
    typename BasicNodeType, typename T,
    fkyaml::enable_if_t<
        fkyaml::is_basic_node<BasicNodeType>::value && std::is_same<typename BasicNodeType::boolean_type, T>::value,
        int> = 0>
inline void to_node(BasicNodeType& n, T b) noexcept
{
    external_node_constructor<fkyaml::node_t::BOOLEAN>::construct(n, b);
}

template <
    typename BasicNodeType, typename T,
    fkyaml::enable_if_t<
        fkyaml::is_basic_node<BasicNodeType>::value &&
            fkyaml::is_compatible_integer_type<typename BasicNodeType::integer_type, T>::value,
        int> = 0>
inline void to_node(BasicNodeType& n, T i) noexcept
{
    external_node_constructor<fkyaml::node_t::INTEGER>::construct(n, i);
}

template <
    typename BasicNodeType, typename T,
    fkyaml::enable_if_t<fkyaml::is_basic_node<BasicNodeType>::value && std::is_floating_point<T>::value, int> = 0>
inline void to_node(BasicNodeType& n, T f) noexcept
{
    external_node_constructor<fkyaml::node_t::FLOAT_NUMBER>::construct(n, f);
}

struct to_node_fn
{
    template <typename BasicNodeType, typename T>
    auto operator()(BasicNodeType& n, T&& val) const noexcept(noexcept(to_node(n, std::forward<T>(val))))
        -> decltype(to_node(n, std::forward<T>(val)))
    {
        return to_node(n, std::forward<T>(val));
    }
};

} // namespace detail

#ifndef FK_YAML_HAS_CXX_17
// namespace to hold default `to_node` function.
// see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html for more details.
namespace // NOLINT(cert-dcl59-cpp,fuchsia-header-anon-namespaces,google-build-namespaces)
{
#endif

/**
 * @brief A blobal object to represent ADL friendly to_node function.
 */
// NOLINTNEXTLINE(misc-definitions-in-headers)
FK_YAML_INLINE_VAR constexpr const auto& to_node = detail::static_const<detail::to_node_fn>::value;

#ifndef FK_YAML_HAS_CXX_17
} // namespace
#endif

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_TO_NODE_HPP_ */