//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.13
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_NODE_HPP
#define FK_YAML_NODE_HPP

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
#include <fkYAML/detail/document_metainfo.hpp>
#include <fkYAML/detail/input/deserializer.hpp>
#include <fkYAML/detail/input/input_adapter.hpp>
#include <fkYAML/detail/iterator.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/meta/type_traits.hpp>
#include <fkYAML/detail/node_attrs.hpp>
#include <fkYAML/detail/node_property.hpp>
#include <fkYAML/detail/node_ref_storage.hpp>
#include <fkYAML/detail/output/serializer.hpp>
#include <fkYAML/detail/types/node_t.hpp>
#include <fkYAML/detail/types/yaml_version_t.hpp>
#include <fkYAML/exception.hpp>
#include <fkYAML/node_type.hpp>
#include <fkYAML/node_value_converter.hpp>
#include <fkYAML/ordered_map.hpp>

FK_YAML_NAMESPACE_BEGIN

/// @brief A class to store value of YAML nodes.
/// @sa https://fktn-k.github.io/fkYAML/api/basic_node/
template <
    template <typename, typename...> class SequenceType = std::vector,
    template <typename, typename, typename...> class MappingType = std::map, typename BooleanType = bool,
    typename IntegerType = std::int64_t, typename FloatNumberType = double, typename StringType = std::string,
    template <typename, typename = void> class ConverterType = node_value_converter>
class basic_node {
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
    /// @deprecated Use fkyaml::node_type enum class. (since 0.3.12)
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/node_t/
    using node_t = detail::node_t;

    /// @brief Definition of YAML version types.
    /// @deprecated Use fkyaml::yaml_version_type enum class. (since 0.3.12)
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/yaml_version_t/
    using yaml_version_t = detail::yaml_version_t;

private:
    template <node_type>
    friend struct fkyaml::detail::external_node_constructor;

    template <typename BasicNodeType>
    friend class fkyaml::detail::basic_deserializer;

    template <typename BasicNodeType>
    friend class fkyaml::detail::basic_serializer;

    /// @brief A type for YAML docs deserializers.
    using deserializer_type = detail::basic_deserializer<basic_node>;
    /// @brief A type for YAML docs serializers.
    using serializer_type = detail::basic_serializer<basic_node>;
    /// @brief A helper type alias for std::initializer_list.
    using initializer_list_t = std::initializer_list<detail::node_ref_storage<basic_node>>;

    /// @brief The actual storage for a YAML node value of the @ref basic_node class.
    /// @details This union combines the different storage types for the YAML value types defined in @ref node_t.
    /// @note Container types are stored as pointers so that the size of this union will not exceed 64 bits by
    /// default.
    union node_value {
        /// @brief Constructs a new basic_node Value object for null types.
        node_value() = default;

        /// @brief Constructs a new basic_node value object with a node type. The default value for the specified
        /// type will be assigned.
        /// @param[in] type A node type.
        explicit node_value(detail::node_attr_t value_type_bit) {
            switch (value_type_bit) {
            case detail::node_attr_bits::seq_bit:
                p_sequence = create_object<sequence_type>();
                break;
            case detail::node_attr_bits::map_bit:
                p_mapping = create_object<mapping_type>();
                break;
            case detail::node_attr_bits::null_bit:
                p_mapping = nullptr;
                break;
            case detail::node_attr_bits::bool_bit:
                boolean = static_cast<boolean_type>(false);
                break;
            case detail::node_attr_bits::int_bit:
                integer = static_cast<integer_type>(0);
                break;
            case detail::node_attr_bits::float_bit:
                float_val = static_cast<float_number_type>(0.0);
                break;
            case detail::node_attr_bits::string_bit:
                p_string = create_object<string_type>();
                break;
            default:   // LCOV_EXCL_LINE
                break; // LCOV_EXCL_LINE
            }
        }

