/**
 * @file Node.hpp
 * @brief Implementation of YAML node data structure.
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

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
namespace fkyaml
{

/**
 * @class Node
 * @brief A class to store value of YAML nodes.
 */
class Node
{
public:
    /** A type for iterators of Node containers. */
    using iterator = Iterator<Node>;
    /** A type for constant iterators of Node containers. */
    using const_iterator = Iterator<const Node>;

    /** A type for sequence Node values. */
    using sequence_type = std::vector<Node>;
    /** A type for mapping Node values. */
    using mapping_type = std::unordered_map<std::string, Node>;
    /** A type for boolean Node values. */
    using boolean_type = bool;
    /** A type for signed integer Node values. */
    using signed_int_type = int64_t;
    /** A type for unsigned integer Node values. */
    using unsigned_int_type = uint64_t;
    /** A type for float number Node values. */
    using float_number_type = double;
    /** A type for string Npde values. */
    using string_type = std::string;

private:
    /**
     * @union NodeValue
     * @brief The actual storage for a YAML node value of the @ref Node class.
     * @details This union combines the different sotrage types for the YAML value types defined in @ref NodeType.
     * @note Container types are stored as pointers so that the size of this union should not exceed 64 bits by default.
     */
    union NodeValue
    {
        /**
         * @brief Construct a new Node Value object for null types.
         */
        NodeValue() = default;

        /**
         * @brief Construct a new Node Value object with Node types. The default value for the specified type will be
         * assigned.
         *
         * @param[in] type A Node type.
         */
        explicit NodeValue(NodeType type)
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

        /**
         * @brief Destroy the existing Node value. This process is recursive if the specified node type is fpr
         * containers.
         *
         * @param[in] type A Node type to determine which Node value is destroyed.
         */
        void Destroy(NodeType type)
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

        /** A pointer to the value of sequence type. */
        sequence_type* sequence;
        /** A pointer to the value of mapping type. This pointer is also used when node type is null. */
        mapping_type* mapping;
        /** A value of boolean type. */
        boolean_type boolean;
        /** A value of signed integer type. */
        signed_int_type signed_int;
        /** A value of unsigned integer type. */
        unsigned_int_type unsigned_int;
        /** A value of float number type. */
        float_number_type float_val;
        /** A pointer to the value of string type. */
        string_type* str;
    };

private:
    /**
     * @brief Allocates and constructs an object with specified type and arguments.
     *
     * @tparam ObjType The target object type.
     * @tparam ArgTypes The packed argument types for constructor arguments.
     * @param[in] args A parameter pack for constructor arguments of the target object type.
     * @return ObjType* An address of allocated memory on the heap.
     */
    template <typename ObjType, typename... ArgTypes>
    static ObjType* CreateObject(ArgTypes&&... args)
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

    /**
     * @brief Destroys and deallocates an object with specified type.
     *
     * @tparam ObjType The target object type.
     * @param[in] obj A pointer to the target object to be destroyed.
     */
    template <typename ObjType>
    static void DestroyObject(ObjType* obj)
    {
        if (!obj)
        {
            return;
        }

        std::allocator<ObjType> alloc;
        std::allocator_traits<decltype(alloc)>::destroy(alloc, obj);
        std::allocator_traits<decltype(alloc)>::deallocate(alloc, obj, 1);
    }

public:
    /**
     * @brief Construct a new Node object of null type.
     */
    Node() noexcept
        : m_node_type(NodeType::NULL_OBJECT),
          m_node_value()
    {
    }

    /**
     * @brief Construct a new Node object with a specified type.
     *
     * @param[in] type A YAML node value type.
     */
    explicit Node(const NodeType type)
        : m_node_type(type),
          m_node_value(type)
    {
    }

    /**
     * @brief Copy constructor of the Node class.
     *
     * @param[in] rhs A Node object to be copied with.
     */
    Node(const Node& rhs)
        : m_node_type(rhs.m_node_type)
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            m_node_value.sequence = CreateObject<sequence_type>(*(rhs.m_node_value.sequence));
            break;
        case NodeType::MAPPING:
            m_node_value.mapping = CreateObject<mapping_type>(*(rhs.m_node_value.mapping));
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

