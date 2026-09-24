//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.5.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_INPUT_DESERIALIZER_HPP
#define FK_YAML_DETAIL_INPUT_DESERIALIZER_HPP

#include <algorithm>
#include <deque>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/input/lexical_analyzer.hpp>
#include <fkYAML/detail/input/node_builder.hpp>
#include <fkYAML/detail/meta/input_adapter_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/types/lexical_token_t.hpp>
#include <fkYAML/detail/chunked_arena.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief Definition of state types of parse contexts.
enum class context_state_t : std::uint8_t {
    BLOCK_MAPPING,                //!< The underlying node is a block mapping.
    BLOCK_MAPPING_EXPLICIT_KEY,   //!< The underlying node is an explicit block mapping key.
    BLOCK_MAPPING_EXPLICIT_VALUE, //!< The underlying node is an explicit block mapping value.
    MAPPING_VALUE,                //!< The underlying node is a block mapping value.
    BLOCK_SEQUENCE,               //!< The underlying node is a block sequence.
    BLOCK_SEQUENCE_ENTRY,         //!< The underlying node is a block sequence entry.
    FLOW_SEQUENCE,                //!< The underlying node is a flow sequence.
    FLOW_SEQUENCE_KEY,            //!< The underlying node is a flow sequence as a key.
    FLOW_MAPPING,                 //!< The underlying node is a flow mapping.
    FLOW_MAPPING_KEY,             //!< The underlying node is a flow mapping as a key.
    FLOW_MAPPING_EXPLICIT_KEY,    //!< The underlying node is an explicit key in a flow collection.
};

/// @brief Definitions of state types for expected flow token hints.
enum class flow_token_state_t : std::uint8_t {
    NEEDS_VALUE_OR_SUFFIX,     //!< Either value or flow suffix (`]` or `}`)
    NEEDS_SEPARATOR_OR_SUFFIX, //!< Either separator (`,`) or flow suffix (`]` or `}`)
};

/// @brief State management for flow context during YAML parsing.
class flow_context_state {
public:
    /// @brief Check whether the flow context is currently active.
    /// @return True if the flow context is active, false otherwise.
    bool is_active() const noexcept {
        return m_depth > 0;
    }

    int32_t get_base_indent() const noexcept {
        return m_base_indent;
    }

    bool needs_separator_or_suffix() const noexcept {
        return m_token_state == flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX;
    }

    bool needs_value_or_suffix() const noexcept {
        return m_token_state == flow_token_state_t::NEEDS_VALUE_OR_SUFFIX;
    }

    void set_token_state(flow_token_state_t state) noexcept {
        m_token_state = state;
    }

    /// @brief Begin a new flow context.
    /// @note This should be called when entering a flow collection.
    void begin() noexcept {
        ++m_depth;
        m_token_state = flow_token_state_t::NEEDS_VALUE_OR_SUFFIX;
    }

    /// @brief Begin a new flow context with a specified parent indentation.
    /// @param parent_indent The indentation level of the parent context.
    void begin(const uint32_t parent_indent) {
        if (m_depth == 0) {
            m_base_indent = static_cast<int32_t>(parent_indent);
        }
        ++m_depth;
        m_token_state = flow_token_state_t::NEEDS_VALUE_OR_SUFFIX;
    }

    /// @brief End the current flow context.
    /// @return True if the flow context was active and is now ended, false otherwise.
    bool end() noexcept {
        if (--m_depth == 0) {
            m_base_indent = -1;
            return true;
        }
        return false;
    }

    void reset() noexcept {
        m_depth = 0;
        m_base_indent = -1;
        m_token_state = flow_token_state_t::NEEDS_VALUE_OR_SUFFIX;
    }

private:
    /// The current depth of flow contexts.
    uint32_t m_depth {0};
    /// The indentation the contents of the outermost flow context must exceed, or -1 if unconstrained.
    int32_t m_base_indent {-1};
    /// A flag to determine the need for a value separator or a flow suffix to follow.
    flow_token_state_t m_token_state {flow_token_state_t::NEEDS_VALUE_OR_SUFFIX};
};

/// @brief Node properties waiting to be applied.
struct pending_node_properties {
    /// @brief Check whether an anchor name is stored.
    bool has_anchor() const noexcept {
        return !m_anchor_name.empty();
    }

    /// @brief Check whether a tag name is stored.
    bool has_tag() const noexcept {
        return !m_tag_name.empty();
    }

    /// @brief Store an anchor name.
    void store_anchor(const str_view name) noexcept {
        m_anchor_name = name;
    }

    /// @brief Store a tag name.
    void store_tag(const str_view name) noexcept {
        m_tag_name = name;
    }

    /// @brief Get the stored anchor name.
    /// @return The stored anchor name.
    str_view get_anchor() const noexcept {
        return m_anchor_name;
    }

    /// @brief Get the stored tag name.
    /// @return The stored tag name.
    str_view get_tag() const noexcept {
        return m_tag_name;
    }

    /// @brief Release the stored anchor name.
    str_view release_anchor() noexcept {
        const str_view name = get_anchor();
        m_anchor_name = {};
        return name;
    }

    /// @brief Release the stored tag name.
    str_view release_tag() noexcept {
        const str_view name = get_tag();
        m_tag_name = {};
        return name;
    }

private:
    /// The pending anchor name.
    str_view m_anchor_name;
    /// The pending tag name.
    str_view m_tag_name;
};

using node_id = std::uint32_t;
static constexpr node_id invalid_node_id = std::numeric_limits<node_id>::max();

enum class kind_type : std::uint8_t {
    EMPTY,
    SCALAR,
    ALIAS,
    SEQUENCE,
    MAPPING,
};

struct buffered_event {
    kind_type kind {kind_type::EMPTY};
    str_view tag;
    str_view anchor;

    lexical_token_t scalar_type {lexical_token_t::PLAIN_SCALAR};
    str_view scalar_value;
    block_scalar_header scalar_header {};

    collection_type collection_style {collection_type::BLOCK};
    node_id first_child_id {invalid_node_id};
    node_id last_child_id {invalid_node_id};
    node_id next_sibling_id {invalid_node_id};
    node_id streaming_mapping_value_id {invalid_node_id};

    bool is_streaming {false};
    bool is_released {false};
};

class event_arena {
public:
    node_id create() {
        const node_id id = m_arena.allocate();
        m_arena.get(id) = buffered_event {};
        return id;
    }

    buffered_event& get(node_id id) {
        return m_arena.get(id);
    }

    const buffered_event& get(node_id id) const {
        return m_arena.get(id);
    }

    void append_child(const node_id parent_id, const node_id child_id) {
        buffered_event& parent = m_arena.get(parent_id);
        if (parent.first_child_id == invalid_node_id) {
            parent.first_child_id = child_id;
        }
        else {
            m_arena.get(parent.last_child_id).next_sibling_id = child_id;
        }
        parent.last_child_id = child_id;
    }

    void release_subtree(const node_id root_id) {
        node_id child_id = m_arena.get(root_id).first_child_id;
        while (child_id != invalid_node_id) {
            const auto& child_event = m_arena.get(child_id);
            const node_id next_id = child_event.next_sibling_id;
            release_subtree(child_id);
            child_id = next_id;
        }
        m_arena.deallocate(root_id);
    }

    void replace(const node_id destination_id, const node_id source_id) {
        FK_YAML_ASSERT(destination_id != source_id);

        buffered_event& destination_event = m_arena.get(destination_id);
        node_id child_id = destination_event.first_child_id;
        while (child_id != invalid_node_id) {
            const node_id next_id = m_arena.get(child_id).next_sibling_id;
            release_subtree(child_id);
            child_id = next_id;
        }

        destination_event = m_arena.get(source_id);
        m_arena.deallocate(source_id);
    }

    void swap(const node_id lhs, const node_id rhs) noexcept {
        using std::swap;
        swap(m_arena.get(lhs), m_arena.get(rhs));
    }

    void clear() {
        m_arena.clear();
    }

private:
    chunked_arena<buffered_event, 64> m_arena;
};

template <typename EventHandler>
class event_node {
public:
    explicit event_node(event_arena& arena)
        : mp_arena(&arena),
          m_id(mp_arena->create()) {
    }

    event_node(event_arena* p_arena, node_id id)
        : mp_arena(p_arena),
          m_id(id) {
    }

    event_node(const event_node& other) noexcept
        : mp_arena(other.mp_arena),
          m_id(other.m_id),
          mp_handler(other.mp_handler) {
    }

    event_node(event_node&&) noexcept = default;

    event_node& operator=(event_node&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        FK_YAML_ASSERT(mp_arena == other.mp_arena);
        mp_arena->replace(m_id, other.m_id);
        get_event().is_streaming = false;

        other.mp_arena = nullptr;
        other.m_id = invalid_node_id;
        return *this;
    }

    static event_node block_sequence(event_arena& arena) {
        event_node node(arena);
        auto& event = node.get_event();
        event.kind = kind_type::SEQUENCE;
        event.collection_style = collection_type::BLOCK;
        return node;
    }

    static event_node flow_sequence(event_arena& arena) {
        event_node node(arena);
        auto& event = node.get_event();
        event.kind = kind_type::SEQUENCE;
        event.collection_style = collection_type::FLOW;
        return node;
    }

    static event_node block_mapping(event_arena& arena) {
        event_node node(arena);
        auto& event = node.get_event();
        event.kind = kind_type::MAPPING;
        event.collection_style = collection_type::BLOCK;
        return node;
    }

    static event_node flow_mapping(event_arena& arena) {
        event_node node(arena);
        auto& event = node.get_event();
        event.kind = kind_type::MAPPING;
        event.collection_style = collection_type::FLOW;
        return node;
    }

    static event_node scalar(
        event_arena& arena, const lexical_token_t type, const str_view value, const block_scalar_header& header = {}) {
        event_node node(arena);
        auto& event = node.get_event();
        event.kind = kind_type::SCALAR;
        event.scalar_type = type;
        event.scalar_value = value;
        event.scalar_header = header;
        return node;
    }

    static event_node alias(event_arena& arena, const str_view value) {
        event_node node(arena);
        auto& event = node.get_event();
        event.kind = kind_type::ALIAS;
        event.scalar_value = value;
        return node;
    }

    event_node reference() const noexcept {
        event_node node(mp_arena, m_id);
        node.mp_handler = mp_handler;
        return node;
    }

    bool is_sequence() const noexcept {
        return get_event().kind == kind_type::SEQUENCE;
    }

    bool is_mapping() const noexcept {
        return get_event().kind == kind_type::MAPPING;
    }

    bool is_scalar() const noexcept {
        switch (get_event().kind) {
        case kind_type::EMPTY:
        case kind_type::SCALAR:
        case kind_type::ALIAS:
            return true;
        default:
            return false;
        }
    }

    bool is_alias() const noexcept {
        return get_event().kind == kind_type::ALIAS;
    }

    void set_handler(EventHandler* handler) noexcept {
        mp_handler = handler;
    }

    bool has_handler() const noexcept {
        return mp_handler != nullptr;
    }

    void start_streaming() {
        auto& event = get_event();
        if (mp_handler == nullptr || event.is_streaming || event.collection_style == collection_type::FLOW) {
            return;
        }

        switch (event.kind) {
        case kind_type::SEQUENCE:
            mp_handler->on_sequence_start({event.tag, event.anchor, event.collection_style});
            break;
        case kind_type::MAPPING:
            mp_handler->on_mapping_start({event.tag, event.anchor, event.collection_style});
            break;
        default:
            return;
        }
        event.is_streaming = true;
    }

    void release() {
        if (mp_handler == nullptr || get_event().is_released) {
            return;
        }

        if (get_event().is_streaming) {
            finish_streaming();
        }
        else {
            dispatch(*mp_handler);
        }
        mp_handler = nullptr;
        get_event().is_released = true;
    }

    event_node get_last_sequence_element() {
        return event_node(mp_arena, get_event().last_child_id);
    }

    event_node add_sequence_entry(event_node&& node) {
        auto& event = get_event();
        if (event.is_streaming && event.first_child_id != invalid_node_id && event.last_child_id != invalid_node_id) {
            const node_id child_id = event.last_child_id;
            event_node child(mp_arena, child_id);
            child.set_handler(mp_handler);
            child.release();
            mp_arena->release_subtree(child_id);
            auto& event = get_event();
            event.first_child_id = invalid_node_id;
            event.last_child_id = invalid_node_id;
        }
        node.set_handler(event.is_streaming ? mp_handler : nullptr);
        mp_arena->append_child(m_id, node.m_id);
        return event_node(mp_arena, event.last_child_id);
    }

    event_node add_mapping_entry(event_node&& key) {
        auto& event = get_event();
        if (event.is_streaming) {
            if (event.streaming_mapping_value_id != invalid_node_id) {
                event_node value(mp_arena, event.streaming_mapping_value_id);
                value.set_handler(mp_handler);
                value.release();
                mp_arena->get(event.streaming_mapping_value_id) = buffered_event {};
            }
            else {
                event.streaming_mapping_value_id = mp_arena->create();
            }

            key.set_handler(mp_handler);
            key.release();
            mp_arena->release_subtree(key.m_id);
            key.m_id = invalid_node_id;
            key.mp_arena = nullptr;

            event_node value(mp_arena, event.streaming_mapping_value_id);
            value.set_handler(mp_handler);
            return value;
        }

        mp_arena->append_child(m_id, key.m_id);
        auto value_id = mp_arena->create();
        mp_arena->append_child(m_id, value_id);
        return event_node(mp_arena, value_id);
    }

    void swap(event_node& other) noexcept {
        FK_YAML_ASSERT(mp_arena == other.mp_arena);
        mp_arena->swap(m_id, other.m_id);
    }

    void set_tag(const str_view tag) {
        get_event().tag = tag;
    }

    void set_anchor(const str_view anchor) {
        get_event().anchor = anchor;
    }

