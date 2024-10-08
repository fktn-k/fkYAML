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
#include <fkYAML/detail/encodings/yaml_escaper.hpp>
#include <fkYAML/detail/input/block_scalar_header.hpp>
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

    basic_node_type parse_flow(lexical_token_t lex_type, tag_t tag_type, str_view token) {
        FK_YAML_ASSERT(
            lex_type == lexical_token_t::PLAIN_SCALAR || lex_type == lexical_token_t::SINGLE_QUOTED_SCALAR ||
            lex_type == lexical_token_t::DOUBLE_QUOTED_SCALAR);
        FK_YAML_ASSERT(tag_type != tag_t::SEQUENCE && tag_type != tag_t::MAPPING);

        token = parse_flow_scalar_token(lex_type, token);
        node_type value_type = decide_value_type(lex_type, tag_type, token);
        return create_scalar_node(value_type, token);
    }

    basic_node_type parse_block(
        lexical_token_t lex_type, tag_t tag_type, str_view token, const block_scalar_header& header) {
        FK_YAML_ASSERT(
            lex_type == lexical_token_t::BLOCK_LITERAL_SCALAR || lex_type == lexical_token_t::BLOCK_FOLDED_SCALAR);
        FK_YAML_ASSERT(tag_type != tag_t::SEQUENCE && tag_type != tag_t::MAPPING);

        if (lex_type == lexical_token_t::BLOCK_LITERAL_SCALAR) {
            token = parse_block_literal_scalar(token, header);
        }
        else {
            token = parse_block_folded_scalar(token, header);
        }

        node_type value_type = decide_value_type(lex_type, tag_type, token);
        return create_scalar_node(value_type, token);
    }

