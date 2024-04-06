//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_YAML_TEST_SUITE_RUNNER_STREAM_EVENT_CALLBACK_HPP
#define FK_YAML_YAML_TEST_SUITE_RUNNER_STREAM_EVENT_CALLBACK_HPP

#include <cassert>
#include <stack>
#include <vector>

#include <fkYAML/node.hpp>

class stream_event_callback {
public:
    using str_view = fkyaml::detail::str_view;

    stream_event_callback(std::vector<fkyaml::node>& docs) noexcept
        : m_documents(docs) {
    }

    ~stream_event_callback() noexcept = default;

    void on_stream_begin() {
    }

    void on_stream_end() {
    }

    void on_document_begin() {
        m_documents.emplace_back(fkyaml::node());
        m_context_stack = {}; // clear at first.
        m_context_stack.emplace(&(m_documents.back()), event_state::UNKNOWN);
    }

    void on_document_end() {
        // empty document will leave an unknown event state.
        assert(
            m_context_stack.empty() ||
            (m_context_stack.size() == 1 && m_context_stack.top().state & event_state::UNKNOWN));
    }

    void on_seq_begin(str_view anchor, str_view tag) {
        *(m_context_stack.top().p_node) = fkyaml::node(fkyaml::node_type::SEQUENCE);
        apply_anchor_and_tag(*(m_context_stack.top().p_node), anchor, tag);

        m_context_stack.top().state |= event_state::SEQ;
    }

    void on_seq_end() {
        assert(m_context_stack.top().state & event_state::SEQ);

        if (m_context_stack.top().state & event_state::KEY) {
            fkyaml::node* p_seq_key = nullptr;
            std::swap(p_seq_key, m_context_stack.top().p_node);
            m_context_stack.pop();

            assert(m_context_stack.top().state & event_state::MAP);
            auto& map = m_context_stack.top().p_node->get_value_ref<fkyaml::node::mapping_type&>();
            auto ret = map.emplace(std::move(*p_seq_key), fkyaml::node());
            assert(ret.second);

            delete p_seq_key;
            p_seq_key = nullptr;

            m_context_stack.emplace(&(ret.first->second), event_state::VAL);
        }
        else {
            m_context_stack.pop();
        }
    }

    void on_map_begin(str_view anchor, str_view tag) {
        *(m_context_stack.top().p_node) = fkyaml::node(fkyaml::node_type::MAPPING);
        apply_anchor_and_tag(*(m_context_stack.top().p_node), anchor, tag);

        m_context_stack.top().state |= event_state::MAP;

        m_context_stack.emplace(new fkyaml::node(), event_state::KEY);
    }

    void on_map_end() {
        assert(m_context_stack.top().state & event_state::KEY);

        // delete the empty dummy key first
        delete m_context_stack.top().p_node;
        m_context_stack.top().p_node = nullptr;
        m_context_stack.pop();

        assert(m_context_stack.top().state & event_state::MAP);

        if (m_context_stack.top().state & event_state::KEY) {
            fkyaml::node* p_map_key = nullptr;
            std::swap(p_map_key, m_context_stack.top().p_node);
            m_context_stack.pop();

            assert(m_context_stack.top().state & event_state::MAP);
            auto& map = m_context_stack.top().p_node->get_value_ref<fkyaml::node::mapping_type&>();
            auto ret = map.emplace(std::move(*p_map_key), fkyaml::node());
            assert(ret.second);

            delete p_map_key;
            p_map_key = nullptr;

            m_context_stack.emplace(&(ret.first->second), event_state::VAL);
        }
        else {
            m_context_stack.pop();
        }
    }

    void on_scalar(str_view scalar, str_view anchor, str_view tag) {
        fkyaml::node scalar_node(std::string(scalar.begin(), scalar.end()));
        apply_anchor_and_tag(*(m_context_stack.top().p_node), anchor, tag);

        uint32_t& state = m_context_stack.top().state;
        if (state & event_state::SEQ) {
            auto& seq = m_context_stack.top().p_node->get_value_ref<fkyaml::node::sequence_type&>();
            seq.emplace_back(std::move(scalar_node));
            return;
        }
        else if (state & event_state::KEY) {
            fkyaml::node* p_key_scalar_node = nullptr;
            std::swap(p_key_scalar_node, m_context_stack.top().p_node);
            *p_key_scalar_node = std::move(scalar_node);

            m_context_stack.pop();
            assert(m_context_stack.top().state & event_state::MAP);

            auto& map = m_context_stack.top().p_node->get_value_ref<fkyaml::node::mapping_type&>();
            auto ret = map.emplace(std::move(*p_key_scalar_node), fkyaml::node());
            assert(ret.second);

            m_context_stack.emplace(&(ret.first->second), event_state::VAL);
        }
        else if (state & event_state::VAL) {
            *(m_context_stack.top().p_node) = std::move(scalar_node);

            m_context_stack.pop();
            assert(m_context_stack.top().state & event_state::MAP);

            m_context_stack.emplace(new fkyaml::node(), event_state::KEY);
        }
        else {
            // root scalar
            assert(m_context_stack.size() == 1);
            *(m_context_stack.top().p_node) = std::move(scalar_node);
            m_context_stack.pop();
        }
    }

    void on_alias(str_view anchor) {
        // TODO
        (void)anchor;
    }

private:
    void apply_anchor_and_tag(fkyaml::node& node, str_view anchor, str_view tag) {
        // TODO
        (void)anchor;

        if (!tag.empty()) {
            node.add_tag_name(std::string(tag.begin(), tag.end()));
        }
    }

    struct event_state {
        static constexpr uint32_t UNKNOWN = 0;
        static constexpr uint32_t MAP = 1u << 0u;
        static constexpr uint32_t SEQ = 1u << 1u;
        static constexpr uint32_t KEY = 1u << 2u;
        static constexpr uint32_t VAL = 1u << 3u;
    };

    struct context {
        context() noexcept = default;

        context(fkyaml::node* p_node, uint32_t state) noexcept
            : p_node(p_node),
              state(state) {
        }

        ~context() {
            if (state & event_state::KEY) {
                assert(p_node == nullptr);
            }
        }

        fkyaml::node* p_node {nullptr};
        uint32_t state {event_state::UNKNOWN};
    };

    std::vector<fkyaml::node>& m_documents;
    std::stack<context> m_context_stack {};
};

#endif /* FK_YAML_YAML_TEST_SUITE_RUNNER_STREAM_EVENT_CALLBACK_HPP */