    void dispatch(EventHandler& handler) const {
        dispatch_events(*mp_arena, m_id, handler);
    }

private:
    buffered_event& get_event() {
        return mp_arena->get(m_id);
    }

    const buffered_event& get_event() const {
        return mp_arena->get(m_id);
    }

    void finish_streaming() {
        auto& event = get_event();
        switch (event.kind) {
        case kind_type::SEQUENCE:
            if (event.first_child_id != invalid_node_id && event.last_child_id != invalid_node_id) {
                const node_id child_id = event.last_child_id;
                event_node child(mp_arena, child_id);
                child.set_handler(mp_handler);
                child.release();
                mp_arena->release_subtree(child_id);
                event.first_child_id = invalid_node_id;
                event.last_child_id = invalid_node_id;
            }
            mp_handler->on_sequence_end({});
            break;
        case kind_type::MAPPING:
            if (event.streaming_mapping_value_id != invalid_node_id) {
                const node_id value_id = event.streaming_mapping_value_id;
                event_node value(mp_arena, value_id);
                value.set_handler(mp_handler);
                value.release();
                mp_arena->release_subtree(value_id);
                event.streaming_mapping_value_id = invalid_node_id;
            }
            mp_handler->on_mapping_end({});
            break;
        default:
            break;
        }
    }

    void dispatch_events(const event_arena& arena, node_id root, EventHandler& handler) const {
        const buffered_event& event = arena.get(root);
        switch (event.kind) {
        case kind_type::EMPTY:
            handler.on_plain_scalar({{}, event.tag, event.anchor});
            break;
        case kind_type::SCALAR:
            dispatch_scalar_event(handler, event);
            break;
        case kind_type::ALIAS:
            handler.on_alias({event.scalar_value});
            break;
        case kind_type::SEQUENCE: {
            handler.on_sequence_start({event.tag, event.anchor, event.collection_style});

            node_id child_id = event.first_child_id;
            while (child_id != invalid_node_id) {
                dispatch_events(arena, child_id, handler);
                child_id = arena.get(child_id).next_sibling_id;
            }

            handler.on_sequence_end({});
            break;
        }
        case kind_type::MAPPING:
            handler.on_mapping_start({event.tag, event.anchor, event.collection_style});

            node_id child_id = event.first_child_id;
            while (child_id != invalid_node_id) {
                node_id key_id = child_id;
                node_id value_id = arena.get(key_id).next_sibling_id;
                dispatch_events(arena, key_id, handler);
                dispatch_events(arena, value_id, handler);
                child_id = arena.get(value_id).next_sibling_id;
            }

            handler.on_mapping_end({});
            break;
        }
    }

    void dispatch_scalar_event(EventHandler& handler, const buffered_event& event) const {
        switch (event.scalar_type) {
        case lexical_token_t::SINGLE_QUOTED_SCALAR:
            handler.on_single_quoted_scalar({event.scalar_value, event.tag, event.anchor});
            break;
        case lexical_token_t::DOUBLE_QUOTED_SCALAR:
            handler.on_double_quoted_scalar({event.scalar_value, event.tag, event.anchor});
            break;
        case lexical_token_t::BLOCK_LITERAL_SCALAR:
            handler.on_literal_scalar({event.scalar_value, event.tag, event.anchor, event.scalar_header});
            break;
        case lexical_token_t::BLOCK_FOLDED_SCALAR:
            handler.on_folded_scalar({event.scalar_value, event.tag, event.anchor, event.scalar_header});
            break;
        default:
            handler.on_plain_scalar({event.scalar_value, event.tag, event.anchor});
            break;
        }
    }

    event_arena* mp_arena;
    node_id m_id {invalid_node_id};
    EventHandler* mp_handler {nullptr};
};

/// @brief A class which provides the feature of deserializing YAML documents.
/// @tparam EventHandler A type which handles deserialization events.
template <typename EventHandler>
class basic_deserializer {
    /** A type for a buffered YAML node event sequence. */
    using event_node_type = event_node<EventHandler>;
    /** A type for the lexical analyzer. */
    using lexer_type = lexical_analyzer;

    /// @brief Context information set for parsing.
    /// @note
    /// A context stores an arena node handle. Detached keys remain arena-owned and are tracked separately until
    /// they are attached to their parent collection.
    struct parse_context {
        /// @brief Construct a new parse_context object.
        parse_context() = default;

        /// @brief Construct a new parse_context object which borrows a node owned by the result tree.
        /// @param line The current line. (count from zero)
        /// @param indent The indentation width in the current line. (count from zero)
        /// @param state The parse context type.
        /// @param p_node The underlying node associated to this context.
        parse_context(uint32_t line, uint32_t indent, context_state_t state, event_node_type* p_node) noexcept
            : line(line),
              indent(indent),
              state(state),
              node(p_node->reference()),
              is_explicit_key(
                  state == context_state_t::BLOCK_MAPPING_EXPLICIT_KEY ||
                  state == context_state_t::FLOW_MAPPING_EXPLICIT_KEY) {
        }

        parse_context(uint32_t line, uint32_t indent, context_state_t state, event_node_type&& node) noexcept
            : line(line),
              indent(indent),
              state(state),
              node(node.reference()),
              owns_detached_node(true),
              is_explicit_key(
                  state == context_state_t::BLOCK_MAPPING_EXPLICIT_KEY ||
                  state == context_state_t::FLOW_MAPPING_EXPLICIT_KEY) {
        }

        // Parse contexts are move-only so that the ownership of an owned node cannot be duplicated.
        parse_context(const parse_context&) = delete;
        parse_context& operator=(const parse_context&) = delete;
        parse_context(parse_context&&) noexcept = default;
        parse_context& operator=(parse_context&&) noexcept = default;
        ~parse_context() = default;

        /// The current line. (count from zero)
        uint32_t line {0};
        /// The indentation width in the current line. (count from zero)
        uint32_t indent {0};
        /// The parse context type.
        context_state_t state {context_state_t::BLOCK_MAPPING};
        /// The arena node associated with this context.
        event_node_type node {};
        /// Whether the node has not yet been attached to its parent collection.
        bool owns_detached_node {false};
        /// Whether this context originated as an explicit mapping key.
        bool is_explicit_key {false};
        /// The node properties which precede the flow collection of this context on an earlier line.
        pending_node_properties held_props {};
    };

    /// @brief Represents the state of a YAML document during parsing.
    class document_state {
    public:
        /// @brief Initialize the document state.
        /// @param last_token_type The type of the last lexical token encountered.
        void initialize(const lexical_token_t last_token_type) {
            m_has_yaml_version = false;
            m_tag_handles.clear();
            m_has_document = false;
            m_expects_root_flow_key_separator = false;
            if (last_token_type != lexical_token_t::END_OF_DIRECTIVES) {
                m_explicit_document_start_line = std::numeric_limits<uint32_t>::max();
            }
        }

        bool has_yaml_version() const noexcept {
            return m_has_yaml_version;
        }

        void has_yaml_version(const bool has_version) noexcept {
            m_has_yaml_version = has_version;
        }

        bool has_tag_handle(const str_view& handle) const noexcept {
            for (const auto& tag_handle : m_tag_handles) {
                if (handle == tag_handle) {
                    return true;
                }
            }
            return false;
        }

        bool add_tag_handle(const str_view& handle) {
            m_tag_handles.emplace_back(handle);
            return true;
        }

        bool has_explicit_document_start() const noexcept {
            return m_explicit_document_start_line != std::numeric_limits<uint32_t>::max();
        }

        /// @brief Check if the document being parsed exists at all: it has contents or an explicit "---".
        /// @return True if the document exists, false otherwise.
        bool has_document() const noexcept {
            return m_has_document;
        }

        /// @brief Set whether the document being parsed exists.
        /// @param has_document True if the document exists, false otherwise.
        void has_document(const bool has_document) noexcept {
            m_has_document = has_document;
        }

        /// @brief Check if a provisional root mapping still requires a separator after its flow collection key.
        /// @return True if a separator is expected, false otherwise.
        bool expects_root_flow_key_separator() const noexcept {
            return m_expects_root_flow_key_separator;
        }

        /// @brief Set whether a provisional root mapping still requires a separator after its flow collection key.
        /// @param expects_root_flow_key_separator True if a separator is expected, false otherwise.
        void expects_root_flow_key_separator(const bool expects_root_flow_key_separator) noexcept {
            m_expects_root_flow_key_separator = expects_root_flow_key_separator;
        }

        /// @brief Set the line where the explicit document start marker was found.
        /// @param line The line number of the explicit document start marker.
        void set_explicit_document_start_line(const uint32_t line) noexcept {
            m_explicit_document_start_line = line;
        }

        /// @brief Check if the given line is the line where the explicit document start marker was found.
        /// @param line The line number to check.
        /// @return True if the given line is the explicit document start line, false otherwise.
        bool is_on_explicit_document_start_line(const uint32_t line) const noexcept {
            return m_explicit_document_start_line != std::numeric_limits<uint32_t>::max() &&
                   m_explicit_document_start_line == line;
        }

    private:
        std::vector<str_view> m_tag_handles {};
        bool m_has_yaml_version {false};
        /// Whether the document being parsed exists at all: it has contents or an explicit "---".
        bool m_has_document {false};
        /// Whether a provisional root mapping still requires a separator after its flow collection key.
        bool m_expects_root_flow_key_separator {false};
        /// The line where the explicit document start marker was found.
        uint32_t m_explicit_document_start_line {std::numeric_limits<uint32_t>::max()};
    };

public:
    /// @brief Construct a new basic_deserializer object.
    basic_deserializer() = default;

public:
    /// @brief Deserialize a single YAML document and return a value built by node_builder.
    template <typename InputAdapterType, enable_if_t<is_input_adapter<InputAdapterType>::value, int> = 0>
    EventHandler deserialize(InputAdapterType&& input_adapter) { // NOLINT(cppcoreguidelines-missing-std-forward)
        EventHandler root;
        node_builder<EventHandler> builder(root);
        basic_deserializer<node_builder<EventHandler>>().deserialize(std::move(input_adapter), builder);
        return root;
    }

    /// @brief Deserialize multiple YAML documents and return values built by node_builder.
    template <typename InputAdapterType, enable_if_t<is_input_adapter<InputAdapterType>::value, int> = 0>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
    std::vector<EventHandler> deserialize_docs(InputAdapterType&& input_adapter) {
        std::vector<EventHandler> roots;
        node_builder<EventHandler> builder(roots);
        basic_deserializer<node_builder<EventHandler>>().deserialize_docs(std::move(input_adapter), builder);
        return roots;
    }

    /// @brief Deserialize a single YAML document and report its events.
    /// @note
    /// If the input consists of multiple YAML documents, this function only parses the first.
    /// If the input may have multiple YAML documents all of which must be parsed into nodes,
    /// prefer the `deserialize_docs()` function.
    /// @tparam InputAdapterType The type of an input adapter object.
    /// @param input_adapter An input adapter object for the input source buffer.
    /// @param handler The handler which receives deserialization events.
    template <typename InputAdapterType, enable_if_t<is_input_adapter<InputAdapterType>::value, int> = 0>
    void deserialize(
        InputAdapterType&& input_adapter,
        EventHandler& handler) { // NOLINT(cppcoreguidelines-missing-std-forward)
        const str_view input_view = input_adapter.get_buffer_view();
        lexer_type lexer(input_view);

        handler.on_stream_start({});
        lexical_token_t type {lexical_token_t::END_OF_BUFFER};
        deserialize_document(lexer, type, handler, true);
        handler.on_stream_end({});
    }

