//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_MAP_RANGE_PROXY_HPP
#define FK_YAML_DETAIL_MAP_RANGE_PROXY_HPP

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

template <typename Iterator>
class map_iterator_proxy {
public:
    using value_type = Iterator;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using pointer = value_type*;
    using iterator_category = std::forward_iterator_tag;

    map_iterator_proxy() = default;

    map_iterator_proxy(const Iterator& i) noexcept
        : m_base_iterator(i) {
    }

    map_iterator_proxy(const map_iterator_proxy&) = default;
    map_iterator_proxy& operator=(const map_iterator_proxy&) = default;
    map_iterator_proxy(map_iterator_proxy&&) = default;
    map_iterator_proxy& operator=(map_iterator_proxy&&) = default;

    ~map_iterator_proxy() = default;

    reference operator*() noexcept {
        return m_base_iterator;
    }

    pointer operator->() noexcept {
        return &m_base_iterator;
    }

    map_iterator_proxy& operator++() noexcept {
        ++m_base_iterator;
        return *this;
    }

    map_iterator_proxy operator++(int) & noexcept {
        auto result = *this;
        ++(*this);
        return result;
    }

    bool operator==(const map_iterator_proxy& rhs) const noexcept {
        return m_base_iterator == rhs.m_base_iterator;
    }

    bool operator!=(const map_iterator_proxy& rhs) const noexcept {
        return m_base_iterator != rhs.m_base_iterator;
    }

    typename Iterator::reference key() const {
        return m_base_iterator.key();
    }

    typename Iterator::reference value() const noexcept {
        return m_base_iterator.value();
    }

private:
    Iterator m_base_iterator {};
};

template <typename BasicNodeType>
class map_range_proxy {
    static_assert(
        is_basic_node<remove_cv_t<BasicNodeType>>::value,
        "map_range_proxy only accepts a basic_node type as its template parameter.");

public:
    using iterator = map_iterator_proxy<typename std::conditional<
        std::is_const<BasicNodeType>::value, typename BasicNodeType::const_iterator,
        typename BasicNodeType::iterator>::type>;

    using const_iterator = map_iterator_proxy<typename BasicNodeType::const_iterator>;

    map_range_proxy(BasicNodeType& map) noexcept
        : mp_map(&map) {
    }

    map_range_proxy(const map_range_proxy&) = default;
    map_range_proxy& operator=(const map_range_proxy&) = default;
    map_range_proxy(map_range_proxy&&) = default;
    map_range_proxy& operator=(map_range_proxy&&) = default;

    ~map_range_proxy() = default;

    iterator begin() noexcept {
        return {mp_map->begin()};
    }

    const_iterator begin() const noexcept {
        return {mp_map->cbegin()};
    }

    iterator end() noexcept {
        return {mp_map->end()};
    }

    const_iterator end() const noexcept {
        return {mp_map->cend()};
    }

private:
    BasicNodeType* mp_map {nullptr};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_MAP_RANGE_PROXY_HPP */
