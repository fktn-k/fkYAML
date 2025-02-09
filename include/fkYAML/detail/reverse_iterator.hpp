//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_REVERSE_ITERATOR_HPP
#define FK_YAML_DETAIL_REVERSE_ITERATOR_HPP

#include <iterator>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief An iterator adapter class that reverses the direction of a given node iterator.
/// @tparam Iterator The base iterator type.
template <typename Iterator>
class reverse_iterator {
    static_assert(
        is_basic_node<typename Iterator::value_type>::value,
        "reverse_iterator only accepts a basic_node type as the underlying iterator's value type");

public:
    /// @brief The base iterator type.
    using iterator_type = Iterator;

    /// @brief The base iterator category.
    using iterator_category = typename Iterator::iterator_category;

    /// @brief The type of the pointed-to elements by base iterators.
    using value_type = typename Iterator::value_type;

    /// @brief The type to represent differences between the pointed-to elements by the base iterators.
    using difference_type = typename Iterator::difference_type;

    /// @brief The type of the pointed-to element pointers by base iterators.
    using pointer = typename Iterator::pointer;

    /// @brief The type of the pointed-to element references by base iterators.
    using reference = typename Iterator::reference;

    /// @brief Constructs a reverse_iterator object.
    reverse_iterator() = default;

    /// @brief Copy constructs a reverse_iterator object.
    reverse_iterator(const reverse_iterator&) = default;

    /// @brief Copy assignments a reverse_iterator object.
    reverse_iterator& operator=(const reverse_iterator&) = default;

    /// @brief Move constructs a reverse_iterator object.
    reverse_iterator(reverse_iterator&&) = default;

    /// @brief Move assignments a reverse_iterator object.
    reverse_iterator& operator=(reverse_iterator&&) = default;

    /// @brief Constructs a reverse_iterator object with an underlying iterator object.
    /// @param i A base iterator object.
    reverse_iterator(const Iterator& i) noexcept
        : m_current(i) {
    }

    /// @brief Copy constructs a reverse_iterator object with a compatible reverse_iterator object.
    /// @tparam U A compatible iterator type with Iterator.
    /// @param other A compatible reverse_iterator object.
    template <typename U, enable_if_t<negation<std::is_same<U, Iterator>>::value, int> = 0>
    reverse_iterator(const reverse_iterator<U>& other) noexcept
        : m_current(other.base()) {
    }

    /// @brief Copy assigns a reverse_iterator object with a compatible reverse_iterator object.
    /// @tparam U A compatible iterator type with Iterator.
    /// @param other A compatible reverse_iterator object.
    /// @return Reference to this reverse_iterator object.
    template <typename U, enable_if_t<negation<std::is_same<U, Iterator>>::value, int> = 0>
    reverse_iterator& operator=(const reverse_iterator<U>& other) noexcept {
        m_current = other.base();
        return *this;
    }

    /// @brief Destructs a reverse_iterator object.
    ~reverse_iterator() = default;

    /// @brief Accesses the underlying iterator object.
    /// @return The underlying iterator object.
    Iterator base() const noexcept {
        return m_current;
    }

    /// @brief Get reference to the pointed-to element.
    /// @return Reference to the pointed-to element.
    reference operator*() const noexcept {
        Iterator tmp = m_current;
        return *--tmp;
    }

    /// @brief Get pointer to the pointed-to element.
    /// @return Pointer to the pointed-to element.
    pointer operator->() const noexcept {
        return &(operator*());
    }

    /// @brief Pre-increments the underlying iterator object.
    /// @return Reference to this reverse_iterator object with its underlying iterator incremented.
    reverse_iterator& operator++() noexcept {
        --m_current;
        return *this;
    }

    /// @brief Post-increments the underlying iterator object.
    /// @return A reverse_iterator object with the underlying iterator as-is.
    reverse_iterator operator++(int) & noexcept {
        auto result = *this;
        --m_current;
        return result;
    }

    /// @brief Pre-decrements the underlying iterator object.
    /// @return Reference to this reverse_iterator with its underlying iterator decremented.
    reverse_iterator& operator--() noexcept {
        ++m_current;
        return *this;
    }

    /// @brief Post-decrements the underlying iterator object.
    /// @return A reverse_iterator object with the underlying iterator as-is.
    reverse_iterator operator--(int) & noexcept {
        auto result = *this;
        ++m_current;
        return result;
    }

    /// @brief Advances the underlying iterator object by `n`.
    /// @param n The distance by which the underlying iterator is advanced.
    /// @return A reverse_iterator object with the underlying iterator advanced by `n`.
    reverse_iterator operator+(difference_type n) const noexcept {
        return reverse_iterator(m_current - n);
    }

