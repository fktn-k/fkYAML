//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.4
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP
#define FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/conversions/to_string.hpp>
#include <fkYAML/detail/encodings/yaml_escaper.hpp>
#include <fkYAML/detail/input/scalar_scanner.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/node_attrs.hpp>
#include <fkYAML/exception.hpp>
#include <fkYAML/node_type.hpp>
#include <fkYAML/yaml_version_type.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief A basic implementation of serialization feature for YAML nodes.
/// @tparam BasicNodeType A BasicNode template class instantiation.
template <typename BasicNodeType>
class basic_serializer {
    static_assert(detail::is_basic_node<BasicNodeType>::value, "basic_serializer only accepts basic_node<...>");

    using map_iterator = typename BasicNodeType::const_map_range::const_iterator;

    struct anchor_reference {
        anchor_reference(std::string name, const uint32_t offset, const std::size_t position)
            : name(std::move(name)),
              offset(offset),
              position(position) {
        }

        std::string name;
        uint32_t offset {0};
        std::size_t position {0};
    };

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
        m_has_anchor_table = (node.mp_meta && !node.mp_meta->anchor_table.empty());

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
        if (!p_meta) {
            // A node which was never part of a parsed document carries no directives.
            return false;
        }

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

            // If there is any anchor defined for this document and the mapping has more than one entry,
            // reorder the mapping entries so the anchor resolution order is preserved.
            if (m_has_anchor_table && node.size() > 1) {
                for (const auto& itr : get_mapping_items_in_serialization_order(node)) {
                    serialize_mapping_entry(itr, cur_indent, str);
                }
            }
            else {
                for (const auto& itr : node.map_items()) {
                    serialize_mapping_entry(itr, cur_indent, str);
                }
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

            if (type_if_plain != node_type::STRING || !is_valid_plain_scalar(str_val)) {
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

    void serialize_mapping_entry(const map_iterator& itr, const uint32_t cur_indent, std::string& str) {
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
            return;
        }

        try_append_anchor(value_node, true, str);
        try_append_tag(value_node, true, str);

        const bool is_scalar = value_node.is_scalar();
        if (is_scalar) {
            str += " ";
            serialize_node(value_node, cur_indent, str);
            str += "\n";
            return;
        }

        const bool is_empty = value_node.empty();
        if (is_empty) {
            str += " ";
        }
        else {
            str += "\n";
        }
        serialize_node(value_node, cur_indent + 2, str);
    }

    /// @brief Check whether two anchor references identify the same anchor.
    /// @param lhs The first anchor reference.
    /// @param rhs The second anchor reference.
    /// @return true if both references identify the same anchor, false otherwise.
    static bool is_same_anchor(const anchor_reference& lhs, const anchor_reference& rhs) noexcept {
        return lhs.name == rhs.name && lhs.offset == rhs.offset;
    }

    /// @brief Check whether an anchor is present in a collection.
    /// @param anchors The collection of anchor references.
    /// @param target The anchor reference to find.
    /// @return true if the target is present, false otherwise.
    static bool has_anchor(const std::vector<anchor_reference>& anchors, const anchor_reference& target) {
        for (const auto& anchor : anchors) {
            if (is_same_anchor(anchor, target)) {
                return true;
            }
        }
        return false;
    }

    /// @brief Check whether an anchor was defined earlier in the same mapping item.
    /// @param anchors The anchor references in the mapping item.
    /// @param target The anchor reference whose preceding definition is checked.
    /// @return true if a preceding definition exists, false otherwise.
    static bool has_prior_anchor_definition(
        const std::vector<anchor_reference>& anchors, const anchor_reference& target) {
        for (const auto& anchor : anchors) {
            if (anchor.position < target.position && is_same_anchor(anchor, target)) {
                return true;
            }
        }
        return false;
    }

    /// @brief Check whether an earlier anchor required by an item remains unemitted.
    /// @param anchors The anchor references in the mapping item.
    /// @param anchors_in_mapping The anchor references that have not been emitted.
    /// @return true if an earlier anchor definition remains unemitted, false otherwise.
    bool has_unemitted_prior_anchor_definition(
        const std::vector<anchor_reference>& anchors, const std::vector<anchor_reference>& anchors_in_mapping) const {
        for (const auto& anchor : anchors) {
            for (const auto& mapping_anchor : anchors_in_mapping) {
                if (mapping_anchor.name == anchor.name && mapping_anchor.offset < anchor.offset) {
                    return true;
                }
            }
        }
        return false;
    }

    /// @brief Check whether an alias requires an anchor definition that remains unemitted.
    /// @param anchors The anchor references in the mapping item.
    /// @param aliases The alias references in the mapping item.
    /// @param anchors_in_mapping The anchor references that have not been emitted.
    /// @return true if an aliased anchor remains unemitted, false otherwise.
    bool has_unemitted_anchor_definition(
        const std::vector<anchor_reference>& anchors, const std::vector<anchor_reference>& aliases,
        const std::vector<anchor_reference>& anchors_in_mapping) const {
        for (const auto& alias : aliases) {
            if (!has_prior_anchor_definition(anchors, alias) && has_anchor(anchors_in_mapping, alias)) {
                return true;
            }
        }
        return false;
    }

    /// @brief Check whether a mapping item can be emitted without violating anchor dependencies.
    /// @param anchors The anchor references in the mapping item.
    /// @param aliases The alias references in the mapping item.
    /// @param anchors_in_mapping The anchor references that have not been emitted.
    /// @return true if the mapping item is ready to be emitted, false otherwise.
    bool is_ready_to_emit(
        const std::vector<anchor_reference>& anchors, const std::vector<anchor_reference>& aliases,
        const std::vector<anchor_reference>& anchors_in_mapping) const {
        return !has_unemitted_prior_anchor_definition(anchors, anchors_in_mapping) &&
               !has_unemitted_anchor_definition(anchors, aliases, anchors_in_mapping);
    }

    /// @brief Remove the anchors defined by an emitted mapping item.
    /// @param anchors The anchor references defined by the emitted mapping item.
    /// @param anchors_in_mapping The anchor references that have not been emitted.
    static void remove_anchors(
        const std::vector<anchor_reference>& anchors, std::vector<anchor_reference>& anchors_in_mapping) {
        for (const auto& anchor : anchors) {
            auto itr = anchors_in_mapping.begin();
            while (itr != anchors_in_mapping.end()) {
                itr = is_same_anchor(*itr, anchor) ? anchors_in_mapping.erase(itr) : std::next(itr);
            }
        }
    }

    /// @brief Reorder mapping items so that their anchor dependencies are emitted first.
    /// @note This function uses stable topological sorting and thus changes the order of mapping items only when
    /// necessary to satisfy anchor dependencies.
    /// @param items The mapping items to reorder.
    /// @param item_anchors The anchor references grouped by mapping item.
    /// @param item_aliases The alias references grouped by mapping item.
    /// @return The mapping items in dependency-respecting serialization order.
    std::vector<map_iterator> reorder_mapping_items_by_dependencies(
        const std::vector<map_iterator>& items, const std::vector<std::vector<anchor_reference>>& item_anchors,
        const std::vector<std::vector<anchor_reference>>& item_aliases) const {
        std::vector<bool> emitted(items.size(), false);
        std::vector<map_iterator> ordered_items;
        ordered_items.reserve(items.size());

        std::vector<anchor_reference> anchors_in_mapping;
        for (const auto& anchors : item_anchors) {
            anchors_in_mapping.insert(anchors_in_mapping.end(), anchors.begin(), anchors.end());
        }

        std::size_t emitted_count = 0;
        while (emitted_count < items.size()) {
            std::size_t ready_item_index = items.size();
            for (std::size_t i = 0; i < items.size(); ++i) {
                const bool is_item_emitted = emitted[i];
                if (is_item_emitted) {
                    continue;
                }

                const bool is_item_ready = is_ready_to_emit(item_anchors[i], item_aliases[i], anchors_in_mapping);
                if (is_item_ready) {
                    ready_item_index = i;
                    break;
                }
            }

            if (ready_item_index == items.size()) {
                break;
            }

            emitted[ready_item_index] = true;
            ordered_items.emplace_back(items[ready_item_index]);
            remove_anchors(item_anchors[ready_item_index], anchors_in_mapping);
            ++emitted_count;
        }

        for (std::size_t i = 0; i < items.size(); ++i) {
            const bool is_item_emitted = emitted[i];
            if (!is_item_emitted) {
                ordered_items.emplace_back(items[i]);
            }
        }

        return ordered_items;
    }

    std::vector<map_iterator> get_mapping_items_in_serialization_order(const BasicNodeType& node) const {
        std::vector<map_iterator> items;
        std::vector<std::vector<anchor_reference>> item_anchors;
        std::vector<std::vector<anchor_reference>> item_aliases;
        bool has_any_anchor = false;
        bool has_any_alias = false;

        for (auto itr : node.map_items()) {
            items.emplace_back(itr);
            item_anchors.emplace_back();
            item_aliases.emplace_back();
            std::size_t position = 0;
            collect_anchor_alias_names(itr.key(), item_anchors.back(), item_aliases.back(), position);
            collect_anchor_alias_names(itr.value(), item_anchors.back(), item_aliases.back(), position);
            has_any_anchor = has_any_anchor || !item_anchors.back().empty();
            has_any_alias = has_any_alias || !item_aliases.back().empty();
        }

        // If there are no anchors (no resolving needed) or aliases (no anchor is referenced), return the items in the
        // order `node.map_items()` returns them.
        if (!has_any_anchor || !has_any_alias) {
            return items;
        }

        return reorder_mapping_items_by_dependencies(items, item_anchors, item_aliases);
    }

    void collect_anchor_alias_names(
        const BasicNodeType& node, std::vector<anchor_reference>& anchors, std::vector<anchor_reference>& aliases,
        std::size_t& position) const {
        if (node.is_alias()) {
            anchor_reference alias_ref(
                node.get_anchor_name(), detail::node_attr_bits::get_anchor_offset(node.m_attrs), position++);
            aliases.emplace_back(std::move(alias_ref));
            return;
        }
        if (node.is_anchor()) {
            anchor_reference anchor_ref(
                node.get_anchor_name(), detail::node_attr_bits::get_anchor_offset(node.m_attrs), position++);
            anchors.emplace_back(std::move(anchor_ref));
        }

        switch (node.get_type()) {
        case node_type::SEQUENCE:
            for (const auto& item : node) {
                collect_anchor_alias_names(item, anchors, aliases, position);
            }
            break;
        case node_type::MAPPING:
            for (auto itr : node.map_items()) {
                collect_anchor_alias_names(itr.key(), anchors, aliases, position);
                collect_anchor_alias_names(itr.value(), anchors, aliases, position);
            }
            break;
        default:
            break;
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

    bool is_valid_plain_scalar(const typename BasicNodeType::string_type& s) const noexcept {
        if (s.empty()) {
            return false;
        }

        typedef typename BasicNodeType::string_type string_type;
        if (s.find_first_of(" \t\n\r,[]{}") != string_type::npos) {
            return false;
        }

        switch (s.front()) {
        case '-':
        case '?':
        case ':':
            return s.size() > 1 && s[1] != ' ';
        case '#':
        case '&':
        case '*':
        case '!':
        case '|':
        case '>':
        case '\'':
        case '"':
        case '%':
        case '@':
        case '`':
        case ' ':
            return false;
        default:
            return s.back() != ' ';
        }
    }

private:
    /// Indicates whether any anchor is present in the YAML document.
    bool m_has_anchor_table {false};

    /// A temporal buffer for conversion from a scalar to a string.
    std::string m_tmp_str_buff;
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP */
