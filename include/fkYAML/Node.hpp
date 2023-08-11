/**
 * Node.hpp - Implementation of YAML node data structure.
 * 
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_NODE_HPP_
#define FK_YAML_NODE_HPP_

#include <cassert>
#include <cstring>
#include <memory>
#include <type_traits>

#include "fkYAML/Exception.hpp"
#include "fkYAML/Iterator.hpp"
#include "fkYAML/NodeType.hpp"

namespace fkyaml
{

class Node
{
public:
    using iterator = Iterator<Node>;
    using const_iterator = Iterator<const Node>;

private:
    union NodeValue {
        NodeValue() = default;

        NodeValue(NodeType type) noexcept
        {
            switch (type)
            {
            case NodeType::SEQUENCE:
                sequence = CreateObject<NodeSequenceType>();
                break;
            case NodeType::MAPPING:
                mapping = CreateObject<NodeMappingType>();
                break;
            case NodeType::NULL_OBJECT:
                mapping = nullptr;
                break;
            case NodeType::BOOLEAN:
                boolean = static_cast<NodeBooleanType>(false);
                break;
            case NodeType::SIGNED_INTEGER:
                signed_int = static_cast<NodeSignedIntType>(0);
                break;
            case NodeType::UNSIGNED_INTEGER:
                unsigned_int = static_cast<NodeUnsignedIntType>(0);
                break;
            case NodeType::FLOAT_NUMBER:
                float_val = static_cast<NodeFloatNumberType>(0.0);
                break;
            case NodeType::STRING:
                str = CreateObject<NodeStringType>("");
                break;
            }
        }

        NodeValue(const NodeSequenceType& seq) noexcept
            : sequence(CreateObject<NodeSequenceType>(seq))
        {}

        NodeValue& operator=(const NodeSequenceType& seq) noexcept
        {
            sequence = CreateObject<NodeSequenceType>(seq);
            return *this;
        }

        NodeValue(NodeSequenceType&& seq) noexcept
            : sequence(CreateObject<NodeSequenceType>(std::move(seq)))
        {}

        NodeValue& operator=(NodeSequenceType&& seq) noexcept
        {
            sequence = CreateObject<NodeSequenceType>(std::move(seq));
            return *this;
        }

        NodeValue(const NodeMappingType& map) noexcept
            : mapping(CreateObject<NodeMappingType>(map))
        {}

        NodeValue& operator=(const NodeMappingType& map) noexcept
        {
            mapping = CreateObject<NodeMappingType>(map);
            return *this;
        }

        NodeValue(NodeMappingType&& map) noexcept
            : mapping(CreateObject<NodeMappingType>(std::move(map)))
        {}

        NodeValue& operator=(NodeMappingType&& map) noexcept
        {
            mapping = CreateObject<NodeMappingType>(std::move(map));
            return *this;
        }

        NodeValue(const NodeBooleanType bool_val) noexcept
            : boolean(bool_val)
        {}

        NodeValue& operator=(const NodeBooleanType bool_val) noexcept
        {
            boolean = bool_val;
            return *this;
        }

        NodeValue(const NodeSignedIntType int_val) noexcept
            : signed_int(int_val)
        {}

        NodeValue& operator=(const NodeSignedIntType int_val) noexcept
        {
            signed_int = int_val;
            return *this;
        }

        NodeValue(const NodeUnsignedIntType uint_val) noexcept
            : unsigned_int(uint_val)
        {}

        NodeValue& operator=(const NodeUnsignedIntType uint_val) noexcept
        {
            unsigned_int = uint_val;
            return *this;
        }

        NodeValue(const NodeFloatNumberType f_val) noexcept
            : float_val(f_val)
        {
        }

        NodeValue& operator=(const NodeFloatNumberType f_val) noexcept
        {
            float_val = f_val;
            return *this;
        }

        NodeValue(const NodeStringType& str_val) noexcept
            : str(CreateObject<NodeStringType>(str_val))
        {}

        NodeValue& operator=(const NodeStringType& str_val) noexcept
        {
            str = CreateObject<NodeStringType>(str_val);
            return *this;
        }

        NodeValue(NodeStringType&& str_val) noexcept
            : str(CreateObject<NodeStringType>(std::move(str_val)))
        {}

        NodeValue& operator=(NodeStringType&& str_val) noexcept
        {
            str = CreateObject<NodeStringType>(std::move(str_val));
            return *this;
        }

        void Destroy(NodeType type) noexcept
        {
            switch (type)
            {
            case NodeType::SEQUENCE:
                for (auto& item : *sequence)
                {
                    item.Destroy();
                }
                sequence->clear();
                DestroyObject<NodeSequenceType>(sequence);
                sequence = nullptr;
                break;
            case NodeType::MAPPING:
                for (auto& item : *mapping)
                {
                    item.second.Destroy();
                }
                mapping->clear();
                DestroyObject<NodeMappingType>(mapping);
                mapping = nullptr;
                break;
            case NodeType::STRING:
                DestroyObject<NodeStringType>(str);
                str = nullptr;
                break;
            default:
                break;
            }
        }

        NodeSequenceType*   sequence;
        NodeMappingType*    mapping;
        NodeBooleanType     boolean;
        NodeSignedIntType   signed_int;
        NodeUnsignedIntType unsigned_int;
        NodeFloatNumberType float_val;
        NodeStringType*     str;
    };

private:
    template <typename ObjType, typename... ArgTypes>
    static ObjType* CreateObject(ArgTypes&&... args) noexcept
    {
        using AllocType = std::allocator<ObjType>;
        using AllocTraitsType = std::allocator_traits<AllocType>;

        AllocType alloc;
        auto deleter = [&](ObjType* obj)
        {
            AllocTraitsType::destroy(alloc, obj);
            AllocTraitsType::deallocate(alloc, obj, 1);
        };

        std::unique_ptr<ObjType, decltype(deleter)> object(AllocTraitsType::allocate(alloc, 1), deleter);
        AllocTraitsType::construct(alloc, object.get(), std::forward<ArgTypes>(args)...);

        assert(object != nullptr);
        return object.release();
    }

    template <typename ObjType>
    static void DestroyObject(ObjType* obj) noexcept
    {
        using AllocType = std::allocator<ObjType>;
        using AllocTraitsType = std::allocator_traits<AllocType>;

        if (!obj) return;

        AllocType alloc;
        AllocTraitsType::destroy(alloc, obj);
        AllocTraitsType::deallocate(alloc, obj, 1);
    }

public:
    Node() noexcept
        : m_node_type(NodeType::NULL_OBJECT),
          m_node_value()
    {
    }

    Node(const NodeType type) noexcept
        : m_node_type(type),
          m_node_value(type)
    {
    }

    Node(const Node& rhs)
        : m_node_type(rhs.m_node_type)
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            m_node_value = *(rhs.m_node_value.sequence);
            break;
        case NodeType::MAPPING:
            m_node_value = *(rhs.m_node_value.mapping);
            break;
        case NodeType::NULL_OBJECT:
            m_node_value.mapping = nullptr;
            break;
        case NodeType::BOOLEAN:
            m_node_value.boolean = rhs.m_node_value.boolean;
            break;
        case NodeType::SIGNED_INTEGER:
            m_node_value.signed_int = rhs.m_node_value.signed_int;
            break;
        case NodeType::UNSIGNED_INTEGER:
            m_node_value.unsigned_int = rhs.m_node_value.unsigned_int;
            break;
        case NodeType::FLOAT_NUMBER:
            m_node_value.float_val = rhs.m_node_value.float_val;
            break;
        case NodeType::STRING:
            m_node_value.str = CreateObject<NodeStringType>(*(rhs.m_node_value.str));
            break;
        }
    }

    Node(Node&& rhs)
        : m_node_type(rhs.m_node_type)
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            m_node_value.sequence = rhs.m_node_value.sequence;
            break;
        case NodeType::MAPPING:
            m_node_value.mapping = rhs.m_node_value.mapping;
            break;
        case NodeType::NULL_OBJECT:
            m_node_value.mapping = m_node_value.mapping;
            break;
        case NodeType::BOOLEAN:
            m_node_value.boolean = rhs.m_node_value.boolean;
            break;
        case NodeType::SIGNED_INTEGER:
            m_node_value.signed_int = rhs.m_node_value.signed_int;
            break;
        case NodeType::UNSIGNED_INTEGER:
            m_node_value.unsigned_int = rhs.m_node_value.unsigned_int;
            break;
        case NodeType::FLOAT_NUMBER:
            m_node_value.float_val = rhs.m_node_value.float_val;
            break;
        case NodeType::STRING:
            m_node_value.str = rhs.m_node_value.str;
            rhs.m_node_value.str = nullptr;
            break;
        }

        rhs.m_node_type = NodeType::NULL_OBJECT;
        std::memset(&rhs.m_node_value, 0, sizeof(rhs.m_node_value));
    }

    ~Node() noexcept
    {
        Destroy();
    }

public:
    // factory methods

    static Node Sequence(const NodeSequenceType& sequence) noexcept
    {
        Node node;
        node.m_node_type = NodeType::SEQUENCE;
        node.m_node_value.sequence = CreateObject<NodeSequenceType>(sequence);
        return node;
    }

    static Node Sequence(NodeSequenceType&& sequence) noexcept
    {
        Node node;
        node.m_node_type = NodeType::SEQUENCE;
        node.m_node_value.sequence = CreateObject<NodeSequenceType>(std::move(sequence));
        return node;
    }

    static Node Mapping(const NodeMappingType& mapping) noexcept
    {
        Node node;
        node.m_node_type = NodeType::MAPPING;
        node.m_node_value.mapping = CreateObject<NodeMappingType>(mapping);
        return node;
    }

    static Node Mapping(NodeMappingType&& mapping) noexcept
    {
        Node node;
        node.m_node_type = NodeType::MAPPING;
        node.m_node_value.mapping = CreateObject<NodeMappingType>(std::move(mapping));
        return node;
    }

    static Node BooleanScalar(const NodeBooleanType boolean) noexcept
    {
        Node node;
        node.m_node_type = NodeType::BOOLEAN;
        node.m_node_value.boolean = boolean;
        return node;
    }

    static Node SignedIntegerScalar(const NodeSignedIntType signed_int) noexcept
    {
        Node node;
        node.m_node_type = NodeType::SIGNED_INTEGER;
        node.m_node_value.signed_int = signed_int;
        return node;
    }

    static Node UnsignedIntegerScalar(const NodeUnsignedIntType unsigned_int) noexcept
    {
        Node node;
        node.m_node_type = NodeType::UNSIGNED_INTEGER;
        node.m_node_value.unsigned_int = unsigned_int;
        return node;
    }

    static Node FloatNumberScalar(const NodeFloatNumberType float_val) noexcept
    {
        Node node;
        node.m_node_type = NodeType::FLOAT_NUMBER;
        node.m_node_value.float_val = float_val;
        return node;
    }

    static Node StringScalar(const NodeStringType& str) noexcept
    {
        Node node;
        node.m_node_type = NodeType::STRING;
        node.m_node_value.str = CreateObject<NodeStringType>(str);
        return node;
    }

    static Node StringScalar(NodeStringType&& str) noexcept
    {
        Node node;
        node.m_node_type = NodeType::STRING;
        node.m_node_value.str = CreateObject<NodeStringType>(std::move(str));
        return node;
    }

public:
    Node& operator=(const Node& rhs)
    {
        Node(rhs).Swap(*this);
        return *this;
    }

    Node& operator=(Node&& rhs)
    {
        Node(std::move(rhs)).Swap(*this);
        return *this;
    }

    Node& operator[](std::size_t index)
    {
        if (!IsSequence())
        {
            throw Exception("The target node is not of a sequence type.");
        }

        return m_node_value.sequence->operator[](index);
    }

    const Node& operator[](std::size_t index) const
    {
        if (!IsSequence())
        {
            throw Exception("The target node is not of a sequence type.");
        }

        return m_node_value.sequence->operator[](index);
    }

    Node& operator[](const std::string& key)
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        return m_node_value.mapping->operator[](key);
    }

    Node& operator[](std::string&& key)
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        return m_node_value.mapping->operator[](std::move(key));
    }

    const Node& operator[](const std::string& key) const
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        return m_node_value.mapping->operator[](key);
    }

    const Node& operator[](std::string&& key) const
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        return m_node_value.mapping->operator[](std::move(key));
    }

public:
    NodeType Type() const noexcept
    {
        return m_node_type;
    }

    bool IsSequence() const noexcept
    {
        return m_node_type == NodeType::SEQUENCE;
    }

    bool IsMapping() const noexcept
    {
        return m_node_type == NodeType::MAPPING;
    }

    bool IsNull() const noexcept
    {
        return m_node_type == NodeType::NULL_OBJECT;
    }

    bool IsBoolean() const noexcept
    {
        return m_node_type == NodeType::BOOLEAN;
    }

    bool IsSignedInteger() const noexcept
    {
        return m_node_type == NodeType::SIGNED_INTEGER;
    }

    bool IsUnsignedInteger() const noexcept
    {
        return m_node_type == NodeType::UNSIGNED_INTEGER;
    }

    bool IsInteger() const noexcept
    {
        return IsSignedInteger() || IsUnsignedInteger();
    }

    bool IsFloatNumber() const noexcept
    {
        return m_node_type == NodeType::FLOAT_NUMBER;
    }

    bool IsNumber() const noexcept
    {
        return IsInteger() || IsFloatNumber();
    }

    bool IsString() const noexcept
    {
        return m_node_type == NodeType::STRING;
    }

    bool IsScalar() const noexcept
    {
        return IsBoolean() || IsNumber() || IsString();
    }

    bool IsEmpty() const
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            return m_node_value.sequence->empty();
        case NodeType::MAPPING:
            return m_node_value.mapping->empty();
        case NodeType::STRING:
            return m_node_value.str->empty();
        default:
            throw Exception("The target node is not of a container type.");
        }
    }

    std::size_t Size() const
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            return m_node_value.sequence->size();
        case NodeType::MAPPING:
            return m_node_value.mapping->size();
        case NodeType::STRING:
            return m_node_value.str->size();
        default:
            throw Exception("The target node is not of a container type.");
        }
    }

    NodeSequenceType& ToSequence()
    {
        if (!IsSequence())
        {
            throw Exception("The target node is not of a sequence type.");
        }

        return *(m_node_value.sequence);
    }

    const NodeSequenceType& ToSequence() const
    {
        if (!IsSequence())
        {
            throw Exception("The target node is not of a sequence type.");
        }

        return *(m_node_value.sequence);
    }

    NodeMappingType& ToMapping()
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        return *(m_node_value.mapping);
    }

    const NodeMappingType& ToMapping() const
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        return *(m_node_value.mapping);
    }

    NodeBooleanType& ToBoolean()
    {
        if (!IsBoolean())
        {
            throw Exception("The target node is not of a boolean type.");
        }

        return m_node_value.boolean;
    }

    const NodeBooleanType& ToBoolean() const
    {
        if (!IsBoolean())
        {
            throw Exception("The target node is not of a boolean type.");
        }

        return m_node_value.boolean;
    }

    NodeSignedIntType& ToSignedInteger()
    {
        if (!IsSignedInteger())
        {
            throw Exception("The target node is not of a signed integer type.");
        }

        return m_node_value.signed_int;
    }

    const NodeSignedIntType& ToSignedInteger() const
    {
        if (!IsSignedInteger())
        {
            throw Exception("The target node is not of a signed integer type.");
        }

        return m_node_value.signed_int;
    }

    NodeUnsignedIntType& ToUnsignedInteger()
    {
        if (!IsUnsignedInteger())
        {
            throw Exception("The target node is not of an unsigned integer type.");
        }

        return m_node_value.unsigned_int;
    }

    const NodeUnsignedIntType& ToUnsignedInteger() const
    {
        if (!IsUnsignedInteger())
        {
            throw Exception("The target node is not of an unsigned integer type.");
        }

        return m_node_value.unsigned_int;
    }

    NodeFloatNumberType& ToFloatNumber()
    {
        if (!IsFloatNumber())
        {
            throw Exception("The target node is not of a float number type.");
        }

        return m_node_value.float_val;
    }

    const NodeFloatNumberType& ToFloatNumber() const
    {
        if (!IsFloatNumber())
        {
            throw Exception("The target node is not of a float number type.");
        }

        return m_node_value.float_val;
    }

    NodeStringType& ToString()
    {
        if (!IsString())
        {
            throw Exception("The target node is not of a string type.");
        }

        return *(m_node_value.str);
    }

    const NodeStringType& ToString() const
    {
        if (!IsString())
        {
            throw Exception("The target node is not of a string type.");
        }

        return *(m_node_value.str);
    }

    void Swap(Node& rhs) noexcept
    {
        using std::swap;
        swap(m_node_type, rhs.m_node_type);

        NodeValue tmp;
        std::memcpy(&tmp,              &m_node_value,     sizeof(NodeValue));
        std::memcpy(&m_node_value,     &rhs.m_node_value, sizeof(NodeValue));
        std::memcpy(&rhs.m_node_value, &tmp,           sizeof(NodeValue));
    }

    iterator Begin()
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            return iterator(fkyaml::SequenceIteratorTag(), m_node_value.sequence->begin());
        case NodeType::MAPPING:
            return iterator(fkyaml::MappingIteratorTag(), m_node_value.mapping->begin());
        default:
            throw Exception("The target node is neither of sequence nor mapping types.");
        }
    }

    iterator begin()
    {
        return Begin();
    }

    const_iterator Begin() const
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            return const_iterator(fkyaml::SequenceIteratorTag(), m_node_value.sequence->begin());
        case NodeType::MAPPING:
            return const_iterator(fkyaml::MappingIteratorTag(), m_node_value.mapping->begin());
        default:
            throw Exception("The target node is neither of sequence nor mapping types.");
        }
    }

    iterator End()
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            return iterator(fkyaml::SequenceIteratorTag(), m_node_value.sequence->end());
        case NodeType::MAPPING:
            return iterator(fkyaml::MappingIteratorTag(), m_node_value.mapping->end());
        default:
            throw Exception("The target node is neither of sequence nor mapping types.");
        }
    }

    iterator end()
    {
        return End();
    }

    const_iterator End() const
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            return const_iterator(fkyaml::SequenceIteratorTag(), m_node_value.sequence->end());
        case NodeType::MAPPING:
            return const_iterator(fkyaml::MappingIteratorTag(), m_node_value.mapping->end());
        default:
            throw Exception("The target node is neither of sequence nor mapping types.");
        }
    }

    const_iterator end() const
    {
        return end();
    }

private:
    void Destroy() noexcept
    {
        m_node_value.Destroy(m_node_type);
        m_node_type = NodeType::NULL_OBJECT;
    }

private:
    NodeType  m_node_type;
    NodeValue m_node_value;
};

} // namespace fkyaml

namespace std
{

template <>
inline void swap<fkyaml::Node>(fkyaml::Node& lhs, fkyaml::Node& rhs) noexcept(
    std::is_nothrow_move_constructible<fkyaml::Node>::value
    && std::is_nothrow_move_assignable<fkyaml::Node>::value)
{
    lhs.Swap(rhs);
}

} // namespace std

#endif /* FK_YAML_NODE_HPP_ */