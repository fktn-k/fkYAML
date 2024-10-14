//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.13
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_ITERATOR_HPP
#define FK_YAML_DETAIL_ITERATOR_HPP

#include <cstddef>
#include <iterator>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief A tag which tells Iterator will contain sequence value iterator.
struct sequence_iterator_tag {};

/// @brief A tag which tells Iterator will contain mapping value iterator.
struct mapping_iterator_tag {};

/// @brief The template definitions of type information used in @ref Iterator class
/// @tparam ValueType The type of iterated elements.
template <typename ValueType>
struct iterator_traits {
    /// A type of iterated elements.
    using value_type = ValueType;
    /// A type to represent difference between iterators.
    using difference_type = std::ptrdiff_t;
    /// A type to represent iterator sizes.
    using size_type = std::size_t;
    /// A type of an element pointer.
    using pointer = value_type*;
    /// A type of reference to an element.
    using reference = value_type&;
};

/// @brief A specialization of @ref iterator_traits for constant value types.
/// @tparam ValueType The type of iterated elements.
template <typename ValueType>
struct iterator_traits<const ValueType> {
    /// A type of iterated elements.
    using value_type = ValueType;
    /// A type to represent difference between iterators.
    using difference_type = std::ptrdiff_t;
    /// A type to represent iterator sizes.
    using size_type = std::size_t;
    /// A type of a constant element pointer.
    using pointer = const value_type*;
    /// A type of constant reference to an element.
    using reference = const value_type&;
};

/// @brief Definitions of iterator types for iterators internally held.
enum class iterator_t {
    SEQUENCE, //!< sequence iterator type.
    MAPPING,  //!< mapping iterator type.
};

/// @brief A class which holds iterators either of sequence or mapping type
/// @tparam ValueType The type of iterated elements.
template <typename ValueType>
class iterator {
public:
    /// A type for iterator traits of instantiated @Iterator template class.
    using ItrTraitsType = iterator_traits<ValueType>;

    /// A type for iterator category tag.
    using iterator_category = std::bidirectional_iterator_tag;
    /// A type of iterated element.
    using value_type = typename ItrTraitsType::value_type;
    /// A type to represent differences between iterators.
    using difference_type = typename ItrTraitsType::difference_type;
    /// A type to represent container sizes.
    using size_type = typename ItrTraitsType::size_type;
    /// A type of an element pointer.
    using pointer = typename ItrTraitsType::pointer;
    /// A type of reference to an element.
    using reference = typename ItrTraitsType::reference;

private:
    /// A type of non-const version of iterated elements.
    using NonConstValueType = typename std::remove_const<ValueType>::type;

    static_assert(is_basic_node<NonConstValueType>::value, "Iterator only accepts basic_node<...>");

    /// @brief The actual storage for iterators internally held in @ref Iterator.
    struct iterator_holder {
        /// A sequence iterator object.
        typename NonConstValueType::sequence_type::iterator sequence_iterator {};
        /// A mapping iterator object.
        typename NonConstValueType::mapping_type::iterator mapping_iterator {};
    };

public:
    /// @brief Construct a new iterator object with sequence iterator object.
    /// @param[in] itr An sequence iterator object.
    iterator(sequence_iterator_tag /* unused */, const typename ValueType::sequence_type::iterator& itr) noexcept
        : m_inner_iterator_type(iterator_t::SEQUENCE) {
        m_iterator_holder.sequence_iterator = itr;
    }

    /// @brief Construct a new iterator object with mapping iterator object.
    /// @param[in] itr An mapping iterator object.
    iterator(mapping_iterator_tag /* unused */, const typename ValueType::mapping_type::iterator& itr) noexcept
        : m_inner_iterator_type(iterator_t::MAPPING) {
        m_iterator_holder.mapping_iterator = itr;
    }

    /// @brief Copy constructor of the iterator class.
    /// @param other An iterator object to be copied with.
    iterator(const iterator& other) noexcept
        : m_inner_iterator_type(other.m_inner_iterator_type) {
        switch (m_inner_iterator_type) {
        case iterator_t::SEQUENCE:
            m_iterator_holder.sequence_iterator = other.m_iterator_holder.sequence_iterator;
            break;
        case iterator_t::MAPPING:
            m_iterator_holder.mapping_iterator = other.m_iterator_holder.mapping_iterator;
            break;
        }
    }

    /// @brief Move constructor of the iterator class.
    /// @param other An iterator object to be moved from.
    iterator(iterator&& other) noexcept
        : m_inner_iterator_type(other.m_inner_iterator_type) {
        switch (m_inner_iterator_type) {
        case iterator_t::SEQUENCE:
            m_iterator_holder.sequence_iterator = std::move(other.m_iterator_holder.sequence_iterator);
            break;
        case iterator_t::MAPPING:
            m_iterator_holder.mapping_iterator = std::move(other.m_iterator_holder.mapping_iterator);
            break;
        }
    }