    /**
     * @brief Move constructor of the Node class.
     *
     * @param[in] rhs A Node object to be moved from.
     */
    Node(Node&& rhs) noexcept
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

    /**
     * @brief Destroy the Node object and its value storage.
     */
    ~Node() noexcept // NOLINT(bugprone-exception-escape)
    {
        m_node_value.Destroy(m_node_type);
        m_node_type = NodeType::NULL_OBJECT;
    }

public:
    /**
     * @brief A factory method for sequence Node objects without sequence_type objects.
     *
     * @return Node A constructed Node object of sequence type.
     */
    static Node Sequence()
    {
        Node node;
        node.m_node_type = NodeType::SEQUENCE;
        node.m_node_value.sequence = CreateObject<sequence_type>();
        return node;
    }

    /**
     * @brief A factory method for sequence Node objects with lvalue sequence_type objects.
     *
     * @param[in] sequence A lvalue source of sequence type.
     * @return Node A constructed Node object of sequence type.
     */
    static Node Sequence(const sequence_type& sequence)
    {
        Node node;
        node.m_node_type = NodeType::SEQUENCE;
        node.m_node_value.sequence = CreateObject<sequence_type>(sequence);
        return node;
    }

    /**
     * @brief A factory method for sequence Node objects with rvalue sequence_type objects.
     *
     * @param[in] sequence A rvalue source of sequence type.
     * @return Node A constructed Node object of sequence type.
     */
    static Node Sequence(sequence_type&& sequence)
    {
        Node node;
        node.m_node_type = NodeType::SEQUENCE;
        node.m_node_value.sequence = CreateObject<sequence_type>(std::move(sequence));
        return node;
    }

    /**
     * @brief A factory method for mapping Node objects without mapping_type objects.
     *
     * @return Node A constructed Node object of mapping type.
     */
    static Node Mapping()
    {
        Node node;
        node.m_node_type = NodeType::MAPPING;
        node.m_node_value.mapping = CreateObject<mapping_type>();
        return node;
    }

    /**
     * @brief A factory method for mapping Node objects with lvalue mapping_type objects.
     *
     * @param[in] mapping A lvalue source of mapping type.
     * @return Node A constructed Node object of mapping type.
     */
    static Node Mapping(const mapping_type& mapping)
    {
        Node node;
        node.m_node_type = NodeType::MAPPING;
        node.m_node_value.mapping = CreateObject<mapping_type>(mapping);
        return node;
    }

    /**
     * @brief A factory method for mapping Node objects with rvalue mapping_type objects.
     *
     * @param[in] mapping A rvalue source of mapping type.
     * @return Node A constructed Node object of mapping type.
     */
    static Node Mapping(mapping_type&& mapping)
    {
        Node node;
        node.m_node_type = NodeType::MAPPING;
        node.m_node_value.mapping = CreateObject<mapping_type>(std::move(mapping));
        return node;
    }

    /**
     * @brief A factory method for boolean scalar Node objects.
     *
     * @param[in] boolean A source of boolean type.
     * @return Node A constructed Node object of boolean type.
     */
    static Node BooleanScalar(const boolean_type boolean) noexcept
    {
        Node node;
        node.m_node_type = NodeType::BOOLEAN;
        node.m_node_value.boolean = boolean;
        return node;
    }

    /**
     * @brief A factory method for signed integer scalar Node objects.
     *
     * @param[in] signed_int A source of signed integer type.
     * @return Node A constructed Node object of signed integer type.
     */
    static Node SignedIntegerScalar(const signed_int_type signed_int) noexcept
    {
        Node node;
        node.m_node_type = NodeType::SIGNED_INTEGER;
        node.m_node_value.signed_int = signed_int;
        return node;
    }

