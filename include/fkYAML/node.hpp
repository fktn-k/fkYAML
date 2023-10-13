/**
 *  _______   __ __   __  _____   __  __  __
 * |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
 * |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.1
 * |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
 *
 * SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
 * SPDX-License-Identifier: MIT
 *
 * @file
 */

#ifndef FK_YAML_NODE_HPP_
#define FK_YAML_NODE_HPP_

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "fkYAML/detail/version_macros.hpp"
#include "fkYAML/detail/assert.hpp"
#include "fkYAML/detail/deserializer.hpp"
#include "fkYAML/detail/input_adapter.hpp"
#include "fkYAML/detail/iterator.hpp"
#include "fkYAML/detail/node_t.hpp"
#include "fkYAML/detail/serializer.hpp"
#include "fkYAML/detail/stl_supplement.hpp"
#include "fkYAML/detail/type_traits.hpp"
#include "fkYAML/detail/yaml_version_t.hpp"
#include "fkYAML/exception.hpp"
#include "fkYAML/node_value_converter.hpp"
#include "fkYAML/ordered_map.hpp"

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

/**
 * @class basic_node
 * @brief A class to store value of YAML nodes.
 *
 * @tparam SequenceType A type for sequence node value containers.
 * @tparam MappingType A type for mapping node value containers.
 * @tparam BooleanType A type for boolean node values.
 * @tparam IntegerType A type for integer node values.
 * @tparam FloatNumberType A type for float number node values.
 * @tparam StringType A type for string node values.
 * @tparam Converter A type for node value converter.
 */
template <
    template <typename, typename...> class SequenceType = std::vector,
    template <typename, typename, typename...> class MappingType = ordered_map, typename BooleanType = bool,
    typename IntegerType = std::int64_t, typename FloatNumberType = double, typename StringType = std::string,
    template <typename, typename = void> class Converter = node_value_converter>
class basic_node
{
public:
    /** A type for constant iterators of basic_node containers. */
    using const_iterator = fkyaml::detail::iterator<const basic_node>;
    /** A type for iterators of basic_node containers. */
    using iterator = fkyaml::detail::iterator<basic_node>;

    /** A type for sequence basic_node values. */
    using sequence_type = SequenceType<basic_node, std::allocator<basic_node>>;
    /** A type for mapping basic_node values. */
    using mapping_type = MappingType<StringType, basic_node>;
    /** A type for boolean basic_node values. */
    using boolean_type = BooleanType;
    /** A type for integer basic_node values. */
    using integer_type = IntegerType;
    /** A type for float number basic_node values. */
    using float_number_type = FloatNumberType;
    /** A type for string basic_node values. */
    using string_type = StringType;

    /**
     * @brief A helper alias to determine converter type for the given target native data type.
     *
     * @tparam T A target native data type.
     * @tparam SFINAE A type placeholder for SFINAE
     */
    template <typename T, typename SFINAE>
    using value_converter_type = Converter<T, SFINAE>;

    using node_t = detail::node_t;
    using yaml_version_t = detail::yaml_version_t;

private:
    template <node_t>
    friend struct fkyaml::detail::external_node_constructor;

    /** A type for YAML docs deserializers. */
    using deserializer_type = detail::basic_deserializer<basic_node>;
    /** A type for YAML docs serializers. */
    using serializer_type = detail::basic_serializer<basic_node>;

    /**
     * @union node_value
     * @brief The actual storage for a YAML node value of the @ref basic_node class.
     * @details This union combines the different sotrage types for the YAML value types defined in @ref node_t.
     * @note Container types are stored as pointers so that the size of this union should not exceed 64 bits by default.
     */
    union node_value
    {
        /**
         * @brief Construct a new basic_node Value object for null types.
         */
        node_value() = default;

        /**
         * @brief Construct a new basic_node Value object with basic_node types. The default value for the specified
         * type will be assigned.
         *
         * @param[in] type A Node type.
         */
        explicit node_value(node_t type)
        {
            switch (type)
            {
            case node_t::SEQUENCE:
                p_sequence = create_object<sequence_type>();
                break;
            case node_t::MAPPING:
                p_mapping = create_object<mapping_type>();
                break;
            case node_t::NULL_OBJECT:
                p_mapping = nullptr;
                break;
            case node_t::BOOLEAN:
                boolean = static_cast<boolean_type>(false);
                break;
            case node_t::INTEGER:
                integer = static_cast<integer_type>(0);
                break;
            case node_t::FLOAT_NUMBER:
                float_val = static_cast<float_number_type>(0.0);
                break;
            case node_t::STRING:
                p_string = create_object<string_type>("");
                break;
            default:                                                     // LCOV_EXCL_LINE
                throw fkyaml::exception("Unsupported node value type."); // LCOV_EXCL_LINE
            }
        }

