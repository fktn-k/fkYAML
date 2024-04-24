///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.4
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
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
#include <fkYAML/detail/types/yaml_version_t.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief A basic implementation of serialization feature for YAML nodes.
/// @tparam BasicNodeType A BasicNode template class instantiation.
template <typename BasicNodeType>
class basic_serializer {
    static_assert(detail::is_basic_node<BasicNodeType>::value, "basic_serializer only accepts basic_node<...>");

public:
    /// @brief Construct a new basic_serializer object.
    basic_serializer() = default;

    /// @brief Serialize the given Node value.
    /// @param node A Node object to be serialized.
    /// @return std::string A serialization result of the given Node value.
    std::string serialize(const BasicNodeType& node) {
        std::string str {};
        serialize_directives(node, str);
        serialize_node(node, 0, str);
        return str;
    } // LCOV_EXCL_LINE

private:
    /// @brief Serialize the directives if any is applied to the node.
    /// @param node The targe node.
    /// @param str A string to hold serialization result.
    void serialize_directives(const BasicNodeType& node, std::string& str) {
        if (!node.mp_directive_set) {
            return;
        }

        const auto& directives = node.mp_directive_set;

        if (directives->is_version_specified) {
            str += "%YAML ";
            switch (directives->version) {
            case yaml_version_t::VER_1_1:
                str += "1.1\n";
                break;
            case yaml_version_t::VER_1_2:
                str += "1.2\n";
                break;
            }
        }

        if (!directives->primary_handle_prefix.empty()) {
            str += "%TAG ! ";
            str += directives->primary_handle_prefix;
            str += "\n";
        }

        if (!directives->secondary_handle_prefix.empty()) {
            str += "%TAG !! ";
            str += directives->secondary_handle_prefix;
            str += "\n";
        }

        if (!directives->named_handle_map.empty()) {
            for (const auto& itr : directives->named_handle_map) {
                str += "%TAG ";
                str += itr.first;
                str += " ";
                str += itr.second;
                str += "\n";
            }
        }

        str += "---\n";
    }

    /// @brief Recursively serialize each Node object.
    /// @param node A Node object to be serialized.
    /// @param cur_indent The current indent width
    /// @param str A string to hold serialization result.
    void serialize_node(const BasicNodeType& node, const uint32_t cur_indent, std::string& str) {
        switch (node.type()) {
        case node_t::SEQUENCE:
            for (const auto& seq_item : node) {
                insert_indentation(cur_indent, str);
                str += "-";

                bool is_appended = try_append_alias(seq_item, true, str);
                if (is_appended) {
                    str += "\n";
                    continue;
                }

                try_append_anchor(seq_item, true, str);
                try_append_tag(seq_item, true, str);

                bool is_scalar = seq_item.is_scalar();
                if (is_scalar) {
                    str += " ";
                    serialize_node(seq_item, cur_indent, str);
                    str += "\n";
                }
                else {
                    str += "\n";
                    serialize_node(seq_item, cur_indent + 2, str);
                }
            }
            break;
        case node_t::MAPPING:
            for (auto itr = node.begin(); itr != node.end(); ++itr) {
                insert_indentation(cur_indent, str);

                bool is_appended = try_append_alias(itr.key(), false, str);
                if (!is_appended) {
                    bool is_anchor_appended = try_append_anchor(itr.key(), false, str);
                    bool is_tag_appended = try_append_tag(itr.key(), is_anchor_appended, str);
                    if (is_anchor_appended || is_tag_appended) {
                        str += " ";
                    }

                    bool is_container = !itr.key().is_scalar();
                    if (is_container) {
                        str += "? ";
                    }
                    uint32_t indent = static_cast<uint32_t>(get_cur_indent(str));
                    serialize_node(itr.key(), indent, str);
                    if (is_container) {
                        // a newline code is already inserted in the above serialize_node() call.
                        insert_indentation(indent - 2, str);
                    }
                }

                str += ":";

                is_appended = try_append_alias(*itr, true, str);
                if (is_appended) {
                    str += "\n";
                    continue;
                }

                try_append_anchor(*itr, true, str);
                try_append_tag(*itr, true, str);

                bool is_scalar = itr->is_scalar();
                if (is_scalar) {
                    str += " ";
                    serialize_node(*itr, cur_indent, str);
                    str += "\n";
                }
                else {
                    str += "\n";
                    serialize_node(*itr, cur_indent + 2, str);
                }
            }
            break;
        case node_t::NULL_OBJECT:
            to_string(nullptr, m_tmp_str_buff);
            str += m_tmp_str_buff;
            break;
        case node_t::BOOLEAN:
            to_string(node.template get_value<typename BasicNodeType::boolean_type>(), m_tmp_str_buff);
            str += m_tmp_str_buff;
            break;
        case node_t::INTEGER:
            to_string(node.template get_value<typename BasicNodeType::integer_type>(), m_tmp_str_buff);
            str += m_tmp_str_buff;
            break;
        case node_t::FLOAT_NUMBER:
            to_string(node.template get_value<typename BasicNodeType::float_number_type>(), m_tmp_str_buff);
            str += m_tmp_str_buff;
            break;
        case node_t::STRING: {
            bool is_escaped = false;
            typename BasicNodeType::string_type str_val = get_string_node_value(node, is_escaped);

            if (is_escaped) {
                // There's no other token type with escapes than strings.
                // Also, escapes must be in double-quoted strings.
                str += '\"';
                str += str_val;
                str += '\"';
                break;
            }

            auto adapter = input_adapter(str_val);
            lexical_analyzer<BasicNodeType> lexer(std::move(adapter));
            lexical_token_t token_type = lexer.get_next_token();

            if (token_type != lexical_token_t::STRING_VALUE) {
                // Surround a string value with double quotes to keep semantic equality.
                // Without them, serialized values will become non-string. (e.g., "1" -> 1)
                str += '\"';
                str += str_val;
                str += '\"';
            }
            else {
                str += str_val;
            }
            break;
        }
        }
    }

