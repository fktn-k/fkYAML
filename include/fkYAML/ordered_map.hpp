//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_ORDERED_MAP_HPP
#define FK_YAML_ORDERED_MAP_HPP

#include <functional>
#include <initializer_list>
#include <memory>
#include <utility>
#include <vector>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>
#include <fkYAML/exception.hpp>

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
class ordered_map : public std::vector<std::pair<const Key, Value>, Allocator> {
public:
    /// @brief A type for keys.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/
    using key_type = Key;

    /// @brief A type for values.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/
    using mapped_type = Value;

    /// @brief A type for internal key-value containers.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/
    using Container = std::vector<std::pair<const Key, Value>, Allocator>;

    /// @brief A type for key-value pairs.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/
    using value_type = typename Container::value_type;

    /// @brief A type for non-const iterators.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/
    using iterator = typename Container::iterator;

    /// @brief A type for const iterators.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/
    using const_iterator = typename Container::const_iterator;

    /// @brief A type for size parameters used in this class.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/
    using size_type = typename Container::size_type;

    /// @brief A type for comparison between keys.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/
    using key_compare = std::equal_to<Key>;

public:
    /// @brief Construct a new ordered_map object.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/constructor/
    ordered_map() noexcept(noexcept(Container()))
        : Container() {
    }

    /// @brief Construct a new ordered_map object with an initializer list.
    /// @param init An initializer list to construct the inner container object.
    /// @sa https://fktn-k.github.io/fkYAML/api/ordered_map/constructor/
    ordered_map(std::initializer_list<value_type> init)
        : Container {init} {
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
    mapped_type& operator[](KeyType&& key) noexcept {
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
    std::pair<iterator, bool> emplace(KeyType&& key, const mapped_type& value) noexcept {
        for (auto itr = this->begin(); itr != this->end(); ++itr) {
            if (m_compare(itr->first, key)) {
                return {itr, false};
            }
        }
        this->emplace_back(std::forward<KeyType>(key), value);
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
    mapped_type& at(KeyType&& key) { // NOLINT(cppcoreguidelines-missing-std-forward)
        for (auto itr = this->begin(); itr != this->end(); ++itr) {
            if (m_compare(itr->first, key)) {
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
    const mapped_type& at(KeyType&& key) const { // NOLINT(cppcoreguidelines-missing-std-forward)
        for (auto itr = this->begin(); itr != this->end(); ++itr) {
            if (m_compare(itr->first, key)) {
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
    iterator find(KeyType&& key) noexcept { // NOLINT(cppcoreguidelines-missing-std-forward)
        for (auto itr = this->begin(); itr != this->end(); ++itr) {
            if (m_compare(itr->first, key)) {
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
    const_iterator find(KeyType&& key) const noexcept { // NOLINT(cppcoreguidelines-missing-std-forward)
        for (auto itr = this->begin(); itr != this->end(); ++itr) {
            if (m_compare(itr->first, key)) {
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

#endif /* FK_YAML_ORDERED_MAP_HPP */