        /// @brief Destroys the existing Node value. This process is recursive if the specified node type is for
        /// containers.
        /// @param[in] type A Node type to determine the value to be destroyed.
        void destroy(detail::node_attr_t value_type_bit) {
            switch (value_type_bit) {
            case detail::node_attr_bits::seq_bit:
                p_sequence->clear();
                destroy_object<sequence_type>(p_sequence);
                p_sequence = nullptr;
                break;
            case detail::node_attr_bits::map_bit:
                p_mapping->clear();
                destroy_object<mapping_type>(p_mapping);
                p_mapping = nullptr;
                break;
            case detail::node_attr_bits::string_bit:
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
    static ObjType* create_object(ArgTypes&&... args) {
        using AllocType = std::allocator<ObjType>;
        using AllocTraitsType = std::allocator_traits<AllocType>;

        AllocType alloc {};
        auto deleter = [&alloc](ObjType* obj) {
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
    static void destroy_object(ObjType* obj) {
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
    FK_YAML_DEPRECATED("Since 0.3.12; Use explicit basic_node(const node_type)")
    explicit basic_node(const node_t type)
        : basic_node(detail::convert_to_node_type(type)) {
    }

    explicit basic_node(const node_type type)
        : m_attrs(detail::node_attr_bits::from_node_type(type)),
          m_node_value(m_attrs & detail::node_attr_mask::value) {
    }

    /// @brief Copy constructor of the basic_node class.
    /// @param[in] rhs A basic_node object to be copied with.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    basic_node(const basic_node& rhs)
        : m_attrs(rhs.m_attrs),
          mp_meta(rhs.mp_meta),
          m_prop(rhs.m_prop) {
        if FK_YAML_LIKELY (!has_anchor_name()) {
            switch (m_attrs & detail::node_attr_mask::value) {
            case detail::node_attr_bits::seq_bit:
                m_node_value.p_sequence = create_object<sequence_type>(*(rhs.m_node_value.p_sequence));
                break;
            case detail::node_attr_bits::map_bit:
                m_node_value.p_mapping = create_object<mapping_type>(*(rhs.m_node_value.p_mapping));
                break;
            case detail::node_attr_bits::null_bit:
                m_node_value.p_mapping = nullptr;
                break;
            case detail::node_attr_bits::bool_bit:
                m_node_value.boolean = rhs.m_node_value.boolean;
                break;
            case detail::node_attr_bits::int_bit:
                m_node_value.integer = rhs.m_node_value.integer;
                break;
            case detail::node_attr_bits::float_bit:
                m_node_value.float_val = rhs.m_node_value.float_val;
                break;
            case detail::node_attr_bits::string_bit:
                m_node_value.p_string = create_object<string_type>(*(rhs.m_node_value.p_string));
                break;
            default:   // LCOV_EXCL_LINE
                break; // LCOV_EXCL_LINE
            }
        }
    }

    /// @brief Move constructor of the basic_node class.
    /// @param[in] rhs A basic_node object to be moved from.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    basic_node(basic_node&& rhs) noexcept
        : m_attrs(rhs.m_attrs),
          mp_meta(std::move(rhs.mp_meta)),
          m_prop(std::move(rhs.m_prop)) {
        if FK_YAML_LIKELY (!has_anchor_name()) {
            switch (m_attrs & detail::node_attr_mask::value) {
            case detail::node_attr_bits::seq_bit:
                FK_YAML_ASSERT(rhs.m_node_value.p_sequence != nullptr);
                m_node_value.p_sequence = rhs.m_node_value.p_sequence;
                rhs.m_node_value.p_sequence = nullptr;
                break;
            case detail::node_attr_bits::map_bit:
                FK_YAML_ASSERT(rhs.m_node_value.p_mapping != nullptr);
                m_node_value.p_mapping = rhs.m_node_value.p_mapping;
                rhs.m_node_value.p_mapping = nullptr;
                break;
            case detail::node_attr_bits::null_bit:
                FK_YAML_ASSERT(rhs.m_node_value.p_mapping == nullptr);
                m_node_value.p_mapping = rhs.m_node_value.p_mapping;
                break;
            case detail::node_attr_bits::bool_bit:
                m_node_value.boolean = rhs.m_node_value.boolean;
                rhs.m_node_value.boolean = static_cast<boolean_type>(false);
                break;
            case detail::node_attr_bits::int_bit:
                m_node_value.integer = rhs.m_node_value.integer;
                rhs.m_node_value.integer = static_cast<integer_type>(0);
                break;
            case detail::node_attr_bits::float_bit:
                m_node_value.float_val = rhs.m_node_value.float_val;
                rhs.m_node_value.float_val = static_cast<float_number_type>(0.0);
                break;
            case detail::node_attr_bits::string_bit:
                FK_YAML_ASSERT(rhs.m_node_value.p_string != nullptr);
                m_node_value.p_string = rhs.m_node_value.p_string;
                rhs.m_node_value.p_string = nullptr;
                break;
            default:   // LCOV_EXCL_LINE
                break; // LCOV_EXCL_LINE
            }
        }

        rhs.m_attrs = detail::node_attr_bits::default_bits;
        rhs.m_node_value.p_mapping = nullptr;
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
        noexcept(ConverterType<U, void>::to_node(std::declval<basic_node&>(), std::declval<CompatibleType>()))) {
        ConverterType<U, void>::to_node(*this, std::forward<CompatibleType>(val));
    }

    /// @brief Construct a new basic node object with a node_ref_storage object.
    /// @tparam NodeRefStorageType Type of basic_node with reference.
    /// @param[in] node_ref_storage A node_ref_storage template class object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    template <
        typename NodeRefStorageType,
        detail::enable_if_t<detail::is_node_ref_storage<NodeRefStorageType>::value, int> = 0>
    basic_node(const NodeRefStorageType& node_ref_storage) noexcept
        : basic_node(node_ref_storage.release()) {
    }

    /// @brief Construct a new basic node object with std::initializer_list.
    /// @param[in] init A initializer list of basic_node objects.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/constructor/
    basic_node(initializer_list_t init) {
        bool is_mapping =
            std::all_of(init.begin(), init.end(), [](const detail::node_ref_storage<basic_node>& node_ref) {
                return node_ref->is_sequence() && node_ref->size() == 2;
            });

        if (is_mapping) {
            m_attrs = detail::node_attr_bits::map_bit;
            m_node_value.p_mapping = create_object<mapping_type>();

            for (auto& elem_ref : init) {
                auto elem = elem_ref.release();
                m_node_value.p_mapping->emplace(
                    std::move((*(elem.m_node_value.p_sequence))[0]), std::move((*(elem.m_node_value.p_sequence))[1]));
            }
        }
        else {
            m_attrs = detail::node_attr_bits::seq_bit;
            m_node_value.p_sequence = create_object<sequence_type>();
            m_node_value.p_sequence->reserve(std::distance(init.begin(), init.end()));
            for (auto& elem_ref : init) {
                m_node_value.p_sequence->emplace_back(std::move(elem_ref.release()));
            }
        }
    }

    /// @brief Destroy the basic_node object and its value storage.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/destructor/
    ~basic_node() noexcept // NOLINT(bugprone-exception-escape)
    {
        if (m_attrs & detail::node_attr_mask::anchoring) {
            if (m_attrs & detail::node_attr_bits::anchor_bit) {
                auto itr = mp_meta->anchor_table.equal_range(m_prop.anchor).first;
                std::advance(itr, detail::node_attr_bits::get_anchor_offset(m_attrs));
                itr->second.m_node_value.destroy(itr->second.m_attrs & detail::node_attr_mask::value);
                itr->second.m_attrs = detail::node_attr_bits::default_bits;
                itr->second.mp_meta.reset();
            }
        }
        else if ((m_attrs & detail::node_attr_bits::null_bit) == 0) {
            m_node_value.destroy(m_attrs & detail::node_attr_mask::value);
        }

        m_attrs = detail::node_attr_bits::default_bits;
        mp_meta.reset();
    }

public:
    /// @brief Deserialize the first YAML document in the input into a basic_node object.
    /// @tparam InputType Type of a compatible input.
    /// @param[in] input An input source in the YAML format.
    /// @return The resulting basic_node object deserialized from the input source.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/deserialize/
    template <typename InputType>
    static basic_node deserialize(InputType&& input) {
        return deserializer_type().deserialize(detail::input_adapter(std::forward<InputType>(input)));
    }

    /// @brief Deserialize the first YAML document in the input ranged by the iterators into a basic_node object.
    /// @tparam ItrType Type of a compatible iterator.
    /// @param[in] begin An iterator to the first element of an input sequence.
    /// @param[in] end An iterator to the past-the-last element of an input sequence.
    /// @return The resulting basic_node object deserialized from the pair of iterators.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/deserialize/
    template <typename ItrType>
    static basic_node deserialize(ItrType&& begin, ItrType&& end) {
        return deserializer_type().deserialize(
            detail::input_adapter(std::forward<ItrType>(begin), std::forward<ItrType>(end)));
    }

    /// @brief Deserialize all YAML documents in the input into basic_node objects.
    /// @tparam InputType Type of a compatible input.
    /// @param[in] input An input source in the YAML format.
    /// @return The resulting basic_node objects deserialized from the input.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/deserialize_docs/
    template <typename InputType>
    static std::vector<basic_node> deserialize_docs(InputType&& input) {
        return deserializer_type().deserialize_docs(detail::input_adapter(std::forward<InputType>(input)));
    }

    /// @brief Deserialize all YAML documents in the input ranged by the iterators into basic_node objects.
    /// @tparam ItrType Type of a compatible iterator.
    /// @param[in] begin An iterator to the first element of an input sequence.
    /// @param[in] end An iterator to the past-the-last element of an input sequence.
    /// @return The resulting basic_node objects deserialized from the pair of iterators.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/deserialize_docs/
    template <typename ItrType>
    static std::vector<basic_node> deserialize_docs(ItrType&& begin, ItrType&& end) {
        return deserializer_type().deserialize_docs(
            detail::input_adapter(std::forward<ItrType>(begin), std::forward<ItrType>(end)));
    }

    /// @brief Serialize a basic_node object into a string.
    /// @param[in] node A basic_node object to be serialized.
    /// @return The resulting string object from the serialization of the given node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/serialize/
    static std::string serialize(const basic_node& node) {
        return serializer_type().serialize(node);
    }

    /// @brief Serialize basic_node objects into a string.
    /// @param docs basic_node objects to be serialized.
    /// @return The resulting string object from the serialization of the given nodes.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/serialize_docs/
    static std::string serialize_docs(const std::vector<basic_node>& docs) {
        return serializer_type().serialize_docs(docs);
    }

    /// @brief A factory method for sequence basic_node objects without sequence_type objects.
    /// @return A YAML sequence node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/sequence/
    static basic_node sequence() {
        basic_node node;
        node.m_attrs = detail::node_attr_bits::seq_bit;
        node.m_node_value.p_sequence = create_object<sequence_type>();
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for sequence basic_node objects with lvalue sequence_type objects.
    /// @param[in] seq A lvalue sequence node value.
    /// @return A YAML sequence node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/sequence/
    static basic_node sequence(const sequence_type& seq) {
        basic_node node;
        node.m_attrs = detail::node_attr_bits::seq_bit;
        node.m_node_value.p_sequence = create_object<sequence_type>(seq);
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for sequence basic_node objects with rvalue sequence_type objects.
    /// @param[in] seq A rvalue sequence node value.
    /// @return A YAML sequence node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/sequence/
    static basic_node sequence(sequence_type&& seq) {
        basic_node node;
        node.m_attrs = detail::node_attr_bits::seq_bit;
        node.m_node_value.p_sequence = create_object<sequence_type>(std::move(seq));
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for mapping basic_node objects without mapping_type objects.
    /// @return A YAML mapping node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/mapping/
    static basic_node mapping() {
        basic_node node;
        node.m_attrs = detail::node_attr_bits::map_bit;
        node.m_node_value.p_mapping = create_object<mapping_type>();
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for mapping basic_node objects with lvalue mapping_type objects.
    /// @param[in] map A lvalue mapping node value.
    /// @return A YAML mapping node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/mapping/
    static basic_node mapping(const mapping_type& map) {
        basic_node node;
        node.m_attrs = detail::node_attr_bits::map_bit;
        node.m_node_value.p_mapping = create_object<mapping_type>(map);
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for mapping basic_node objects with rvalue mapping_type objects.
    /// @param[in] map A rvalue mapping node value.
    /// @return A YAML mapping node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/mapping/
    static basic_node mapping(mapping_type&& map) {
        basic_node node;
        node.m_attrs = detail::node_attr_bits::map_bit;
        node.m_node_value.p_mapping = create_object<mapping_type>(std::move(map));
        return node;
    } // LCOV_EXCL_LINE

    /// @brief A factory method for alias basic_node objects referencing the given anchor basic_node object.
    /// @note The given anchor basic_node must have a non-empty anchor name.
    /// @param[in] anchor_node A basic_node object with an anchor name.
    /// @return An alias YAML node created from the given anchor node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/alias_of/
    static basic_node alias_of(const basic_node& anchor_node) {
        using namespace detail::node_attr_bits;

        if FK_YAML_UNLIKELY (!anchor_node.has_anchor_name() || !(anchor_node.m_attrs & anchor_bit)) {
            throw fkyaml::exception("Cannot create an alias without anchor name.");
        }

        basic_node node = anchor_node;
        node.m_attrs &= ~detail::node_attr_mask::anchoring;
        node.m_attrs |= alias_bit;
        return node;
    } // LCOV_EXCL_LINE

public:
    /// @brief A copy assignment operator of the basic_node class.
    /// @param[in] rhs A lvalue basic_node object to be copied with.
    /// @return Reference to this basic_node object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator=/
    basic_node& operator=(const basic_node& rhs) noexcept {
        basic_node(rhs).swap(*this);
        return *this;
    }

    /// @brief A move assignment operator of the basic_node class.
    /// @param[in] rhs A rvalue basic_node object to be moved from.
    /// @return Reference to this basic_node object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator=/
    basic_node& operator=(basic_node&& rhs) noexcept {
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
    basic_node& operator[](KeyType&& key) {
        if FK_YAML_UNLIKELY (is_scalar()) {
            throw fkyaml::type_error("operator[] is unavailable for a scalar node.", get_type());
        }

        basic_node n = std::forward<KeyType>(key);
        const node_value* p_node_value = get_node_value_ptr();

        if (is_sequence()) {
            if FK_YAML_UNLIKELY (!n.is_integer()) {
                throw fkyaml::type_error(
                    "An argument of operator[] for sequence nodes must be an integer.", get_type());
            }
            FK_YAML_ASSERT(p_node_value->p_sequence != nullptr);
            return p_node_value->p_sequence->operator[](n.get_value<int>());
        }

        FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);
        return p_node_value->p_mapping->operator[](std::move(n));
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
    const basic_node& operator[](KeyType&& key) const {
        if FK_YAML_UNLIKELY (is_scalar()) {
            throw fkyaml::type_error("operator[] is unavailable for a scalar node.", get_type());
        }

        basic_node node_key = std::forward<KeyType>(key);
        const node_value* p_node_value = get_node_value_ptr();

        if (is_sequence()) {
            if FK_YAML_UNLIKELY (!node_key.is_integer()) {
                throw fkyaml::type_error(
                    "An argument of operator[] for sequence nodes must be an integer.", get_type());
            }
            FK_YAML_ASSERT(p_node_value->p_sequence != nullptr);
            return p_node_value->p_sequence->operator[](node_key.get_value<int>());
        }

        FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);
        return p_node_value->p_mapping->operator[](std::move(node_key));
    }

    /// @brief A subscript operator of the basic_node class with a basic_node key object.
    /// @tparam KeyType A key type which is a kind of the basic_node template class.
    /// @param key A key to the target value in a sequence/mapping node.
    /// @return The value associated with the given key, or a default basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator[]/
    template <
        typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
    basic_node& operator[](KeyType&& key) {
        if FK_YAML_UNLIKELY (is_scalar()) {
            throw fkyaml::type_error("operator[] is unavailable for a scalar node.", get_type());
        }

        const node_value* p_node_value = get_node_value_ptr();

        if (is_sequence()) {
            if FK_YAML_UNLIKELY (!key.is_integer()) {
                throw fkyaml::type_error(
                    "An argument of operator[] for sequence nodes must be an integer.", get_type());
            }
            FK_YAML_ASSERT(p_node_value->p_sequence != nullptr);
            return p_node_value->p_sequence->operator[](key.template get_value<int>());
        }

        FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);
        return p_node_value->p_mapping->operator[](std::forward<KeyType>(key));
    }

    /// @brief A subscript operator of the basic_node class with a basic_node key object.
    /// @tparam KeyType A key type which is a kind of the basic_node template class.
    /// @param key A key to the target value in a sequence/mapping node.
    /// @return The value associated with the given key, or a default basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator[]/
    template <
        typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
    const basic_node& operator[](KeyType&& key) const {
        if FK_YAML_UNLIKELY (is_scalar()) {
            throw fkyaml::type_error("operator[] is unavailable for a scalar node.", get_type());
        }

        const node_value* p_node_value = get_node_value_ptr();

        if (is_sequence()) {
            if FK_YAML_UNLIKELY (!key.is_integer()) {
                throw fkyaml::type_error(
                    "An argument of operator[] for sequence nodes must be an integer.", get_type());
            }
            FK_YAML_ASSERT(p_node_value->p_sequence != nullptr);
            return p_node_value->p_sequence->operator[](key.template get_value<int>());
        }

        FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);
        return p_node_value->p_mapping->operator[](std::forward<KeyType>(key));
    }

    /// @brief An equal-to operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true if both types and values are equal, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_eq/
    bool operator==(const basic_node& rhs) const noexcept {
        detail::node_attr_t this_val_bit = get_node_attrs() & detail::node_attr_mask::value;
        if (this_val_bit != (rhs.get_node_attrs() & detail::node_attr_mask::value)) {
            return false;
        }

        const node_value* this_node_value_ptr = get_node_value_ptr();
        const node_value* other_node_value_ptr = rhs.get_node_value_ptr();

        bool ret = false;
        switch (this_val_bit) {
        case detail::node_attr_bits::seq_bit:
            ret = (*(this_node_value_ptr->p_sequence) == *(other_node_value_ptr->p_sequence));
            break;
        case detail::node_attr_bits::map_bit:
            ret = (*(this_node_value_ptr->p_mapping) == *(other_node_value_ptr->p_mapping));
            break;
        case detail::node_attr_bits::null_bit:
            // Always true for comparisons between null nodes.
            ret = true;
            break;
        case detail::node_attr_bits::bool_bit:
            ret = (this_node_value_ptr->boolean == other_node_value_ptr->boolean);
            break;
        case detail::node_attr_bits::int_bit:
            ret = (this_node_value_ptr->integer == other_node_value_ptr->integer);
            break;
        case detail::node_attr_bits::float_bit:
            ret =
                (std::abs(this_node_value_ptr->float_val - other_node_value_ptr->float_val) <
                 std::numeric_limits<float_number_type>::epsilon());
            break;
        case detail::node_attr_bits::string_bit:
            ret = (*(this_node_value_ptr->p_string) == *(other_node_value_ptr->p_string));
            break;
        default:   // LCOV_EXCL_LINE
            break; // LCOV_EXCL_LINE
        }

        return ret;
    }

    /// @brief A not-equal-to operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true if either types or values are different, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_ne/
    bool operator!=(const basic_node& rhs) const noexcept {
        return !operator==(rhs);
    }

    /// @brief A less-than operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true this basic_node object is less than `rhs`.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_lt/
    bool operator<(const basic_node& rhs) const noexcept {
        if (operator==(rhs)) {
            return false;
        }

        detail::node_attr_t this_val_bit = get_node_attrs() & detail::node_attr_mask::value;
        detail::node_attr_t other_val_bit = rhs.get_node_attrs() & detail::node_attr_mask::value;

        if (this_val_bit < other_val_bit) {
            return true;
        }

        if (this_val_bit != other_val_bit) {
            return false;
        }

        const node_value* p_this_value = get_node_value_ptr();
        const node_value* p_other_value = rhs.get_node_value_ptr();

        bool ret = false;
        switch (this_val_bit) {
        case detail::node_attr_bits::seq_bit:
            ret = (*(p_this_value->p_sequence) < *(p_other_value->p_sequence));
            break;
        case detail::node_attr_bits::map_bit:
            ret = (*(p_this_value->p_mapping) < *(p_other_value->p_mapping));
            break;
        case detail::node_attr_bits::null_bit: // LCOV_EXCL_LINE
            // Will not come here since null nodes are always the same.
            break; // LCOV_EXCL_LINE
        case detail::node_attr_bits::bool_bit:
            // false < true
            ret = (!p_this_value->boolean && p_other_value->boolean);
            break;
        case detail::node_attr_bits::int_bit:
            ret = (p_this_value->integer < p_other_value->integer);
            break;
        case detail::node_attr_bits::float_bit:
            ret = (p_this_value->float_val < p_other_value->float_val);
            break;
        case detail::node_attr_bits::string_bit:
            ret = (*(p_this_value->p_string) < *(p_other_value->p_string));
            break;
        default:   // LCOV_EXCL_LINE
            break; // LCOV_EXCL_LINE
        }

        return ret;
    }

    /// @brief A less-than-or-equal-to operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true this basic_node object is less than or equal to `rhs`.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_le/
    bool operator<=(const basic_node& rhs) const noexcept {
        return !rhs.operator<(*this);
    }

    /// @brief A greater-than operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true this basic_node object is greater than `rhs`.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_gt/
    bool operator>(const basic_node& rhs) const noexcept {
        return !operator<=(rhs);
    }

    /// @brief A greater-than-or-equal-to operator of the basic_node class.
    /// @param rhs A basic_node object to be compared with this basic_node object.
    /// @return true this basic_node object is greater than or equal to `rhs`.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/operator_ge/
    bool operator>=(const basic_node& rhs) const noexcept {
        return !operator<(rhs);
    }

public:
    /// @brief Returns the type of the current basic_node value.
    /// @return The type of the YAML node value.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_type/
    node_type get_type() const noexcept {
        detail::node_attr_t attrs = get_node_attrs();
        return detail::node_attr_bits::to_node_type(attrs);
    }

    /// @brief Returns the type of the current basic_node value.
    /// @deprecated Use get_type() function. (since 0.3.12)
    /// @return The type of the YAML node value.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/type/
    FK_YAML_DEPRECATED("Since 0.3.12; Use get_type()")
    node_t type() const noexcept {
        node_type tmp_type = get_type();
        return detail::convert_from_node_type(tmp_type);
    }

    /// @brief Tests whether the current basic_node value is of sequence type.
    /// @return true if the type is sequence, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_sequence/
    bool is_sequence() const noexcept {
        return get_node_attrs() & detail::node_attr_bits::seq_bit;
    }

    /// @brief Tests whether the current basic_node value is of mapping type.
    /// @return true if the type is mapping, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_mapping/
    bool is_mapping() const noexcept {
        return get_node_attrs() & detail::node_attr_bits::map_bit;
    }

    /// @brief Tests whether the current basic_node value is of null type.
    /// @return true if the type is null, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_null/
    bool is_null() const noexcept {
        return get_node_attrs() & detail::node_attr_bits::null_bit;
    }

    /// @brief Tests whether the current basic_node value is of boolean type.
    /// @return true if the type is boolean, false otherwise
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_boolean/
    bool is_boolean() const noexcept {
        return get_node_attrs() & detail::node_attr_bits::bool_bit;
    }

    /// @brief Tests whether the current basic_node value is of integer type.
    /// @return true if the type is integer, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_integer/
    bool is_integer() const noexcept {
        return get_node_attrs() & detail::node_attr_bits::int_bit;
    }

    /// @brief Tests whether the current basic_node value is of float number type.
    /// @return true if the type is floating point number, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_float_number/
    bool is_float_number() const noexcept {
        return get_node_attrs() & detail::node_attr_bits::float_bit;
    }

    /// @brief Tests whether the current basic_node value is of string type.
    /// @return true if the type is string, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_string/
    bool is_string() const noexcept {
        return get_node_attrs() & detail::node_attr_bits::string_bit;
    }

    /// @brief Tests whether the current basic_node value is of scalar types.
    /// @return true if the type is scalar, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_scalar/
    bool is_scalar() const noexcept {
        return get_node_attrs() & detail::node_attr_bits::scalar_bits;
    }

    /// @brief Tests whether the current basic_node is an anchor node.
    /// @return true if the current basic_node is an anchor node, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_anchor/
    bool is_anchor() const noexcept {
        return m_attrs & detail::node_attr_bits::anchor_bit;
    }

    /// @brief Tests whether the current basic_node is an alias node.
    /// @return true if the current basic_node is an alias node, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/is_alias/
    bool is_alias() const noexcept {
        return m_attrs & detail::node_attr_bits::alias_bit;
    }

    /// @brief Tests whether the current basic_node value (sequence, mapping, string) is empty.
    /// @return true if the node value is empty, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/empty/
    bool empty() const {
        switch (get_node_attrs() & detail::node_attr_mask::value) {
        case detail::node_attr_bits::seq_bit: {
            const node_value* p_node_value = get_node_value_ptr();
            FK_YAML_ASSERT(p_node_value->p_sequence != nullptr);
            return p_node_value->p_sequence->empty();
        }
        case detail::node_attr_bits::map_bit: {
            const node_value* p_node_value = get_node_value_ptr();
            FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);
            return p_node_value->p_mapping->empty();
        }
        case detail::node_attr_bits::string_bit: {
            const node_value* p_node_value = get_node_value_ptr();
            FK_YAML_ASSERT(p_node_value->p_string != nullptr);
            return p_node_value->p_string->empty();
        }
        default:
            throw fkyaml::type_error("The target node is not of a container type.", get_type());
        }
    }

    /// @brief Returns the size of the current basic_node value (sequence, mapping, string).
    /// @return The size of a node value.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/size/
    std::size_t size() const {
        const node_value* p_node_value = get_node_value_ptr();
        switch (get_node_attrs() & detail::node_attr_mask::value) {
        case detail::node_attr_bits::seq_bit:
            FK_YAML_ASSERT(p_node_value->p_sequence != nullptr);
            return p_node_value->p_sequence->size();
        case detail::node_attr_bits::map_bit:
            FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);
            return p_node_value->p_mapping->size();
        case detail::node_attr_bits::string_bit:
            FK_YAML_ASSERT(p_node_value->p_string != nullptr);
            return p_node_value->p_string->size();
        default:
            throw fkyaml::type_error("The target node is not of a container type.", get_type());
        }
    }

    /// @brief Check whether this basic_node object has a given key in its inner mapping node value.
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
    bool contains(KeyType&& key) const {
        if FK_YAML_LIKELY (get_node_attrs() & detail::node_attr_bits::map_bit) {
            const node_value* p_node_value = get_node_value_ptr();
            FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);

            const mapping_type& map = *p_node_value->p_mapping;
            basic_node node_key = std::forward<KeyType>(key);
            return map.find(std::move(node_key)) != map.end();
        }

        return false;
    }

    /// @brief Check whether this basic_node object has a given key in its inner mapping Node value.
    /// @tparam KeyType A key type which is a kind of basic_node template class.
    /// @param[in] key A key to the target value in the YAML mapping node value.
    /// @return true if the YAML node is a mapping and has the given key, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/contains/
    template <
        typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
    bool contains(KeyType&& key) const {
        if FK_YAML_LIKELY (get_node_attrs() & detail::node_attr_bits::map_bit) {
            const node_value* p_node_value = get_node_value_ptr();
            FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);

            const mapping_type& map = *p_node_value->p_mapping;
            return map.find(std::forward<KeyType>(key)) != map.end();
        }

        return false;
    }

    /// @brief Get a basic_node object with a key of a compatible type.
    /// @tparam KeyType A key type compatible with basic_node
    /// @param key A key to the target basic_node object in a sequence/mapping node.
    /// @return Reference to the basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/at/
    template <
        typename KeyType, detail::enable_if_t<
                              detail::conjunction<
                                  detail::negation<detail::is_basic_node<KeyType>>,
                                  detail::is_node_compatible_type<basic_node, KeyType>>::value,
                              int> = 0>
    basic_node& at(KeyType&& key) {
        if FK_YAML_UNLIKELY (is_scalar()) {
            throw fkyaml::type_error("at() is unavailable for a scalar node.", get_type());
        }

        basic_node node_key = std::forward<KeyType>(key);
        const node_value* p_node_value = get_node_value_ptr();

        if (is_sequence()) {
            if FK_YAML_UNLIKELY (!node_key.is_integer()) {
                throw fkyaml::type_error("An argument of at() for sequence nodes must be an integer.", get_type());
            }

            FK_YAML_ASSERT(p_node_value->p_sequence != nullptr);
            sequence_type& seq = *p_node_value->p_sequence;
            int index = node_key.template get_value<int>();
            int size = static_cast<int>(seq.size());
            if FK_YAML_UNLIKELY (index >= size) {
                throw fkyaml::out_of_range(index);
            }
            return seq.at(index);
        }

        FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);
        mapping_type& map = *p_node_value->p_mapping;
        bool is_found = map.find(node_key) != map.end();
        if FK_YAML_UNLIKELY (!is_found) {
            throw fkyaml::out_of_range(serialize(node_key).c_str());
        }
        return map.at(node_key);
    }

    /// @brief Get a basic_node object with a key of a compatible type.
    /// @tparam KeyType A key type compatible with basic_node
    /// @param key A key to the target basic_node object in a sequence/mapping node.
    /// @return Constant reference to the basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/at/
    template <
        typename KeyType, detail::enable_if_t<
                              detail::conjunction<
                                  detail::negation<detail::is_basic_node<KeyType>>,
                                  detail::is_node_compatible_type<basic_node, KeyType>>::value,
                              int> = 0>
    const basic_node& at(KeyType&& key) const {
        if FK_YAML_UNLIKELY (is_scalar()) {
            throw fkyaml::type_error("at() is unavailable for a scalar node.", get_type());
        }

        basic_node node_key = std::forward<KeyType>(key);
        const node_value* p_node_value = get_node_value_ptr();

        if (is_sequence()) {
            if FK_YAML_UNLIKELY (!node_key.is_integer()) {
                throw fkyaml::type_error("An argument of at() for sequence nodes must be an integer.", get_type());
            }

            FK_YAML_ASSERT(p_node_value->p_sequence != nullptr);
            const sequence_type& seq = *p_node_value->p_sequence;
            int index = node_key.template get_value<int>();
            int size = static_cast<int>(seq.size());
            if FK_YAML_UNLIKELY (index >= size) {
                throw fkyaml::out_of_range(index);
            }
            return seq.at(index);
        }

        FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);
        const mapping_type& map = *p_node_value->p_mapping;
        bool is_found = map.find(node_key) != map.end();
        if FK_YAML_UNLIKELY (!is_found) {
            throw fkyaml::out_of_range(serialize(node_key).c_str());
        }
        return map.at(node_key);
    }

    /// @brief Get a basic_node object with a basic_node key object.
    /// @tparam KeyType A key type which is a kind of the basic_node template class.
    /// @param key A key to the target basic_node object in a sequence/mapping node.
    /// @return Reference to the basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/at/
    template <
        typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
    basic_node& at(KeyType&& key) {
        if FK_YAML_UNLIKELY (is_scalar()) {
            throw fkyaml::type_error("at() is unavailable for a scalar node.", get_type());
        }

        const node_value* p_node_value = get_node_value_ptr();

        if (is_sequence()) {
            if FK_YAML_UNLIKELY (!key.is_integer()) {
                throw fkyaml::type_error("An argument of at() for sequence nodes must be an integer.", get_type());
            }

            FK_YAML_ASSERT(p_node_value->p_sequence != nullptr);
            sequence_type& seq = *p_node_value->p_sequence;
            int index = std::forward<KeyType>(key).template get_value<int>();
            int size = static_cast<int>(seq.size());
            if FK_YAML_UNLIKELY (index >= size) {
                throw fkyaml::out_of_range(index);
            }
            return seq.at(index);
        }

        FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);
        mapping_type& map = *p_node_value->p_mapping;
        bool is_found = map.find(key) != map.end();
        if FK_YAML_UNLIKELY (!is_found) {
            throw fkyaml::out_of_range(serialize(key).c_str());
        }
        return map.at(key);
    }

    /// @brief Get a basic_node object with a basic_node key object.
    /// @tparam KeyType A key type which is a kind of the basic_node template class.
    /// @param key A key to the target basic_node object in a sequence/mapping node.
    /// @return Constant reference to the basic_node object associated with the given key.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/at/
    template <
        typename KeyType, detail::enable_if_t<detail::is_basic_node<detail::remove_cvref_t<KeyType>>::value, int> = 0>
    const basic_node& at(KeyType&& key) const {
        if FK_YAML_UNLIKELY (is_scalar()) {
            throw fkyaml::type_error("at() is unavailable for a scalar node.", get_type());
        }

        const node_value* p_node_value = get_node_value_ptr();

        if (is_sequence()) {
            if FK_YAML_UNLIKELY (!key.is_integer()) {
                throw fkyaml::type_error("An argument of at() for sequence nodes must be an integer.", get_type());
            }

            FK_YAML_ASSERT(p_node_value->p_sequence != nullptr);
            const sequence_type& seq = *p_node_value->p_sequence;
            int index = std::forward<KeyType>(key).template get_value<int>();
            int size = static_cast<int>(seq.size());
            if FK_YAML_UNLIKELY (index >= size) {
                throw fkyaml::out_of_range(index);
            }
            return seq.at(index);
        }

        FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);
        const mapping_type& map = *p_node_value->p_mapping;
        bool is_found = map.find(key) != map.end();
        if FK_YAML_UNLIKELY (!is_found) {
            throw fkyaml::out_of_range(serialize(key).c_str());
        }
        return map.at(key);
    }

