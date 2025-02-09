//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_MAP_RANGE_PROXY_HPP
#define FK_YAML_DETAIL_MAP_RANGE_PROXY_HPP

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief A helper iterator class which wraps a mapping iterator object.
/// @tparam Iterator The base iterator type.
template <typename Iterator>
class map_iterator_proxy {
public:
    /// @brief The type of the pointed-to elements by base iterators.
    using value_type = Iterator;

    /// @brief The type to represent difference between the pointed-to elements by base iterators.
    using difference_type = std::ptrdiff_t;

    /// @brief The type of the pointed-to element references by base iterators.
    using reference = value_type&;

    /// @brief The type of the pointed-to element pointers by base iterators.
    using pointer = value_type*;

    /// @brief The iterator category.
    using iterator_category = std::forward_iterator_tag;

    /// @brief Constructs a map_iterator_proxy object.
    map_iterator_proxy() = default;

    /// @brief Constructs a map_iterator_proxy object with an Iterator object.
    /// @param i A base iterator object.
    map_iterator_proxy(const Iterator& i) noexcept
        : m_base_iterator(i) {
    }

    /// @brief Copy constructs a map_iterator_proxy object.
    map_iterator_proxy(const map_iterator_proxy&) = default;

    /// @brief Copy assigns a map_iterator_proxy object.
    map_iterator_proxy& operator=(const map_iterator_proxy&) = default;

    /// @brief Move constructs a map_iterator_proxy object.
    map_iterator_proxy(map_iterator_proxy&&) = default;

    /// @biref Move assigns a map_iterator_proxy object.
    map_iterator_proxy& operator=(map_iterator_proxy&&) = default;

    /// @brief Destructs a map_iterator_proxy object.
    ~map_iterator_proxy() = default;

    /// @brief Get reference to the base iterator object.
    /// @return Reference to the base iterator object.
    reference operator*() noexcept {
        return m_base_iterator;
    }

    /// @brief Get pointer to the base iterator object.
    /// @return Pointer to the base iterator object.
    pointer operator->() noexcept {
        return &m_base_iterator;
    }

    /// @brief Pre-increments the base iterator object.
    /// @return Reference to this map_iterator_proxy object.
    map_iterator_proxy& operator++() noexcept {
        ++m_base_iterator;
        return *this;
    }

    /// @brief Post-increments the base iterator object.
    /// @return A map_iterator_proxy object with its base iterator incremented.
    map_iterator_proxy operator++(int) & noexcept {
        auto result = *this;
        ++(*this);
        return result;
    }

    /// @brief Check equality between map_iterator_proxy objects.
    /// @param rhs A map_iterator_proxy object to compare with.
    /// @return true if this map_iterator_proxy object is equal to `rhs`, false otherwise.
    bool operator==(const map_iterator_proxy& rhs) const noexcept {
        return m_base_iterator == rhs.m_base_iterator;
    }

    /// @brief Check inequality between map_iterator_proxy objects.
    /// @param rhs A map_iterator_proxy object to compare with.
    /// @return true if this map_iterator_proxy object is not equal to `rhs`, false otherwise.
    bool operator!=(const map_iterator_proxy& rhs) const noexcept {
        return m_base_iterator != rhs.m_base_iterator;
    }

    /// @brief Get the mapping key node pointed by the base iterator.
    /// @return Reference to the mapping key node.
    typename Iterator::reference key() const {
        return m_base_iterator.key();
    }

    /// @brief Get the mapping value node pointed by the base iterator.
    /// @return Reference to the mapping value node.
    typename Iterator::reference value() const noexcept {
        return m_base_iterator.value();
    }

private:
    /// The base iterator object.
    Iterator m_base_iterator {};
};

/// @brief A helper struct which allows accessing node iterator member functions in range-based for loops.
/// @tparam BasicNodeType A basic_node template instance type.
template <typename BasicNodeType>
class map_range_proxy {
    static_assert(
        is_basic_node<BasicNodeType>::value,
        "map_range_proxy only accepts a basic_node type as its template parameter.");

public:
    /// @brief The type of non-const iterators.
    using iterator = map_iterator_proxy<typename std::conditional<
        std::is_const<BasicNodeType>::value, typename BasicNodeType::const_iterator,
        typename BasicNodeType::iterator>::type>;

    /// @brief The type of const iterators.
    using const_iterator = map_iterator_proxy<typename BasicNodeType::const_iterator>;

    /// @brief Constructs a map_range_proxy object with a BasicNodeType object.
    /// @param map A mapping node object.
    map_range_proxy(BasicNodeType& map) noexcept
        : mp_map(&map) {
    }

    /// @brief Copy constructs a map_range_proxy object.
    map_range_proxy(const map_range_proxy&) = default;

    /// @brief Copy assigns a map_range_proxy object.
    /// @return Reference to this map_range_proxy object.
    map_range_proxy& operator=(const map_range_proxy&) = default;

    /// @brief Move constructs a map_range_proxy object.
    map_range_proxy(map_range_proxy&&) = default;

    /// @brief Move assigns a map_range_proxy object.
    /// @return Reference to this map_range_proxy object.
    map_range_proxy& operator=(map_range_proxy&&) = default;

    /// @brief Destructs a map_range_proxy object.
    ~map_range_proxy() = default;

    /// @brief Get an iterator to the first element.
    /// @return An iterator to the first element.
    iterator begin() noexcept {
        return {mp_map->begin()};
    }

    /// @brief Get a const iterator to the first element.
    /// @return A const iterator to the first element.
    const_iterator begin() const noexcept {
        return {mp_map->cbegin()};
    }

    /// @brief Get an iterator to the past-the-last element.
    /// @return An iterator to the past-the-last element.
    iterator end() noexcept {
        return {mp_map->end()};
    }

    /// @brief Get a const iterator to the past-the-last element.
    /// @return A const iterator to the past-the-last element.
    const_iterator end() const noexcept {
        return {mp_map->cend()};
    }

private:
    /// Pointer to the mapping node object. (non-null)
    BasicNodeType* mp_map {nullptr};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_MAP_RANGE_PROXY_HPP */
