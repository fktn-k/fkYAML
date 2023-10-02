#include <cstdint>
#include <unordered_map>

#include "fkYAML/VersioningMacros.hpp"
#include "fkYAML/Exception.hpp"
#include "fkYAML/LexicalAnalyzer.hpp"
#include "fkYAML/Node.hpp"
#include "fkYAML/TypeTraits.hpp"

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
        m_current_node = &root;

        LexicalTokenType type = m_lexer.GetNextToken();
        while (type != LexicalTokenType::END_OF_BUFFER)
        {
            switch (type)
            {
            case LexicalTokenType::KEY_SEPARATOR:
                if (m_node_stack.empty() || !m_node_stack.back()->IsMapping())
                {
                    throw Exception("A key separator found while a value token is expected.");
                }
                if (m_current_node->IsSequence() && m_current_node->Size() == 1)
                {
                    // make sequence node to mapping node.
                    string_type tmp_str = m_current_node->operator[](0).ToString();
                    m_current_node->operator[](0) = BasicNodeType::Mapping();
                    m_node_stack.emplace_back(m_current_node);
                    m_current_node = &(m_current_node->operator[](0));
                    SetYamlVersion(*m_current_node);
                    m_current_node->ToMapping().emplace(tmp_str, BasicNodeType());
                    m_node_stack.emplace_back(m_current_node);
                    m_current_node = &(m_current_node->operator[](tmp_str));
                    SetYamlVersion(*m_current_node);
                }
                break;
            case LexicalTokenType::VALUE_SEPARATOR:
                if (!m_current_node->IsSequence() && !m_current_node->IsMapping())
                {
                    throw Exception("A value separator must appear in a container node.");
                }
                break;
            case LexicalTokenType::ANCHOR_PREFIX: {
                if (m_current_node->IsMapping())
                {
                    throw Exception("A mapping node cannot be an anchor.");
                }
                m_anchor_name = m_lexer.GetString();
                m_needs_anchor_impl = true;
                break;
            }
            case LexicalTokenType::ALIAS_PREFIX: {
                if (m_current_node->IsMapping())
                {
                    throw Exception("Cannot apply alias to a mapping node.");
                }
                m_anchor_name = m_lexer.GetString();
                if (m_anchor_table.find(m_anchor_name) == m_anchor_table.end())
                {
                    throw Exception("The given anchor name must appear prior to the alias node.");
                }
                AssignNodeValue(BasicNodeType::AliasOf(m_anchor_table.at(m_anchor_name)));
                break;
            }
            case LexicalTokenType::COMMENT_PREFIX:
                break;
            case LexicalTokenType::YAML_VER_DIRECTIVE: {
                FK_YAML_ASSERT(m_current_node == &root);
                UpdateYamlVersionFrom(m_lexer.GetYamlVersion());
                SetYamlVersion(*m_current_node);
                break;
            }
            case LexicalTokenType::TAG_DIRECTIVE:
                // TODO: implement tag directive deserialization.
            case LexicalTokenType::INVALID_DIRECTIVE:
                // TODO: should output a warning log. Currently just ignore this case.
                break;
            case LexicalTokenType::SEQUENCE_BLOCK_PREFIX:
                if (m_current_node->IsMapping())
                {
                    if (m_current_node->IsEmpty())
                    {
                        *m_current_node = BasicNodeType::Sequence();
                        break;
                    }

                    // for the second or later mapping items in a sequence node.
                    m_node_stack.back()->ToSequence().emplace_back(BasicNodeType::Mapping());
                    m_current_node = &(m_node_stack.back()->ToSequence().back());
                    SetYamlVersion(*m_current_node);
                    break;
                }
                break;
            case LexicalTokenType::SEQUENCE_FLOW_BEGIN:
                if (m_current_node->IsMapping())
                {
                    throw Exception("Cannot assign a sequence value as a key.");
                }
                *m_current_node = BasicNodeType::Sequence();
                SetYamlVersion(*m_current_node);
                break;
            case LexicalTokenType::SEQUENCE_FLOW_END:
                if (!m_current_node->IsSequence())
                {
                    throw Exception("Invalid sequence flow ending found.");
                }
                m_current_node = m_node_stack.back();
                m_node_stack.pop_back();
                break;
            case LexicalTokenType::MAPPING_BLOCK_PREFIX:
                *m_current_node = BasicNodeType::Mapping();
                SetYamlVersion(*m_current_node);
                break;
            case LexicalTokenType::MAPPING_FLOW_BEGIN:
                if (m_current_node->IsMapping())
                {
                    throw Exception("Cannot assign a mapping value as a key.");
                }
                *m_current_node = BasicNodeType::Mapping();
                SetYamlVersion(*m_current_node);
                break;
            case LexicalTokenType::MAPPING_FLOW_END:
                if (!m_current_node->IsMapping())
                {
                    throw Exception("Invalid mapping flow ending found.");
                }
                m_current_node = m_node_stack.back();
                m_node_stack.pop_back();
                break;
            case LexicalTokenType::NULL_VALUE:
                if (m_current_node->IsMapping())
                {
                    AddNewKey(m_lexer.GetString());
                    break;
                }

                // Just make sure that the actual value is really a null value.
                m_lexer.GetNull();
                AssignNodeValue(BasicNodeType());
                break;
            case LexicalTokenType::BOOLEAN_VALUE:
                if (m_current_node->IsMapping())
                {
                    AddNewKey(m_lexer.GetString());
                    break;
                }
                AssignNodeValue(BasicNodeType::BooleanScalar(m_lexer.GetBoolean()));
                break;
            case LexicalTokenType::INTEGER_VALUE:
                if (m_current_node->IsMapping())
                {
                    AddNewKey(m_lexer.GetString());
                    break;
                }
                AssignNodeValue(BasicNodeType::IntegerScalar(m_lexer.GetInteger()));
                break;
            case LexicalTokenType::FLOAT_NUMBER_VALUE:
                if (m_current_node->IsMapping())
                {
                    AddNewKey(m_lexer.GetString());
                    break;
                }
                AssignNodeValue(BasicNodeType::FloatNumberScalar(m_lexer.GetFloatNumber()));
                break;
            case LexicalTokenType::STRING_VALUE:
                if (m_current_node->IsMapping())
                {
                    AddNewKey(m_lexer.GetString());
                    break;
                }
                AssignNodeValue(BasicNodeType::StringScalar(m_lexer.GetString()));
                break;
            default:                                                 // LCOV_EXCL_LINE
                throw Exception("Unsupported lexical token found."); // LCOV_EXCL_LINE
            }

            type = m_lexer.GetNextToken();
        }

        m_current_node = nullptr;
        m_needs_anchor_impl = false;
        m_anchor_table.clear();
        m_node_stack.clear();

        return root;
    }