    /// @brief Get the YAML version for this basic_node object.
    /// @return The YAML version if already set, `yaml_version_type::VERSION_1_2` otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_yaml_version_type/
    yaml_version_type get_yaml_version_type() const noexcept {
        return mp_meta->is_version_specified ? mp_meta->version : yaml_version_type::VERSION_1_2;
    }

    /// @brief Set the YAML version for this basic_node object.
    /// @param[in] version The target YAML version.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/set_yaml_version_type/
    void set_yaml_version_type(const yaml_version_type version) noexcept {
        mp_meta->version = version;
        mp_meta->is_version_specified = true;
    }

    /// @brief Get the YAML version for this basic_node object.
    /// @deprecated Use get_yaml_version_type() function. (since 0.3.12)
    /// @return The YAML version if already set, `yaml_version_t::VER_1_2` otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_yaml_version/
    FK_YAML_DEPRECATED("Since 0.3.12; Use get_yaml_version_type()")
    yaml_version_t get_yaml_version() const noexcept {
        yaml_version_type tmp_type = get_yaml_version_type();
        return detail::convert_from_yaml_version_type(tmp_type);
    }

    /// @brief Set the YAML version for this basic_node object.
    /// @deprecated Use set_yaml_version_type(yaml_version_type) function. (since 0.3.12)
    /// @param[in] version The target YAML version.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/set_yaml_version/
    FK_YAML_DEPRECATED("Since 0.3.12; Use set_yaml_version_type(const yaml_version_type)")
    void set_yaml_version(const yaml_version_t version) noexcept {
        set_yaml_version_type(detail::convert_to_yaml_version_type(version));
    }

