//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.5.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FKYAML_DETAIL_INPUT_NODE_BUILDER_HPP
#define FKYAML_DETAIL_INPUT_NODE_BUILDER_HPP

#include <iterator>
#include <memory>
#include <stack>
#include <vector>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/input/scalar_parser.hpp>
#include <fkYAML/detail/input/stream_events.hpp>
#include <fkYAML/detail/input/tag_resolver.hpp>
#include <fkYAML/detail/document_metainfo.hpp>
#include <fkYAML/detail/string_formatter.hpp>
#include <fkYAML/exception.hpp>
#include <fkYAML/node_type.hpp>
#include <fkYAML/yaml_version_type.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

template <typename BasicNodeType>
class node_builder {
    class node_slot {
    public:
        explicit node_slot(BasicNodeType& node)
            : mp_borrowed_node(&node) {
        }

        BasicNodeType& get() {
            return *mp_borrowed_node;
        }

        bool has_mapping_key() const {
            return static_cast<bool>(mp_mapping_key);
        }

        void set_mapping_key(BasicNodeType node) {
            mp_mapping_key = std::unique_ptr<BasicNodeType>(new BasicNodeType(std::move(node)));
        }

        BasicNodeType& get_mapping_key() {
            return *mp_mapping_key;
        }

        BasicNodeType take_mapping_key() {
            auto key = std::move(*mp_mapping_key);
            mp_mapping_key.reset();
            return key;
        }

    private:
        BasicNodeType* mp_borrowed_node {nullptr};
        std::unique_ptr<BasicNodeType> mp_mapping_key;
    };

public:
    explicit node_builder(BasicNodeType& root)
        : mp_root(&root) {
    }

    explicit node_builder(std::vector<BasicNodeType>& roots)
        : mp_roots(&roots) {
    }

    void on_stream_start(const stream_start_event& /*unused*/) {
    }

    void on_stream_end(const stream_end_event& /*unused*/) {
    }

    void on_yaml_directive(const yaml_directive_event& event) {
        if (!mp_meta) {
            mp_meta = std::make_shared<document_metainfo<BasicNodeType>>();
        }
        mp_meta->version = event.version == "1.1" ? yaml_version_type::VERSION_1_1 : yaml_version_type::VERSION_1_2;
        mp_meta->is_version_specified = true;
    }

    void on_tag_directive(const tag_directive_event& event) {
        if (!mp_meta) {
            mp_meta = std::make_shared<document_metainfo<BasicNodeType>>();
        }
        if (event.handle == "!") {
            mp_meta->primary_handle_prefix = to_string(event.prefix);
            return;
        }
        if (event.handle == "!!") {
            mp_meta->secondary_handle_prefix = to_string(event.prefix);
            return;
        }
        mp_meta->named_handle_map.emplace(to_string(event.handle), to_string(event.prefix));
    }

    void on_document_start(const document_start_event& /*unused*/) {
        if (!mp_meta) {
            mp_meta = std::make_shared<document_metainfo<BasicNodeType>>();
        }
        if (mp_roots) {
            mp_roots->emplace_back();
            mp_root = &mp_roots->back();
        }
        m_has_root = false;
    }

    void on_document_end(const document_end_event& /*unused*/) {
        if (!m_has_root) {
            root() = BasicNodeType();
            apply_meta(root());
        }
        mp_meta.reset();
    }

    void on_sequence_start(const sequence_start_event& event) {
        auto node = BasicNodeType::sequence();
        apply_meta(node);
        resolve_tag(event.tag);
        apply_tag(node, event.tag);

        if (m_node_stack.empty()) {
            root() = std::move(node);
            apply_anchor(root(), event.anchor);
            m_has_root = true;
            m_node_stack.emplace(root());
            return;
        }

        auto& parent_slot = m_node_stack.top();
        auto& parent_node = parent_slot.get();
        switch (parent_node.get_type()) {
        case node_type::SEQUENCE: {
            auto& parent_seq = parent_node.as_seq();
            parent_seq.emplace_back(std::move(node));
            apply_anchor(parent_seq.back(), event.anchor);
            m_node_stack.emplace(parent_seq.back());
            break;
        }
        case node_type::MAPPING:
            handle_collection_event(parent_slot, std::move(node), event.anchor);
            break;
        default:
            // Handle other types or error
            break;
        }
    }

