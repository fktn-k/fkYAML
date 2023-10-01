/**
 *   __ _  __     __      __  __ _
 *  / _| | \ \   / //\   |  \/  | |
 * | |_| | _\ \_/ //  \  | \  / | |
 * |  _| |/ /\   // /\ \ | |\/| | |
 * | | |   <  | |/ ____ \| |  | | |____
 * |_| |_|\_\ |_/_/    \_\_|  |_|______|
 *
 * @file Node.hpp
 * @brief Implementation of YAML node data structure.
 * @version 0.0.0
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_NODE_HPP_
#define FK_YAML_NODE_HPP_

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "fkYAML/VersioningMacros.hpp"
#include "fkYAML/Assert.hpp"
#include "fkYAML/Exception.hpp"
#include "fkYAML/Iterator.hpp"
#include "fkYAML/NodeType.hpp"
#include "fkYAML/OrderedMap.hpp"
#include "fkYAML/TypeTraits.hpp"
#include "fkYAML/YAMLVersionType.hpp"

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

/**
 * @class BasicNode
 * @brief A class to store value of YAML nodes.
 *
 * @tparam SequenceType A type for sequence node value containers.
 * @tparam MappingType A type for mapping node value containers.
 * @tparam BooleanType A type for boolean node values.
 * @tparam IntegerType A type for integer node values.
 * @tparam FloatNumberType A type for float number node values.
 * @tparam StringType A type for string node values.
 */
template <
    template <typename, typename...> class SequenceType = std::vector,
    template <typename, typename, typename...> class MappingType = OrderedMap, typename BooleanType = bool,
    typename IntegerType = std::int64_t, typename FloatNumberType = double, typename StringType = std::string>
class BasicNode
{
public:
    /** A type for iterators of BasicNode containers. */
    using iterator = Iterator<BasicNode>;
    /** A type for constant iterators of BasicNode containers. */
    using const_iterator = Iterator<const BasicNode>;

    /** A type for sequence BasicNode values. */
    using sequence_type = SequenceType<BasicNode, std::allocator<BasicNode>>;
    /** A type for mapping BasicNode values. */
    using mapping_type = MappingType<StringType, BasicNode>;
    /** A type for boolean BasicNode values. */
    using boolean_type = BooleanType;
    /** A type for integer BasicNode values. */
    using integer_type = IntegerType;
    /** A type for float number BasicNode values. */
    using float_number_type = FloatNumberType;
    /** A type for string BasicNode values. */
    using string_type = StringType;

private:
    /**
     * @union NodeValue
     * @brief The actual storage for a YAML node value of the @ref BasicNode class.
     * @details This union combines the different sotrage types for the YAML value types defined in @ref NodeType.
     * @note Container types are stored as pointers so that the size of this union should not exceed 64 bits by default.
     */
    union NodeValue
    {
        /**
         * @brief Construct a new BasicNode Value object for null types.
         */
        NodeValue() = default;

        /**
         * @brief Construct a new BasicNode Value object with BasicNode types. The default value for the specified type
         * will be assigned.
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
            case NodeType::INTEGER:
                integer = static_cast<integer_type>(0);
                break;
            case NodeType::FLOAT_NUMBER:
                float_val = static_cast<float_number_type>(0.0);
                break;
            case NodeType::STRING:
                str = CreateObject<string_type>("");
                break;
            default:                                             // LCOV_EXCL_LINE
                throw Exception("Unsupported node value type."); // LCOV_EXCL_LINE
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
                std::vector<BasicNode> stack;

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
                    BasicNode current_node(std::move(stack.back()));
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
        /** A value of integer type. */
        integer_type integer;
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

        FK_YAML_ASSERT(object != nullptr);
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
     * @brief Construct a new BasicNode object of null type.
     */
    BasicNode() noexcept
        : m_node_type(NodeType::NULL_OBJECT),
          m_yaml_version_type(YamlVersionType::VER_1_2),
          m_node_value(),
          m_anchor_name(nullptr)
    {
    }

    /**
     * @brief Construct a new BasicNode object with a specified type.
     * @note If you construct an alias node, call BasicNode::AliasOf() instead.
     *
     * @param[in] type A YAML node value type.
     */
    explicit BasicNode(const NodeType type)
        : m_node_type(type),
          m_yaml_version_type(YamlVersionType::VER_1_2),
          m_node_value(type),
          m_anchor_name(nullptr)
    {
    }

