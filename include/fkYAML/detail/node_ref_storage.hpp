//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_NODE_REF_STORAGE_HPP
#define FK_YAML_DETAIL_NODE_REF_STORAGE_HPP

#include <initializer_list>
#include <type_traits>
#include <utility>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief A temporal storage for basic_node class objects.
/// @note This class makes it easier to handle lvalue basic_node objects in basic_node ctor with std::initializer_list.
/// @tparam BasicNodeType A basic_node template instance type.
template <typename BasicNodeType>
class node_ref_storage {
    static_assert(is_basic_node<BasicNodeType>::value, "node_ref_storage only accepts basic_node<...>");

    using node_type = BasicNodeType;

    template <typename... Args>
    struct has_single_basic_node_arg : std::false_type {};

    template <typename Arg>
    struct has_single_basic_node_arg<Arg> : is_basic_node<Arg> {};

public:
    /// @brief Construct a new node ref storage object with an rvalue basic_node object.
    /// @param n An rvalue basic_node object.
    explicit node_ref_storage(node_type&& n) noexcept(std::is_nothrow_move_constructible<node_type>::value)
        : m_owned_value(std::move(n)),
          m_has_node_ref(true) {
    }

    /// @brief Construct a new node ref storage object with an lvalue basic_node object.
    /// @param n An lvalue basic_node object.
    explicit node_ref_storage(const node_type& n) noexcept
        : m_value_ref(&n),
          m_has_node_ref(true) {
    }

    /// @brief Construct a new node ref storage object with a std::initializer_list object.
    /// @param init A std::initializer_list object.
    node_ref_storage(std::initializer_list<node_ref_storage> init)
        : m_owned_value(init) {
    }

    /// @brief Construct a new node ref storage object with variadic template arguments
    /// @tparam Args Types of arguments to construct a basic_node object.
    /// @param args Arguments to construct a basic_node object.
    template <typename... Args, enable_if_t<std::is_constructible<node_type, Args...>::value, int> = 0>
    node_ref_storage(Args&&... args)
        : m_owned_value(std::forward<Args>(args)...),
          m_has_node_ref(has_single_basic_node_arg<Args...>::value) {
    }

    // allow only move construct/assignment
    node_ref_storage(const node_ref_storage&) = delete;
    node_ref_storage(node_ref_storage&&) = default;
    node_ref_storage& operator=(const node_ref_storage&) = delete;
    node_ref_storage& operator=(node_ref_storage&&) = default;

    ~node_ref_storage() = default;

public:
    /// @brief An arrow operator for node_ref_storage objects.
    /// @return const node_type* A constant pointer to a basic_node object.
    const node_type* operator->() const noexcept {
        return m_value_ref ? m_value_ref : &m_owned_value;
    }

    /// @brief Releases a basic_node object internally held.
    /// @return node_type A basic_node object internally held.
    node_type release() const noexcept {
        return m_value_ref ? *m_value_ref : std::move(m_owned_value);
    }

    bool has_node_ref() const noexcept {
        return m_has_node_ref;
    }

private:
    /// A storage for a basic_node object given with rvalue reference.
    mutable node_type m_owned_value = nullptr;
    /// A pointer to a basic_node object given with lvalue reference.
    const node_type* m_value_ref = nullptr;
    /// Whether this object was constructed directly from a basic_node object.
    bool m_has_node_ref = false;
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_NODE_REF_STORAGE_HPP */
