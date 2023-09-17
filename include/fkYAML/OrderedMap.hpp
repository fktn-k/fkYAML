/**
 * @file Deserializer.hpp
 * @brief Implementation of a minimal map-like container which preserves insertion order.
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_ORDERED_MAP_HPP_
#define FK_YAML_ORDERED_MAP_HPP_

#include <functional>
#include <initializer_list>
#include <memory>
#include <utility>
#include <vector>

#include "fkYAML/VersioningMacros.hpp"
#include "fkYAML/Exception.hpp"

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

/**
 * @brief A minimal map-like container which preserves insertion order.
 *
 * @tparam Key
 * @tparam Value
 * @tparam IgnoredCompare
 * @tparam Allocator
 */
template <
    typename Key, typename Value, typename IgnoredCompare = std::less<Key>,
    typename Allocator = std::allocator<std::pair<const Key, Value>>>
class OrderedMap : public std::vector<std::pair<const Key, Value>, Allocator>
{
public:
    using key_type = Key;
    using mapped_type = Value;
    using Container = std::vector<std::pair<const Key, Value>, Allocator>;
    using value_type = typename Container::value_type;
    using iterator = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;
    using size_type = typename Container::size_type;
    using key_compare = std::equal_to<Key>;

public:
    /**
     * @brief Construct a new OrderedMap object.
     */
    OrderedMap() noexcept(noexcept(Container()))
        : Container(),
          m_compare()
    {
    }

    /**
     * @brief Construct a new OrderedMap object with an initializer list.
     *
     * @param init An initializer list to construct the inner container object.
     */
    OrderedMap(std::initializer_list<value_type> init)
        : Container {init},
          m_compare()
    {
    }

public:
    mapped_type& operator[](const key_type& key) noexcept
    {
        return emplace(key, mapped_type()).first->second;
    }

public:
    // NOLINTNEXTLINE(readability-identifier-naming)
    std::pair<iterator, bool> emplace(const key_type& key, const mapped_type& value) noexcept
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

    // NOLINTNEXTLINE(readability-identifier-naming)
    mapped_type& at(const key_type& key)
    {
        for (auto itr = this->begin(); itr != this->end(); ++itr)
        {
            if (m_compare(itr->first, key))
            {
                return itr->second;
            }
        }
        throw Exception("key not found.");
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    const mapped_type& at(const key_type& key) const
    {
        for (auto itr = this->begin(); itr != this->end(); ++itr)
        {
            if (m_compare(itr->first, key))
            {
                return itr->second;
            }
        }
        throw Exception("key not found.");
    }

    // NOLINTNEXTLINE(readability-identifier-naming)
    iterator find(const key_type& key) noexcept
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

    // NOLINTNEXTLINE(readability-identifier-naming)
    const_iterator find(const key_type& key) const noexcept
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
    key_compare m_compare;
};

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_ORDERED_MAP_HPP_ */