    /**
     * @brief Copy constructor of the BasicNode class.
     *
     * @param[in] rhs A BasicNode object to be copied with.
     */
    BasicNode(const BasicNode& rhs)
        : m_node_type(rhs.m_node_type),
          m_yaml_version_type(rhs.m_yaml_version_type),
          m_anchor_name(nullptr)
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            m_node_value.sequence = CreateObject<sequence_type>(*(rhs.m_node_value.sequence));
            FK_YAML_ASSERT(m_node_value.sequence != nullptr);
            break;
        case NodeType::MAPPING:
            m_node_value.mapping = CreateObject<mapping_type>(*(rhs.m_node_value.mapping));
            FK_YAML_ASSERT(m_node_value.mapping != nullptr);
            break;
        case NodeType::NULL_OBJECT:
            m_node_value.mapping = nullptr;
            break;
        case NodeType::BOOLEAN:
            m_node_value.boolean = rhs.m_node_value.boolean;
            break;
        case NodeType::INTEGER:
            m_node_value.integer = rhs.m_node_value.integer;
            break;
        case NodeType::FLOAT_NUMBER:
            m_node_value.float_val = rhs.m_node_value.float_val;
            break;
        case NodeType::STRING:
            m_node_value.str = CreateObject<string_type>(*(rhs.m_node_value.str));
            FK_YAML_ASSERT(m_node_value.str != nullptr);
            break;
        default:                                               // LCOV_EXCL_LINE
            throw Exception("Not supported node value type."); // LCOV_EXCL_LINE
        }

        if (rhs.m_anchor_name)
        {
            DestroyObject<std::string>(m_anchor_name);
            m_anchor_name = CreateObject<std::string>(*(rhs.m_anchor_name));
            FK_YAML_ASSERT(m_anchor_name != nullptr);
        }
    }

    /**
     * @brief Move constructor of the BasicNode class.
     *
     * @param[in] rhs A BasicNode object to be moved from.
     */
    BasicNode(BasicNode&& rhs) noexcept // NOLINT(bugprone-exception-escape)
        : m_node_type(rhs.m_node_type),
          m_yaml_version_type(rhs.m_yaml_version_type),
          m_anchor_name(rhs.m_anchor_name)
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            FK_YAML_ASSERT(rhs.m_node_value.sequence != nullptr);
            m_node_value.sequence = rhs.m_node_value.sequence;
            rhs.m_node_value.sequence = nullptr;
            break;
        case NodeType::MAPPING:
            FK_YAML_ASSERT(rhs.m_node_value.mapping != nullptr);
            m_node_value.mapping = rhs.m_node_value.mapping;
            rhs.m_node_value.mapping = nullptr;
            break;
        case NodeType::NULL_OBJECT:
            FK_YAML_ASSERT(rhs.m_node_value.mapping == nullptr);
            m_node_value.mapping = rhs.m_node_value.mapping;
            break;
        case NodeType::BOOLEAN:
            m_node_value.boolean = rhs.m_node_value.boolean;
            rhs.m_node_value.boolean = static_cast<boolean_type>(false);
            break;
        case NodeType::INTEGER:
            m_node_value.integer = rhs.m_node_value.integer;
            rhs.m_node_value.integer = static_cast<integer_type>(0);
            break;
        case NodeType::FLOAT_NUMBER:
            m_node_value.float_val = rhs.m_node_value.float_val;
            rhs.m_node_value.float_val = static_cast<float_number_type>(0.0);
            break;
        case NodeType::STRING:
            FK_YAML_ASSERT(rhs.m_node_value.str != nullptr);
            m_node_value.str = rhs.m_node_value.str;
            rhs.m_node_value.str = nullptr;
            break;
        default:                                             // LCOV_EXCL_LINE
            throw Exception("Unsupported node value type."); // LCOV_EXCL_LINE
        }

        rhs.m_node_type = NodeType::NULL_OBJECT;
        rhs.m_yaml_version_type = YamlVersionType::VER_1_2;
        rhs.m_node_value.mapping = nullptr;
        rhs.m_anchor_name = nullptr;
    }

    /**
     * @brief Destroy the BasicNode object and its value storage.
     */
    ~BasicNode() noexcept // NOLINT(bugprone-exception-escape)
    {
        DestroyObject<std::string>(m_anchor_name);
        m_anchor_name = nullptr;
        m_node_value.Destroy(m_node_type);
        m_node_type = NodeType::NULL_OBJECT;
    }

