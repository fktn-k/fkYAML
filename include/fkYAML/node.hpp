///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_NODE_HPP_
#define FK_YAML_NODE_HPP_

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/assert.hpp>
#include <fkYAML/detail/input/deserializer.hpp>
#include <fkYAML/detail/input/input_adapter.hpp>
#include <fkYAML/detail/iterator.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>
#include <fkYAML/detail/node_property.hpp>
#include <fkYAML/detail/node_ref_storage.hpp>
#include <fkYAML/detail/output/serializer.hpp>
#include <fkYAML/detail/types/node_t.hpp>
#include <fkYAML/detail/types/yaml_version_t.hpp>
#include <fkYAML/exception.hpp>
#include <fkYAML/node_value_converter.hpp>
#include <fkYAML/ordered_map.hpp>

/// @brief namespace for fkYAML library.
FK_YAML_NAMESPACE_BEGIN

/// @brief A class to store value of YAML nodes.
/// @sa https://fktn-k.github.io/fkYAML/api/basic_node/
template <
    template <typename, typename...> class SequenceType = std::vector,
    template <typename, typename, typename...> class MappingType = std::map, typename BooleanType = bool,
    typename IntegerType = std::int64_t, typename FloatNumberType = double, typename StringType = std::string,
    template <typename, typename = void> class ConverterType = node_value_converter>
class basic_node
{
public:
    /// @brief A type for iterators of basic_node containers.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/iterator/
    using iterator = fkyaml::detail::iterator<basic_node>;

    /// @brief A type for constant iterators of basic_node containers.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/const_iterator/
    using const_iterator = fkyaml::detail::iterator<const basic_node>;

    /// @brief A type for sequence basic_node values.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/sequence_type/
    using sequence_type = SequenceType<basic_node, std::allocator<basic_node>>;

    /// @brief A type for mapping basic_node values.
    /// @note std::unordered_map is not supported since it does not allow incomplete types.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/mapping_type/
    using mapping_type = MappingType<basic_node, basic_node>;

    /// @brief A type for boolean basic_node values.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/boolean_type/
    using boolean_type = BooleanType;

    /// @brief A type for integer basic_node values.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/integer_type/
    using integer_type = IntegerType;

    /// @brief A type for float number basic_node values.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/float_number_type/
    using float_number_type = FloatNumberType;

    /// @brief A type for string basic_node values.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/string_type/
    using string_type = StringType;

    /// @brief A helper alias to determine converter type for the given target native data type.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/value_converter_type/
    template <typename T, typename SFINAE>
    using value_converter_type = ConverterType<T, SFINAE>;

    /// @brief Definition of node value types.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/node_t/
    using node_t = detail::node_t;

    /// @brief Definition of YAML version types.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/yaml_version_t/
    using yaml_version_t = detail::yaml_version_t;

private:
    template <node_t>
    friend struct fkyaml::detail::external_node_constructor;

    /// @brief A type for YAML docs deserializers.
    using deserializer_type = detail::basic_deserializer<basic_node>;
    /// @brief A type for YAML docs serializers.
    using serializer_type = detail::basic_serializer<basic_node>;
    /// @brief A helper type alias for std::initializer_list.
    using initializer_list_t = std::initializer_list<detail::node_ref_storage<basic_node>>;

    /// @brief The actual storage for a YAML node value of the @ref basic_node class.
    /// @details This union combines the different sotrage types for the YAML value types defined in @ref node_t.
    /// @note Container types are stored as pointers so that the size of this union will not exceed 64 bits by
    /// default.
    union node_value
    {
        /// @brief Constructs a new basic_node Value object for null types.
        node_value() = default;

        /// @brief Constructs a new basic_node value object with a node type. The default value for the specified
        /// type will be assigned.
        /// @param[in] type A node type.
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
                p_string = create_object<string_type>();
                break;
            }
        }

        /// @brief Destroys the existing Node value. This process is recursive if the specified node type is for
        /// containers.
        /// @param[in] type A Node type to determine the value to be destroyed.
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

        /// A pointer to the value of sequence type.
        sequence_type* p_sequence;
        /// A pointer to the value of mapping type. This pointer is also used when node type is null.
        mapping_type* p_mapping {nullptr};
        /// A value of boolean type.
        boolean_type boolean;
        /// A value of integer type.
        integer_type integer;
        /// A value of float number type.
        float_number_type float_val;
        /// A pointer to the value of string type.
        string_type* p_string;
    };

private:
    /// @brief Allocates and constructs an object with a given type and arguments.
    /// @tparam ObjType The target object type.
    /// @tparam ArgTypes The packed argument types for constructor arguments.
    /// @param[in] args A parameter pack for constructor arguments of the target object type.
    /// @return ObjType* An address of allocated memory on the heap.
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

    /// @brief Destroys and deallocates an object with specified type.
    /// @warning Make sure the `obj` parameter is not nullptr before calling this function.
    /// @tparam ObjType The target object type.
    /// @param[in] obj A pointer to the target object to be destroyed.
    template <typename ObjType>
    static void destroy_object(ObjType* obj)
    {
        FK_YAML_ASSERT(obj != nullptr);
        std::allocator<ObjType> alloc;
        std::allocator_traits<decltype(alloc)>::destroy(alloc, obj);
        std::allocator_traits<decltype(alloc)>::deallocate(alloc, obj, 1);
    }

