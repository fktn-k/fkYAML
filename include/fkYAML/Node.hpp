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
#include "fkYAML/Iterator.hpp"
#include "fkYAML/NodeType.hpp"

namespace fkyaml
{

class Node
{
public:
    using iterator = Iterator<Node>;
    using const_iterator = Iterator<const Node>;

    using sequence_type = std::vector<Node>;
    using mapping_type = std::unordered_map<std::string, Node>;
    using boolean_type = bool;
    using signed_int_type = int64_t;
    using unsigned_int_type = uint64_t;
    using float_number_type = double;
    using string_type = std::string;

private:
    union NodeValue
    {
        NodeValue()
        {
        }

        ~NodeValue()
        {
        }

        NodeValue(NodeType type) noexcept
        {
            switch (type)
            {
            case NodeType::SEQUENCE:
                sequence = CreateObject<sequence_type>();
                break;
            case NodeType::MAPPING:
                mapping = CreateObject<mapping_type>();
                break;
            case NodeType::NULL_OBJECT:
                mapping = nullptr;
                break;
            case NodeType::BOOLEAN:
                boolean = static_cast<boolean_type>(false);
                break;
            case NodeType::SIGNED_INTEGER:
                signed_int = static_cast<signed_int_type>(0);
                break;
            case NodeType::UNSIGNED_INTEGER:
                unsigned_int = static_cast<unsigned_int_type>(0);
                break;
            case NodeType::FLOAT_NUMBER:
                float_val = static_cast<float_number_type>(0.0);
                break;
            case NodeType::STRING:
                str = CreateObject<string_type>("");
                break;
            }
        }

        NodeValue(const sequence_type& seq) noexcept
            : sequence(CreateObject<sequence_type>(seq))
        {
        }

        NodeValue& operator=(const sequence_type& seq) noexcept
        {
            sequence = CreateObject<sequence_type>(seq);
            return *this;
        }

        NodeValue(sequence_type&& seq) noexcept
            : sequence(CreateObject<sequence_type>(std::move(seq)))
        {
        }

        NodeValue& operator=(sequence_type&& seq) noexcept
        {
            sequence = CreateObject<sequence_type>(std::move(seq));
            return *this;
        }

        NodeValue(const mapping_type& map) noexcept
            : mapping(CreateObject<mapping_type>(map))
        {
        }

        NodeValue& operator=(const mapping_type& map) noexcept
        {
            mapping = CreateObject<mapping_type>(map);
            return *this;
        }

        NodeValue(mapping_type&& map) noexcept
            : mapping(CreateObject<mapping_type>(std::move(map)))
        {
        }

        NodeValue& operator=(mapping_type&& map) noexcept
        {
            mapping = CreateObject<mapping_type>(std::move(map));
            return *this;
        }

        NodeValue(const boolean_type bool_val) noexcept
            : boolean(bool_val)
        {
        }

        NodeValue& operator=(const boolean_type bool_val) noexcept
        {
            boolean = bool_val;
            return *this;
        }

        NodeValue(const signed_int_type int_val) noexcept
            : signed_int(int_val)
        {
        }

        NodeValue& operator=(const signed_int_type int_val) noexcept
        {
            signed_int = int_val;
            return *this;
        }

        NodeValue(const unsigned_int_type uint_val) noexcept
            : unsigned_int(uint_val)
        {
        }

        NodeValue& operator=(const unsigned_int_type uint_val) noexcept
        {
            unsigned_int = uint_val;
            return *this;
        }

        NodeValue(const float_number_type f_val) noexcept
            : float_val(f_val)
        {
        }

        NodeValue& operator=(const float_number_type f_val) noexcept
        {
            float_val = f_val;
            return *this;
        }

        NodeValue(const string_type& str_val) noexcept
            : str(CreateObject<string_type>(str_val))
        {
        }

        NodeValue& operator=(const string_type& str_val) noexcept
        {
            str = CreateObject<string_type>(str_val);
            return *this;
        }

        NodeValue(string_type&& str_val) noexcept
            : str(CreateObject<string_type>(std::move(str_val)))
        {
        }

        NodeValue& operator=(string_type&& str_val) noexcept
        {
            str = CreateObject<string_type>(std::move(str_val));
            return *this;
        }

