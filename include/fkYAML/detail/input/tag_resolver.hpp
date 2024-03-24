///  _______   __ __   __  _____   __  __  __
/// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
/// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.2
/// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
///
/// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
/// SPDX-License-Identifier: MIT
///
/// @file

#ifndef FK_YAML_DETAIL_INPUT_TAG_RESOLVER_HPP_
#define FK_YAML_DETAIL_INPUT_TAG_RESOLVER_HPP_

#include <string>
#include <utility>
#include <unordered_map>

#include <fkYAML/detail/macros/version_macros.hpp>
#include <fkYAML/detail/assert.hpp>
#include <fkYAML/exception.hpp>

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

/**
 * @namespace detail
 * @brief namespace for internal implementaions of fkYAML library.
 */
namespace detail
{

class tag_resolver
{
public:
    void update_primary_tag_handle_prefix(std::string&& prefix)
    {
        m_primary_tag_handle_prefix = std::move(prefix);
    }

    void update_secondary_tag_handle_prefix(std::string&& prefix)
    {
        if (m_secondary_tag_handle_prefix != prefix)
        {
            m_secondary_tag_handle_prefix = std::move(prefix);
            m_secondary_tag_handle_prefix_changed = true;
        }
    }

    void add_named_handle(std::string&&, std::string&&)
    {
    }

    /// @brief Resolve the input tag name into an expanded tag name prepended with a registered prefix.
    /// @param tag
    /// @return
    std::string resolve_tag(const std::string& tag)
    {
        if (tag.empty())
        {
            throw invalid_tag("tag must not be empty.", "");
        }
        if (tag[0] != '!')
        {
            throw invalid_tag("tag must start with \'!\'", tag.c_str());
        }

        if (tag.size() == 1)
        {
            // Non-specific tag ("!") will be interpreted as one of the following:
            //   * tag:yaml.org,2002:seq
            //   * tag:yaml.org,2002:map
            //   * tag:yaml.org,2002:str
            // See the "Non-Specific Tags" section in https://yaml.org/spec/1.2.2/#691-node-tags.
            // The interpretation cannot take place here because the input lacks the corresponding value.
            return "!";
        }

        std::string resolved {"!<"};
        switch (tag[1])
        {
        case '!':
            resolved += m_secondary_tag_handle_prefix + tag;
            break;
        case '<':
            resolved += "<";
            resolved += tag;
            break;
        default: {
            auto tag_end_pos = tag.find_last_of('!');

            // handle a named handle (!tag!suffix)
            if (tag_end_pos != std::string::npos)
            {
                if (tag_end_pos == tag.size() - 1)
                {
                    // Tag shorthand must be followed by a non-empty suffix.
                    // See the "Tag Shorthands" section in https://yaml.org/spec/1.2.2/#691-node-tags.
                    throw invalid_tag("named handle has no suffix.", tag.c_str());
                }

                // find the extracted named handle in the map.
                auto named_handle_itr = m_named_handle_map.find(tag.substr(0, tag_end_pos + 1));
                if (named_handle_itr == m_named_handle_map.end())
                {
                    throw invalid_tag("named handle has not been registered.", tag.c_str());
                }

                // Defer the percent-encoding (%xx -> a UTF-8 byte) until a tag name is queried because:
                // * it will not affect the results of tag resolution, and
                // * it will make it hard to gurantee semantic equality during a round trip.
                // So, just the content of the suffix is validated.

                auto suffix_itr = tag.begin() + tag_end_pos + 1;
                auto suffix_end = tag.end();
                for (; suffix_itr != suffix_end; ++suffix_itr)
                {
                    if (*suffix_itr == '%')
                    {
                        for (int i = 0; i < 2; i++, ++suffix_itr)
                        {
                            // Normalize a character for a-f/A-F comparison
                            int octet_char = std::tolower(*suffix_itr);

                            if ('0' <= octet_char && octet_char <= '9')
                            {
                                continue;
                            }

                            if ('a' <= octet_char && octet_char <= 'f')
                            {
                                continue;
                            }

                            throw invalid_tag("a percent-encoding must be in the \%<HEXDIG><HEXDIG> format", tag.c_str());
                        }

                        continue;
                    }

                    // Check if the current character is one of reserved/unreserved characters which are allowed for use.
                    // See the following links for details:
                    // * reserved characters:   https://datatracker.ietf.org/doc/html/rfc3986#section-2.2
                    // * unreserved characters: https://datatracker.ietf.org/doc/html/rfc3986#section-2.3

                    bool is_allowed_character = false;
                    switch (*suffix_itr)
                    {
                    // reserved characters (gen-delims)
                    case ':':
                    case '/':
                    case '?':
                    case '#':
                    case '[':
                    case ']':
                    case '@':
                    // reserved characters (sub-delims)
                    case '!':
                    case '$':
                    case '&':
                    case '\'':
                    case '(':
                    case ')':
                    case '*':
                    case '+':
                    case ',':
                    case ';':
                    case '=':
                    // unreserved characters
                    case '-':
                    case '.':
                    case '_':
                    case '~':
                    // alphabets and digits are checked after this switch-case statement.
                        is_allowed_character = true;
                        break;
                    }

                    if (is_allowed_character || std::isalnum(*suffix_itr))
                    {
                        continue;
                    }

                    throw invalid_tag("tag contains an unallowed ASCII character.", tag.c_str());
                }

                resolved += named_handle_itr->second + tag.substr(tag_end_pos + 1);
            }

            break;
        }
        }

        resolved += ">";

        return resolved;
    }

private:
    std::string decode_tag_shorthand_suffix(const std::string& suffix)
    {
        std::size_t suffix_size = suffix.size();
        std::string decoded {};
        decoded.reserve(suffix_size);



        for (std::size_t i = 0; i < suffix_size; i++)
        {
            if (suffix[i] != '%')
            {
                decoded.push_back(suffix[i]);
                continue;
            }

            if (i + 2 >= suffix_size)
            {
                throw invalid_tag("tag shorthand contains invalid parcent-encoding.", suffix.c_str());
            }
        }
    }


    static constexpr const char* s_default_secondary_tag_prefix = "tag:yaml.org,2002:";

    std::string m_primary_tag_handle_prefix {""};
    bool m_secondary_tag_handle_prefix_changed {false};
    std::string m_secondary_tag_handle_prefix {s_default_secondary_tag_prefix};
    std::unordered_map<std::string /*handle_name*/, std::string /**/> m_named_handle_map {};
};

}; // namespace detail

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_TAG_RESOLVER_HPP_ */
