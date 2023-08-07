/**
 * Node.hpp - Implementation of YAML node data structure.
 * 
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_NODE_HPP_
#define FK_YAML_NODE_HPP_

#include <cassert>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "fkYAML/Exception.hpp"

namespace fkyaml
{

enum class NodeType
{
    SEQUENCE,
    MAPPING,
    NULL_OBJECT,
    BOOLEAN,
    SIGNED_INTEGER,
    UNSIGNED_INTEGER,
    FLOAT_NUMBER,
    STRING,
};

class Node;

using NodeSequenceType    = std::vector<Node>;
using NodeMappingType     = std::unordered_map<std::string, Node>;
using NodeBooleanType     = bool;
using NodeSignedIntType   = int64_t;
using NodeUnsignedIntType = uint64_t;
using NodeFloatNumberType = double;
using NodeStringType      = std::string;

class Node
{
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
        }

        NodeValue(NodeSequenceType&& seq) noexcept
            : sequence(CreateObject<NodeSequenceType>(std::move(seq)))
        {}

        NodeValue& operator=(NodeSequenceType&& seq) noexcept
        {
            sequence = CreateObject<NodeSequenceType>(std::move(seq));
        }

        NodeValue(const NodeMappingType& map) noexcept
            : mapping(CreateObject<NodeMappingType>(map))
        {}

        NodeValue& operator=(const NodeMappingType& map) noexcept
        {
            mapping = CreateObject<NodeMappingType>(map);
        }

        NodeValue(NodeMappingType&& map) noexcept
            : mapping(CreateObject<NodeMappingType>(std::move(map)))
        {}

        NodeValue& operator=(NodeMappingType&& map) noexcept
        {
            mapping = CreateObject<NodeMappingType>(std::move(map));
        }

        NodeValue(const NodeBooleanType bool_val) noexcept
            : boolean(bool_val)
        {}

        NodeValue& operator=(const NodeBooleanType bool_val) noexcept
        {
            boolean = bool_val;
        }

        NodeValue(const NodeSignedIntType int_val) noexcept
            : signed_int(int_val)
        {}

        NodeValue& operator=(const NodeSignedIntType int_val) noexcept
        {
            signed_int = int_val;
        }

        NodeValue(const NodeUnsignedIntType uint_val) noexcept
            : unsigned_int(uint_val)
        {}

        NodeValue& operator=(const NodeUnsignedIntType uint_val) noexcept
        {
            unsigned_int = uint_val;
        }

        NodeValue(const NodeFloatNumberType f_val) noexcept
            : float_val(f_val)
        {
        }

        NodeValue& operator=(const NodeFloatNumberType f_val) noexcept
        {
            float_val = f_val;
        }

        NodeValue(const NodeStringType& str_val) noexcept
            : str(CreateObject<NodeStringType>(str_val))
        {}

        NodeValue& operator=(const NodeStringType& str_val) noexcept
        {
            str = CreateObject<NodeStringType>(str_val);
        }

        NodeValue(NodeStringType&& str_val) noexcept
            : str(CreateObject<NodeStringType>(std::move(str_val)))
        {}

        NodeValue& operator=(NodeStringType&& str_val) noexcept
        {
            str = CreateObject<NodeStringType>(std::move(str_val));
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
            AllocType::deallocate(obj, 1);
        };

        std::unique_ptr<ObjType, decltype(deleter)> object(AllocType::allocate(1), deleter);
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
        AllocType::deallocate(obj, 1);
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

    template <NodeType, typename ValueType>
    Node(ValueType val);

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

    template <typename T, bool IsPrimitive = std::is_scalar<T>::value>
    struct RetValue
    {
        using Type = typename std::remove_reference<T>::type&;
    };

    template <typename T>
    struct RetValue<T, true>
    {
        using Type = typename std::remove_reference<T>::type;
    };

    template <typename T>
    using RetValueType = typename RetValue<T>::Type;

    template <typename T>
    RetValueType<T> Value();

    template <typename T>
    const RetValueType<T> Value() const;

    void Swap(Node& rhs) noexcept
    {
        using std::swap;
        swap(m_node_type, rhs.m_node_type);

        NodeValue tmp;
        std::memcpy(&tmp,           &m_node_value,     sizeof(NodeValue));
        std::memcpy(&m_node_value,     &rhs.m_node_value, sizeof(NodeValue));
        std::memcpy(&rhs.m_node_value, &tmp,           sizeof(NodeValue));
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

template <NodeType, typename ValueType>
Node::Node(ValueType val)
{
    static_assert(false, "invalid instantiation of template Node constructor.");
}

template <>
Node::Node<NodeType::SEQUENCE>(const NodeSequenceType& sequence)
    : m_node_type(NodeType::SEQUENCE),
      m_node_value(sequence)
{
}

template <>
Node::Node<NodeType::SEQUENCE>(NodeSequenceType&& sequence)
    : m_node_type(NodeType::SEQUENCE),
      m_node_value(std::move(sequence))
{
}

template <>
Node::Node<NodeType::MAPPING>(const NodeMappingType& mapping)
    : m_node_type(NodeType::MAPPING),
      m_node_value(mapping)
{
}

template <>
Node::Node<NodeType::MAPPING>(NodeMappingType&& mapping)
    : m_node_type(NodeType::MAPPING),
      m_node_value(std::move(mapping))
{
}

template <>
Node::Node<NodeType::BOOLEAN>(const NodeBooleanType boolean)
    : m_node_type(NodeType::BOOLEAN),
      m_node_value(boolean)
{
}

template <>
Node::Node<NodeType::SIGNED_INTEGER>(const NodeSignedIntType signed_int)
    : m_node_type(NodeType::SIGNED_INTEGER),
      m_node_value(signed_int)
{
}

template <>
Node::Node<NodeType::UNSIGNED_INTEGER>(const NodeUnsignedIntType unsigned_int)
    : m_node_type(NodeType::UNSIGNED_INTEGER),
      m_node_value(unsigned_int)
{
}

template <>
Node::Node<NodeType::FLOAT_NUMBER>(const NodeFloatNumberType float_val)
    : m_node_type(NodeType::FLOAT_NUMBER),
      m_node_value(float_val)
{
}

template <>
Node::Node<NodeType::STRING>(const NodeStringType& str)
    : m_node_type(NodeType::STRING),
      m_node_value(str)
{
}

template <>
Node::Node<NodeType::STRING>(NodeStringType&& str)
    : m_node_type(NodeType::STRING),
      m_node_value(std::move(str))
{
}

template <typename RetType>
inline Node::RetValueType<RetType> Node::Value()
{
    static_assert(false, "invalid temlate instantiation for Node::Value<T>().");
}

template <typename RetType>
inline const Node::RetValueType<RetType> Node::Value() const
{
    static_assert(false, "invalid template instantiation for Node::Value<T>() const");
}

template <>
inline Node::RetValueType<NodeSequenceType> Node::Value<NodeSequenceType>()
{
    if (!IsSequence())
    {
        throw Exception("The target node is not of a sequence type.");
    }
    return *(m_node_value.sequence);
}

template <>
inline const Node::RetValueType<NodeSequenceType> Node::Value<NodeSequenceType>() const
{
    if (!IsSequence())
    {
        throw Exception("The target node is not of a sequence type.");
    }
    return *(m_node_value.sequence);
}

template <>
inline Node::RetValueType<NodeMappingType> Node::Value<NodeMappingType>()
{
    if (!IsMapping())
    {
        throw Exception("The target node is not of a mapping type.");
    }
    return *(m_node_value.mapping);
}

template <>
inline const Node::RetValueType<NodeMappingType> Node::Value<NodeMappingType>() const
{
    if (!IsMapping())
    {
        throw Exception("The target node is not of a mapping type.");
    }
    return *(m_node_value.mapping);
}

template <>
inline Node::RetValueType<NodeBooleanType> Node::Value<NodeBooleanType>()
{
    if (!IsBoolean())
    {
        throw Exception("The target node is not of a boolean type.");
    }
    return m_node_value.boolean;
}

template <>
inline const Node::RetValueType<NodeBooleanType> Node::Value<NodeBooleanType>() const
{
    if (!IsBoolean())
    {
        throw Exception("The target node is not of a boolean type.");
    }
    return m_node_value.boolean;
}

template <>
inline Node::RetValueType<int8_t> Node::Value<int8_t>()
{
    if (!IsSignedInteger())
    {
        throw Exception("The target node is not of a signed integer type");
    }
    return static_cast<int8_t>(m_node_value.signed_int);
}

template <>
inline const Node::RetValueType<int8_t> Node::Value<int8_t>() const
{
    if (!IsSignedInteger())
    {
        throw Exception("The target node is not of a signed integer type");
    }
    return static_cast<int8_t>(m_node_value.signed_int);
}

template <>
inline Node::RetValueType<int16_t> Node::Value<int16_t>()
{
    if (!IsSignedInteger())
    {
        throw Exception("The target node is not of a signed integer type.");
    }
    return static_cast<int16_t>(m_node_value.signed_int);
}

template <>
inline const Node::RetValueType<int16_t> Node::Value<int16_t>() const
{
    if (!IsSignedInteger())
    {
        throw Exception("The target node is not of a signed integer type.");
    }
    return static_cast<int16_t>(m_node_value.signed_int);
}

template <>
inline Node::RetValueType<int32_t> Node::Value<int32_t>()
{
    if (!IsSignedInteger())
    {
        throw Exception("The target node is not of a signed integer type.");
    }
    return static_cast<int32_t>(m_node_value.signed_int);
}

template <>
inline const Node::RetValueType<int32_t> Node::Value<int32_t>() const
{
    if (!IsSignedInteger())
    {
        throw Exception("The target node is not of a signed integer type.");
    }
    return static_cast<int32_t>(m_node_value.signed_int);
}

template <>
inline Node::RetValueType<int64_t> Node::Value<int64_t>()
{
    if (!IsSignedInteger())
    {
        throw Exception("The target node is not of a signed integer type.");
    }
    return static_cast<int64_t>(m_node_value.signed_int);
}

template <>
inline const Node::RetValueType<int64_t> Node::Value<int64_t>() const
{
    if (!IsSignedInteger())
    {
        throw Exception("The target node is not of a signed integer type.");
    }
    return static_cast<int64_t>(m_node_value.signed_int);
}

template <>
inline Node::RetValueType<uint8_t> Node::Value<uint8_t>()
{
    if (!IsUnsignedInteger())
    {
        throw Exception("The target node is not of a unsigned integer type");
    }
    return static_cast<uint8_t>(m_node_value.unsigned_int);
}

template <>
inline const Node::RetValueType<uint8_t> Node::Value<uint8_t>() const
{
    if (!IsUnsignedInteger())
    {
        throw Exception("The target node is not of a unsigned integer type");
    }
    return static_cast<uint8_t>(m_node_value.unsigned_int);
}

template <>
inline Node::RetValueType<uint16_t> Node::Value<uint16_t>()
{
    if (!IsUnsignedInteger())
    {
        throw Exception("The target node is not of a unsigned integer type.");
    }
    return static_cast<uint16_t>(m_node_value.unsigned_int);
}

template <>
inline const Node::RetValueType<uint16_t> Node::Value<uint16_t>() const
{
    if (!IsUnsignedInteger())
    {
        throw Exception("The target node is not of a unsigned integer type.");
    }
    return static_cast<uint16_t>(m_node_value.unsigned_int);
}

template <>
inline Node::RetValueType<uint32_t> Node::Value<uint32_t>()
{
    if (!IsUnsignedInteger())
    {
        throw Exception("The target node is not of a unsigned integer type.");
    }
    return static_cast<uint32_t>(m_node_value.unsigned_int);
}

template <>
inline const Node::RetValueType<uint32_t> Node::Value<uint32_t>() const
{
    if (!IsUnsignedInteger())
    {
        throw Exception("The target node is not of a unsigned integer type.");
    }
    return static_cast<uint32_t>(m_node_value.unsigned_int);
}

template <>
inline Node::RetValueType<uint64_t> Node::Value<uint64_t>()
{
    if (!IsUnsignedInteger())
    {
        throw Exception("The target node is not of a unsigned integer type.");
    }
    return static_cast<uint64_t>(m_node_value.unsigned_int);
}

template <>
inline const Node::RetValueType<uint64_t> Node::Value<uint64_t>() const
{
    if (!IsUnsignedInteger())
    {
        throw Exception("The target node is not of a unsigned integer type.");
    }
    return static_cast<uint64_t>(m_node_value.unsigned_int);
}

template <>
inline Node::RetValueType<float> Node::Value<float>()
{
    if (!IsFloatNumber())
    {
        throw Exception("The target node is not of a floating point type.");
    }
    return static_cast<float>(m_node_value.float_val);
}

template <>
inline const Node::RetValueType<float> Node::Value<float>() const
{
    if (!IsFloatNumber())
    {
        throw Exception("The target node is not of a floating point type.");
    }
    return static_cast<float>(m_node_value.float_val);
}

template <>
inline Node::RetValueType<double> Node::Value<double>()
{
    if (!IsFloatNumber())
    {
        throw Exception("The target node is not of a floating point type.");
    }
    return static_cast<double>(m_node_value.float_val);
}

template <>
inline const Node::RetValueType<double> Node::Value<double>() const
{
    if (!IsFloatNumber())
    {
        throw Exception("The target node is not of a floating point type.");
    }
    return static_cast<double>(m_node_value.float_val);
}

template <>
inline Node::RetValueType<NodeStringType> Node::Value<NodeStringType>()
{
    if (!IsString())
    {
        throw Exception("The target node is not of a string type.");
    }
    return *(m_node_value.str);
}

template <>
inline const Node::RetValueType<NodeStringType> Node::Value<NodeStringType>() const
{
    if (!IsString())
    {
        throw Exception("The target node is not of a string type.");
    }
    return *(m_node_value.str);
}

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