    /// @brief Destroys an iterator.
    ~iterator() = default;

public:
    /// @brief A copy assignment operator of the iterator class.
    /// @param rhs An iterator object to be copied with.
    /// @return iterator& Reference to this iterator object.
    iterator& operator=(const iterator& rhs) noexcept {
        if FK_YAML_UNLIKELY (&rhs == this) {
            return *this;
        }

        m_inner_iterator_type = rhs.m_inner_iterator_type;
        switch (m_inner_iterator_type) {
        case iterator_t::SEQUENCE:
            m_iterator_holder.sequence_iterator = rhs.m_iterator_holder.sequence_iterator;
            break;
        case iterator_t::MAPPING:
            m_iterator_holder.mapping_iterator = rhs.m_iterator_holder.mapping_iterator;
            break;
        }

        return *this;
    }

    /// @brief A move assignment operator of the iterator class.
    /// @param rhs An iterator object to be moved from.
    /// @return iterator& Reference to this iterator object.
    iterator& operator=(iterator&& rhs) noexcept {
        if FK_YAML_UNLIKELY (&rhs == this) {
            return *this;
        }

        m_inner_iterator_type = rhs.m_inner_iterator_type;
        switch (m_inner_iterator_type) {
        case iterator_t::SEQUENCE:
            m_iterator_holder.sequence_iterator = std::move(rhs.m_iterator_holder.sequence_iterator);
            break;
        case iterator_t::MAPPING:
            m_iterator_holder.mapping_iterator = std::move(rhs.m_iterator_holder.mapping_iterator);
            break;
        }

        return *this;
    }

    /// @brief An arrow operator of the iterator class.
    /// @return pointer A pointer to the BasicNodeType object internally referenced by the actual iterator object.
    pointer operator->() noexcept {
        if (m_inner_iterator_type == iterator_t::SEQUENCE) {
            return &(*(m_iterator_holder.sequence_iterator));
        }

        // m_inner_iterator_type == iterator_t::MAPPING:
        return &(m_iterator_holder.mapping_iterator->second);
    }

    /// @brief A dereference operator of the iterator class.
    /// @return reference Reference to the Node object internally referenced by the actual iterator object.
    reference operator*() noexcept {
        if (m_inner_iterator_type == iterator_t::SEQUENCE) {
            return *(m_iterator_holder.sequence_iterator);
        }

        // m_inner_iterator_type == iterator_t::MAPPING:
        return m_iterator_holder.mapping_iterator->second;
    }

    /// @brief A compound assignment operator by sum of the Iterator class.
    /// @param i The difference from this Iterator object with which it moves forward.
    /// @return Iterator& Reference to this Iterator object.
    iterator& operator+=(difference_type i) noexcept {
        switch (m_inner_iterator_type) {
        case iterator_t::SEQUENCE:
            std::advance(m_iterator_holder.sequence_iterator, i);
            break;
        case iterator_t::MAPPING:
            std::advance(m_iterator_holder.mapping_iterator, i);
            break;
        }
        return *this;
    }

    /// @brief A plus operator of the iterator class.
    /// @param i The difference from this iterator object.
    /// @return iterator An iterator object which has been added @a i.
    iterator operator+(difference_type i) const noexcept {
        auto result = *this;
        result += i;
        return result;
    }

    /// @brief An pre-increment operator of the iterator class.
    /// @return iterator& Reference to this iterator object.
    iterator& operator++() noexcept {
        switch (m_inner_iterator_type) {
        case iterator_t::SEQUENCE:
            std::advance(m_iterator_holder.sequence_iterator, 1);
            break;
        case iterator_t::MAPPING:
            std::advance(m_iterator_holder.mapping_iterator, 1);
            break;
        }
        return *this;
    }

    /// @brief A post-increment operator of the iterator class.
    /// @return iterator An iterator object which has been incremented.
    iterator operator++(int) & noexcept {
        auto result = *this;
        ++(*this);
        return result;
    }

    /// @brief A compound assignment operator by difference of the iterator class.
    /// @param i The difference from this iterator object with which it moves backward.
    /// @return iterator& Reference to this iterator object.
    iterator& operator-=(difference_type i) noexcept {
        return operator+=(-i);
    }

    /// @brief A minus operator of the iterator class.
    /// @param i The difference from this iterator object.
    /// @return iterator An iterator object from which has been subtracted @ i.
    iterator operator-(difference_type i) noexcept {
        auto result = *this;
        result -= i;
        return result;
    }

