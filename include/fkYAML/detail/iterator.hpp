//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_ITERATOR_HPP
#define FK_YAML_DETAIL_ITERATOR_HPP

#include <cstddef>
#include <iterator>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief The template definitions of type information used in @ref Iterator class
/// @tparam ValueType The type of iterated elements.
template <typename ValueType>
struct iterator_traits {
    /// A type of iterated elements.
    using value_type = typename ValueType::value_type;
    /// A type to represent difference between iterators.
    using difference_type = typename ValueType::difference_type;
    /// A type of an element pointer.
    using pointer = typename ValueType::pointer;
    /// A type of reference to an element.
    using reference = typename ValueType::reference;
};

/// @brief A specialization of @ref iterator_traits for constant value types.
/// @tparam ValueType The type of iterated elements.
template <typename ValueType>
struct iterator_traits<const ValueType> {
    /// A type of iterated elements.
    using value_type = typename ValueType::value_type;
    /// A type to represent difference between iterators.
    using difference_type = typename ValueType::difference_type;
    /// A type of a constant element pointer.
    using pointer = typename ValueType::const_pointer;
    /// A type of constant reference to an element.
    using reference = typename ValueType::const_reference;
};

/// @brief Definitions of iterator types for iterators internally held.
enum class iterator_t : std::uint8_t {
    SEQUENCE, //!< sequence iterator type.
    MAPPING,  //!< mapping iterator type.
};

/// @brief The actual storage for iterators internally held in iterator.
template <typename BasicNodeType>
struct iterator_holder {
    static_assert(
        is_basic_node<BasicNodeType>::value,
        "iterator_holder class only accepts a basic_node as its template parameter.");

    /// A sequence iterator object.
    typename BasicNodeType::sequence_type::iterator sequence_iterator {};
    /// A mapping iterator object.
    typename BasicNodeType::mapping_type::iterator mapping_iterator {};
};

/// @brief A class which holds iterators either of sequence or mapping type
/// @tparam ValueType The type of iterated elements.
template <typename ValueType>
class iterator {
    /// @brief The iterator type with ValueType of different const-ness.
    using other_iterator_type = typename std::conditional<
        std::is_const<ValueType>::value, iterator<typename std::remove_const<ValueType>::type>,
        iterator<const ValueType>>::type;

    friend other_iterator_type;

public:
    /// A type for iterator traits of instantiated @Iterator template class.
    using iterator_traits_type = iterator_traits<ValueType>;

    /// A type for iterator category tag.
    using iterator_category = std::bidirectional_iterator_tag;
    /// A type of iterated element.
    using value_type = typename iterator_traits_type::value_type;
    /// A type to represent differences between iterators.
    using difference_type = typename iterator_traits_type::difference_type;
    /// A type of an element pointer.
    using pointer = typename iterator_traits_type::pointer;
    /// A type of reference to an element.
    using reference = typename iterator_traits_type::reference;

    static_assert(is_basic_node<value_type>::value, "iterator class only accepts a basic_node as its value type.");

    /// @brief Constructs an iterator object.
    iterator() = default;

    /// @brief Construct a new iterator object with sequence iterator object.
    /// @param[in] itr An sequence iterator object.
    iterator(const typename value_type::sequence_type::iterator& itr) noexcept {
        m_iterator_holder.sequence_iterator = itr;
    }

    /// @brief Construct a new iterator object with mapping iterator object.
    /// @param[in] itr An mapping iterator object.
    iterator(const typename value_type::mapping_type::iterator& itr) noexcept
        : m_inner_iterator_type(iterator_t::MAPPING) {
        m_iterator_holder.mapping_iterator = itr;
    }

    /// @brief Copy constructs an iterator.
    iterator(const iterator&) = default;

    /// @brief Copy constructs an iterator from another iterator with different const-ness in ValueType.
    /// @note This copy constructor is not defined if ValueType is not const to avoid const removal from ValueType.
    /// @tparam OtherIterator The iterator type to copy from.
    /// @param other An iterator to copy from with different const-ness in ValueType.
    template <
        typename OtherIterator,
        enable_if_t<
            conjunction<std::is_same<OtherIterator, other_iterator_type>, std::is_const<ValueType>>::value, int> = 0>
    iterator(const OtherIterator& other) noexcept
        : m_inner_iterator_type(other.m_inner_iterator_type),
          m_iterator_holder(other.m_iterator_holder) {
    }

    /// @brief A copy assignment operator of the iterator class.
    iterator& operator=(const iterator&) = default;

    template <
        typename OtherIterator,
        enable_if_t<
            conjunction<std::is_same<OtherIterator, other_iterator_type>, std::is_const<ValueType>>::value, int> = 0>
    iterator& operator=(const OtherIterator& other) noexcept {
        m_inner_iterator_type = other.m_inner_iterator_type;
        m_iterator_holder = other.m_iterator_holder;
        return *this;
    }

    /// @brief Move constructs an iterator.
    iterator(iterator&&) = default;

    /// @brief A move assignment operator of the iterator class.
    iterator& operator=(iterator&&) = default;

