//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.12
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_INPUT_SCALAR_PARSER_HPP_
#define FK_YAML_DETAIL_INPUT_SCALAR_PARSER_HPP_

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/assert.hpp>
#include <fkYAML/detail/conversions/scalar_conv.hpp>
#include <fkYAML/detail/input/scalar_scanner.hpp>
#include <fkYAML/detail/input/tag_t.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/str_view.hpp>
#include <fkYAML/detail/types/lexical_token_t.hpp>
#include <fkYAML/exception.hpp>
#include <fkYAML/node_type.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

template <typename BasicNodeType>
class scalar_parser {
    static_assert(is_basic_node<BasicNodeType>::value, "scalar_parser only accepts basic_node<...>");

public:
    using basic_node_type = BasicNodeType;

private:
    /** A type for boolean node values. */
    using boolean_type = typename basic_node_type::boolean_type;
    /** A type for integer node values. */
    using integer_type = typename basic_node_type::integer_type;
    /** A type for floating point node values. */
    using float_number_type = typename basic_node_type::float_number_type;
    /** A type for string node values. */
    using string_type = typename basic_node_type::string_type;

public:
    scalar_parser(uint32_t line, uint32_t indent) noexcept
        : m_line(line),
          m_indent(indent) {
    }

    ~scalar_parser() noexcept = default;

    scalar_parser(const scalar_parser&) noexcept = default;
    scalar_parser(scalar_parser&&) noexcept = default;
    scalar_parser& operator=(const scalar_parser&) noexcept = default;
    scalar_parser& operator=(scalar_parser&&) noexcept = default;

    basic_node_type parse(lexical_token_t lex_type, tag_t tag_type, str_view token) {
        FK_YAML_ASSERT(
            lex_type == lexical_token_t::PLAIN_SCALAR || lex_type == lexical_token_t::SINGLE_QUOTED_SCALAR ||
            lex_type == lexical_token_t::DOUBLE_QUOTED_SCALAR || lex_type == lexical_token_t::BLOCK_SCALAR);
        FK_YAML_ASSERT(tag_type != tag_t::SEQUENCE && tag_type != tag_t::MAPPING);

        token = parse_scalar_token(lex_type, token);
        node_type value_type = decide_value_type(lex_type, tag_type, token);
        return create_scalar_node(value_type, token);
    }

private:
    str_view parse_scalar_token(lexical_token_t lex_type, str_view token) {
        switch (lex_type) {
        case lexical_token_t::SINGLE_QUOTED_SCALAR:
            token = parse_single_quoted_scalar(token);
            break;
        case lexical_token_t::PLAIN_SCALAR:
        default:
            break;
        }

        return token;
    }

    str_view parse_single_quoted_scalar(str_view token) noexcept {
        if (token.empty()) {
            return token;
        }

        std::size_t pos = token.find_first_of("\'\n");
        if (pos == str_view::npos) {
            return token;
        }

        if (m_buffer.capacity() < token.size()) {
            m_buffer.reserve(token.size());
        }

        do {
            if (token[pos] == '\'') {
                // unescape escaped single quote. ('' -> ')
                m_buffer.append(token.begin(), token.begin() + (pos + 1));
                token.remove_prefix(pos + 2); // move next to the escaped single quote.
                continue;
            }

            // process line folding

            // discard trailing white spaces which precedes the line break in the current line.
            std::size_t last_non_space_pos = token.substr(0, pos).find_last_not_of(" \t");
            if (last_non_space_pos == str_view::npos) {
                m_buffer.append(token.begin(), pos);
            }
            else {
                m_buffer.append(token.begin(), last_non_space_pos + 1);
            }
            token.remove_prefix(pos + 1); // move next to the LF

            uint32_t empty_line_counts = 0;
            do {
                std::size_t non_space_pos = token.find_first_not_of(" \t");
                if (non_space_pos == str_view::npos || token[non_space_pos] != '\n') {
                    if (non_space_pos == str_view::npos) {
                        non_space_pos = token.size();
                    }
                    token.remove_prefix(non_space_pos);
                    break;
                }

                ++empty_line_counts;
            } while (true);

            if (empty_line_counts > 0) {
                m_buffer.append(empty_line_counts, '\n');
            }
            else {
                m_buffer.push_back(' ');
            }

            pos = token.find_first_of("\'\n");
        } while (pos != str_view::npos);

        if (!token.empty()) {
            m_buffer.append(token.begin(), token.size());
        }

        return {m_buffer};
    }

    node_type decide_value_type(lexical_token_t lex_type, tag_t tag_type, str_view token) const noexcept {
        node_type value_type {node_type::STRING};
        if (lex_type == lexical_token_t::PLAIN_SCALAR) {
            value_type = scalar_scanner::scan(token.begin(), token.end());
        }

        switch (tag_type) {
        case tag_t::NULL_VALUE:
            value_type = node_type::NULL_OBJECT;
            break;
        case tag_t::BOOLEAN:
            value_type = node_type::BOOLEAN;
            break;
        case tag_t::INTEGER:
            value_type = node_type::INTEGER;
            break;
        case tag_t::FLOATING_NUMBER:
            value_type = node_type::FLOAT;
            break;
        case tag_t::STRING:
            value_type = node_type::STRING;
            break;
        case tag_t::NON_SPECIFIC:
            // scalars with the non-specific tag is resolved to a string tag.
            // See the "Non-Specific Tags" section in https://yaml.org/spec/1.2.2/#691-node-tags.
            value_type = node_type::STRING;
            break;
        case tag_t::NONE:
        case tag_t::CUSTOM_TAG:
        default:
            break;
        }

        return value_type;
    }

    basic_node_type create_scalar_node(node_type type, str_view token) {
        basic_node_type node {};

        switch (type) {
        case node_type::NULL_OBJECT: {
            std::nullptr_t null = nullptr;
            bool converted = detail::aton(token.begin(), token.end(), null);
            if FK_YAML_UNLIKELY (!converted) {
                throw parse_error("Failed to convert a scalar to a null.", m_line, m_indent);
            }
            // The above `node` variable is already null, so no instance creation is needed.
            break;
        }
        case node_type::BOOLEAN: {
            boolean_type boolean = static_cast<boolean_type>(false);
            bool converted = detail::atob(token.begin(), token.end(), boolean);
            if FK_YAML_UNLIKELY (!converted) {
                throw parse_error("Failed to convert a scalar to a boolean.", m_line, m_indent);
            }
            node = basic_node_type(boolean);
            break;
        }
        case node_type::INTEGER: {
            integer_type integer = 0;
            bool converted = detail::atoi(token.begin(), token.end(), integer);
            if FK_YAML_UNLIKELY (!converted) {
                throw parse_error("Failed to convert a scalar to an integer.", m_line, m_indent);
            }
            node = basic_node_type(integer);
            break;
        }
        case node_type::FLOAT: {
            float_number_type float_val = 0;
            bool converted = detail::atof(token.begin(), token.end(), float_val);
            if FK_YAML_UNLIKELY (!converted) {
                throw parse_error("Failed to convert a scalar to a floating point value", m_line, m_indent);
            }
            node = basic_node_type(float_val);
            break;
        }
        case node_type::STRING:
            node = basic_node_type(std::string(token.begin(), token.end()));
            break;
        default:   // LCOV_EXCL_LINE
            break; // LCOV_EXCL_LINE
        }

        return node;
    }

    uint32_t m_line {0};
    uint32_t m_indent {0};
    std::string m_buffer {};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_SCALAR_PARSER_HPP_ */