    /// @brief Check whether this basic_node object has already had any anchor name.
    /// @return true if ths basic_node has an anchor name, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/has_anchor_name/
    bool has_anchor_name() const noexcept {
        return (m_attrs & detail::node_attr_mask::anchoring) && !m_prop.anchor.empty();
    }

    /// @brief Get the anchor name associated with this basic_node object.
    /// @note Some anchor name must be set before calling this method. Call has_anchor_name() to see if this basic_node
    /// object has any anchor name.
    /// @return The anchor name associated with the node.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_anchor_name/
    const std::string& get_anchor_name() const {
        if FK_YAML_UNLIKELY (!has_anchor_name()) {
            throw fkyaml::exception("No anchor name has been set.");
        }
        return m_prop.anchor;
    }

    /// @brief Add an anchor name to this basic_node object.
    /// @note If this basic_node object has already had any anchor name, the new anchor name will overwrite the old one.
    /// @param[in] anchor_name An anchor name. This should not be empty.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/add_anchor_name/
    void add_anchor_name(const std::string& anchor_name) {
        if (is_anchor()) {
            m_attrs &= ~detail::node_attr_mask::anchoring;
            auto itr = mp_meta->anchor_table.equal_range(m_prop.anchor).first;
            std::advance(itr, detail::node_attr_bits::get_anchor_offset(m_attrs));
            mp_meta.reset();
            itr->second.swap(*this);
            mp_meta->anchor_table.erase(itr);
        }

        auto p_meta = mp_meta;

        basic_node node;
        node.swap(*this);
        p_meta->anchor_table.emplace(anchor_name, std::move(node));

        m_attrs &= ~detail::node_attr_mask::anchoring;
        m_attrs |= detail::node_attr_bits::anchor_bit;
        mp_meta = p_meta;
        uint32_t offset = static_cast<uint32_t>(mp_meta->anchor_table.count(anchor_name) - 1);
        detail::node_attr_bits::set_anchor_offset(offset, m_attrs);
        m_prop.anchor = anchor_name;
    }

    /// @brief Add an anchor name to this basic_node object.
    /// @note If this basic_node object has already had any anchor name, the new anchor name will overwrite the old one.
    /// @param[in] anchor_name An anchor name. This should not be empty.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/add_anchor_name/
    void add_anchor_name(std::string&& anchor_name) {
        if (is_anchor()) {
            m_attrs &= ~detail::node_attr_mask::anchoring;
            auto itr = mp_meta->anchor_table.equal_range(m_prop.anchor).first;
            std::advance(itr, detail::node_attr_bits::get_anchor_offset(m_attrs));
            mp_meta.reset();
            itr->second.swap(*this);
            mp_meta->anchor_table.erase(itr);
        }

        auto p_meta = mp_meta;

        basic_node node;
        node.swap(*this);
        p_meta->anchor_table.emplace(anchor_name, std::move(node));

        m_attrs &= ~detail::node_attr_mask::anchoring;
        m_attrs |= detail::node_attr_bits::anchor_bit;
        mp_meta = p_meta;
        uint32_t offset = static_cast<uint32_t>(mp_meta->anchor_table.count(anchor_name) - 1);
        detail::node_attr_bits::set_anchor_offset(offset, m_attrs);
        m_prop.anchor = std::move(anchor_name);
    }

    /// @brief Check whether this basic_node object has already had any tag name.
    /// @return true if ths basic_node has a tag name, false otherwise.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/has_tag_name/
    bool has_tag_name() const noexcept {
        return !m_prop.tag.empty();
    }