    /// @brief Get the current indentation width.
    /// @param s The target string object.
    /// @return The current indentation width.
    std::size_t get_cur_indent(const std::string& s) const noexcept {
        bool is_empty = s.empty();
        if (is_empty) {
            return 0;
        }

        std::size_t last_lf_pos = s.rfind('\n');
        return (last_lf_pos != std::string::npos) ? s.size() - last_lf_pos - 1 : s.size();
    }

    /// @brief Insert indentation to the serialization result.
    /// @param indent The indent width to be inserted.
    /// @param str A string to hold serialization result.
    void insert_indentation(const uint32_t indent, std::string& str) const noexcept {
        if (indent == 0) {
            return;
        }

        str.append(indent - get_cur_indent(str), ' ');
    }

    /// @brief Append an anchor property if it's available. Do nothing otherwise.
    /// @param node The target node which is possibly an anchor node.
    /// @param prepends_space Whether or not to prepend a space before an anchor property.
    /// @param str A string to hold serialization result.
    /// @return true if an anchor property has been appended, false otherwise.
    bool try_append_anchor(const BasicNodeType& node, bool prepends_space, std::string& str) const {
        if (node.is_anchor()) {
            if (prepends_space) {
                str += " ";
            }
            str += "&" + node.get_anchor_name();
            return true;
        }
        return false;
    }

    /// @brief Append an alias property if it's available. Do nothing otherwise.
    /// @param node The target node which is possibly an alias node.
    /// @param prepends_space Whether or not to prepend a space before an alias property.
    /// @param str A string to hold serialization result.
    /// @return true if an alias property has been appended, false otherwise.
    bool try_append_alias(const BasicNodeType& node, bool prepends_space, std::string& str) const {
        if (node.is_alias()) {
            if (prepends_space) {
                str += " ";
            }
            str += "*" + node.get_anchor_name();
            return true;
        }
        return false;
    }

    /// @brief Append a tag name if it's available. Do nothing otherwise.
    /// @param[in] node The target node which possibly has a tag name.
    /// @param[out] str A string to hold serialization result.
    /// @return true if a tag name has been appended, false otherwise.
    bool try_append_tag(const BasicNodeType& node, bool prepends_space, std::string& str) const {
        if (node.has_tag_name()) {
            if (prepends_space) {
                str += " ";
            }
            str += node.get_tag_name();
            return true;
        }
        return false;
    }