private:
    str_view parse_flow_scalar_token(lexical_token_t lex_type, str_view token) {
        switch (lex_type) {
        case lexical_token_t::SINGLE_QUOTED_SCALAR:
            token = parse_single_quoted_scalar(token);
            break;
        case lexical_token_t::DOUBLE_QUOTED_SCALAR:
            token = parse_double_quoted_scalar(token);
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

        constexpr str_view filter = "\'\n";
        std::size_t pos = token.find_first_of(filter);
        if (pos == str_view::npos) {
            return token;
        }

        m_use_owned_buffer = true;

        if (m_buffer.capacity() < token.size()) {
            m_buffer.reserve(token.size());
        }

        do {
            FK_YAML_ASSERT(pos < token.size());
            FK_YAML_ASSERT(token[pos] == '\'' || token[pos] == '\n');

            if (token[pos] == '\'') {
                // unescape escaped single quote. ('' -> ')
                FK_YAML_ASSERT(pos + 1 < token.size());
                m_buffer.append(token.begin(), token.begin() + (pos + 1));
                token.remove_prefix(pos + 2); // move next to the escaped single quote.
            }
            else {
                process_line_folding(token, pos);
            }

            pos = token.find_first_of(filter);
        } while (pos != str_view::npos);

        if (!token.empty()) {
            m_buffer.append(token.begin(), token.size());
        }

        return {m_buffer};
    }

    str_view parse_double_quoted_scalar(str_view token) {
        if (token.empty()) {
            return token;
        }

        constexpr str_view filter = "\\\n";
        std::size_t pos = token.find_first_of(filter);
        if (pos == str_view::npos) {
            return token;
        }

        m_use_owned_buffer = true;

        if (m_buffer.capacity() < token.size()) {
            m_buffer.reserve(token.size());
        }

        do {
            FK_YAML_ASSERT(pos < token.size());
            FK_YAML_ASSERT(token[pos] == '\\' || token[pos] == '\n');

            if (token[pos] == '\\') {
                FK_YAML_ASSERT(pos + 1 < token.size());
                m_buffer.append(token.begin(), token.begin() + pos);

                if (token[pos + 1] != '\n') {
                    token.remove_prefix(pos);
                    const char* p_escape_begin = token.begin();
                    bool is_valid_escaping = yaml_escaper::unescape(p_escape_begin, token.end(), m_buffer);
                    if FK_YAML_UNLIKELY (!is_valid_escaping) {
                        throw parse_error(
                            "Unsupported escape sequence is found in a double quoted scalar.", m_line, m_indent);
                    }

                    // `p_escape_begin` points to the last element of the escape sequence.
                    token.remove_prefix((p_escape_begin - token.begin()) + 1);
                }
                else {
                    std::size_t non_space_pos = token.find_first_not_of(" \t", pos + 2);
                    if (non_space_pos == str_view::npos) {
                        non_space_pos = token.size();
                    }
                    token.remove_prefix(non_space_pos);
                }
            }
            else {
                process_line_folding(token, pos);
            }

            pos = token.find_first_of(filter);
        } while (pos != str_view::npos);

        if (!token.empty()) {
            m_buffer.append(token.begin(), token.size());
        }

        return {m_buffer};
    }

    str_view parse_block_literal_scalar(str_view token, const block_scalar_header& header) {
        FK_YAML_ASSERT(header.indent > 0);

        if FK_YAML_UNLIKELY (token.empty()) {
            return token;
        }

        m_use_owned_buffer = true;
        m_buffer.reserve(token.size());

        std::size_t cur_line_begin_pos = 0;
        do {
            bool has_newline_at_end = true;
            std::size_t cur_line_end_pos = token.find('\n', cur_line_begin_pos);
            if (cur_line_end_pos == str_view::npos) {
                has_newline_at_end = false;
                cur_line_end_pos = token.size();
            }

            std::size_t line_size = cur_line_end_pos - cur_line_begin_pos;
            str_view line = token.substr(cur_line_begin_pos, line_size);

            if (line.size() > header.indent) {
                m_buffer.append(line.begin() + header.indent, line.end());
            }

            if (!has_newline_at_end) {
                break;
            }

            m_buffer.push_back('\n');
            cur_line_begin_pos = cur_line_end_pos + 1;
        } while (cur_line_begin_pos < token.size());

        process_chomping(header.chomp);

        return {m_buffer};
    }

    str_view parse_block_folded_scalar(str_view token, const block_scalar_header& header) {
        FK_YAML_ASSERT(header.indent > 0);

        if FK_YAML_UNLIKELY (token.empty()) {
            return token;
        }

        m_use_owned_buffer = true;
        m_buffer.reserve(token.size());

        std::size_t cur_line_begin_pos = 0;
        bool prev_line_has_content = false;
        do {
            bool has_newline_at_end = true;
            std::size_t cur_line_end_pos = token.find('\n', cur_line_begin_pos);
            if (cur_line_end_pos == str_view::npos) {
                has_newline_at_end = false;
                cur_line_end_pos = token.size();
            }

            std::size_t line_size = cur_line_end_pos - cur_line_begin_pos;
            str_view line = token.substr(cur_line_begin_pos, line_size);

            if (line.size() <= header.indent) {
                // empty or less-indented lines are turned into a newline
                m_buffer.push_back('\n');
                prev_line_has_content = false;
                continue;
            }
            else {
                if (prev_line_has_content) {
                    m_buffer.push_back(' ');
                    // `prev_line_has_content` is not set to false since the current line also has contents.
                }

                m_buffer.append(line.begin(), line.end());

                std::size_t non_space_pos = line.find_first_not_of(' ');
                if (non_space_pos > header.indent && has_newline_at_end) {
                    // more-indented lines are not folded.
                    m_buffer.push_back('\n');
                }
            }

            if (!has_newline_at_end) {
                break;
            }

            cur_line_begin_pos = cur_line_end_pos + 1;
        } while (cur_line_begin_pos < token.size());

        std::size_t non_break_pos = m_buffer.find_last_not_of('\n');
        if (non_break_pos != std::string::npos) {
            // The final content line break are not folded.
            m_buffer.push_back('\n');
        }

        process_chomping(header.chomp);

        return {m_buffer};
    }

    void process_chomping(chomping_indicator_t chomp) {
        if (!m_buffer.empty()) {
            switch (chomp) {
            case chomping_indicator_t::STRIP: {
                std::size_t content_end_pos = m_buffer.find_last_not_of('\n');
                if (content_end_pos == std::string::npos) {
                    // if the scalar has no content line, all lines are considered as trailing empty lines.
                    m_buffer.clear();
                    break;
                }

                if (content_end_pos == m_buffer.size() - 1) {
                    // no last content line break nor trailing empty lines.
                    break;
                }

                // remove the last content line break and all trailing empty lines.
                m_buffer.erase(content_end_pos + 1);

                break;
            }
            case chomping_indicator_t::CLIP: {
                std::size_t content_end_pos = m_buffer.find_last_not_of('\n');
                if (content_end_pos == std::string::npos) {
                    // if the scalar has no content line, all lines are considered as trailing empty lines.
                    m_buffer.clear();
                    break;
                }

                if (content_end_pos == m_buffer.size() - 1) {
                    // no trailing empty lines
                    break;
                }

                // remove all trailing empty lines.
                m_buffer.erase(content_end_pos + 2);

                break;
            }
            case chomping_indicator_t::KEEP:
                break;
            }
        }
    }

    void process_line_folding(str_view& token, std::size_t newline_pos) noexcept {
        // discard trailing white spaces which precedes the line break in the current line.
        std::size_t last_non_space_pos = token.substr(0, newline_pos + 1).find_last_not_of(" \t");
        if (last_non_space_pos == str_view::npos) {
            m_buffer.append(token.begin(), newline_pos);
        }
        else {
            m_buffer.append(token.begin(), last_non_space_pos + 1);
        }
        token.remove_prefix(newline_pos + 1); // move next to the LF

        uint32_t empty_line_counts = 0;
        do {
            std::size_t non_space_pos = token.find_first_not_of(" \t");
            if (non_space_pos == str_view::npos) {
                // Line folding ignores trailing spaces.
                token.remove_prefix(token.size());
                break;
            }
            else if (token[non_space_pos] != '\n') {
                token.remove_prefix(non_space_pos);
                break;
            }

            token.remove_prefix(non_space_pos + 1);
            ++empty_line_counts;
        } while (true);

        if (empty_line_counts > 0) {
            m_buffer.append(empty_line_counts, '\n');
        }
        else {
            m_buffer.push_back(' ');
        }
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
            if (m_use_owned_buffer) {
                node = basic_node_type(std::move(m_buffer));
                m_use_owned_buffer = false;
            }
            else {
                node = basic_node_type(std::string(token.begin(), token.end()));
            }
            break;
        default:   // LCOV_EXCL_LINE
            break; // LCOV_EXCL_LINE
        }

        return node;
    }

    uint32_t m_line {0};
    uint32_t m_indent {0};
    bool m_use_owned_buffer {false};
    std::string m_buffer {};
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_SCALAR_PARSER_HPP_ */