    /// @brief Get the tag name associated with this basic_node object.
    /// @note Some tag name must be set before calling this method. Call has_tag_name() to see if this basic_node
    /// object has any tag name.
    /// @return The tag name associated with the node. It may be empty.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_tag_name/
    const std::string& get_tag_name() const {
        if FK_YAML_UNLIKELY (!has_tag_name()) {
            throw fkyaml::exception("No tag name has been set.");
        }
        return m_prop.tag;
    }

    /// @brief Add a tag name to this basic_node object.
    /// @note If this basic_node object has already had any tag name, the new tag name will overwrite the old one.
    /// @param[in] tag_name A tag name to get associated with this basic_node object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/add_tag_name/
    void add_tag_name(const std::string& tag_name) {
        m_prop.tag = tag_name;
    }

    /// @brief Add a tag name to this basic_node object.
    /// @note If this basic_node object has already had any tag name, the new tag name will overwrite the old one.
    /// @param[in] tag_name A tag name to get associated with this basic_node object.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/add_tag_name/
    void add_tag_name(std::string&& tag_name) {
        m_prop.tag = std::move(tag_name);
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
    T get_value() const noexcept(noexcept(
        ConverterType<ValueType, void>::from_node(std::declval<const basic_node&>(), std::declval<ValueType&>()))) {
        auto ret = ValueType();
        if (has_anchor_name()) {
            auto itr = mp_meta->anchor_table.equal_range(m_prop.anchor).first;
            std::advance(itr, detail::node_attr_bits::get_anchor_offset(m_attrs));
            ConverterType<ValueType, void>::from_node(itr->second, ret);
        }
        else {
            ConverterType<ValueType, void>::from_node(*this, ret);
        }
        return ret;
    }

    /// @brief Explicit reference access to the internally stored YAML node value.
    /// @tparam ReferenceType Reference type to the target YAML node value.
    /// @return Reference to the internally stored YAML node value.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/get_value_ref/
    template <typename ReferenceType, detail::enable_if_t<std::is_reference<ReferenceType>::value, int> = 0>
    ReferenceType get_value_ref() {
        if (has_anchor_name()) {
            auto itr = mp_meta->anchor_table.equal_range(m_prop.anchor).first;
            std::advance(itr, detail::node_attr_bits::get_anchor_offset(m_attrs));
            return itr->second.get_value_ref_impl(static_cast<detail::add_pointer_t<ReferenceType>>(nullptr));
        }
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
    ReferenceType get_value_ref() const {
        if (has_anchor_name()) {
            auto itr = mp_meta->anchor_table.equal_range(m_prop.anchor).first;
            std::advance(itr, detail::node_attr_bits::get_anchor_offset(m_attrs));
            return itr->second.get_value_ref_impl(static_cast<detail::add_pointer_t<ReferenceType>>(nullptr));
        }
        return get_value_ref_impl(static_cast<detail::add_pointer_t<ReferenceType>>(nullptr));
    }

    /// @brief Swaps the internally stored data with the specified basic_node object.
    /// @param[in] rhs A basic_node object to be swapped with.
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/swap/
    void swap(basic_node& rhs) noexcept {
        using std::swap;
        swap(m_attrs, rhs.m_attrs);
        swap(mp_meta, rhs.mp_meta);

        node_value tmp {};
        std::memcpy(&tmp, &m_node_value, sizeof(node_value));
        std::memcpy(&m_node_value, &rhs.m_node_value, sizeof(node_value));
        std::memcpy(&rhs.m_node_value, &tmp, sizeof(node_value));

        swap(m_prop.tag, rhs.m_prop.tag);
        swap(m_prop.anchor, rhs.m_prop.anchor);
    }

    /// @brief Returns the first iterator of basic_node values of container types (sequence or mapping) from a non-const
    /// basic_node object. Throws exception if the basic_node value is not of container types.
    /// @return An iterator to the first element of a YAML node value (either sequence or mapping).
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/begin/
    iterator begin() {
        switch (get_node_attrs() & detail::node_attr_mask::value) {
        case detail::node_attr_bits::seq_bit: {
            const node_value* p_node_value = get_node_value_ptr();
            FK_YAML_ASSERT(p_node_value->p_sequence != nullptr);
            return {detail::sequence_iterator_tag(), p_node_value->p_sequence->begin()};
        }
        case detail::node_attr_bits::map_bit: {
            const node_value* p_node_value = get_node_value_ptr();
            FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);
            return {detail::mapping_iterator_tag(), p_node_value->p_mapping->begin()};
        }
        default:
            throw fkyaml::type_error("The target node is neither of sequence nor mapping types.", get_type());
        }
    }

    /// @brief Returns the first iterator of basic_node values of container types (sequence or mapping) from a const
    /// basic_node object. Throws exception if the basic_node value is not of container types.
    /// @return A constant iterator to the first element of a YAML node value (either sequence or mapping).
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/begin/
    const_iterator begin() const {
        switch (get_node_attrs() & detail::node_attr_mask::value) {
        case detail::node_attr_bits::seq_bit: {
            const node_value* p_node_value = get_node_value_ptr();
            FK_YAML_ASSERT(p_node_value->p_sequence != nullptr);
            return {detail::sequence_iterator_tag(), p_node_value->p_sequence->begin()};
        }
        case detail::node_attr_bits::map_bit: {
            const node_value* p_node_value = get_node_value_ptr();
            FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);
            return {detail::mapping_iterator_tag(), p_node_value->p_mapping->begin()};
        }
        default:
            throw fkyaml::type_error("The target node is neither of sequence nor mapping types.", get_type());
        }
    }