    /// @brief Deserialize multiple YAML documents into YAML nodes.
    /// @tparam InputAdapterType The type of an adapter object.
    /// @param input_adapter An input adapter object for the input source buffer.
    /// @param handler The handler which receives deserialization events.
    template <typename InputAdapterType, enable_if_t<is_input_adapter<InputAdapterType>::value, int> = 0>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
    void deserialize_docs(InputAdapterType&& input_adapter, EventHandler& handler) {
        const str_view input_view = input_adapter.get_buffer_view();
        lexer_type lexer(input_view);

        handler.on_stream_start({});
        lexical_token_t type {lexical_token_t::END_OF_BUFFER};
        bool begun_by_marker = false;

        do {
            deserialize_document(lexer, type, handler, begun_by_marker);
            // A "---" which ends a document begins the next one, even if that one is empty.
            begun_by_marker = (type == lexical_token_t::END_OF_DIRECTIVES);
            // Break the loop if the last end-of-document marker is followed by the end-of-buffer token,
            // which indicates that there are no more documents to parse.
            // ```yaml
            // foo: bar
            // ... # the last end-of-document marker
            // # no more documents after this line
            // ```
            if (type == lexical_token_t::END_OF_DOCUMENT) {
                // A next document may start from the directive part. Ensure '%' is lexed as a directive token
                // during the lookahead; otherwise it can be cached as a plain scalar and break parsing.
                lexer.enter_directives();
                const lexical_token_t next_type = lexer.peek_next_token().type;
                if (next_type == lexical_token_t::END_OF_BUFFER) {
                    break;
                }
            }
        } while (type != lexical_token_t::END_OF_BUFFER);
        handler.on_stream_end({});
    } // LCOV_EXCL_LINE

private:
    /// @brief Deserialize a YAML document into a YAML node.
    /// @param lexer The lexical analyzer to be used.
    /// @param last_type The variable to store the last lexical token type.
    /// @param handler The handler which receives committed deserialization events.
    /// @param emit_empty_document Whether an empty document must still be reported.
    void deserialize_document(
        lexer_type& lexer, lexical_token_t& last_type, EventHandler& handler, const bool emit_empty_document) {
        lexical_token token {};

        m_document_state.initialize(last_type);

        event_node_type root(m_arena);
        mp_current_node = &root;

        // parse directives first.
        deserialize_directives(lexer, token, handler);

        // parse node properties for root node if any
        uint32_t line = lexer.get_lines_processed();
        uint32_t indent = lexer.get_last_token_begin_pos();
        const bool found_props = deserialize_node_properties(lexer, token, line, indent);

        // A stream which only holds comments, white spaces or a bare "..." contains no document. Node
        // properties on their own, however, do make up one: they belong to an empty scalar.
        // ```yaml
        // !
        // # -> one document holding an empty scalar tagged with the non-specific tag
        // ```
        const bool has_contents =
            token.type != lexical_token_t::END_OF_BUFFER && token.type != lexical_token_t::END_OF_DOCUMENT;
        if (has_contents || found_props) {
            m_document_state.has_document(true);
        }
        const bool emits_document = m_document_state.has_document() || emit_empty_document;
        if (emits_document) {
            handler.on_document_start({m_document_state.has_explicit_document_start()});
            root.set_handler(&handler);
        }

        switch (token.type) {
        case lexical_token_t::SEQUENCE_BLOCK_PREFIX: {
            if FK_YAML_UNLIKELY (m_document_state.is_on_explicit_document_start_line(lexer.get_lines_processed())) {
                throw parse_error("A block sequence entry cannot be on the document start line.", line, indent);
            }
            if FK_YAML_UNLIKELY (found_props && line == lexer.get_lines_processed()) {
                throw parse_error(
                    "Node properties cannot precede a block sequence entry on the same line.", line, indent);
            }
            check_tab_in_indentation(lexer, lexer.get_lines_processed(), lexer.get_last_token_begin_pos());
            root = event_node_type::block_sequence(m_arena);
            if (found_props) {
                // If node properties are found before the block sequence entry prefix, the properties belong to the
                // root sequence node.
                apply_deferred_properties(root);
                apply_node_properties(root);
            }
            root.start_streaming();
            root.add_sequence_entry(event_node_type(m_arena));

            const uint32_t seq_line = lexer.get_lines_processed();
            const uint32_t seq_indent = lexer.get_last_token_begin_pos();
            m_context_stack.emplace_back(seq_line, seq_indent, context_state_t::BLOCK_SEQUENCE, &root);

            event_node_type entry = root.get_last_sequence_element();
            m_context_stack.emplace_back(seq_line, seq_indent, context_state_t::BLOCK_SEQUENCE_ENTRY, &entry);
            mp_current_node = &m_context_stack.back().node;

            token = lexer.get_next_token();
            line = lexer.get_lines_processed();
            indent = lexer.get_last_token_begin_pos();
            break;
        }
        case lexical_token_t::SEQUENCE_FLOW_BEGIN:
            m_flow_context_state.begin();
            lexer.enter_flow_context();
            root = event_node_type::flow_sequence(m_arena);
            if (found_props && line < lexer.get_lines_processed()) {
                defer_node_properties();
            }
            apply_node_properties(root);
            m_context_stack.emplace_back(
                lexer.get_lines_processed(), lexer.get_last_token_begin_pos(), context_state_t::FLOW_SEQUENCE, &root);
            m_context_stack.back().held_props = take_deferred_properties();
            token = lexer.get_next_token();
            line = lexer.get_lines_processed();
            indent = lexer.get_last_token_begin_pos();
            break;
        case lexical_token_t::MAPPING_FLOW_BEGIN:
            m_flow_context_state.begin();
            lexer.enter_flow_context();
            root = event_node_type::flow_mapping(m_arena);
            if (found_props && line < lexer.get_lines_processed()) {
                defer_node_properties();
            }
            apply_node_properties(root);
            m_context_stack.emplace_back(
                lexer.get_lines_processed(), lexer.get_last_token_begin_pos(), context_state_t::FLOW_MAPPING, &root);
            m_context_stack.back().held_props = take_deferred_properties();
            token = lexer.get_next_token();
            line = lexer.get_lines_processed();
            indent = lexer.get_last_token_begin_pos();
            break;
        case lexical_token_t::EXPLICIT_KEY_PREFIX: {
            // If the explicit key prefix (? ) is detected here, the root node of current document must be a mapping.
            // Also, tag and anchor if any are associated to the root mapping node.
            // No get_next_token() call here to handle the token event in the deserialize_node() function.
            root = event_node_type::block_mapping(m_arena);
            apply_deferred_properties(root);
            apply_node_properties(root);
            root.start_streaming();
            parse_context context(
                lexer.get_lines_processed(), lexer.get_last_token_begin_pos(), context_state_t::BLOCK_MAPPING, &root);
            m_context_stack.emplace_back(std::move(context));
            line = lexer.get_lines_processed();
            indent = lexer.get_last_token_begin_pos();
            break;
        }
        case lexical_token_t::KEY_SEPARATOR:
            root = event_node_type::block_mapping(m_arena);
            apply_deferred_properties(root);
            root.start_streaming();
            // apply_node_properties(root);
            m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, &root);
            add_empty_key_entry(lexer, token, line, indent);
            break;
        case lexical_token_t::BLOCK_LITERAL_SCALAR:
        case lexical_token_t::BLOCK_FOLDED_SCALAR:
            // If a block scalar token is detected here, current document contains single scalar.
            // Do nothing here since the token is handled in the deserialize_node() function.
            break;
        case lexical_token_t::PLAIN_SCALAR:
        case lexical_token_t::SINGLE_QUOTED_SCALAR:
        case lexical_token_t::DOUBLE_QUOTED_SCALAR:
        case lexical_token_t::ALIAS_PREFIX:
        case lexical_token_t::ANCHOR_PREFIX:
        case lexical_token_t::TAG_PREFIX:
            // Defer handling the above token events until the next call on the deserialize_scalar() function since the
            // meaning depends on subsequent events.
            if (found_props && line < lexer.get_lines_processed()) {
                // If node properties and a followed node are on different lines, defer the properties until the root
                // node type is known.
                defer_node_properties();
                line = lexer.get_lines_processed();
                indent = lexer.get_last_token_begin_pos();
            }
            break;
        default:
            // Do nothing since current document has no contents.
            break;
        }

        // parse YAML nodes recursively
        deserialize_node(lexer, token, line, indent, last_type);
        FK_YAML_ASSERT(
            last_type == lexical_token_t::END_OF_BUFFER || last_type == lexical_token_t::END_OF_DIRECTIVES ||
            last_type == lexical_token_t::END_OF_DOCUMENT);

        materialize_tagged_empty_node(*mp_current_node, line, indent);
        // The current node may be an empty node which never received the document metainfo. An anchor
        // name must be registered in the shared metainfo, or aliases cannot resolve it.
        apply_node_properties(*mp_current_node);
        apply_deferred_properties(*mp_current_node);

        // An explicit key at the end of a document has no value either. Its own contents may have left
        // more contexts on the stack, so those are unwound first.
        // ```yaml
        // ? foo
        // ? bar: baz
        // # -> {foo: null, {bar: baz}: null}
        // ```
        while (!m_context_stack.empty()) {
            if (m_context_stack.back().state == context_state_t::BLOCK_MAPPING_EXPLICIT_KEY) {
                add_explicit_key_with_null_value();
                continue;
            }
            m_context_stack.pop_back();
        }

        if (emits_document) {
            root.release();
            handler.on_document_end({last_type == lexical_token_t::END_OF_DOCUMENT});
        }

