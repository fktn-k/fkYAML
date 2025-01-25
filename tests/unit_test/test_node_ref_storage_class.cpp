//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <utility>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("NodeRefStorage_CtorWithLvalueNode") {
    fkyaml::node node = true;
    fkyaml::detail::node_ref_storage<fkyaml::node> storage(node);
    REQUIRE(node.is_boolean());

    fkyaml::node retrieved_node = storage.release();
    REQUIRE(retrieved_node.is_boolean());
    REQUIRE(retrieved_node.get_value<fkyaml::node::boolean_type>() == true);
}

TEST_CASE("NodeRefStorage_CtorWithRvalueNode") {
    fkyaml::node node = 3.14;
    fkyaml::detail::node_ref_storage<fkyaml::node> storage(std::move(node));
    REQUIRE(node.is_null());

    fkyaml::node retrieved_node = storage.release();
    REQUIRE(retrieved_node.is_float_number());
    REQUIRE(retrieved_node.get_value<fkyaml::node::float_number_type>() == 3.14);
}

TEST_CASE("NodeRefStorage_ArrowOperator") {
    fkyaml::node node = 123;
    fkyaml::detail::node_ref_storage<fkyaml::node> storage(node);
    REQUIRE(storage->is_integer());
    REQUIRE(storage->get_value<fkyaml::node::integer_type>() == 123);

    fkyaml::node node2 = {true, false};
    fkyaml::detail::node_ref_storage<fkyaml::node> storage2(std::move(node2));
    REQUIRE(storage2->is_sequence());
    REQUIRE(storage2->size() == 2);
}

TEST_CASE("NodeRefStorage_Release") {
    fkyaml::node node = 123;
    fkyaml::detail::node_ref_storage<fkyaml::node> storage(node);
    fkyaml::node released_node = storage.release();
    REQUIRE(&released_node != &node);
    REQUIRE(released_node.is_integer());
    REQUIRE(released_node.get_value<fkyaml::node::integer_type>() == 123);

    fkyaml::node node2 = {true, false};
    fkyaml::detail::node_ref_storage<fkyaml::node> storage2(std::move(node2));
    fkyaml::node released_node2 = storage2.release();
    REQUIRE(released_node2.is_sequence());
    REQUIRE(released_node2.size() == 2);
}