    /// @brief Advances the underlying iterator object by `n`.
    /// @param n The distance by which the underlying iterator is advanced.
    /// @return Reference to this reverse_iterator object with the underlying iterator advanced by `n`.
    reverse_iterator& operator+=(difference_type n) noexcept {
        m_current -= n;
        return *this;
    }

    /// @brief Decrements the underlying iterator object by `n`.
    /// @param n The distance by which the underlying iterator is decremented.
    /// @return A reverse_iterator object with the underlying iterator decremented by `n`.
    reverse_iterator operator-(difference_type n) const noexcept {
        return reverse_iterator(m_current + n);
    }

    /// @brief Decrements the underlying iterator object by `n`.
    /// @param n The distance by which the underlying iterator is decremented.
    /// @return Reference to this reverse_iterator object with the underlying iterator decremented by `n`.
    reverse_iterator& operator-=(difference_type n) noexcept {
        m_current += n;
        return *this;
    }

    /// @brief Get the mapping key node of the underlying iterator.
    /// @return The mapping key node of the underlying iterator.
    auto key() const -> decltype(std::declval<Iterator>().key()) {
        Iterator itr = --(base());
        return itr.key();
    }

    /// @brief Get reference to the underlying iterator's value.
    /// @return Reference to the underlying iterator's value.
    reference value() noexcept {
        Iterator itr = --(base());
        return *itr;
    }

private:
    ///
    Iterator m_current;
};

/// @brief Check equality between reverse_iterator objects.
/// @tparam IteratorL Base iterator type for `lhs`.
/// @tparam IteratorR Base iterator type for `rhs`.
/// @param lhs A reverse_iterator object.
/// @param rhs A reverse_iterator object.
/// @return true if the two reverse_iterator objects are equal, false otherwise.
template <typename IteratorL, typename IteratorR>
inline bool operator==(const reverse_iterator<IteratorL>& lhs, const reverse_iterator<IteratorR>& rhs) {
    return lhs.base() == rhs.base();
}

/// @brief Check inequality between reverse_iterator objects.
/// @tparam IteratorL Base iterator type for `lhs`.
/// @tparam IteratorR Base iterator type for `rhs`.
/// @param lhs A reverse_iterator object.
/// @param rhs A reverse_iterator object.
/// @return true if the two reverse_iterator objects are not equal, false otherwise.
template <typename IteratorL, typename IteratorR>
inline bool operator!=(const reverse_iterator<IteratorL>& lhs, const reverse_iterator<IteratorR>& rhs) {
    return lhs.base() != rhs.base();
}

/// @brief Check if `lhs` is less than `rhs`.
/// @tparam IteratorL Base iterator type for `lhs`.
/// @tparam IteratorR Base iterator type for `rhs`.
/// @param lhs A reverse_iterator object.
/// @param rhs A reverse_iterator object.
/// @return true if `lhs` is less than `rhs`, false otherwise.
template <typename IteratorL, typename IteratorR>
inline bool operator<(const reverse_iterator<IteratorL>& lhs, const reverse_iterator<IteratorR>& rhs) {
    return lhs.base() > rhs.base();
}

/// @brief Check if `lhs` is less than or equal to `rhs`.
/// @tparam IteratorL Base iterator type for `lhs`.
/// @tparam IteratorR Base iterator type for `rhs`.
/// @param lhs A reverse_iterator object.
/// @param rhs A reverse_iterator object.
/// @return true if `lhs` is less than or equal to `rhs`, false otherwise.
template <typename IteratorL, typename IteratorR>
inline bool operator<=(const reverse_iterator<IteratorL>& lhs, const reverse_iterator<IteratorR>& rhs) {
    return lhs.base() >= rhs.base();
}

/// @brief Check if `lhs` is greater than `rhs`.
/// @tparam IteratorL Base iterator type for `lhs`.
/// @tparam IteratorR Base iterator type for `rhs`.
/// @param lhs A reverse_iterator object.
/// @param rhs A reverse_iterator object.
/// @return true if `lhs` is greater than `rhs`, false otherwise.
template <typename IteratorL, typename IteratorR>
inline bool operator>(const reverse_iterator<IteratorL>& lhs, const reverse_iterator<IteratorR>& rhs) {
    return lhs.base() < rhs.base();
}

/// @brief Check if `lhs` is greater than or equal to `rhs`.
/// @tparam IteratorL Base iterator type for `lhs`.
/// @tparam IteratorR Base iterator type for `rhs`.
/// @param lhs A reverse_iterator object.
/// @param rhs A reverse_iterator object.
/// @return true if `lhs` is greater than or equal to `rhs`, false otherwise.
template <typename IteratorL, typename IteratorR>
inline bool operator>=(const reverse_iterator<IteratorL>& lhs, const reverse_iterator<IteratorR>& rhs) {
    return lhs.base() <= rhs.base();
}

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_REVERSE_ITERATOR_HPP */