    /**
     * @brief A factory method for unsigned integer scalar Node objects.
     *
     * @param[in] unsigned_int A source of unsigned integer type.
     * @return Node A constructed Node object of unsigned integer type.
     */
    static Node UnsignedIntegerScalar(const unsigned_int_type unsigned_int) noexcept
    {
        Node node;
        node.m_node_type = NodeType::UNSIGNED_INTEGER;
        node.m_node_value.unsigned_int = unsigned_int;
        return node;
    }

    /**
     * @brief A factory method for float number scalar Node objects.
     *
     * @param[in] float_val A source of unsigned integer type.
     * @return Node A constructed Node object of float number type.
     */
    static Node FloatNumberScalar(const float_number_type float_val) noexcept
    {
        Node node;
        node.m_node_type = NodeType::FLOAT_NUMBER;
        node.m_node_value.float_val = float_val;
        return node;
    }

    /**
     * @brief A factory method for string Node objects without string_type objects.
     *
     * @return Node A constructed Node object of string type.
     */
    static Node StringScalar()
    {
        Node node;
        node.m_node_type = NodeType::STRING;
        node.m_node_value.str = CreateObject<string_type>();
        return node;
    }

    /**
     * @brief A factory method for string Node objects with lvalue string_type objects.
     *
     * @param[in] str A lvalue source of string type.
     * @return Node A constructed Node object of string type.
     */
    static Node StringScalar(const string_type& str)
    {
        Node node;
        node.m_node_type = NodeType::STRING;
        node.m_node_value.str = CreateObject<string_type>(str);
        return node;
    }

    /**
     * @brief A factory method for string Node objects with rvalue string_type objects.
     *
     * @param[in] str A rvalue source of string type.
     * @return Node A constructed Node object of string type.
     */
    static Node StringScalar(string_type&& str)
    {
        Node node;
        node.m_node_type = NodeType::STRING;
        node.m_node_value.str = CreateObject<string_type>(std::move(str));
        return node;
    }

public:
    /**
     * @brief A copy assignment operator of the Node class.
     *
     * @param[in] rhs A lvalue Node object to be copied with.
     * @return Node& Reference to this Node object.
     */
    Node& operator=(const Node& rhs) noexcept
    {
        Node(rhs).Swap(*this);
        return *this;
    }

    /**
     * @brief A move assignment operator of the Node class.
     *
     * @param[in] rhs A rvalue Node object to be moved from.
     * @return Node& Reference to this Node object.
     */
    Node& operator=(Node&& rhs) noexcept
    {
        Node(std::move(rhs)).Swap(*this);
        return *this;
    }

    /**
     * @brief A subscript operator for non-const Node objects.
     *
     * @param[in] index An index of sequence Node values.
     * @return Node& Reference to a Node object located at the specified index.
     */
    Node& operator[](std::size_t index) // NOLINT(readability-make-member-function-const)
    {
        if (!IsSequence())
        {
            throw Exception("The target node is not of a sequence type.");
        }

        return m_node_value.sequence->operator[](index);
    }

    /**
     * @brief A subscript operator for const Node objects.
     *
     * @param[in] index An index of sequence Node values.
     * @return const Node& Constant reference to a Node object located at the specified index.
     */
    const Node& operator[](std::size_t index) const
    {
        if (!IsSequence())
        {
            throw Exception("The target node is not of a sequence type.");
        }

        return m_node_value.sequence->operator[](index);
    }

    /**
     * @brief A subscript operator for non-const Node objects.
     *
     * @param[in] key A lvalue key of mapping Node values.
     * @return Node& Constant reference to a Node object associated with the specified key.
     */
    Node& operator[](const std::string& key) // NOLINT(readability-make-member-function-const)
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        return m_node_value.mapping->operator[](key);
    }

    /**
     * @brief A subscript operator for non-const Node objects.
     *
     * @param[in] key A rvalue key of mapping Node values.
     * @return Node& Reference to a Node object associated with the specified key.
     */
    Node& operator[](std::string&& key) // NOLINT(readability-make-member-function-const)
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        return m_node_value.mapping->operator[](std::move(key));
    }

    /**
     * @brief A subscript operator for const Node objects.
     *
     * @param[in] key A lvalue key of mapping Node values.
     * @return const Node& Constant reference to a Node object associated with the specified key.
     */
    const Node& operator[](const std::string& key) const
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        return m_node_value.mapping->operator[](key);
    }

    /**
     * @brief A subscript operator for const Node objects.
     *
     * @param[in] key A rvalue key of mapping Node values.
     * @return const Node& Constant reference to a Node object associated with the specified key.
     */
    const Node& operator[](std::string&& key) const
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        return m_node_value.mapping->operator[](std::move(key));
    }