public:
    /// @brief Constructs a new basic_node object of null type.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    basic_node() = default;

    /// @brief Constructs a new basic_node object with a specified type.
    /// @param[in] type A YAML node type.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    explicit basic_node(const node_t type)
        : m_node_type(type),
          m_node_value(type)
    {
    }

    /// @brief Copy constructor of the basic_node class.
    /// @param[in] rhs A basic_node object to be copied with.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    basic_node(const basic_node& rhs)
        : m_node_type(rhs.m_node_type),
          m_yaml_version_type(rhs.m_yaml_version_type),
          m_prop(rhs.m_prop)
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
        }
    }

    /// @brief Move constructor of the basic_node class.
    /// @param[in] rhs A basic_node object to be moved from.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    basic_node(basic_node&& rhs) noexcept
        : m_node_type(rhs.m_node_type),
          m_yaml_version_type(rhs.m_yaml_version_type),
          m_prop(std::move(rhs.m_prop))
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
        }

        rhs.m_node_type = node_t::NULL_OBJECT;
        rhs.m_yaml_version_type = yaml_version_t::VER_1_2;
        rhs.m_node_value.p_mapping = nullptr;
        rhs.m_prop.anchor_status = detail::anchor_status_t::NONE;
    }

    /// @brief Construct a new basic_node object from a value of compatible types.
    /// @tparam CompatibleType Type of native data which is compatible with node values.
    /// @tparam U Type of compatible native data without cv-qualifiers and reference.
    /// @param[in] val The value of a compatible type.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    template <
        typename CompatibleType, typename U = detail::remove_cvref_t<CompatibleType>,
        detail::enable_if_t<
            detail::conjunction<
                detail::negation<detail::is_basic_node<U>>,
                detail::disjunction<detail::is_node_compatible_type<basic_node, U>>>::value,
            int> = 0>
    basic_node(CompatibleType&& val) noexcept(
        noexcept(ConverterType<U>::to_node(std::declval<basic_node&>(), std::declval<CompatibleType>())))
    {
        ConverterType<U>::to_node(*this, std::forward<CompatibleType>(val));
    }

    /// @brief Construct a new basic node object with a node_ref_storage object.
    /// @tparam NodeRefStorageType Type of basic_node with reference.
    /// @param[in] node_ref_storage A node_ref_storage template class object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    template <
        typename NodeRefStorageType,
        detail::enable_if_t<detail::is_node_ref_storage<NodeRefStorageType>::value, int> = 0>
    basic_node(const NodeRefStorageType& node_ref_storage) noexcept
        : basic_node(node_ref_storage.release())
    {
    }

    /// @brief Construct a new basic node object with std::initializer_list.
    /// @param[in] init A initializer list of basic_node objects.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    basic_node(initializer_list_t init)
    {
        bool is_mapping =
            std::all_of(init.begin(), init.end(), [](const detail::node_ref_storage<basic_node>& node_ref) {
                return node_ref->is_sequence() && node_ref->size() == 2;
            });

        if (is_mapping)
        {
            m_node_type = node_t::MAPPING;
            m_node_value.p_mapping = create_object<mapping_type>();

            for (auto& elem_ref : init)
            {
                auto elem = elem_ref.release();
                m_node_value.p_mapping->emplace(
                    std::move((*(elem.m_node_value.p_sequence))[0]), std::move((*(elem.m_node_value.p_sequence))[1]));
            }
        }
        else
        {
            m_node_type = node_t::SEQUENCE;
            m_node_value.p_sequence = create_object<sequence_type>(init.begin(), init.end());
        }
    }

    /// @brief Destroy the basic_node object and its value storage.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/destructor/
    ~basic_node() noexcept // NOLINT(bugprone-exception-escape)
    {
        m_node_value.destroy(m_node_type);
        m_node_type = node_t::NULL_OBJECT;
    }

