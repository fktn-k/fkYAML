///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.2.1
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP_
#define FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP_

#include <cmath>
#include <sstream>
#include <string>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/conversions/to_string.hpp>
#include <fkYAML/detail/input/input_adapter.hpp>
#include <fkYAML/detail/input/lexical_analyzer.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/types/node_t.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_NAMESPACE_BEGIN

/// @brief namespace for internal implementations of fkYAML library.
namespace detail
{

/// @brief A basic implementation of serialization feature for YAML nodes.
/// @tparam BasicNodeType A BasicNode template class instantiation.
template <typename BasicNodeType>
class basic_serializer
{
    static_assert(detail::is_basic_node<BasicNodeType>::value, "basic_serializer only accepts basic_node<...>");

public:
    /// @brief Construct a new basic_serializer object.
    basic_serializer() = default;

    /// @brief Serialize the given Node value.
    /// @param node A Node object to be serialized.
    /// @return std::string A serialization result of the given Node value.
    std::string serialize(const BasicNodeType& node)
    {
        std::string str {};
        serialize_node(node, 0, str);
        return str;
    } // LCOV_EXCL_LINE

private:
    /// @brief Recursively serialize each Node object.
    /// @param node A Node object to be serialized.
    /// @param cur_indent The current indent width
    /// @param str A string to hold serialization result.
    void serialize_node(const BasicNodeType& node, const uint32_t cur_indent, std::string& str)
    {
        switch (node.type())
        {
        case node_t::SEQUENCE:
            for (const auto& seq_item : node)
            {
                insert_indentation(cur_indent, str);
                str += "-";
                bool is_scalar = seq_item.is_scalar();
                if (is_scalar)
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
                bool is_scalar = itr->is_scalar();
                if (is_scalar)
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
            to_string(m_tmp_str_buff, nullptr);
            str += m_tmp_str_buff;
            break;
        case node_t::BOOLEAN:
            to_string(m_tmp_str_buff, node.template get_value<typename BasicNodeType::boolean_type>());
            str += m_tmp_str_buff;
            break;
        case node_t::INTEGER:
            to_string(m_tmp_str_buff, node.template get_value<typename BasicNodeType::integer_type>());
            str += m_tmp_str_buff;
            break;
        case node_t::FLOAT_NUMBER:
            to_string(m_tmp_str_buff, node.template get_value<typename BasicNodeType::float_number_type>());
            str += m_tmp_str_buff;
            break;
        case node_t::STRING: {
            using string_type = typename BasicNodeType::string_type;

            // Check if the string value contains a character needed to be escaped on output.
            const string_type& s = node.template get_value_ref<const string_type&>();
            string_type escaped;
            bool has_escape = false;
            size_t size = s.size();
            for (size_t i = 0; i < size; i++)
            {
                switch (s[i])
                {
                case '\a':
                    escaped += "\\a";
                    has_escape = true;
                    break;
                case '\b':
                    escaped += "\\b";
                    has_escape = true;
                    break;
                case '\t':
                    escaped += "\\t";
                    has_escape = true;
                    break;
                case '\n':
                    escaped += "\\n";
                    has_escape = true;
                    break;
                case '\v':
                    escaped += "\\v";
                    has_escape = true;
                    break;
                case '\f':
                    escaped += "\\f";
                    has_escape = true;
                    break;
                case '\r':
                    escaped += "\\r";
                    has_escape = true;
                    break;
                case 0x1B:
                    escaped += "\\e";
                    has_escape = true;
                    break;
                case '\"':
                    escaped += "\\\"";
                    has_escape = true;
                    break;
                case '\\':
                    escaped += "\\\\";
                    has_escape = true;
                    break;
                default:
                    if (i + 1 < size && s[i] == char(0xC2u) && s[i + 1] == char(0x85u))
                    {
                        escaped += "\\N";
                        i++;
                        has_escape = true;
                        break;
                    }
                    if (i + 1 < size && s[i] == char(0xC2u) && s[i + 1] == char(0xA0u))
                    {
                        escaped += "\\_";
                        i++;
                        has_escape = true;
                        break;
                    }
                    if (i + 2 < size && s[i] == char(0xE2u) && s[i + 1] == char(0x80u) && s[i + 2] == char(0xA8u))
                    {
                        escaped += "\\L";
                        i += 2;
                        has_escape = true;
                        break;
                    }
                    if (i + 2 < size && s[i] == char(0xE2u) && s[i + 1] == char(0x80u) && s[i + 2] == char(0xA9u))
                    {
                        escaped += "\\P";
                        i += 2;
                        has_escape = true;
                        break;
                    }
                    escaped += s[i];
                    break;
                }
            }

            if (has_escape)
            {
                // There's no other token type with escapes than strings.
                // Also, escapes must be in double-quoted strings.
                str += '\"';
                str += escaped;
                str += '\"';
                break;
            }

            auto adapter = input_adapter(s);
            lexical_analyzer<BasicNodeType, decltype(adapter)> lexer(std::move(adapter));
            lexical_token_t token_type = lexer.get_next_token();

            if (token_type != lexical_token_t::STRING_VALUE)
            {
                // Surround a string value with double quotes to keep semantic equality.
                // Without them, serialized values will become non-string. (e.g., "1" -> 1)
                str += '\"';
                str += s;
                str += '\"';
            }
            else
            {
                str += s;
            }
            break;
        }
        default:                                                     // LCOV_EXCL_LINE
            throw fkyaml::exception("Unsupported node type found."); // LCOV_EXCL_LINE
        }
    }

    /// @brief Serialize mapping keys.
    /// @param key A key string to be serialized.
    /// @param str A string to hold serialization result.
    void serialize_key(const std::string& key, std::string& str)
    {
        str += key + ":";
    }

    /// @brief Insert indentation to the serialization result.
    /// @param cur_indent The current indent width to be inserted.
    /// @param str A string to hold serialization result.
    void insert_indentation(const uint32_t cur_indent, std::string& str)
    {
        for (uint32_t i = 0; i < cur_indent; ++i)
        {
            str += " ";
        }
    }

private:
    /// A temporal buffer for conversion from a scalar to a string.
    std::string m_tmp_str_buff;
};

} // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP_ */