public:
    /**
     * @brief Returns the type of the current Node value.
     *
     * @return NodeType The type of the current Node value.
     * @retval NodeType::SEQUENCE         sequence type.
     * @retval NodeType::MAPPINT          mapping type.
     * @retval NodeType::NULL_OBJECT      null type.
     * @retval NodeType::BOOLEAN          boolean type.
     * @retval NodeType::SIGNED_INTEGER   signed integer type.
     * @retval NodeType::UNSIGNED_INTEGER unsigned integer type.
     * @retval NodeType::FLOAT_NUMBER     float number type.
     * @retval NodeType::STRING           string type.
     */
    NodeType Type() const noexcept
    {
        return m_node_type;
    }

    /**
     * @brief Tests whether the current Node value is of sequence type.
     *
     * @return bool A result of testing whetehre the current Node value is of sequence type.
     * @retval true  The current Node value is of sequence type.
     * @return false The current Node value is not of sequence type.
     */
    bool IsSequence() const noexcept
    {
        return m_node_type == NodeType::SEQUENCE;
    }

    /**
     * @brief Tests whether the current Node value is of mapping type.
     *
     * @return bool A result of testing whetehre the current Node value is of mapping type.
     * @retval true  The current Node value is of mapping type.
     * @return false The current Node value is not of mapping type.
     */
    bool IsMapping() const noexcept
    {
        return m_node_type == NodeType::MAPPING;
    }

    /**
     * @brief Tests whether the current Node value is of null type.
     *
     * @return bool A result of testing whetehre the current Node value is of null type.
     * @retval true  The current Node value is of null type.
     * @return false The current Node value is not of null type.
     */
    bool IsNull() const noexcept
    {
        return m_node_type == NodeType::NULL_OBJECT;
    }

    /**
     * @brief Tests whether the current Node value is of boolean type.
     *
     * @return bool A result of testing whetehre the current Node value is of boolean type.
     * @retval true  The current Node value is of boolean type.
     * @return false The current Node value is not of boolean type.
     */
    bool IsBoolean() const noexcept
    {
        return m_node_type == NodeType::BOOLEAN;
    }

    /**
     * @brief Tests whether the current Node value is of signed integer type.
     *
     * @return bool A result of testing whetehre the current Node value is of signed integer type.
     * @retval true  The current Node value is of signed integer type.
     * @return false The current Node value is not of signed integer type.
     */
    bool IsSignedInteger() const noexcept
    {
        return m_node_type == NodeType::SIGNED_INTEGER;
    }

    /**
     * @brief Tests whether the current Node value is of unsigned integer type.
     *
     * @return bool A result of testing whetehre the current Node value is of unsigned integer type.
     * @retval true  The current Node value is of unsigned integer type.
     * @return false The current Node value is not of unsigned integer type.
     */
    bool IsUnsignedInteger() const noexcept
    {
        return m_node_type == NodeType::UNSIGNED_INTEGER;
    }

    /**
     * @brief Tests whether the current Node value is of float number type.
     *
     * @return bool A result of testing whetehre the current Node value is of float number type.
     * @retval true  The current Node value is of float number type.
     * @return false The current Node value is not of float number type.
     */
    bool IsFloatNumber() const noexcept
    {
        return m_node_type == NodeType::FLOAT_NUMBER;
    }

    /**
     * @brief Tests whether the current Node value is of string type.
     *
     * @return bool A result of testing whetehre the current Node value is of string type.
     * @retval true  The current Node value is of string type.
     * @return false The current Node value is not of string type.
     */
    bool IsString() const noexcept
    {
        return m_node_type == NodeType::STRING;
    }

    /**
     * @brief Tests whether the current Node value is of scalar types.
     *
     * @return bool A result of testing whetehre the current Node value is of scalar types.
     * @retval true  The current Node value is of scalar types.
     * @return false The current Node value is not of scalar types.
     */
    bool IsScalar() const noexcept
    {
        return !IsSequence() && !IsMapping();
    }

    /**
     * @brief Tests whether the current Node value (sequence, mapping, string) is empty.
     *
     * @return bool A result of testing whetehre the current Node value is empty.
     * @retval true  The current Node value is empty.
     * @return false The current Node value is not empty.
     */
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

    /**
     * @brief Returns the size of the current Node value (sequence, mapping, string).
     *
     * @return std::size_t The size of the current Node value.
     */
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

    /**
     * @brief Returns reference to sequence Node value from a non-const Node object. Throws exception if the Node value
     * is not of sequence type.
     *
     * @return sequence_type& Reference to sequence Node value.
     */
    sequence_type& ToSequence() // NOLINT(readability-make-member-function-const)
    {
        if (!IsSequence())
        {
            throw Exception("The target node is not of a sequence type.");
        }

        return *(m_node_value.sequence);
    }

    /**
     * @brief Returns reference to sequence Node value from a const Node object.  Throws exception if the Node value is
     * not of sequence type.
     *
     * @return const sequence_type& Constant reference to sequence Node value.
     */
    const sequence_type& ToSequence() const
    {
        if (!IsSequence())
        {
            throw Exception("The target node is not of a sequence type.");
        }

        return *(m_node_value.sequence);
    }

    /**
     * @brief Returns reference to mapping Node value from a non-const Node object. Throws exception if the Node value
     * is not of mapping type.
     *
     * @return mapping_type& Reference to mapping Node value.
     */
    mapping_type& ToMapping() // NOLINT(readability-make-member-function-const)
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        return *(m_node_value.mapping);
    }

    /**
     * @brief Returns reference to mapping Node value from a const Node object.  Throws exception if the Node value is
     * not of mapping type.
     *
     * @return const mapping_type& Constant reference to mapping Node value.
     */
    const mapping_type& ToMapping() const
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        return *(m_node_value.mapping);
    }

    /**
     * @brief Returns reference to boolean Node value from a non-const Node object. Throws exception if the Node value
     * is not of boolean type.
     *
     * @return boolean_type& Reference to boolean Node value.
     */
    boolean_type& ToBoolean()
    {
        if (!IsBoolean())
        {
            throw Exception("The target node is not of a boolean type.");
        }

        return m_node_value.boolean;
    }

    /**
     * @brief Returns reference to boolean Node value from a const Node object.  Throws exception if the Node value is
     * not of boolean type.
     *
     * @return const boolean_type& Constant reference to boolean Node value.
     */
    const boolean_type& ToBoolean() const
    {
        if (!IsBoolean())
        {
            throw Exception("The target node is not of a boolean type.");
        }

        return m_node_value.boolean;
    }

    /**
     * @brief Returns reference to signed integer Node value from a non-const Node object. Throws exception if the Node
     * value is not of signed integer type.
     *
     * @return signed_int_type& Reference to signed integer Node value.
     */
    signed_int_type& ToSignedInteger()
    {
        if (!IsSignedInteger())
        {
            throw Exception("The target node is not of a signed integer type.");
        }

        return m_node_value.signed_int;
    }

    /**
     * @brief Returns reference to signed integer Node value from a const Node object. Throws exception if the Node
     * value is not of signed integer type.
     *
     * @return const signed_int_type& Constant reference to signed integer Node value.
     */
    const signed_int_type& ToSignedInteger() const
    {
        if (!IsSignedInteger())
        {
            throw Exception("The target node is not of a signed integer type.");
        }

        return m_node_value.signed_int;
    }

    /**
     * @brief Returns reference to unsigned integer Node value from a non-const Node object. Throws exception if the
     * Node value is not of unsigned integer type.
     *
     * @return unsigned_int_type& Reference to unsigned integer Node value.
     */
    unsigned_int_type& ToUnsignedInteger()
    {
        if (!IsUnsignedInteger())
        {
            throw Exception("The target node is not of an unsigned integer type.");
        }

        return m_node_value.unsigned_int;
    }

    /**
     * @brief Returns reference to unsigned integer Node value from a const Node object. Throws exception if the Node
     * value is not of unsigned integer type.
     *
     * @return const unsigned_int_type& Constant reference to unsigned integer Node value.
     */
    const unsigned_int_type& ToUnsignedInteger() const
    {
        if (!IsUnsignedInteger())
        {
            throw Exception("The target node is not of an unsigned integer type.");
        }

        return m_node_value.unsigned_int;
    }

    /**
     * @brief Returns reference to float number Node value from a non-const Node object. Throws exception if the Node
     * value is not of float number type.
     *
     * @return float_number_type& Reference to float number Node value.
     */
    float_number_type& ToFloatNumber()
    {
        if (!IsFloatNumber())
        {
            throw Exception("The target node is not of a float number type.");
        }

        return m_node_value.float_val;
    }

    /**
     * @brief Returns reference to float number Node value from a const Node object. Throws exception if the Node value
     * is not of float number type.
     *
     * @return const float_number_type& Constant reference to float number Node value.
     */
    const float_number_type& ToFloatNumber() const
    {
        if (!IsFloatNumber())
        {
            throw Exception("The target node is not of a float number type.");
        }

        return m_node_value.float_val;
    }

    /**
     * @brief Returns reference to string Node value from a non-const Node object. Throws exception if the Node value is
     * not of string type.
     *
     * @return string_type& Reference to string Node value.
     */
    string_type& ToString() // NOLINT(readability-make-member-function-const)
    {
        if (!IsString())
        {
            throw Exception("The target node is not of a string type.");
        }

        return *(m_node_value.str);
    }

    /**
     * @brief Returns reference to string Node value from a const Node object. Throws exception if the Node value is not
     * of string type.
     *
     * @return const string_type& Constant reference to string Node value.
     */
    const string_type& ToString() const
    {
        if (!IsString())
        {
            throw Exception("The target node is not of a string type.");
        }

        return *(m_node_value.str);
    }

    /**
     * @brief Swaps data with the specified Node object.
     *
     * @param[in] rhs A Node object to be swapped with.
     */
    void Swap(Node& rhs) noexcept
    {
        using std::swap;
        swap(m_node_type, rhs.m_node_type);

        NodeValue tmp {};
        std::memcpy(&tmp, &m_node_value, sizeof(NodeValue));
        std::memcpy(&m_node_value, &rhs.m_node_value, sizeof(NodeValue));
        std::memcpy(&rhs.m_node_value, &tmp, sizeof(NodeValue));
    }

    /**
     * @brief Returns the first iterator of Node values of container types (sequence or mapping) from a non-const Node
     * object. Throws exception if the Node value is not of container types.
     *
     * @return iterator The first iterator of Node values of container types.
     */
    iterator Begin()
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            return {fkyaml::SequenceIteratorTag(), m_node_value.sequence->begin()};
        case NodeType::MAPPING:
            return {fkyaml::MappingIteratorTag(), m_node_value.mapping->begin()};
        default:
            throw Exception("The target node is neither of sequence nor mapping types.");
        }
    }

    /**
     * @brief Returns the first iterator of Node values of container types (sequence or mapping) from a non-const Node
     * object. Throws exception if the Node value is not of container types.
     * @note This is a work-around for range-based for statements, and just a wrapper method of Begin() for non-const
     * Node objects.
     *
     * @return iterator The first iterator of Node values of container types.
     */
    iterator begin() // NOLINT(readability-identifier-naming)
    {
        return Begin();
    }

    /**
     * @brief Returns the first iterator of Node values of container types (sequence or mapping) from a const Node
     * object. Throws exception if the Node value is not of container types.
     *
     * @return const_iterator The first iterator of Node values of container types.
     */
    const_iterator Begin() const
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            return {fkyaml::SequenceIteratorTag(), m_node_value.sequence->begin()};
        case NodeType::MAPPING:
            return {fkyaml::MappingIteratorTag(), m_node_value.mapping->begin()};
        default:
            throw Exception("The target node is neither of sequence nor mapping types.");
        }
    }

    /**
     * @brief Returns the first iterator of Node values of container types (sequence or mapping) from a const Node
     * object. Throws exception if the Node value is not of container types.
     * @note This is a work-around for range-based for statements, and just a wrapper method of Begin() for const Node
     * objects.
     *
     * @return const_iterator The first iterator of Node values of container types.
     */
    const_iterator begin() const // NOLINT(readability-identifier-naming)
    {
        return Begin();
    }

    /**
     * @brief Returns the last iterator of Node values of container types (sequence or mapping) from a non-const Node
     * object. Throws exception if the Node value is not of container types.
     *
     * @return iterator The last iterator of Node values of container types.
     */
    iterator End()
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            return {fkyaml::SequenceIteratorTag(), m_node_value.sequence->end()};
        case NodeType::MAPPING:
            return {fkyaml::MappingIteratorTag(), m_node_value.mapping->end()};
        default:
            throw Exception("The target node is neither of sequence nor mapping types.");
        }
    }

    /**
     * @brief Returns the last iterator of Node values of container types (sequence or mapping) from a non-const Node
     * object. Throws exception if the Node value is not of container types.
     * @note This is a work-around for range-based for statements, and just a wrapper method of End() for const Node
     * objects.
     *
     * @return iterator The last iterator of Node values of container types.
     */
    iterator end() // NOLINT(readability-identifier-naming)
    {
        return End();
    }

    /**
     * @brief Returns the last iterator of Node values of container types (sequence or mapping) from a const Node
     * object. Throws exception if the Node value is not of container types.
     *
     * @return const_iterator The last iterator of Node values of container types.
     */
    const_iterator End() const
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            return {fkyaml::SequenceIteratorTag(), m_node_value.sequence->end()};
        case NodeType::MAPPING:
            return {fkyaml::MappingIteratorTag(), m_node_value.mapping->end()};
        default:
            throw Exception("The target node is neither of sequence nor mapping types.");
        }
    }

    /**
     * @brief Returns the last iterator of Node values of container types (sequence or mapping) from a const Node
     * object. Throws exception if the Node value is not of container types.
     * @note This is a work-around for range-based for statements, and just a wrapper method of End() for const Node
     * objects.
     *
     * @return const_iterator The last iterator of Node values of container types.
     */
    const_iterator end() const // NOLINT(readability-identifier-naming)
    {
        return end();
    }

