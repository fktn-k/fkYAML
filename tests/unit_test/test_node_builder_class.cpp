//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.5.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <doctest/doctest.h>

#include <fkYAML/node.hpp>

TEST_CASE("NodeBuilder_EmptyDocument") {
    fkyaml::node root;
    fkyaml::detail::node_builder<fkyaml::node> builder(root);

    builder.on_stream_start({});
    builder.on_document_start({false});
    builder.on_document_end({false});
    builder.on_stream_end({});

    REQUIRE(root.is_null());
}

TEST_CASE("NodeBuilder_Mapping") {
    fkyaml::node root;
    fkyaml::detail::node_builder<fkyaml::node> builder(root);

    // ```yaml
    // foo: bar
    // ```
    builder.on_stream_start({});
    builder.on_document_start({false});
    builder.on_mapping_start({{}, {}, fkyaml::detail::collection_type::BLOCK});
    builder.on_plain_scalar({"foo", {}, {}});
    builder.on_plain_scalar({"bar", {}, {}});
    builder.on_mapping_end({});
    builder.on_document_end({false});
    builder.on_stream_end({});

    REQUIRE(root.is_mapping());
    REQUIRE(root.size() == 1);
    REQUIRE(root["foo"].is_string());
    REQUIRE(root["foo"].as_str() == "bar");
}

TEST_CASE("NodeBuilder_MappingWithCollectionKeys") {
    fkyaml::node root;
    fkyaml::detail::node_builder<fkyaml::node> builder(root);

    // ```yaml
    // [foo]: bar
    // {baz: qux}: quux
    // ```
    builder.on_stream_start({});
    builder.on_document_start({false});
    builder.on_mapping_start({{}, {}, fkyaml::detail::collection_type::BLOCK});
    builder.on_sequence_start({{}, {}, fkyaml::detail::collection_type::FLOW});
    builder.on_plain_scalar({"foo", {}, {}});
    builder.on_sequence_end({});
    builder.on_plain_scalar({"bar", {}, {}});
    builder.on_mapping_start({{}, {}, fkyaml::detail::collection_type::FLOW});
    builder.on_plain_scalar({"baz", {}, {}});
    builder.on_plain_scalar({"qux", {}, {}});
    builder.on_mapping_end({});
    builder.on_plain_scalar({"quux", {}, {}});
    builder.on_mapping_end({});
    builder.on_document_end({false});
    builder.on_stream_end({});

    REQUIRE(root.is_mapping());
    REQUIRE(root.size() == 2);

    auto itr = root.as_map().begin();
    REQUIRE(itr->first.is_sequence());
    REQUIRE(itr->first[0].as_str() == "foo");
    REQUIRE(itr->second.as_str() == "bar");

    ++itr;
    REQUIRE(itr->first.is_mapping());
    REQUIRE(itr->first["baz"].as_str() == "qux");
    REQUIRE(itr->second.as_str() == "quux");
}

TEST_CASE("NodeBuilder_MappingWithCollectionValues") {
    fkyaml::node root;
    fkyaml::detail::node_builder<fkyaml::node> builder(root);

    // ```yaml
    // foo: [bar]
    // baz: {qux: quux}
    // ```
    builder.on_stream_start({});
    builder.on_document_start({false});
    builder.on_mapping_start({{}, {}, fkyaml::detail::collection_type::BLOCK});
    builder.on_plain_scalar({"foo", {}, {}});
    builder.on_sequence_start({{}, {}, fkyaml::detail::collection_type::FLOW});
    builder.on_plain_scalar({"bar", {}, {}});
    builder.on_sequence_end({});
    builder.on_plain_scalar({"baz", {}, {}});
    builder.on_mapping_start({{}, {}, fkyaml::detail::collection_type::FLOW});
    builder.on_plain_scalar({"qux", {}, {}});
    builder.on_plain_scalar({"quux", {}, {}});
    builder.on_mapping_end({});
    builder.on_mapping_end({});
    builder.on_document_end({false});
    builder.on_stream_end({});

    REQUIRE(root.is_mapping());
    REQUIRE(root.size() == 2);
    REQUIRE(root["foo"].is_sequence());
    REQUIRE(root["foo"][0].as_str() == "bar");
    REQUIRE(root["baz"].is_mapping());
    REQUIRE(root["baz"]["qux"].as_str() == "quux");
}

TEST_CASE("NodeBuilder_Sequence") {
    fkyaml::node root;
    fkyaml::detail::node_builder<fkyaml::node> builder(root);

    // ```yaml
    // - []
    // - {}
    // - null
    // - true
    // - 123
    // - 3.14
    // - - &anchor foo
    //   - 'foo'
    //   - "foo"
    //   - >-
    //     foo
    //   - |-
    //     foo
    // - *anchor
    // ```
    builder.on_stream_start({});
    builder.on_stream_end({});
    builder.on_document_start({false});
    builder.on_sequence_start({{}, {}, fkyaml::detail::collection_type::BLOCK});
    builder.on_sequence_start({{}, {}, fkyaml::detail::collection_type::FLOW});
    builder.on_sequence_end({});
    builder.on_mapping_start({{}, {}, fkyaml::detail::collection_type::FLOW});
    builder.on_mapping_end({});
    builder.on_plain_scalar({"null", {}, {}});
    builder.on_plain_scalar({"true", {}, {}});
    builder.on_plain_scalar({"123", {}, {}});
    builder.on_plain_scalar({"3.14", {}, {}});
    builder.on_sequence_start({{}, {}, fkyaml::detail::collection_type::BLOCK});
    builder.on_plain_scalar({"foo", {}, "anchor"});
    builder.on_single_quoted_scalar({"foo", {}, {}});
    builder.on_double_quoted_scalar({"foo", {}, {}});
    builder.on_literal_scalar({"    foo\n", {}, {}, {fkyaml::detail::chomping_indicator_t::STRIP, 4}});
    builder.on_folded_scalar({"    foo\n", {}, {}, {fkyaml::detail::chomping_indicator_t::STRIP, 4}});
    builder.on_sequence_end({});
    builder.on_alias({"anchor"});
    builder.on_sequence_end({});
    builder.on_document_end({false});
    builder.on_stream_end({});

    REQUIRE(root.is_sequence());
    REQUIRE(root.size() == 8);
    REQUIRE(root[0].is_sequence());
    REQUIRE(root[0].empty());
    REQUIRE(root[1].is_mapping());
    REQUIRE(root[1].empty());
    REQUIRE(root[2].is_null());
    REQUIRE(root[3].is_boolean());
    REQUIRE(root[3].as_bool() == true);
    REQUIRE(root[4].is_integer());
    REQUIRE(root[4].as_int() == 123);
    REQUIRE(root[5].is_float_number());
    REQUIRE(root[5].as_float() == doctest::Approx(3.14));
    REQUIRE(root[6].is_sequence());
    REQUIRE(root[6].size() == 5);
    REQUIRE(root[6][0].is_anchor());
    REQUIRE(root[6][0].get_anchor_name() == "anchor");
    REQUIRE(root[6][0].is_string());
    REQUIRE(root[6][0].as_str() == "foo");
    REQUIRE(root[6][1].is_string());
    REQUIRE(root[6][1].as_str() == "foo");
    REQUIRE(root[6][2].is_string());
    REQUIRE(root[6][2].as_str() == "foo");
    REQUIRE(root[6][3].is_string());
    REQUIRE(root[6][3].as_str() == "foo");
    REQUIRE(root[6][4].is_string());
    REQUIRE(root[6][4].as_str() == "foo");
    REQUIRE(root[7].is_alias());
    REQUIRE(root[7].get_anchor_name() == "anchor");
    REQUIRE(root[7].is_string());
    REQUIRE(root[7].as_str() == "foo");
}