    /// @brief Get a string value from the given node and, if necessary, escape its contents.
    /// @param[in] node The target string YAML node.
    /// @param[out] is_escaped Whether or not the contents of an ouput string has been escaped.
    /// @return The (escaped) string node value.
    typename BasicNodeType::string_type get_string_node_value(const BasicNodeType& node, bool& is_escaped) {
        FK_YAML_ASSERT(node.is_string());

        using string_type = typename BasicNodeType::string_type;

        // Check if the string value contains a character needed to be escaped on output.
        const string_type& s = node.template get_value_ref<const string_type&>();
        size_t size = s.size();
        string_type escaped {};
        escaped.reserve(size);

        for (size_t i = 0; i < size; i++) {
            switch (s[i]) {
            case 0x01:
                escaped += "\\u0001";
                is_escaped = true;
                break;
            case 0x02:
                escaped += "\\u0002";
                is_escaped = true;
                break;
            case 0x03:
                escaped += "\\u0003";
                is_escaped = true;
                break;
            case 0x04:
                escaped += "\\u0004";
                is_escaped = true;
                break;
            case 0x05:
                escaped += "\\u0005";
                is_escaped = true;
                break;
            case 0x06:
                escaped += "\\u0006";
                is_escaped = true;
                break;
            case '\a':
                escaped += "\\a";
                is_escaped = true;
                break;
            case '\b':
                escaped += "\\b";
                is_escaped = true;
                break;
            case '\t':
                escaped += "\\t";
                is_escaped = true;
                break;
            case '\n':
                escaped += "\\n";
                is_escaped = true;
                break;
            case '\v':
                escaped += "\\v";
                is_escaped = true;
                break;
            case '\f':
                escaped += "\\f";
                is_escaped = true;
                break;
            case '\r':
                escaped += "\\r";
                is_escaped = true;
                break;
            case 0x0E:
                escaped += "\\u000E";
                is_escaped = true;
                break;
            case 0x0F:
                escaped += "\\u000F";
                is_escaped = true;
                break;
            case 0x10:
                escaped += "\\u0010";
                is_escaped = true;
                break;
            case 0x11:
                escaped += "\\u0011";
                is_escaped = true;
                break;
            case 0x12:
                escaped += "\\u0012";
                is_escaped = true;
                break;
            case 0x13:
                escaped += "\\u0013";
                is_escaped = true;
                break;
            case 0x14:
                escaped += "\\u0014";
                is_escaped = true;
                break;
            case 0x15:
                escaped += "\\u0015";
                is_escaped = true;
                break;
            case 0x16:
                escaped += "\\u0016";
                is_escaped = true;
                break;
            case 0x17:
                escaped += "\\u0017";
                is_escaped = true;
                break;
            case 0x18:
                escaped += "\\u0018";
                is_escaped = true;
                break;
            case 0x19:
                escaped += "\\u0019";
                is_escaped = true;
                break;
            case 0x1A:
                escaped += "\\u001A";
                is_escaped = true;
                break;
            case 0x1B:
                escaped += "\\e";
                is_escaped = true;
                break;
            case 0x1C:
                escaped += "\\u001C";
                is_escaped = true;
                break;
            case 0x1D:
                escaped += "\\u001D";
                is_escaped = true;
                break;
            case 0x1E:
                escaped += "\\u001E";
                is_escaped = true;
                break;
            case 0x1F:
                escaped += "\\u001F";
                is_escaped = true;
                break;
            case '\"':
                escaped += "\\\"";
                is_escaped = true;
                break;
            case '\\':
                escaped += "\\\\";
                is_escaped = true;
                break;
            default:
                if (i + 1 < size && s[i] == char(0xC2u) && s[i + 1] == char(0x85u)) {
                    escaped += "\\N";
                    i++;
                    is_escaped = true;
                    break;
                }
                if (i + 1 < size && s[i] == char(0xC2u) && s[i + 1] == char(0xA0u)) {
                    escaped += "\\_";
                    i++;
                    is_escaped = true;
                    break;
                }
                if (i + 2 < size && s[i] == char(0xE2u) && s[i + 1] == char(0x80u) && s[i + 2] == char(0xA8u)) {
                    escaped += "\\L";
                    i += 2;
                    is_escaped = true;
                    break;
                }
                if (i + 2 < size && s[i] == char(0xE2u) && s[i + 1] == char(0x80u) && s[i + 2] == char(0xA9u)) {
                    escaped += "\\P";
                    i += 2;
                    is_escaped = true;
                    break;
                }
                escaped += s[i];
                break;
            }
        }

        return escaped;
    } // LCOV_EXCL_LINE

private:
    /// A temporal buffer for conversion from a scalar to a string.
    std::string m_tmp_str_buff;
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP_ */
