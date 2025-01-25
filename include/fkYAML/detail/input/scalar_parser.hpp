//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_INPUT_SCALAR_PARSER_HPP
#define FK_YAML_DETAIL_INPUT_SCALAR_PARSER_HPP

#include <fkYAML/detail/macros/define_macros.hpp>
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

/// @brief A parser for YAML scalars.
/// @tparam BasicNodeType A type of the container for parsed YAML scalars.
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
    /// @brief Constructs a new scalar_parser object.
    /// @param line Current line.
    /// @param indent Current indentation.
    scalar_parser(uint32_t line, uint32_t indent) noexcept
        : m_line(line),
          m_indent(indent) {
    }

    /// @brief Destroys a scalar_parser object.
    ~scalar_parser() noexcept = default;

    // std::string's copy constructor/assignment operator may throw a exception.
    scalar_parser(const scalar_parser&) = default;
    scalar_parser& operator=(const scalar_parser&) = default;

    scalar_parser(scalar_parser&&) noexcept = default;
    scalar_parser& operator=(scalar_parser&&) noexcept(std::is_nothrow_move_assignable<std::string>::value) = default;

    /// @brief Parses a token into a flow scalar (either plain, single quoted or double quoted)
    /// @param lex_type Lexical token type for the scalar.
    /// @param tag_type Tag type for the scalar.
    /// @param token Scalar contents.
    /// @return Parsed YAML flow scalar object.
    basic_node_type parse_flow(lexical_token_t lex_type, tag_t tag_type, str_view token) {
        FK_YAML_ASSERT(
            lex_type == lexical_token_t::PLAIN_SCALAR || lex_type == lexical_token_t::SINGLE_QUOTED_SCALAR ||
            lex_type == lexical_token_t::DOUBLE_QUOTED_SCALAR);
        FK_YAML_ASSERT(tag_type != tag_t::SEQUENCE && tag_type != tag_t::MAPPING);

        token = parse_flow_scalar_token(lex_type, token);
        const node_type value_type = decide_value_type(lex_type, tag_type, token);
        return create_scalar_node(value_type, tag_type, token);
    }

    /// @brief Parses a token into a block scalar (either literal or folded)
    /// @param lex_type Lexical token type for the scalar.
    /// @param tag_type Tag type for the scalar.
    /// @param token Scalar contents.
    /// @param header Block scalar header information.
    /// @return Parsed YAML block scalar object.
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

        const node_type value_type = decide_value_type(lex_type, tag_type, token);
        return create_scalar_node(value_type, tag_type, token);
    }

