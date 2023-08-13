/**
 * Iterator.hpp - Implementation of the YAML node iterator.
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_ITERATOR_HPP_
#define FK_YAML_ITERATOR_HPP_

#include <cstddef>
#include <iterator>

#include "fkYAML/Exception.hpp"

namespace fkyaml
{

struct SequenceIteratorTag
{
};
struct MappingIteratorTag
{
};

template <typename ValueType>
struct IteratorTraits
{
    using value_type = ValueType;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;
    using pointer = value_type*;
    using reference = value_type&;
};

template <typename ValueType>
struct IteratorTraits<const ValueType>
{
    using value_type = ValueType;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;
    using pointer = const value_type*;
    using reference = const value_type&;
};

template <typename ValueType>
class Iterator
{
public:
    using ItrTraitsType = IteratorTraits<ValueType>;

    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename ItrTraitsType::value_type;
    using difference_type = typename ItrTraitsType::difference_type;
    using size_type = typename ItrTraitsType::size_type;
    using pointer = typename ItrTraitsType::pointer;
    using reference = typename ItrTraitsType::reference;

private:
    enum class InnerIteratorType
    {
        SEQUENCE,
        MAPPING,
    };

    using NonConstValueType = typename std::remove_const<ValueType>::type;

    struct IteratorHolder
    {
        typename NonConstValueType::sequence_type::iterator sequence_iterator {};
        typename NonConstValueType::mapping_type::iterator mapping_iterator {};
    };

public:
    Iterator(SequenceIteratorTag /* unused */, const typename ValueType::sequence_type::iterator& itr) noexcept
        : m_inner_iterator_type(InnerIteratorType::SEQUENCE)
    {
        m_iterator_holder.sequence_iterator = itr;
    }

    Iterator(MappingIteratorTag /* unused */, const typename ValueType::mapping_type::iterator& itr) noexcept
        : m_inner_iterator_type(InnerIteratorType::MAPPING)
    {
          m_iterator_holder.mapping_iterator = itr;
    }

    Iterator(const Iterator& other) noexcept
        : m_inner_iterator_type(other.m_inner_iterator_type)
    {
        switch (m_inner_iterator_type)
        {
        case InnerIteratorType::SEQUENCE:
            m_iterator_holder.sequence_iterator = other.m_iterator_holder.sequence_iterator;
            break;
        case InnerIteratorType::MAPPING:
            m_iterator_holder.mapping_iterator = other.m_iterator_holder.mapping_iterator;
            break;
        }
    }

    Iterator(Iterator&& other) noexcept
        : m_inner_iterator_type(other.m_inner_iterator_type)
    {
        switch (m_inner_iterator_type)
        {
        case InnerIteratorType::SEQUENCE:
            m_iterator_holder.sequence_iterator = std::move(other.m_iterator_holder.sequence_iterator);
            break;
        case InnerIteratorType::MAPPING:
            m_iterator_holder.mapping_iterator = std::move(other.m_iterator_holder.mapping_iterator);
            break;
        }
    }

    ~Iterator() = default;