    void on_sequence_end(const sequence_end_event& /*unused*/) {
        m_node_stack.pop();
    }

    void on_mapping_start(const mapping_start_event& event) {
        auto node = BasicNodeType::mapping();
        apply_meta(node);
        resolve_tag(event.tag);
        apply_tag(node, event.tag);

        if (m_node_stack.empty()) {
            root() = std::move(node);
            apply_anchor(root(), event.anchor);
            m_has_root = true;
            m_node_stack.emplace(root());
            return;
        }

        auto& parent_slot = m_node_stack.top();
        auto& parent_node = parent_slot.get();
        switch (parent_node.get_type()) {
        case node_type::SEQUENCE: {
            auto& parent_seq = parent_node.as_seq();
            parent_seq.emplace_back(std::move(node));
            apply_anchor(parent_seq.back(), event.anchor);
            m_node_stack.emplace(parent_seq.back());
            break;
        }
        case node_type::MAPPING:
            handle_collection_event(parent_slot, std::move(node), event.anchor);
            break;
        default:
            // Handle other types or error
            break;
        }
    }

    void on_mapping_end(const mapping_end_event& /*unused*/) {
        m_node_stack.pop();
    }

    void on_plain_scalar(const plain_scalar_event& event) {
        handle_flow_scalar_event(lexical_token_t::PLAIN_SCALAR, event.value, event.tag, event.anchor);
    }

    void on_single_quoted_scalar(const single_quoted_scalar_event& event) {
        handle_flow_scalar_event(lexical_token_t::SINGLE_QUOTED_SCALAR, event.value, event.tag, event.anchor);
    }

    void on_double_quoted_scalar(const double_quoted_scalar_event& event) {
        handle_flow_scalar_event(lexical_token_t::DOUBLE_QUOTED_SCALAR, event.value, event.tag, event.anchor);
    }

    void on_literal_scalar(const literal_scalar_event& event) {
        handle_block_scalar_event(
            lexical_token_t::BLOCK_LITERAL_SCALAR, event.value, event.tag, event.anchor, event.header);
    }

    void on_folded_scalar(const folded_scalar_event& event) {
        handle_block_scalar_event(
            lexical_token_t::BLOCK_FOLDED_SCALAR, event.value, event.tag, event.anchor, event.header);
    }

    void on_alias(const alias_event& event) {
        const std::string anchor_name = to_string(event.value);

        auto& anchor_table = mp_meta->anchor_table;
        const auto anchor_counts = static_cast<uint32_t>(anchor_table.count(anchor_name));
        if FK_YAML_UNLIKELY (anchor_counts == 0) {
            throw parse_error("The given anchor name must appear prior to the alias node.", 0, 0);
        }

        BasicNodeType node {};
        node.m_attrs.set(detail::node_attr_bits::alias_bit);
        node.prop().anchor = anchor_name;
        node.m_attrs.set_anchor_offset(anchor_counts - 1);
        apply_meta(node);

        if (m_node_stack.empty()) {
            root() = std::move(node);
            m_has_root = true;
        }
        else {
            handle_scalar_event(std::move(node), {}, {});
        }

        // Check if the alias node is self-referential.
        // If so, throw a parse error to avoid infinite recursion and stack overflow during deserialization.
        auto itr = anchor_table.equal_range(anchor_name).first;
        std::advance(itr, anchor_counts - 1);
        const auto& anchor = itr->second;
        const bool is_self_referential = anchor.contains_self_referential_alias(anchor_name, anchor_counts - 1);
        if FK_YAML_UNLIKELY (is_self_referential) {
            const std::string msg = format("Self-referential alias (*%s) is found.", anchor_name.c_str());
            throw parse_error(msg.c_str(), 0, 0);
        }
    }

private:
    BasicNodeType& root() {
        return *mp_root;
    }

    void apply_meta(BasicNodeType& node) {
        node.mp_meta = mp_meta;
    }

    static void apply_tag(BasicNodeType& node, const str_view& tag) {
        if (!tag.empty()) {
            node.add_tag_name(to_string(tag));
        }
    }

    static void apply_anchor(BasicNodeType& node, const str_view& anchor) {
        if (!anchor.empty()) {
            node.add_anchor_name(to_string(anchor));
        }
    }

    static std::string to_string(const str_view& view) {
        return std::string(view.begin(), view.end());
    }