public:
    /**
     * @brief A factory method for sequence BasicNode objects without sequence_type objects.
     *
     * @return BasicNode A constructed BasicNode object of sequence type.
     */
    static BasicNode Sequence()
    {
        BasicNode node;
        node.m_node_type = NodeType::SEQUENCE;
        node.m_node_value.sequence = CreateObject<sequence_type>();
        FK_YAML_ASSERT(node.m_node_value.sequence != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for sequence BasicNode objects with lvalue sequence_type objects.
     *
     * @param[in] sequence A lvalue source of sequence type.
     * @return BasicNode A constructed BasicNode object of sequence type.
     */
    static BasicNode Sequence(const sequence_type& sequence)
    {
        BasicNode node;
        node.m_node_type = NodeType::SEQUENCE;
        node.m_node_value.sequence = CreateObject<sequence_type>(sequence);
        FK_YAML_ASSERT(node.m_node_value.sequence != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for sequence BasicNode objects with rvalue sequence_type objects.
     *
     * @param[in] sequence A rvalue source of sequence type.
     * @return BasicNode A constructed BasicNode object of sequence type.
     */
    static BasicNode Sequence(sequence_type&& sequence)
    {
        BasicNode node;
        node.m_node_type = NodeType::SEQUENCE;
        node.m_node_value.sequence = CreateObject<sequence_type>(std::move(sequence));
        FK_YAML_ASSERT(node.m_node_value.sequence != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for mapping BasicNode objects without mapping_type objects.
     *
     * @return BasicNode A constructed BasicNode object of mapping type.
     */
    static BasicNode Mapping()
    {
        BasicNode node;
        node.m_node_type = NodeType::MAPPING;
        node.m_node_value.mapping = CreateObject<mapping_type>();
        FK_YAML_ASSERT(node.m_node_value.mapping != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for mapping BasicNode objects with lvalue mapping_type objects.
     *
     * @param[in] mapping A lvalue source of mapping type.
     * @return BasicNode A constructed BasicNode object of mapping type.
     */
    static BasicNode Mapping(const mapping_type& mapping)
    {
        BasicNode node;
        node.m_node_type = NodeType::MAPPING;
        node.m_node_value.mapping = CreateObject<mapping_type>(mapping);
        FK_YAML_ASSERT(node.m_node_value.mapping != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for mapping BasicNode objects with rvalue mapping_type objects.
     *
     * @param[in] mapping A rvalue source of mapping type.
     * @return BasicNode A constructed BasicNode object of mapping type.
     */
    static BasicNode Mapping(mapping_type&& mapping)
    {
        BasicNode node;
        node.m_node_type = NodeType::MAPPING;
        node.m_node_value.mapping = CreateObject<mapping_type>(std::move(mapping));
        FK_YAML_ASSERT(node.m_node_value.mapping != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for boolean scalar BasicNode objects.
     *
     * @param[in] boolean A source of boolean type.
     * @return BasicNode A constructed BasicNode object of boolean type.
     */
    static BasicNode BooleanScalar(const boolean_type boolean) noexcept
    {
        BasicNode node;
        node.m_node_type = NodeType::BOOLEAN;
        node.m_node_value.boolean = boolean;
        return node;
    }

    /**
     * @brief A factory method for integer scalar BasicNode objects.
     *
     * @param[in] integer A source of integer type.
     * @return BasicNode A constructed BasicNode object of integer type.
     */
    static BasicNode IntegerScalar(const integer_type integer) noexcept
    {
        BasicNode node;
        node.m_node_type = NodeType::INTEGER;
        node.m_node_value.integer = integer;
        return node;
    }

    /**
     * @brief A factory method for float number scalar BasicNode objects.
     *
     * @param[in] float_val A source of unsigned integer type.
     * @return BasicNode A constructed BasicNode object of float number type.
     */
    static BasicNode FloatNumberScalar(const float_number_type float_val) noexcept
    {
        BasicNode node;
        node.m_node_type = NodeType::FLOAT_NUMBER;
        node.m_node_value.float_val = float_val;
        return node;
    }

    /**
     * @brief A factory method for string BasicNode objects without string_type objects.
     *
     * @return BasicNode A constructed BasicNode object of string type.
     */
    static BasicNode StringScalar()
    {
        BasicNode node;
        node.m_node_type = NodeType::STRING;
        node.m_node_value.str = CreateObject<string_type>();
        FK_YAML_ASSERT(node.m_node_value.str != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for string BasicNode objects with lvalue string_type objects.
     *
     * @param[in] str A lvalue source of string type.
     * @return BasicNode A constructed BasicNode object of string type.
     */
    static BasicNode StringScalar(const string_type& str)
    {
        BasicNode node;
        node.m_node_type = NodeType::STRING;
        node.m_node_value.str = CreateObject<string_type>(str);
        FK_YAML_ASSERT(node.m_node_value.str != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for string BasicNode objects with rvalue string_type objects.
     *
     * @param[in] str A rvalue source of string type.
     * @return BasicNode A constructed BasicNode object of string type.
     */
    static BasicNode StringScalar(string_type&& str)
    {
        BasicNode node;
        node.m_node_type = NodeType::STRING;
        node.m_node_value.str = CreateObject<string_type>(std::move(str));
        FK_YAML_ASSERT(node.m_node_value.str != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for alias BasicNode objects referencing the given anchor BasicNode object.
     * @note The given anchor BasicNode must have a non-empty anchor name.
     *
     * @param anchor_node An anchor node to be referenced by the newly constructed BasicNode object.
     * @return BasicNode A constructed BasicNode object of alias type.
     */
    static BasicNode AliasOf(const BasicNode& anchor_node)
    {
        if (!anchor_node.m_anchor_name || anchor_node.m_anchor_name->empty())
        {
            throw Exception("Cannot create an alias without anchor name.");
        }

        BasicNode node = anchor_node;
        return node;
    }

public:
    /**
     * @brief A copy assignment operator of the BasicNode class.
     *
     * @param[in] rhs A lvalue BasicNode object to be copied with.
     * @return BasicNode& Reference to this BasicNode object.
     */
    BasicNode& operator=(const BasicNode& rhs) noexcept
    {
        BasicNode(rhs).Swap(*this);
        return *this;
    }

    /**
     * @brief A move assignment operator of the BasicNode class.
     *
     * @param[in] rhs A rvalue BasicNode object to be moved from.
     * @return BasicNode& Reference to this BasicNode object.
     */
    BasicNode& operator=(BasicNode&& rhs) noexcept
    {
        BasicNode(std::move(rhs)).Swap(*this);
        return *this;
    }

    /**
     * @brief A subscript operator for non-const BasicNode objects.
     *
     * @param[in] index An index of sequence BasicNode values.
     * @return BasicNode& Reference to a BasicNode object located at the specified index.
     */
    BasicNode& operator[](std::size_t index) // NOLINT(readability-make-member-function-const)
    {
        if (!IsSequence())
        {
            throw Exception("The target node is not of a sequence type.");
        }

        FK_YAML_ASSERT(m_node_value.sequence != nullptr);
        FK_YAML_ASSERT(index < m_node_value.sequence->size());
        return m_node_value.sequence->operator[](index);
    }

    /**
     * @brief A subscript operator for const BasicNode objects.
     *
     * @param[in] index An index of sequence BasicNode values.
     * @return const BasicNode& Constant reference to a BasicNode object located at the specified index.
     */
    const BasicNode& operator[](std::size_t index) const
    {
        if (!IsSequence())
        {
            throw Exception("The target node is not of a sequence type.");
        }

        FK_YAML_ASSERT(m_node_value.sequence != nullptr);
        FK_YAML_ASSERT(index < m_node_value.sequence->size());
        return m_node_value.sequence->operator[](index);
    }

    /**
     * @brief A subscript operator for non-const BasicNode objects.
     *
     * @tparam KeyType A type for the input key.
     * @param[in] key A key to the target BasicNode object..
     * @return BasicNode& Reference to a BasicNode object associated with the given key.
     */
    template <
        typename KeyType,
        fkyaml::enable_if_t<
            IsUsableAsKeyType<typename mapping_type::key_compare, typename mapping_type::key_type, KeyType>::value,
            int> = 0>
    BasicNode& operator[](KeyType&& key) // NOLINT(readability-make-member-function-const)
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        FK_YAML_ASSERT(m_node_value.mapping != nullptr);
        return m_node_value.mapping->operator[](std::forward<KeyType>(key));
    }

    /**
     * @brief A subscript operator for const BasicNode objects.
     *
     * @tparam KeyType A type for the input key.
     * @param[in] key A key to the BasicNode object.
     * @return const BasicNode& Constant reference to a BasicNode object associated with the given key.
     */
    template <
        typename KeyType,
        fkyaml::enable_if_t<
            IsUsableAsKeyType<typename mapping_type::key_compare, typename mapping_type::key_type, KeyType>::value,
            int> = 0>
    const BasicNode& operator[](KeyType&& key) const
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        FK_YAML_ASSERT(m_node_value.mapping != nullptr);
        return m_node_value.mapping->operator[](std::forward<KeyType>(key));
    }

public:
    /**
     * @brief Returns the type of the current BasicNode value.
     *
     * @return NodeType The type of the current BasicNode value.
     * @retval NodeType::SEQUENCE         sequence type.
     * @retval NodeType::MAPPINT          mapping type.
     * @retval NodeType::NULL_OBJECT      null type.
     * @retval NodeType::BOOLEAN          boolean type.
     * @retval NodeType::INTEGER          integer type.
     * @retval NodeType::FLOAT_NUMBER     float number type.
     * @retval NodeType::STRING           string type.
     */
    NodeType Type() const noexcept
    {
        return m_node_type;
    }

    /**
     * @brief Tests whether the current BasicNode value is of sequence type.
     *
     * @return bool A result of testing whetehre the current BasicNode value is of sequence type.
     * @retval true  The current BasicNode value is of sequence type.
     * @return false The current BasicNode value is not of sequence type.
     */
    bool IsSequence() const noexcept
    {
        return m_node_type == NodeType::SEQUENCE;
    }

    /**
     * @brief Tests whether the current BasicNode value is of mapping type.
     *
     * @return bool A result of testing whetehre the current BasicNode value is of mapping type.
     * @retval true  The current BasicNode value is of mapping type.
     * @return false The current BasicNode value is not of mapping type.
     */
    bool IsMapping() const noexcept
    {
        return m_node_type == NodeType::MAPPING;
    }

    /**
     * @brief Tests whether the current BasicNode value is of null type.
     *
     * @return bool A result of testing whetehre the current BasicNode value is of null type.
     * @retval true  The current BasicNode value is of null type.
     * @return false The current BasicNode value is not of null type.
     */
    bool IsNull() const noexcept
    {
        return m_node_type == NodeType::NULL_OBJECT;
    }

    /**
     * @brief Tests whether the current BasicNode value is of boolean type.
     *
     * @return bool A result of testing whetehre the current BasicNode value is of boolean type.
     * @retval true  The current BasicNode value is of boolean type.
     * @return false The current BasicNode value is not of boolean type.
     */
    bool IsBoolean() const noexcept
    {
        return m_node_type == NodeType::BOOLEAN;
    }

    /**
     * @brief Tests whether the current BasicNode value is of integer type.
     *
     * @return bool A result of testing whetehre the current BasicNode value is of integer type.
     * @retval true  The current BasicNode value is of integer type.
     * @return false The current BasicNode value is not of integer type.
     */
    bool IsInteger() const noexcept
    {
        return m_node_type == NodeType::INTEGER;
    }

    /**
     * @brief Tests whether the current BasicNode value is of float number type.
     *
     * @return bool A result of testing whetehre the current BasicNode value is of float number type.
     * @retval true  The current BasicNode value is of float number type.
     * @return false The current BasicNode value is not of float number type.
     */
    bool IsFloatNumber() const noexcept
    {
        return m_node_type == NodeType::FLOAT_NUMBER;
    }

    /**
     * @brief Tests whether the current BasicNode value is of string type.
     *
     * @return bool A result of testing whetehre the current BasicNode value is of string type.
     * @retval true  The current BasicNode value is of string type.
     * @return false The current BasicNode value is not of string type.
     */
    bool IsString() const noexcept
    {
        return m_node_type == NodeType::STRING;
    }

    /**
     * @brief Tests whether the current BasicNode value is of scalar types.
     *
     * @return bool A result of testing whetehre the current BasicNode value is of scalar types.
     * @retval true  The current BasicNode value is of scalar types.
     * @return false The current BasicNode value is not of scalar types.
     */
    bool IsScalar() const noexcept
    {
        return !IsSequence() && !IsMapping();
    }

    /**
     * @brief Tests whether the current BasicNode value (sequence, mapping, string) is empty.
     *
     * @return bool A result of testing whetehre the current BasicNode value is empty.
     * @retval true  The current BasicNode value is empty.
     * @return false The current BasicNode value is not empty.
     */
    bool IsEmpty() const
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.sequence != nullptr);
            return m_node_value.sequence->empty();
        case NodeType::MAPPING:
            FK_YAML_ASSERT(m_node_value.mapping != nullptr);
            return m_node_value.mapping->empty();
        case NodeType::STRING:
            FK_YAML_ASSERT(m_node_value.str != nullptr);
            return m_node_value.str->empty();
        default:
            throw Exception("The target node is not of a container type.");
        }
    }

    /**
     * @brief Returns the size of the current BasicNode value (sequence, mapping, string).
     *
     * @return std::size_t The size of the current BasicNode value.
     */
    std::size_t Size() const
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.sequence != nullptr);
            return m_node_value.sequence->size();
        case NodeType::MAPPING:
            FK_YAML_ASSERT(m_node_value.mapping != nullptr);
            return m_node_value.mapping->size();
        case NodeType::STRING:
            FK_YAML_ASSERT(m_node_value.str != nullptr);
            return m_node_value.str->size();
        default:
            throw Exception("The target node is not of a container type.");
        }
    }

    /**
     * @brief Check whether or not this BasicNode object has a given key in its inner mapping Node value.
     *
     * @tparam KeyType A type for the input key.
     * @param[in] key A key to the target BasicNode object.
     * @return true If this BasicNode object has a given key.
     * @return false If this BasicNode object does not have a given key.
     */
    template <
        typename KeyType,
        fkyaml::enable_if_t<
            IsUsableAsKeyType<typename mapping_type::key_compare, typename mapping_type::key_type, KeyType>::value,
            int> = 0>
    bool Contains(KeyType&& key) const
    {
        switch (m_node_type)
        {
        case NodeType::MAPPING: {
            FK_YAML_ASSERT(m_node_value.mapping != nullptr);
            mapping_type& map = *m_node_value.mapping;
            return map.find(std::forward<KeyType>(key)) != map.end();
        }
        default:
            return false;
        }
    }

    /**
     * @brief Get the YAML version specification for this BasicNode object.
     *
     * @return YamlVersionType The YAML version specification.
     */
    YamlVersionType GetVersion() const noexcept
    {
        return m_yaml_version_type;
    }

    /**
     * @brief Set the YAML version specification for this BasicNode object.
     *
     * @param version The YAML version specification.
     */
    void SetVersion(const YamlVersionType version) noexcept
    {
        m_yaml_version_type = version;
    }

    /**
     * @brief Check whether or not this BasicNode object has already had any anchor name.
     *
     * @return true If this BasicNode object has already had any anchor name.
     * @return false If this BasicNode object has not had any anchor name yet.
     */
    bool HasAnchorName() const noexcept
    {
        return m_anchor_name != nullptr;
    }

    /**
     * @brief Get the anchor name associated to this BasicNode object.
     * @note Some anchor name must be set before calling this method. Call BasicNode::HasAnchorName() to see if this
     * BasicNode object has any anchor name.
     *
     * @return const std::string& Const reference to the anchor name.
     */
    const std::string& GetAnchorName() const
    {
        if (!m_anchor_name)
        {
            throw Exception("No anchor name has been set.");
        }
        return *m_anchor_name;
    }

    /**
     * @brief Add an anchor name to this BasicNode object.
     * @note If this BasicNode object has already had any anchor name, the new anchor name will overwrite the old one.
     *
     * @param anchor_name An anchor name associated to this BasicNode object.
     */
    void AddAnchorName(const std::string& anchor_name)
    {
        DestroyObject<std::string>(m_anchor_name);
        m_anchor_name = CreateObject<std::string>(anchor_name);
        FK_YAML_ASSERT(m_anchor_name != nullptr);
    }

    /**
     * @brief Add an anchor name to this BasicNode object.
     * @note If this BasicNode object has already had any anchor name, the new anchor name will overwrite the old one.
     *
     * @param anchor_name An anchor name associated to this BasicNode object.
     */
    void AddAnchorName(std::string&& anchor_name)
    {
        DestroyObject<std::string>(m_anchor_name);
        m_anchor_name = CreateObject<std::string>(std::move(anchor_name));
        FK_YAML_ASSERT(m_anchor_name != nullptr);
    }

    /**
     * @brief Returns reference to sequence BasicNode value from a non-const BasicNode object. Throws exception if the
     * BasicNode value is not of sequence type.
     *
     * @return sequence_type& Reference to sequence BasicNode value.
     */
    sequence_type& ToSequence() // NOLINT(readability-make-member-function-const)
    {
        if (!IsSequence())
        {
            throw Exception("The target node is not of a sequence type.");
        }

        FK_YAML_ASSERT(m_node_value.sequence != nullptr);
        return *(m_node_value.sequence);
    }

    /**
     * @brief Returns reference to sequence BasicNode value from a const BasicNode object.  Throws exception if the
     * BasicNode value is not of sequence type.
     *
     * @return const sequence_type& Constant reference to sequence BasicNode value.
     */
    const sequence_type& ToSequence() const
    {
        if (!IsSequence())
        {
            throw Exception("The target node is not of a sequence type.");
        }

        FK_YAML_ASSERT(m_node_value.sequence != nullptr);
        return *(m_node_value.sequence);
    }

    /**
     * @brief Returns reference to mapping BasicNode value from a non-const BasicNode object. Throws exception if the
     * BasicNode value is not of mapping type.
     *
     * @return mapping_type& Reference to mapping BasicNode value.
     */
    mapping_type& ToMapping() // NOLINT(readability-make-member-function-const)
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        FK_YAML_ASSERT(m_node_value.mapping != nullptr);
        return *(m_node_value.mapping);
    }

    /**
     * @brief Returns reference to mapping BasicNode value from a const BasicNode object.  Throws exception if the
     * BasicNode value is not of mapping type.
     *
     * @return const mapping_type& Constant reference to mapping BasicNode value.
     */
    const mapping_type& ToMapping() const
    {
        if (!IsMapping())
        {
            throw Exception("The target node is not of a mapping type.");
        }

        FK_YAML_ASSERT(m_node_value.mapping != nullptr);
        return *(m_node_value.mapping);
    }

    /**
     * @brief Returns reference to boolean BasicNode value from a non-const BasicNode object. Throws exception if the
     * BasicNode value is not of boolean type.
     *
     * @return boolean_type& Reference to boolean BasicNode value.
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
     * @brief Returns reference to boolean BasicNode value from a const BasicNode object.  Throws exception if the
     * BasicNode value is not of boolean type.
     *
     * @return const boolean_type& Constant reference to boolean BasicNode value.
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
     * @brief Returns reference to  integer BasicNode value from a non-const BasicNode object. Throws exception if
     * the BasicNode value is not of  integer type.
     *
     * @return integer_type& Reference to  integer BasicNode value.
     */
    integer_type& ToInteger()
    {
        if (!IsInteger())
        {
            throw Exception("The target node is not of integer type.");
        }

        return m_node_value.integer;
    }

    /**
     * @brief Returns reference to  integer BasicNode value from a const BasicNode object. Throws exception if the
     * BasicNode value is not of  integer type.
     *
     * @return const integer_type& Constant reference to  integer BasicNode value.
     */
    const integer_type& ToInteger() const
    {
        if (!IsInteger())
        {
            throw Exception("The target node is not of integer type.");
        }

        return m_node_value.integer;
    }

    /**
     * @brief Returns reference to float number BasicNode value from a non-const BasicNode object. Throws exception if
     * the BasicNode value is not of float number type.
     *
     * @return float_number_type& Reference to float number BasicNode value.
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
     * @brief Returns reference to float number BasicNode value from a const BasicNode object. Throws exception if the
     * BasicNode value is not of float number type.
     *
     * @return const float_number_type& Constant reference to float number BasicNode value.
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
     * @brief Returns reference to string BasicNode value from a non-const BasicNode object. Throws exception if the
     * BasicNode value is not of string type.
     *
     * @return string_type& Reference to string BasicNode value.
     */
    string_type& ToString() // NOLINT(readability-make-member-function-const)
    {
        if (!IsString())
        {
            throw Exception("The target node is not of a string type.");
        }

        FK_YAML_ASSERT(m_node_value.str != nullptr);
        return *(m_node_value.str);
    }

    /**
     * @brief Returns reference to string BasicNode value from a const BasicNode object. Throws exception if the
     * BasicNode value is not of string type.
     *
     * @return const string_type& Constant reference to string BasicNode value.
     */
    const string_type& ToString() const
    {
        if (!IsString())
        {
            throw Exception("The target node is not of a string type.");
        }

        FK_YAML_ASSERT(m_node_value.str != nullptr);
        return *(m_node_value.str);
    }

    /**
     * @brief Swaps data with the specified BasicNode object.
     *
     * @param[in] rhs A BasicNode object to be swapped with.
     */
    void Swap(BasicNode& rhs) noexcept
    {
        using std::swap;
        swap(m_node_type, rhs.m_node_type);
        swap(m_yaml_version_type, rhs.m_yaml_version_type);

        NodeValue tmp {};
        std::memcpy(&tmp, &m_node_value, sizeof(NodeValue));
        std::memcpy(&m_node_value, &rhs.m_node_value, sizeof(NodeValue));
        std::memcpy(&rhs.m_node_value, &tmp, sizeof(NodeValue));

        swap(m_anchor_name, rhs.m_anchor_name);
    }

    /**
     * @brief Returns the first iterator of BasicNode values of container types (sequence or mapping) from a non-const
     * BasicNode object. Throws exception if the BasicNode value is not of container types.
     *
     * @return iterator The first iterator of BasicNode values of container types.
     */
    iterator Begin()
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.sequence != nullptr);
            return {fkyaml::SequenceIteratorTag(), m_node_value.sequence->begin()};
        case NodeType::MAPPING:
            FK_YAML_ASSERT(m_node_value.mapping != nullptr);
            return {fkyaml::MappingIteratorTag(), m_node_value.mapping->begin()};
        default:
            throw Exception("The target node is neither of sequence nor mapping types.");
        }
    }

    /**
     * @brief Returns the first iterator of BasicNode values of container types (sequence or mapping) from a non-const
     * BasicNode object. Throws exception if the BasicNode value is not of container types.
     * @note This is a work-around for range-based for statements, and just a wrapper method of Begin() for non-const
     * BasicNode objects.
     *
     * @return iterator The first iterator of BasicNode values of container types.
     */
    iterator begin() // NOLINT(readability-identifier-naming)
    {
        return Begin();
    }

    /**
     * @brief Returns the first iterator of BasicNode values of container types (sequence or mapping) from a const
     * BasicNode object. Throws exception if the BasicNode value is not of container types.
     *
     * @return const_iterator The first iterator of BasicNode values of container types.
     */
    const_iterator Begin() const
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.sequence != nullptr);
            return {fkyaml::SequenceIteratorTag(), m_node_value.sequence->begin()};
        case NodeType::MAPPING:
            FK_YAML_ASSERT(m_node_value.mapping != nullptr);
            return {fkyaml::MappingIteratorTag(), m_node_value.mapping->begin()};
        default:
            throw Exception("The target node is neither of sequence nor mapping types.");
        }
    }

    /**
     * @brief Returns the first iterator of BasicNode values of container types (sequence or mapping) from a const
     * BasicNode object. Throws exception if the BasicNode value is not of container types.
     * @note This is a work-around for range-based for statements, and just a wrapper method of Begin() for const
     * BasicNode objects.
     *
     * @return const_iterator The first iterator of BasicNode values of container types.
     */
    const_iterator begin() const // NOLINT(readability-identifier-naming)
    {
        return Begin();
    }

    /**
     * @brief Returns the last iterator of BasicNode values of container types (sequence or mapping) from a non-const
     * BasicNode object. Throws exception if the BasicNode value is not of container types.
     *
     * @return iterator The last iterator of BasicNode values of container types.
     */
    iterator End()
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.sequence != nullptr);
            return {fkyaml::SequenceIteratorTag(), m_node_value.sequence->end()};
        case NodeType::MAPPING:
            FK_YAML_ASSERT(m_node_value.mapping != nullptr);
            return {fkyaml::MappingIteratorTag(), m_node_value.mapping->end()};
        default:
            throw Exception("The target node is neither of sequence nor mapping types.");
        }
    }

    /**
     * @brief Returns the last iterator of BasicNode values of container types (sequence or mapping) from a non-const
     * BasicNode object. Throws exception if the BasicNode value is not of container types.
     * @note This is a work-around for range-based for statements, and just a wrapper method of End() for const
     * BasicNode objects.
     *
     * @return iterator The last iterator of BasicNode values of container types.
     */
    iterator end() // NOLINT(readability-identifier-naming)
    {
        return End();
    }

    /**
     * @brief Returns the last iterator of BasicNode values of container types (sequence or mapping) from a const
     * BasicNode object. Throws exception if the BasicNode value is not of container types.
     *
     * @return const_iterator The last iterator of BasicNode values of container types.
     */
    const_iterator End() const
    {
        switch (m_node_type)
        {
        case NodeType::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.sequence != nullptr);
            return {fkyaml::SequenceIteratorTag(), m_node_value.sequence->end()};
        case NodeType::MAPPING:
            FK_YAML_ASSERT(m_node_value.mapping != nullptr);
            return {fkyaml::MappingIteratorTag(), m_node_value.mapping->end()};
        default:
            throw Exception("The target node is neither of sequence nor mapping types.");
        }
    }

    /**
     * @brief Returns the last iterator of BasicNode values of container types (sequence or mapping) from a const
     * BasicNode object. Throws exception if the BasicNode value is not of container types.
     * @note This is a work-around for range-based for statements, and just a wrapper method of End() for const
     * BasicNode objects.
     *
     * @return const_iterator The last iterator of BasicNode values of container types.
     */
    const_iterator end() const // NOLINT(readability-identifier-naming)
    {
        return End();
    }

private:
    /** The current node value type. */
    NodeType m_node_type;
    /** The YAML version specification. */
    YamlVersionType m_yaml_version_type;
    /** The current node value. */
    NodeValue m_node_value;
    /** The anchor name for this node. */
    std::string* m_anchor_name;
};

/**
 * @brief default YAML node value container.
 */
using Node = BasicNode<>;

/** A default type for sequence Node values. */
using NodeSequenceType = typename Node::sequence_type;

/** A default type for mapping Node values. */
using NodeMappingType = typename Node::mapping_type;

/** A default type for boolean Node values. */
using NodeBooleanType = typename Node::boolean_type;

/** A default type for integer Node values. */
using NodeIntegerType = typename Node::integer_type;

/** A default type for float number Node values. */
using NodeFloatNumberType = typename Node::float_number_type;

/** A default type for string Node values. */
using NodeStringType = typename Node::string_type;

FK_YAML_NAMESPACE_END

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