    /// @brief Returns the last iterator of basic_node values of container types (sequence or mapping) from a non-const
    /// basic_node object. Throws exception if the basic_node value is not of container types.
    /// @return An iterator to the past-the end element of a YAML node value (either sequence or mapping).
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/end/
    iterator end() {
        switch (get_node_attrs() & detail::node_attr_mask::value) {
        case detail::node_attr_bits::seq_bit: {
            const node_value* p_node_value = get_node_value_ptr();
            FK_YAML_ASSERT(p_node_value->p_sequence != nullptr);
            return {detail::sequence_iterator_tag(), p_node_value->p_sequence->end()};
        }
        case detail::node_attr_bits::map_bit: {
            const node_value* p_node_value = get_node_value_ptr();
            FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);
            return {detail::mapping_iterator_tag(), p_node_value->p_mapping->end()};
        }
        default:
            throw fkyaml::type_error("The target node is neither of sequence nor mapping types.", get_type());
        }
    }

    /// @brief Returns the last iterator of basic_node values of container types (sequence or mapping) from a const
    /// basic_node object. Throws exception if the basic_node value is not of container types.
    /// @return A constant iterator to the past-the end element of a YAML node value (either sequence or mapping).
    /// @sa https://fktn-k.github.io/fkYAML/api/basic_node/end/
    const_iterator end() const {
        switch (get_node_attrs() & detail::node_attr_mask::value) {
        case detail::node_attr_bits::seq_bit: {
            const node_value* p_node_value = get_node_value_ptr();
            FK_YAML_ASSERT(p_node_value->p_sequence != nullptr);
            return {detail::sequence_iterator_tag(), p_node_value->p_sequence->end()};
        }
        case detail::node_attr_bits::map_bit: {
            const node_value* p_node_value = get_node_value_ptr();
            FK_YAML_ASSERT(p_node_value->p_mapping != nullptr);
            return {detail::mapping_iterator_tag(), p_node_value->p_mapping->end()};
        }
        default:
            throw fkyaml::type_error("The target node is neither of sequence nor mapping types.", get_type());
        }
    }

private:
    /// @brief Returns the pointer to the node_value object of either this node or the associated anchor node.
    /// @return The pointer to the node_value object of either this node or the associated anchor node.
    const node_value* get_node_value_ptr() const {
        if (has_anchor_name()) {
            auto itr = mp_meta->anchor_table.equal_range(m_prop.anchor).first;
            std::advance(itr, detail::node_attr_bits::get_anchor_offset(m_attrs));
            return &(itr->second.m_node_value);
        }
        return &m_node_value;
    }

    detail::node_attr_t get_node_attrs() const {
        if (has_anchor_name()) {
            auto itr = mp_meta->anchor_table.equal_range(m_prop.anchor).first;
            std::advance(itr, detail::node_attr_bits::get_anchor_offset(m_attrs));
            return itr->second.m_attrs;
        }
        return m_attrs;
    }

    /// @brief Returns reference to the sequence node value.
    /// @throw fkyaml::exception The node value is not a sequence.
    /// @return Reference to the sequence node value.
    sequence_type& get_value_ref_impl(sequence_type* /*unused*/) {
        if FK_YAML_LIKELY (m_attrs & detail::node_attr_bits::seq_bit) {
            return *(m_node_value.p_sequence);
        }
        throw fkyaml::type_error("The node value is not a sequence.", get_type());
    }

    /// @brief Returns constant reference to the sequence node value.
    /// @throw fkyaml::exception The node value is not a sequence.
    /// @return Constant reference to the sequence node value.
    const sequence_type& get_value_ref_impl(const sequence_type* /*unused*/) const {
        if FK_YAML_LIKELY (m_attrs & detail::node_attr_bits::seq_bit) {
            return *(m_node_value.p_sequence);
        }
        throw fkyaml::type_error("The node value is not a sequence.", get_type());
    }