        /**
         * @brief Destroy the existing Node value. This process is recursive if the specified node type is fpr
         * containers.
         *
         * @param[in] type A Node type to determine which Node value is destroyed.
         */
        void destroy(node_t type)
        {
            if (type == node_t::SEQUENCE || type == node_t::MAPPING)
            {
                std::vector<basic_node> stack;

                if (type == node_t::SEQUENCE)
                {
                    stack.reserve(p_sequence->size());
                    std::move(p_sequence->begin(), p_sequence->end(), std::back_inserter(stack));
                }
                else
                {
                    stack.reserve(p_mapping->size());
                    for (auto&& it : *p_mapping)
                    {
                        stack.push_back(std::move(it.second));
                    }
                }

                while (!stack.empty())
                {
                    basic_node current_node(std::move(stack.back()));
                    stack.pop_back();

                    if (current_node.is_sequence())
                    {
                        std::move(
                            current_node.m_node_value.p_sequence->begin(),
                            current_node.m_node_value.p_sequence->end(),
                            std::back_inserter(stack));
                        current_node.m_node_value.p_sequence->clear();
                    }
                    else if (current_node.is_mapping())
                    {
                        for (auto&& it : *current_node.m_node_value.p_mapping)
                        {
                            stack.push_back(std::move(it.second));
                        }
                        current_node.m_node_value.p_mapping->clear();
                    }
                }
            }

            switch (type)
            {
            case node_t::SEQUENCE:
                destroy_object<sequence_type>(p_sequence);
                p_sequence = nullptr;
                break;
            case node_t::MAPPING:
                destroy_object<mapping_type>(p_mapping);
                p_mapping = nullptr;
                break;
            case node_t::STRING:
                destroy_object<string_type>(p_string);
                p_string = nullptr;
                break;
            default:
                break;
            }
        }

        /** A pointer to the value of sequence type. */
        sequence_type* p_sequence;
        /** A pointer to the value of mapping type. This pointer is also used when node type is null. */
        mapping_type* p_mapping;
        /** A value of boolean type. */
        boolean_type boolean;
        /** A value of integer type. */
        integer_type integer;
        /** A value of float number type. */
        float_number_type float_val;
        /** A pointer to the value of string type. */
        string_type* p_string;
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
    static ObjType* create_object(ArgTypes&&... args)
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
    static void destroy_object(ObjType* obj)
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
     * @brief Construct a new basic_node object of null type.
     */
    basic_node() noexcept
        : m_node_type(node_t::NULL_OBJECT),
          m_yaml_version_type(yaml_version_t::VER_1_2),
          m_node_value(),
          m_anchor_name(nullptr)
    {
    }

    /**
     * @brief Construct a new basic_node object with a specified type.
     * @note If you construct an alias node, call basic_node::AliasOf() instead.
     *
     * @param[in] type A YAML node value type.
     */
    explicit basic_node(const node_t type)
        : m_node_type(type),
          m_yaml_version_type(yaml_version_t::VER_1_2),
          m_node_value(type),
          m_anchor_name(nullptr)
    {
    }

    /**
     * @brief Copy constructor of the basic_node class.
     *
     * @param[in] rhs A basic_node object to be copied with.
     */
    basic_node(const basic_node& rhs)
        : m_node_type(rhs.m_node_type),
          m_yaml_version_type(rhs.m_yaml_version_type),
          m_anchor_name(nullptr)
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            m_node_value.p_sequence = create_object<sequence_type>(*(rhs.m_node_value.p_sequence));
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            break;
        case node_t::MAPPING:
            m_node_value.p_mapping = create_object<mapping_type>(*(rhs.m_node_value.p_mapping));
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            break;
        case node_t::NULL_OBJECT:
            m_node_value.p_mapping = nullptr;
            break;
        case node_t::BOOLEAN:
            m_node_value.boolean = rhs.m_node_value.boolean;
            break;
        case node_t::INTEGER:
            m_node_value.integer = rhs.m_node_value.integer;
            break;
        case node_t::FLOAT_NUMBER:
            m_node_value.float_val = rhs.m_node_value.float_val;
            break;
        case node_t::STRING:
            m_node_value.p_string = create_object<string_type>(*(rhs.m_node_value.p_string));
            FK_YAML_ASSERT(m_node_value.p_string != nullptr);
            break;
        default:                                                       // LCOV_EXCL_LINE
            throw fkyaml::exception("Not supported node value type."); // LCOV_EXCL_LINE
        }

