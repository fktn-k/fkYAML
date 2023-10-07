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

#ifndef FK_YAML_SERIALIZER_HPP_
#define FK_YAML_SERIALIZER_HPP_

#include <cmath>
#include <sstream>
#include <string>

#include "fkYAML/VersioningMacros.hpp"
#include "fkYAML/Exception.hpp"
#include "fkYAML/Node.hpp"
#include "fkYAML/NodeType.hpp"
#include "fkYAML/TypeTraits.hpp"

FK_YAML_NAMESPACE_BEGIN

/**
 * @brief A basic implementation of serialization feature for YAML nodes.
 *
 * @tparam BasicNodeType A BasicNode template class instantiation.
 */
template <typename BasicNodeType = node>
class basic_serializer
{
    static_assert(is_basic_node<BasicNodeType>::value, "basic_serializer only accepts (const) BasicNode<...>");

public:
    /**
     * @brief Construct a new basic_serializer object.
     */
    basic_serializer() = default;

    /**
     * @brief Serialize the given Node value.
     *
     * @param node A Node object to be serialized.
     * @return std::string A serialization result of the given Node value.
     */
    std::string serialize(BasicNodeType& node)
    {
        std::string str {};
        serialize_node(node, 0, str);
        return str;
    }

private:
    /**
     * @brief Recursively serialize each Node object.
     *
     * @param node A Node object to be serialized.
     * @param cur_indent The current indent width
     * @param str A string to hold serialization result.
     */
    void serialize_node(BasicNodeType& node, const uint32_t cur_indent, std::string& str)
    {
        switch (node.type())
        {
        case node_t::SEQUENCE:
            for (auto& seq_item : node)
            {
                insert_indentation(cur_indent, str);
                str += "-";
                if (seq_item.is_scalar())
                {
                    str += " ";
                    serialize_node(seq_item, cur_indent, str);
                    str += "\n";
                }
                else
                {
                    str += "\n";
                    serialize_node(seq_item, cur_indent + 2, str);
                }
            }
            break;
        case node_t::MAPPING:
            for (auto itr = node.begin(); itr != node.end(); ++itr)
            {
                insert_indentation(cur_indent, str);
                serialize_key(itr.key(), str);
                if (itr->is_scalar())
                {
                    str += " ";
                    serialize_node(*itr, cur_indent, str);
                    str += "\n";
                }
                else
                {
                    str += "\n";
                    serialize_node(*itr, cur_indent + 2, str);
                }
            }
            break;
        case node_t::NULL_OBJECT:
            str += "null";
            break;
        case node_t::BOOLEAN:
            if (node.to_boolean())
            {
                str += "true";
            }
            else
            {
                str += "false";
            }
            break;
        case node_t::INTEGER:
            str += std::to_string(node.to_integer());
            break;
        case node_t::FLOAT_NUMBER: {
            typename BasicNodeType::float_number_type float_val = node.to_float_number();
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
                ss << node.to_float_number();
                str += ss.str();
            }
            break;
        }
        case node_t::STRING:
            str += node.to_string();
            break;
        default:
            throw fkyaml::exception("Unsupported node type found.");
        }
    }

    /**
     * @brief Serialize mapping keys.
     *
     * @param key A key string to be serialized.
     * @param str A string to hold serialization result.
     */
    void serialize_key(const std::string& key, std::string& str)
    {
        str += key + ":";
    }

    /**
     * @brief Insert indentation to the serialization result.
     *
     * @param cur_indent The current indent width to be inserted.
     * @param str A string to hold serialization result.
     */
    void insert_indentation(const uint32_t cur_indent, std::string& str)
    {
        for (uint32_t i = 0; i < cur_indent; ++i)
        {
            str += " ";
        }
    }
};

/**
 * @brief default YAML node serializer.
 */
using serializer = basic_serializer<>;

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_SERIALIZER_HPP_ */