    /// @brief A pre-decrement operator of the iterator class.
    /// @return iterator& Reference to this iterator object.
    iterator& operator--() noexcept {
        switch (m_inner_iterator_type) {
        case iterator_t::SEQUENCE:
            std::advance(m_iterator_holder.sequence_iterator, -1);
            break;
        case iterator_t::MAPPING:
            std::advance(m_iterator_holder.mapping_iterator, -1);
            break;
        }
        return *this;
    }

    /// @brief A post-decrement operator of the iterator class
    /// @return iterator An iterator object which has been decremented.
    iterator operator--(int) & noexcept {
        auto result = *this;
        --(*this);
        return result;
    }

    /// @brief An equal-to operator of the iterator class.
    /// @param rhs An iterator object to be compared with this iterator object.
    /// @return true  This iterator object is equal to the other.
    /// @return false This iterator object is not equal to the other.
    bool operator==(const iterator& rhs) const {
        if FK_YAML_UNLIKELY (m_inner_iterator_type != rhs.m_inner_iterator_type) {
            throw fkyaml::exception("Cannot compare iterators of different container types.");
        }

        if (m_inner_iterator_type == iterator_t::SEQUENCE) {
            return (m_iterator_holder.sequence_iterator == rhs.m_iterator_holder.sequence_iterator);
        }

        // m_inner_iterator_type == iterator_t::MAPPING
        return (m_iterator_holder.mapping_iterator == rhs.m_iterator_holder.mapping_iterator);
    }

    /// @brief An not-equal-to operator of the iterator class.
    /// @param rhs An iterator object to be compared with this iterator object.
    /// @return true  This iterator object is not equal to the other.
    /// @return false This iterator object is equal to the other.
    bool operator!=(const iterator& rhs) const {
        return !operator==(rhs);
    }

    /// @brief A less-than operator of the iterator class.
    /// @param rhs An iterator object to be compared with this iterator object.
    /// @return true  This iterator object is less than the other.
    /// @return false This iterator object is not less than the other.
    bool operator<(const iterator& rhs) const {
        if FK_YAML_UNLIKELY (m_inner_iterator_type != rhs.m_inner_iterator_type) {
            throw fkyaml::exception("Cannot compare iterators of different container types.");
        }

        if FK_YAML_UNLIKELY (m_inner_iterator_type == iterator_t::MAPPING) {
            throw fkyaml::exception("Cannot compare order of iterators of the mapping container type");
        }

        return (m_iterator_holder.sequence_iterator < rhs.m_iterator_holder.sequence_iterator);
    }

    ///  @brief A less-than-or-equal-to operator of the iterator class.
    ///  @param rhs An iterator object to be compared with this iterator object.
    ///  @return true  This iterator object is either less than or equal to the other.
    ///  @return false This iterator object is neither less than nor equal to the other.
    bool operator<=(const iterator& rhs) const {
        return !rhs.operator<(*this);
    }

    /// @brief A greater-than operator of the iterator class.
    /// @param rhs An iterator object to be compared with this iterator object.
    /// @return true  This iterator object is greater than the other.
    /// @return false This iterator object is not greater than the other.
    bool operator>(const iterator& rhs) const {
        return !operator<=(rhs);
    }

    /// @brief A greater-than-or-equal-to operator of the iterator class.
    /// @param rhs An iterator object to be compared with this iterator object.
    /// @return true  This iterator object is either greater than or equal to the other.
    /// @return false This iterator object is neither greater than nor equal to the other.
    bool operator>=(const iterator& rhs) const {
        return !operator<(rhs);
    }

public:
    /// @brief Get the type of the internal iterator implementation.
    /// @return iterator_t The type of the internal iterator implementation.
    iterator_t type() const noexcept {
        return m_inner_iterator_type;
    }

    /// @brief Get the key string of the YAML mapping node for the current iterator.
    /// @return const std::string& The key string of the YAML mapping node for the current iterator.
    const typename ValueType::mapping_type::key_type& key() const {
        if FK_YAML_UNLIKELY (m_inner_iterator_type == iterator_t::SEQUENCE) {
            throw fkyaml::exception("Cannot retrieve key from non-mapping iterators.");
        }

        return m_iterator_holder.mapping_iterator->first;
    }

    /// @brief Get the reference of the YAML node for the current iterator.
    /// @return reference A reference to the YAML node for the current iterator.
    reference value() noexcept {
        return operator*();
    }

private:
    /// A type of the internally-held iterator.
    iterator_t m_inner_iterator_type {iterator_t::SEQUENCE};
    /// A holder of actual iterators.
    mutable iterator_holder m_iterator_holder {};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_ITERATOR_HPP */