        void Destroy(NodeType type) noexcept
        {
            if (type == NodeType::SEQUENCE || type == NodeType::MAPPING)
            {
                std::vector<Node> stack;

                if (type == NodeType::SEQUENCE)
                {
                    stack.reserve(sequence->size());
                    std::move(sequence->begin(), sequence->end(), std::back_inserter(stack));
                }
                else
                {
                    stack.reserve(mapping->size());
                    for (auto&& it : *mapping)
                    {
                        stack.push_back(std::move(it.second));
                    }
                }

                while (!stack.empty())
                {
                    Node current_node(std::move(stack.back()));
                    stack.pop_back();

                    if (current_node.IsSequence())
                    {
                        std::move(
                            current_node.m_node_value.sequence->begin(),
                            current_node.m_node_value.sequence->end(),
                            std::back_inserter(stack));
                        current_node.m_node_value.sequence->clear();
                    }
                    else if (current_node.IsMapping())
                    {
                        for (auto&& it : *current_node.m_node_value.mapping)
                        {
                            stack.push_back(std::move(it.second));
                        }
                        current_node.m_node_value.mapping->clear();
                    }
                }
            }

            switch (type)
            {
            case NodeType::SEQUENCE:
                DestroyObject<sequence_type>(sequence);
                sequence = nullptr;
                break;
            case NodeType::MAPPING:
                DestroyObject<mapping_type>(mapping);
                mapping = nullptr;
                break;
            case NodeType::STRING:
                DestroyObject<string_type>(str);
                str = nullptr;
                break;
            default:
                break;
            }
        }

        sequence_type* sequence;
        mapping_type* mapping;
        boolean_type boolean;
        signed_int_type signed_int;
        unsigned_int_type unsigned_int;
        float_number_type float_val;
        string_type* str;
    };

private:
    template <typename ObjType, typename... ArgTypes>
    static ObjType* CreateObject(ArgTypes&&... args) noexcept
    {
        using AllocType = std::allocator<ObjType>;
        using AllocTraitsType = std::allocator_traits<AllocType>;

        AllocType alloc;
        auto deleter = [&](ObjType* obj) {
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
        if (!obj)
            return;

        std::allocator<ObjType> alloc;
        std::allocator_traits<decltype(alloc)>::destroy(alloc, obj);
        std::allocator_traits<decltype(alloc)>::deallocate(alloc, obj, 1);
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
            m_node_value.str = CreateObject<string_type>(*(rhs.m_node_value.str));
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
            rhs.m_node_value.sequence = nullptr;
            break;
        case NodeType::MAPPING:
            m_node_value.mapping = rhs.m_node_value.mapping;
            rhs.m_node_value.mapping = nullptr;
            break;
        case NodeType::NULL_OBJECT:
            m_node_value.mapping = rhs.m_node_value.mapping = nullptr;
            break;
        case NodeType::BOOLEAN:
            m_node_value.boolean = rhs.m_node_value.boolean;
            rhs.m_node_value.boolean = static_cast<boolean_type>(false);
            break;
        case NodeType::SIGNED_INTEGER:
            m_node_value.signed_int = rhs.m_node_value.signed_int;
            rhs.m_node_value.signed_int = static_cast<signed_int_type>(0);
            break;
        case NodeType::UNSIGNED_INTEGER:
            m_node_value.unsigned_int = rhs.m_node_value.unsigned_int;
            rhs.m_node_value.unsigned_int = static_cast<unsigned_int_type>(0);
            break;
        case NodeType::FLOAT_NUMBER:
            m_node_value.float_val = rhs.m_node_value.float_val;
            rhs.m_node_value.float_val = static_cast<float_number_type>(0.0);
            break;
        case NodeType::STRING:
            m_node_value.str = rhs.m_node_value.str;
            rhs.m_node_value.str = nullptr;
            break;
        }

        rhs.m_node_type = NodeType::NULL_OBJECT;
        rhs.m_node_value.mapping = nullptr;
    }

    ~Node() noexcept
    {
        Destroy();
    }

public:
    // factory methods

    static Node Sequence(const sequence_type& sequence) noexcept
    {
        Node node;
        node.m_node_type = NodeType::SEQUENCE;
        node.m_node_value.sequence = CreateObject<sequence_type>(sequence);
        return node;
    }

    static Node Sequence(sequence_type&& sequence) noexcept
    {
        Node node;
        node.m_node_type = NodeType::SEQUENCE;
        node.m_node_value.sequence = CreateObject<sequence_type>(std::move(sequence));
        return node;
    }

    static Node Mapping(const mapping_type& mapping) noexcept
    {
        Node node;
        node.m_node_type = NodeType::MAPPING;
        node.m_node_value.mapping = CreateObject<mapping_type>(mapping);
        return node;
    }

    static Node Mapping(mapping_type&& mapping) noexcept
    {
        Node node;
        node.m_node_type = NodeType::MAPPING;
        node.m_node_value.mapping = CreateObject<mapping_type>(std::move(mapping));
        return node;
    }

    static Node BooleanScalar(const boolean_type boolean) noexcept
    {
        Node node;
        node.m_node_type = NodeType::BOOLEAN;
        node.m_node_value.boolean = boolean;
        return node;
    }