        // reset parameters for the next call.
        mp_current_node = nullptr;
        m_pending_properties = {};
        m_flow_context_state.reset();
        m_context_stack.clear();
        m_arena.clear();
    }

    /// @brief Deserializes the YAML directives if specified.
    /// @param lexer The lexical analyzer to be used.
    /// @param last_token Storage for last lexical token type.
    void deserialize_directives(lexer_type& lexer, lexical_token& last_token, EventHandler& handler) {
        bool lacks_end_of_directives_marker = false;
        lexer.enter_directives();

        for (;;) {
            const lexical_token token = lexer.get_next_token();

            switch (token.type) {
            case lexical_token_t::YAML_VER_DIRECTIVE:
                if FK_YAML_UNLIKELY (m_document_state.has_yaml_version()) {
                    throw parse_error(
                        "YAML version cannot be specified more than once.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }

                handler.on_yaml_directive({lexer.get_yaml_version()});
                m_document_state.has_yaml_version(true);
                lacks_end_of_directives_marker = true;
                break;
            case lexical_token_t::TAG_DIRECTIVE: {
                const str_view tag_handle = lexer.get_tag_handle();
                if FK_YAML_UNLIKELY (m_document_state.has_tag_handle(tag_handle)) {
                    throw parse_error(
                        "The same handle cannot be specified more than once by the TAG directives.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }
                handler.on_tag_directive({tag_handle, lexer.get_tag_prefix()});
                m_document_state.add_tag_handle(tag_handle);
                lacks_end_of_directives_marker = true;
                break;
            }
            case lexical_token_t::INVALID_DIRECTIVE:
                // TODO: should output a warning log. Currently just ignore this case.
                break;
            case lexical_token_t::END_OF_DIRECTIVES:
                if (m_document_state.has_document()) {
                    // A "---" which follows another one ends the document that one began and begins the
                    // next, even though the document it ends holds no node at all.
                    // ```yaml
                    // ---
                    // ---
                    // # -> two documents, both empty
                    // ```
                    m_document_state.set_explicit_document_start_line(lexer.get_lines_processed());
                    last_token = token;
                    lexer.exit_directives();
                    return;
                }

                lacks_end_of_directives_marker = false;
                m_document_state.has_document(true);
                m_document_state.set_explicit_document_start_line(lexer.get_lines_processed());
                break;
            default:
                if FK_YAML_UNLIKELY (lacks_end_of_directives_marker) {
                    throw parse_error(
                        "The end of directives marker (---) is missing after directives.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }
                // end the parsing of directives if the other tokens are found.
                last_token = token;
                lexer.exit_directives();
                return;
            }
        }
    }

    /// @brief Deserializes the YAML nodes recursively.
    /// @param lexer The lexical analyzer to be used.
    /// @param first_type The first lexical token.
    /// @param last_type Storage for last lexical token type.
    void deserialize_node(
        lexer_type& lexer, const lexical_token& first_token, uint32_t first_line, uint32_t first_indent,
        lexical_token_t& last_type) {
        lexical_token token = first_token;
        uint32_t line = first_line;
        uint32_t indent = first_indent;

        do {
            if (m_flow_context_state.get_base_indent() >= 0) {
                // The contents of a flow context nested in a block context must be more indented than the block
                // context it belongs to.
                // ```yaml
                // foo: [bar,
                // baz]
                // # ^ this line is not indented enough.
                // ```
                const auto token_indent = static_cast<int32_t>(lexer.get_last_token_begin_pos());
                if FK_YAML_UNLIKELY (token_indent <= m_flow_context_state.get_base_indent()) {
                    throw parse_error(
                        "Contents of a flow context must be more indented than its parent block context.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }
            }

            switch (token.type) {
            case lexical_token_t::EXPLICIT_KEY_PREFIX: {
                if FK_YAML_UNLIKELY (m_context_stack.empty()) {
                    throw parse_error("An explicit key is not allowed in this context.", line, indent);
                }

                if (m_flow_context_state.is_active()) {
                    if FK_YAML_UNLIKELY (m_flow_context_state.needs_separator_or_suffix()) {
                        throw parse_error("An explicit key is found without separated with a comma.", line, indent);
                    }

                    token = lexer.get_next_token();
                    m_context_stack.emplace_back(
                        line, indent, context_state_t::FLOW_MAPPING_EXPLICIT_KEY, event_node_type(m_arena));
                    mp_current_node = &m_context_stack.back().node;
                    indent = lexer.get_last_token_begin_pos();
                    line = lexer.get_lines_processed();
                    continue;
                }

                if (indent == m_context_stack.back().indent) {
                    // The preceding explicit key, if any, has no value at this point.
                    add_explicit_key_with_null_value();
                }

                const bool needs_to_move_back = indent == 0 || indent < m_context_stack.back().indent;
                if (needs_to_move_back) {
                    pop_to_parent_node(line, indent, [indent](const parse_context& c) {
                        return c.state == context_state_t::BLOCK_MAPPING && indent == c.indent;
                    });
                }

                switch (m_context_stack.back().state) {
                case context_state_t::MAPPING_VALUE:
                case context_state_t::BLOCK_MAPPING_EXPLICIT_KEY:
                case context_state_t::BLOCK_MAPPING_EXPLICIT_VALUE:
                case context_state_t::BLOCK_SEQUENCE_ENTRY:
                    // This path is needed in case the input contains nested explicit keys.
                    // ```yaml
                    // foo:
                    //   ? ? foo
                    //     : bar
                    //   : ? baz
                    //     : - ? qux
                    //         : 123
                    // ```
                    *mp_current_node = event_node_type::block_mapping(m_arena);
                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                    break;
                default:
                    break;
                }

                token = lexer.get_next_token();
                if (token.type == lexical_token_t::SEQUENCE_BLOCK_PREFIX) {
                    check_tab_in_indentation(lexer, lexer.get_lines_processed(), lexer.get_last_token_begin_pos());

                    // The key node is owned by its context until the corresponding KEY_SEPARATOR event.
                    event_node_type key_node = event_node_type::block_sequence(m_arena);
                    m_context_stack.emplace_back(
                        line, indent, context_state_t::BLOCK_MAPPING_EXPLICIT_KEY, std::move(key_node));
                    event_node_type* p_node = &m_context_stack.back().node;

                    const uint32_t seq_line = lexer.get_lines_processed();
                    const uint32_t seq_indent = lexer.get_last_token_begin_pos();
                    m_context_stack.emplace_back(seq_line, seq_indent, context_state_t::BLOCK_SEQUENCE, p_node);

                    event_node_type entry = p_node->add_sequence_entry(event_node_type(m_arena));
                    m_context_stack.emplace_back(seq_line, seq_indent, context_state_t::BLOCK_SEQUENCE_ENTRY, &entry);
                    mp_current_node = &m_context_stack.back().node;

                    break;
                }

                // The key node is owned by its context until the corresponding KEY_SEPARATOR event.
                m_context_stack.emplace_back(
                    line, indent, context_state_t::BLOCK_MAPPING_EXPLICIT_KEY, event_node_type(m_arena));
                mp_current_node = &m_context_stack.back().node;
                indent = lexer.get_last_token_begin_pos();
                line = lexer.get_lines_processed();

                continue;
            }
            case lexical_token_t::KEY_SEPARATOR: {
                if FK_YAML_UNLIKELY (m_context_stack.empty()) {
                    throw parse_error("A key separator is not allowed in this context.", line, indent);
                }
                if (m_context_stack.back().state == context_state_t::FLOW_MAPPING_EXPLICIT_KEY) {
                    add_explicit_flow_key(line, indent);
                    break;
                }
                {
                    // A key separator after node properties on a line of their own begins an entry whose empty key
                    // the properties are for.
                    const parse_context& last_context = m_context_stack.back();
                    const bool begins_entry_with_empty_key =
                        !m_flow_context_state.is_active() &&
                        (m_pending_properties.has_tag() || m_pending_properties.has_anchor()) &&
                        last_context.line < line &&
                        (last_context.state == context_state_t::BLOCK_MAPPING ||
                         last_context.state == context_state_t::MAPPING_VALUE ||
                         last_context.state == context_state_t::BLOCK_SEQUENCE_ENTRY) &&
                        !has_explicit_key_context_at(indent);
                    if (begins_entry_with_empty_key && indent <= last_context.indent) {
                        // ```yaml
                        // foo: 1
                        // &anchor : bar
                        // # -> {foo: 1, &anchor null: bar}
                        // ```
                        pop_to_parent_node(line, indent, [indent](const parse_context& c) {
                            return c.state == context_state_t::BLOCK_MAPPING && indent == c.indent;
                        });
                        add_empty_key_entry(lexer, token, line, indent);
                        continue;
                    }
                    if (begins_entry_with_empty_key && last_context.state == context_state_t::MAPPING_VALUE) {
                        // The mapping value is a mapping whose first key is empty.
                        // ```yaml
                        // foo:
                        //   &anchor : bar
                        // # -> {foo: {&anchor null: bar}}
                        // ```
                        *mp_current_node = event_node_type::block_mapping(m_arena);
                        apply_deferred_properties(*mp_current_node);
                        m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                        add_empty_key_entry(lexer, token, line, indent);
                        continue;
                    }
                }

                if (m_context_stack.back().state == context_state_t::BLOCK_SEQUENCE_ENTRY) {
                    // The entry is a mapping whose first key is empty.
                    // ```yaml
                    // - : foo
                    // # -> [{null: foo}]
                    // ```
                    *mp_current_node = event_node_type::block_mapping(m_arena);
                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                    add_empty_key_entry(lexer, token, line, indent);
                    continue;
                }

                if (m_context_stack.back().state == context_state_t::BLOCK_MAPPING_EXPLICIT_VALUE) {
                    *mp_current_node = event_node_type::block_mapping(m_arena);
                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                    add_empty_key_entry(lexer, token, line, indent);
                    continue;
                }

                {
                    const parse_context& cur_context = m_context_stack.back();
                    const bool is_explicit_key_content =
                        cur_context.state == context_state_t::BLOCK_MAPPING_EXPLICIT_KEY && cur_context.line == line;
                    if (is_explicit_key_content) {
                        // The contents of an explicit key begin with a key separator, so the key is a mapping
                        // whose first entry has an empty key. Whether that entry has a value, and whether the
                        // explicit key itself has one, is not known yet.
                        // ```yaml
                        // ? : foo
                        // #  ^ this key separator begins the contents of the explicit key
                        // ```
                        *mp_current_node = event_node_type::block_mapping(m_arena);
                        m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                        add_empty_key_entry(lexer, token, line, indent);
                        continue;
                    }
                }

                if (m_flow_context_state.is_active()) {
                    if (m_context_stack.back().state != context_state_t::MAPPING_VALUE) {
                        // No key precedes this separator, so the entry has an empty key.
                        // ```yaml
                        // { : foo }
                        // # -> {null: foo}
                        // ```
                        add_empty_key_entry(lexer, token, line, indent);
                        continue;
                    }
                    break;
                }

                // hold the line count of the key separator for later use.
                const uint32_t old_indent = indent;
                const uint32_t old_line = line;

                token = lexer.get_next_token();
                line = lexer.get_lines_processed();
                indent = lexer.get_last_token_begin_pos();

                const bool found_props = deserialize_node_properties(lexer, token, line, indent);
                if (found_props && line == lexer.get_lines_processed() &&
                    token.type != lexical_token_t::KEY_SEPARATOR) {
                    settle_explicit_key_value_with_props(old_line, old_indent);
                    // defer applying node properties for the subsequent node on the same line.
                    continue;
                }
                if (found_props && line > old_line && line == lexer.get_lines_processed()) {
                    // The properties are followed by a key separator on their line (any other token is handled
                    // above), so they are for the empty key of an entry which begins there, and the key separator
                    // is handled at the position of the properties.
                    // ```yaml
                    // foo:
                    // &anchor : bar
                    // ```
                    continue;
                }

                const bool has_explicit_key_context = has_explicit_key_context_at(old_indent);

                if (found_props && line < lexer.get_lines_processed() && !has_explicit_key_context) {
                    // The properties belong to whatever begins on the following line, which the token
                    // after it decides.
                    // ```yaml
                    // foo: &anchor
                    //   bar: baz   # the anchor is for the mapping, not for the "bar" key.
                    // foo: &anchor
                    // : bar        # the anchor is for the empty value of "foo", not for the empty key.
                    // ```
                    defer_node_properties();
                }

                if (found_props && has_explicit_key_context) {
                    // The properties follow the separator of an explicit key, so they belong to its
                    // value. They must not be deferred here, or they would overwrite the properties
                    // of the key which are already deferred.
                    settle_explicit_key_value_with_props(old_line, old_indent);
                    if (line < lexer.get_lines_processed()) {
                        // The value begins on a line after its properties, so which node they belong
                        // to is not known yet; a block collection, whose entries cannot share the
                        // line of the separator, is among the possibilities.
                        // ```yaml
                        // ? foo
                        // : &anchor
                        //   - bar
                        // # -> {foo: &anchor [bar]}
                        // ```
                        defer_node_properties();
                        line = lexer.get_lines_processed();
                        indent = lexer.get_last_token_begin_pos();
                    }
                    continue;
                }

                line = lexer.get_lines_processed();
                indent = lexer.get_last_token_begin_pos();

                const bool is_implicit_same_line =
                    (line == old_line) && (m_context_stack.empty() || old_indent > m_context_stack.back().indent);
                if (is_implicit_same_line) {
                    // a key separator for an implicit key with its value on the same line.
                    if FK_YAML_UNLIKELY (token.type == lexical_token_t::SEQUENCE_BLOCK_PREFIX) {
                        // ```yaml
                        // foo: - bar   # error: a block sequence cannot begin on the line of its mapping key.
                        // ```
                        throw parse_error("A block sequence cannot begin on the line of a mapping key.", line, indent);
                    }
                    continue;
                }

                if (line > old_line) {
                    const bool is_explicit_value_begin =
                        m_context_stack.back().state == context_state_t::BLOCK_MAPPING_EXPLICIT_KEY &&
                        (token.type == lexical_token_t::SEQUENCE_BLOCK_PREFIX ||
                         indent > m_context_stack.back().indent);
                    if (is_explicit_value_begin) {
                        // The value of an explicit key can begin on a line after its key separator.
                        // ```yaml
                        // ? foo
                        // :
                        //   bar
                        // # -> {foo: bar}
                        // ```
                        add_explicit_key_with_empty_value(old_line, old_indent);
                    }

                    if (token.type == lexical_token_t::SEQUENCE_BLOCK_PREFIX) {
                        if (m_context_stack.back().state == context_state_t::MAPPING_VALUE && defers_props() &&
                            indent < m_context_stack.back().indent) {
                            pop_to_parent_node(line, indent, [indent](const parse_context& c) {
                                return c.state == context_state_t::BLOCK_SEQUENCE && indent == c.indent;
                            });
                            continue;
                        }

                        // a key separator preceding block sequence entries
                        initialize_block_sequence_value(line, indent, true);

                        token = lexer.get_next_token();
                        line = lexer.get_lines_processed();
                        indent = lexer.get_last_token_begin_pos();

                        const bool has_props = deserialize_node_properties(lexer, token, line, indent);
                        if (has_props) {
                            const uint32_t line_after_props = lexer.get_lines_processed();
                            if (line == line_after_props) {
                                // Skip updating the current indent to avoid stacking a wrong indentation.
                                //
                                // ```yaml
                                // &foo bar: baz
                                // ^
                                // the correct indent width for the "bar" node key.
                                // ```
                                continue;
                            }

                            // if node properties and the followed node are on different lines (i.e., the properties are
                            // for a container node), the application and the line advancement must happen here.
                            // Otherwise, a false indent error will be emitted. See
                            // https://github.com/fktn-k/fkYAML/issues/368 for more details.
                            line = line_after_props;
                            indent = lexer.get_last_token_begin_pos();
                            *mp_current_node = event_node_type::block_mapping(m_arena);
                            m_context_stack.emplace_back(
                                line_after_props, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                            apply_deferred_properties(*mp_current_node);
                            apply_node_properties(*mp_current_node);
                        }

                        continue;
                    }

                    // A key separator on a following line may be the value separator of an explicit
                    // key whose contents were mapping entries, rather than a separator in the current
                    // mapping:
                    // ```yaml
                    // ? foo: !!str # the tag belongs to this omitted value
                    // : foo: !!str # this separator begins the explicit key's value
                    // ```
                    // Defer the properties of the omitted value and leave the separator to the next
                    // iteration, which closes the explicit key and applies them.
                    if (token.type == lexical_token_t::KEY_SEPARATOR) {
                        const auto is_explicit_key_at_indent = [indent](const parse_context& c) {
                            return c.state == context_state_t::BLOCK_MAPPING_EXPLICIT_KEY && indent == c.indent;
                        };
                        const bool closes_explicit_key =
                            indent < m_context_stack.back().indent &&
                            std::any_of(m_context_stack.rbegin(), m_context_stack.rend(), is_explicit_key_at_indent);
                        if (closes_explicit_key) {
                            defer_node_properties();
                            continue;
                        }

                        const bool begins_entry_with_empty_key =
                            m_context_stack.back().state == context_state_t::MAPPING_VALUE &&
                            !has_explicit_key_context_at(indent);
                        if (begins_entry_with_empty_key) {
                            // Nothing but properties, if any, is between this key separator and the one of the
                            // preceding key, so the preceding value is omitted and this separator begins an entry
                            // with an empty key. The deferred properties are for the omitted value.
                            if (indent <= m_context_stack.back().indent) {
                                // ```yaml
                                // foo: &anchor
                                // : bar
                                // # -> {foo: &anchor null, null: bar}
                                // ```
                                pop_to_parent_node(line, indent, [indent](const parse_context& c) {
                                    return c.state == context_state_t::BLOCK_MAPPING && indent == c.indent;
                                });
                            }
                            else {
                                // ```yaml
                                // foo: &anchor
                                //   : bar
                                // # -> {foo: &anchor {null: bar}}
                                // ```
                                *mp_current_node = event_node_type::block_mapping(m_arena);
                                apply_deferred_properties(*mp_current_node);
                                m_context_stack.emplace_back(
                                    line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                            }
                            add_empty_key_entry(lexer, token, line, indent);
                            continue;
                        }
                    }

                    const bool is_omitted_mapping_value_without_properties =
                        (token.type != lexical_token_t::KEY_SEPARATOR || !defers_props()) &&
                        indent <= m_context_stack.back().indent;
                    if (is_omitted_mapping_value_without_properties) {
                        // An explicit key can omit its value as well, in which case the entry must still be
                        // added to the parent mapping.
                        // ```yaml
                        // ? foo
                        // :
                        // bar: baz
                        // # -> {foo: null, bar: baz}
                        // ```
                        const bool is_entry_added = add_explicit_key_with_null_value();
                        if (!is_entry_added) {
                            if FK_YAML_UNLIKELY (m_context_stack.back().state != context_state_t::MAPPING_VALUE) {
                                // The key separator does not follow a mapping key, for example:
                                // ```yaml
                                // ? foo
                                // :
                                // :
                                // ```
                                throw parse_error("A key separator is not allowed in this context.", line, indent);
                            }

                            // Mapping values can be omitted and are considered to be null.
                            // ```yaml
                            // foo:
                            // bar:
                            //   baz:
                            // qux:
                            // # -> {foo: null, bar: {baz: null}, qux: null}
                            // ```
                            pop_to_parent_node(line, indent, [indent](const parse_context& c) {
                                return (c.state == context_state_t::BLOCK_MAPPING) && (indent == c.indent);
                            });
                        }
                    }

                    // defer checking the existence of a key separator after the following scalar until the next
                    // deserialize_scalar() call.
                    continue;
                }

                // handle explicit mapping key separators.
                if (m_context_stack.back().state != context_state_t::BLOCK_MAPPING_EXPLICIT_KEY) {
                    // The contents of the explicit key may have left their own contexts on the stack.
                    // ```yaml
                    // ? :
                    // : v
                    // # -> {{null: null}: v}
                    // ```
                    // old_indent is the position of this key separator, while indent already refers to the
                    // token which follows it.
                    const auto is_key_context = [old_indent](const parse_context& c) {
                        return c.state == context_state_t::BLOCK_MAPPING_EXPLICIT_KEY && old_indent == c.indent;
                    };
                    const bool has_key_context =
                        std::any_of(m_context_stack.rbegin(), m_context_stack.rend(), is_key_context);
                    if FK_YAML_UNLIKELY (!has_key_context) {
                        throw parse_error("Unexpected explicit mapping key separator is found.", line, indent);
                    }

                    pop_to_parent_node(old_line, old_indent, is_key_context);
                }

                add_explicit_key_with_empty_value(old_line, old_indent);

                if (token.type == lexical_token_t::SEQUENCE_BLOCK_PREFIX) {
                    initialize_block_sequence_value(line, indent, false);
                    break;
                }

                continue;
            }
            case lexical_token_t::ANCHOR_PREFIX:
            case lexical_token_t::TAG_PREFIX: {
                const uint32_t props_line = lexer.get_lines_processed();
                deserialize_node_properties(lexer, token, line, indent);

                if (!m_flow_context_state.is_active() && lexer.get_lines_processed() > props_line) {
                    // The properties belong to whatever begins on the following line. Which node that
                    // is depends on the token after it, so the binding waits until that is known.
                    // In the flow context, line breaks do not change the node which properties belong to.
                    // ```yaml
                    // - !circle
                    //   center: 1   # the tag is for the mapping, not for the "center" key.
                    // ```
                    defer_node_properties();
                    line = lexer.get_lines_processed();
                    indent = lexer.get_last_token_begin_pos();
                    continue;
                }

                // Skip updating the current indent to avoid stacking a wrong indentation.
                // Note that node properties for block sequences as a mapping value are processed when a
                // `lexical_token_t::KEY_SEPARATOR` token is processed.
                //
                // ```yaml
                // &foo bar: baz
                // ^
                // the correct indent width for the "bar" node key.
                // ```
                continue;
            }
            case lexical_token_t::SEQUENCE_BLOCK_PREFIX: {
                check_tab_in_indentation(lexer, lexer.get_lines_processed(), lexer.get_last_token_begin_pos());
                if FK_YAML_UNLIKELY (m_flow_context_state.is_active()) {
                    throw parse_error("A block sequence entry is not allowed in the flow context.", line, indent);
                }

                if FK_YAML_UNLIKELY (m_context_stack.empty()) {
                    throw parse_error("invalid block sequence entry is found.", line, indent);
                }

                // A block sequence cannot begin on the line of its node properties.
                // ```yaml
                // foo: &anchor - bar   # error
                // ```
                if FK_YAML_UNLIKELY (m_pending_properties.has_anchor() || m_pending_properties.has_tag()) {
                    throw parse_error(
                        "Node properties cannot precede a block sequence entry on the same line.", line, indent);
                }

                const uint32_t parent_indent = m_context_stack.back().indent;
                if (indent == parent_indent) {
                    // If the previous block sequence entry is empty, just move to the parent context.
                    // ```yaml
                    // foo:
                    //   -
                    //   - bar
                    // # ^ (here)
                    // # -> {foo: [null, bar]}
                    // ```
                    pop_to_parent_node(line, indent, [](const parse_context& c) {
                        return c.state == context_state_t::BLOCK_SEQUENCE;
                    });
                }
                else if (indent < parent_indent) {
                    pop_to_parent_node(line, indent, [indent](const parse_context& c) {
                        return c.state == context_state_t::BLOCK_SEQUENCE && indent == c.indent;
                    });
                }
                else /*parent_indent < indent*/ {
                    if FK_YAML_UNLIKELY (m_context_stack.back().state == context_state_t::BLOCK_SEQUENCE) {
                        // bad indentation like the following YAML:
                        // ```yaml
                        // - "foo"
                        //   - bar
                        // # ^
                        // ```
                        throw parse_error("bad indentation of a mapping entry.", line, indent);
                    }

                    *mp_current_node = event_node_type::block_sequence(m_arena);
                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_SEQUENCE, mp_current_node);
                    apply_deferred_properties(*mp_current_node);
                    apply_node_properties(*mp_current_node);
                    mp_current_node->start_streaming();
                }

                event_node_type entry = mp_current_node->add_sequence_entry(event_node_type(m_arena));
                m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_SEQUENCE_ENTRY, &entry);
                mp_current_node = &m_context_stack.back().node;
                break;
            }
            case lexical_token_t::SEQUENCE_FLOW_BEGIN:
                if (!m_flow_context_state.is_active()) {
                    lexer.enter_flow_context();

                    if FK_YAML_UNLIKELY (m_context_stack.empty()) {
                        if (!defers_props()) {
                            throw parse_error("invalid flow sequence beginning is found.", line, indent);
                        }
                        *mp_current_node = event_node_type::block_mapping(m_arena);
                        apply_deferred_properties(*mp_current_node);
                        m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                        m_document_state.expects_root_flow_key_separator(true);
                    }

                    if (indent <= m_context_stack.back().indent) {
                        pop_to_parent_node(line, indent, [indent](const parse_context& c) {
                            switch (c.state) {
                            case context_state_t::BLOCK_MAPPING:
                            case context_state_t::MAPPING_VALUE:
                                return indent == c.indent;
                            default:
                                return false;
                            }
                        });
                    }
                }
                else if FK_YAML_UNLIKELY (m_flow_context_state.needs_separator_or_suffix()) {
                    throw parse_error("Flow sequence beginning is found without separated with a comma.", line, indent);
                }

                m_flow_context_state.begin(current_context(line, indent).indent);

                switch (current_context(line, indent).state) {
                case context_state_t::BLOCK_SEQUENCE:
                case context_state_t::FLOW_SEQUENCE: {
                    event_node_type sequence =
                        mp_current_node->add_sequence_entry(event_node_type::flow_sequence(m_arena));
                    m_context_stack.emplace_back(line, indent, context_state_t::FLOW_SEQUENCE, &sequence);
                    mp_current_node = &m_context_stack.back().node;
                    break;
                }
                case context_state_t::BLOCK_MAPPING:
                case context_state_t::FLOW_MAPPING:
                    // The key node is owned by its context until the corresponding SEQUENCE_FLOW_END event.
                    m_context_stack.emplace_back(
                        line, indent, context_state_t::FLOW_SEQUENCE_KEY, event_node_type::flow_sequence(m_arena));
                    mp_current_node = &m_context_stack.back().node;
                    break;
                default: {
                    *mp_current_node = event_node_type::flow_sequence(m_arena);
                    parse_context& last_context = m_context_stack.back();
                    last_context.line = line;
                    last_context.indent = indent;
                    last_context.state = context_state_t::FLOW_SEQUENCE;
                    // The collection may turn out to be an implicit key, which is only known at its end.
                    last_context.held_props = take_deferred_properties();
                    break;
                }
                }

                apply_deferred_properties(*mp_current_node);
                apply_node_properties(*mp_current_node);

                m_flow_context_state.set_token_state(flow_token_state_t::NEEDS_VALUE_OR_SUFFIX);
                break;
            case lexical_token_t::SEQUENCE_FLOW_END: {
                if FK_YAML_UNLIKELY (!m_flow_context_state.is_active()) {
                    throw parse_error("Flow sequence ending is found outside the flow context.", line, indent);
                }

                const bool is_flow_explicit_key =
                    !m_context_stack.empty() &&
                    m_context_stack.back().state == context_state_t::FLOW_MAPPING_EXPLICIT_KEY;
                if (is_flow_explicit_key) {
                    add_explicit_flow_key(line, indent);
                }

                if (m_flow_context_state.end()) {
                    lexer.exit_flow_context();
                }

                close_empty_flow_sequence_entry(line, indent);
                close_omitted_mapping_value(line, indent);
                close_single_pair_mapping(line, indent);

                const bool has_valid_beginning =
                    !m_context_stack.empty() && (m_context_stack.back().state == context_state_t::FLOW_SEQUENCE ||
                                                 m_context_stack.back().state == context_state_t::FLOW_SEQUENCE_KEY);
                if FK_YAML_UNLIKELY (!has_valid_beginning) {
                    throw parse_error("No corresponding flow sequence beginning is found.", line, indent);
                }

                // keep the last state for later processing.
                parse_context& last_context = m_context_stack.back();
                event_node_type collection_node = last_context.node.reference();
                mp_current_node = &collection_node;
                const uint32_t collection_begin_line = last_context.line;
                const uint32_t collection_begin_indent = last_context.indent;
                indent = collection_begin_indent;
                const bool is_multiline_collection = collection_begin_line != lexer.get_lines_processed();
                const bool is_explicit_key = last_context.is_explicit_key;
                const pending_node_properties held_props = last_context.held_props;
                event_node_type owned_node = collection_node.reference();
                const bool owns_detached_node = last_context.owns_detached_node;
                m_context_stack.pop_back();

                // handle cases where the flow sequence is a mapping key node. A context owns its node only
                // while that node is a key which has not been added to its parent mapping yet.

                if (!m_context_stack.empty() && owns_detached_node) {
                    if (is_explicit_key) {
                        restore_explicit_flow_collection_key(
                            lexer,
                            std::move(owned_node),
                            held_props,
                            collection_begin_line,
                            collection_begin_indent,
                            is_multiline_collection,
                            token,
                            line,
                            indent);
                        continue;
                    }
                    if FK_YAML_UNLIKELY (is_multiline_collection) {
                        throw parse_error(
                            "An implicit mapping key cannot span multiple lines.",
                            lexer.get_lines_processed(),
                            lexer.get_last_token_begin_pos());
                    }
                    if (m_document_state.expects_root_flow_key_separator()) {
                        const lexical_token_t next_type = lexer.peek_next_token().type;
                        if FK_YAML_UNLIKELY (next_type != lexical_token_t::KEY_SEPARATOR) {
                            throw parse_error(
                                "A flow collection key must be followed by a key separator.", line, indent);
                        }
                        m_document_state.expects_root_flow_key_separator(false);
                    }
                    event_node_type key_node = std::move(owned_node);
                    mp_current_node = &m_context_stack.back().node;
                    m_flow_context_state.set_token_state(flow_token_state_t::NEEDS_VALUE_OR_SUFFIX);

                    add_new_key(std::move(key_node), line, indent);
                    break;
                }

                token = lexer.get_next_token();
                if (token.type == lexical_token_t::KEY_SEPARATOR) {
                    if FK_YAML_UNLIKELY (is_multiline_collection) {
                        throw parse_error(
                            "An implicit mapping key cannot span multiple lines.",
                            lexer.get_lines_processed(),
                            lexer.get_last_token_begin_pos());
                    }
                    event_node_type key_node = event_node_type::block_mapping(m_arena);
                    mp_current_node->swap(key_node);
                    // The properties which precede the collection are for the mapping which it begins as a key.
                    apply_held_properties(held_props, *mp_current_node);

                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                    m_flow_context_state.set_token_state(flow_token_state_t::NEEDS_VALUE_OR_SUFFIX);

                    add_new_key(std::move(key_node), line, indent);
                }
                else {
                    if FK_YAML_UNLIKELY (m_context_stack.empty() && !is_document_end_token(token.type)) {
                        throw parse_error(
                            "Multiple root nodes are not allowed in the same document.",
                            lexer.get_lines_processed(),
                            lexer.get_last_token_begin_pos());
                    }
                    apply_held_properties(held_props, *mp_current_node);
                    mp_current_node->release();
                    if (!m_context_stack.empty()) {
                        mp_current_node = &m_context_stack.back().node;
                    }
                    if (m_flow_context_state.is_active()) {
                        m_flow_context_state.set_token_state(flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX);
                    }
                }

                indent = lexer.get_last_token_begin_pos();
                line = lexer.get_lines_processed();
                continue;
            }
            case lexical_token_t::MAPPING_FLOW_BEGIN:
                if (!m_flow_context_state.is_active()) {
                    lexer.enter_flow_context();

                    if FK_YAML_UNLIKELY (m_context_stack.empty()) {
                        if (!defers_props()) {
                            throw parse_error("invalid flow mapping beginning is found.", line, indent);
                        }
                        *mp_current_node = event_node_type::block_mapping(m_arena);
                        apply_deferred_properties(*mp_current_node);
                        m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                        m_document_state.expects_root_flow_key_separator(true);
                    }

                    if (indent <= m_context_stack.back().indent) {
                        pop_to_parent_node(line, indent, [indent](const parse_context& c) {
                            switch (c.state) {
                            case context_state_t::BLOCK_MAPPING:
                            case context_state_t::MAPPING_VALUE:
                                return indent == c.indent;
                            default:
                                return false;
                            }
                        });
                    }
                }
                else if FK_YAML_UNLIKELY (m_flow_context_state.needs_separator_or_suffix()) {
                    throw parse_error("Flow mapping beginning is found without separated with a comma.", line, indent);
                }

                m_flow_context_state.begin(current_context(line, indent).indent);

                switch (current_context(line, indent).state) {
                case context_state_t::BLOCK_SEQUENCE:
                case context_state_t::FLOW_SEQUENCE: {
                    event_node_type mapping =
                        mp_current_node->add_sequence_entry(event_node_type::flow_mapping(m_arena));
                    m_context_stack.emplace_back(line, indent, context_state_t::FLOW_MAPPING, &mapping);
                    mp_current_node = &m_context_stack.back().node;
                    break;
                }
                case context_state_t::BLOCK_MAPPING:
                case context_state_t::FLOW_MAPPING:
                    // The key node is owned by its context until the corresponding MAPPING_FLOW_END event.
                    m_context_stack.emplace_back(
                        line, indent, context_state_t::FLOW_MAPPING_KEY, event_node_type::flow_mapping(m_arena));
                    mp_current_node = &m_context_stack.back().node;
                    break;
                default: {
                    *mp_current_node = event_node_type::flow_mapping(m_arena);
                    parse_context& last_context = m_context_stack.back();
                    last_context.line = line;
                    last_context.indent = indent;
                    last_context.state = context_state_t::FLOW_MAPPING;
                    // The collection may turn out to be an implicit key, which is only known at its end.
                    last_context.held_props = take_deferred_properties();
                    break;
                }
                }

                apply_deferred_properties(*mp_current_node);
                apply_node_properties(*mp_current_node);

                line = lexer.get_lines_processed();
                indent = lexer.get_last_token_begin_pos();

                m_flow_context_state.set_token_state(flow_token_state_t::NEEDS_VALUE_OR_SUFFIX);
                break;
            case lexical_token_t::MAPPING_FLOW_END: {
                if FK_YAML_UNLIKELY (!m_flow_context_state.is_active()) {
                    throw parse_error("Flow mapping ending is found outside the flow context.", line, indent);
                }

                const bool is_flow_explicit_key =
                    !m_context_stack.empty() &&
                    m_context_stack.back().state == context_state_t::FLOW_MAPPING_EXPLICIT_KEY;
                if (is_flow_explicit_key) {
                    add_explicit_flow_key(line, indent);
                }

                if (m_flow_context_state.end()) {
                    lexer.exit_flow_context();
                }

                close_omitted_mapping_value(line, indent);

                const bool has_valid_beginning =
                    !m_context_stack.empty() && (m_context_stack.back().state == context_state_t::FLOW_MAPPING ||
                                                 m_context_stack.back().state == context_state_t::FLOW_MAPPING_KEY);
                if FK_YAML_UNLIKELY (!has_valid_beginning) {
                    throw parse_error("No corresponding flow mapping beginning is found.", line, indent);
                }

                // keep the last state for later processing.
                parse_context& last_context = m_context_stack.back();
                event_node_type collection_node = last_context.node.reference();
                mp_current_node = &collection_node;
                const uint32_t collection_begin_line = last_context.line;
                const uint32_t collection_begin_indent = last_context.indent;
                indent = collection_begin_indent;
                const bool is_multiline_collection = collection_begin_line != lexer.get_lines_processed();
                const bool is_explicit_key = last_context.is_explicit_key;
                const pending_node_properties held_props = last_context.held_props;
                event_node_type owned_node = collection_node.reference();
                const bool owns_detached_node = last_context.owns_detached_node;
                m_context_stack.pop_back();

                // handle cases where the flow mapping is a mapping key node. A context owns its node only
                // while that node is a key which has not been added to its parent mapping yet.

                if (!m_context_stack.empty() && owns_detached_node) {
                    if (is_explicit_key) {
                        restore_explicit_flow_collection_key(
                            lexer,
                            std::move(owned_node),
                            held_props,
                            collection_begin_line,
                            collection_begin_indent,
                            is_multiline_collection,
                            token,
                            line,
                            indent);
                        continue;
                    }
                    if FK_YAML_UNLIKELY (is_multiline_collection) {
                        throw parse_error(
                            "An implicit mapping key cannot span multiple lines.",
                            lexer.get_lines_processed(),
                            lexer.get_last_token_begin_pos());
                    }
                    if (m_document_state.expects_root_flow_key_separator()) {
                        const lexical_token_t next_type = lexer.peek_next_token().type;
                        if FK_YAML_UNLIKELY (next_type != lexical_token_t::KEY_SEPARATOR) {
                            throw parse_error(
                                "A flow collection key must be followed by a key separator.", line, indent);
                        }
                        m_document_state.expects_root_flow_key_separator(false);
                    }
                    event_node_type key_node = std::move(owned_node);
                    mp_current_node = &m_context_stack.back().node;
                    m_flow_context_state.set_token_state(flow_token_state_t::NEEDS_VALUE_OR_SUFFIX);

                    add_new_key(std::move(key_node), line, indent);
                    break;
                }

                token = lexer.get_next_token();
                if (token.type == lexical_token_t::KEY_SEPARATOR) {
                    if FK_YAML_UNLIKELY (is_multiline_collection) {
                        throw parse_error(
                            "An implicit mapping key cannot span multiple lines.",
                            lexer.get_lines_processed(),
                            lexer.get_last_token_begin_pos());
                    }
                    event_node_type key_node = event_node_type::block_mapping(m_arena);
                    mp_current_node->swap(key_node);
                    // The properties which precede the collection are for the mapping which it begins as a key.
                    apply_held_properties(held_props, *mp_current_node);

                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                    m_flow_context_state.set_token_state(flow_token_state_t::NEEDS_VALUE_OR_SUFFIX);

                    add_new_key(std::move(key_node), line, indent);
                }
                else {
                    if FK_YAML_UNLIKELY (m_context_stack.empty() && !is_document_end_token(token.type)) {
                        throw parse_error(
                            "Multiple root nodes are not allowed in the same document.",
                            lexer.get_lines_processed(),
                            lexer.get_last_token_begin_pos());
                    }
                    apply_held_properties(held_props, *mp_current_node);
                    mp_current_node->release();
                    if (!m_context_stack.empty()) {
                        mp_current_node = &m_context_stack.back().node;
                    }
                    if (m_flow_context_state.is_active()) {
                        m_flow_context_state.set_token_state(flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX);
                    }
                }

                indent = lexer.get_last_token_begin_pos();
                line = lexer.get_lines_processed();
                continue;
            }
            case lexical_token_t::VALUE_SEPARATOR:
                if FK_YAML_UNLIKELY (!m_flow_context_state.is_active()) {
                    throw parse_error("invalid value separator is found.", line, indent);
                }
                if (!m_context_stack.empty() &&
                    m_context_stack.back().state == context_state_t::FLOW_MAPPING_EXPLICIT_KEY) {
                    add_explicit_flow_key(line, indent);
                }
                close_empty_flow_sequence_entry(line, indent);
                close_omitted_mapping_value(line, indent);
                if FK_YAML_UNLIKELY (!m_flow_context_state.needs_separator_or_suffix()) {
                    throw parse_error("invalid value separator is found.", line, indent);
                }
                close_single_pair_mapping(line, indent);
                m_flow_context_state.set_token_state(flow_token_state_t::NEEDS_VALUE_OR_SUFFIX);
                break;
            case lexical_token_t::ALIAS_PREFIX: {
                // An alias node must not specify any properties (tag, anchor), but deferred ones are
                // for the collection which this alias begins rather than for the alias itself.
                // https://yaml.org/spec/1.2.2/#71-alias-nodes
                if FK_YAML_UNLIKELY (m_pending_properties.has_tag()) {
                    throw parse_error("Tag cannot be specified to an alias node", line, indent);
                }
                if FK_YAML_UNLIKELY (m_pending_properties.has_anchor()) {
                    throw parse_error("Anchor cannot be specified to an alias node.", line, indent);
                }

                event_node_type node = event_node_type::alias(m_arena, token.str);

                deserialize_scalar(lexer, std::move(node), indent, line, token);
                continue;
            }
            case lexical_token_t::PLAIN_SCALAR:
            case lexical_token_t::SINGLE_QUOTED_SCALAR:
            case lexical_token_t::DOUBLE_QUOTED_SCALAR: {
                event_node_type node = event_node_type::scalar(m_arena, token.type, token.str);

                deserialize_scalar(lexer, std::move(node), indent, line, token);
                continue;
            }
            case lexical_token_t::BLOCK_LITERAL_SCALAR:
            case lexical_token_t::BLOCK_FOLDED_SCALAR: {
                event_node_type node =
                    event_node_type::scalar(m_arena, token.type, token.str, lexer.get_block_scalar_header());

                deserialize_scalar(lexer, std::move(node), indent, line, token);
                continue;
            }
            // these tokens end parsing the current YAML document.
            case lexical_token_t::END_OF_BUFFER:
                // This handles an empty input.
                last_type = token.type;
                return;
            case lexical_token_t::END_OF_DIRECTIVES:
            case lexical_token_t::END_OF_DOCUMENT:
                if FK_YAML_UNLIKELY (m_flow_context_state.is_active()) {
                    throw parse_error("An invalid document marker found in a flow collection", line, indent);
                }
                if (token.type == lexical_token_t::END_OF_DIRECTIVES) {
                    m_document_state.set_explicit_document_start_line(line);
                }
                last_type = token.type;
                return;
            // no way to come here while lexically analyzing document contents.
            case lexical_token_t::YAML_VER_DIRECTIVE: // LCOV_EXCL_LINE
            case lexical_token_t::TAG_DIRECTIVE:      // LCOV_EXCL_LINE
            case lexical_token_t::INVALID_DIRECTIVE:  // LCOV_EXCL_LINE
                detail::unreachable();                // LCOV_EXCL_LINE
            }

            token = lexer.get_next_token();
            indent = lexer.get_last_token_begin_pos();
            line = lexer.get_lines_processed();
        } while (token.type != lexical_token_t::END_OF_BUFFER);

        if FK_YAML_UNLIKELY (m_flow_context_state.is_active()) {
            throw parse_error("An unclosed flow collection found at the end of input", line, indent);
        }

        last_type = token.type;
    }

    /// @brief Deserializes YAML node properties (anchor and/or tag names) if they exist
    /// @param lexer The lexical analyzer to be used.
    /// @param last_type The variable to store the last lexical token type.
    /// @param line The variable to store the line of either the first property or the last non-property token.
    /// @param indent The variable to store the indent of either the first property or the last non-property token.
    /// @return true if any property is found, false otherwise.
    bool deserialize_node_properties(lexer_type& lexer, lexical_token& last_token, uint32_t& line, uint32_t& indent) {
        m_pending_properties = {};

        lexical_token token = last_token;
        bool ends_loop {false};
        do {
            if (line < lexer.get_lines_processed()) {
                break;
            }

            switch (token.type) {
            case lexical_token_t::ANCHOR_PREFIX:
                if FK_YAML_UNLIKELY (m_pending_properties.has_anchor()) {
                    throw parse_error(
                        "anchor name cannot be specified more than once to the same node.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }

                m_pending_properties.store_anchor(token.str);

                if (!m_pending_properties.has_tag()) {
                    line = lexer.get_lines_processed();
                    indent = lexer.get_last_token_begin_pos();
                }

                token = lexer.get_next_token();
                break;
            case lexical_token_t::TAG_PREFIX: {
                if FK_YAML_UNLIKELY (m_pending_properties.has_tag()) {
                    throw parse_error(
                        "tag name cannot be specified more than once to the same node.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }

                m_pending_properties.store_tag(token.str);

                if (!m_pending_properties.has_anchor()) {
                    line = lexer.get_lines_processed();
                    indent = lexer.get_last_token_begin_pos();
                }

                token = lexer.get_next_token();
                break;
            }
            default:
                ends_loop = true;
                break;
            }
        } while (!ends_loop);

        last_token = token;
        const bool prop_specified = m_pending_properties.has_anchor() || m_pending_properties.has_tag();
        if (!prop_specified) {
            line = lexer.get_lines_processed();
            indent = lexer.get_last_token_begin_pos();
        }
        else if FK_YAML_UNLIKELY (
            line < lexer.get_lines_processed() && !m_flow_context_state.is_active() && !m_context_stack.empty() &&
            indent <= m_context_stack.back().indent) {
            // Node properties which end their line belong to a node nested in the current block collection, so
            // they must be more indented than it.
            // ```yaml
            // - item1
            // &node   # error: not more indented than the sequence.
            // - item2
            // ```
            throw parse_error("Node properties are not indented enough.", line, indent);
        }

        return prop_specified;
    }

    /// @brief Reject a tab character used as the indentation of a block collection.
    /// @note Indentation must consist of spaces only, while a tab which follows it is valid separation
    /// white space. See https://yaml.org/spec/1.2.2/#61-indentation-spaces for more details.
    /// ```yaml
    /// -\t-    # the nested sequence is indented with a tab, which is an error
    /// -\t-1   # the tab only separates the entry from its scalar, which is valid
    /// ```
    /// @param lexer The lexical analyzer to be used.
    /// @param line The line of the node which begins the block collection.
    /// @param indent The indentation width of the node which begins the block collection.
    void check_tab_in_indentation(lexer_type& lexer, const uint32_t line, const uint32_t indent) const {
        if FK_YAML_UNLIKELY (!m_flow_context_state.is_active() && lexer.has_tab_in_indentation(indent)) {
            throw parse_error("A tab character cannot be used as indentation.", line, indent);
        }
    }

    /// @brief Add new key string to the current YAML node.
    /// @param key a key string to be added to the current YAML node.
    /// @param line The line where the key is found.
    /// @param indent The indentation width in the current line where the key is found.
    void add_new_key(event_node_type&& key, const uint32_t line, const uint32_t indent) {
        if (!m_flow_context_state.is_active()) {
            if FK_YAML_UNLIKELY (m_context_stack.empty()) {
                throw parse_error("A mapping key is not allowed in this context.", line, indent);
            }
            if FK_YAML_UNLIKELY (m_context_stack.back().indent < indent) {
                // bad indentation like the following YAML:
                // ```yaml
                // foo: true
                //   baz: 123
                // # ^
                // ```
                throw parse_error("bad indentation of a mapping entry.", line, indent);
            }

            pop_to_parent_node(line, indent, [indent](const parse_context& c) {
                return (c.state == context_state_t::BLOCK_MAPPING) && (indent == c.indent);
            });
        }
        else {
            if (mp_current_node->is_sequence()) {
                event_node_type mapping = mp_current_node->add_sequence_entry(event_node_type::block_mapping(m_arena));
                m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, &mapping);
                mp_current_node = &m_context_stack.back().node;
            }
        }

        mp_current_node->start_streaming();
        event_node_type value = mp_current_node->add_mapping_entry(std::move(key));
        const parse_context& key_context = current_context(line, indent);
        m_context_stack.emplace_back(key_context.line, key_context.indent, context_state_t::MAPPING_VALUE, &value);
        mp_current_node = &m_context_stack.back().node;
    }

    void restore_explicit_flow_collection_key(
        lexer_type& lexer, event_node_type&& owned_node, const pending_node_properties& held_props,
        const uint32_t collection_begin_line, const uint32_t collection_begin_indent,
        const bool is_multiline_collection, lexical_token& token, uint32_t& line, uint32_t& indent) {
        const context_state_t explicit_key_state = m_flow_context_state.is_active()
                                                       ? context_state_t::FLOW_MAPPING_EXPLICIT_KEY
                                                       : context_state_t::BLOCK_MAPPING_EXPLICIT_KEY;
        m_context_stack.emplace_back(
            collection_begin_line, collection_begin_indent, explicit_key_state, std::move(owned_node));
        mp_current_node = &m_context_stack.back().node;

        const uint32_t collection_end_line = lexer.get_lines_processed();
        token = lexer.get_next_token();
        line = lexer.get_lines_processed();
        indent = lexer.get_last_token_begin_pos();

        const bool begins_compact_mapping = !m_flow_context_state.is_active() &&
                                            token.type == lexical_token_t::KEY_SEPARATOR && line == collection_end_line;
        if (!begins_compact_mapping) {
            // The collection is the explicit key itself.
            // ```yaml
            // ? &anchor
            //   [foo]
            // : bar   # the anchor is for the [foo] key.
            // ```
            apply_held_properties(held_props, *mp_current_node);
            return;
        }
        if FK_YAML_UNLIKELY (is_multiline_collection) {
            throw parse_error(
                "An implicit mapping key cannot span multiple lines.",
                lexer.get_lines_processed(),
                lexer.get_last_token_begin_pos());
        }

        // The collection is the first key of a mapping as the explicit key.
        // ```yaml
        // ? &anchor
        //   [foo]: bar   # the anchor is for the {[foo]: bar} mapping.
        // : baz
        // ```
        event_node_type collection_key = event_node_type::block_mapping(m_arena);
        mp_current_node->swap(collection_key);
        apply_held_properties(held_props, *mp_current_node);
        event_node_type value = mp_current_node->add_mapping_entry(std::move(collection_key));
        m_context_stack.emplace_back(line, indent, context_state_t::MAPPING_VALUE, &value);
        mp_current_node = &m_context_stack.back().node;

        token = lexer.get_next_token();
        line = lexer.get_lines_processed();
        indent = lexer.get_last_token_begin_pos();
    }

    /// @brief Assign node value to the current node.
    /// @param node_value A rvalue event_node_type object to be assigned to the current node.
    void assign_node_value(event_node_type&& node_value, const uint32_t line, const uint32_t indent) {
        if (mp_current_node->is_sequence()) {
            if FK_YAML_UNLIKELY (!m_flow_context_state.is_active()) {
                throw parse_error("invalid block sequence entry is found.", line, indent);
            }

            if FK_YAML_UNLIKELY (!m_flow_context_state.needs_value_or_suffix()) {
                // Flow sequence entries are not allowed to be empty.
                // ```yaml
                // [foo,,bar]
                // ```
                throw parse_error("flow sequence entry is found without separated with a comma.", line, indent);
            }

            mp_current_node->add_sequence_entry(std::move(node_value));
            m_flow_context_state.set_token_state(flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX);
            return;
        }

        // a scalar node
        *mp_current_node = std::move(node_value);
        if FK_YAML_UNLIKELY (m_context_stack.empty()) {
            // single scalar document.
            return;
        }

        if FK_YAML_LIKELY (
            m_context_stack.back().state != context_state_t::BLOCK_MAPPING_EXPLICIT_KEY &&
            m_context_stack.back().state != context_state_t::FLOW_MAPPING_EXPLICIT_KEY) {
            m_context_stack.pop_back();
            mp_current_node = &current_context(line, indent).node;

            if (m_flow_context_state.is_active()) {
                m_flow_context_state.set_token_state(flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX);
            }
        }
    }

    /// @brief Deserialize a detected scalar node.
    /// @param lexer The lexical analyzer to be used.
    /// @param node A scalar node.
    /// @param indent The current indentation width. Can be updated in this function.
    /// @param line The number of processed lines. Can be updated in this function.
    /// @param token The storage for last lexical token.
    /// @return true if next token has already been got, false otherwise.
    void deserialize_scalar(
        lexer_type& lexer, event_node_type&& node, uint32_t& indent, uint32_t& line, lexical_token& token) {
        token = lexer.get_next_token();
        const bool is_mapping_key = mp_current_node->is_mapping() || token.type == lexical_token_t::KEY_SEPARATOR;
        if (is_mapping_key) {
            apply_node_properties(node);
        }
        else if (!node.is_alias()) {
            if FK_YAML_UNLIKELY (m_deferred_properties.has_anchor() && m_pending_properties.has_anchor()) {
                throw parse_error("anchor name cannot be specified more than once to the same node.", line, indent);
            }
            if FK_YAML_UNLIKELY (m_deferred_properties.has_tag() && m_pending_properties.has_tag()) {
                throw parse_error("tag name cannot be specified more than once to the same node.", line, indent);
            }
            apply_deferred_properties(node);
            apply_node_properties(node);
        }

        if (!m_context_stack.empty() && m_context_stack.back().state == context_state_t::FLOW_MAPPING_EXPLICIT_KEY) {
            assign_node_value(std::move(node), line, indent);
            indent = lexer.get_last_token_begin_pos();
            line = lexer.get_lines_processed();
            return;
        }

        if (mp_current_node->is_mapping()) {
            // An implicit key in the block context must be followed by the ":" indicator on the same line, while in
            // the flow context the two can be separated by line breaks.
            // ```yaml
            // {"foo"
            // : "bar"}
            // ```
            const bool is_key_sep_followed = (token.type == lexical_token_t::KEY_SEPARATOR) &&
                                             (line == lexer.get_lines_processed() || m_flow_context_state.is_active());
            if (!is_key_sep_followed) {
                // A flow mapping entry may consist of a key alone, with both the ":" indicator and the
                // value omitted. The entry then ends at the separator or the suffix which follows it.
                // ```yaml
                // {foo, bar: baz}
                // ```
                const bool ends_omitted_entry = (token.type == lexical_token_t::VALUE_SEPARATOR) ||
                                                (token.type == lexical_token_t::MAPPING_FLOW_END);
                if FK_YAML_UNLIKELY (!ends_omitted_entry) {
                    throw parse_error(
                        "The \":\" mapping value indicator must be followed after a mapping key.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }
            }
            check_tab_in_indentation(lexer, line, indent);
            add_new_key(std::move(node), line, indent);
        }
        else if (token.type == lexical_token_t::KEY_SEPARATOR) {
            if FK_YAML_UNLIKELY (line != lexer.get_lines_processed()) {
                // This path is for explicit mapping key separator like:
                // ```yaml
                //   ? foo
                //   : bar
                // # ^ this separator
                // ```
                assign_node_value(std::move(node), line, indent);
                indent = lexer.get_last_token_begin_pos();
                line = lexer.get_lines_processed();

                if (m_context_stack.empty()) {
                    throw parse_error("invalid explicit mapping key separator is found.", line, indent);
                }

                if (m_context_stack.back().state != context_state_t::BLOCK_MAPPING_EXPLICIT_KEY) {
                    const parse_context& cur_context = m_context_stack.back();
                    if (cur_context.state == context_state_t::BLOCK_MAPPING && cur_context.indent == indent) {
                        // A key separator which begins a line belongs to an entry with an empty key.
                        // ```yaml
                        // foo: bar
                        // : baz
                        // # -> {foo: bar, null: baz}
                        // ```
                        add_empty_key_entry(lexer, token, line, indent);
                        return;
                    }

                    pop_to_parent_node(line, indent, [indent](const parse_context& c) {
                        return c.state == context_state_t::BLOCK_MAPPING_EXPLICIT_KEY && indent == c.indent;
                    });
                }
                return;
            }

            if (mp_current_node->is_scalar()) {
                if FK_YAML_LIKELY (!m_context_stack.empty()) {
                    parse_context& cur_context = m_context_stack.back();

                    const bool is_omitted_mapping_value_with_properties =
                        cur_context.state == context_state_t::MAPPING_VALUE && cur_context.indent == indent &&
                        defers_props();
                    if (is_omitted_mapping_value_with_properties) {
                        pop_to_parent_node(line, indent, [indent](const parse_context& c) {
                            return c.state == context_state_t::BLOCK_MAPPING && indent == c.indent;
                        });
                        check_tab_in_indentation(lexer, line, indent);
                        add_new_key(std::move(node), line, indent);
                        indent = lexer.get_last_token_begin_pos();
                        line = lexer.get_lines_processed();
                        return;
                    }

                    switch (cur_context.state) {
                    case context_state_t::BLOCK_MAPPING_EXPLICIT_KEY:
                        if (cur_context.indent == indent) {
                            // A mapping entry which follows an explicit key without its value, for example:
                            // ```yaml
                            // ? foo
                            // bar: 123
                            // # -> {foo: null, bar: 123}
                            // ```
                            add_explicit_key_with_null_value();
                            check_tab_in_indentation(lexer, line, indent);
                            add_new_key(std::move(node), line, indent);
                            indent = lexer.get_last_token_begin_pos();
                            line = lexer.get_lines_processed();
                            return;
                        }

                        m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                        break;
                    case context_state_t::BLOCK_MAPPING_EXPLICIT_VALUE:
                        m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                        break;
                    case context_state_t::BLOCK_SEQUENCE_ENTRY:
                        if FK_YAML_UNLIKELY (cur_context.indent >= indent) {
                            // This handles combination of empty block sequence entry and block mapping entry with the
                            // same indentation level, for examples:
                            // ```yaml
                            // foo:
                            //   bar:
                            //   -         # These entries are indented
                            //   baz: 123  # with the same width.
                            // # ^^^
                            // ```
                            pop_to_parent_node(line, indent, [indent](const parse_context& c) {
                                return c.state == context_state_t::BLOCK_MAPPING && indent == c.indent;
                            });
                            check_tab_in_indentation(lexer, line, indent);
                            add_new_key(std::move(node), line, indent);
                            indent = lexer.get_last_token_begin_pos();
                            line = lexer.get_lines_processed();
                            return;
                        }

                        m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                        break;
                    case context_state_t::MAPPING_VALUE:
                    default:
                        if FK_YAML_UNLIKELY (cur_context.line == line) {
                            throw parse_error("Multiple mapping keys are specified on the same line.", line, indent);
                        }
                        cur_context.line = line;
                        cur_context.indent = indent;
                        cur_context.state = context_state_t::BLOCK_MAPPING;
                        break;
                    }

                    *mp_current_node = event_node_type::block_mapping(m_arena);
                    // The scalar turned out to be a key, so any deferred properties are for the mapping
                    // which it begins rather than for the key itself.
                    apply_deferred_properties(*mp_current_node);
                }
                else {
                    // root mapping node
                    if FK_YAML_UNLIKELY (m_document_state.is_on_explicit_document_start_line(line)) {
                        throw parse_error("A block mapping entry cannot be on the document start line.", line, indent);
                    }

                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                    *mp_current_node = event_node_type::block_mapping(m_arena);
                    apply_deferred_properties(*mp_current_node);
                }
            }
            check_tab_in_indentation(lexer, line, indent);
            add_new_key(std::move(node), line, indent);
        }
        else {
            if (defers_props()) {
                // Non-alias values consume deferred properties before reaching this point. An alias
                // cannot carry properties, which is only known once it turns out not to be a key.
                // https://yaml.org/spec/1.2.2/#71-alias-nodes
                throw parse_error("Node properties cannot be specified to an alias node.", line, indent);
            }
            if FK_YAML_UNLIKELY (m_context_stack.empty() && !is_document_end_token(token.type)) {
                throw parse_error("Multiple root nodes are not allowed in the same document.", line, indent);
            }
            assign_node_value(std::move(node), line, indent);
        }

        indent = lexer.get_last_token_begin_pos();
        line = lexer.get_lines_processed();
    }

    /// @brief Returns the parse context on the top of the context stack.
    /// @note
    /// Accessing an empty context stack is undefined behavior, so the emptiness is checked before the access.
    /// No known input reaches the throw now that a parse context owns its key node, which is why it is left out
    /// of the coverage measurement. The check stays because the alternative for a caller is dereferencing an
    /// invalid iterator.
    /// @param line The current line count.
    /// @param indent The current indentation width.
    /// @return The parse context on the top of the context stack.
    parse_context& current_context(const uint32_t line, const uint32_t indent) {
        // LCOV_EXCL_START
        if FK_YAML_UNLIKELY (m_context_stack.empty()) {
            throw parse_error("No parent context is found.", line, indent);
        }
        // LCOV_EXCL_STOP
        return m_context_stack.back();
    }

    static bool is_document_end_token(const lexical_token_t type) noexcept {
        return type == lexical_token_t::END_OF_BUFFER || type == lexical_token_t::END_OF_DIRECTIVES ||
               type == lexical_token_t::END_OF_DOCUMENT;
    }

    /// @brief Adds a mapping entry whose key is empty and moves to the token which follows it.
    /// @note
    /// An empty key is a null node. Its value can be omitted as well, in which case the following token
    /// belongs to the parent mapping rather than to this entry.
    /// ```yaml
    /// :
    /// foo: bar
    /// # -> {null: null, foo: bar}
    /// ```
    /// @param lexer The lexical analyzer to be used.
    /// @param token The storage for the token which follows the key separator.
    /// @param line The line of the key separator. Updated to the line of the following token.
    /// @param indent The indentation width of the key separator. Updated for the following token.
    void add_empty_key_entry(lexer_type& lexer, lexical_token& token, uint32_t& line, uint32_t& indent) {
        const uint32_t key_line = line;
        const uint32_t key_indent = indent;
        event_node_type key_node(m_arena);
        materialize_tagged_empty_node(key_node, line, indent);
        apply_node_properties(key_node);
        add_new_key(std::move(key_node), line, indent);

        token = lexer.get_next_token();
        line = lexer.get_lines_processed();
        indent = lexer.get_last_token_begin_pos();

        const bool is_block_sequence_entry =
            token.type == lexical_token_t::SEQUENCE_BLOCK_PREFIX && line > key_line && indent <= key_indent;
        if (is_block_sequence_entry) {
            initialize_block_sequence_value(line, indent, false);

            token = lexer.get_next_token();
            line = lexer.get_lines_processed();
            indent = lexer.get_last_token_begin_pos();
            return;
        }

        if (line > key_line && indent <= key_indent) {
            pop_to_parent_node(line, indent, [key_indent](const parse_context& c) {
                return c.state == context_state_t::BLOCK_MAPPING && key_indent == c.indent;
            });
        }
    }

    /// @brief Initializes a block sequence as the current mapping value.
    /// @param line The line where the sequence begins.
    /// @param indent The indentation width where the sequence begins.
    /// @param apply_properties Whether pending node properties belong to the sequence.
    void initialize_block_sequence_value(const uint32_t line, const uint32_t indent, const bool apply_properties) {
        *mp_current_node = event_node_type::block_sequence(m_arena);
        if (apply_properties) {
            apply_deferred_properties(*mp_current_node);
            apply_node_properties(*mp_current_node);
        }
        mp_current_node->start_streaming();
        mp_current_node->add_sequence_entry(event_node_type(m_arena));

        auto& cur_context = m_context_stack.back();
        cur_context.line = line;
        cur_context.indent = indent;
        cur_context.state = context_state_t::BLOCK_SEQUENCE;

        event_node_type entry = mp_current_node->get_last_sequence_element();
        m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_SEQUENCE_ENTRY, &entry);
        mp_current_node = &m_context_stack.back().node;
    }

    /// @brief Checks whether an explicit key context exists at the given indentation.
    /// @param indent The indentation width of the explicit key context to look for.
    /// @return true if such a context is on the context stack, false otherwise.
    bool has_explicit_key_context_at(const uint32_t indent) const noexcept {
        const auto is_explicit_key_context = [indent](const parse_context& c) {
            return c.state == context_state_t::BLOCK_MAPPING_EXPLICIT_KEY && indent == c.indent;
        };
        return std::any_of(m_context_stack.rbegin(), m_context_stack.rend(), is_explicit_key_context);
    }

    /// @brief Settles an explicit key into its value context when properties follow its separator.
    /// @note As in `? key` followed by `: &anchor`, the value context of the explicit key is settled
    /// before its node is known so that the properties which follow the separator are bound to the
    /// (empty) value node. Does nothing when no matching explicit key context exists.
    /// @param line The line of the key separator.
    /// @param indent The indentation width of the key separator.
    void settle_explicit_key_value_with_props(const uint32_t line, const uint32_t indent) {
        if (!has_explicit_key_context_at(indent)) {
            return;
        }
        if (m_context_stack.back().state != context_state_t::BLOCK_MAPPING_EXPLICIT_KEY) {
            const auto is_explicit_key_context = [indent](const parse_context& c) {
                return c.state == context_state_t::BLOCK_MAPPING_EXPLICIT_KEY && indent == c.indent;
            };
            pop_to_parent_node(line, indent, is_explicit_key_context);
        }
        add_explicit_key_with_empty_value(line, indent);
    }

    /// @brief Adds an entry for an explicit key and makes its value node the current node.
    /// @note The current context must be the context of the explicit key.
    /// @param line The line where the value of the explicit key begins.
    /// @param indent The indentation width where the value of the explicit key begins.
    void add_explicit_key_with_empty_value(const uint32_t line, const uint32_t indent) {
        FK_YAML_ASSERT(m_context_stack.back().state == context_state_t::BLOCK_MAPPING_EXPLICIT_KEY);

        // Deferred properties precede the key, so they belong to the key rather than to its value.
        apply_deferred_properties(m_context_stack.back().node);

        event_node_type key_node = m_context_stack.back().node.reference();
        m_context_stack.pop_back();
        event_node_type* p_parent_node = &current_context(line, indent).node;
        p_parent_node->start_streaming();
        event_node_type value = p_parent_node->add_mapping_entry(std::move(key_node));
        m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING_EXPLICIT_VALUE, &value);
        mp_current_node = &m_context_stack.back().node;
    }

    /// @brief Adds an explicit flow mapping key and makes its value node the current node.
    /// @param line The line where the value separator or entry ending is found.
    /// @param indent The indentation width where the value separator or entry ending is found.
    void add_explicit_flow_key(const uint32_t line, const uint32_t indent) {
        FK_YAML_ASSERT(m_context_stack.back().state == context_state_t::FLOW_MAPPING_EXPLICIT_KEY);

        apply_deferred_properties(m_context_stack.back().node);
        event_node_type key_node = m_context_stack.back().node.reference();
        m_context_stack.pop_back();
        mp_current_node = &current_context(line, indent).node;
        add_new_key(std::move(key_node), line, indent);
    }

    /// @brief Adds an entry with a null value for an explicit key which is not followed by its value.
    /// @note
    /// An explicit key is kept in its own context until its value is found. If no value follows the key, the
    /// entry must still be added to the parent mapping since an omitted value is a null value.
    /// ```yaml
    /// ? foo
    /// ? bar
    /// # -> {foo: null, bar: null}
    /// ```
    /// @return true if an entry has been added, false if the current context is not an explicit key.
    bool add_explicit_key_with_null_value() {
        const bool is_explicit_key =
            m_context_stack.size() > 1 && m_context_stack.back().state == context_state_t::BLOCK_MAPPING_EXPLICIT_KEY;
        if (!is_explicit_key) {
            return false;
        }

        // Deferred properties precede the key, so they belong to the key rather than to its value.
        apply_deferred_properties(m_context_stack.back().node);

        event_node_type key_node = m_context_stack.back().node.reference();
        m_context_stack.pop_back();
        m_context_stack.back().node.start_streaming();
        m_context_stack.back().node.add_mapping_entry(std::move(key_node));
        mp_current_node = &m_context_stack.back().node;
        return true;
    }

    /// @brief Pops parent contexts to a block mapping with the given indentation.
    /// @tparam Pred Functor type to test parent contexts.
    /// @param line The current line count.
    /// @param indent The indentation level of the target parent block mapping.
    template <typename Pred>
    void pop_to_parent_node(uint32_t line, uint32_t indent, Pred&& pred) {
        // LCOV_EXCL_START
        if FK_YAML_UNLIKELY (m_context_stack.empty()) {
            throw parse_error("No parent block mapping is found.", line, indent);
        }
        // LCOV_EXCL_STOP

        auto itr = std::find_if(m_context_stack.rbegin(), m_context_stack.rend(), std::forward<Pred>(pred));
        const bool is_indent_valid = (itr != m_context_stack.rend());
        if FK_YAML_UNLIKELY (!is_indent_valid) {
            throw parse_error("Detected invalid indentation.", line, indent);
        }

        const auto pop_count = static_cast<uint32_t>(std::distance(m_context_stack.rbegin(), itr));
        const auto target_size = m_context_stack.size() - pop_count;

        while (m_context_stack.size() > target_size) {
            if (m_context_stack.back().state == context_state_t::BLOCK_MAPPING_EXPLICIT_KEY) {
                // Simply popping an explicit key context would drop the entry for the `? key`
                // entirely. Settle it as an entry with a null value before moving to the parent.
                add_explicit_key_with_null_value();
                continue;
            }

            if (defers_props()) {
                // The entry which the properties preceded ends here without a node of its own, so they
                // belong to its empty value. Any node which did follow them would have taken them before
                // its context could be popped, so the current node is still the empty one.
                // ```yaml
                // foo: &anchor
                // bar: 1        # the anchor is for the empty value of "foo".
                // ```
                materialize_tagged_empty_node(*mp_current_node, line, indent);
                // The empty node never received the document metainfo, in which an anchor name must be
                // registered for aliases to resolve it.
                apply_deferred_properties(*mp_current_node);
            }

            m_context_stack.pop_back();
        }

        mp_current_node = &m_context_stack.back().node;
    }

    /// @brief Closes the implicit single pair mapping wrapping a flow sequence entry, if one is open.
    /// @note
    /// A flow sequence entry may be a mapping entry written without braces, e.g. `[foo: 1]` meaning
    /// `[{foo: 1}]`. Such an entry is wrapped in a mapping when its key separator is found, and that
    /// wrapper must be closed once the entry ends, either at a separator or at the sequence suffix.
    /// Block content cannot appear inside a flow collection, so a block mapping context found here can
    /// only be that wrapper.
    /// @param line Current line.
    /// @param indent Current indentation.
    void close_single_pair_mapping(const uint32_t line, const uint32_t indent) {
        // LCOV_EXCL_START
        if FK_YAML_UNLIKELY (m_context_stack.empty()) {
            throw parse_error("No parent flow collection is found.", line, indent);
        }
        // LCOV_EXCL_STOP

        if (m_context_stack.back().state == context_state_t::BLOCK_MAPPING) {
            m_context_stack.pop_back();
            mp_current_node = &current_context(line, indent).node;
        }
    }

    /// @brief Adds a flow sequence entry which has node properties only, if one is pending.
    /// @note
    /// A flow sequence entry may consist of node properties only, in which case the entry is an empty node
    /// with those properties, e.g. `[foo, &anchor]` meaning `[foo, &anchor null]`. The entry is added once it
    /// ends, either at a separator or at the sequence suffix.
    /// @param line Current line.
    /// @param indent Current indentation.
    void close_empty_flow_sequence_entry(const uint32_t line, const uint32_t indent) {
        // LCOV_EXCL_START
        if FK_YAML_UNLIKELY (m_context_stack.empty()) {
            throw parse_error("No parent flow collection is found.", line, indent);
        }
        // LCOV_EXCL_STOP

        parse_context& last_context = m_context_stack.back();
        const bool is_sequence_entry = last_context.state == context_state_t::FLOW_SEQUENCE ||
                                       last_context.state == context_state_t::FLOW_SEQUENCE_KEY;
        if (is_sequence_entry && (m_pending_properties.has_anchor() || m_pending_properties.has_tag())) {
            event_node_type entry(m_arena);
            materialize_tagged_empty_node(entry, line, indent);
            apply_node_properties(entry);
            last_context.node.add_sequence_entry(std::move(entry));
            m_flow_context_state.set_token_state(flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX);
        }
    }

    /// @brief Completes a flow mapping entry whose value was omitted, if one is pending.
    /// @note
    /// A flow mapping entry may leave its value empty, in which case the value is null, e.g. `{foo: }`
    /// meaning `{foo: null}`. The value node is already null since `add_new_key` default constructs it,
    /// so the entry only needs its context closed once it ends, either at a separator or at the mapping
    /// suffix.
    /// @param line Current line.
    /// @param indent Current indentation.
    void close_omitted_mapping_value(const uint32_t line, const uint32_t indent) {
        // LCOV_EXCL_START
        if FK_YAML_UNLIKELY (m_context_stack.empty()) {
            throw parse_error("No parent flow collection is found.", line, indent);
        }
        // LCOV_EXCL_STOP

        if (m_context_stack.back().state == context_state_t::MAPPING_VALUE) {
            materialize_tagged_empty_node(*mp_current_node, line, indent);
            apply_node_properties(*mp_current_node);
            m_context_stack.pop_back();
            mp_current_node = &current_context(line, indent).node;
            m_flow_context_state.set_token_state(flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX);
        }
    }

    /// @brief Materializes a tagged empty scalar event.
    void materialize_tagged_empty_node(event_node_type& node, const uint32_t /*line*/, const uint32_t /*indent*/) {
        if (m_pending_properties.has_tag() || m_deferred_properties.has_tag()) {
            node = event_node_type::scalar(m_arena, lexical_token_t::PLAIN_SCALAR, {});
        }
    }

    /// @brief Move the pending node properties aside until the node they belong to is known.
    /// @note They precede their node by a line, so the node which follows may carry properties of its
    /// own. Keeping the two apart lets both be bound to the right node.
    /// ```yaml
    /// foo: &map
    ///   &key bar: baz   # &map is for the mapping, &key is for the "bar" key.
    /// ```
    void defer_node_properties() {
        if (m_pending_properties.has_anchor()) {
            m_deferred_properties.store_anchor(m_pending_properties.release_anchor());
        }
        if (m_pending_properties.has_tag()) {
            m_deferred_properties.store_tag(m_pending_properties.release_tag());
        }
    }

    /// @brief Check whether any node properties are waiting to be bound.
    /// @return true if properties precede a node whose kind is not known yet, false otherwise.
    bool defers_props() const noexcept {
        return m_deferred_properties.has_anchor() || m_deferred_properties.has_tag();
    }

    /// @brief Set the node properties which precede their node to the given node.
    /// @param node A node type object the deferred properties belong to.
    void apply_deferred_properties(event_node_type& node) {
        apply_held_properties(take_deferred_properties(), node);
    }

    /// @brief Take the node properties which precede their node, leaving none deferred.
    /// @return The deferred node properties.
    pending_node_properties take_deferred_properties() noexcept {
        const pending_node_properties props = m_deferred_properties;
        m_deferred_properties = {};
        return props;
    }

    /// @brief Set the given node properties to the given node.
    /// @param props The node properties to be set.
    /// @param node A node type object the properties belong to.
    void apply_held_properties(const pending_node_properties& props, event_node_type& node) {
        if (props.has_anchor()) {
            node.set_anchor(props.get_anchor());
        }

        if (props.has_tag()) {
            node.set_tag(props.get_tag());
        }
    }

    /// @brief Set YAML node properties (anchor and/or tag names) to the given node.
    /// @param node A node type object to be set YAML node properties.
    void apply_node_properties(event_node_type& node) {
        if (m_pending_properties.has_anchor()) {
            node.set_anchor(m_pending_properties.release_anchor());
        }

        if (m_pending_properties.has_tag()) {
            node.set_tag(m_pending_properties.release_tag());
        }
    }

private:
    /// The currently focused YAML node.
    event_node_type* mp_current_node {nullptr};
    /// The stack of parse contexts.
    std::deque<parse_context> m_context_stack {};
    /// The current state of the document being parsed.
    document_state m_document_state {};
    /// The current state of the flow context.
    flow_context_state m_flow_context_state {};
    /// Node properties which precede their node and are not bound yet.
    pending_node_properties m_deferred_properties {};
    /// Node properties read for the current node and waiting to be applied.
    pending_node_properties m_pending_properties {};
    event_arena m_arena {};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_DESERIALIZER_HPP */