private:
    /// @brief Parses a token into a flow scalar contents.
    /// @param lex_type Lexical token type for the scalar.
    /// @param token Scalar contents.
    /// @return View into the parsed scalar contents.
    str_view parse_flow_scalar_token(lexical_token_t lex_type, str_view token) {
        switch (lex_type) {
        case lexical_token_t::PLAIN_SCALAR:
            token = parse_plain_scalar(token);
            break;
        case lexical_token_t::SINGLE_QUOTED_SCALAR:
            token = parse_single_quoted_scalar(token);
            break;
        case lexical_token_t::DOUBLE_QUOTED_SCALAR:
            token = parse_double_quoted_scalar(token);
            break;
        default:           // LCOV_EXCL_LINE
            unreachable(); // LCOV_EXCL_LINE
        }

        return token;
    }

    /// @brief Parses plain scalar contents.
    /// @param token Scalar contents.
    /// @return View into the parsed scalar contents.
    str_view parse_plain_scalar(str_view token) noexcept {
        // plain scalars cannot be empty.
        FK_YAML_ASSERT(!token.empty());

        std::size_t newline_pos = token.find('\n');
        if (newline_pos == str_view::npos) {
            return token;
        }

        m_use_owned_buffer = true;

        if (m_buffer.capacity() < token.size()) {
            m_buffer.reserve(token.size());
        }

        do {
            process_line_folding(token, newline_pos);
            newline_pos = token.find('\n');
        } while (newline_pos != str_view::npos);

        m_buffer.append(token.begin(), token.size());

        return {m_buffer};
    }

    /// @brief Parses single quoted scalar contents.
    /// @param token Scalar contents.
    /// @return View into the parsed scalar contents.
    str_view parse_single_quoted_scalar(str_view token) noexcept {
        if (token.empty()) {
            return token;
        }

        constexpr str_view filter {"\'\n"};
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

    /// @brief Parses double quoted scalar contents.
    /// @param token Scalar contents.
    /// @return View into the parsed scalar contents.
    str_view parse_double_quoted_scalar(str_view token) {
        if (token.empty()) {
            return token;
        }

        constexpr str_view filter {"\\\n"};
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
                    const bool is_valid_escaping = yaml_escaper::unescape(p_escape_begin, token.end(), m_buffer);
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

    /// @brief Parses block literal scalar contents.
    /// @param token Scalar contents.
    /// @param header Block scalar header information.
    /// @return View into the parsed scalar contents.
    str_view parse_block_literal_scalar(str_view token, const block_scalar_header& header) {
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

            const std::size_t line_size = cur_line_end_pos - cur_line_begin_pos;
            const str_view line = token.substr(cur_line_begin_pos, line_size);

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

    /// @brief Parses block folded scalar contents.
    /// @param token Scalar contents.
    /// @param header Block scalar header information.
    /// @return View into the parsed scalar contents.
    str_view parse_block_folded_scalar(str_view token, const block_scalar_header& header) {
        if FK_YAML_UNLIKELY (token.empty()) {
            return token;
        }

        m_use_owned_buffer = true;
        m_buffer.reserve(token.size());

        constexpr str_view white_space_filter {" \t"};

        std::size_t cur_line_begin_pos = 0;
        bool has_newline_at_end = true;
        bool can_be_folded = false;
        do {
            std::size_t cur_line_end_pos = token.find('\n', cur_line_begin_pos);
            if (cur_line_end_pos == str_view::npos) {
                has_newline_at_end = false;
                cur_line_end_pos = token.size();
            }

            const std::size_t line_size = cur_line_end_pos - cur_line_begin_pos;
            const str_view line = token.substr(cur_line_begin_pos, line_size);
            const bool is_empty = line.find_first_not_of(white_space_filter) == str_view::npos;

            if (line.size() <= header.indent) {
                // A less-indented line is turned into a newline.
                m_buffer.push_back('\n');
                can_be_folded = false;
            }
            else if (is_empty) {
                // more-indented empty lines are not folded.
                m_buffer.push_back('\n');
                m_buffer.append(line.begin() + header.indent, line.end());
                m_buffer.push_back('\n');
            }
            else {
                const std::size_t non_space_pos = line.find_first_not_of(white_space_filter);
                const bool is_more_indented = (non_space_pos != str_view::npos) && (non_space_pos > header.indent);

                if (can_be_folded) {
                    if (is_more_indented) {
                        // The content line right before more-indented lines is not folded.
                        m_buffer.push_back('\n');
                    }
                    else {
                        m_buffer.push_back(' ');
                    }

                    can_be_folded = false;
                }

                m_buffer.append(line.begin() + header.indent, line.end());

                if (is_more_indented && has_newline_at_end) {
                    // more-indented lines are not folded.
                    m_buffer.push_back('\n');
                }
                else {
                    can_be_folded = true;
                }
            }

            if (!has_newline_at_end) {
                break;
            }

            cur_line_begin_pos = cur_line_end_pos + 1;
        } while (cur_line_begin_pos < token.size());

        if (has_newline_at_end && can_be_folded) {
            // The final content line break are not folded.
            m_buffer.push_back('\n');
        }

        process_chomping(header.chomp);

        return {m_buffer};
    }

    /// @brief Discards final content line break and trailing empty lines depending on the given chomping type.
    /// @param chomp Chomping method type.
    void process_chomping(chomping_indicator_t chomp) {
        switch (chomp) {
        case chomping_indicator_t::STRIP: {
            const std::size_t content_end_pos = m_buffer.find_last_not_of('\n');
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
            const std::size_t content_end_pos = m_buffer.find_last_not_of('\n');
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

    /// @brief Applies line folding to flow scalar contents.
    /// @param token Flow scalar contents.
    /// @param newline_pos Position of the target newline code.
    void process_line_folding(str_view& token, std::size_t newline_pos) noexcept {
        // discard trailing white spaces which precedes the line break in the current line.
        const std::size_t last_non_space_pos = token.substr(0, newline_pos + 1).find_last_not_of(" \t");
        if (last_non_space_pos == str_view::npos) {
            m_buffer.append(token.begin(), newline_pos);
        }
        else {
            m_buffer.append(token.begin(), last_non_space_pos + 1);
        }
        token.remove_prefix(newline_pos + 1); // move next to the LF

        uint32_t empty_line_counts = 0;
        do {
            const std::size_t non_space_pos = token.find_first_not_of(" \t");
            if (non_space_pos == str_view::npos) {
                // Line folding ignores trailing spaces.
                token.remove_prefix(token.size());
                break;
            }
            if (token[non_space_pos] != '\n') {
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

    /// @brief Decides scalar value type based on the lexical/tag types and scalar contents.
    /// @param lex_type Lexical token type for the scalar.
    /// @param tag_type Tag type for the scalar.
    /// @param token Scalar contents.
    /// @return Scalar value type.
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

    /// @brief Creates YAML scalar object based on the value type and contents.
    /// @param type Scalar value type.
    /// @param token Scalar contents.
    /// @return A YAML scalar object.
    basic_node_type create_scalar_node(node_type val_type, tag_t tag_type, str_view token) {
        switch (val_type) {
        case node_type::NULL_OBJECT: {
            std::nullptr_t null = nullptr;
            const bool converted = detail::aton(token.begin(), token.end(), null);
            if FK_YAML_UNLIKELY (!converted) {
                throw parse_error("Failed to convert a scalar to a null.", m_line, m_indent);
            }
            // The default basic_node object is a null scalar node.
            return basic_node_type {};
        }
        case node_type::BOOLEAN: {
            auto boolean = static_cast<boolean_type>(false);
            const bool converted = detail::atob(token.begin(), token.end(), boolean);
            if FK_YAML_UNLIKELY (!converted) {
                throw parse_error("Failed to convert a scalar to a boolean.", m_line, m_indent);
            }
            return basic_node_type(boolean);
        }
        case node_type::INTEGER: {
            integer_type integer = 0;
            const bool converted = detail::atoi(token.begin(), token.end(), integer);
            if FK_YAML_LIKELY (converted) {
                return basic_node_type(integer);
            }
            if FK_YAML_UNLIKELY (tag_type == tag_t::INTEGER) {
                throw parse_error("Failed to convert a scalar to an integer.", m_line, m_indent);
            }

            // conversion error from a scalar which is not tagged with !!int is recovered by treating it as a string
            // scalar. See https://github.com/fktn-k/fkYAML/issues/428.
            return basic_node_type(string_type(token.begin(), token.end()));
        }
        case node_type::FLOAT: {
            float_number_type float_val = 0;
            const bool converted = detail::atof(token.begin(), token.end(), float_val);
            if FK_YAML_LIKELY (converted) {
                return basic_node_type(float_val);
            }
            if FK_YAML_UNLIKELY (tag_type == tag_t::FLOATING_NUMBER) {
                throw parse_error("Failed to convert a scalar to a floating point value", m_line, m_indent);
            }

            // conversion error from a scalar which is not tagged with !!float is recovered by treating it as a string
            // scalar. See https://github.com/fktn-k/fkYAML/issues/428.
            return basic_node_type(string_type(token.begin(), token.end()));
        }
        case node_type::STRING:
            if (!m_use_owned_buffer) {
                return basic_node_type(string_type(token.begin(), token.end()));
            }
            m_use_owned_buffer = false;
            return basic_node_type(std::move(m_buffer));
        default:                   // LCOV_EXCL_LINE
            detail::unreachable(); // LCOV_EXCL_LINE
        }
    }

    /// Current line
    uint32_t m_line {0};
    /// Current indentation for the scalar
    uint32_t m_indent {0};
    /// Whether the parsed contents are stored in an owned buffer.
    bool m_use_owned_buffer {false};
    /// Owned buffer storage for parsing. This buffer is used when scalar contents need mutation.
    std::string m_buffer;
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_SCALAR_PARSER_HPP */
