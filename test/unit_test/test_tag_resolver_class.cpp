//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#ifdef FK_YAML_TEST_USE_SINGLE_HEADER
    #include <fkYAML/node.hpp>
#else
    #include <fkYAML/detail/input/tag_resolver.hpp>
#endif

TEST_CASE("TagResolverClassTest_ResolveTag", "[TagResolverClassTest]")
{
    using test_pair_t = std::pair<std::string, fkyaml::detail::tag_t>;

    fkyaml::detail::tag_t tag_type {};
    std::shared_ptr<fkyaml::detail::directive_set> directives {};

    SECTION("valid tag name with default tag handle prefixes")
    {
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

        REQUIRE_NOTHROW(tag_type = fkyaml::detail::tag_resolver::resolve_tag(test_pair.first, directives));
        REQUIRE(tag_type == test_pair.second);
    }

    SECTION("valid tag name with non-default primary handle prefix")
    {
        directives = std::shared_ptr<fkyaml::detail::directive_set>(new fkyaml::detail::directive_set());
        directives->primary_handle_prefix = "tag:example.com,2000:";

        auto test_pair = GENERATE(
            test_pair_t {"!str", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!<!str>", fkyaml::detail::tag_t::CUSTOM_TAG}
        );

        REQUIRE_NOTHROW(tag_type = fkyaml::detail::tag_resolver::resolve_tag(test_pair.first, directives));
        REQUIRE(tag_type == test_pair.second);
    }

    SECTION("valid tag name with non-default secondary handle prefix")
    {
        directives = std::shared_ptr<fkyaml::detail::directive_set>(new fkyaml::detail::directive_set());
        directives->secondary_handle_prefix = "tag:example.com,2000";

        auto test_pair = GENERATE(
            test_pair_t {"!!seq", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!!map", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!!null", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!!bool", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!!int", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!!float", fkyaml::detail::tag_t::CUSTOM_TAG},
            test_pair_t {"!!str", fkyaml::detail::tag_t::CUSTOM_TAG});

        REQUIRE_NOTHROW(tag_type = fkyaml::detail::tag_resolver::resolve_tag(test_pair.first, directives));
        REQUIRE(tag_type == test_pair.second);
    }

    SECTION("valid tag name with named handles")
    {
        directives = std::shared_ptr<fkyaml::detail::directive_set>(new fkyaml::detail::directive_set());
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

        REQUIRE_NOTHROW(tag_type = fkyaml::detail::tag_resolver::resolve_tag(test_pair.first, directives));
        REQUIRE(tag_type == test_pair.second);
    }

    SECTION("invalid tag name with empty directive_set")
    {
        auto tag = GENERATE(
            std::string(""),
            std::string("invalid"),
            std::string("!invalid!tag")
        );

        REQUIRE_THROWS_AS(fkyaml::detail::tag_resolver::resolve_tag(tag, directives), fkyaml::invalid_tag);
    }

    SECTION("invalid tag name with non-empty directive_set")
    {
        directives = std::shared_ptr<fkyaml::detail::directive_set>(new fkyaml::detail::directive_set());
        directives->named_handle_map.emplace("!valid!", "tag:example.com,2000");

        auto tag = GENERATE(
            std::string("!invalid!tag")
        );

        REQUIRE_THROWS_AS(fkyaml::detail::tag_resolver::resolve_tag(tag, directives), fkyaml::invalid_tag);
    }
}
