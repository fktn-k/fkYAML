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
        Node root;

        Node* p_current_node = &root;
        Node* p_parent_node = p_current_node; // NOLINT(clang-analyzer-deadcode.DeadStores)

        LexicalTokenType type = m_lexer.GetNextToken();
        while (type != LexicalTokenType::END_OF_BUFFER)
        {
            switch (type)
            {
            case LexicalTokenType::KEY_SEPARATOR:
            case LexicalTokenType::VALUE_SEPARATOR:
            case LexicalTokenType::ANCHOR_PREFIX:
            case LexicalTokenType::ALIAS_PREFIX:
            case LexicalTokenType::COMMENT_PREFIX:
            case LexicalTokenType::DIRECTIVE_PREFIX:
            case LexicalTokenType::SEQUENCE_BLOCK_PREFIX:
            case LexicalTokenType::SEQUENCE_FLOW_BEGIN:
            case LexicalTokenType::SEQUENCE_FLOW_END:
            case LexicalTokenType::MAPPING_FLOW_BEGIN:
            case LexicalTokenType::MAPPING_FLOW_END:
                break;
            case LexicalTokenType::NULL_VALUE:
                m_lexer.GetNull();
                break;
            case LexicalTokenType::BOOLEAN_VALUE:
                *p_current_node = Node::BooleanScalar(m_lexer.GetBoolean());
                break;
            case LexicalTokenType::SIGNED_INT_VALUE:
                *p_current_node = Node::SignedIntegerScalar(m_lexer.GetSignedInt());
                break;
            case LexicalTokenType::UNSIGNED_INT_VALUE:
                *p_current_node = Node::UnsignedIntegerScalar(m_lexer.GetUnsignedInt());
                break;
            case LexicalTokenType::FLOAT_NUMBER_VALUE:
                *p_current_node = Node::FloatNumberScalar(m_lexer.GetFloatNumber());
                break;
            case LexicalTokenType::STRING_VALUE:
                *p_current_node = Node::StringScalar(m_lexer.GetString());
                break;
            default:
                break;
            }

            type = m_lexer.GetNextToken();
        }

        return std::move(root);
    }

private:
    LexicalAnalyzer m_lexer {};
    uint32_t m_current_indent_width = 0;
};

} // namespace fkyaml

#endif /* FK_YAML_DESERIALIZER_HPP_ */