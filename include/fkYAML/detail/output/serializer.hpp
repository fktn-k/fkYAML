//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.5.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP
#define FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP

#include <algorithm>
#include <cmath>
#include <deque>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/conversions/to_string.hpp>
#include <fkYAML/detail/encodings/yaml_escaper.hpp>
#include <fkYAML/detail/input/scalar_scanner.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/node_attrs.hpp>
#include <fkYAML/detail/output/output_adapter.hpp>
#include <fkYAML/exception.hpp>
#include <fkYAML/node_type.hpp>
#include <fkYAML/yaml_version_type.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Predefined spaces for indentation in YAML serialization.
static const str_view indentation_spaces = "        "
                                           "        "
                                           "        "
                                           "        "
                                           "        "
                                           "        "
                                           "        "
                                           "        ";

/// @brief A basic implementation of serialization feature for YAML nodes.
/// @tparam BasicNodeType A BasicNode template class instantiation.
template <typename BasicNodeType>
class basic_serializer {
    static_assert(detail::is_basic_node<BasicNodeType>::value, "basic_serializer only accepts basic_node<...>");

    using map_iterator = typename BasicNodeType::const_map_range::const_iterator;

    struct anchor_reference_event {
        anchor_reference_event(std::string name, const uint32_t offset, const std::size_t position, bool is_anchor)
            : name(std::move(name)),
              offset(offset),
              position(position),
              is_anchor(is_anchor) {
        }

        std::string name;
        uint32_t offset {0};
        std::size_t position {0};
        bool is_anchor {false};
    };

    struct anchor_reference_span {
        anchor_reference_span() = default;

        anchor_reference_span(uint32_t first_index, uint32_t count)
            : first_index(first_index),
              count(count) {
        }

        uint32_t first_index {0};
        uint32_t count {0};
    };

    struct mapping_item_span {
        mapping_item_span() = default;

        mapping_item_span(uint32_t first_item_reference_index, uint32_t item_count)
            : first_item_reference_index(first_item_reference_index),
              item_count(item_count) {
        }

        uint32_t first_item_reference_index {0};
        uint32_t item_count {0};
    };

public:
    /// @brief Construct a new basic_serializer object.
    basic_serializer() = default;

    /// @brief Serialize the given Node value.
    /// @param node A Node object to be serialized.
    /// @param adapter An output_adapter object to write the serialized YAML content.
    void serialize(const BasicNodeType& node, output_adapter& adapter) {
        m_output_adapter = &adapter;
        m_current_indent = 0;
        serialize_document(node);
    }

    void serialize_docs(const std::vector<BasicNodeType>& docs, output_adapter& adapter) {
        m_output_adapter = &adapter;
        m_current_indent = 0;
        const auto size = static_cast<uint32_t>(docs.size());
        for (uint32_t i = 0; i < size; i++) {
            serialize_document(docs[i]);
            if (i + 1 < size) {
                // Append the end-of-document marker for the next document.
                write("...\n", 4);
            }
        }
    }

private:
    void serialize_document(const BasicNodeType& node) {
        m_has_anchor_table = (node.mp_meta && !node.mp_meta->anchor_table.empty());

        m_anchor_reference_events.clear();
        m_mapping_item_references.clear();
        m_anchor_reference_cache.clear();

        // Collect the anchor/alias events of the whole document once. Only mappings which do contain
        // any are cached, so a mapping missing from the cache needs no reordering at all.
        if (m_has_anchor_table) {
            std::size_t position = 0;
            collect_anchor_alias_names(node, position);
        }

        const bool dirs_serialized = serialize_directives(node);

        // the root node cannot be an alias node.
        const bool root_has_props = node.is_anchor() || node.has_tag_name();

        if (root_has_props) {
            if (dirs_serialized) {
                write(" ", 1);
            }
            bool is_anchor_appended = try_append_anchor(node, false);
            try_append_tag(node, is_anchor_appended);
            write("\n", 1);
        }
        else if (dirs_serialized) {
            write("\n", 1);
        }
        serialize_node(node, 0);
    }

