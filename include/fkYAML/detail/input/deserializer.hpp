///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.4
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
#include <set>
#include <unordered_map>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/directive_set.hpp>
#include <fkYAML/detail/input/lexical_analyzer.hpp>
#include <fkYAML/detail/input/tag_resolver.hpp>
#include <fkYAML/detail/meta/input_adapter_traits.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/meta/stl_supplement.hpp>
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
    /** A type for sequence node value containers. */
    using sequence_type = typename node_type::sequence_type;
    /** A type for mapping node value containers. */
    using mapping_type = typename node_type::mapping_type;
    /** A type for boolean node values. */
    using boolean_type = typename node_type::boolean_type;
    /** A type for integer node values. */
    using integer_type = typename node_type::integer_type;
    /** A type for float number node values. */
    using float_number_type = typename node_type::float_number_type;
    /** A type for string node values. */
    using string_type = typename node_type::string_type;

    struct indentation {
        indentation() = default;

        indentation(std::size_t _line, std::size_t _indent, bool _is_explicit_key)
            : line(_line),
              indent(_indent),
              is_explicit_key(_is_explicit_key) {
        }

        std::size_t line {0};
        std::size_t indent {0};
        bool is_explicit_key {false};
    };

public:
    /// @brief Construct a new basic_deserializer object.
    basic_deserializer() = default;

public:
    /// @brief Deserialize a YAML-formatted source string into a YAML node.
    /// @param source A YAML-formatted source string.
    /// @return node_type A root YAML node deserialized from the source string.
    template <typename InputAdapterType, enable_if_t<is_input_adapter<InputAdapterType>::value, int> = 0>
    node_type deserialize(InputAdapterType&& input_adapter) {
        lexer_type lexer(std::forward<InputAdapterType>(input_adapter));
        lexical_token_t type {lexical_token_t::END_OF_BUFFER};

        node_type root = node_type::mapping();
        mp_current_node = &root;

        // parse directives first.
        deserialize_directives(lexer, root, type);

        switch (type) {
        case lexical_token_t::SEQUENCE_BLOCK_PREFIX:
        case lexical_token_t::SEQUENCE_FLOW_BEGIN:
            root = node_type::sequence();
            apply_directive_set(root);
            break;
        default:
            break;
        }

        // parse YAML nodes recursively
        deserialize_node(lexer, type);

        // reset parameters for the next call.
        mp_current_node = nullptr;
        mp_directive_set.reset();
        m_needs_anchor_impl = false;
        m_anchor_table.clear();
        m_node_stack.clear();
        m_indent_stack.clear();

        return root;
    }

