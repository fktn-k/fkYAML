//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef FK_YAML_DETAIL_INPUT_TAG_RESOLVER_HPP
#define FK_YAML_DETAIL_INPUT_TAG_RESOLVER_HPP

#include <memory>
#include <string>

#include <fkYAML/detail/macros/define_macros.hpp>
#include <fkYAML/detail/assert.hpp>
#include <fkYAML/detail/document_metainfo.hpp>
#include <fkYAML/detail/input/tag_t.hpp>
#include <fkYAML/detail/meta/node_traits.hpp>
#include <fkYAML/detail/str_view.hpp>
#include <fkYAML/exception.hpp>

FK_YAML_DETAIL_NAMESPACE_BEGIN

static constexpr str_view default_primary_handle_prefix {"!"};
static constexpr str_view default_secondary_handle_prefix {"tag:yaml.org,2002:"};

template <typename BasicNodeType>
class tag_resolver {
    static_assert(is_basic_node<BasicNodeType>::value, "tag_resolver only accepts basic_node<...>.");
    using doc_metainfo_type = document_metainfo<BasicNodeType>;

public:
    /// @brief Resolve the input tag name into an expanded tag name prepended with a registered prefix.
    /// @param tag The input tag name.
    /// @return The type of a node deduced from the given tag name.
    static tag_t resolve_tag(const str_view tag, const std::shared_ptr<doc_metainfo_type>& directives) {
        const std::string normalized = normalize_tag_name(tag, directives);
        return convert_to_tag_type(normalized);
    }

private:
    static std::string normalize_tag_name(const str_view tag, const std::shared_ptr<doc_metainfo_type>& directives) {
        if FK_YAML_UNLIKELY (tag.empty()) {
            throw invalid_tag("tag must not be empty.", "");
        }
        if FK_YAML_UNLIKELY (tag[0] != '!') {
            throw invalid_tag("tag must start with \'!\'", std::string(tag.begin(), tag.end()).c_str());
        }

        if (tag.size() == 1) {
            // Non-specific tag ("!") will be interpreted as one of the following:
            //   * tag:yaml.org,2002:seq
            //   * tag:yaml.org,2002:map
            //   * tag:yaml.org,2002:str
            // See the "Non-Specific Tags" section in https://yaml.org/spec/1.2.2/#691-node-tags.
            // The interpretation cannot take place here because the input lacks the corresponding value.
            return {tag.begin(), tag.end()};
        }

        std::string normalized {"!<"};
        switch (tag[1]) {
        case '!': {
            // handle a secondary tag handle (!!suffix -> !<[secondary][suffix]>)
            const bool is_null_or_empty = !directives || directives->secondary_handle_prefix.empty();
            if (is_null_or_empty) {
                normalized.append(default_secondary_handle_prefix.begin(), default_secondary_handle_prefix.end());
            }
            else {
                normalized += directives->secondary_handle_prefix;
            }

            const str_view body = tag.substr(2);
            normalized.append(body.begin(), body.end());
            break;
        }
        case '<':
            if (tag[2] == '!') {
                const bool is_null_or_empty = !directives || directives->primary_handle_prefix.empty();
                if (is_null_or_empty) {
                    normalized.append(default_primary_handle_prefix.begin(), default_primary_handle_prefix.end());
                }
                else {
                    normalized += directives->primary_handle_prefix;
                }

                const str_view body = tag.substr(3);
                return normalized.append(body.begin(), body.end());
            }

            // verbatim tags must be delivered as-is to the application.
            // See https://yaml.org/spec/1.2.2/#691-node-tags for more details.
            return {tag.begin(), tag.end()};
        default: {
            const std::size_t tag_end_pos = tag.find_first_of('!', 1);

            // handle a named handle (!tag!suffix -> !<[tag][suffix]>)
            if (tag_end_pos != std::string::npos) {
                // there must be a non-empty suffix. (already checked by the lexer.)
                FK_YAML_ASSERT(tag_end_pos < tag.size() - 1);

                const bool is_null_or_empty = !directives || directives->named_handle_map.empty();
                if FK_YAML_UNLIKELY (is_null_or_empty) {
                    throw invalid_tag(
                        "named handle has not been registered.", std::string(tag.begin(), tag.end()).c_str());
                }

                // find the extracted named handle in the map.
                const str_view named_handle = tag.substr(0, tag_end_pos + 1);
                auto named_handle_itr = directives->named_handle_map.find({named_handle.begin(), named_handle.end()});
                auto end_itr = directives->named_handle_map.end();
                if FK_YAML_UNLIKELY (named_handle_itr == end_itr) {
                    throw invalid_tag(
                        "named handle has not been registered.", std::string(tag.begin(), tag.end()).c_str());
                }

                // The YAML spec prohibits expanding the percent-encoded characters (%xx -> a UTF-8 byte).
                // So no conversion takes place.
                // See https://yaml.org/spec/1.2.2/#56-miscellaneous-characters for more details.

                normalized += named_handle_itr->second;
                const str_view body = tag.substr(tag_end_pos + 1);
                normalized.append(body.begin(), body.end());
                break;
            }

            // handle a primary tag handle (!suffix -> !<[primary][suffix]>)
            const bool is_null_or_empty = !directives || directives->primary_handle_prefix.empty();
            if (is_null_or_empty) {
                normalized.append(default_primary_handle_prefix.begin(), default_primary_handle_prefix.end());
            }
            else {
                normalized += directives->primary_handle_prefix;
            }

            const str_view body = tag.substr(1);
            normalized.append(body.begin(), body.end());
            break;
        }
        }

        normalized += ">";
        return normalized;
    }

    static tag_t convert_to_tag_type(const std::string& normalized) {
        if (normalized == "!") {
            return tag_t::NON_SPECIFIC;
        }

        if (normalized.size() < 24 /* size of !<tag:yaml.org,2002:xxx */) {
            return tag_t::CUSTOM_TAG;
        }
        if (normalized.rfind("!<tag:yaml.org,2002:", 0) == std::string::npos) {
            return tag_t::CUSTOM_TAG;
        }

        if (normalized == "!<tag:yaml.org,2002:seq>") {
            return tag_t::SEQUENCE;
        }
        if (normalized == "!<tag:yaml.org,2002:map>") {
            return tag_t::MAPPING;
        }
        if (normalized == "!<tag:yaml.org,2002:null>") {
            return tag_t::NULL_VALUE;
        }
        if (normalized == "!<tag:yaml.org,2002:bool>") {
            return tag_t::BOOLEAN;
        }
        if (normalized == "!<tag:yaml.org,2002:int>") {
            return tag_t::INTEGER;
        }
        if (normalized == "!<tag:yaml.org,2002:float>") {
            return tag_t::FLOATING_NUMBER;
        }
        if (normalized == "!<tag:yaml.org,2002:str>") {
            return tag_t::STRING;
        }

        return tag_t::CUSTOM_TAG;
    }
};

FK_YAML_DETAIL_NAMESPACE_END

#endif /* FK_YAML_DETAIL_INPUT_TAG_RESOLVER_HPP */