private:
    /** The current node value type. */
    NodeType m_node_type;
    /** The current node value. */
    NodeValue m_node_value {};
};

/** A type for sequence Node values. */
using NodeSequenceType = typename Node::sequence_type;

/** A type for mapping Node values. */
using NodeMappingType = typename Node::mapping_type;

/** A type for boolean Node values. */
using NodeBooleanType = typename Node::boolean_type;

/** A type for signed integer Node values. */
using NodeSignedIntType = typename Node::signed_int_type;

/** A type for unsigned integer Node values. */
using NodeUnsignedIntType = typename Node::unsigned_int_type;

/** A type for float number Node values. */
using NodeFloatNumberType = typename Node::float_number_type;

/** A type for string Node values. */
using NodeStringType = typename Node::string_type;

} // namespace fkyaml

namespace std
{

/**
 * @brief A specialization of std::swap for Node class.
 *
 * @tparam N/A
 * @param lhs A Node object of left-hand-side.
 * @param rhs A Node object of right-hand-side.
 */
template <>
inline void swap<fkyaml::Node>(fkyaml::Node& lhs, fkyaml::Node& rhs) noexcept(noexcept(lhs.Swap(rhs)))
{
    lhs.Swap(rhs);
}

} // namespace std

#endif /* FK_YAML_NODE_HPP_ */