        if (rhs.m_anchor_name)
        {
            destroy_object<std::string>(m_anchor_name);
            m_anchor_name = create_object<std::string>(*(rhs.m_anchor_name));
            FK_YAML_ASSERT(m_anchor_name != nullptr);
        }
    }

    /**
     * @brief Move constructor of the basic_node class.
     *
     * @param[in] rhs A basic_node object to be moved from.
     */
    basic_node(basic_node&& rhs) noexcept // NOLINT(bugprone-exception-escape)
        : m_node_type(rhs.m_node_type),
          m_yaml_version_type(rhs.m_yaml_version_type),
          m_anchor_name(rhs.m_anchor_name)
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            FK_YAML_ASSERT(rhs.m_node_value.p_sequence != nullptr);
            m_node_value.p_sequence = rhs.m_node_value.p_sequence;
            rhs.m_node_value.p_sequence = nullptr;
            break;
        case node_t::MAPPING:
            FK_YAML_ASSERT(rhs.m_node_value.p_mapping != nullptr);
            m_node_value.p_mapping = rhs.m_node_value.p_mapping;
            rhs.m_node_value.p_mapping = nullptr;
            break;
        case node_t::NULL_OBJECT:
            FK_YAML_ASSERT(rhs.m_node_value.p_mapping == nullptr);
            m_node_value.p_mapping = rhs.m_node_value.p_mapping;
            break;
        case node_t::BOOLEAN:
            m_node_value.boolean = rhs.m_node_value.boolean;
            rhs.m_node_value.boolean = static_cast<boolean_type>(false);
            break;
        case node_t::INTEGER:
            m_node_value.integer = rhs.m_node_value.integer;
            rhs.m_node_value.integer = static_cast<integer_type>(0);
            break;
        case node_t::FLOAT_NUMBER:
            m_node_value.float_val = rhs.m_node_value.float_val;
            rhs.m_node_value.float_val = static_cast<float_number_type>(0.0);
            break;
        case node_t::STRING:
            FK_YAML_ASSERT(rhs.m_node_value.p_string != nullptr);
            m_node_value.p_string = rhs.m_node_value.p_string;
            rhs.m_node_value.p_string = nullptr;
            break;
        default:                                                     // LCOV_EXCL_LINE
            throw fkyaml::exception("Unsupported node value type."); // LCOV_EXCL_LINE
        }

        rhs.m_node_type = node_t::NULL_OBJECT;
        rhs.m_yaml_version_type = yaml_version_t::VER_1_2;
        rhs.m_node_value.p_mapping = nullptr;
        rhs.m_anchor_name = nullptr;
    }

    template <
        typename CompatibleType, typename U = detail::remove_cvref_t<CompatibleType>,
        detail::enable_if_t<
            detail::conjunction<
                detail::negation<detail::is_basic_node<U>>,
                detail::disjunction<
                    std::is_same<CompatibleType, std::nullptr_t>, detail::is_compatible_type<basic_node, U>>>::value,
            int> = 0>
    explicit basic_node(CompatibleType&& val) noexcept(
        noexcept(Converter<U>::to_node(std::declval<basic_node&>(), std::declval<CompatibleType>())))
        : m_node_type(node_t::NULL_OBJECT),
          m_yaml_version_type(yaml_version_t::VER_1_2),
          m_node_value(),
          m_anchor_name(nullptr)
    {
        Converter<U>::to_node(*this, std::forward<CompatibleType>(val));
    }

    /**
     * @brief Destroy the basic_node object and its value storage.
     */
    ~basic_node() noexcept // NOLINT(bugprone-exception-escape)
    {
        destroy_object<std::string>(m_anchor_name);
        m_anchor_name = nullptr;
        m_node_value.destroy(m_node_type);
        m_node_type = node_t::NULL_OBJECT;
    }

