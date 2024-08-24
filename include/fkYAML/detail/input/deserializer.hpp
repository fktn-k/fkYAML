///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_INPUT_DESERIALIZER_HPP_
#define FK_YAML_DETAIL_INPUT_DESERIALIZER_HPP_

#include <algorithm>
#include <cstdint>
#include <deque>
#include <unordered_map>
#include <vector>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/document_metainfo.hpp>
#include <fkYAML/detail/input/lexical_analyzer.hpp>
#include <fkYAML/detail/input/tag_resolver.hpp>
#include <fkYAML/detail/meta/input_adapter_traits.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
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
    using node_type = BasicNodeType;
    /** A type for the lexical analyzer. */
    using lexer_type = lexical_analyzer<node_type>;
    /** A type for the document metainfo. */
    using doc_metainfo_type = document_metainfo<node_type>;
    /** A type for the tag resolver. */
    using tag_resolver_type = tag_resolver<node_type>;
    /** A type for sequence node value containers. */
    using sequence_type = typename node_type::sequence_type;
    /** A type for mapping node value containers. */
    using mapping_type = typename node_type::mapping_type;
    /** A type for string node values. */
    using string_type = typename node_type::string_type;

    /// @brief Definition of state types of parse contexts.
    enum class context_state_t {
        BLOCK_MAPPING,                //!< The underlying node is a block mapping.
        BLOCK_MAPPING_EXPLICIT_KEY,   //!< The underlying node is an explicit block mapping key.
        BLOCK_MAPPING_EXPLICIT_VALUE, //!< The underlying node is an explicit block mapping value.
        MAPPING_VALUE,                //!< The underlying node is a block mapping value.
        BLOCK_SEQUENCE,               //!< The underlying node is a block sequence.
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
        /// @param _line The current line. (count from zero)
        /// @param _indent The indentation width in the current line. (count from zero)
        /// @param _state The parse context type.
        /// @param _p_node The underlying node associated to this context.
        parse_context(uint32_t _line, uint32_t _indent, context_state_t _state, node_type* _p_node)
            : line(_line),
              indent(_indent),
              state(_state),
              p_node(_p_node) {
        }

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
        node_type* p_node {nullptr};
    };

    /// @brief Definitions of state types for expected flow token hints.
    enum class flow_token_state_t {
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
    /// @return node_type A root YAML node deserialized from the source string.
    template <typename InputAdapterType, enable_if_t<is_input_adapter<InputAdapterType>::value, int> = 0>
    node_type deserialize(InputAdapterType&& input_adapter) {
        lexical_token_t type {lexical_token_t::END_OF_BUFFER};
        lexer_type lexer(std::forward<InputAdapterType>(input_adapter));
        return deserialize_document(lexer, type);
    }

    /// @brief Deserialize multiple YAML documents into YAML nodes.
    /// @tparam InputAdapterType The type of an adapter object.
    /// @param input_adapter An input adapter object for the input source buffer.
    /// @return std::vector<node_type> Root YAML nodes for deserialized YAML documents.
    template <typename InputAdapterType, enable_if_t<is_input_adapter<InputAdapterType>::value, int> = 0>
    std::vector<node_type> deserialize_docs(InputAdapterType&& input_adapter) {
        lexer_type lexer(std::forward<InputAdapterType>(input_adapter));
        std::vector<node_type> nodes {};
        lexical_token_t type {lexical_token_t::END_OF_BUFFER};

        do {
            nodes.emplace_back(deserialize_document(lexer, type));
        } while (type != lexical_token_t::END_OF_BUFFER);

        return nodes;
    }

private:
    /// @brief Deserialize a YAML document into a YAML node.
    /// @param lexer The lexical analyzer to be used.
    /// @param last_type The variable to store the last lexical token type.
    /// @return node_type A root YAML node deserialized from the YAML document.
    node_type deserialize_document(lexer_type& lexer, lexical_token_t& last_type) {
        lexical_token_t type {lexical_token_t::END_OF_BUFFER};

        node_type root;
        mp_meta = root.mp_meta;

        // parse directives first.
        deserialize_directives(lexer, type);

        // parse node properties for root node if any
        uint32_t line = lexer.get_lines_processed();
        uint32_t indent = lexer.get_last_token_begin_pos();
        bool found_props = deserialize_node_properties(lexer, type, line, indent);

        switch (type) {
        case lexical_token_t::SEQUENCE_BLOCK_PREFIX: {
            root = node_type::sequence();
            apply_directive_set(root);
            if (found_props) {
                // If node properties are found before the block sequence entry prefix, the properties belong to the
                // root sequence node.
                apply_node_properties(root);
            }
            parse_context context(
                lexer.get_lines_processed(), lexer.get_last_token_begin_pos(), context_state_t::BLOCK_SEQUENCE, &root);
            m_context_stack.emplace_back(std::move(context));
            type = lexer.get_next_token();
            break;
        }
        case lexical_token_t::SEQUENCE_FLOW_BEGIN:
            ++m_flow_context_depth;
            root = node_type::sequence();
            apply_directive_set(root);
            apply_node_properties(root);
            m_context_stack.emplace_back(
                lexer.get_lines_processed(), lexer.get_last_token_begin_pos(), context_state_t::FLOW_SEQUENCE, &root);
            type = lexer.get_next_token();
            break;
        case lexical_token_t::MAPPING_FLOW_BEGIN:
            ++m_flow_context_depth;
            root = node_type::mapping();
            apply_directive_set(root);
            apply_node_properties(root);
            m_context_stack.emplace_back(
                lexer.get_lines_processed(), lexer.get_last_token_begin_pos(), context_state_t::FLOW_MAPPING, &root);
            type = lexer.get_next_token();
            break;
        default: {
            root = node_type::mapping();
            apply_directive_set(root);
            if (found_props && line < lexer.get_lines_processed()) {
                // If node properties and a followed node are on the different line, the properties belong to the root
                // node.
                apply_node_properties(root);
            }
            parse_context context(
                lexer.get_lines_processed(), lexer.get_last_token_begin_pos(), context_state_t::BLOCK_MAPPING, &root);
            m_context_stack.emplace_back(std::move(context));
            break;
        }
        }

        mp_current_node = &root;

        // parse YAML nodes recursively
        deserialize_node(lexer, type, last_type);
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
    /// @param last_type The variable to store the last lexical token type.
    void deserialize_directives(lexer_type& lexer, lexical_token_t& last_type) {
        bool lacks_end_of_directives_marker = false;

        for (;;) {
            lexical_token_t type = lexer.get_next_token();

            switch (type) {
            case lexical_token_t::YAML_VER_DIRECTIVE:
                if (mp_meta->is_version_specified) {
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
                const std::string& tag_handle = lexer.get_tag_handle();
                switch (tag_handle.size()) {
                case 1: {
                    bool is_already_specified = !mp_meta->primary_handle_prefix.empty();
                    if (is_already_specified) {
                        throw parse_error(
                            "Primary handle cannot be specified more than once.",
                            lexer.get_lines_processed(),
                            lexer.get_last_token_begin_pos());
                    }
                    mp_meta->primary_handle_prefix = lexer.get_tag_prefix();
                    lacks_end_of_directives_marker = true;
                    break;
                }
                case 2: {
                    bool is_already_specified = !mp_meta->secondary_handle_prefix.empty();
                    if (is_already_specified) {
                        throw parse_error(
                            "Secondary handle cannot be specified more than once.",
                            lexer.get_lines_processed(),
                            lexer.get_last_token_begin_pos());
                    }
                    mp_meta->secondary_handle_prefix = lexer.get_tag_prefix();
                    lacks_end_of_directives_marker = true;
                    break;
                }
                default: {
                    bool is_already_specified =
                        !(mp_meta->named_handle_map.emplace(tag_handle, lexer.get_tag_prefix()).second);
                    if (is_already_specified) {
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
                if (lacks_end_of_directives_marker) {
                    throw parse_error(
                        "The end of directives marker (---) is missing after directives.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }
                // end the parsing of directives if the other tokens are found.
                last_type = type;
                return;
            }
        }
    }

    /// @brief Deserializes the YAML nodes recursively.
    /// @param lexer The lexical analyzer to be used.
    /// @param first_type The first lexical token type.
    void deserialize_node(lexer_type& lexer, lexical_token_t first_type, lexical_token_t& last_type) {
        lexical_token_t type = first_type;
        uint32_t line = lexer.get_lines_processed();
        uint32_t indent = lexer.get_last_token_begin_pos();

        do {
            switch (type) {
            case lexical_token_t::EXPLICIT_KEY_PREFIX: {
                uint32_t pop_num = 0;
                if (indent == 0) {
                    pop_num = static_cast<uint32_t>(m_context_stack.size() - 1);
                }
                else {
                    bool needs_to_move_back = indent < m_context_stack.back().indent;
                    if (needs_to_move_back) {
                        auto target_itr = std::find_if( // LCOV_EXCL_LINE
                            m_context_stack.rbegin(),
                            m_context_stack.rend(),
                            [indent](const parse_context& c) { return indent > c.indent; });
                        pop_num = static_cast<uint32_t>(std::distance(m_context_stack.rbegin(), target_itr));
                    }
                }
                if (pop_num > 0) {
                    for (uint32_t i = 0; i < pop_num; i++) {
                        // move back to the previous container node.
                        m_context_stack.pop_back();
                    }
                    mp_current_node = m_context_stack.back().p_node;
                }

                if (mp_current_node->is_null()) {
                    // This path is needed in case the input contains nested explicit keys like the following YAML
                    // snippet:
                    // ```yaml
                    // ? ? foo
                    //   : bar
                    // : baz
                    // ```
                    *mp_current_node = node_type::mapping();
                    apply_directive_set(*mp_current_node);
                }

                if (m_context_stack.back().state == context_state_t::BLOCK_SEQUENCE) {
                    sequence_type& seq = mp_current_node->template get_value_ref<sequence_type&>();
                    seq.emplace_back(node_type::mapping());
                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, &(seq.back()));
                }

                type = lexer.get_next_token();
                if (type == lexical_token_t::SEQUENCE_BLOCK_PREFIX) {
                    // heap-allocated node will be freed in handling the corresponding KEY_SEPARATOR event
                    m_context_stack.emplace_back(
                        line, indent, context_state_t::BLOCK_MAPPING_EXPLICIT_KEY, new node_type(node_t::SEQUENCE));
                    mp_current_node = m_context_stack.back().p_node;
                    apply_directive_set(*mp_current_node);
                    parse_context context(
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos(),
                        context_state_t::BLOCK_SEQUENCE,
                        mp_current_node);
                    m_context_stack.emplace_back(std::move(context));
                    break;
                }

                // heap-allocated node will be freed in handling the corresponding KEY_SEPARATOR event
                m_context_stack.emplace_back(
                    line, indent, context_state_t::BLOCK_MAPPING_EXPLICIT_KEY, new node_type());
                mp_current_node = m_context_stack.back().p_node;
                apply_directive_set(*mp_current_node);
                indent = lexer.get_last_token_begin_pos();
                line = lexer.get_lines_processed();

                continue;
            }
            case lexical_token_t::KEY_SEPARATOR: {
                bool is_empty_seq = mp_current_node->is_sequence() && mp_current_node->empty();
                if (is_empty_seq) {
                    throw parse_error("sequence key should not be empty.", line, indent);
                }

                // hold the line count of the key separator for later use.
                uint32_t old_indent = indent;
                uint32_t old_line = line;

                type = lexer.get_next_token();
                line = lexer.get_lines_processed();
                indent = lexer.get_last_token_begin_pos();

                bool found_props = deserialize_node_properties(lexer, type, line, indent);
                if (found_props && line == lexer.get_lines_processed()) {
                    // defer applying node properties for the subsequent node on the same line.
                    continue;
                }

                line = lexer.get_lines_processed();
                indent = lexer.get_last_token_begin_pos();

                if (m_flow_context_depth > 0) {
                    continue;
                }

                bool is_implicit_same_line =
                    (line == old_line) && (m_context_stack.empty() || old_indent > m_context_stack.back().indent);
                if (is_implicit_same_line) {
                    // a key separator for an implicit key with its value on the same line.
                    continue;
                }

                if (line > old_line) {
                    if (m_needs_tag_impl) {
                        tag_t tag_type = tag_resolver_type::resolve_tag(m_tag_name, mp_meta);
                        if (tag_type == tag_t::MAPPING || tag_type == tag_t::CUSTOM_TAG) {
                            // set YAML node properties here to distinguish them from those for the first key node
                            // as shown in the following snippet:
                            //
                            // ```yaml
                            // foo: !!map
                            //   !!str 123: true
                            //   ^
                            //   this !!str tag overwrites the preceeding !!map tag.
                            // ```
                            *mp_current_node = node_type::mapping();
                            apply_directive_set(*mp_current_node);
                            apply_node_properties(*mp_current_node);
                            m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                            continue;
                        }
                    }

                    if (type == lexical_token_t::SEQUENCE_BLOCK_PREFIX) {
                        // a key separator preceeding block sequence entries
                        *mp_current_node = node_type::sequence();
                        apply_directive_set(*mp_current_node);
                        apply_node_properties(*mp_current_node);
                        auto& cur_context = m_context_stack.back();
                        cur_context.line = line;
                        cur_context.indent = indent;
                        cur_context.state = context_state_t::BLOCK_SEQUENCE;

                        type = lexer.get_next_token();
                        line = lexer.get_lines_processed();
                        indent = lexer.get_last_token_begin_pos();

                        bool has_props = deserialize_node_properties(lexer, type, line, indent);
                        if (has_props) {
                            uint32_t line_after_props = lexer.get_lines_processed();
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
                            mp_current_node->template get_value_ref<sequence_type&>().emplace_back(
                                node_type::mapping());
                            mp_current_node = &mp_current_node->template get_value_ref<sequence_type&>().back();
                            m_context_stack.emplace_back(
                                line_after_props, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                            apply_node_properties(*mp_current_node);
                        }

                        continue;
                    }

                    // defer checking the existence of a key separator after the following scalar until the next
                    // deserialize_scalar() call.
                    continue;
                }

                // handle explicit mapping key separators.

                while (m_context_stack.back().state != context_state_t::BLOCK_MAPPING_EXPLICIT_KEY) {
                    m_context_stack.pop_back();
                }

                node_type key_node = std::move(*m_context_stack.back().p_node);
                m_context_stack.pop_back();
                m_context_stack.back().p_node->template get_value_ref<mapping_type&>().emplace(key_node, node_type());
                mp_current_node = &(m_context_stack.back().p_node->operator[](std::move(key_node)));
                m_context_stack.emplace_back(
                    line, indent, context_state_t::BLOCK_MAPPING_EXPLICIT_VALUE, mp_current_node);

                if (type == lexical_token_t::SEQUENCE_BLOCK_PREFIX) {
                    *mp_current_node = node_type::sequence();
                    apply_directive_set(*mp_current_node);
                    apply_node_properties(*mp_current_node);
                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_SEQUENCE, mp_current_node);
                    break;
                }

                continue;
            }
            // just ignore directives
            case lexical_token_t::YAML_VER_DIRECTIVE:
            case lexical_token_t::TAG_DIRECTIVE:
            case lexical_token_t::INVALID_DIRECTIVE:
                break;
            case lexical_token_t::ANCHOR_PREFIX:
            case lexical_token_t::TAG_PREFIX:
                deserialize_node_properties(lexer, type, line, indent);
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
                bool is_further_nested = m_context_stack.back().indent < indent;
                if (is_further_nested) {
                    mp_current_node->template get_value_ref<sequence_type&>().emplace_back(node_type::sequence());
                    mp_current_node = &(mp_current_node->template get_value_ref<sequence_type&>().back());
                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_SEQUENCE, mp_current_node);
                    apply_directive_set(*mp_current_node);
                    apply_node_properties(*mp_current_node);
                    break;
                }

                // move back to the previous sequence if necessary.
                while (m_context_stack.back().state != context_state_t::BLOCK_SEQUENCE ||
                       indent != m_context_stack.back().indent) {
                    m_context_stack.pop_back();
                }
                mp_current_node = m_context_stack.back().p_node;
                break;
            }
            case lexical_token_t::SEQUENCE_FLOW_BEGIN:
                if (m_flow_context_depth == 0) {
                    uint32_t pop_num = 0;
                    if (indent == 0) {
                        pop_num = static_cast<uint32_t>(m_context_stack.size() - 1);
                    }
                    else if (indent <= m_context_stack.back().indent) {
                        auto target_itr = std::find_if( // LCOV_EXCL_LINE
                            m_context_stack.rbegin(),
                            m_context_stack.rend(),
                            [indent](const parse_context& c) {
                                if (indent != c.indent) {
                                    return false;
                                }

                                switch (c.state) {
                                case context_state_t::BLOCK_MAPPING:
                                case context_state_t::MAPPING_VALUE:
                                    return true;
                                default:
                                    return false;
                                }
                            });
                        bool is_indent_valid = (target_itr != m_context_stack.rend());
                        if (!is_indent_valid) {
                            throw parse_error("Detected invalid indentaion.", line, indent);
                        }

                        pop_num = static_cast<uint32_t>(std::distance(m_context_stack.rbegin(), target_itr));
                    }
                    if (pop_num > 0) {
                        for (uint32_t i = 0; i < pop_num; i++) {
                            // move back to the previous container node.
                            m_context_stack.pop_back();
                        }
                        mp_current_node = m_context_stack.back().p_node;
                    }
                }
                else if (m_flow_token_state == flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX) {
                    throw parse_error("Flow sequence begininng is found without separated with a comma.", line, indent);
                }

                ++m_flow_context_depth;

                switch (m_context_stack.back().state) {
                case context_state_t::BLOCK_SEQUENCE:
                case context_state_t::FLOW_SEQUENCE:
                    mp_current_node->template get_value_ref<sequence_type&>().emplace_back(node_type::sequence());
                    mp_current_node = &(mp_current_node->template get_value_ref<sequence_type&>().back());
                    m_context_stack.emplace_back(line, indent, context_state_t::FLOW_SEQUENCE, mp_current_node);
                    break;
                case context_state_t::BLOCK_MAPPING:
                case context_state_t::FLOW_MAPPING:
                    // heap-allocated node will be freed in handling the corresponding SEQUENCE_FLOW_END event.
                    m_context_stack.emplace_back(
                        line, indent, context_state_t::FLOW_SEQUENCE_KEY, new node_type(node_t::SEQUENCE));
                    mp_current_node = m_context_stack.back().p_node;
                    break;
                default: {
                    *mp_current_node = node_type::sequence();
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
                if (m_flow_context_depth == 0) {
                    throw parse_error("Flow sequence ending is found outside the flow context.", line, indent);
                }
                --m_flow_context_depth;

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

                bool is_valid = itr != m_context_stack.rend();
                if (!is_valid) {
                    throw parse_error("No corresponding flow sequence beginning is found.", line, indent);
                }

                // keep the last state for later processing.
                parse_context& last_context = m_context_stack.back();
                mp_current_node = last_context.p_node;
                last_context.p_node = nullptr;
                indent = last_context.indent;
                context_state_t state = last_context.state;
                m_context_stack.pop_back();

                // handle cases where the flow sequence is a mapping key node.

                if (!m_context_stack.empty() && state == context_state_t::FLOW_SEQUENCE_KEY) {
                    node_type key_node = std::move(*mp_current_node);
                    delete mp_current_node;
                    mp_current_node = m_context_stack.back().p_node;
                    m_flow_token_state = flow_token_state_t::NEEDS_VALUE_OR_SUFFIX;

                    add_new_key(std::move(key_node), line, indent);
                    break;
                }

                type = lexer.get_next_token();
                if (type == lexical_token_t::KEY_SEPARATOR) {
                    node_type key_node = node_type::mapping();
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
                    uint32_t pop_num = 0;
                    if (indent == 0) {
                        pop_num = static_cast<uint32_t>(m_context_stack.size() - 1);
                    }
                    else if (indent <= m_context_stack.back().indent) {
                        auto target_itr = std::find_if( // LCOV_EXCL_LINE
                            m_context_stack.rbegin(),
                            m_context_stack.rend(),
                            [indent](const parse_context& c) {
                                if (indent != c.indent) {
                                    return false;
                                }

                                switch (c.state) {
                                case context_state_t::BLOCK_MAPPING:
                                case context_state_t::MAPPING_VALUE:
                                    return true;
                                default:
                                    return false;
                                }
                            });
                        bool is_indent_valid = (target_itr != m_context_stack.rend());
                        if (!is_indent_valid) {
                            throw parse_error("Detected invalid indentaion.", line, indent);
                        }

                        pop_num = static_cast<uint32_t>(std::distance(m_context_stack.rbegin(), target_itr));
                    }
                    if (pop_num > 0) {
                        for (uint32_t i = 0; i < pop_num; i++) {
                            // move back to the previous container node.
                            m_context_stack.pop_back();
                        }
                        mp_current_node = m_context_stack.back().p_node;
                    }
                }
                else if (m_flow_token_state == flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX) {
                    throw parse_error("Flow mapping begininng is found without separated with a comma.", line, indent);
                }

                ++m_flow_context_depth;

                switch (m_context_stack.back().state) {
                case context_state_t::BLOCK_SEQUENCE:
                case context_state_t::FLOW_SEQUENCE:
                    mp_current_node->template get_value_ref<sequence_type&>().emplace_back(node_type::mapping());
                    mp_current_node = &(mp_current_node->template get_value_ref<sequence_type&>().back());
                    m_context_stack.emplace_back(line, indent, context_state_t::FLOW_MAPPING, mp_current_node);
                    break;
                case context_state_t::BLOCK_MAPPING:
                case context_state_t::FLOW_MAPPING:
                    // heap-allocated node will be freed in handling the corresponding MAPPING_FLOW_END event.
                    m_context_stack.emplace_back(
                        line, indent, context_state_t::FLOW_MAPPING_KEY, new node_type(node_t::MAPPING));
                    mp_current_node = m_context_stack.back().p_node;
                    break;
                default: {
                    *mp_current_node = node_type::mapping();
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
                if (m_flow_context_depth == 0) {
                    throw parse_error("Flow mapping ending is found outside the flow context.", line, indent);
                }
                --m_flow_context_depth;

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

                bool is_valid = itr != m_context_stack.rend();
                if (!is_valid) {
                    throw parse_error("No corresponding flow mapping beginning is found.", line, indent);
                }

                // keep the last state for later processing.
                parse_context& last_context = m_context_stack.back();
                mp_current_node = last_context.p_node;
                last_context.p_node = nullptr;
                indent = last_context.indent;
                context_state_t state = last_context.state;
                m_context_stack.pop_back();

                // handle cases where the flow mapping is a mapping key node.

                if (!m_context_stack.empty() && state == context_state_t::FLOW_MAPPING_KEY) {
                    node_type key_node = std::move(*mp_current_node);
                    delete mp_current_node;
                    mp_current_node = m_context_stack.back().p_node;
                    m_flow_token_state = flow_token_state_t::NEEDS_VALUE_OR_SUFFIX;

                    add_new_key(std::move(key_node), line, indent);
                    break;
                }

                type = lexer.get_next_token();
                if (type == lexical_token_t::KEY_SEPARATOR) {
                    node_type key_node = node_type::mapping();
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
                if (m_flow_token_state != flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX) {
                    throw parse_error("invalid value separator is found.", line, indent);
                }
                m_flow_token_state = flow_token_state_t::NEEDS_VALUE_OR_SUFFIX;
                break;
            case lexical_token_t::ALIAS_PREFIX:
            case lexical_token_t::NULL_VALUE:
            case lexical_token_t::BOOLEAN_VALUE:
            case lexical_token_t::INTEGER_VALUE:
            case lexical_token_t::FLOAT_NUMBER_VALUE:
            case lexical_token_t::STRING_VALUE: {
                bool do_continue = deserialize_scalar(lexer, indent, line, type);
                if (do_continue) {
                    continue;
                }
                break;
            }
            // these tokens end parsing the current YAML document.
            case lexical_token_t::END_OF_BUFFER: // This handles an empty input.
            case lexical_token_t::END_OF_DIRECTIVES:
            case lexical_token_t::END_OF_DOCUMENT:
                last_type = type;
                return;
            }

            type = lexer.get_next_token();
            indent = lexer.get_last_token_begin_pos();
            line = lexer.get_lines_processed();
        } while (type != lexical_token_t::END_OF_BUFFER);

        last_type = type;
    }

    /// @brief Deserializes YAML node properties (anchor and/or tag names) if they exist
    /// @param lexer The lexical analyzer to be used.
    /// @param last_type The variable to store the last lexical token type.
    /// @param line The variable to store the line of either the first property or the last non-property token.
    /// @param indent The variable to store the indent of either the first property or the last non-property token.
    /// @return true if any property is found, false otherwise.
    bool deserialize_node_properties(lexer_type& lexer, lexical_token_t& last_type, uint32_t& line, uint32_t& indent) {
        m_needs_anchor_impl = m_needs_tag_impl = false;

        lexical_token_t type = last_type;
        bool ends_loop {false};
        do {
            if (line < lexer.get_lines_processed()) {
                break;
            }

            switch (type) {
            case lexical_token_t::ANCHOR_PREFIX:
                if (m_needs_anchor_impl) {
                    throw parse_error(
                        "anchor name cannot be specified more than once to the same node.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }

                m_anchor_name = lexer.get_string();
                m_needs_anchor_impl = true;

                if (!m_needs_tag_impl) {
                    line = lexer.get_lines_processed();
                    indent = lexer.get_last_token_begin_pos();
                }

                type = lexer.get_next_token();
                break;
            case lexical_token_t::TAG_PREFIX: {
                if (m_needs_tag_impl) {
                    throw parse_error(
                        "tag name cannot be specified more than once to the same node.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }

                m_tag_name = lexer.get_string();
                m_needs_tag_impl = true;

                if (!m_needs_anchor_impl) {
                    line = lexer.get_lines_processed();
                    indent = lexer.get_last_token_begin_pos();
                }

                type = lexer.get_next_token();
                break;
            }
            default:
                ends_loop = true;
                break;
            }
        } while (!ends_loop);

        last_type = type;
        bool prop_specified = m_needs_anchor_impl || m_needs_tag_impl;
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
    void add_new_key(node_type&& key, const uint32_t line, const uint32_t indent) {
        if (m_flow_context_depth == 0) {
            uint32_t pop_num = 0;
            if (indent == 0) {
                pop_num = static_cast<uint32_t>(m_context_stack.size() - 1);
            }
            else if (indent <= m_context_stack.back().indent) {
                auto target_itr =
                    std::find_if(m_context_stack.rbegin(), m_context_stack.rend(), [indent](const parse_context& c) {
                        // the target node is a block mapping key node with the same indentation.
                        return (indent == c.indent) && (c.state == context_state_t::BLOCK_MAPPING);
                    });
                bool is_indent_valid = (target_itr != m_context_stack.rend());
                if (!is_indent_valid) {
                    throw parse_error("Detected invalid indentaion.", line, indent);
                }

                pop_num = static_cast<uint32_t>(std::distance(m_context_stack.rbegin(), target_itr));
            }
            if (pop_num > 0) {
                for (uint32_t i = 0; i < pop_num; i++) {
                    // move back to the previous container node.
                    m_context_stack.pop_back();
                }
                mp_current_node = m_context_stack.back().p_node;
            }
        }
        else if (m_flow_token_state != flow_token_state_t::NEEDS_VALUE_OR_SUFFIX) {
            throw parse_error("Flow mapping entry is found without separated with a comma.", line, indent);
        }

        if (mp_current_node->is_sequence()) {
            mp_current_node->template get_value_ref<sequence_type&>().emplace_back(node_type::mapping());
            mp_current_node = &(mp_current_node->operator[](mp_current_node->size() - 1));
            m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
        }

        auto itr = mp_current_node->template get_value_ref<mapping_type&>().emplace(std::move(key), node_type());
        if (!itr.second) {
            throw parse_error("Detected duplication in mapping keys.", line, indent);
        }

        mp_current_node = &(itr.first->second);
        parse_context& key_context = m_context_stack.back();
        m_context_stack.emplace_back(
            key_context.line, key_context.indent, context_state_t::MAPPING_VALUE, mp_current_node);
    }

    /// @brief Assign node value to the current node.
    /// @param node_value A rvalue node_type object to be assigned to the current node.
    void assign_node_value(node_type&& node_value, const uint32_t line, const uint32_t indent) {
        if (mp_current_node->is_sequence()) {
            if (m_flow_context_depth > 0) {
                if (m_flow_token_state != flow_token_state_t::NEEDS_VALUE_OR_SUFFIX) {
                    throw parse_error("flow sequence entry is found without separated with a comma.", line, indent);
                }
                m_flow_token_state = flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX;
            }

            mp_current_node->template get_value_ref<sequence_type&>().emplace_back(std::move(node_value));
            return;
        }

        // a scalar node
        *mp_current_node = std::move(node_value);
        if (m_flow_context_depth > 0 || m_context_stack.back().state != context_state_t::BLOCK_MAPPING_EXPLICIT_KEY) {
            m_context_stack.pop_back();
            mp_current_node = m_context_stack.back().p_node;

            if (m_flow_context_depth > 0) {
                m_flow_token_state = flow_token_state_t::NEEDS_SEPARATOR_OR_SUFFIX;
            }
        }
    }

    /// @brief Creates a YAML scalar node with the retrieved token information by the lexer.
    /// @param lexer The lexical analyzer to be used.
    /// @param type The type of the last lexical token.
    /// @param indent The last indent size.
    /// @param line The last line.
    /// @return The created YAML scalar node.
    node_type create_scalar_node(lexer_type& lexer, lexical_token_t type, uint32_t indent, uint32_t line) {
        FK_YAML_ASSERT(
            type == lexical_token_t::NULL_VALUE || type == lexical_token_t::BOOLEAN_VALUE ||
            type == lexical_token_t::INTEGER_VALUE || type == lexical_token_t::FLOAT_NUMBER_VALUE ||
            type == lexical_token_t::STRING_VALUE || type == lexical_token_t::ALIAS_PREFIX);

        if (m_needs_tag_impl) {
            if (type == lexical_token_t::ALIAS_PREFIX) {
                throw parse_error("Tag cannot be specified to alias nodes", line, indent);
            }

            tag_t tag_type = tag_resolver_type::resolve_tag(m_tag_name, mp_meta);

            FK_YAML_ASSERT(tag_type != tag_t::SEQUENCE && tag_type != tag_t::MAPPING);

            switch (tag_type) {
            case tag_t::NULL_VALUE:
                type = lexical_token_t::NULL_VALUE;
                break;
            case tag_t::BOOLEAN:
                type = lexical_token_t::BOOLEAN_VALUE;
                break;
            case tag_t::INTEGER:
                type = lexical_token_t::INTEGER_VALUE;
                break;
            case tag_t::FLOATING_NUMBER:
                type = lexical_token_t::FLOAT_NUMBER_VALUE;
                break;
            case tag_t::STRING:
                type = lexical_token_t::STRING_VALUE;
                break;
            case tag_t::NON_SPECIFIC:
                // scalars with the non-specific tag is resolved to a string tag.
                // See the "Non-Specific Tags" section in https://yaml.org/spec/1.2.2/#691-node-tags.
                type = lexical_token_t::STRING_VALUE;
                break;
            case tag_t::CUSTOM_TAG:
            default:
                break;
            }
        }

        node_type node {};
        switch (type) {
        case lexical_token_t::NULL_VALUE:
            node = node_type(lexer.get_null());
            break;
        case lexical_token_t::BOOLEAN_VALUE:
            node = node_type(lexer.get_boolean());
            break;
        case lexical_token_t::INTEGER_VALUE:
            node = node_type(lexer.get_integer());
            break;
        case lexical_token_t::FLOAT_NUMBER_VALUE:
            node = node_type(lexer.get_float_number());
            break;
        case lexical_token_t::STRING_VALUE:
            node = node_type(lexer.get_string());
            break;
        case lexical_token_t::ALIAS_PREFIX: {
            const string_type& alias_name = lexer.get_string();
            uint32_t anchor_counts = static_cast<uint32_t>(mp_meta->anchor_table.count(alias_name));
            if (anchor_counts == 0) {
                throw parse_error("The given anchor name must appear prior to the alias node.", line, indent);
            }
            node.m_prop.anchor_status = detail::anchor_status_t::ALIAS;
            node.m_prop.anchor = alias_name;
            node.m_prop.anchor_offset = anchor_counts - 1;
            break;
        }
        default:   // LCOV_EXCL_LINE
            break; // LCOV_EXCL_LINE
        }

        apply_directive_set(node);
        apply_node_properties(node);

        return node;
    }

    /// @brief Deserialize a detected scalar node.
    /// @param lexer The lexical analyzer to be used.
    /// @param node A detected scalar node by a lexer.
    /// @param indent The current indentation width. Can be updated in this function.
    /// @param line The number of processed lines. Can be updated in this function.
    /// @return true if next token has already been got, false otherwise.
    bool deserialize_scalar(lexer_type& lexer, uint32_t& indent, uint32_t& line, lexical_token_t& type) {
        node_type node = create_scalar_node(lexer, type, indent, line);

        if (mp_current_node->is_mapping()) {
            add_new_key(std::move(node), line, indent);
            return false;
        }

        type = lexer.get_next_token();
        if (type == lexical_token_t::KEY_SEPARATOR) {
            if (line != lexer.get_lines_processed()) {
                // This path is for explicit mapping key separator like:
                //
                // ```yaml
                //   ? foo
                //   : bar
                // # ^ this separator
                // ```
                assign_node_value(std::move(node), line, indent);
                if (m_context_stack.back().state != context_state_t::BLOCK_MAPPING_EXPLICIT_KEY) {
                    mp_current_node = m_context_stack.back().p_node;
                    m_context_stack.pop_back();
                }
                indent = lexer.get_last_token_begin_pos();
                line = lexer.get_lines_processed();
                return true;
            }

            if (mp_current_node->is_scalar()) {
                parse_context& cur_context = m_context_stack.back();
                switch (cur_context.state) {
                case context_state_t::BLOCK_MAPPING_EXPLICIT_KEY:
                case context_state_t::BLOCK_MAPPING_EXPLICIT_VALUE:
                    m_context_stack.emplace_back(line, indent, context_state_t::BLOCK_MAPPING, mp_current_node);
                    break;
                default:
                    if (cur_context.line == line) {
                        throw parse_error("Multiple mapping keys are specified on the same line.", line, indent);
                    }
                    cur_context.line = line;
                    cur_context.indent = indent;
                    cur_context.state = context_state_t::BLOCK_MAPPING;
                    break;
                }

                *mp_current_node = node_type::mapping();
                apply_directive_set(*mp_current_node);
            }
            add_new_key(std::move(node), line, indent);
        }
        else {
            assign_node_value(std::move(node), line, indent);
        }
        indent = lexer.get_last_token_begin_pos();
        line = lexer.get_lines_processed();
        return true;
    }

    /// @brief Set YAML directive properties to the given node.
    /// @param node A node_type object to be set YAML directive properties.
    void apply_directive_set(node_type& node) noexcept {
        node.mp_meta = mp_meta;
    }

    /// @brief Set YAML node properties (anchor and/or tag names) to the given node.
    /// @param node A node type object to be set YAML node properties.
    void apply_node_properties(node_type& node) {
        if (m_needs_anchor_impl) {
            node.add_anchor_name(m_anchor_name);
            m_needs_anchor_impl = false;
            m_anchor_name.clear();
        }

        if (m_needs_tag_impl) {
            node.add_tag_name(m_tag_name);
            m_needs_tag_impl = false;
            m_tag_name.clear();
        }
    }

    /// @brief Update the target YAML version with an input string.
    /// @param version_str A YAML version string.
    yaml_version_t convert_yaml_version(const string_type& version_str) noexcept {
        return (version_str == "1.1") ? yaml_version_t::VER_1_1 : yaml_version_t::VER_1_2;
    }

private:
    /// The currently focused YAML node.
    node_type* mp_current_node {nullptr};
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
    string_type m_anchor_name {};
    /// The last tag name.
    string_type m_tag_name {};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_DESERIALIZER_HPP_ */
