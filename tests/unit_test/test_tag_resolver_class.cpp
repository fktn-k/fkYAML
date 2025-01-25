//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("TagResolver_ResolveTag") {
    using test_pair_t = std::pair<fkyaml::detail::str_view, fkyaml::detail::tag_t>;

    fkyaml::detail::tag_t tag_type {};
    std::shared_ptr<fkyaml::detail::document_metainfo<fkyaml::node>> directives {};

    SECTION("valid tag name with default tag handle prefixes") {
        auto test_pair = GENERATE(
            test_pair_t {"!", fkyaml::detail::tag_t::NON_SPECIFIC},
            test_pair_t {"!local", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!<!local>", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!<tag:test>", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!<tag:yaml.org,2002:test>", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!<tag:yaml.org,2002:seq>", fkyaml::detail::tag_t::SEQUENCE},
            test_pair_t {"!<tag:yaml.org,2002:map>", fkyaml::detail::tag_t::MAPPING},
            test_pair_t {"!<tag:yaml.org,2002:null>", fkyaml::detail::tag_t::NULL_VALUE},
            test_pair_t {"!<tag:yaml.org,2002:bool>", fkyaml::detail::tag_t::BOOLEAN},
            test_pair_t {"!<tag:yaml.org,2002:int>", fkyaml::detail::tag_t::INTEGER},
            test_pair_t {"!<tag:yaml.org,2002:float>", fkyaml::detail::tag_t::FLOATING_NUMBER},
            test_pair_t {"!<tag:yaml.org,2002:str>", fkyaml::detail::tag_t::STRING},
            test_pair_t {"!!seq", fkyaml::detail::tag_t::SEQUENCE},
            test_pair_t {"!!map", fkyaml::detail::tag_t::MAPPING},
            test_pair_t {"!!null", fkyaml::detail::tag_t::NULL_VALUE},
            test_pair_t {"!!bool", fkyaml::detail::tag_t::BOOLEAN},
            test_pair_t {"!!int", fkyaml::detail::tag_t::INTEGER},
            test_pair_t {"!!float", fkyaml::detail::tag_t::FLOATING_NUMBER},
            test_pair_t {"!!str", fkyaml::detail::tag_t::STRING});

        REQUIRE_NOTHROW(
            tag_type = fkyaml::detail::tag_resolver<fkyaml::node>::resolve_tag(test_pair.first, directives));
        REQUIRE(tag_type == test_pair.second);
    }

    SECTION("valid tag name with non-default primary handle prefix") {
        directives = std::shared_ptr<fkyaml::detail::document_metainfo<fkyaml::node>>(
            new fkyaml::detail::document_metainfo<fkyaml::node>());
        directives->primary_handle_prefix = "tag:example.com,2000:";

        auto test_pair = GENERATE(
            test_pair_t {"!str", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!<!str>", fkyaml::detail::tag_t::CUSTOM_TAG});

        REQUIRE_NOTHROW(
            tag_type = fkyaml::detail::tag_resolver<fkyaml::node>::resolve_tag(test_pair.first, directives));
        REQUIRE(tag_type == test_pair.second);
    }

    SECTION("valid tag name with non-default secondary handle prefix") {
        directives = std::shared_ptr<fkyaml::detail::document_metainfo<fkyaml::node>>(
            new fkyaml::detail::document_metainfo<fkyaml::node>());
        directives->secondary_handle_prefix = "tag:example.com,2000";

        auto test_pair = GENERATE(
            test_pair_t {"!!seq", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!!map", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!!null", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!!bool", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!!int", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!!float", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!!str", fkyaml::detail::tag_t::CUSTOM_TAG});

        REQUIRE_NOTHROW(
            tag_type = fkyaml::detail::tag_resolver<fkyaml::node>::resolve_tag(test_pair.first, directives));
        REQUIRE(tag_type == test_pair.second);
    }

    SECTION("valid tag name with named handles") {
        directives = std::shared_ptr<fkyaml::detail::document_metainfo<fkyaml::node>>(
            new fkyaml::detail::document_metainfo<fkyaml::node>());
        directives->named_handle_map.emplace("!yaml!", "tag:yaml.org,2002:");
        directives->named_handle_map.emplace("!test0!", "!test-");

        auto test_pair = GENERATE(
            test_pair_t {"!yaml!seq", fkyaml::detail::tag_t::SEQUENCE},
            test_pair_t {"!yaml!map", fkyaml::detail::tag_t::MAPPING},
            test_pair_t {"!yaml!null", fkyaml::detail::tag_t::NULL_VALUE},
            test_pair_t {"!yaml!bool", fkyaml::detail::tag_t::BOOLEAN},
            test_pair_t {"!yaml!int", fkyaml::detail::tag_t::INTEGER},
            test_pair_t {"!yaml!float", fkyaml::detail::tag_t::FLOATING_NUMBER},
            test_pair_t {"!yaml!str", fkyaml::detail::tag_t::STRING},
            test_pair_t {"!test0!foo", fkyaml::detail::tag_t::CUSTOM_TAG});

        REQUIRE_NOTHROW(
            tag_type = fkyaml::detail::tag_resolver<fkyaml::node>::resolve_tag(test_pair.first, directives));
        REQUIRE(tag_type == test_pair.second);
    }

    SECTION("invalid tag name with empty document_metainfo<fkyaml::node>") {
        auto tag = GENERATE(
            fkyaml::detail::str_view(""),
            fkyaml::detail::str_view("invalid"),
            fkyaml::detail::str_view("!invalid!tag"));

        REQUIRE_THROWS_AS(
            fkyaml::detail::tag_resolver<fkyaml::node>::resolve_tag(tag, directives), fkyaml::invalid_tag);
    }

    SECTION("invalid tag name with non-empty document_metainfo<fkyaml::node>") {
        directives = std::shared_ptr<fkyaml::detail::document_metainfo<fkyaml::node>>(
            new fkyaml::detail::document_metainfo<fkyaml::node>());
        directives->named_handle_map.emplace("!valid!", "tag:example.com,2000");

        const fkyaml::detail::str_view tag = "!invalid!tag";

        REQUIRE_THROWS_AS(
            fkyaml::detail::tag_resolver<fkyaml::node>::resolve_tag(tag, directives), fkyaml::invalid_tag);
    }
}
