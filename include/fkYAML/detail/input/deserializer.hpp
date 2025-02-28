//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_INPUT_DESERIALIZER_HPP
#define FK_YAML_DETAIL_INPUT_DESERIALIZER_HPP

#include <algorithm>
#include <deque>
#include <vector>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/document_metainfo.hpp>
#include <fkYAML/detail/input/lexical_analyzer.hpp>
#include <fkYAML/detail/input/scalar_parser.hpp>
#include <fkYAML/detail/input/tag_resolver.hpp>
#include <fkYAML/detail/meta/input_adapter_traits.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
#include <fkYAML/detail/node_attrs.hpp>
#include <fkYAML/detail/node_property.hpp>
#include <fkYAML/detail/types/lexical_token_t.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

/// @brief A class which provides the feature of deserializing YAML documents.
/// @tparam BasicNodeType A type of the container for deserialized YAML values.
template <typename BasicNodeType>
class basic_deserializer {
    static_assert(is_basic_node<BasicNodeType>::value, "basic_deserializer only accepts basic_node<...>");

    /** A type for the target basic_node. */
    using basic_node_type = BasicNodeType;
    /** A type for the lexical analyzer. */
    using lexer_type = lexical_analyzer;
    /** A type for the document metainfo. */
    using doc_metainfo_type = document_metainfo<basic_node_type>;
    /** A type for the tag resolver. */
    using tag_resolver_type = tag_resolver<basic_node_type>;
    /** A type for the scalar parser. */
    using scalar_parser_type = scalar_parser<basic_node_type>;
    /** A type for sequence node value containers. */
    using sequence_type = typename basic_node_type::sequence_type;
    /** A type for mapping node value containers. */
    using mapping_type = typename basic_node_type::mapping_type;

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
    };

    /// @brief Context information set for parsing.
    struct parse_context {
        /// @brief Construct a new parse_context object.
        parse_context() = default;

        /// @brief Construct a new parse_context object with non-default values for each parameter.
        /// @param line The current line. (count from zero)
        /// @param indent The indentation width in the current line. (count from zero)
        /// @param state The parse context type.
        /// @param p_node The underlying node associated to this context.
        parse_context(uint32_t line, uint32_t indent, context_state_t state, basic_node_type* p_node) noexcept
            : line(line),
              indent(indent),
              state(state),
              p_node(p_node) {
        }

        parse_context(const parse_context&) noexcept = default;
        parse_context& operator=(const parse_context&) noexcept = default;
        parse_context(parse_context&&) noexcept = default;
        parse_context& operator=(parse_context&&) noexcept = default;

        ~parse_context() {
            switch (state) {
            case context_state_t::BLOCK_MAPPING_EXPLICIT_KEY:
            case context_state_t::FLOW_SEQUENCE_KEY:
            case context_state_t::FLOW_MAPPING_KEY:
                delete p_node;
                p_node = nullptr;
                break;
            default:
                break;
            }
        }

        /// The current line. (count from zero)
        uint32_t line {0};
        /// The indentation width in the current line. (count from zero)
        uint32_t indent {0};
        /// The parse context type.
        context_state_t state {context_state_t::BLOCK_MAPPING};
        /// The pointer to the associated node to this context.
        basic_node_type* p_node {nullptr};
    };

    /// @brief Definitions of state types for expected flow token hints.
    enum class flow_token_state_t : std::uint8_t {
        NEEDS_VALUE_OR_SUFFIX,     //!< Either value or flow suffix (`]` or `}`)
        NEEDS_SEPARATOR_OR_SUFFIX, //!< Either separator (`,`) or flow suffix (`]` or `}`)
    };

public:
    /// @brief Construct a new basic_deserializer object.
    basic_deserializer() = default;

public:
    /// @brief Deserialize a single YAML document into a YAML node.
    /// @note
    /// If the input consists of multiple YAML documents, this function only parses the first.
    /// If the input may have multiple YAML documents all of which must be parsed into nodes,
    /// prefer the `deserialize_docs()` function.
    /// @tparam InputAdapterType The type of an input adapter object.
    /// @param input_adapter An input adapter object for the input source buffer.
    /// @return basic_node_type A root YAML node deserialized from the source string.
    template <typename InputAdapterType, enable_if_t<is_input_adapter<InputAdapterType>::value, int> = 0>
    basic_node_type deserialize(InputAdapterType&& input_adapter) { // NOLINT(cppcoreguidelines-missing-std-forward)
        const str_view input_view = input_adapter.get_buffer_view();
        lexer_type lexer(input_view);

        lexical_token_t type {lexical_token_t::END_OF_BUFFER};
        return deserialize_document(lexer, type);
    }

    /// @brief Deserialize multiple YAML documents into YAML nodes.
    /// @tparam InputAdapterType The type of an adapter object.
    /// @param input_adapter An input adapter object for the input source buffer.
    /// @return std::vector<basic_node_type> Root YAML nodes for deserialized YAML documents.
    template <typename InputAdapterType, enable_if_t<is_input_adapter<InputAdapterType>::value, int> = 0>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
    std::vector<basic_node_type> deserialize_docs(InputAdapterType&& input_adapter) {
        const str_view input_view = input_adapter.get_buffer_view();
        lexer_type lexer(input_view);

        std::vector<basic_node_type> nodes {};
        lexical_token_t type {lexical_token_t::END_OF_BUFFER};

        do {
            nodes.emplace_back(deserialize_document(lexer, type));
        } while (type != lexical_token_t::END_OF_BUFFER);

        return nodes;
    } // LCOV_EXCL_LINE