public:
    /// @brief Deserialize an input source into a basic_node object.
    /// @tparam InputType Type of a compatible input.
    /// @param[in] input An input source in the YAML format.
    /// @return The resulting basic_node object deserialized from the input source.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/deserialize/
    template <typename InputType>
    static basic_node deserialize(InputType&& input)
    {
        return deserializer_type().deserialize(detail::input_adapter(std::forward<InputType>(input)));
    }

    /// @brief Deserialize input iterators into a basic_node object.
    /// @tparam ItrType Type of a compatible iterator.
    /// @param[in] begin An iterator to the first element of an input sequence.
    /// @param[in] end An iterator to the past-the-last element of an input sequence.
    /// @return The resulting basic_node object deserialized from the pair of iterators.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/deserialize/
    template <typename ItrType>
    static basic_node deserialize(ItrType&& begin, ItrType&& end)
    {
        return deserializer_type().deserialize(
            detail::input_adapter(std::forward<ItrType>(begin), std::forward<ItrType>(end)));
    }

    /// @brief Serialize a basic_node object into a string.
    /// @param[in] node A basic_node object to be serialized.
    /// @return The resulting string object from the serialization of the node object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/serialize/
    static std::string serialize(const basic_node& node)
    {
        return serializer_type().serialize(node);
    }

    /// @brief A factory method for sequence basic_node objects without sequence_type objects.
    /// @return A YAML sequence node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/sequence/
    static basic_node sequence()
    {
        basic_node node;
        node.m_node_type = node_t::SEQUENCE;
        node.m_node_value.p_sequence = create_object<sequence_type>();
        FK_YAML_ASSERT(node.m_node_value.p_sequence != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for sequence basic_node objects with lvalue sequence_type objects.
    /// @param[in] seq A lvalue sequence node value.
    /// @return A YAML sequence node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/sequence/
    static basic_node sequence(const sequence_type& seq)
    {
        basic_node node;
        node.m_node_type = node_t::SEQUENCE;
        node.m_node_value.p_sequence = create_object<sequence_type>(seq);
        FK_YAML_ASSERT(node.m_node_value.p_sequence != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for sequence basic_node objects with rvalue sequence_type objects.
    /// @param[in] seq A rvalue sequence node value.
    /// @return A YAML sequence node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/sequence/
    static basic_node sequence(sequence_type&& seq)
    {
        basic_node node;
        node.m_node_type = node_t::SEQUENCE;
        node.m_node_value.p_sequence = create_object<sequence_type>(std::move(seq));
        FK_YAML_ASSERT(node.m_node_value.p_sequence != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for mapping basic_node objects without mapping_type objects.
    /// @return A YAML mapping node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/mapping/
    static basic_node mapping()
    {
        basic_node node;
        node.m_node_type = node_t::MAPPING;
        node.m_node_value.p_mapping = create_object<mapping_type>();
        FK_YAML_ASSERT(node.m_node_value.p_mapping != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for mapping basic_node objects with lvalue mapping_type objects.
    /// @param[in] map A lvalue mapping node value.
    /// @return A YAML mapping node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/mapping/
    static basic_node mapping(const mapping_type& map)
    {
        basic_node node;
        node.m_node_type = node_t::MAPPING;
        node.m_node_value.p_mapping = create_object<mapping_type>(map);
        FK_YAML_ASSERT(node.m_node_value.p_mapping != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for mapping basic_node objects with rvalue mapping_type objects.
    /// @param[in] map A rvalue mapping node value.
    /// @return A YAML mapping node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/mapping/
    static basic_node mapping(mapping_type&& map)
    {
        basic_node node;
        node.m_node_type = node_t::MAPPING;
        node.m_node_value.p_mapping = create_object<mapping_type>(std::move(map));
        FK_YAML_ASSERT(node.m_node_value.p_mapping != nullptr);
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for alias basic_node objects referencing the given anchor basic_node object.
    /// @note The given anchor basic_node must have a non-empty anchor name.
    /// @param[in] anchor_node A basic_node object with an anchor name.
    /// @return An alias YAML node created from the given anchor node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/alias_of/
    static basic_node alias_of(const basic_node& anchor_node)
    {
        if (!anchor_node.has_anchor_name() || anchor_node.m_prop.anchor_status != detail::anchor_status_t::ANCHOR)
        {
            throw fkyaml::exception("Cannot create an alias without anchor name.");
        }

        basic_node node = anchor_node;
        node.m_prop.anchor_status = detail::anchor_status_t::ALIAS;
        node.m_prop.anchor = anchor_node.m_prop.anchor;
        return node;
    } // LCOV_EXCL_LINE

public:
    /// @brief A copy assignment operator of the basic_node class.
    /// @param[in] rhs A lvalue basic_node object to be copied with.
    /// @return Reference to this basic_node object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator=/
    basic_node& operator=(const basic_node& rhs) noexcept
    {
        basic_node(rhs).swap(*this);
        return *this;
    }

    /// @brief A move assignment operator of the basic_node class.
    /// @param[in] rhs A rvalue basic_node object to be moved from.
    /// @return Reference to this basic_node object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator=/
    basic_node& operator=(basic_node&& rhs) noexcept
    {
        basic_node(std::move(rhs)).swap(*this);
        return *this;
    }

    /// @brief A subscript operator of the basic_node class with a key of a compatible type with basic_node.
    /// @tparam KeyType A key type compatible with basic_node
    /// @param key A key to the target value in a sequence/mapping node.
    /// @return The value associated with the given key, or a default basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator[]/
    template <
        typename KeyType, detail::enable_if_t<
                              detail::conjunction<
                                  detail::negation<detail::is_basic_node<KeyType>>,
                                  detail::is_node_compatible_type<basic_node, KeyType>>::value,
                              int> = 0>
    basic_node& operator[](KeyType&& key)
    {
        if (is_scalar())
        {
            throw fkyaml::type_error("operator[] is unavailable for a scalar node.", m_node_type);
        }

        basic_node n = std::forward<KeyType>(key);

        if (is_sequence())
        {
            if (!n.is_integer())
            {
                throw fkyaml::type_error(
                    "An argument of operator[] for sequence nodes must be an integer.", m_node_type);
            }
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return m_node_value.p_sequence->operator[](n.get_value<int>());
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        return m_node_value.p_mapping->operator[](std::move(n));
    }

    /// @brief A subscript operator of the basic_node class with a key of a compatible type with basic_node.
    /// @tparam KeyType A key type compatible with basic_node
    /// @param key A key to the target value in a sequence/mapping node.
    /// @return The value associated with the given key, or a default basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator[]/
    template <
        typename KeyType, detail::enable_if_t<
                              detail::conjunction<
                                  detail::negation<detail::is_basic_node<KeyType>>,
                                  detail::is_node_compatible_type<basic_node, KeyType>>::value,
                              int> = 0>
    const basic_node& operator[](KeyType&& key) const
    {
        if (is_scalar())
        {
            throw fkyaml::type_error("operator[] is unavailable for a scalar node.", m_node_type);
        }

        basic_node node_key = std::forward<KeyType>(key);

        if (is_sequence())
        {
            if (!node_key.is_integer())
            {
                throw fkyaml::type_error(
                    "An argument of operator[] for sequence nodes must be an integer.", m_node_type);
            }
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return m_node_value.p_sequence->operator[](node_key.get_value<int>());
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        return m_node_value.p_mapping->operator[](std::move(node_key));
    }

    /// @brief A subscript operator of the basic_node class with a basic_node key object.
    /// @tparam KeyType A key type which is a kind of the basic_node template class.
    /// @param key A key to the target value in a sequence/mapping node.
    /// @return The value associated with the given key, or a default basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator[]/
    template <
        typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
    basic_node& operator[](KeyType&& key)
    {
        if (is_scalar())
        {
            throw fkyaml::type_error("operator[] is unavailable for a scalar node.", m_node_type);
        }

        if (is_sequence())
        {
            if (!key.is_integer())
            {
                throw fkyaml::type_error(
                    "An argument of operator[] for sequence nodes must be an integer.", m_node_type);
            }
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return m_node_value.p_sequence->operator[](key.template get_value<int>());
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        return m_node_value.p_mapping->operator[](std::forward<KeyType>(key));
    }

    /// @brief A subscript operator of the basic_node class with a basic_node key object.
    /// @tparam KeyType A key type which is a kind of the basic_node template class.
    /// @param key A key to the target value in a sequence/mapping node.
    /// @return The value associated with the given key, or a default basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator[]/
    template <
        typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
    const basic_node& operator[](KeyType&& key) const
    {
        if (is_scalar())
        {
            throw fkyaml::type_error("operator[] is unavailable for a scalar node.", m_node_type);
        }

        if (is_sequence())
        {
            if (!key.is_integer())
            {
                throw fkyaml::type_error(
                    "An argument of operator[] for sequence nodes must be an integer.", m_node_type);
            }
            FK_YAML_ASSERT(m_node_value.p_sequence != nullptr);
            return m_node_value.p_sequence->operator[](key.template get_value<int>());
        }

        FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
        return m_node_value.p_mapping->operator[](std::forward<KeyType>(key));
    }

    /// @brief An equal-to operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true if both types and values are equal, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_eq/
    bool operator==(const basic_node& rhs) const noexcept
    {
        if (m_node_type != rhs.m_node_type)
        {
            return false;
        }

        bool ret = false;
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            ret = (*(m_node_value.p_sequence) == *(rhs.m_node_value.p_sequence));
            break;
        case node_t::MAPPING:
            ret = (*(m_node_value.p_mapping) == *(rhs.m_node_value.p_mapping));
            break;
        case node_t::NULL_OBJECT:
            // Always true for comparisons between null nodes.
            ret = true;
            break;
        case node_t::BOOLEAN:
            ret = (m_node_value.boolean == rhs.m_node_value.boolean);
            break;
        case node_t::INTEGER:
            ret = (m_node_value.integer == rhs.m_node_value.integer);
            break;
        case node_t::FLOAT_NUMBER:
            ret =
                (std::abs(m_node_value.float_val - rhs.m_node_value.float_val) <
                 std::numeric_limits<float_number_type>::epsilon());
            break;
        case node_t::STRING:
            ret = (*(m_node_value.p_string) == *(rhs.m_node_value.p_string));
            break;
        }

        return ret;
    }

    /// @brief A not-equal-to operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true if either types or values are different, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_ne/
    bool operator!=(const basic_node& rhs) const noexcept
    {
        return !operator==(rhs);
    }

    /// @brief A less-than operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true this basic_node object is less than `rhs`.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_lt/
    bool operator<(const basic_node& rhs) const noexcept
    {
        if (operator==(rhs))
        {
            return false;
        }

        if (uint32_t(m_node_type) < uint32_t(rhs.m_node_type))
        {
            return true;
        }

        if (m_node_type != rhs.m_node_type)
        {
            return false;
        }

        bool ret = false;
        switch (m_node_type)
        {
        case node_t::SEQUENCE:
            ret = (*(m_node_value.p_sequence) < *(rhs.m_node_value.p_sequence));
            break;
        case node_t::MAPPING:
            ret = (*(m_node_value.p_mapping) < *(rhs.m_node_value.p_mapping));
            break;
        case node_t::NULL_OBJECT: // LCOV_EXCL_LINE
            // Will not come here since null nodes are alyways the same.
            break; // LCOV_EXCL_LINE
        case node_t::BOOLEAN:
            // false < true
            ret = (!m_node_value.boolean && rhs.m_node_value.boolean);
            break;
        case node_t::INTEGER:
            ret = (m_node_value.integer < rhs.m_node_value.integer);
            break;
        case node_t::FLOAT_NUMBER:
            ret = (m_node_value.float_val < rhs.m_node_value.float_val);
            break;
        case node_t::STRING:
            ret = (*(m_node_value.p_string) < *(rhs.m_node_value.p_string));
            break;
        }

        return ret;
    }

    /// @brief A less-than-or-equal-to operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true this basic_node object is less than or equal to `rhs`.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_le/
    bool operator<=(const basic_node& rhs) const noexcept
    {
        return !rhs.operator<(*this);
    }

    /// @brief A greater-than operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true this basic_node object is greater than `rhs`.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_gt/
    bool operator>(const basic_node& rhs) const noexcept
    {
        return !operator<=(rhs);
    }

    /// @brief A greater-than-or-equal-to operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true this basic_node object is greater than or equal to `rhs`.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_ge/
    bool operator>=(const basic_node& rhs) const noexcept
    {
        return !operator<(rhs);
    }

public:
    /// @brief Returns the type of the current basic_node value.
    /// @return The type of the YAML node value.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/type/
    node_t type() const noexcept
    {
        return m_node_type;
    }

    /// @brief Tests whether the current basic_node value is of sequence type.
    /// @return true if the type is sequence, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_sequence/
    bool is_sequence() const noexcept
    {
        return m_node_type == node_t::SEQUENCE;
    }

    /// @brief Tests whether the current basic_node value is of mapping type.
    /// @return true if the type is mapping, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_mapping/
    bool is_mapping() const noexcept
    {
        return m_node_type == node_t::MAPPING;
    }

    /// @brief Tests whether the current basic_node value is of null type.
    /// @return true if the type is null, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_null/
    bool is_null() const noexcept
    {
        return m_node_type == node_t::NULL_OBJECT;
    }

    /// @brief Tests whether the current basic_node value is of boolean type.
    /// @return true if the type is boolean, false otherwise
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_boolean/
    bool is_boolean() const noexcept
    {
        return m_node_type == node_t::BOOLEAN;
    }

    /// @brief Tests whether the current basic_node value is of integer type.
    /// @return true if the type is integer, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_integer/
    bool is_integer() const noexcept
    {
        return m_node_type == node_t::INTEGER;
    }

    /// @brief Tests whether the current basic_node value is of float number type.
    /// @return true if the type is floating point number, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_float_number/
    bool is_float_number() const noexcept
    {
        return m_node_type == node_t::FLOAT_NUMBER;
    }

    /// @brief Tests whether the current basic_node value is of string type.
    /// @return true if the type is string, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_string/
    bool is_string() const noexcept
    {
        return m_node_type == node_t::STRING;
    }

    /// @brief Tests whether the current basic_node value is of scalar types.
    /// @return true if the type is scalar, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_scalar/
    bool is_scalar() const noexcept
    {
        return !is_sequence() && !is_mapping();
    }

    /// @brief Tests whether the current basic_node is an anchor node.
    /// @return true if the current basic_node is an anchor node, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_anchor/
    bool is_anchor() const noexcept
    {
        return m_prop.anchor_status == detail::anchor_status_t::ANCHOR;
    }

    /// @brief Tests whether the current basic_node is an alias node.
    /// @return true if the current basic_node is an alias node, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_alias/
    bool is_alias() const noexcept
    {
        return m_prop.anchor_status == detail::anchor_status_t::ALIAS;
    }

    /// @brief Tests whether the current basic_node value (sequence, mapping, string) is empty.
    /// @return true if the node value is empty, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/empty/
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
            throw fkyaml::type_error("The target node is not of a container type.", m_node_type);
        }
    }

    /// @brief Returns the size of the current basic_node value (sequence, mapping, string).
    /// @return The size of a node value.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/size/
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
            throw fkyaml::type_error("The target node is not of a container type.", m_node_type);
        }
    }

    /// @brief Check whether or not this basic_node object has a given key in its inner mapping node value.
    /// @tparam KeyType A key type compatible with basic_node.
    /// @param key A key to the target value in the mapping node value.
    /// @return true if the target node is a mapping and has the given key, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/contains/
    template <
        typename KeyType, detail::enable_if_t<
                              detail::conjunction<
                                  detail::negation<detail::is_basic_node<detail::remove_cvref_t<KeyType>>>,
                                  detail::is_node_compatible_type<basic_node, detail::remove_cvref_t<KeyType>>>::value,
                              int> = 0>
    bool contains(KeyType&& key) const
    {
        switch (m_node_type)
        {
        case node_t::MAPPING: {
            FK_YAML_ASSERT(m_node_value.p_mapping != nullptr);
            mapping_type& map = *m_node_value.p_mapping;
            basic_node node_key = std::forward<KeyType>(key);
            return map.find(node_key) != map.end();
        }
        default:
            return false;
        }
    }

    /// @brief Check whether or not this basic_node object has a given key in its inner mapping Node value.
    /// @tparam KeyType A key type which is a kind of basic_node template class.
    /// @param[in] key A key to the target value in the YAML mapping node value.
    /// @return true if the YAML node is a mapping and has the given key, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/contains/
    template <
        typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
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

    /// @brief Get the YAML version specification for this basic_node object.
    /// @return The version of the YAML format applied to the basic_node object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_yaml_version/
    yaml_version_t get_yaml_version() const noexcept
    {
        return m_yaml_version_type;
    }

    /// @brief Set the YAML version specification for this basic_node object.
    /// @param[in] A version of the YAML format.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/set_yaml_version/
    void set_yaml_version(const yaml_version_t version) noexcept
    {
        m_yaml_version_type = version;
    }

    /// @brief Check whether or not this basic_node object has already had any anchor name.
    /// @return true if ths basic_node has an anchor name, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/has_anchor_name/
    bool has_anchor_name() const noexcept
    {
        return m_prop.anchor_status != detail::anchor_status_t::NONE && !m_prop.anchor.empty();
    }

    /// @brief Get the anchor name associated to this basic_node object.
    /// @note Some anchor name must be set before calling this method. Call basic_node::HasAnchorName() to see if this
    /// basic_node object has any anchor name.
    /// @return The anchor name associated to the node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_anchor_name/
    const std::string& get_anchor_name() const
    {
        if (!has_anchor_name())
        {
            throw fkyaml::exception("No anchor name has been set.");
        }
        return m_prop.anchor;
    }

    /// @brief Add an anchor name to this basic_node object.
    /// @note If this basic_node object has already had any anchor name, the new anchor name will overwrite the old one.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/add_anchor_name/
    void add_anchor_name(const std::string& anchor_name)
    {
        m_prop.anchor_status = detail::anchor_status_t::ANCHOR;
        m_prop.anchor = anchor_name;
    }

    /// @brief Add an anchor name to this basic_node object.
    /// @note If this basic_node object has already had any anchor name, the new anchor name will overwrite the old one.
    /// @param[in] anchor_name An anchor name.This should not be empty.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/add_anchor_name/
    void add_anchor_name(std::string&& anchor_name)
    {
        m_prop.anchor_status = detail::anchor_status_t::ANCHOR;
        m_prop.anchor = std::move(anchor_name);
    }

    /// @brief Get the node value object converted into a given type.
    /// @note This function requires T objects to be default constructible.
    /// @tparam T A compatible value type which might be cv-qualified or a reference type.
    /// @tparam ValueType A compatible value type, without cv-qualifiers and reference by default.
    /// @return A compatible native data value converted from the basic_node object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_value/
    template <
        typename T, typename ValueType = detail::remove_cvref_t<T>,
        detail::enable_if_t<
            detail::conjunction<
                std::is_default_constructible<ValueType>, detail::has_from_node<basic_node, ValueType>>::value,
            int> = 0>
    T get_value() const noexcept(
        noexcept(ConverterType<ValueType>::from_node(std::declval<const basic_node&>(), std::declval<ValueType&>())))
    {
        auto ret = ValueType();
        ConverterType<ValueType>::from_node(*this, ret);
        return ret;
    }

    /// @brief Explicit reference access to the internally stored YAML node value.
    /// @tparam ReferenceType Reference type to the target YAML node value.
    /// @return Reference to the internally stored YAML node value.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_value_ref/
    template <typename ReferenceType, detail::enable_if_t<std::is_reference<ReferenceType>::value, int> = 0>
    ReferenceType get_value_ref()
    {
        return get_value_ref_impl(static_cast<detail::add_pointer_t<ReferenceType>>(nullptr));
    }

    /// @brief Explicit reference access to the internally stored YAML node value.
    /// @tparam ReferenceType Constant reference type to the target YAML node value.
    /// @return Constant reference to the internally stored YAML node value.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_value_ref/
    template <
        typename ReferenceType,
        detail::enable_if_t<
            detail::conjunction<
                std::is_reference<ReferenceType>, std::is_const<detail::remove_reference_t<ReferenceType>>>::value,
            int> = 0>
    ReferenceType get_value_ref() const
    {
        return get_value_ref_impl(static_cast<detail::add_pointer_t<ReferenceType>>(nullptr));
    }

    /// @brief Swaps the internally stored data with the specified basic_node object.
    /// @param[in] rhs A basic_node object to be swapped with.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/swap/
    void swap(basic_node& rhs) noexcept
    {
        using std::swap;
        swap(m_node_type, rhs.m_node_type);
        swap(m_yaml_version_type, rhs.m_yaml_version_type);

        node_value tmp {};
        std::memcpy(&tmp, &m_node_value, sizeof(node_value));
        std::memcpy(&m_node_value, &rhs.m_node_value, sizeof(node_value));
        std::memcpy(&rhs.m_node_value, &tmp, sizeof(node_value));

        swap(m_prop.tag, rhs.m_prop.tag);
        swap(m_prop.anchor_status, rhs.m_prop.anchor_status);
        swap(m_prop.anchor, rhs.m_prop.anchor);
    }

    /// @brief Returns the first iterator of basic_node values of container types (sequence or mapping) from a non-const
    /// basic_node object. Throws exception if the basic_node value is not of container types.
    /// @return An iterator to the first element of a YAML node value (either sequence or mapping).
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/begin/
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
            throw fkyaml::type_error("The target node is neither of sequence nor mapping types.", m_node_type);
        }
    }

    /// @brief Returns the first iterator of basic_node values of container types (sequence or mapping) from a const
    /// basic_node object. Throws exception if the basic_node value is not of container types.
    /// @return A constant iterator to the first element of a YAML node value (either sequence or mapping).
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/begin/
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
            throw fkyaml::type_error("The target node is neither of sequence nor mapping types.", m_node_type);
        }
    }

    /// @brief Returns the last iterator of basic_node values of container types (sequence or mapping) from a non-const
    /// basic_node object. Throws exception if the basic_node value is not of container types.
    /// @return An iterator to the past-the end element of a YAML node value (either sequence or mapping).
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/end/
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
            throw fkyaml::type_error("The target node is neither of sequence nor mapping types.", m_node_type);
        }
    }

    /// @brief Returns the last iterator of basic_node values of container types (sequence or mapping) from a const
    /// basic_node object. Throws exception if the basic_node value is not of container types.
    /// @return A constant iterator to the past-the end element of a YAML node value (either sequence or mapping).
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/end/
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
            throw fkyaml::type_error("The target node is neither of sequence nor mapping types.", m_node_type);
        }
    }

private:
    /// @brief Returns reference to the sequence node value.
    /// @throw fkyaml::exception The node value is not a sequence.
    /// @return Reference to the sequence node value.
    sequence_type& get_value_ref_impl(sequence_type* /*unused*/)
    {
        if (!is_sequence())
        {
            throw fkyaml::type_error("The node value is not a sequence.", m_node_type);
        }
        return *(m_node_value.p_sequence);
    }

    /// @brief Returns constant reference to the sequence node value.
    /// @throw fkyaml::exception The node value is not a sequence.
    /// @return Constant reference to the sequence node value.
    const sequence_type& get_value_ref_impl(const sequence_type* /*unused*/) const
    {
        if (!is_sequence())
        {
            throw fkyaml::type_error("The node value is not a sequence.", m_node_type);
        }
        return *(m_node_value.p_sequence);
    }

    /// @brief Returns reference to the mapping node value.
    /// @throw fkyaml::exception The node value is not a mapping.
    /// @return Reference to the mapping node value.
    mapping_type& get_value_ref_impl(mapping_type* /*unused*/)
    {
        if (!is_mapping())
        {
            throw fkyaml::type_error("The node value is not a mapping.", m_node_type);
        }
        return *(m_node_value.p_mapping);
    }

    /// @brief Returns constant reference to the mapping node value.
    /// @throw fkyaml::exception The node value is not a mapping.
    /// @return Constant reference to the mapping node value.
    const mapping_type& get_value_ref_impl(const mapping_type* /*unused*/) const
    {
        if (!is_mapping())
        {
            throw fkyaml::type_error("The node value is not a mapping.", m_node_type);
        }
        return *(m_node_value.p_mapping);
    }

    /// @brief Returns reference to the boolean node value.
    /// @throw fkyaml::exception The node value is not a boolean.
    /// @return Reference to the boolean node value.
    boolean_type& get_value_ref_impl(boolean_type* /*unused*/)
    {
        if (!is_boolean())
        {
            throw fkyaml::type_error("The node value is not a boolean.", m_node_type);
        }
        return m_node_value.boolean;
    }

    /// @brief Returns reference to the boolean node value.
    /// @throw fkyaml::exception The node value is not a boolean.
    /// @return Constant reference to the boolean node value.
    const boolean_type& get_value_ref_impl(const boolean_type* /*unused*/) const
    {
        if (!is_boolean())
        {
            throw fkyaml::type_error("The node value is not a boolean.", m_node_type);
        }
        return m_node_value.boolean;
    }

    /// @brief Returns reference to the integer node value.
    /// @throw fkyaml::exception The node value is not an integer.
    /// @return Reference to the integer node value.
    integer_type& get_value_ref_impl(integer_type* /*unused*/)
    {
        if (!is_integer())
        {
            throw fkyaml::type_error("The node value is not an integer.", m_node_type);
        }
        return m_node_value.integer;
    }

    /// @brief Returns reference to the integer node value.
    /// @throw fkyaml::exception The node value is not an integer.
    /// @return Constant reference to the integer node value.
    const integer_type& get_value_ref_impl(const integer_type* /*unused*/) const
    {
        if (!is_integer())
        {
            throw fkyaml::type_error("The node value is not an integer.", m_node_type);
        }
        return m_node_value.integer;
    }

    /// @brief Returns reference to the floating point number node value.
    /// @throw fkyaml::exception The node value is not a floating point number.
    /// @return Reference to the floating point number node value.
    float_number_type& get_value_ref_impl(float_number_type* /*unused*/)
    {
        if (!is_float_number())
        {
            throw fkyaml::type_error("The node value is not a floating point number.", m_node_type);
        }
        return m_node_value.float_val;
    }

    /// @brief Returns reference to the floating point number node value.
    /// @throw fkyaml::exception The node value is not a floating point number.
    /// @return Constant reference to the floating point number node value.
    const float_number_type& get_value_ref_impl(const float_number_type* /*unused*/) const
    {
        if (!is_float_number())
        {
            throw fkyaml::type_error("The node value is not a floating point number.", m_node_type);
        }
        return m_node_value.float_val;
    }

    /// @brief Returns reference to the string node value.
    /// @throw fkyaml::exception The node value is not a string.
    /// @return Reference to the string node value.
    string_type& get_value_ref_impl(string_type* /*unused*/)
    {
        if (!is_string())
        {
            throw fkyaml::type_error("The node value is not a string.", m_node_type);
        }
        return *(m_node_value.p_string);
    }

    /// @brief Returns reference to the string node value.
    /// @throw fkyaml::exception The node value is not a string.
    /// @return Constant reference to the string node value.
    const string_type& get_value_ref_impl(const string_type* /*unused*/) const
    {
        if (!is_string())
        {
            throw fkyaml::type_error("The node value is not a string.", m_node_type);
        }
        return *(m_node_value.p_string);
    }

    /// The current node value type.
    node_t m_node_type {node_t::NULL_OBJECT};
    /// The YAML version specification.
    yaml_version_t m_yaml_version_type {yaml_version_t::VER_1_2};
    /// The current node value.
    node_value m_node_value {};
    /// The property set of this node.
    detail::node_property m_prop {};
};

