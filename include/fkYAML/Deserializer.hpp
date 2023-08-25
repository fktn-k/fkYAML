/**
 * @file Deserializer.hpp
 * @brief Implementation of the deserializer for YAML documents.
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#include <cstdint>

#include "fkYAML/Exception.hpp"
#include "fkYAML/LexicalAnalyzer.hpp"
#include "fkYAML/Node.hpp"

#ifndef FK_YAML_DESERIALIZER_HPP_
#define FK_YAML_DESERIALIZER_HPP_

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
namespace fkyaml
{

/**
 * @class Deserializer
 * @brief A class which provides the feature of deserializing YAML documents.
 */
class Deserializer
{
public:
    /**
     * @brief Construct a new Deserializer object.
     */
    Deserializer() = default;

public:
    /**
     * @brief Deserialize a YAML-formatted source string into a YAML node.
     *
     * @param source A YAML-formatted source string.
     * @return Node A root YAML node deserialized from the source string.
     */
    Node Deserialize(const char* const source)
    {
        if (!source)
        {
            throw Exception("The given source for deserialization is nullptr.");
        }

        m_lexer.SetInputBuffer(source);
        Node root = Node::Mapping();
        std::vector<Node*> node_stack;

        Node* p_current_node = &root;

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
                    NodeStringType tmp_str = p_current_node->operator[](0).ToString();
                    p_current_node->operator[](0) = Node::Mapping();
                    node_stack.emplace_back(p_current_node);
                    p_current_node = &(p_current_node->operator[](0));
                    p_current_node->ToMapping().emplace(tmp_str, Node());
                    node_stack.emplace_back(p_current_node);
                    p_current_node = &(p_current_node->operator[](tmp_str));
                }
                break;
            case LexicalTokenType::VALUE_SEPARATOR:
                if (!p_current_node->IsSequence() && !p_current_node->IsMapping())
                {
                    throw Exception("A value separator must appear in a container node.");
                }
                break;
            case LexicalTokenType::ANCHOR_PREFIX:
            case LexicalTokenType::ALIAS_PREFIX:
            case LexicalTokenType::COMMENT_PREFIX:
            case LexicalTokenType::DIRECTIVE_PREFIX:
                break;
            case LexicalTokenType::SEQUENCE_BLOCK_PREFIX:
                if (p_current_node->IsNull())
                {
                    *p_current_node = Node::Sequence();
                    break;
                }
                if (p_current_node->IsMapping())
                {
                    if (p_current_node->IsEmpty())
                    {
                        *p_current_node = Node::Sequence();
                        break;
                    }

                    // for the second or later mapping items in a sequence node.
                    node_stack.back()->ToSequence().emplace_back(Node::Mapping());
                    p_current_node = &(node_stack.back()->ToSequence().back());
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
                *p_current_node = Node::Sequence();
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
                *p_current_node = Node::Mapping();
                break;
            case LexicalTokenType::MAPPING_FLOW_BEGIN:
                if (p_current_node->IsMapping())
                {
                    throw Exception("Cannot assign a mapping value as a key.");
                }
                *p_current_node = Node::Mapping();
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
                }

                break;
            case LexicalTokenType::BOOLEAN_VALUE:
                if (p_current_node->IsMapping())
                {
                    throw Exception("Cannot assign a boolean as a key.");
                }

                if (p_current_node->IsSequence())
                {
                    p_current_node->ToSequence().emplace_back(Node::BooleanScalar(m_lexer.GetBoolean()));
                }
                else // a scalar node
                {
                    *p_current_node = Node::BooleanScalar(m_lexer.GetBoolean());
                    p_current_node = node_stack.back();
                    node_stack.pop_back();
                }
                break;
            case LexicalTokenType::SIGNED_INT_VALUE:
                if (p_current_node->IsMapping())
                {
                    throw Exception("Cannot assign a signed integer as a key.");
                }

                if (p_current_node->IsSequence())
                {
                    p_current_node->ToSequence().emplace_back(Node::SignedIntegerScalar(m_lexer.GetSignedInt()));
                }
                else // a scalar node
                {
                    *p_current_node = Node::SignedIntegerScalar(m_lexer.GetSignedInt());
                    p_current_node = node_stack.back();
                    node_stack.pop_back();
                }
                break;
            case LexicalTokenType::UNSIGNED_INT_VALUE:
                if (p_current_node->IsMapping())
                {
                    throw Exception("Cannot assign an unsigned integer as a key.");
                }

                if (p_current_node->IsSequence())
                {
                    p_current_node->ToSequence().emplace_back(Node::UnsignedIntegerScalar(m_lexer.GetUnsignedInt()));
                }
                else
                {
                    *p_current_node = Node::UnsignedIntegerScalar(m_lexer.GetUnsignedInt());
                    p_current_node = node_stack.back();
                    node_stack.pop_back();
                }
                break;
            case LexicalTokenType::FLOAT_NUMBER_VALUE:
                if (p_current_node->IsMapping())
                {
                    throw Exception("Cannot assign a float number as a key.");
                }

                if (p_current_node->IsSequence())
                {
                    p_current_node->ToSequence().emplace_back(Node::FloatNumberScalar(m_lexer.GetFloatNumber()));
                }
                else // a scalar
                {
                    *p_current_node = Node::FloatNumberScalar(m_lexer.GetFloatNumber());
                    p_current_node = node_stack.back();
                    node_stack.pop_back();
                }
                break;
            case LexicalTokenType::STRING_VALUE:
                if (p_current_node->IsMapping())
                {
                    p_current_node->ToMapping().emplace(m_lexer.GetString(), Node());
                    node_stack.push_back(p_current_node);
                    p_current_node = &(p_current_node->ToMapping().at(m_lexer.GetString()));
                    break;
                }
                if (p_current_node->IsSequence())
                {
                    p_current_node->ToSequence().emplace_back(Node::StringScalar(m_lexer.GetString()));
                    break;
                }
                // a scalar node
                *p_current_node = Node::StringScalar(m_lexer.GetString());
                p_current_node = node_stack.back();
                node_stack.pop_back();
                break;
            default:
                throw Exception("Unsupported lexical token found.");
            }

            type = m_lexer.GetNextToken();
        }

        return root;
    }

private:
    LexicalAnalyzer m_lexer {};
    uint32_t m_current_indent_width = 0;
};

} // namespace fkyaml

#endif /* FK_YAML_DESERIALIZER_HPP_ */