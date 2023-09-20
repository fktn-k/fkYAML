/**
 * @file Deserializer.hpp
 * @brief Implementation of the deserializer for YAML documents.
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#include <cstdint>
#include <unordered_map>

#include "fkYAML/VersioningMacros.hpp"
#include "fkYAML/Exception.hpp"
#include "fkYAML/LexicalAnalyzer.hpp"
#include "fkYAML/Node.hpp"
#include "fkYAML/NodeTypeTraits.hpp"

#ifndef FK_YAML_DESERIALIZER_HPP_
#define FK_YAML_DESERIALIZER_HPP_

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

/**
 * @class BasicDeserializer
 * @brief A class which provides the feature of deserializing YAML documents.
 */

/**
 * @class BasicDeserializer
 * @brief A class which provides the feature of deserializing YAML documents.
 *
 * @tparam BasicNodeType A type of the container for deserialized YAML values.
 */
template <typename BasicNodeType = Node>
class BasicDeserializer
{
    static_assert(IsBasicNode<BasicNodeType>::value, "BasicDeserializer only accepts (const) BasicNode<...>");

    /** A type for sequence node value containers. */
    using sequence_type = typename BasicNodeType::sequence_type;
    /** A type for mapping node value containers. */
    using mapping_type = typename BasicNodeType::mapping_type;
    /** A type for boolean node values. */
    using boolean_type = typename BasicNodeType::boolean_type;
    /** A type for integer node values. */
    using integer_type = typename BasicNodeType::integer_type;
    /** A type for float number node values. */
    using float_number_type = typename BasicNodeType::float_number_type;
    /** A type for string node values. */
    using string_type = typename BasicNodeType::string_type;
    /** A type for the lexical analyzer object used by this deserializer. */
    using lexer_type = LexicalAnalyzer<BasicNodeType>;

public:
    /**
     * @brief Construct a new BasicDeserializer object.
     */
    BasicDeserializer() = default;

public:
    /**
     * @brief Deserialize a YAML-formatted source string into a YAML node.
     *
     * @param source A YAML-formatted source string.
     * @return BasicNodeType A root YAML node deserialized from the source string.
     */
    BasicNodeType Deserialize(const char* const source)
    {
        if (!source)
        {
            throw Exception("The given source for deserialization is nullptr.");
        }

        m_lexer.SetInputBuffer(source);
        BasicNodeType root = BasicNodeType::Mapping();
        std::vector<BasicNodeType*> node_stack {};
        std::unordered_map<string_type, BasicNodeType> anchor_table {};

        BasicNodeType* p_current_node = &root;

        string_type anchor_name {};
        bool needs_anchor_impl = false;

        YamlVersionType yaml_version {YamlVersionType::VER_1_2};

        LexicalTokenType type = m_lexer.GetNextToken();
        while (type != LexicalTokenType::END_OF_BUFFER)
        {
            switch (type)
            {
            case LexicalTokenType::KEY_SEPARATOR:
                if (node_stack.empty() || !node_stack.back()->IsMapping())
                {
                    throw Exception("A key separator found while a value token is expected.");
                }
                if (p_current_node->IsSequence() && p_current_node->Size() == 1 &&
                    p_current_node->operator[](0).IsString())
                {
                    // make sequence node to mapping node.
                    string_type tmp_str = p_current_node->operator[](0).ToString();
                    p_current_node->operator[](0) = BasicNodeType::Mapping();
                    node_stack.emplace_back(p_current_node);
                    p_current_node = &(p_current_node->operator[](0));
                    p_current_node->SetVersion(yaml_version);
                    p_current_node->ToMapping().emplace(tmp_str, BasicNodeType());
                    node_stack.emplace_back(p_current_node);
                    p_current_node = &(p_current_node->operator[](tmp_str));
                    p_current_node->SetVersion(yaml_version);
                }
                break;
            case LexicalTokenType::VALUE_SEPARATOR:
                if (!p_current_node->IsSequence() && !p_current_node->IsMapping())
                {
                    throw Exception("A value separator must appear in a container node.");
                }
                break;
            case LexicalTokenType::ANCHOR_PREFIX: {
                if (p_current_node->IsMapping())
                {
                    throw Exception("A mapping node cannot be an anchor.");
                }
                anchor_name = m_lexer.GetString();
                needs_anchor_impl = true;
                break;
            }
            case LexicalTokenType::ALIAS_PREFIX: {
                if (p_current_node->IsMapping())
                {
                    throw Exception("Cannot apply alias to a mapping node.");
                }
                anchor_name = m_lexer.GetString();
                if (anchor_table.find(anchor_name) == anchor_table.end())
                {
                    throw Exception("The given anchor name must appear prior to the alias node.");
                }
                if (p_current_node->IsSequence())
                {
                    p_current_node->ToSequence().emplace_back(BasicNodeType::AliasOf(anchor_table.at(anchor_name)));
                    p_current_node->ToSequence().back().SetVersion(yaml_version);
                    anchor_name.clear();
                    break;
                }
                *p_current_node = BasicNodeType::AliasOf(anchor_table.at(anchor_name));
                anchor_name.clear();
                break;
            }
            case LexicalTokenType::COMMENT_PREFIX:
                break;
            case LexicalTokenType::YAML_VER_DIRECTIVE: {
                FK_YAML_ASSERT(p_current_node == &root);
                yaml_version = ConvertToVersionType(m_lexer.GetYamlVersion());
                p_current_node->SetVersion(yaml_version);
                break;
            }
            case LexicalTokenType::TAG_DIRECTIVE:
                // TODO: implement tag directive deserialization.
            case LexicalTokenType::INVALID_DIRECTIVE:
                // TODO: should output a warning log. Currently just ignore this case.
                break;
            case LexicalTokenType::SEQUENCE_BLOCK_PREFIX:
                if (p_current_node->IsNull())
                {
                    *p_current_node = BasicNodeType::Sequence();
                    p_current_node->SetVersion(yaml_version);
                    break;
                }
                if (p_current_node->IsMapping())
                {
                    if (p_current_node->IsEmpty())
                    {
                        *p_current_node = BasicNodeType::Sequence();
                        break;
                    }

                    // for the second or later mapping items in a sequence node.
                    node_stack.back()->ToSequence().emplace_back(BasicNodeType::Mapping());
                    p_current_node = &(node_stack.back()->ToSequence().back());
                    p_current_node->SetVersion(yaml_version);
                    break;
                }
                if (!p_current_node->IsSequence())
                {
                    throw Exception("Cannot assign a sequence value to a scalar node.");
                }
                break;
            case LexicalTokenType::SEQUENCE_FLOW_BEGIN:
                if (p_current_node->IsMapping())
                {
                    throw Exception("Cannot assign a sequence value as a key.");
                }
                *p_current_node = BasicNodeType::Sequence();
                p_current_node->SetVersion(yaml_version);
                break;
            case LexicalTokenType::SEQUENCE_FLOW_END:
                if (!p_current_node->IsSequence())
                {
                    throw Exception("Invalid sequence flow ending found.");
                }
                p_current_node = node_stack.back();
                node_stack.pop_back();
                break;
            case LexicalTokenType::MAPPING_BLOCK_PREFIX:
                if (!p_current_node->IsNull())
                {
                    throw Exception("Cannot assign a mapping value as a key.");
                }
                *p_current_node = BasicNodeType::Mapping();
                p_current_node->SetVersion(yaml_version);
                break;
            case LexicalTokenType::MAPPING_FLOW_BEGIN:
                if (p_current_node->IsMapping())
                {
                    throw Exception("Cannot assign a mapping value as a key.");
                }
                *p_current_node = BasicNodeType::Mapping();
                p_current_node->SetVersion(yaml_version);
                break;
            case LexicalTokenType::MAPPING_FLOW_END:
                if (!p_current_node->IsMapping())
                {
                    throw Exception("Invalid mapping flow ending found.");
                }
                p_current_node = node_stack.back();
                node_stack.pop_back();
                break;
            case LexicalTokenType::NULL_VALUE:
                if (p_current_node->IsMapping())
                {
                    throw Exception("Cannot assign a null value as a key.");
                }

                // A null value is already assigned in the default Node ctor.
                // Just make sure that the actual value is really a null value.
                m_lexer.GetNull();

                if (p_current_node->IsSequence())
                {
                    p_current_node->ToSequence().emplace_back();
                    p_current_node->ToSequence().back().SetVersion(yaml_version);
                    break;
                }

                p_current_node->SetVersion(yaml_version);
                p_current_node = node_stack.back();
                node_stack.pop_back();
                break;
            case LexicalTokenType::BOOLEAN_VALUE:
                if (p_current_node->IsMapping())
                {
                    throw Exception("Cannot assign a boolean as a key.");
                }

                if (p_current_node->IsSequence())
                {
                    p_current_node->ToSequence().emplace_back(BasicNodeType::BooleanScalar(m_lexer.GetBoolean()));
                    p_current_node->ToSequence().back().SetVersion(yaml_version);
                    if (needs_anchor_impl)
                    {
                        p_current_node->ToSequence().back().AddAnchorName(anchor_name);
                        anchor_table[anchor_name] = p_current_node->ToSequence().back();
                        needs_anchor_impl = false;
                        anchor_name.clear();
                    }
                    break;
                }
                // a scalar node
                *p_current_node = BasicNodeType::BooleanScalar(m_lexer.GetBoolean());
                p_current_node->SetVersion(yaml_version);
                if (needs_anchor_impl)
                {
                    p_current_node->AddAnchorName(anchor_name);
                    anchor_table[anchor_name] = *p_current_node;
                    needs_anchor_impl = false;
                    anchor_name.clear();
                }
                p_current_node = node_stack.back();
                node_stack.pop_back();
                break;
            case LexicalTokenType::INTEGER_VALUE:
                if (p_current_node->IsMapping())
                {
                    throw Exception("Cannot assign a signed integer as a key.");
                }

                if (p_current_node->IsSequence())
                {
                    p_current_node->ToSequence().emplace_back(BasicNodeType::IntegerScalar(m_lexer.GetInteger()));
                    p_current_node->ToSequence().back().SetVersion(yaml_version);
                    if (needs_anchor_impl)
                    {
                        p_current_node->ToSequence().back().AddAnchorName(anchor_name);
                        anchor_table[anchor_name] = p_current_node->ToSequence().back();
                        needs_anchor_impl = false;
                        anchor_name.clear();
                    }
                    break;
                }
                // a scalar node
                *p_current_node = BasicNodeType::IntegerScalar(m_lexer.GetInteger());
                p_current_node->SetVersion(yaml_version);
                if (needs_anchor_impl)
                {
                    p_current_node->AddAnchorName(anchor_name);
                    anchor_table[anchor_name] = *p_current_node;
                    needs_anchor_impl = false;
                    anchor_name.clear();
                }
                p_current_node = node_stack.back();
                node_stack.pop_back();
                break;
            case LexicalTokenType::FLOAT_NUMBER_VALUE:
                if (p_current_node->IsMapping())
                {
                    throw Exception("Cannot assign a float number as a key.");
                }

                if (p_current_node->IsSequence())
                {
                    p_current_node->ToSequence().emplace_back(
                        BasicNodeType::FloatNumberScalar(m_lexer.GetFloatNumber()));
                    p_current_node->ToSequence().back().SetVersion(yaml_version);
                    if (needs_anchor_impl)
                    {
                        p_current_node->ToSequence().back().AddAnchorName(anchor_name);
                        anchor_table[anchor_name] = p_current_node->ToSequence().back();
                        needs_anchor_impl = false;
                        anchor_name.clear();
                    }
                    break;
                }
                // a scalar node
                *p_current_node = BasicNodeType::FloatNumberScalar(m_lexer.GetFloatNumber());
                p_current_node->SetVersion(yaml_version);
                if (needs_anchor_impl)
                {
                    p_current_node->AddAnchorName(anchor_name);
                    anchor_table[anchor_name] = *p_current_node;
                    needs_anchor_impl = false;
                    anchor_name.clear();
                }
                p_current_node = node_stack.back();
                node_stack.pop_back();
                break;
            case LexicalTokenType::STRING_VALUE:
                if (p_current_node->IsMapping())
                {
                    p_current_node->ToMapping().emplace(m_lexer.GetString(), BasicNodeType());
                    node_stack.push_back(p_current_node);
                    p_current_node = &(p_current_node->ToMapping().at(m_lexer.GetString()));
                    break;
                }
                if (p_current_node->IsSequence())
                {
                    p_current_node->ToSequence().emplace_back(BasicNodeType::StringScalar(m_lexer.GetString()));
                    p_current_node->ToSequence().back().SetVersion(yaml_version);
                    if (needs_anchor_impl)
                    {
                        p_current_node->ToSequence().back().AddAnchorName(anchor_name);
                        anchor_table[anchor_name] = p_current_node->ToSequence().back();
                        needs_anchor_impl = false;
                        anchor_name.clear();
                    }
                    break;
                }
                // a scalar node
                *p_current_node = BasicNodeType::StringScalar(m_lexer.GetString());
                p_current_node->SetVersion(yaml_version);
                if (needs_anchor_impl)
                {
                    p_current_node->AddAnchorName(anchor_name);
                    anchor_table[anchor_name] = *p_current_node;
                    needs_anchor_impl = false;
                    anchor_name.clear();
                }
                p_current_node = node_stack.back();
                node_stack.pop_back();
                break;
            default:                                                 // LCOV_EXCL_LINE
                throw Exception("Unsupported lexical token found."); // LCOV_EXCL_LINE
            }

            type = m_lexer.GetNextToken();
        }

        return root;
    }

private:
    /**
     * @brief Convert a YAML version string to YAMLVersionType object.
     *
     * @param version_str A YAML version string.
     * @return YamlVersionType A converted YAMLVersionType object.
     */
    YamlVersionType ConvertToVersionType(const string_type& version_str) const noexcept
    {
        if (version_str == "1.1")
        {
            return YamlVersionType::VER_1_1;
        }
        return YamlVersionType::VER_1_2;
    }

private:
    lexer_type m_lexer {};
    uint32_t m_current_indent_width = 0;
};

/**
 * @brief default YAML document deserializer.
 */
using Deserializer = BasicDeserializer<>;

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DESERIALIZER_HPP_ */