    tag_t resolve_tag(const str_view& tag) {
        if (tag.empty()) {
            return tag_t::NONE;
        }
        return tag_resolver<BasicNodeType>::resolve_tag(tag, mp_meta);
    }

    void handle_collection_event(node_slot& parent_slot, BasicNodeType node, const str_view& anchor) {
        if (!parent_slot.has_mapping_key()) {
            apply_anchor(node, anchor);
            parent_slot.set_mapping_key(std::move(node));
            m_node_stack.emplace(parent_slot.get_mapping_key());
            return;
        }

        auto& parent_map = parent_slot.get().as_map();
        auto itr = parent_map.emplace(parent_slot.take_mapping_key(), std::move(node));
        if FK_YAML_UNLIKELY (!itr.second) {
            throw parse_error("Detected duplication in mapping keys.", 0, 0);
        }
        apply_anchor(itr.first->second, anchor);
        m_node_stack.emplace(itr.first->second);
    }

    void handle_flow_scalar_event(
        lexical_token_t type, const str_view& value, const str_view& tag, const str_view& anchor) {
        tag_t tag_type = resolve_tag(tag);
        ensure_scalar_tag(tag_type);
        if (type == lexical_token_t::PLAIN_SCALAR && value.empty()) {
            BasicNodeType node;
            switch (tag_type) {
            case tag_t::STRING:
            case tag_t::NON_SPECIFIC:
            case tag_t::CUSTOM_TAG:
                node = BasicNodeType(typename BasicNodeType::string_type());
                break;
            case tag_t::NONE:
            case tag_t::NULL_VALUE:
                break;
            default:
                throw parse_error("Unsupported tag for an empty node.", 0, 0);
            }
            handle_scalar_event(std::move(node), tag, anchor);
            return;
        }
        auto node = scalar_parser<BasicNodeType>(0, 0).parse_flow(type, tag_type, value);
        handle_scalar_event(std::move(node), tag, anchor);
    }

    void handle_block_scalar_event(
        lexical_token_t type, const str_view& value, const str_view& tag, const str_view& anchor,
        const block_scalar_header& header) {
        tag_t tag_type = resolve_tag(tag);
        ensure_scalar_tag(tag_type);
        auto node = scalar_parser<BasicNodeType>(0, 0).parse_block(type, tag_type, value, header);
        handle_scalar_event(std::move(node), tag, anchor);
    }

    static void ensure_scalar_tag(const tag_t tag_type) {
        if FK_YAML_UNLIKELY (tag_type == tag_t::SEQUENCE || tag_type == tag_t::MAPPING) {
            throw parse_error("A sequence or mapping tag cannot be specified to a scalar node.", 0, 0);
        }
    }

    void handle_scalar_event(BasicNodeType node, const str_view& tag, const str_view& anchor) {
        apply_meta(node);
        apply_tag(node, tag);

        if (m_node_stack.empty()) {
            root() = std::move(node);
            apply_anchor(root(), anchor);
            m_has_root = true;
            return;
        }

        auto& parent_slot = m_node_stack.top();
        auto& parent_node = parent_slot.get();
        switch (parent_node.get_type()) {
        case node_type::SEQUENCE: {
            auto& parent_seq = parent_node.as_seq();
            parent_seq.emplace_back(std::move(node));
            apply_anchor(parent_seq.back(), anchor);
            break;
        }
        case node_type::MAPPING: {
            if (!parent_slot.has_mapping_key()) {
                apply_anchor(node, anchor);
                parent_slot.set_mapping_key(std::move(node));
                break;
            }

            auto& parent_map = parent_node.as_map();
            const auto result = parent_map.emplace(parent_slot.take_mapping_key(), std::move(node));
            if FK_YAML_UNLIKELY (!result.second) {
                throw parse_error("Detected duplication in mapping keys.", 0, 0);
            }
            apply_anchor(result.first->second, anchor);
            break;
        }
        default:
            // Handle other types or error
            break;
        }
    }

    BasicNodeType* mp_root {nullptr};
    std::vector<BasicNodeType>* mp_roots {nullptr};
    bool m_has_root {false};
    std::shared_ptr<document_metainfo<BasicNodeType>> mp_meta;
    std::stack<node_slot> m_node_stack;
};

FK_YAML_DETAIL_NAMESPACE_END

#endif // FKYAML_DETAIL_INPUT_NODE_BUILDER_HPP
