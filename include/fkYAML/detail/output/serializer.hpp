//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP
#define FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP

#include <cmath>
#include <sstream>
#include <string>
#include <vector>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/conversions/to_string.hpp>
#include <fkYAML/detail/encodings/yaml_escaper.hpp>
#include <fkYAML/detail/input/scalar_scanner.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/exception.hpp>
#include <fkYAML/node_type.hpp>
#include <fkYAML/yaml_version_type.hpp>

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
        serialize_document(node, str);
        return str;
    } // LCOV_EXCL_LINE

    std::string serialize_docs(const std::vector<BasicNodeType>& docs) {
        std::string str {};

        const auto size = static_cast<uint32_t>(docs.size());
        for (uint32_t i = 0; i < size; i++) {
            serialize_document(docs[i], str);
            if (i + 1 < size) {
                // Append the end-of-document marker for the next document.
                str += "...\n";
            }
        }

        return str;
    } // LCOV_EXCL_LINE

private:
    void serialize_document(const BasicNodeType& node, std::string& str) {
        const bool dirs_serialized = serialize_directives(node, str);

        // the root node cannot be an alias node.
        const bool root_has_props = node.is_anchor() || node.has_tag_name();

        if (root_has_props) {
            if (dirs_serialized) {
                str.back() = ' '; // replace the last LF with a white space
            }
            bool is_anchor_appended = try_append_anchor(node, false, str);
            try_append_tag(node, is_anchor_appended, str);
            str += "\n";
        }
        serialize_node(node, 0, str);
    }

    /// @brief Serialize the directives if any is applied to the node.
    /// @param node The target node.
    /// @param str A string to hold serialization result.
    /// @return bool true if any directive is serialized, false otherwise.
    bool serialize_directives(const BasicNodeType& node, std::string& str) {
        const auto& p_meta = node.mp_meta;
        bool needs_directive_end = false;

        if (p_meta->is_version_specified) {
            str += "%YAML ";
            switch (p_meta->version) {
            case yaml_version_type::VERSION_1_1:
                str += "1.1\n";
                break;
            case yaml_version_type::VERSION_1_2:
                str += "1.2\n";
                break;
            }
            needs_directive_end = true;
        }

        if (!p_meta->primary_handle_prefix.empty()) {
            str += "%TAG ! ";
            str += p_meta->primary_handle_prefix;
            str += "\n";
            needs_directive_end = true;
        }

        if (!p_meta->secondary_handle_prefix.empty()) {
            str += "%TAG !! ";
            str += p_meta->secondary_handle_prefix;
            str += "\n";
            needs_directive_end = true;
        }

        if (!p_meta->named_handle_map.empty()) {
            for (const auto& itr : p_meta->named_handle_map) {
                str += "%TAG ";
                str += itr.first;
                str += " ";
                str += itr.second;
                str += "\n";
            }
            needs_directive_end = true;
        }

        if (needs_directive_end) {
            str += "---\n";
        }

        return needs_directive_end;
    }

    /// @brief Recursively serialize each Node object.
    /// @param node A Node object to be serialized.
    /// @param cur_indent The current indent width
    /// @param str A string to hold serialization result.
    void serialize_node(const BasicNodeType& node, const uint32_t cur_indent, std::string& str) {
        switch (node.get_type()) {
        case node_type::SEQUENCE:
            if (node.size() == 0) {
                str += "[]\n";
                return;
            }
            for (const auto& seq_item : node) {
                insert_indentation(cur_indent, str);
                str += "-";

                const bool is_appended = try_append_alias(seq_item, true, str);
                if (is_appended) {
                    str += "\n";
                    continue;
                }

                try_append_anchor(seq_item, true, str);
                try_append_tag(seq_item, true, str);

                const bool is_scalar = seq_item.is_scalar();
                if (is_scalar) {
                    str += " ";
                    serialize_node(seq_item, cur_indent, str);
                    str += "\n";
                    continue;
                }

                const bool is_empty = seq_item.empty();
                if (!is_empty) {
                    str += "\n";
                    serialize_node(seq_item, cur_indent + 2, str);
                    continue;
                }

                // an empty sequence or mapping
                if (seq_item.is_sequence()) {
                    str += " []\n";
                }
                else /*seq_item.is_mapping()*/ {
                    str += " {}\n";
                }
            }
            break;
        case node_type::MAPPING:
            if (node.size() == 0) {
                str += "{}\n";
                return;
            }
            for (auto itr : node.map_items()) {
                insert_indentation(cur_indent, str);

                // serialize a mapping key node.
                const auto& key_node = itr.key();

                bool is_appended = try_append_alias(key_node, false, str);
                if (is_appended) {
                    // The trailing white space is necessary since anchor names can contain a colon (:) at its end.
                    str += " ";
                }
                else {
                    const bool is_anchor_appended = try_append_anchor(key_node, false, str);
                    const bool is_tag_appended = try_append_tag(key_node, is_anchor_appended, str);
                    if (is_anchor_appended || is_tag_appended) {
                        str += " ";
                    }

                    const bool is_container = !key_node.is_scalar();
                    if (is_container) {
                        str += "? ";
                    }
                    const auto indent = static_cast<uint32_t>(get_cur_indent(str));
                    serialize_node(key_node, indent, str);
                    if (is_container) {
                        // a newline code is already inserted in the above serialize_node() call.
                        insert_indentation(indent - 2, str);
                    }
                }

                str += ":";

                // serialize a mapping value node.
                const auto& value_node = itr.value();

                is_appended = try_append_alias(value_node, true, str);
                if (is_appended) {
                    str += "\n";
                    continue;
                }

                try_append_anchor(value_node, true, str);
                try_append_tag(value_node, true, str);

                const bool is_scalar = itr->is_scalar();
                if (is_scalar) {
                    str += " ";
                    serialize_node(value_node, cur_indent, str);
                    str += "\n";
                    continue;
                }

                const bool is_empty = itr->empty();
                if (is_empty) {
                    str += " ";
                }
                else {
                    str += "\n";
                }
                serialize_node(value_node, cur_indent + 2, str);
            }
            break;
        case node_type::NULL_OBJECT:
            to_string(nullptr, m_tmp_str_buff);
            str += m_tmp_str_buff;
            break;
        case node_type::BOOLEAN:
            to_string(node.template get_value<typename BasicNodeType::boolean_type>(), m_tmp_str_buff);
            str += m_tmp_str_buff;
            break;
        case node_type::INTEGER:
            to_string(node.template get_value<typename BasicNodeType::integer_type>(), m_tmp_str_buff);
            str += m_tmp_str_buff;
            break;
        case node_type::FLOAT:
            to_string(node.template get_value<typename BasicNodeType::float_number_type>(), m_tmp_str_buff);
            str += m_tmp_str_buff;
            break;
        case node_type::STRING: {
            bool is_escaped = false;
            auto str_val = get_string_node_value(node, is_escaped);

            if (is_escaped) {
                // There's no other token type with escapes than strings.
                // Also, escapes must be in double-quoted strings.
                str += '\"';
                str += str_val;
                str += '\"';
                break;
            }

            // The next line is intentionally excluded from the LCOV coverage target since the next line is somehow
            // misrecognized as it has a binary branch. Possibly begin() or end() has some conditional branch(es)
            // internally. Confirmed with LCOV 1.14 on Ubuntu22.04.
            const node_type type_if_plain =
                scalar_scanner::scan(str_val.c_str(), str_val.c_str() + str_val.size()); // LCOV_EXCL_LINE

            if (type_if_plain != node_type::STRING) {
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
        const bool is_empty = s.empty();
        if (is_empty) {
            return 0;
        }

        const std::size_t last_lf_pos = s.rfind('\n');
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
    /// @param prepends_space Whether to prepend a space before an anchor property.
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
    /// @param prepends_space Whether to prepend a space before an alias property.
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
    /// @param[out] is_escaped Whether the contents of an output string has been escaped.
    /// @return The (escaped) string node value.
    typename BasicNodeType::string_type get_string_node_value(const BasicNodeType& node, bool& is_escaped) {
        FK_YAML_ASSERT(node.is_string());

        const auto& s = node.as_str();
        return yaml_escaper::escape(s.c_str(), s.c_str() + s.size(), is_escaped);
    } // LCOV_EXCL_LINE

private:
    /// A temporal buffer for conversion from a scalar to a string.
    std::string m_tmp_str_buff;
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP */
