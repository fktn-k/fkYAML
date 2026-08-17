//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <memory>
#include <utility>
#include <vector>

#include <doctest/doctest.h>

#include <fkYAML/node.hpp>

#include "event.hpp"
#include "validation_compositor.hpp"

namespace {

yaml_test_suite_runner::event make_event(
    yaml_test_suite_runner::event_type type, std::initializer_list<yaml_test_suite_runner::event_param> params = {}) {
    return yaml_test_suite_runner::event(type, std::vector<yaml_test_suite_runner::event_param>(params));
}

} // namespace

TEST_CASE("validation_compositor_builds_implicit_integer_validator") {
    using namespace yaml_test_suite_runner;

    validation_compositor<fkyaml::node> compositor;
    compositor.handle_event(event(event_type::STREAM_START));
    compositor.handle_event(event(event_type::DOCUMENT_START));
    compositor.handle_event(make_event(event_type::SCALAR, {{event_param_type::VALUE, ":42"}}));
    compositor.handle_event(event(event_type::DOCUMENT_END));
    compositor.handle_event(event(event_type::STREAM_END));

    std::unique_ptr<validator<fkyaml::node>> validator_ptr = compositor.take_validator();

    CHECK_NOTHROW(validator_ptr->validate(fkyaml::node(42)));
    CHECK_THROWS_AS(validator_ptr->validate(fkyaml::node(std::string("42"))), validation_error);
}

TEST_CASE("validation_compositor_validates_tagged_scalar_with_normalized_tag_name") {
    using namespace yaml_test_suite_runner;

    validation_compositor<fkyaml::node> compositor;
    compositor.handle_event(event(event_type::STREAM_START));
    compositor.handle_event(event(event_type::DOCUMENT_START));
    compositor.handle_event(make_event(
        event_type::SCALAR, {{event_param_type::TAG, "tag:yaml.org,2002:str"}, {event_param_type::VALUE, ":42"}}));
    compositor.handle_event(event(event_type::DOCUMENT_END));
    compositor.handle_event(event(event_type::STREAM_END));

    std::unique_ptr<validator<fkyaml::node>> validator_ptr = compositor.take_validator();

    fkyaml::node tagged_scalar(std::string("42"));
    tagged_scalar.add_tag_name("!!str");
    CHECK_NOTHROW(validator_ptr->validate(tagged_scalar));
}

TEST_CASE("validation_compositor_validates_nested_collections_and_aliases") {
    using namespace yaml_test_suite_runner;

    validation_compositor<fkyaml::node> compositor;
    compositor.handle_event(event(event_type::STREAM_START));
    compositor.handle_event(event(event_type::DOCUMENT_START));
    compositor.handle_event(event(event_type::MAPPING_START));
    compositor.handle_event(make_event(event_type::SCALAR, {{event_param_type::VALUE, ":items"}}));
    compositor.handle_event(make_event(event_type::SEQUENCE_START, {{event_param_type::ANCHOR, "seq"}}));
    compositor.handle_event(make_event(event_type::SCALAR, {{event_param_type::VALUE, ":1"}}));
    compositor.handle_event(make_event(event_type::SCALAR, {{event_param_type::VALUE, "\"two"}}));
    compositor.handle_event(event(event_type::SEQUENCE_END));
    compositor.handle_event(make_event(event_type::SCALAR, {{event_param_type::VALUE, ":ref"}}));
    compositor.handle_event(make_event(event_type::ALIAS, {{event_param_type::ALIAS, "seq"}}));
    compositor.handle_event(event(event_type::MAPPING_END));
    compositor.handle_event(event(event_type::DOCUMENT_END));
    compositor.handle_event(event(event_type::STREAM_END));

    std::unique_ptr<validator<fkyaml::node>> validator_ptr = compositor.take_validator();

    fkyaml::node items = fkyaml::node::sequence({fkyaml::node(1), fkyaml::node(std::string("two"))});
    items.add_anchor_name("seq");

    fkyaml::node root = fkyaml::node::mapping();
    root["items"] = items;
    root["ref"] = fkyaml::node::alias_of(root["items"]);

    CHECK_NOTHROW(validator_ptr->validate(root));

    root["ref"] = fkyaml::node(std::string("not-an-alias"));
    CHECK_THROWS_AS(validator_ptr->validate(root), validation_error);
}

TEST_CASE("validation_compositor_validates_non_scalar_mapping_keys") {
    using namespace yaml_test_suite_runner;

    validation_compositor<fkyaml::node> compositor;
    compositor.handle_event(event(event_type::STREAM_START));
    compositor.handle_event(event(event_type::DOCUMENT_START));
    compositor.handle_event(event(event_type::MAPPING_START));
    compositor.handle_event(event(event_type::SEQUENCE_START));
    compositor.handle_event(make_event(event_type::SCALAR, {{event_param_type::VALUE, ":a"}}));
    compositor.handle_event(make_event(event_type::SCALAR, {{event_param_type::VALUE, ":b"}}));
    compositor.handle_event(event(event_type::SEQUENCE_END));
    compositor.handle_event(make_event(event_type::SCALAR, {{event_param_type::VALUE, ":pair"}}));
    compositor.handle_event(event(event_type::MAPPING_END));
    compositor.handle_event(event(event_type::DOCUMENT_END));
    compositor.handle_event(event(event_type::STREAM_END));

    std::unique_ptr<validator<fkyaml::node>> validator_ptr = compositor.take_validator();

    fkyaml::node key = fkyaml::node::sequence({fkyaml::node(std::string("a")), fkyaml::node(std::string("b"))});
    fkyaml::node root = fkyaml::node::mapping();
    root[key] = fkyaml::node(std::string("pair"));

    CHECK_NOTHROW(validator_ptr->validate(root));
}