/// @brief Swap function for basic_node objects.
/// @param[in] lhs A left-side-hand basic_node object to be swapped with.
/// @param[in] rhs A right-side-hand basic_node object to be swapped with.
/// @sa https://fktn-k.github.io/fkYAML/api/swap/
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType,
    template <typename, typename = void> class ConverterType>
inline void swap(
    basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>& lhs,
    basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>&
        rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

/// @brief Insertion operator for basic_node template class. A wrapper for the serialization feature.
/// @param[in] os An output stream object.
/// @param[in] n A basic_node object.
/// @return Reference to the output stream object `os`.
/// @sa https://fktn-k.github.io/fkYAML/api/basic_node/insertion_operator/
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType,
    template <typename, typename = void> class ConverterType>
inline std::ostream& operator<<(
    std::ostream& os,
    const basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>&
        n)
{
    os << basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>::
            serialize(n);
    return os;
}

/// @brief Extraction operator for basic_node template class. A wrapper for the deserialization feature with input
/// streams.
/// @param[in] is An input stream object.
/// @param[in] n A basic_node object.
/// @return Reference to the input stream object `is`.
/// @sa https://fktn-k.github.io/fkYAML/api/basic_node/extraction_operator/
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType,
    template <typename, typename = void> class ConverterType>
inline std::istream& operator>>(
    std::istream& is,
    basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>& n)
{
    n = basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>::
        deserialize(is);
    return is;
}

/// @brief default YAML node value container.
/// @sa https://fktn-k.github.io/fkYAML/api/basic_node/node/
using node = basic_node<>;

/// @brief namespace for user-defined literals for the fkYAML library.
inline namespace literals
{
/// @brief namespace for user-defined literals for YAML node objects.
inline namespace yaml_literals
{

/// @brief The user-defined string literal which deserializes a `char` array into a `node` object.
/// @param s An input `char` array.
/// @param n The size of `s`.
/// @return The resulting `node` object deserialized from `s`.
/// @sa https://fktn-k.github.io/fkYAML/api/operator_literal_yaml/
inline fkyaml::node operator"" _yaml(const char* s, std::size_t n)
{
    return fkyaml::node::deserialize((const char*)s, (const char*)s + n);
}

/// @brief The user-defined string literal which deserializes a `char16_t` array into a `node` object.
/// @param s An input `char16_t` array.
/// @param n The size of `s`.
/// @return The resulting `node` object deserialized from `s`.
/// @sa https://fktn-k.github.io/fkYAML/api/operator_literal_yaml/
inline fkyaml::node operator"" _yaml(const char16_t* s, std::size_t n)
{
    return fkyaml::node::deserialize((const char16_t*)s, (const char16_t*)s + n);
}

/// @brief The user-defined string literal which deserializes a `char32_t` array into a `node` object.
/// @param s An input `char32_t` array.
/// @param n The size of `s`.
/// @return The resulting `node` object deserialized from `s`.
/// @sa https://fktn-k.github.io/fkYAML/api/operator_literal_yaml/
inline fkyaml::node operator"" _yaml(const char32_t* s, std::size_t n)
{
    return fkyaml::node::deserialize((const char32_t*)s, (const char32_t*)s + n);
}

#ifdef FK_YAML_HAS_CHAR8_T
/// @brief The user-defined string literal which deserializes a `char8_t` array into a `node` object.
/// @param s An input `char8_t` array.
/// @param n The size of `s`.
/// @return The resulting `node` object deserialized from `s`.
inline fkyaml::node operator"" _yaml(const char8_t* s, std::size_t n)
{
    return fkyaml::node::deserialize((const char8_t*)s, (const char8_t*)s + n);
}
#endif

} // namespace yaml_literals
} // namespace literals

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_NODE_HPP_ */