private:
    /**
     * @brief Add new key string to the current YAML node.
     *
     * @param key a key string to be added to the current YAML node.
     */
    void AddNewKey(const string_type& key) noexcept
    {
        m_current_node->ToMapping().emplace(key, BasicNodeType());
        m_node_stack.push_back(m_current_node);
        m_current_node = &(m_current_node->ToMapping().at(key));
    }

    /**
     * @brief Assign node value to the current node.
     *
     * @param node_value A rvalue BasicNodeType object to be assigned to the current node.
     */
    void AssignNodeValue(BasicNodeType&& node_value) noexcept
    {
        if (m_current_node->IsSequence())
        {
            m_current_node->ToSequence().emplace_back(std::move(node_value));
            SetYamlVersion(m_current_node->ToSequence().back());
            if (m_needs_anchor_impl)
            {
                m_current_node->ToSequence().back().AddAnchorName(m_anchor_name);
                m_anchor_table[m_anchor_name] = m_current_node->ToSequence().back();
                m_needs_anchor_impl = false;
                m_anchor_name.clear();
            }
            return;
        }

        // a scalar node
        *m_current_node = std::move(node_value);
        SetYamlVersion(*m_current_node);
        if (m_needs_anchor_impl)
        {
            m_current_node->AddAnchorName(m_anchor_name);
            m_anchor_table[m_anchor_name] = *m_current_node;
            m_needs_anchor_impl = false;
            m_anchor_name.clear();
        }
        m_current_node = m_node_stack.back();
        m_node_stack.pop_back();
    }

    /**
     * @brief Set the YamlVersionType object to the given node.
     *
     * @param node A BasicNodeType object to be set the YamlVersionType object.
     */
    void SetYamlVersion(BasicNodeType& node) noexcept
    {
        node.SetVersion(m_yaml_version);
    }

    /**
     * @brief Update the target YAML version with an input string.
     *
     * @param version_str A YAML version string.
     */
    void UpdateYamlVersionFrom(const string_type& version_str) noexcept
    {
        if (version_str == "1.1")
        {
            m_yaml_version = YamlVersionType::VER_1_1;
            return;
        }
        m_yaml_version = YamlVersionType::VER_1_2;
    }

private:
    lexer_type m_lexer {};                                     /** A lexical analyzer object. */
    BasicNodeType* m_current_node = nullptr;                   /** The currently focused YAML node. */
    std::vector<BasicNodeType*> m_node_stack;                  /** The stack of YAML nodes. */
    YamlVersionType m_yaml_version = YamlVersionType::VER_1_2; /** The YAML version specification type. */
    uint32_t m_current_indent_width = 0;                       /** The current indentation width. */
    bool m_needs_anchor_impl = false; /** A flag to determine the need for YAML anchor node implementation */
    string_type m_anchor_name {};     /** The last YAML anchor name. */
    std::unordered_map<std::string, BasicNodeType> m_anchor_table; /** The table of YAML anchor nodes. */
};

/**
 * @brief default YAML document deserializer.
 */
using Deserializer = BasicDeserializer<>;

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DESERIALIZER_HPP_ */