    /// @brief Returns reference to the mapping node value.
    /// @throw fkyaml::exception The node value is not a mapping.
    /// @return Reference to the mapping node value.
    mapping_type& get_value_ref_impl(mapping_type* /*unused*/) {
        if FK_YAML_LIKELY (m_attrs & detail::node_attr_bits::map_bit) {
            return *(m_node_value.p_mapping);
        }
        throw fkyaml::type_error("The node value is not a mapping.", get_type());
    }

    /// @brief Returns constant reference to the mapping node value.
    /// @throw fkyaml::exception The node value is not a mapping.
    /// @return Constant reference to the mapping node value.
    const mapping_type& get_value_ref_impl(const mapping_type* /*unused*/) const {
        if FK_YAML_LIKELY (m_attrs & detail::node_attr_bits::map_bit) {
            return *(m_node_value.p_mapping);
        }
        throw fkyaml::type_error("The node value is not a mapping.", get_type());
    }

    /// @brief Returns reference to the boolean node value.
    /// @throw fkyaml::exception The node value is not a boolean.
    /// @return Reference to the boolean node value.
    boolean_type& get_value_ref_impl(boolean_type* /*unused*/) {
        if FK_YAML_LIKELY (m_attrs & detail::node_attr_bits::bool_bit) {
            return m_node_value.boolean;
        }
        throw fkyaml::type_error("The node value is not a boolean.", get_type());
    }

    /// @brief Returns reference to the boolean node value.
    /// @throw fkyaml::exception The node value is not a boolean.
    /// @return Constant reference to the boolean node value.
    const boolean_type& get_value_ref_impl(const boolean_type* /*unused*/) const {
        if FK_YAML_LIKELY (m_attrs & detail::node_attr_bits::bool_bit) {
            return m_node_value.boolean;
        }
        throw fkyaml::type_error("The node value is not a boolean.", get_type());
    }

    /// @brief Returns reference to the integer node value.
    /// @throw fkyaml::exception The node value is not an integer.
    /// @return Reference to the integer node value.
    integer_type& get_value_ref_impl(integer_type* /*unused*/) {
        if FK_YAML_LIKELY (m_attrs & detail::node_attr_bits::int_bit) {
            return m_node_value.integer;
        }
        throw fkyaml::type_error("The node value is not an integer.", get_type());
    }

    /// @brief Returns reference to the integer node value.
    /// @throw fkyaml::exception The node value is not an integer.
    /// @return Constant reference to the integer node value.
    const integer_type& get_value_ref_impl(const integer_type* /*unused*/) const {
        if FK_YAML_LIKELY (m_attrs & detail::node_attr_bits::int_bit) {
            return m_node_value.integer;
        }
        throw fkyaml::type_error("The node value is not an integer.", get_type());
    }

    /// @brief Returns reference to the floating point number node value.
    /// @throw fkyaml::exception The node value is not a floating point number.
    /// @return Reference to the floating point number node value.
    float_number_type& get_value_ref_impl(float_number_type* /*unused*/) {
        if FK_YAML_LIKELY (m_attrs & detail::node_attr_bits::float_bit) {
            return m_node_value.float_val;
        }
        throw fkyaml::type_error("The node value is not a floating point number.", get_type());
    }

    /// @brief Returns reference to the floating point number node value.
    /// @throw fkyaml::exception The node value is not a floating point number.
    /// @return Constant reference to the floating point number node value.
    const float_number_type& get_value_ref_impl(const float_number_type* /*unused*/) const {
        if FK_YAML_LIKELY (m_attrs & detail::node_attr_bits::float_bit) {
            return m_node_value.float_val;
        }
        throw fkyaml::type_error("The node value is not a floating point number.", get_type());
    }

    /// @brief Returns reference to the string node value.
    /// @throw fkyaml::exception The node value is not a string.
    /// @return Reference to the string node value.
    string_type& get_value_ref_impl(string_type* /*unused*/) {
        if FK_YAML_LIKELY (m_attrs & detail::node_attr_bits::string_bit) {
            return *(m_node_value.p_string);
        }
        throw fkyaml::type_error("The node value is not a string.", get_type());
    }

    /// @brief Returns reference to the string node value.
    /// @throw fkyaml::exception The node value is not a string.
    /// @return Constant reference to the string node value.
    const string_type& get_value_ref_impl(const string_type* /*unused*/) const {
        if FK_YAML_LIKELY (m_attrs & detail::node_attr_bits::string_bit) {
            return *(m_node_value.p_string);
        }
        throw fkyaml::type_error("The node value is not a string.", get_type());
    }

    /// The current node attributes.
    detail::node_attr_t m_attrs {detail::node_attr_bits::default_bits};
    /// The shared set of YAML directives applied to this node.
    mutable std::shared_ptr<detail::document_metainfo<basic_node>> mp_meta {
        std::shared_ptr<detail::document_metainfo<basic_node>>(new detail::document_metainfo<basic_node>())};
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
        rhs) noexcept(noexcept(lhs.swap(rhs))) {
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
        n) {
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
    basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>& n) {
    n = basic_node<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType, ConverterType>::
        deserialize(is);
    return is;
}

/// @brief default YAML node value container.
/// @sa https://fktn-k.github.io/fkYAML/api/basic_node/node/
using node = basic_node<>;

/// @brief namespace for user-defined literals for the fkYAML library.
inline namespace literals {
/// @brief namespace for user-defined literals for YAML node objects.
inline namespace yaml_literals {

/// @brief The user-defined string literal which deserializes a `char` array into a `node` object.
/// @param s An input `char` array.
/// @param n The size of `s`.
/// @return The resulting `node` object deserialized from `s`.
/// @sa https://fktn-k.github.io/fkYAML/api/operator_literal_yaml/
inline fkyaml::node operator"" _yaml(const char* s, std::size_t n) {
    return fkyaml::node::deserialize(std::move(s), std::move(s + n));
}

/// @brief The user-defined string literal which deserializes a `char16_t` array into a `node` object.
/// @param s An input `char16_t` array.
/// @param n The size of `s`.
/// @return The resulting `node` object deserialized from `s`.
/// @sa https://fktn-k.github.io/fkYAML/api/operator_literal_yaml/
inline fkyaml::node operator"" _yaml(const char16_t* s, std::size_t n) {
    return fkyaml::node::deserialize(std::move(s), std::move(s + n));
}

/// @brief The user-defined string literal which deserializes a `char32_t` array into a `node` object.
/// @param s An input `char32_t` array.
/// @param n The size of `s`.
/// @return The resulting `node` object deserialized from `s`.
/// @sa https://fktn-k.github.io/fkYAML/api/operator_literal_yaml/
inline fkyaml::node operator"" _yaml(const char32_t* s, std::size_t n) {
    return fkyaml::node::deserialize(std::move(s), std::move(s + n));
}

#if FK_YAML_HAS_CHAR8_T
/// @brief The user-defined string literal which deserializes a `char8_t` array into a `node` object.
/// @param s An input `char8_t` array.
/// @param n The size of `s`.
/// @return The resulting `node` object deserialized from `s`.
inline fkyaml::node operator"" _yaml(const char8_t* s, std::size_t n) {
    return fkyaml::node::deserialize((const char8_t*)s, (const char8_t*)s + n);
}
#endif

} // namespace yaml_literals
} // namespace literals

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_NODE_HPP */