public:
    Iterator& operator=(const Iterator& other) noexcept // NOLINT(cert-oop54-cpp)
    {
        if (&other == this)
        {
            return *this;
        }

        m_inner_iterator_type = other.m_inner_iterator_type;
        switch (m_inner_iterator_type)
        {
        case InnerIteratorType::SEQUENCE:
            m_iterator_holder.sequence_iterator = other.m_iterator_holder.sequence_iterator;
            break;
        case InnerIteratorType::MAPPING:
            m_iterator_holder.mapping_iterator = other.m_iterator_holder.mapping_iterator;
            break;
        }

        return *this;
    }

    Iterator& operator=(Iterator&& other) noexcept
    {
        if (&other == this)
        {
            return *this;
        }

        m_inner_iterator_type = other.m_inner_iterator_type;
        switch (m_inner_iterator_type)
        {
        case InnerIteratorType::SEQUENCE:
            m_iterator_holder.sequence_iterator = std::move(other.m_iterator_holder.sequence_iterator);
            break;
        case InnerIteratorType::MAPPING:
            m_iterator_holder.mapping_iterator = std::move(other.m_iterator_holder.mapping_iterator);
            break;
        }

        return *this;
    }

    pointer operator->() noexcept
    {
        switch (m_inner_iterator_type)
        {
        case InnerIteratorType::SEQUENCE:
            return &(*(m_iterator_holder.sequence_iterator));
        case InnerIteratorType::MAPPING:
            return &(m_iterator_holder.mapping_iterator->second);
        default:
            return nullptr;
        }
    }

    reference operator*() noexcept
    {
        switch (m_inner_iterator_type)
        {
        case InnerIteratorType::SEQUENCE:
            return *(m_iterator_holder.sequence_iterator);
        case InnerIteratorType::MAPPING:
            return *(m_iterator_holder.mapping_iterator).second;
        }
    }

    Iterator& operator+=(difference_type i)
    {
        switch (m_inner_iterator_type)
        {
        case InnerIteratorType::SEQUENCE:
            std::advance(m_iterator_holder.sequence_iterator, i);
            break;
        case InnerIteratorType::MAPPING:
            throw Exception("Cannot use offsets with operators of the mapping container type.");
        }
        return *this;
    }

    Iterator operator+(difference_type i) const
    {
        auto result = *this;
        result += i;
        return result;
    }

    Iterator& operator++() noexcept
    {
        switch (m_inner_iterator_type)
        {
        case InnerIteratorType::SEQUENCE:
            std::advance(m_iterator_holder.sequence_iterator, 1);
            break;
        case InnerIteratorType::MAPPING:
            std::advance(m_iterator_holder.mapping_iterator, 1);
            break;
        }
        return *this;
    }

    Iterator operator++(int) & noexcept // NOLINT(cert-dcl21-cpp)
    {
        auto result = *this;
        ++(*this);
        return result;
    }

    Iterator& operator-=(difference_type i)
    {
        return operator+=(-i);
    }

    Iterator operator-(difference_type i)
    {
        auto result = *this;
        result -= i;
        return result;
    }

    Iterator& operator--() noexcept
    {
        switch (m_inner_iterator_type)
        {
        case InnerIteratorType::SEQUENCE:
            std::advance(m_iterator_holder.sequence_iterator, -1);
            break;
        case InnerIteratorType::MAPPING:
            std::advance(m_iterator_holder.mapping_iterator, -1);
            break;
        }
        return *this;
    }

    Iterator operator--(int) & noexcept // NOLINT(cert-dcl21-cpp)
    {
        auto result = *this;
        --(*this);
        return result;
    }

    bool operator==(const Iterator& other) const
    {
        if (m_inner_iterator_type != other.m_inner_iterator_type)
        {
            throw Exception("Cannot compare iterators of different container types.");
        }

        switch (m_inner_iterator_type)
        {
        case InnerIteratorType::SEQUENCE:
            return (m_iterator_holder.sequence_iterator == other.m_iterator_holder.sequence_iterator);
        case InnerIteratorType::MAPPING:
            return (m_iterator_holder.mapping_iterator == other.m_iterator_holder.mapping_iterator);
        default:
            return false;
        }
    }

    bool operator!=(const Iterator& other) const
    {
        return !operator==(other);
    }

    bool operator<(const Iterator& other) const
    {
        if (m_inner_iterator_type != other.m_inner_iterator_type)
        {
            throw Exception("Cannot compare iterators of different container types.");
        }

        switch (m_inner_iterator_type)
        {
        case InnerIteratorType::SEQUENCE:
            return (m_iterator_holder.sequence_iterator < other.m_iterator_holder.sequence_iterator);
        case InnerIteratorType::MAPPING:
            throw Exception("Cannot compare order of iterators of the mapping container type");
        }
    }

    bool operator<=(const Iterator& other) const
    {
        return !other.operator<(*this);
    }

    bool operator>(const Iterator& other) const
    {
        return !operator<=(other);
    }

    bool operator>=(const Iterator& other) const
    {
        return !operator<(other);
    }

private:
    InnerIteratorType m_inner_iterator_type;
    mutable IteratorHolder m_iterator_holder;
};

} // namespace fkyaml

#endif /* FK_YAML_ITERATOR_HPP_ */