private:
    /// @brief Deserializes the YAML directives if specified.
    /// @param lexer The lexical analyzer to be used.
    /// @param root The root YAML node.
    /// @param type The variable to store the last lexical token type.
    void deserialize_directives(lexer_type& lexer, node_type& root, lexical_token_t& last_type) {
        for (;;) {
            lexical_token_t type = lexer.get_next_token();

            switch (type) {
            case lexical_token_t::YAML_VER_DIRECTIVE:
                if (!mp_directive_set) {
                    mp_directive_set = std::shared_ptr<directive_set>(new directive_set());
                }
                if (!root.mp_directive_set) {
                    root.mp_directive_set = mp_directive_set;
                }

                if (mp_directive_set->is_version_specified) {
                    throw parse_error(
                        "YAML version cannot be specified more than once.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }

                mp_directive_set->version = convert_yaml_version(lexer.get_yaml_version());
                mp_directive_set->is_version_specified = true;
                break;
            case lexical_token_t::TAG_DIRECTIVE: {
                if (!mp_directive_set) {
                    mp_directive_set = std::shared_ptr<directive_set>(new directive_set());
                }
                if (!root.mp_directive_set) {
                    root.mp_directive_set = mp_directive_set;
                }

                const std::string& tag_handle = lexer.get_tag_handle();
                switch (tag_handle.size()) {
                case 1: {
                    bool is_already_specified = !mp_directive_set->primary_handle_prefix.empty();
                    if (is_already_specified) {
                        throw parse_error(
                            "Primary handle cannot be specified more than once.",
                            lexer.get_lines_processed(),
                            lexer.get_last_token_begin_pos());
                    }
                    mp_directive_set->primary_handle_prefix = lexer.get_tag_prefix();
                    break;
                }
                case 2: {
                    bool is_already_specified = !mp_directive_set->secondary_handle_prefix.empty();
                    if (is_already_specified) {
                        throw parse_error(
                            "Secondary handle cannot be specified more than once.",
                            lexer.get_lines_processed(),
                            lexer.get_last_token_begin_pos());
                    }
                    mp_directive_set->secondary_handle_prefix = lexer.get_tag_prefix();
                    break;
                }
                default: {
                    bool is_already_specified =
                        !(mp_directive_set->named_handle_map.emplace(tag_handle, lexer.get_tag_prefix()).second);
                    if (is_already_specified) {
                        throw parse_error(
                            "The same named handle cannot be specified more than once.",
                            lexer.get_lines_processed(),
                            lexer.get_last_token_begin_pos());
                    }
                    break;
                }
                }
                break;
            }
            case lexical_token_t::INVALID_DIRECTIVE:
                // TODO: should output a warning log. Currently just ignore this case.
                break;
            case lexical_token_t::END_OF_DIRECTIVES:
                last_type = type;
                return;
            default:
                // end the parsing of directives if the other tokens are found.
                last_type = type;
                return;
            }
        }
    }

    /// @brief Deserializes the YAML nodes recursively.
    /// @param lexer The lexical analyzer to be used.
    /// @param first_type The first lexical token type.
    void deserialize_node(lexer_type& lexer, lexical_token_t first_type) {
        lexical_token_t type = first_type;
        std::size_t line = lexer.get_lines_processed();
        std::size_t indent = lexer.get_last_token_begin_pos();

        do {
            switch (type) {
            case lexical_token_t::END_OF_BUFFER:
                // This handles an empty input.
                break;
            case lexical_token_t::EXPLICIT_KEY_PREFIX: {
                bool needs_to_move_back = !m_indent_stack.empty() && indent < m_indent_stack.back().indent;
                if (needs_to_move_back) {
                    auto target_itr = std::find_if( // LCOV_EXCL_LINE
                        m_indent_stack.rbegin(),
                        m_indent_stack.rend(),
                        [indent](const indentation& i) { return indent > i.indent; });

                    auto pop_num = std::distance(m_indent_stack.rbegin(), target_itr);
                    for (auto i = 0; i < pop_num; i++) {
                        // move back to the previous container node.
                        mp_current_node = m_node_stack.back();
                        m_node_stack.pop_back();
                        m_indent_stack.pop_back();
                    }
                }

                if (mp_current_node->is_null()) {
                    *mp_current_node = node_type::mapping();
                }

                m_node_stack.push_back(mp_current_node);
                m_indent_stack.emplace_back(line, indent, true);

                type = lexer.get_next_token();
                if (type == lexical_token_t::SEQUENCE_BLOCK_PREFIX) {
                    m_indent_stack.emplace_back(lexer.get_lines_processed(), lexer.get_last_token_begin_pos(), false);
                    mp_current_node = new node_type(node_t::SEQUENCE);
                    apply_directive_set(*mp_current_node);
                    break;
                }

                mp_current_node = new node_type();
                apply_directive_set(*mp_current_node);
                indent = lexer.get_last_token_begin_pos();
                line = lexer.get_lines_processed();

                continue;
            }
            case lexical_token_t::KEY_SEPARATOR: {
                bool is_stack_empty = m_node_stack.empty();
                if (is_stack_empty) {
                    throw parse_error("A key separator found without key.", line, indent);
                }

                // hold the line count of the key separator for later use.
                std::size_t old_indent = indent;
                std::size_t old_line = line;

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

                bool is_implicit_same_line =
                    (line == old_line) && (m_indent_stack.empty() || old_indent > m_indent_stack.back().indent);
                if (is_implicit_same_line) {
                    // a key separator for an implicit key with its value on the same line.
                    continue;
                }

                if (line > old_line) {
                    if (m_needs_tag_impl) {
                        tag_t tag_type = tag_resolver::resolve_tag(m_tag_name, mp_directive_set);
                        if (tag_type == tag_t::MAPPING) {
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
                            continue;
                        }
                    }

                    switch (type) {
                    case lexical_token_t::SEQUENCE_BLOCK_PREFIX:
                        // a key separator preceeding block sequence entries
                        *mp_current_node = node_type::sequence();
                        apply_directive_set(*mp_current_node);
                        apply_node_properties(*mp_current_node);
                        break;
                    case lexical_token_t::EXPLICIT_KEY_PREFIX:
                        // a key separator for a explicit block mapping key.
                        *mp_current_node = node_type::mapping();
                        apply_directive_set(*mp_current_node);
                        break;
                    // defer checking the existence of a key separator after the scalar until a deserialize_scalar()
                    // call.
                    case lexical_token_t::NULL_VALUE:
                    case lexical_token_t::BOOLEAN_VALUE:
                    case lexical_token_t::INTEGER_VALUE:
                    case lexical_token_t::FLOAT_NUMBER_VALUE:
                    case lexical_token_t::STRING_VALUE:
                    // defer handling these tokens until the next loop.
                    case lexical_token_t::MAPPING_FLOW_BEGIN:
                    case lexical_token_t::SEQUENCE_FLOW_BEGIN:
                        break;
                    default:   // LCOV_EXCL_LINE
                        break; // LCOV_EXCL_LINE
                    }

                    continue;
                }

                // handle explicit mapping key separators.

                while (!m_indent_stack.back().is_explicit_key) {
                    mp_current_node = m_node_stack.back();
                    m_node_stack.pop_back();
                    m_indent_stack.pop_back();
                }

                if (m_node_stack.back()->is_sequence()) {
                    mp_current_node = m_node_stack.back();
                    m_node_stack.pop_back();
                }
                if (m_node_stack.back() == mp_current_node) {
                    // This path is for nested explicit mapping keys like:
                    // ```yaml
                    // ? ? foo
                    //   : bar
                    // : baz
                    // ```
                    m_node_stack.pop_back();
                }

                node_type* key_node = mp_current_node;
                m_node_stack.back()->template get_value_ref<mapping_type&>().emplace(*key_node, node_type());
                mp_current_node = &(m_node_stack.back()->operator[](*key_node));
                delete key_node;
                key_node = nullptr;
                m_node_stack.push_back(m_node_stack.back());
                m_indent_stack.back().is_explicit_key = false;

                if (type == lexical_token_t::SEQUENCE_BLOCK_PREFIX) {
                    *mp_current_node = node_type::sequence();
                    apply_directive_set(*mp_current_node);
                    apply_node_properties(*mp_current_node);
                }
                indent = lexer.get_last_token_begin_pos();
                line = lexer.get_lines_processed();
                continue;
            }
            case lexical_token_t::VALUE_SEPARATOR:
                FK_YAML_ASSERT(m_flow_context_depth > 0);
                break;
            // just ignore directives
            case lexical_token_t::YAML_VER_DIRECTIVE:
            case lexical_token_t::TAG_DIRECTIVE:
            case lexical_token_t::INVALID_DIRECTIVE:
                break;
            case lexical_token_t::ANCHOR_PREFIX:
            case lexical_token_t::TAG_PREFIX:
                deserialize_node_properties(lexer, type, line, indent);

                // Skip updating the current indent to avoid stacking a wrong indentation.
                //
                //   &foo bar: baz
                //   ^
                //   the correct indent width for the "bar" node key.

                continue;
            case lexical_token_t::SEQUENCE_BLOCK_PREFIX:
                if (mp_current_node->is_sequence()) {
                    bool is_empty = mp_current_node->empty();
                    if (is_empty) {
                        m_indent_stack.emplace_back(line, indent, false);
                        break;
                    }

                    // move back to the previous sequence if necessary.
                    while (!mp_current_node->is_sequence() || indent != m_indent_stack.back().indent) {
                        mp_current_node = m_node_stack.back();
                        m_node_stack.pop_back();
                        m_indent_stack.pop_back();
                    }
                    break;
                }

                // if the current node is a mapping.
                if (m_node_stack.empty()) {
                    throw parse_error("Invalid sequence block prefix(- ) found.", line, indent);
                }

                // move back to the previous sequence if necessary.
                while (!mp_current_node->is_sequence() || indent != m_indent_stack.back().indent) {
                    mp_current_node = m_node_stack.back();
                    m_node_stack.pop_back();
                    m_indent_stack.pop_back();
                }

                // for mappings in a sequence.
                mp_current_node->template get_value_ref<sequence_type&>().emplace_back(node_type::mapping());
                m_node_stack.push_back(mp_current_node);
                mp_current_node = &(mp_current_node->template get_value_ref<sequence_type&>().back());
                apply_directive_set(*mp_current_node);
                break;
            case lexical_token_t::SEQUENCE_FLOW_BEGIN:
                ++m_flow_context_depth;
                *mp_current_node = node_type::sequence();
                apply_directive_set(*mp_current_node);
                apply_node_properties(*mp_current_node);
                break;
            case lexical_token_t::SEQUENCE_FLOW_END: {
                FK_YAML_ASSERT(m_flow_context_depth > 0);
                --m_flow_context_depth;
                bool is_stack_empty = m_node_stack.empty();
                if (!is_stack_empty) {
                    mp_current_node = m_node_stack.back();
                    m_node_stack.pop_back();
                }
                break;
            }
            case lexical_token_t::MAPPING_FLOW_BEGIN:
                ++m_flow_context_depth;
                *mp_current_node = node_type::mapping();
                apply_directive_set(*mp_current_node);
                apply_node_properties(*mp_current_node);
                break;
            case lexical_token_t::MAPPING_FLOW_END: {
                FK_YAML_ASSERT(m_flow_context_depth > 0);
                --m_flow_context_depth;
                bool is_stack_empty = m_node_stack.empty();
                if (!is_stack_empty) {
                    mp_current_node = m_node_stack.back();
                    m_node_stack.pop_back();
                }
                break;
            }
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
            case lexical_token_t::END_OF_DIRECTIVES:
                break;
            case lexical_token_t::END_OF_DOCUMENT:
                // TODO: This token should be handled to support multiple documents.
                break;
            }

            type = lexer.get_next_token();
            indent = lexer.get_last_token_begin_pos();
            line = lexer.get_lines_processed();
        } while (type != lexical_token_t::END_OF_BUFFER);
    }

    /// @brief Deserializes YAML node properties (anchor and/or tag names) if they exist
    /// @param lexer The lexical analyzer to be used.
    /// @param last_type The variable to store the last lexical token type.
    /// @param line The variable to store the line of either the first property or the last non-property token.
    /// @param indent The variable to store the indent of either the first property or the last non-property token.
    /// @return true if any property is found, false otherwise.
    bool deserialize_node_properties(
        lexer_type& lexer, lexical_token_t& last_type, std::size_t& line, std::size_t& indent) {
        std::set<lexical_token_t> prop_types {lexical_token_t::ANCHOR_PREFIX, lexical_token_t::TAG_PREFIX};

        lexical_token_t type = last_type;
        bool ends_loop {false};
        do {
            if (line < lexer.get_lines_processed()) {
                break;
            }

            switch (type) {
            case lexical_token_t::ANCHOR_PREFIX: {
                bool already_specified = prop_types.find(type) == prop_types.end();
                if (already_specified) {
                    throw parse_error(
                        "anchor name cannot be specified more than once to the same node.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }

                prop_types.erase(type);
                m_anchor_name = lexer.get_string();
                m_needs_anchor_impl = true;

                if (prop_types.size() == 1) {
                    line = lexer.get_lines_processed();
                    indent = lexer.get_last_token_begin_pos();
                }

                break;
            }
            case lexical_token_t::TAG_PREFIX: {
                bool already_specified = prop_types.find(type) == prop_types.end();
                if (already_specified) {
                    throw parse_error(
                        "tag name cannot be specified more than once to the same node.",
                        lexer.get_lines_processed(),
                        lexer.get_last_token_begin_pos());
                }

                prop_types.erase(type);
                m_tag_name = lexer.get_string();
                m_needs_tag_impl = true;

                if (prop_types.size() == 1) {
                    line = lexer.get_lines_processed();
                    indent = lexer.get_last_token_begin_pos();
                }

                break;
            }
            default:
                ends_loop = true;
                break;
            }

            if (!ends_loop) {
                type = lexer.get_next_token();
            }
        } while (!ends_loop);

        last_type = type;
        if (prop_types.size() == 2) {
            line = lexer.get_lines_processed();
            indent = lexer.get_last_token_begin_pos();
        }

        return prop_types.size() < 2;
    }

    /// @brief Add new key string to the current YAML node.
    /// @param key a key string to be added to the current YAML node.
    void add_new_key(node_type&& key, const std::size_t indent, const std::size_t line) {
        if (!m_indent_stack.empty() && indent < m_indent_stack.back().indent) {
            auto target_itr =
                std::find_if(m_indent_stack.rbegin(), m_indent_stack.rend(), [indent](const indentation& i) {
                    return indent == i.indent;
                });
            bool is_indent_valid = (target_itr != m_indent_stack.rend());
            if (!is_indent_valid) {
                throw parse_error("Detected invalid indentaion.", line, indent);
            }

            auto pop_num = std::distance(m_indent_stack.rbegin(), target_itr);
            for (auto i = 0; i < pop_num; i++) {
                // move back to the previous container node.
                mp_current_node = m_node_stack.back();
                m_node_stack.pop_back();
                m_indent_stack.pop_back();
            }
        }

        if (mp_current_node->is_sequence()) {
            mp_current_node->template get_value_ref<sequence_type&>().emplace_back(node_type::mapping());
            m_node_stack.push_back(mp_current_node);
            mp_current_node = &(mp_current_node->operator[](mp_current_node->size() - 1));
        }

        mapping_type& map = mp_current_node->template get_value_ref<mapping_type&>();
        bool is_empty = map.empty();
        if (is_empty) {
            if (m_flow_context_depth == 0) {
                m_indent_stack.emplace_back(line, indent, false);
            }
        }
        else {
            // check key duplication in the current mapping if not empty.
            auto itr = map.find(key);
            if (itr != map.end()) {
                throw parse_error("Detected duplication in mapping keys.", line, indent);
            }
        }

        map.emplace(key, node_type());
        m_node_stack.push_back(mp_current_node);
        mp_current_node = &(mp_current_node->operator[](key));
    }

    /// @brief Assign node value to the current node.
    /// @param node_value A rvalue node_type object to be assigned to the current node.
    void assign_node_value(node_type&& node_value) noexcept {
        if (mp_current_node->is_sequence()) {
            mp_current_node->template get_value_ref<sequence_type&>().emplace_back(std::move(node_value));
            return;
        }

        // a scalar node
        *mp_current_node = std::move(node_value);
        if (m_flow_context_depth > 0 || !m_indent_stack.back().is_explicit_key) {
            mp_current_node = m_node_stack.back();
            m_node_stack.pop_back();
        }
    }

    /// @brief Creates a YAML scalar node with the retrieved token information by the lexer.
    /// @param lexer The lexical analyzer to be used.
    /// @param type The type of the last lexical token.
    /// @param indent The last indent size.
    /// @param line The last line.
    /// @return The created YAML scalar node.
    node_type create_scalar_node(lexer_type& lexer, lexical_token_t type, std::size_t indent, std::size_t line) {
        FK_YAML_ASSERT(
            type == lexical_token_t::NULL_VALUE || type == lexical_token_t::BOOLEAN_VALUE ||
            type == lexical_token_t::INTEGER_VALUE || type == lexical_token_t::FLOAT_NUMBER_VALUE ||
            type == lexical_token_t::STRING_VALUE || type == lexical_token_t::ALIAS_PREFIX);

        if (m_needs_tag_impl) {
            if (type == lexical_token_t::ALIAS_PREFIX) {
                throw parse_error("Tag cannot be specified to alias nodes", line, indent);
            }

            tag_t tag_type = tag_resolver::resolve_tag(m_tag_name, mp_directive_set);

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
            auto itr = m_anchor_table.find(alias_name);
            if (itr == m_anchor_table.end()) {
                throw parse_error("The given anchor name must appear prior to the alias node.", line, indent);
            }
            node = node_type::alias_of(m_anchor_table[alias_name]);
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
    /// @param node A detected scalar node by a lexer.
    /// @param indent The current indentation width. Can be updated in this function.
    /// @param line The number of processed lines. Can be updated in this function.
    /// @return true if next token has already been got, false otherwise.
    bool deserialize_scalar(lexer_type& lexer, std::size_t& indent, std::size_t& line, lexical_token_t& type) {
        node_type node = create_scalar_node(lexer, type, indent, line);

        if (mp_current_node->is_mapping()) {
            add_new_key(std::move(node), indent, line);
            return false;
        }

        type = lexer.get_next_token();
        if (type == lexical_token_t::KEY_SEPARATOR) {
            if (mp_current_node->is_scalar()) {
                if (line != lexer.get_lines_processed()) {
                    // This path is for explicit mapping key separator(:)
                    assign_node_value(std::move(node));
                    if (!m_indent_stack.back().is_explicit_key) {
                        m_indent_stack.pop_back();
                    }
                    indent = lexer.get_last_token_begin_pos();
                    line = lexer.get_lines_processed();
                    return true;
                }

                indentation& last_indent = m_indent_stack.back();
                if (last_indent.line == line && !last_indent.is_explicit_key) {
                    throw parse_error("multiple mapping keys are specified on the same line.", line, indent);
                }

                *mp_current_node = node_type::mapping();
                apply_directive_set(*mp_current_node);
            }
            add_new_key(std::move(node), indent, line);
        }
        else {
            assign_node_value(std::move(node));
        }
        indent = lexer.get_last_token_begin_pos();
        line = lexer.get_lines_processed();
        return true;
    }

    /// @brief Set YAML directive properties to the given node.
    /// @param node A node_type object to be set YAML directive properties.
    void apply_directive_set(node_type& node) noexcept {
        if (mp_directive_set) {
            node.mp_directive_set = mp_directive_set;
        }
    }

    /// @brief Set YAML node properties (anchor and/or tag names) to the given node.
    /// @param node A node type object to be set YAML node properties.
    void apply_node_properties(node_type& node) {
        if (m_needs_anchor_impl) {
            node.add_anchor_name(m_anchor_name);
            m_anchor_table[m_anchor_name] = node;
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
    /// The stack of YAML nodes.
    std::deque<node_type*> m_node_stack {};
    /// The stack of indentation widths.
    std::deque<indentation> m_indent_stack {};
    /// The current depth of flow contexts.
    std::size_t m_flow_context_depth {0};
    /// The set of YAML directives.
    std::shared_ptr<detail::directive_set> mp_directive_set {};
    /// A flag to determine the need for YAML anchor node implementation.
    bool m_needs_anchor_impl {false};
    /// A flag to determine the need for a corresponding node with the last YAML tag.
    bool m_needs_tag_impl {false};
    /// The last YAML anchor name.
    string_type m_anchor_name {};
    /// The table of YAML anchor nodes.
    std::unordered_map<std::string, node_type> m_anchor_table {};
    /// The last tag name.
    string_type m_tag_name {};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_DESERIALIZER_HPP_ */