    /// @brief Serialize the directives if any is applied to the node.
    /// @param node The target node.
    /// @return bool true if any directive is serialized, false otherwise.
    bool serialize_directives(const BasicNodeType& node) {
        const auto& p_meta = node.mp_meta;
        if (!p_meta) {
            // A node which was never part of a parsed document carries no directives.
            return false;
        }

        bool needs_directive_end = false;

        if (p_meta->is_version_specified) {
            write("%YAML ", 6);
            switch (p_meta->version) {
            case yaml_version_type::VERSION_1_1:
                write("1.1\n", 4);
                break;
            case yaml_version_type::VERSION_1_2:
                write("1.2\n", 4);
                break;
            }
            needs_directive_end = true;
        }

        if (!p_meta->primary_handle_prefix.empty()) {
            const auto& primary_handle_prefix = p_meta->primary_handle_prefix;
            write("%TAG ! ", 7);
            write(primary_handle_prefix.data(), primary_handle_prefix.size());
            write("\n", 1);
            needs_directive_end = true;
        }

        if (!p_meta->secondary_handle_prefix.empty()) {
            const auto& secondary_handle_prefix = p_meta->secondary_handle_prefix;
            write("%TAG !! ", 8);
            write(secondary_handle_prefix.data(), secondary_handle_prefix.size());
            write("\n", 1);
            needs_directive_end = true;
        }

        if (!p_meta->named_handle_map.empty()) {
            for (const auto& itr : p_meta->named_handle_map) {
                const auto& handle = itr.first;
                const auto& prefix = itr.second;
                write("%TAG ", 5);
                write(handle.data(), handle.size());
                write(" ", 1);
                write(prefix.data(), prefix.size());
                write("\n", 1);
            }
            needs_directive_end = true;
        }

        if (needs_directive_end) {
            write("---", 3);
        }

        return needs_directive_end;
    }

    /// @brief Recursively serialize each Node object.
    /// @param node A Node object to be serialized.
    /// @param cur_indent The current indent width
    void serialize_node(const BasicNodeType& node, const uint32_t cur_indent) {
        switch (node.get_type()) {
        case node_type::SEQUENCE:
            if (node.size() == 0) {
                write("[]\n", 3);
                return;
            }
            for (const auto& seq_item : node) {
                insert_indentation(cur_indent);
                write("-", 1);

                const bool is_appended = try_append_alias(seq_item, true);
                if (is_appended) {
                    write("\n", 1);
                    continue;
                }

                try_append_anchor(seq_item, true);
                try_append_tag(seq_item, true);

                const bool is_scalar = seq_item.is_scalar();
                if (is_scalar) {
                    write(" ", 1);
                    serialize_node(seq_item, cur_indent);
                    write("\n", 1);
                    continue;
                }

                const bool is_empty = seq_item.empty();
                if (!is_empty) {
                    write("\n", 1);
                    serialize_node(seq_item, cur_indent + 2);
                    continue;
                }

                // an empty sequence or mapping
                if (seq_item.is_sequence()) {
                    write(" []\n", 4);
                }
                else /*seq_item.is_mapping()*/ {
                    write(" {}\n", 4);
                }
            }
            break;
        case node_type::MAPPING:
            if (node.size() == 0) {
                write("{}\n", 3);
                return;
            }

            // If there is any anchor defined for this document and the mapping has more than one entry,
            // reorder the mapping entries so the anchor resolution order is preserved.
            // Only a document which defines an anchor can ever need its mapping entries reordered.
            if (m_has_anchor_table && node.size() > 1 && serialize_reordered_mapping(node, cur_indent)) {
                break;
            }

            for (const auto& itr : node.map_items()) {
                serialize_mapping_entry(itr, cur_indent);
            }
            break;
        case node_type::NULL_OBJECT:
            to_string(nullptr, m_tmp_str_buff);
            write(m_tmp_str_buff.c_str(), m_tmp_str_buff.size());
            break;
        case node_type::BOOLEAN:
            to_string(node.template get_value<typename BasicNodeType::boolean_type>(), m_tmp_str_buff);
            write(m_tmp_str_buff.c_str(), m_tmp_str_buff.size());
            break;
        case node_type::INTEGER:
            if (node.is_uint()) {
                // An unsigned integer may exceed the range of the signed integer type.
                to_string(node.as_uint(), m_tmp_str_buff);
            }
            else {
                to_string(node.template get_value<typename BasicNodeType::integer_type>(), m_tmp_str_buff);
            }
            write(m_tmp_str_buff.c_str(), m_tmp_str_buff.size());
            break;
        case node_type::FLOAT:
            to_string(node.template get_value<typename BasicNodeType::float_number_type>(), m_tmp_str_buff);
            write(m_tmp_str_buff.c_str(), m_tmp_str_buff.size());
            break;
        case node_type::STRING: {
            bool is_escaped = false;
            auto str_val = get_string_node_value(node, is_escaped);

            if (is_escaped) {
                // There's no other token type with escapes than strings.
                // Also, escapes must be in double-quoted strings.
                write("\"", 1);
                write(str_val.c_str(), str_val.size());
                write("\"", 1);
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
                write("\"", 1);
                write(str_val.c_str(), str_val.size());
                write("\"", 1);
            }
            else {
                write(str_val.c_str(), str_val.size());
            }
            break;
        }
        }
    }