public:
    /**
     * @brief Deserialize an input source into a basic_node object.
     *
     * @tparam InputType A type of an input source.
     * @param input An input source in YAML format.
     * @return basic_node The result of deserialization.
     */
    template <typename InputType>
    static basic_node deserialize(InputType&& input)
    {
        return deserializer_type().deserialize(detail::input_adapter(std::forward<InputType>(input)));
    }

    /**
     * @brief Deserialize input iterators into a basic_node object.
     *
     * @tparam ItrType A type of input iterators.
     * @param begin The beginning of input iterators.
     * @param end The end of input iterators.
     * @return basic_node The result of deserialization.
     */
    template <typename ItrType>
    static basic_node deserialize(ItrType&& begin, ItrType&& end)
    {
        return deserializer_type().deserialize(
            detail::input_adapter(std::forward<ItrType>(begin), std::forward<ItrType>(end)));
    }

    /**
     * @brief Serialize a basic_node object into a string.
     *
     * @param node A basic_node object.
     * @return std::string The result of serialization.
     */
    static std::string serialize(const basic_node& node)
    {
        return serializer_type().serialize(node);
    }

    /**
     * @brief A factory method for sequence basic_node objects without sequence_type objects.
     *
     * @return basic_node A constructed basic_node object of sequence type.
     */
    static basic_node sequence()
    {
        basic_node node;
        node.m_node_type = node_t::SEQUENCE;
        node.m_node_value.p_sequence = create_object<sequence_type>();
        FK_YAML_ASSERT(node.m_node_value.p_sequence != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for sequence basic_node objects with lvalue sequence_type objects.
     *
     * @param[in] seq A lvalue source of sequence type.
     * @return basic_node A constructed basic_node object of sequence type.
     */
    static basic_node sequence(const sequence_type& seq)
    {
        basic_node node;
        node.m_node_type = node_t::SEQUENCE;
        node.m_node_value.p_sequence = create_object<sequence_type>(seq);
        FK_YAML_ASSERT(node.m_node_value.p_sequence != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for sequence basic_node objects with rvalue sequence_type objects.
     *
     * @param[in] seq A rvalue source of sequence type.
     * @return basic_node A constructed basic_node object of sequence type.
     */
    static basic_node sequence(sequence_type&& seq)
    {
        basic_node node;
        node.m_node_type = node_t::SEQUENCE;
        node.m_node_value.p_sequence = create_object<sequence_type>(std::move(seq));
        FK_YAML_ASSERT(node.m_node_value.p_sequence != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for mapping basic_node objects without mapping_type objects.
     *
     * @return basic_node A constructed basic_node object of mapping type.
     */
    static basic_node mapping()
    {
        basic_node node;
        node.m_node_type = node_t::MAPPING;
        node.m_node_value.p_mapping = create_object<mapping_type>();
        FK_YAML_ASSERT(node.m_node_value.p_mapping != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for mapping basic_node objects with lvalue mapping_type objects.
     *
     * @param[in] map A lvalue source of mapping type.
     * @return basic_node A constructed basic_node object of mapping type.
     */
    static basic_node mapping(const mapping_type& map)
    {
        basic_node node;
        node.m_node_type = node_t::MAPPING;
        node.m_node_value.p_mapping = create_object<mapping_type>(map);
        FK_YAML_ASSERT(node.m_node_value.p_mapping != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for mapping basic_node objects with rvalue mapping_type objects.
     *
     * @param[in] map A rvalue source of mapping type.
     * @return basic_node A constructed basic_node object of mapping type.
     */
    static basic_node mapping(mapping_type&& map)
    {
        basic_node node;
        node.m_node_type = node_t::MAPPING;
        node.m_node_value.p_mapping = create_object<mapping_type>(std::move(map));
        FK_YAML_ASSERT(node.m_node_value.p_mapping != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for boolean scalar basic_node objects.
     *
     * @param[in] boolean A source of boolean type.
     * @return basic_node A constructed basic_node object of boolean type.
     */
    static basic_node boolean_scalar(const boolean_type boolean) noexcept
    {
        basic_node node;
        node.m_node_type = node_t::BOOLEAN;
        node.m_node_value.boolean = boolean;
        return node;
    }

    /**
     * @brief A factory method for integer scalar basic_node objects.
     *
     * @param[in] integer A source of integer type.
     * @return basic_node A constructed basic_node object of integer type.
     */
    static basic_node integer_scalar(const integer_type integer) noexcept
    {
        basic_node node;
        node.m_node_type = node_t::INTEGER;
        node.m_node_value.integer = integer;
        return node;
    }

    /**
     * @brief A factory method for float number scalar basic_node objects.
     *
     * @param[in] float_val A source of unsigned integer type.
     * @return basic_node A constructed basic_node object of float number type.
     */
    static basic_node float_number_scalar(const float_number_type float_val) noexcept
    {
        basic_node node;
        node.m_node_type = node_t::FLOAT_NUMBER;
        node.m_node_value.float_val = float_val;
        return node;
    }

    /**
     * @brief A factory method for string basic_node objects without string_type objects.
     *
     * @return basic_node A constructed basic_node object of string type.
     */
    static basic_node string_scalar()
    {
        basic_node node;
        node.m_node_type = node_t::STRING;
        node.m_node_value.p_string = create_object<string_type>();
        FK_YAML_ASSERT(node.m_node_value.p_string != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for string basic_node objects with lvalue string_type objects.
     *
     * @param[in] str A lvalue source of string type.
     * @return basic_node A constructed basic_node object of string type.
     */
    static basic_node string_scalar(const string_type& str)
    {
        basic_node node;
        node.m_node_type = node_t::STRING;
        node.m_node_value.p_string = create_object<string_type>(str);
        FK_YAML_ASSERT(node.m_node_value.p_string != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for string basic_node objects with rvalue string_type objects.
     *
     * @param[in] str A rvalue source of string type.
     * @return basic_node A constructed basic_node object of string type.
     */
    static basic_node string_scalar(string_type&& str)
    {
        basic_node node;
        node.m_node_type = node_t::STRING;
        node.m_node_value.p_string = create_object<string_type>(std::move(str));
        FK_YAML_ASSERT(node.m_node_value.p_string != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /**
     * @brief A factory method for alias basic_node objects referencing the given anchor basic_node object.
     * @note The given anchor basic_node must have a non-empty anchor name.
     *
     * @param anchor_node An anchor node to be referenced by the newly constructed basic_node object.
     * @return basic_node A constructed basic_node object of alias type.
     */
    static basic_node alias_of(const basic_node& anchor_node)
    {
        if (!anchor_node.m_anchor_name || anchor_node.m_anchor_name->empty())
        {
            throw fkyaml::exception("Cannot create an alias without anchor name.");
        }

        basic_node node = anchor_node;
        return node;
    }

public:
    /**
     * @brief A copy assignment operator of the basic_node class.
     *
     * @param[in] rhs A lvalue basic_node object to be copied with.
     * @return basic_node& Reference to this basic_node object.
     */
    basic_node& operator=(const basic_node& rhs) noexcept
    {
        basic_node(rhs).swap(*this);
        return *this;
    }

    /**
     * @brief A move assignment operator of the basic_node class.
     *
     * @param[in] rhs A rvalue basic_node object to be moved from.
     * @return basic_node& Reference to this basic_node object.
     */
    basic_node& operator=(basic_node&& rhs) noexcept
    {
        basic_node(std::move(rhs)).swap(*this);
        return *this;
    }

    /**
     * @brief A subscript operator for non-const basic_node objects.
     *
     * @param[in] index An index of sequence basic_node values.
     * @return basic_node& Reference to a basic_node object located at the specified index.
     */
    basic_node& operator[](std::size_t index) // NOLINT(readability-make-member-function-const)
    {
        if (!is_sequence())
        {
            throw fkyaml::exception("The target node is not of a sequence type.");
        }

        FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
        FK_YAML_ASSERT(index < m_node_value.p_sequence->size());
        return m_node_value.p_sequence->operator[](index);
    }

    /**
     * @brief A subscript operator for const basic_node objects.
     *
     * @param[in] index An index of sequence basic_node values.
     * @return const basic_node& Constant reference to a basic_node object located at the specified index.
     */
    const basic_node& operator[](std::size_t index) const
    {
        if (!is_sequence())
        {
            throw fkyaml::exception("The target node is not of a sequence type.");
        }

        FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
        FK_YAML_ASSERT(index < m_node_value.p_sequence->size());
        return m_node_value.p_sequence->operator[](index);
    }

    /**
     * @brief A subscript operator for non-const basic_node objects.
     *
     * @tparam KeyType A type for the input key.
     * @param[in] key A key to the target basic_node object..
     * @return basic_node& Reference to a basic_node object associated with the given key.
     */
    template <
        typename KeyType, detail::enable_if_t<
                              detail::is_usable_as_key_type<
                                  typename mapping_type::key_compare, typename mapping_type::key_type, KeyType>::value,
                              int> = 0>
    basic_node& operator[](KeyType&& key) // NOLINT(readability-make-member-function-const)
    {
        if (!is_mapping())
        {
            throw fkyaml::exception("The target node is not of a mapping type.");
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        return m_node_value.p_mapping->operator[](std::forward<KeyType>(key));
    }

    /**
     * @brief A subscript operator for const basic_node objects.
     *
     * @tparam KeyType A type for the input key.
     * @param[in] key A key to the basic_node object.
     * @return const basic_node& Constant reference to a basic_node object associated with the given key.
     */
    template <
        typename KeyType, detail::enable_if_t<
                              detail::is_usable_as_key_type<
                                  typename mapping_type::key_compare, typename mapping_type::key_type, KeyType>::value,
                              int> = 0>
    const basic_node& operator[](KeyType&& key) const
    {
        if (!is_mapping())
        {
            throw fkyaml::exception("The target node is not of a mapping type.");
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        return m_node_value.p_mapping->operator[](std::forward<KeyType>(key));
    }

public:
    /**
     * @brief Returns the type of the current basic_node value.
     *
     * @return node_t The type of the current basic_node value.
     * @retval node_t::SEQUENCE         sequence type.
     * @retval node_t::MAPPINT          mapping type.
     * @retval node_t::NULL_OBJECT      null type.
     * @retval node_t::BOOLEAN          boolean type.
     * @retval node_t::INTEGER          integer type.
     * @retval node_t::FLOAT_NUMBER     float number type.
     * @retval node_t::STRING           string type.
     */
    node_t type() const noexcept
    {
        return m_node_type;
    }

    /**
     * @brief Tests whether the current basic_node value is of sequence type.
     *
     * @return bool A result of testing whetehre the current basic_node value is of sequence type.
     * @retval true  The current basic_node value is of sequence type.
     * @return false The current basic_node value is not of sequence type.
     */
    bool is_sequence() const noexcept
    {
        return m_node_type == node_t::SEQUENCE;
    }

    /**
     * @brief Tests whether the current basic_node value is of mapping type.
     *
     * @return bool A result of testing whetehre the current basic_node value is of mapping type.
     * @retval true  The current basic_node value is of mapping type.
     * @return false The current basic_node value is not of mapping type.
     */
    bool is_mapping() const noexcept
    {
        return m_node_type == node_t::MAPPING;
    }

    /**
     * @brief Tests whether the current basic_node value is of null type.
     *
     * @return bool A result of testing whetehre the current basic_node value is of null type.
     * @retval true  The current basic_node value is of null type.
     * @return false The current basic_node value is not of null type.
     */
    bool is_null() const noexcept
    {
        return m_node_type == node_t::NULL_OBJECT;
    }

    /**
     * @brief Tests whether the current basic_node value is of boolean type.
     *
     * @return bool A result of testing whetehre the current basic_node value is of boolean type.
     * @retval true  The current basic_node value is of boolean type.
     * @return false The current basic_node value is not of boolean type.
     */
    bool is_boolean() const noexcept
    {
        return m_node_type == node_t::BOOLEAN;
    }

    /**
     * @brief Tests whether the current basic_node value is of integer type.
     *
     * @return bool A result of testing whetehre the current basic_node value is of integer type.
     * @retval true  The current basic_node value is of integer type.
     * @return false The current basic_node value is not of integer type.
     */
    bool is_integer() const noexcept
    {
        return m_node_type == node_t::INTEGER;
    }

    /**
     * @brief Tests whether the current basic_node value is of float number type.
     *
     * @return bool A result of testing whetehre the current basic_node value is of float number type.
     * @retval true  The current basic_node value is of float number type.
     * @return false The current basic_node value is not of float number type.
     */
    bool is_float_number() const noexcept
    {
        return m_node_type == node_t::FLOAT_NUMBER;
    }

    /**
     * @brief Tests whether the current basic_node value is of string type.
     *
     * @return bool A result of testing whetehre the current basic_node value is of string type.
     * @retval true  The current basic_node value is of string type.
     * @return false The current basic_node value is not of string type.
     */
    bool is_string() const noexcept
    {
        return m_node_type == node_t::STRING;
    }

    /**
     * @brief Tests whether the current basic_node value is of scalar types.
     *
     * @return bool A result of testing whetehre the current basic_node value is of scalar types.
     * @retval true  The current basic_node value is of scalar types.
     * @return false The current basic_node value is not of scalar types.
     */
    bool is_scalar() const noexcept
    {
        return !is_sequence() && !is_mapping();
    }

    /**
     * @brief Tests whether the current basic_node value (sequence, mapping, string) is empty.
     *
     * @return bool A result of testing whetehre the current basic_node value is empty.
     * @retval true  The current basic_node value is empty.
     * @return false The current basic_node value is not empty.
     */
    bool empty() const
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return m_node_value.p_sequence->empty();
        case node_t::MAPPING:
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            return m_node_value.p_mapping->empty();
        case node_t::STRING:
            FK_YAML_ASSERT(m_node_value.p_string != nullptr);
            return m_node_value.p_string->empty();
        default:
            throw fkyaml::exception("The target node is not of a container type.");
        }
    }

    /**
     * @brief Returns the size of the current basic_node value (sequence, mapping, string).
     *
     * @return std::size_t The size of the current basic_node value.
     */
    std::size_t size() const
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return m_node_value.p_sequence->size();
        case node_t::MAPPING:
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            return m_node_value.p_mapping->size();
        case node_t::STRING:
            FK_YAML_ASSERT(m_node_value.p_string != nullptr);
            return m_node_value.p_string->size();
        default:
            throw fkyaml::exception("The target node is not of a container type.");
        }
    }

    /**
     * @brief Check whether or not this basic_node object has a given key in its inner mapping Node value.
     *
     * @tparam KeyType A type for the input key.
     * @param[in] key A key to the target basic_node object.
     * @return true If this basic_node object has a given key.
     * @return false If this basic_node object does not have a given key.
     */
    template <
        typename KeyType, detail::enable_if_t<
                              detail::is_usable_as_key_type<
                                  typename mapping_type::key_compare, typename mapping_type::key_type, KeyType>::value,
                              int> = 0>
    bool contains(KeyType&& key) const
    {
        switch (m_node_type)
        {
        case node_t::MAPPING: {
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            mapping_type& map = *m_node_value.p_mapping;
            return map.find(std::forward<KeyType>(key)) != map.end();
        }
        default:
            return false;
        }
    }

    /**
     * @brief Get the YAML version specification for this basic_node object.
     *
     * @return yaml_version_t The YAML version specification.
     */
    yaml_version_t get_yaml_version() const noexcept
    {
        return m_yaml_version_type;
    }

    /**
     * @brief Set the YAML version specification for this basic_node object.
     *
     * @param version The YAML version specification.
     */
    void set_yaml_version(const yaml_version_t version) noexcept
    {
        m_yaml_version_type = version;
    }

    /**
     * @brief Check whether or not this basic_node object has already had any anchor name.
     *
     * @return true If this basic_node object has already had any anchor name.
     * @return false If this basic_node object has not had any anchor name yet.
     */
    bool has_anchor_name() const noexcept
    {
        return m_anchor_name != nullptr;
    }

    /**
     * @brief Get the anchor name associated to this basic_node object.
     * @note Some anchor name must be set before calling this method. Call basic_node::HasAnchorName() to see if this
     * basic_node object has any anchor name.
     *
     * @return const std::string& Const reference to the anchor name.
     */
    const std::string& get_anchor_name() const
    {
        if (!m_anchor_name)
        {
            throw fkyaml::exception("No anchor name has been set.");
        }
        return *m_anchor_name;
    }

    /**
     * @brief Add an anchor name to this basic_node object.
     * @note If this basic_node object has already had any anchor name, the new anchor name will overwrite the old one.
     *
     * @param anchor_name An anchor name associated to this basic_node object.
     */
    void add_anchor_name(const std::string& anchor_name)
    {
        destroy_object<std::string>(m_anchor_name);
        m_anchor_name = create_object<std::string>(anchor_name);
        FK_YAML_ASSERT(m_anchor_name != nullptr);
    }

    /**
     * @brief Add an anchor name to this basic_node object.
     * @note If this basic_node object has already had any anchor name, the new anchor name will overwrite the old one.
     *
     * @param anchor_name An anchor name associated to this basic_node object.
     */
    void add_anchor_name(std::string&& anchor_name)
    {
        destroy_object<std::string>(m_anchor_name);
        m_anchor_name = create_object<std::string>(std::move(anchor_name));
        FK_YAML_ASSERT(m_anchor_name != nullptr);
    }

    /**
     * @brief Get the node value object converted into a given type.
     * @note This function requires T objects to be default constructible.
     *
     * @tparam T A native data type for conversion.
     * @tparam ValueType T without cv qualifiers and reference.
     * @return T A native data converted from the node value.
     */
    template <
        typename T, typename ValueType = detail::remove_cvref_t<T>,
        detail::enable_if_t<
            detail::conjunction<
                std::is_default_constructible<ValueType>, detail::has_from_node<basic_node, ValueType>>::value,
            int> = 0>
    T get_value() const noexcept(
        noexcept(Converter<ValueType>::from_node(std::declval<const basic_node&>(), std::declval<ValueType&>())))
    {
        auto ret = ValueType();
        Converter<ValueType>::from_node(*this, ret);
        return ret;
    }

    /**
     * @brief Returns reference to sequence basic_node value from a non-const basic_node object. Throws exception if the
     * basic_node value is not of sequence type.
     *
     * @return sequence_type& Reference to sequence basic_node value.
     */
    sequence_type& to_sequence() // NOLINT(readability-make-member-function-const)
    {
        if (!is_sequence())
        {
            throw fkyaml::exception("The target node is not of a sequence type.");
        }

        FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
        return *(m_node_value.p_sequence);
    }

    /**
     * @brief Returns reference to sequence basic_node value from a const basic_node object.  Throws exception if the
     * basic_node value is not of sequence type.
     *
     * @return const sequence_type& Constant reference to sequence basic_node value.
     */
    const sequence_type& to_sequence() const
    {
        if (!is_sequence())
        {
            throw fkyaml::exception("The target node is not of a sequence type.");
        }

        FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
        return *(m_node_value.p_sequence);
    }

    /**
     * @brief Returns reference to mapping basic_node value from a non-const basic_node object. Throws exception if the
     * basic_node value is not of mapping type.
     *
     * @return mapping_type& Reference to mapping basic_node value.
     */
    mapping_type& to_mapping() // NOLINT(readability-make-member-function-const)
    {
        if (!is_mapping())
        {
            throw fkyaml::exception("The target node is not of a mapping type.");
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        return *(m_node_value.p_mapping);
    }

    /**
     * @brief Returns reference to mapping basic_node value from a const basic_node object.  Throws exception if the
     * basic_node value is not of mapping type.
     *
     * @return const mapping_type& Constant reference to mapping basic_node value.
     */
    const mapping_type& to_mapping() const
    {
        if (!is_mapping())
        {
            throw fkyaml::exception("The target node is not of a mapping type.");
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        return *(m_node_value.p_mapping);
    }

    /**
     * @brief Returns reference to boolean basic_node value from a non-const basic_node object. Throws exception if the
     * basic_node value is not of boolean type.
     *
     * @return boolean_type& Reference to boolean basic_node value.
     */
    boolean_type& to_boolean()
    {
        if (!is_boolean())
        {
            throw fkyaml::exception("The target node is not of a boolean type.");
        }

        return m_node_value.boolean;
    }

    /**
     * @brief Returns reference to boolean basic_node value from a const basic_node object.  Throws exception if the
     * basic_node value is not of boolean type.
     *
     * @return const boolean_type& Constant reference to boolean basic_node value.
     */
    const boolean_type& to_boolean() const
    {
        if (!is_boolean())
        {
            throw fkyaml::exception("The target node is not of a boolean type.");
        }

        return m_node_value.boolean;
    }

    /**
     * @brief Returns reference to  integer basic_node value from a non-const basic_node object. Throws exception if
     * the basic_node value is not of  integer type.
     *
     * @return integer_type& Reference to  integer basic_node value.
     */
    integer_type& to_integer()
    {
        if (!is_integer())
        {
            throw fkyaml::exception("The target node is not of integer type.");
        }

        return m_node_value.integer;
    }

    /**
     * @brief Returns reference to  integer basic_node value from a const basic_node object. Throws exception if the
     * basic_node value is not of  integer type.
     *
     * @return const integer_type& Constant reference to  integer basic_node value.
     */
    const integer_type& to_integer() const
    {
        if (!is_integer())
        {
            throw fkyaml::exception("The target node is not of integer type.");
        }

        return m_node_value.integer;
    }

    /**
     * @brief Returns reference to float number basic_node value from a non-const basic_node object. Throws exception if
     * the basic_node value is not of float number type.
     *
     * @return float_number_type& Reference to float number basic_node value.
     */
    float_number_type& to_float_number()
    {
        if (!is_float_number())
        {
            throw fkyaml::exception("The target node is not of a float number type.");
        }

        return m_node_value.float_val;
    }

    /**
     * @brief Returns reference to float number basic_node value from a const basic_node object. Throws exception if the
     * basic_node value is not of float number type.
     *
     * @return const float_number_type& Constant reference to float number basic_node value.
     */
    const float_number_type& to_float_number() const
    {
        if (!is_float_number())
        {
            throw fkyaml::exception("The target node is not of a float number type.");
        }

        return m_node_value.float_val;
    }

    /**
     * @brief Returns reference to string basic_node value from a non-const basic_node object. Throws exception if the
     * basic_node value is not of string type.
     *
     * @return string_type& Reference to string basic_node value.
     */
    string_type& to_string() // NOLINT(readability-make-member-function-const)
    {
        if (!is_string())
        {
            throw fkyaml::exception("The target node is not of a string type.");
        }

        FK_YAML_ASSERT(m_node_value.p_string != nullptr);
        return *(m_node_value.p_string);
    }

    /**
     * @brief Returns reference to string basic_node value from a const basic_node object. Throws exception if the
     * basic_node value is not of string type.
     *
     * @return const string_type& Constant reference to string basic_node value.
     */
    const string_type& to_string() const
    {
        if (!is_string())
        {
            throw fkyaml::exception("The target node is not of a string type.");
        }

        FK_YAML_ASSERT(m_node_value.p_string != nullptr);
        return *(m_node_value.p_string);
    }

    /**
     * @brief Swaps data with the specified basic_node object.
     *
     * @param[in] rhs A basic_node object to be swapped with.
     */
    void swap(basic_node& rhs) noexcept
    {
        using std::swap;
        swap(m_node_type, rhs.m_node_type);
        swap(m_yaml_version_type, rhs.m_yaml_version_type);

        node_value tmp {};
        std::memcpy(&tmp, &m_node_value, sizeof(node_value));
        std::memcpy(&m_node_value, &rhs.m_node_value, sizeof(node_value));
        std::memcpy(&rhs.m_node_value, &tmp, sizeof(node_value));

        swap(m_anchor_name, rhs.m_anchor_name);
    }

    /**
     * @brief Returns the first iterator of basic_node values of container types (sequence or mapping) from a non-const
     * basic_node object. Throws exception if the basic_node value is not of container types.
     *
     * @return iterator The first iterator of basic_node values of container types.
     */
    iterator begin()
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return {detail::sequence_iterator_tag(), m_node_value.p_sequence->begin()};
        case node_t::MAPPING:
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            return {detail::mapping_iterator_tag(), m_node_value.p_mapping->begin()};
        default:
            throw fkyaml::exception("The target node is neither of sequence nor mapping types.");
        }
    }

    /**
     * @brief Returns the first iterator of basic_node values of container types (sequence or mapping) from a const
     * basic_node object. Throws exception if the basic_node value is not of container types.
     *
     * @return const_iterator The first iterator of basic_node values of container types.
     */
    const_iterator begin() const
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return {detail::sequence_iterator_tag(), m_node_value.p_sequence->begin()};
        case node_t::MAPPING:
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            return {detail::mapping_iterator_tag(), m_node_value.p_mapping->begin()};
        default:
            throw fkyaml::exception("The target node is neither of sequence nor mapping types.");
        }
    }

    /**
     * @brief Returns the last iterator of basic_node values of container types (sequence or mapping) from a non-const
     * basic_node object. Throws exception if the basic_node value is not of container types.
     *
     * @return iterator The last iterator of basic_node values of container types.
     */
    iterator end()
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return {detail::sequence_iterator_tag(), m_node_value.p_sequence->end()};
        case node_t::MAPPING:
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            return {detail::mapping_iterator_tag(), m_node_value.p_mapping->end()};
        default:
            throw fkyaml::exception("The target node is neither of sequence nor mapping types.");
        }
    }

    /**
     * @brief Returns the last iterator of basic_node values of container types (sequence or mapping) from a const
     * basic_node object. Throws exception if the basic_node value is not of container types.
     *
     * @return const_iterator The last iterator of basic_node values of container types.
     */
    const_iterator end() const
    {
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return {detail::sequence_iterator_tag(), m_node_value.p_sequence->end()};
        case node_t::MAPPING:
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            return {detail::mapping_iterator_tag(), m_node_value.p_mapping->end()};
        default:
            throw fkyaml::exception("The target node is neither of sequence nor mapping types.");
        }
    }

private:
    /** The current node value type. */
    node_t m_node_type;
    /** The YAML version specification. */
    yaml_version_t m_yaml_version_type;
    /** The current node value. */
    node_value m_node_value;
    /** The anchor name for this node. */
    std::string* m_anchor_name;
};

/**
 * @brief default YAML node value container.
 */
using node = basic_node<>;

/** A default type for sequence node values. */
using node_sequence_type = typename node::sequence_type;

/** A default type for mapping node values. */
using node_mapping_type = typename node::mapping_type;

/** A default type for boolean node values. */
using node_boolean_type = typename node::boolean_type;

/** A default type for integer node values. */
using node_integer_type = typename node::integer_type;

/** A default type for float number node values. */
using node_float_number_type = typename node::float_number_type;

/** A default type for string node values. */
using node_string_type = typename node::string_type;

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
inline void swap<fkyaml::node>(fkyaml::node& lhs, fkyaml::node& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

} // namespace std

#endif /* FK_YAML_NODE_HPP_ */