    /// @brief Destroys an iterator.
    ~iterator() = default;

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
    reference operator*() const noexcept {
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
    iterator operator-(difference_type i) const noexcept {
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
    template <
        typename Iterator,
        enable_if_t<
            disjunction<std::is_same<Iterator, iterator>, std::is_same<Iterator, other_iterator_type>>::value, int> = 0>
    bool operator==(const Iterator& rhs) const {
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
    template <
        typename Iterator,
        enable_if_t<
            disjunction<std::is_same<Iterator, iterator>, std::is_same<Iterator, other_iterator_type>>::value, int> = 0>
    bool operator!=(const Iterator& rhs) const {
        return !operator==(rhs);
    }

    /// @brief A less-than operator of the iterator class.
    /// @param rhs An iterator object to be compared with this iterator object.
    /// @return true  This iterator object is less than the other.
    /// @return false This iterator object is not less than the other.
    template <
        typename Iterator,
        enable_if_t<
            disjunction<std::is_same<Iterator, iterator>, std::is_same<Iterator, other_iterator_type>>::value, int> = 0>
    bool operator<(const Iterator& rhs) const {
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
    template <
        typename Iterator,
        enable_if_t<
            disjunction<std::is_same<Iterator, iterator>, std::is_same<Iterator, other_iterator_type>>::value, int> = 0>
    bool operator<=(const Iterator& rhs) const {
        return !rhs.operator<(*this);
    }

    /// @brief A greater-than operator of the iterator class.
    /// @param rhs An iterator object to be compared with this iterator object.
    /// @return true  This iterator object is greater than the other.
    /// @return false This iterator object is not greater than the other.
    template <
        typename Iterator,
        enable_if_t<
            disjunction<std::is_same<Iterator, iterator>, std::is_same<Iterator, other_iterator_type>>::value, int> = 0>
    bool operator>(const Iterator& rhs) const {
        return !operator<=(rhs);
    }

    /// @brief A greater-than-or-equal-to operator of the iterator class.
    /// @param rhs An iterator object to be compared with this iterator object.
    /// @return true  This iterator object is either greater than or equal to the other.
    /// @return false This iterator object is neither greater than nor equal to the other.
    template <
        typename Iterator,
        enable_if_t<
            disjunction<std::is_same<Iterator, iterator>, std::is_same<Iterator, other_iterator_type>>::value, int> = 0>
    bool operator>=(const Iterator& rhs) const {
        return !operator<(rhs);
    }

public:
    /// @brief Get the type of the internal iterator implementation.
    /// @return iterator_t The type of the internal iterator implementation.
    iterator_t type() const noexcept {
        return m_inner_iterator_type;
    }

    /// @brief Get the mapping key node of the current iterator.
    /// @return The mapping key node of the current iterator.
    const typename value_type::mapping_type::key_type& key() const {
        if FK_YAML_UNLIKELY (m_inner_iterator_type == iterator_t::SEQUENCE) {
            throw fkyaml::exception("Cannot retrieve key from non-mapping iterators.");
        }

        return m_iterator_holder.mapping_iterator->first;
    }

    /// @brief Get reference to the YAML node of the current iterator.
    /// @return Reference to the YAML node of the current iterator.
    reference value() const noexcept {
        return operator*();
    }

private:
    /// A type of the internally-held iterator.
    iterator_t m_inner_iterator_type {iterator_t::SEQUENCE};
    /// A holder of actual iterators.
    iterator_holder<value_type> m_iterator_holder {};
};

/// @brief Get reference to a mapping key node.
/// @tparam ValueType The iterator value type.
/// @tparam I The element index.
/// @param i An iterator object.
/// @return Reference to a mapping key node.
template <std::size_t I, typename ValueType, enable_if_t<I == 0, int> = 0>
inline auto get(const iterator<ValueType>& i) -> decltype(i.key()) {
    return i.key();
}

/// @brief Get reference to a mapping value node.
/// @tparam ValueType The iterator value type.
/// @tparam I The element index
/// @param i An iterator object.
/// @return Reference to a mapping value node.
template <std::size_t I, typename ValueType, enable_if_t<I == 1, int> = 0>
inline auto get(const iterator<ValueType>& i) -> decltype(i.value()) {
    return i.value();
}

FK_YAML_DETAIL_NAMESPACE_END

namespace std {

#ifdef __clang__
// clang emits warnings against mixed usage of class/struct for tuple_size/tuple_element.
// see also: https://groups.google.com/a/isocpp.org/g/std-discussion/c/QC-AMb5oO1w
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#endif

/// @brief Parcial pecialization of std::tuple_size for iterator class.
/// @tparam ValueType The iterator value type.
template <typename ValueType>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct tuple_size<::fkyaml::detail::iterator<ValueType>> : integral_constant<size_t, 2> {};

/// @brief Parcial specialization of std::tuple_element for iterator class.
/// @tparam ValueType The iterator value type.
/// @tparam I The element index.
template <size_t I, typename ValueType>
// NOLINTNEXTLINE(cert-dcl58-cpp)
struct tuple_element<I, ::fkyaml::detail::iterator<ValueType>> {
    using type = decltype(get<I>(std::declval<::fkyaml::detail::iterator<ValueType>>()));
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

} // namespace std

#endif /* FK_YAML_DETAIL_ITERATOR_HPP */