private:
    /// @brief Deserialize a YAML document into a YAML node.
    /// @param lexer The lexical analyzer to be used.
    /// @param last_type The variable to store the last lexical token type.
    /// @return basic_node_type A root YAML node deserialized from the YAML document.
    basic_node_type deserialize_document(lexer_type& lexer, lexical_token_t& last_type) {
        lexical_token token {};

        basic_node_type root;
        mp_current_node = &root;
        mp_meta = root.mp_meta;

        // parse directives first.
        deserialize_directives(lexer, token);

        // parse node properties for root node if any
        uint32_t line = lexer.get_lines_processed();
        uint32_t indent = lexer.get_last_token_begin_pos();
        const bool found_props = deserialize_node_properties(lexer, token, line, indent);

        switch (token.type) {
        case lexical_token_t::SEQUENCE_BLOCK_PREFIX: {
            root = basic_node_type::sequence({basic_node_type()});
            apply_directive_set(root);
            if (found_props) {
                // If node properties are found before the block sequence entry prefix, the properties belong to the
                // root sequence node.
                apply_node_properties(root);
            }

            parse_context context(
                lexer.get_lines_processed(), lexer.get_last_token_begin_pos(), context_state_t::BLOCK_SEQUENCE, &root);
            m_context_stack.emplace_back(context);

            mp_current_node = &(root.as_seq().back());
            apply_directive_set(*mp_current_node);
            context.state = context_state_t::BLOCK_SEQUENCE_ENTRY;
            context.p_node = mp_current_node;
            m_context_stack.emplace_back(std::move(context));

            token = lexer.get_next_token();
            line = lexer.get_lines_processed();
            indent = lexer.get_last_token_begin_pos();
            break;
        }
        case lexical_token_t::SEQUENCE_FLOW_BEGIN:
            ++m_flow_context_depth;
            lexer.set_context_state(true);
            root = basic_node_type::sequence();
            apply_directive_set(root);
            apply_node_properties(root);
            m_context_stack.emplace_back(
                lexer.get_lines_processed(), lexer.get_last_token_begin_pos(), context_state_t::FLOW_SEQUENCE, &root);
            token = lexer.get_next_token();
            line = lexer.get_lines_processed();
            indent = lexer.get_last_token_begin_pos();
            break;
        case lexical_token_t::MAPPING_FLOW_BEGIN:
            ++m_flow_context_depth;
            lexer.set_context_state(true);
            root = basic_node_type::mapping();
            apply_directive_set(root);
            apply_node_properties(root);
            m_context_stack.emplace_back(
                lexer.get_lines_processed(), lexer.get_last_token_begin_pos(), context_state_t::FLOW_MAPPING, &root);
            token = lexer.get_next_token();
            line = lexer.get_lines_processed();
            indent = lexer.get_last_token_begin_pos();
            break;
        case lexical_token_t::EXPLICIT_KEY_PREFIX: {
            // If the explicit key prefix (? ) is detected here, the root node of current document must be a mapping.
            // Also, tag and anchor if any are associated to the root mapping node.
            // No get_next_token() call here to handle the token event in the deserialize_node() function.
            root = basic_node_type::mapping();
            apply_directive_set(root);
            apply_node_properties(root);
            parse_context context(
                lexer.get_lines_processed(), lexer.get_last_token_begin_pos(), context_state_t::BLOCK_MAPPING, &root);
            m_context_stack.emplace_back(std::move(context));
            line = lexer.get_lines_processed();
            indent = lexer.get_last_token_begin_pos();
            break;
        }
        case lexical_token_t::BLOCK_LITERAL_SCALAR:
        case lexical_token_t::BLOCK_FOLDED_SCALAR:
            // If a block scalar token is detected here, current document contains single scalar.
            // Do nothing here since the token is handled in the deserialize_node() function.
            break;
        case lexical_token_t::PLAIN_SCALAR:
        case lexical_token_t::SINGLE_QUOTED_SCALAR:
        case lexical_token_t::DOUBLE_QUOTED_SCALAR:
        case lexical_token_t::ALIAS_PREFIX:
            // Defer handling the above token events until the next call on the deserialize_scalar() function since the
            // meaning depends on subsequent events.
            if (found_props && line < lexer.get_lines_processed()) {
                // If node properties and a followed node are on the different line, the properties belong to the root
                // node.
                if (m_needs_anchor_impl) {
                    m_root_anchor_name = m_anchor_name;
                    m_needs_anchor_impl = false;
                    m_anchor_name = {};
                }

                if (m_needs_tag_impl) {
                    m_root_tag_name = m_tag_name;
                    m_needs_tag_impl = false;
                    m_tag_name = {};
                }

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

        // reset parameters for the next call.
        mp_current_node = nullptr;
        mp_meta.reset();
        m_needs_tag_impl = false;
        m_needs_anchor_impl = false;
        m_flow_context_depth = 0;
        m_flow_token_state = flow_token_state_t::NEEDS_VALUE_OR_SUFFIX;
        m_context_stack.clear();

        return root;
    }

    /// @brief Deserializes the YAML directives if specified.
    /// @param lexer The lexical analyzer to be used.
    /// @param last_token Storage for last lexical token type.
    void deserialize_directives(lexer_type& lexer, lexical_token& last_token) {
        bool lacks_end_of_directives_marker = false;
        lexer.set_document_state(true);

        for (;;) {
            const lexical_token token = lexer.get_next_token();

            switch (token.type) {
            case lexical_token_t::YAML_VER_DIRECTIVE:
                if FK_YAML_UNLIKELY (mp_meta->is_version_specified) {
                    throw parse_error(
                        "YAML version cannot be specified more than once.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }

                mp_meta->version = convert_yaml_version(lexer.get_yaml_version());
                mp_meta->is_version_specified = true;
                lacks_end_of_directives_marker = true;
                break;
            case lexical_token_t::TAG_DIRECTIVE: {
                const str_view tag_handle_view = lexer.get_tag_handle();
                switch (tag_handle_view.size()) {
                case 1 /* ! */: {
                    const bool is_already_specified = !mp_meta->primary_handle_prefix.empty();
                    if FK_YAML_UNLIKELY (is_already_specified) {
                        throw parse_error(
                            "Primary handle cannot be specified more than once.",
                            lexer.get_lines_processed(),
                            lexer.get_last_token_begin_pos());
                    }
                    const str_view tag_prefix = lexer.get_tag_prefix();
                    mp_meta->primary_handle_prefix.assign(tag_prefix.begin(), tag_prefix.end());
                    lacks_end_of_directives_marker = true;
                    break;
                }
                case 2 /* !! */: {
                    const bool is_already_specified = !mp_meta->secondary_handle_prefix.empty();
                    if FK_YAML_UNLIKELY (is_already_specified) {
                        throw parse_error(
                            "Secondary handle cannot be specified more than once.",
                            lexer.get_lines_processed(),
                            lexer.get_last_token_begin_pos());
                    }
                    const str_view tag_prefix = lexer.get_tag_prefix();
                    mp_meta->secondary_handle_prefix.assign(tag_prefix.begin(), tag_prefix.end());
                    lacks_end_of_directives_marker = true;
                    break;
                }
                default /* !<handle>! */: {
                    std::string tag_handle(tag_handle_view.begin(), tag_handle_view.end());
                    const str_view tag_prefix_view = lexer.get_tag_prefix();
                    std::string tag_prefix(tag_prefix_view.begin(), tag_prefix_view.end());
                    const bool is_already_specified =
                        !(mp_meta->named_handle_map.emplace(std::move(tag_handle), std::move(tag_prefix)).second);
                    if FK_YAML_UNLIKELY (is_already_specified) {
                        throw parse_error(
                            "The same named handle cannot be specified more than once.",
                            lexer.get_lines_processed(),
                            lexer.get_last_token_begin_pos());
                    }
                    lacks_end_of_directives_marker = true;
                    break;
                }
                }
                break;
            }
            case lexical_token_t::INVALID_DIRECTIVE:
                // TODO: should output a warning log. Currently just ignore this case.
                break;
            case lexical_token_t::END_OF_DIRECTIVES:
                lacks_end_of_directives_marker = false;
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
                lexer.set_document_state(false);
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
            switch (token.type) {
            case lexical_token_t::EXPLICIT_KEY_PREFIX: {
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
                    *mp_current_node = basic_node_type::mapping();
                    apply_directive_set(*mp_current_node);
                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                    break;
                default:
                    break;
                }

                token = lexer.get_next_token();
                if (token.type == lexical_token_t::SEQUENCE_BLOCK_PREFIX) {
                    // heap-allocated node will be freed in handling the corresponding KEY_SEPARATOR event
                    auto* p_node = new basic_node_type(node_type::SEQUENCE);
                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING_EXPLICIT_KEY, p_node);

                    apply_directive_set(*p_node);
                    parse_context context(
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos(),
                        context_state_t::BLOCK_SEQUENCE,
                        p_node);
                    m_context_stack.emplace_back(context);

                    p_node->as_seq().emplace_back(basic_node_type());
                    mp_current_node = &(p_node->as_seq().back());
                    apply_directive_set(*mp_current_node);
                    context.state = context_state_t::BLOCK_SEQUENCE_ENTRY;
                    context.p_node = mp_current_node;
                    m_context_stack.emplace_back(std::move(context));

                    break;
                }

                // heap-allocated node will be freed in handling the corresponding KEY_SEPARATOR event
                m_context_stack.emplace_back(
                    line, indent, context_state_t::BLOCK_MAPPING_EXPLICIT_KEY, new basic_node_type());
                mp_current_node = m_context_stack.back().p_node;
                apply_directive_set(*mp_current_node);
                indent = lexer.get_last_token_begin_pos();
                line = lexer.get_lines_processed();

                continue;
            }
            case lexical_token_t::KEY_SEPARATOR: {
                FK_YAML_ASSERT(!m_context_stack.empty());
                if FK_YAML_UNLIKELY (m_context_stack.back().state == context_state_t::BLOCK_SEQUENCE_ENTRY) {
                    // empty mapping keys are not supported.
                    // ```yaml
                    // - : foo
                    // ```
                    throw parse_error("sequence key should not be empty.", line, indent);
                }

                if (m_flow_context_depth > 0) {
                    break;
                }

                // hold the line count of the key separator for later use.
                const uint32_t old_indent = indent;
                const uint32_t old_line = line;

                token = lexer.get_next_token();
                line = lexer.get_lines_processed();
                indent = lexer.get_last_token_begin_pos();

                const bool found_props = deserialize_node_properties(lexer, token, line, indent);
                if (found_props && line == lexer.get_lines_processed()) {
                    // defer applying node properties for the subsequent node on the same line.
                    continue;
                }

                line = lexer.get_lines_processed();
                indent = lexer.get_last_token_begin_pos();

                const bool is_implicit_same_line =
                    (line == old_line) && (m_context_stack.empty() || old_indent > m_context_stack.back().indent);
                if (is_implicit_same_line) {
                    // a key separator for an implicit key with its value on the same line.
                    continue;
                }

                if (line > old_line) {
                    if (m_needs_tag_impl) {
                        const tag_t tag_type = tag_resolver_type::resolve_tag(m_tag_name, mp_meta);
                        if (tag_type == tag_t::MAPPING || tag_type == tag_t::CUSTOM_TAG) {
                            // set YAML node properties here to distinguish them from those for the first key node
                            // as shown in the following snippet:
                            //
                            // ```yaml
                            // foo: !!map
                            //   !!str 123: true
                            //   ^
                            //   this !!str tag overwrites the preceding !!map tag.
                            // ```
                            *mp_current_node = basic_node_type::mapping();
                            apply_directive_set(*mp_current_node);
                            apply_node_properties(*mp_current_node);
                            m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                            continue;
                        }
                    }

                    if (token.type == lexical_token_t::SEQUENCE_BLOCK_PREFIX) {
                        // a key separator preceding block sequence entries
                        *mp_current_node = basic_node_type::sequence({basic_node_type()});
                        apply_directive_set(*mp_current_node);
                        apply_node_properties(*mp_current_node);
                        auto& cur_context = m_context_stack.back();
                        cur_context.line = line;
                        cur_context.indent = indent;
                        cur_context.state = context_state_t::BLOCK_SEQUENCE;

                        mp_current_node = &(mp_current_node->as_seq().back());
                        apply_directive_set(*mp_current_node);
                        parse_context entry_context = cur_context;
                        entry_context.state = context_state_t::BLOCK_SEQUENCE_ENTRY;
                        entry_context.p_node = mp_current_node;
                        m_context_stack.emplace_back(std::move(entry_context));

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
                            *mp_current_node = basic_node_type::mapping();
                            m_context_stack.emplace_back(
                                line_after_props, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                            apply_directive_set(*mp_current_node);
                            apply_node_properties(*mp_current_node);
                        }

                        continue;
                    }

                    if (indent <= m_context_stack.back().indent) {
                        FK_YAML_ASSERT(m_context_stack.back().state == context_state_t::MAPPING_VALUE);

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

                    // defer checking the existence of a key separator after the following scalar until the next
                    // deserialize_scalar() call.
                    continue;
                }

                // handle explicit mapping key separators.
                FK_YAML_ASSERT(m_context_stack.back().state == context_state_t::BLOCK_MAPPING_EXPLICIT_KEY);

                basic_node_type key_node = std::move(*m_context_stack.back().p_node);
                m_context_stack.pop_back();
                m_context_stack.back().p_node->as_map().emplace(key_node, basic_node_type());
                mp_current_node = &(m_context_stack.back().p_node->operator[](std::move(key_node)));
                m_context_stack.emplace_back(
                    old_line, old_indent, context_state_t::BLOCK_MAPPING_EXPLICIT_VALUE, mp_current_node);

                if (token.type == lexical_token_t::SEQUENCE_BLOCK_PREFIX) {
                    *mp_current_node = basic_node_type::sequence({basic_node_type()});
                    apply_directive_set(*mp_current_node);
                    apply_node_properties(*mp_current_node);
                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_SEQUENCE, mp_current_node);

                    mp_current_node = &(mp_current_node->as_seq().back());
                    parse_context entry_context = m_context_stack.back();
                    entry_context.state = context_state_t::BLOCK_SEQUENCE_ENTRY;
                    entry_context.p_node = mp_current_node;
                    m_context_stack.emplace_back(std::move(entry_context));
                    break;
                }

                continue;
            }
            case lexical_token_t::ANCHOR_PREFIX:
            case lexical_token_t::TAG_PREFIX:
                deserialize_node_properties(lexer, token, line, indent);
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
            case lexical_token_t::SEQUENCE_BLOCK_PREFIX: {
                FK_YAML_ASSERT(!m_context_stack.empty());
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

                    *mp_current_node = basic_node_type::sequence();
                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_SEQUENCE, mp_current_node);
                    apply_directive_set(*mp_current_node);
                    apply_node_properties(*mp_current_node);
                }

                auto& seq = mp_current_node->as_seq();
                seq.emplace_back(basic_node_type());
                mp_current_node = &(seq.back());
                apply_directive_set(*mp_current_node);
                m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_SEQUENCE_ENTRY, mp_current_node);
                break;
            }
            case lexical_token_t::SEQUENCE_FLOW_BEGIN:
                if (m_flow_context_depth == 0) {
                    lexer.set_context_state(true);

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
                else if FK_YAML_UNLIKELY (m_flow_token_state == flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX) {
                    throw parse_error("Flow sequence beginning is found without separated with a comma.", line, indent);
                }

                ++m_flow_context_depth;

                switch (m_context_stack.back().state) {
                case context_state_t::BLOCK_SEQUENCE:
                case context_state_t::FLOW_SEQUENCE:
                    mp_current_node->as_seq().emplace_back(basic_node_type::sequence());
                    mp_current_node = &(mp_current_node->as_seq().back());
                    m_context_stack.emplace_back(line, indent, context_state_t::FLOW_SEQUENCE, mp_current_node);
                    break;
                case context_state_t::BLOCK_MAPPING:
                case context_state_t::FLOW_MAPPING:
                    // heap-allocated node will be freed in handling the corresponding SEQUENCE_FLOW_END event.
                    m_context_stack.emplace_back(
                        line, indent, context_state_t::FLOW_SEQUENCE_KEY, new basic_node_type(node_type::SEQUENCE));
                    mp_current_node = m_context_stack.back().p_node;
                    break;
                default: {
                    *mp_current_node = basic_node_type::sequence();
                    parse_context& last_context = m_context_stack.back();
                    last_context.line = line;
                    last_context.indent = indent;
                    last_context.state = context_state_t::FLOW_SEQUENCE;
                    break;
                }
                }

                apply_directive_set(*mp_current_node);
                apply_node_properties(*mp_current_node);

                m_flow_token_state = flow_token_state_t::NEEDS_VALUE_OR_SUFFIX;
                break;
            case lexical_token_t::SEQUENCE_FLOW_END: {
                if FK_YAML_UNLIKELY (m_flow_context_depth == 0) {
                    throw parse_error("Flow sequence ending is found outside the flow context.", line, indent);
                }

                if (--m_flow_context_depth == 0) {
                    lexer.set_context_state(false);
                }

                // find the corresponding flow sequence beginning.
                auto itr = std::find_if( // LCOV_EXCL_LINE
                    m_context_stack.rbegin(),
                    m_context_stack.rend(),
                    [](const parse_context& c) {
                        switch (c.state) {
                        case context_state_t::FLOW_SEQUENCE_KEY:
                        case context_state_t::FLOW_SEQUENCE:
                            return true;
                        default:
                            return false;
                        }
                    });

                const bool is_valid = itr != m_context_stack.rend();
                if FK_YAML_UNLIKELY (!is_valid) {
                    throw parse_error("No corresponding flow sequence beginning is found.", line, indent);
                }

                // keep the last state for later processing.
                parse_context& last_context = m_context_stack.back();
                mp_current_node = last_context.p_node;
                last_context.p_node = nullptr;
                indent = last_context.indent;
                const context_state_t state = last_context.state;
                m_context_stack.pop_back();

                // handle cases where the flow sequence is a mapping key node.

                if (!m_context_stack.empty() && state == context_state_t::FLOW_SEQUENCE_KEY) {
                    basic_node_type key_node = std::move(*mp_current_node);
                    delete mp_current_node;
                    mp_current_node = m_context_stack.back().p_node;
                    m_flow_token_state = flow_token_state_t::NEEDS_VALUE_OR_SUFFIX;

                    add_new_key(std::move(key_node), line, indent);
                    break;
                }

                token = lexer.get_next_token();
                if (token.type == lexical_token_t::KEY_SEPARATOR) {
                    basic_node_type key_node = basic_node_type::mapping();
                    apply_directive_set(key_node);
                    mp_current_node->swap(key_node);

                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                    m_flow_token_state = flow_token_state_t::NEEDS_VALUE_OR_SUFFIX;

                    add_new_key(std::move(key_node), line, indent);
                }
                else {
                    if (!m_context_stack.empty()) {
                        mp_current_node = m_context_stack.back().p_node;
                    }
                    if (m_flow_context_depth > 0) {
                        m_flow_token_state = flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX;
                    }
                }

                indent = lexer.get_last_token_begin_pos();
                line = lexer.get_lines_processed();
                continue;
            }
            case lexical_token_t::MAPPING_FLOW_BEGIN:
                if (m_flow_context_depth == 0) {
                    lexer.set_context_state(true);

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
                else if FK_YAML_UNLIKELY (m_flow_token_state == flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX) {
                    throw parse_error("Flow mapping beginning is found without separated with a comma.", line, indent);
                }

                ++m_flow_context_depth;

                switch (m_context_stack.back().state) {
                case context_state_t::BLOCK_SEQUENCE:
                case context_state_t::FLOW_SEQUENCE:
                    mp_current_node->as_seq().emplace_back(basic_node_type::mapping());
                    mp_current_node = &(mp_current_node->as_seq().back());
                    m_context_stack.emplace_back(line, indent, context_state_t::FLOW_MAPPING, mp_current_node);
                    break;
                case context_state_t::BLOCK_MAPPING:
                case context_state_t::FLOW_MAPPING:
                    // heap-allocated node will be freed in handling the corresponding MAPPING_FLOW_END event.
                    m_context_stack.emplace_back(
                        line, indent, context_state_t::FLOW_MAPPING_KEY, new basic_node_type(node_type::MAPPING));
                    mp_current_node = m_context_stack.back().p_node;
                    break;
                default: {
                    *mp_current_node = basic_node_type::mapping();
                    parse_context& last_context = m_context_stack.back();
                    last_context.line = line;
                    last_context.indent = indent;
                    last_context.state = context_state_t::FLOW_MAPPING;
                    break;
                }
                }

                apply_directive_set(*mp_current_node);
                apply_node_properties(*mp_current_node);

                line = lexer.get_lines_processed();
                indent = lexer.get_last_token_begin_pos();

                m_flow_token_state = flow_token_state_t::NEEDS_VALUE_OR_SUFFIX;
                break;
            case lexical_token_t::MAPPING_FLOW_END: {
                if FK_YAML_UNLIKELY (m_flow_context_depth == 0) {
                    throw parse_error("Flow mapping ending is found outside the flow context.", line, indent);
                }

                if (--m_flow_context_depth == 0) {
                    lexer.set_context_state(false);
                }

                // find the corresponding flow mapping beginning.
                auto itr = std::find_if( // LCOV_EXCL_LINE
                    m_context_stack.rbegin(),
                    m_context_stack.rend(),
                    [](const parse_context& c) {
                        switch (c.state) {
                        case context_state_t::FLOW_MAPPING_KEY:
                        case context_state_t::FLOW_MAPPING:
                            return true;
                        default:
                            return false;
                        }
                    });

                const bool is_valid = itr != m_context_stack.rend();
                if FK_YAML_UNLIKELY (!is_valid) {
                    throw parse_error("No corresponding flow mapping beginning is found.", line, indent);
                }

                // keep the last state for later processing.
                parse_context& last_context = m_context_stack.back();
                mp_current_node = last_context.p_node;
                last_context.p_node = nullptr;
                indent = last_context.indent;
                const context_state_t state = last_context.state;
                m_context_stack.pop_back();

                // handle cases where the flow mapping is a mapping key node.

                if (!m_context_stack.empty() && state == context_state_t::FLOW_MAPPING_KEY) {
                    basic_node_type key_node = std::move(*mp_current_node);
                    delete mp_current_node;
                    mp_current_node = m_context_stack.back().p_node;
                    m_flow_token_state = flow_token_state_t::NEEDS_VALUE_OR_SUFFIX;

                    add_new_key(std::move(key_node), line, indent);
                    break;
                }

                token = lexer.get_next_token();
                if (token.type == lexical_token_t::KEY_SEPARATOR) {
                    basic_node_type key_node = basic_node_type::mapping();
                    apply_directive_set(key_node);
                    mp_current_node->swap(key_node);

                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                    m_flow_token_state = flow_token_state_t::NEEDS_VALUE_OR_SUFFIX;

                    add_new_key(std::move(key_node), line, indent);
                }
                else {
                    if (!m_context_stack.empty()) {
                        mp_current_node = m_context_stack.back().p_node;
                    }
                    if (m_flow_context_depth > 0) {
                        m_flow_token_state = flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX;
                    }
                }

                indent = lexer.get_last_token_begin_pos();
                line = lexer.get_lines_processed();
                continue;
            }
            case lexical_token_t::VALUE_SEPARATOR:
                FK_YAML_ASSERT(m_flow_context_depth > 0);
                if FK_YAML_UNLIKELY (m_flow_token_state != flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX) {
                    throw parse_error("invalid value separator is found.", line, indent);
                }
                m_flow_token_state = flow_token_state_t::NEEDS_VALUE_OR_SUFFIX;
                break;
            case lexical_token_t::ALIAS_PREFIX: {
                // An alias node must not specify any properties (tag, anchor).
                // https://yaml.org/spec/1.2.2/#71-alias-nodes
                if FK_YAML_UNLIKELY (m_needs_tag_impl) {
                    throw parse_error("Tag cannot be specified to an alias node", line, indent);
                }
                if FK_YAML_UNLIKELY (m_needs_anchor_impl) {
                    throw parse_error("Anchor cannot be specified to an alias node.", line, indent);
                }

                std::string token_str = std::string(token.str.begin(), token.str.end());

                const auto anchor_counts = static_cast<uint32_t>(mp_meta->anchor_table.count(token_str));
                if FK_YAML_UNLIKELY (anchor_counts == 0) {
                    throw parse_error("The given anchor name must appear prior to the alias node.", line, indent);
                }

                basic_node_type node {};
                node.m_attrs |= detail::node_attr_bits::alias_bit;
                node.m_prop.anchor = std::move(token_str);
                detail::node_attr_bits::set_anchor_offset(anchor_counts - 1, node.m_attrs);

                apply_directive_set(node);
                apply_node_properties(node);

                deserialize_scalar(lexer, std::move(node), indent, line, token);
                continue;
            }
            case lexical_token_t::PLAIN_SCALAR:
            case lexical_token_t::SINGLE_QUOTED_SCALAR:
            case lexical_token_t::DOUBLE_QUOTED_SCALAR: {
                tag_t tag_type {tag_t::NONE};
                if (m_needs_tag_impl) {
                    tag_type = tag_resolver_type::resolve_tag(m_tag_name, mp_meta);
                }

                basic_node_type node = scalar_parser_type(line, indent).parse_flow(token.type, tag_type, token.str);
                apply_directive_set(node);
                apply_node_properties(node);

                deserialize_scalar(lexer, std::move(node), indent, line, token);
                continue;
            }
            case lexical_token_t::BLOCK_LITERAL_SCALAR:
            case lexical_token_t::BLOCK_FOLDED_SCALAR: {
                tag_t tag_type {tag_t::NONE};
                if (m_needs_tag_impl) {
                    tag_type = tag_resolver_type::resolve_tag(m_tag_name, mp_meta);
                }

                basic_node_type node =
                    scalar_parser_type(line, indent)
                        .parse_block(token.type, tag_type, token.str, lexer.get_block_scalar_header());
                apply_directive_set(node);
                apply_node_properties(node);

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
                if FK_YAML_UNLIKELY (m_flow_context_depth > 0) {
                    throw parse_error("An invalid document marker found in a flow collection", line, indent);
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

        last_type = token.type;
    }

    /// @brief Deserializes YAML node properties (anchor and/or tag names) if they exist
    /// @param lexer The lexical analyzer to be used.
    /// @param last_type The variable to store the last lexical token type.
    /// @param line The variable to store the line of either the first property or the last non-property token.
    /// @param indent The variable to store the indent of either the first property or the last non-property token.
    /// @return true if any property is found, false otherwise.
    bool deserialize_node_properties(lexer_type& lexer, lexical_token& last_token, uint32_t& line, uint32_t& indent) {
        m_needs_anchor_impl = m_needs_tag_impl = false;

        lexical_token token = last_token;
        bool ends_loop {false};
        do {
            if (line < lexer.get_lines_processed()) {
                break;
            }

            switch (token.type) {
            case lexical_token_t::ANCHOR_PREFIX:
                if FK_YAML_UNLIKELY (m_needs_anchor_impl) {
                    throw parse_error(
                        "anchor name cannot be specified more than once to the same node.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }

                m_anchor_name = token.str;
                m_needs_anchor_impl = true;

                if (!m_needs_tag_impl) {
                    line = lexer.get_lines_processed();
                    indent = lexer.get_last_token_begin_pos();
                }

                token = lexer.get_next_token();
                break;
            case lexical_token_t::TAG_PREFIX: {
                if FK_YAML_UNLIKELY (m_needs_tag_impl) {
                    throw parse_error(
                        "tag name cannot be specified more than once to the same node.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }

                m_tag_name = token.str;
                m_needs_tag_impl = true;

                if (!m_needs_anchor_impl) {
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
        const bool prop_specified = m_needs_anchor_impl || m_needs_tag_impl;
        if (!prop_specified) {
            line = lexer.get_lines_processed();
            indent = lexer.get_last_token_begin_pos();
        }

        return prop_specified;
    }

    /// @brief Add new key string to the current YAML node.
    /// @param key a key string to be added to the current YAML node.
    /// @param line The line where the key is found.
    /// @param indent The indentation width in the current line where the key is found.
    void add_new_key(basic_node_type&& key, const uint32_t line, const uint32_t indent) {
        if (m_flow_context_depth == 0) {
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
            if FK_YAML_UNLIKELY (m_flow_token_state != flow_token_state_t::NEEDS_VALUE_OR_SUFFIX) {
                throw parse_error("Flow mapping entry is found without separated with a comma.", line, indent);
            }

            if (mp_current_node->is_sequence()) {
                mp_current_node->as_seq().emplace_back(basic_node_type::mapping());
                mp_current_node = &(mp_current_node->operator[](mp_current_node->size() - 1));
                m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
            }
        }

        auto itr = mp_current_node->as_map().emplace(std::move(key), basic_node_type());
        if FK_YAML_UNLIKELY (!itr.second) {
            throw parse_error("Detected duplication in mapping keys.", line, indent);
        }

        mp_current_node = &(itr.first->second);
        const parse_context& key_context = m_context_stack.back();
        m_context_stack.emplace_back(
            key_context.line, key_context.indent, context_state_t::MAPPING_VALUE, mp_current_node);
    }

    /// @brief Assign node value to the current node.
    /// @param node_value A rvalue basic_node_type object to be assigned to the current node.
    void assign_node_value(basic_node_type&& node_value, const uint32_t line, const uint32_t indent) {
        if (mp_current_node->is_sequence()) {
            FK_YAML_ASSERT(m_flow_context_depth > 0);

            if FK_YAML_UNLIKELY (m_flow_token_state != flow_token_state_t::NEEDS_VALUE_OR_SUFFIX) {
                // Flow sequence entries are not allowed to be empty.
                // ```yaml
                // [foo,,bar]
                // ```
                throw parse_error("flow sequence entry is found without separated with a comma.", line, indent);
            }

            mp_current_node->as_seq().emplace_back(std::move(node_value));
            m_flow_token_state = flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX;
            return;
        }

        // a scalar node
        *mp_current_node = std::move(node_value);
        if FK_YAML_UNLIKELY (m_context_stack.empty()) {
            // single scalar document.
            return;
        }

        if FK_YAML_LIKELY (m_context_stack.back().state != context_state_t::BLOCK_MAPPING_EXPLICIT_KEY) {
            m_context_stack.pop_back();
            mp_current_node = m_context_stack.back().p_node;

            if (m_flow_context_depth > 0) {
                m_flow_token_state = flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX;
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
        lexer_type& lexer, basic_node_type&& node, uint32_t& indent, uint32_t& line, lexical_token& token) {
        token = lexer.get_next_token();
        if (mp_current_node->is_mapping()) {
            const bool is_key_sep_followed =
                (token.type == lexical_token_t::KEY_SEPARATOR) && (line == lexer.get_lines_processed());
            if FK_YAML_UNLIKELY (!is_key_sep_followed) {
                throw parse_error(
                    "The \":\" mapping value indicator must be followed after a mapping key.",
                    lexer.get_lines_processed(),
                    lexer.get_last_token_begin_pos());
            }
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

                if (m_context_stack.back().state != context_state_t::BLOCK_MAPPING_EXPLICIT_KEY) {
                    pop_to_parent_node(line, indent, [indent](const parse_context& c) {
                        return c.state == context_state_t::BLOCK_MAPPING_EXPLICIT_KEY && indent == c.indent;
                    });
                }
                return;
            }

            if (mp_current_node->is_scalar()) {
                if FK_YAML_LIKELY (!m_context_stack.empty()) {
                    parse_context& cur_context = m_context_stack.back();
                    switch (cur_context.state) {
                    case context_state_t::BLOCK_MAPPING_EXPLICIT_KEY:
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
                            add_new_key(std::move(node), line, indent);
                            indent = lexer.get_last_token_begin_pos();
                            line = lexer.get_lines_processed();
                            return;
                        }

                        m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                        break;
                    default:
                        if FK_YAML_UNLIKELY (cur_context.line == line) {
                            throw parse_error("Multiple mapping keys are specified on the same line.", line, indent);
                        }
                        cur_context.line = line;
                        cur_context.indent = indent;
                        cur_context.state = context_state_t::BLOCK_MAPPING;
                        break;
                    }

                    *mp_current_node = basic_node_type::mapping();
                    apply_directive_set(*mp_current_node);
                }
                else {
                    // root mapping node

                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                    *mp_current_node = basic_node_type::mapping();
                    apply_directive_set(*mp_current_node);

                    // apply node properties if any to the root mapping node.
                    if (!m_root_anchor_name.empty()) {
                        mp_current_node->add_anchor_name(
                            std::string(m_root_anchor_name.begin(), m_root_anchor_name.end()));
                        m_root_anchor_name = {};
                    }
                    if (!m_root_tag_name.empty()) {
                        mp_current_node->add_tag_name(std::string(m_root_tag_name.begin(), m_root_tag_name.end()));
                        m_root_tag_name = {};
                    }
                }
            }
            add_new_key(std::move(node), line, indent);
        }
        else {
            assign_node_value(std::move(node), line, indent);
        }

        indent = lexer.get_last_token_begin_pos();
        line = lexer.get_lines_processed();
    }

    /// @brief Pops parent contexts to a block mapping with the given indentation.
    /// @tparam Pred Functor type to test parent contexts.
    /// @param line The current line count.
    /// @param indent The indentation level of the target parent block mapping.
    template <typename Pred>
    void pop_to_parent_node(uint32_t line, uint32_t indent, Pred&& pred) {
        FK_YAML_ASSERT(!m_context_stack.empty());

        // LCOV_EXCL_START
        auto itr = std::find_if(m_context_stack.rbegin(), m_context_stack.rend(), std::forward<Pred>(pred));
        // LCOV_EXCL_STOP
        const bool is_indent_valid = (itr != m_context_stack.rend());
        if FK_YAML_UNLIKELY (!is_indent_valid) {
            throw parse_error("Detected invalid indentation.", line, indent);
        }

        const auto pop_num = static_cast<uint32_t>(std::distance(m_context_stack.rbegin(), itr));

        // move back to the parent block mapping.
        for (uint32_t i = 0; i < pop_num; i++) {
            m_context_stack.pop_back();
        }
        mp_current_node = m_context_stack.back().p_node;
    }

    /// @brief Set YAML directive properties to the given node.
    /// @param node A basic_node_type object to be set YAML directive properties.
    void apply_directive_set(basic_node_type& node) noexcept {
        node.mp_meta = mp_meta;
    }

    /// @brief Set YAML node properties (anchor and/or tag names) to the given node.
    /// @param node A node type object to be set YAML node properties.
    void apply_node_properties(basic_node_type& node) {
        if (m_needs_anchor_impl) {
            node.add_anchor_name(std::string(m_anchor_name.begin(), m_anchor_name.end()));
            m_needs_anchor_impl = false;
            m_anchor_name = {};
        }

        if (m_needs_tag_impl) {
            node.add_tag_name(std::string(m_tag_name.begin(), m_tag_name.end()));
            m_needs_tag_impl = false;
            m_tag_name = {};
        }
    }

    /// @brief Update the target YAML version with an input string.
    /// @param version_str A YAML version string.
    yaml_version_type convert_yaml_version(str_view version_str) noexcept {
        return (version_str.compare("1.1") == 0) ? yaml_version_type::VERSION_1_1 : yaml_version_type::VERSION_1_2;
    }

private:
    /// The currently focused YAML node.
    basic_node_type* mp_current_node {nullptr};
    /// The stack of parse contexts.
    std::deque<parse_context> m_context_stack {};
    /// The current depth of flow contexts.
    uint32_t m_flow_context_depth {0};
    /// The set of YAML directives.
    std::shared_ptr<doc_metainfo_type> mp_meta {};
    /// A flag to determine the need for YAML anchor node implementation.
    bool m_needs_anchor_impl {false};
    /// A flag to determine the need for a corresponding node with the last YAML tag.
    bool m_needs_tag_impl {false};
    /// A flag to determine the need for a value separator or a flow suffix to follow.
    flow_token_state_t m_flow_token_state {flow_token_state_t::NEEDS_VALUE_OR_SUFFIX};
    /// The last YAML anchor name.
    str_view m_anchor_name;
    /// The last tag name.
    str_view m_tag_name;
    /// The root YAML anchor name. (maybe empty and unused)
    str_view m_root_anchor_name;
    /// The root tag name. (maybe empty and unused)
    str_view m_root_tag_name;
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_DESERIALIZER_HPP */