    /// @brief Serialize a single mapping entry (key-value pair) in a YAML mapping node.
    /// @param itr An iterator pointing to the mapping entry to serialize.
    /// @param cur_indent The current indentation level for the serialized output.
    void serialize_mapping_entry(const map_iterator& itr, const uint32_t cur_indent) {
        insert_indentation(cur_indent);

        // serialize a mapping key node.
        const auto& key_node = itr.key();

        bool is_appended = try_append_alias(key_node, false);
        if (is_appended) {
            // The trailing white space is necessary since anchor names can contain a colon (:) at its end.
            write(" ", 1);
        }
        else {
            const bool is_anchor_appended = try_append_anchor(key_node, false);
            const bool is_tag_appended = try_append_tag(key_node, is_anchor_appended);
            if (is_anchor_appended || is_tag_appended) {
                write(" ", 1);
            }

            const bool is_container = !key_node.is_scalar();
            if (is_container) {
                write("? ", 2);
            }
            const auto indent = m_current_indent;
            serialize_node(key_node, indent);
            if (is_container) {
                // a newline code is already inserted in the above serialize_node() call.
                insert_indentation(indent - 2);
            }
        }

        write(":", 1);

        // serialize a mapping value node.
        const auto& value_node = itr.value();

        is_appended = try_append_alias(value_node, true);
        if (is_appended) {
            write("\n", 1);
            return;
        }

        try_append_anchor(value_node, true);
        try_append_tag(value_node, true);

        const bool is_scalar = value_node.is_scalar();
        if (is_scalar) {
            write(" ", 1);
            serialize_node(value_node, cur_indent);
            write("\n", 1);
            return;
        }

        const bool is_empty = value_node.empty();
        if (is_empty) {
            write(" ", 1);
        }
        else {
            write("\n", 1);
        }
        serialize_node(value_node, cur_indent + 2);
    }

    /// @brief Check whether two anchor references identify the same anchor.
    /// @param lhs The first anchor reference.
    /// @param rhs The second anchor reference.
    /// @return true if both references identify the same anchor, false otherwise.
    static bool is_same_anchor(const anchor_reference_event& lhs, const anchor_reference_event& rhs) noexcept {
        // Compare the offset first, since it rules out most candidates without touching the name.
        return lhs.offset == rhs.offset && lhs.name == rhs.name;
    }

