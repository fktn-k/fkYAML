/**
 *   __ _  __     __      __  __ _
 *  / _| | \ \   / //\   |  \/  | |
 * | |_| | _\ \_/ //  \  | \  / | |
 * |  _| |/ /\   // /\ \ | |\/| | |
 * | | |   <  | |/ ____ \| |  | | |____
 * |_| |_|\_\ |_/_/    \_\_|  |_|______|
 *
 * @file Deserializer.hpp
 * @brief Implementation of the serializer for YAML nodes.
 * @version 0.0.0
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_SERIALIZER_HPP_
#define FK_YAML_SERIALIZER_HPP_

#include <cmath>
#include <sstream>
#include <string>

#include "fkYAML/VersioningMacros.hpp"
#include "fkYAML/Exception.hpp"
#include "fkYAML/Node.hpp"
#include "fkYAML/NodeType.hpp"
#include "fkYAML/NodeTypeTraits.hpp"

FK_YAML_NAMESPACE_BEGIN

template <typename BasicNodeType = Node>
class BasicSerializer
{
    static_assert(IsBasicNode<BasicNodeType>::value, "BasicSerializer only accepts (const) BasicNode<...>");

public:
    /**
     * @brief Construct a new BasicSerializer object.
     */
    BasicSerializer() = default;

    std::string Serialize(BasicNodeType& node)
    {
        std::string str {};
        SerializeNode(node, 0, str);
        return str;
    }

private:
    void SerializeNode(BasicNodeType& node, const uint32_t cur_indent, std::string& str)
    {
        switch (node.Type())
        {
        case NodeType::SEQUENCE:
            for (auto& seq_item : node)
            {
                InsertIndentation(cur_indent, str);
                str += "-";
                if (seq_item.IsScalar())
                {
                    str += " ";
                    SerializeNode(seq_item, cur_indent, str);
                    str += "\n";
                }
                else
                {
                    str += "\n";
                    SerializeNode(seq_item, cur_indent + 2, str);
                }
            }
            break;
        case NodeType::MAPPING:
            for (auto itr = node.Begin(); itr != node.End(); ++itr)
            {
                InsertIndentation(cur_indent, str);
                SerializeKey(itr.Key(), str);
                if (itr->IsScalar())
                {
                    str += " ";
                    SerializeNode(*itr, cur_indent, str);
                    str += "\n";
                }
                else
                {
                    str += "\n";
                    SerializeNode(*itr, cur_indent + 2, str);
                }
            }
            break;
        case NodeType::NULL_OBJECT:
            str += "null";
            break;
        case NodeType::BOOLEAN:
            if (node.ToBoolean())
            {
                str += "true";
            }
            else
            {
                str += "false";
            }
            break;
        case NodeType::INTEGER:
            str += std::to_string(node.ToInteger());
            break;
        case NodeType::FLOAT_NUMBER: {
            typename BasicNodeType::float_number_type float_val = node.ToFloatNumber();
            if (std::isnan(float_val))
            {
                str += ".nan";
            }
            else if (std::isinf(float_val))
            {
                if (float_val == std::numeric_limits<typename BasicNodeType::float_number_type>::infinity())
                {
                    str += ".inf";
                }
                else
                {
                    str += "-.inf";
                }
            }
            else
            {
                std::stringstream ss;
                ss << node.ToFloatNumber();
                str += ss.str();
            }
            break;
        }
        case NodeType::STRING:
            str += node.ToString();
            break;
        default:
            throw Exception("Unsupported node type found.");
        }
    }

    void SerializeKey(const std::string& key, std::string& str)
    {
        str += key + ":";
    }

    void InsertIndentation(const uint32_t cur_indent, std::string& str)
    {
        for (uint32_t i = 0; i < cur_indent; ++i)
        {
            str += " ";
        }
    }
};

using Serializer = BasicSerializer<>;

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_SERIALIZER_HPP_ */