    static Node SignedIntegerScalar(const signed_int_type signed_int) noexcept
    {
        Node node;
        node.m_node_type = NodeType::SIGNED_INTEGER;
        node.m_node_value.signed_int = signed_int;
        return node;
    }

    static Node UnsignedIntegerScalar(const unsigned_int_type unsigned_int) noexcept
    {
        Node node;
        node.m_node_type = NodeType::UNSIGNED_INTEGER;
        node.m_node_value.unsigned_int = unsigned_int;
        return node;
    }

    static Node FloatNumberScalar(const float_number_type float_val) noexcept
    {
        Node node;
        node.m_node_type = NodeType::FLOAT_NUMBER;
        node.m_node_value.float_val = float_val;
        return node;
    }

    static Node StringScalar(const string_type& str) noexcept
    {
        Node node;
        node.m_node_type = NodeType::STRING;
        node.m_node_value.str = CreateObject<string_type>(str);
        return node;
    }

    static Node StringScalar(string_type&& str) noexcept
    {
        Node node;
        node.m_node_type = NodeType::STRING;
        node.m_node_value.str = CreateObject<string_type>(std::move(str));
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

    sequence_type& ToSequence()
    {
        if (!IsSequence())
        {
            throw Exception("The target node is not of a sequence type.");
        }

        return *(m_node_value.sequence);
    }

    const sequence_type& ToSequence() const
    {
        if (!IsSequence())
        {
            throw Exception("The target node is not of a sequence type.");
        }

        return *(m_node_value.sequence);
    }

    mapping_type& ToMapping()
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        return *(m_node_value.mapping);
    }

    const mapping_type& ToMapping() const
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        return *(m_node_value.mapping);
    }

    boolean_type& ToBoolean()
    {
        if (!IsBoolean())
        {
            throw Exception("The target node is not of a boolean type.");
        }

        return m_node_value.boolean;
    }

    const boolean_type& ToBoolean() const
    {
        if (!IsBoolean())
        {
            throw Exception("The target node is not of a boolean type.");
        }

        return m_node_value.boolean;
    }

    signed_int_type& ToSignedInteger()
    {
        if (!IsSignedInteger())
        {
            throw Exception("The target node is not of a signed integer type.");
        }

        return m_node_value.signed_int;
    }

    const signed_int_type& ToSignedInteger() const
    {
        if (!IsSignedInteger())
        {
            throw Exception("The target node is not of a signed integer type.");
        }

        return m_node_value.signed_int;
    }

    unsigned_int_type& ToUnsignedInteger()
    {
        if (!IsUnsignedInteger())
        {
            throw Exception("The target node is not of an unsigned integer type.");
        }

        return m_node_value.unsigned_int;
    }

    const unsigned_int_type& ToUnsignedInteger() const
    {
        if (!IsUnsignedInteger())
        {
            throw Exception("The target node is not of an unsigned integer type.");
        }

        return m_node_value.unsigned_int;
    }

    float_number_type& ToFloatNumber()
    {
        if (!IsFloatNumber())
        {
            throw Exception("The target node is not of a float number type.");
        }

        return m_node_value.float_val;
    }

    const float_number_type& ToFloatNumber() const
    {
        if (!IsFloatNumber())
        {
            throw Exception("The target node is not of a float number type.");
        }

        return m_node_value.float_val;
    }

    string_type& ToString()
    {
        if (!IsString())
        {
            throw Exception("The target node is not of a string type.");
        }

        return *(m_node_value.str);
    }

    const string_type& ToString() const
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
        std::memcpy(&tmp, &m_node_value, sizeof(NodeValue));
        std::memcpy(&m_node_value, &rhs.m_node_value, sizeof(NodeValue));
        std::memcpy(&rhs.m_node_value, &tmp, sizeof(NodeValue));
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
    NodeType m_node_type;
    NodeValue m_node_value;
};

using NodeSequenceType = typename Node::sequence_type;
using NodeMappingType = typename Node::mapping_type;
using NodeBooleanType = typename Node::boolean_type;
using NodeSignedIntType = typename Node::signed_int_type;
using NodeUnsignedIntType = typename Node::unsigned_int_type;
using NodeFloatNumberType = typename Node::float_number_type;
using NodeStringType = typename Node::string_type;

} // namespace fkyaml

namespace std
{

template <>
inline void swap<fkyaml::Node>(fkyaml::Node& lhs, fkyaml::Node& rhs) noexcept(
    std::is_nothrow_move_constructible<fkyaml::Node>::value&& std::is_nothrow_move_assignable<fkyaml::Node>::value)
{
    lhs.Swap(rhs);
}

} // namespace std

#endif /* FK_YAML_NODE_HPP_ */