    /// @brief Check whether an anchor is present in a collection.
    /// @param anchors The collection of anchor references.
    /// @param target The anchor reference to find.
    /// @return true if the target is present, false otherwise.
    bool has_anchor(const anchor_reference_span& anchors, const anchor_reference_event& target) const {
        const auto last_event_index = anchors.first_index + anchors.count;
        for (uint32_t i = anchors.first_index; i < last_event_index; ++i) {
            const auto& anchor = m_anchor_reference_events[i];
            if (anchor.is_anchor && is_same_anchor(anchor, target)) {
                return true;
            }
        }
        return false;
    }

    bool has_anchor(const std::vector<uint32_t>& anchor_indices, const anchor_reference_event& target) const {
        for (const auto anchor_index : anchor_indices) {
            if (is_same_anchor(m_anchor_reference_events[anchor_index], target)) {
                return true;
            }
        }
        return false;
    }

    /// @brief Check whether an anchor was defined earlier in the same mapping item.
    /// @param anchors The anchor references in the mapping item.
    /// @param target The anchor reference whose preceding definition is checked.
    /// @return true if a preceding definition exists, false otherwise.
    bool has_prior_anchor_definition(
        const anchor_reference_span& item_reference, const anchor_reference_event& target) const {
        const auto last_event_index = item_reference.first_index + item_reference.count;
        for (uint32_t i = item_reference.first_index; i < last_event_index; ++i) {
            const auto& anchor = m_anchor_reference_events[i];
            const bool is_anchor_defined_prior_to_target =
                anchor.is_anchor && anchor.position < target.position && is_same_anchor(anchor, target);
            if (is_anchor_defined_prior_to_target) {
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
        const anchor_reference_span& item_reference, const std::vector<uint32_t>& anchor_indices) const {
        const auto last_event_index = item_reference.first_index + item_reference.count;
        for (uint32_t i = item_reference.first_index; i < last_event_index; ++i) {
            const auto& anchor = m_anchor_reference_events[i];
            if (!anchor.is_anchor) {
                continue;
            }
            for (const auto anchor_index : anchor_indices) {
                const auto& mapping_anchor = m_anchor_reference_events[anchor_index];
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
        const anchor_reference_span& item_reference, const std::vector<uint32_t>& anchor_indices) const {
        const auto last_event_index = item_reference.first_index + item_reference.count;
        for (uint32_t i = item_reference.first_index; i < last_event_index; ++i) {
            const auto& alias = m_anchor_reference_events[i];
            if (alias.is_anchor) {
                continue;
            }
            if (!has_prior_anchor_definition(item_reference, alias) && has_anchor(anchor_indices, alias)) {
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
        const anchor_reference_span& item_reference, const std::vector<uint32_t>& anchor_indices) const {
        return !has_unemitted_prior_anchor_definition(item_reference, anchor_indices) &&
               !has_unemitted_anchor_definition(item_reference, anchor_indices);
    }

    /// @brief Remove the anchors defined by an emitted mapping item.
    /// @param anchors The anchor references defined by the emitted mapping item.
    /// @param anchors_in_mapping The anchor references that have not been emitted.
    void remove_anchors(const anchor_reference_span& anchors, std::vector<uint32_t>& anchor_indices) const {
        const auto last_event_index = anchors.first_index + anchors.count;
        for (uint32_t i = anchors.first_index; i < last_event_index; ++i) {
            const auto& anchor = m_anchor_reference_events[i];
            if (!anchor.is_anchor) {
                continue;
            }
            auto itr = anchor_indices.begin();
            while (itr != anchor_indices.end()) {
                const auto& mapping_anchor = m_anchor_reference_events[*itr];
                itr = is_same_anchor(mapping_anchor, anchor) ? anchor_indices.erase(itr) : std::next(itr);
            }
        }
    }

    /// @brief Reorder mapping items so that their anchor dependencies are emitted first.
    /// @note This function uses stable topological sorting and thus changes the order of mapping items only when
    /// necessary to satisfy anchor dependencies.
    /// @param items The mapping items to reorder.
    /// @param item_anchors The anchor references grouped by mapping item.
    /// @param item_aliases The alias references grouped by mapping item.
    /// @param anchors_in_mapping The anchor references defined in the mapping.
    /// @return The mapping items in dependency-respecting serialization order.
    std::vector<map_iterator> reorder_mapping_items_by_dependencies(
        const std::vector<map_iterator>& items, const std::vector<anchor_reference_span>& item_references,
        std::vector<uint32_t> anchor_indices) const {
        std::vector<bool> emitted(items.size(), false);
        std::vector<map_iterator> ordered_items;
        ordered_items.reserve(items.size());

        std::size_t emitted_count = 0;
        // Every item before this index has already been emitted, so the scan below can skip them.
        std::size_t first_unemitted_index = 0;
        while (emitted_count < items.size()) {
            std::size_t ready_item_index = items.size();
            for (std::size_t i = first_unemitted_index; i < items.size(); ++i) {
                const bool is_item_emitted = emitted[i];
                if (is_item_emitted) {
                    continue;
                }

                const bool is_item_ready = is_ready_to_emit(item_references[i], anchor_indices);
                if (is_item_ready) {
                    ready_item_index = i;
                    break;
                }
            }

            emitted[ready_item_index] = true;
            ordered_items.emplace_back(items[ready_item_index]);
            remove_anchors(item_references[ready_item_index], anchor_indices);
            ++emitted_count;
            while (first_unemitted_index < items.size() && emitted[first_unemitted_index]) {
                ++first_unemitted_index;
            }
        }

        return ordered_items;
    }

    /// @brief Serialize a mapping whose entries have to be reordered for anchor resolution.
    /// @param node The mapping to serialize.
    /// @param cur_indent The current indent width.
    /// @return true if the mapping was serialized here, false if it needs no reordering.
    bool serialize_reordered_mapping(const BasicNodeType& node, const uint32_t cur_indent) {
        std::vector<map_iterator> ordered_items;
        const bool is_reordered = get_mapping_items_in_serialization_order(node, ordered_items);
        if (!is_reordered) {
            return false;
        }

        for (const auto& itr : ordered_items) {
            serialize_mapping_entry(itr, cur_indent);
        }
        return true;
    }
    /// @brief Collect the mapping items in the order they have to be serialized in.
    /// @param node The mapping to serialize.
    /// @param ordered_items Receives the reordered items, untouched unless reordering is needed.
    /// @return true if the items were reordered, false if the mapping keeps its own order.
    bool get_mapping_items_in_serialization_order(const BasicNodeType& node, std::vector<map_iterator>& ordered_items) {
        const auto mapping_reference_itr = m_anchor_reference_cache.find(&node);
        if (mapping_reference_itr == m_anchor_reference_cache.end()) {
            return false;
        }

        const mapping_item_span mapping_reference = mapping_reference_itr->second;
        const auto& first_item_reference = m_mapping_item_references[mapping_reference.first_item_reference_index];
        const auto& last_item_reference =
            m_mapping_item_references[mapping_reference.first_item_reference_index + mapping_reference.item_count - 1];
        const anchor_reference_span mapping_event_reference {
            first_item_reference.first_index,
            last_item_reference.first_index + last_item_reference.count - first_item_reference.first_index};

        bool has_any_anchor = false;
        bool has_any_alias = false;
        const auto last_index = mapping_event_reference.first_index + mapping_event_reference.count;
        for (uint32_t event_index = mapping_event_reference.first_index; event_index < last_index; ++event_index) {
            has_any_anchor = has_any_anchor || m_anchor_reference_events[event_index].is_anchor;
            has_any_alias = has_any_alias || !m_anchor_reference_events[event_index].is_anchor;
        }

        // If there are no anchors (no resolving needed) or aliases (no anchor is referenced), return the items in the
        // order `node.map_items()` returns them.
        const bool needs_reordering = has_any_anchor && has_any_alias;
        if (!needs_reordering) {
            return false;
        }

        std::vector<map_iterator> items;
        std::vector<anchor_reference_span> item_references;
        items.reserve(mapping_reference.item_count);
        item_references.reserve(mapping_reference.item_count);
        std::vector<uint32_t> anchor_indices;

        auto itr = node.map_items().begin();
        for (uint32_t i = 0; i < mapping_reference.item_count; ++i, ++itr) {
            items.emplace_back(itr);
            const auto& item_reference = m_mapping_item_references[mapping_reference.first_item_reference_index + i];
            item_references.emplace_back(item_reference);

            const auto last_event_index = item_reference.first_index + item_reference.count;
            for (uint32_t event_index = item_reference.first_index; event_index < last_event_index; ++event_index) {
                if (m_anchor_reference_events[event_index].is_anchor) {
                    anchor_indices.emplace_back(event_index);
                }
            }
        }

        for (std::size_t i = 0; i < items.size(); ++i) {
            const auto& item_reference = item_references[i];
            const auto last_event_index = item_reference.first_index + item_reference.count;
            for (uint32_t event_index = item_reference.first_index; event_index < last_event_index; ++event_index) {
                const auto& alias = m_anchor_reference_events[event_index];
                if (alias.is_anchor) {
                    continue;
                }
                const bool is_anchor_defined_in_different_item =
                    has_anchor(anchor_indices, alias) && !has_anchor(item_reference, alias);
                if (is_anchor_defined_in_different_item) {
                    ordered_items =
                        reorder_mapping_items_by_dependencies(items, item_references, std::move(anchor_indices));
                    return true;
                }
            }
        }

        // Reordering is only needed when an alias refers to an anchor in another mapping item.
        return false;
    }

    anchor_reference_span collect_anchor_alias_names(const BasicNodeType& node, std::size_t& position) {
        const auto first_event_index = static_cast<uint32_t>(m_anchor_reference_events.size());
        if (node.is_alias()) {
            anchor_reference_event alias_event(
                node.get_anchor_name(), node.m_attrs.get_anchor_offset(), position++, false);
            m_anchor_reference_events.emplace_back(std::move(alias_event));
        }
        else if (node.is_anchor()) {
            anchor_reference_event anchor_event(
                node.get_anchor_name(), node.m_attrs.get_anchor_offset(), position++, true);
            m_anchor_reference_events.emplace_back(std::move(anchor_event));
        }

        if (!node.is_alias()) {
            switch (node.get_type()) {
            case node_type::SEQUENCE:
                for (const auto& item : node) {
                    collect_anchor_alias_names(item, position);
                }
                break;
            case node_type::MAPPING: {
                std::vector<anchor_reference_span> item_references;
                item_references.reserve(node.size());
                for (auto itr : node.map_items()) {
                    const auto key_reference = collect_anchor_alias_names(itr.key(), position);
                    const auto value_reference = collect_anchor_alias_names(itr.value(), position);
                    const auto event_count =
                        value_reference.first_index + value_reference.count - key_reference.first_index;
                    item_references.emplace_back(anchor_reference_span {key_reference.first_index, event_count});
                }
                const auto first_item_reference_index = static_cast<uint32_t>(m_mapping_item_references.size());
                m_mapping_item_references.insert(
                    m_mapping_item_references.end(), item_references.begin(), item_references.end());
                const mapping_item_span reference {
                    first_item_reference_index, static_cast<uint32_t>(item_references.size())};
                if (m_anchor_reference_events.size() > first_event_index) {
                    m_anchor_reference_cache.emplace(&node, reference);
                }
                break;
            }
            default:
                break;
            }
        }

        if (node.is_mapping()) {
            const auto event_count = static_cast<uint32_t>(m_anchor_reference_events.size()) - first_event_index;
            return anchor_reference_span {first_event_index, event_count};
        }
        return anchor_reference_span {
            first_event_index, static_cast<uint32_t>(m_anchor_reference_events.size()) - first_event_index};
    }

    void write(const char* p_data, const std::size_t size) {
        m_output_adapter->write(p_data, size);

        for (std::size_t i = size; i > 0; --i) {
            if (p_data[i - 1] == '\n') {
                m_current_indent = static_cast<uint32_t>(size - i);
                return;
            }
        }
        m_current_indent += static_cast<uint32_t>(size);
    }

    /// @brief Insert indentation to the serialization result.
    /// @param indent The indent width to be inserted.
    void insert_indentation(const uint32_t indent) {
        if (indent <= m_current_indent) {
            return;
        }

        uint32_t remaining_spaces = indent - m_current_indent;
        while (remaining_spaces > 0) {
            const auto size = std::min<std::size_t>(remaining_spaces, indentation_spaces.size());
            write(indentation_spaces.data(), size);
            remaining_spaces -= static_cast<uint32_t>(size);
        }
    }

    /// @brief Append an anchor property if it's available. Do nothing otherwise.
    /// @param node The target node which is possibly an anchor node.
    /// @param prepends_space Whether to prepend a space before an anchor property.
    /// @return true if an anchor property has been appended, false otherwise.
    bool try_append_anchor(const BasicNodeType& node, bool prepends_space) {
        if (node.is_anchor()) {
            if (prepends_space) {
                write(" ", 1);
            }
            const auto& anchor = node.get_anchor_name();
            write("&", 1);
            write(anchor.data(), anchor.size());
            return true;
        }
        return false;
    }

    /// @brief Append an alias property if it's available. Do nothing otherwise.
    /// @param node The target node which is possibly an alias node.
    /// @param prepends_space Whether to prepend a space before an alias property.
    /// @return true if an alias property has been appended, false otherwise.
    bool try_append_alias(const BasicNodeType& node, bool prepends_space) {
        if (node.is_alias()) {
            if (prepends_space) {
                write(" ", 1);
            }
            const auto& anchor = node.get_anchor_name();
            write("*", 1);
            write(anchor.data(), anchor.size());
            return true;
        }
        return false;
    }

    /// @brief Append a tag name if it's available. Do nothing otherwise.
    /// @param[in] node The target node which possibly has a tag name.
    /// @param[out] prepends_space Whether to prepend a space before a tag name.
    /// @return true if a tag name has been appended, false otherwise.
    bool try_append_tag(const BasicNodeType& node, bool prepends_space) {
        if (node.has_tag_name()) {
            if (prepends_space) {
                write(" ", 1);
            }
            const auto& tag = node.get_tag_name();
            write(tag.data(), tag.size());
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

        using string_type = typename BasicNodeType::string_type;
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
    /// A queue to hold anchor reference events.
    std::deque<anchor_reference_event> m_anchor_reference_events;
    /// Anchor reference spans for mapping items.
    /// Each entry represents an anchor reference span for a mapping item.
    std::vector<anchor_reference_span> m_mapping_item_references;
    /// Anchor reference cache for quick lookup of mapping reference spans.
    /// Each entry maps a YAML node to its corresponding mapping reference span.
    std::unordered_map<const BasicNodeType*, mapping_item_span> m_anchor_reference_cache;
    /// A temporal buffer for conversion from a scalar to a string.
    std::string m_tmp_str_buff;
    /// Output adapter for writing serialized YAML content.
    output_adapter* m_output_adapter {nullptr};
    /// Current indentation width after the last line feed.
    uint32_t m_current_indent {0};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_OUTPUT_SERIALIZER_HPP */
