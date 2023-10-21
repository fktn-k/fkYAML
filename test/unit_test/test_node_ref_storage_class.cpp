//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.1.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <utility>

#include <catch2/catch.hpp>

#include <fkYAML/detail/node_ref_storage.hpp>
#include <fkYAML/node.hpp>

TEST_CASE("NodeRefStorageTest_CtorWithLvalueNodeTest", "[NodeRefStorageTest]")
{
    fkyaml::node node = true;
    fkyaml::detail::node_ref_storage<fkyaml::node> storage(node);
    REQUIRE(node.is_boolean());

    fkyaml::node retrieved_node = storage.release();
    REQUIRE(retrieved_node.is_boolean());
    REQUIRE(retrieved_node.to_boolean() == true);
}

TEST_CASE("NodeRefStorageTest_CtorWithRvalueNodeTest", "[NodeRefStorageTest]")
{
    fkyaml::node node = 3.14;
    fkyaml::detail::node_ref_storage<fkyaml::node> storage(std::move(node));
    REQUIRE(node.is_null());

    fkyaml::node retrieved_node = storage.release();
    REQUIRE(retrieved_node.is_float_number());
    REQUIRE(retrieved_node.to_float_number() == 3.14);
}

TEST_CASE("NodeRefStorageTest_ArrowOperatorTest", "[NodeRefStorageTest]")
{
    fkyaml::node node = 123;
    fkyaml::detail::node_ref_storage<fkyaml::node> storage(node);
    REQUIRE(storage.operator->() == &node);
    REQUIRE(storage->is_integer());
    REQUIRE(storage->to_integer() == 123);

    fkyaml::node node2 = {true, false};
    fkyaml::detail::node_ref_storage<fkyaml::node> storage2(std::move(node2));
    REQUIRE(storage2->is_sequence());
    REQUIRE(storage2->size() == 2);
}

TEST_CASE("NodeRefStorageTest_ReleaseTest", "[NodeRefStorageTest]")
{
    fkyaml::node node = 123;
    fkyaml::detail::node_ref_storage<fkyaml::node> storage(node);
    fkyaml::node released_node = storage.release();
    REQUIRE(&released_node != &node);
    REQUIRE(released_node.is_integer());
    REQUIRE(released_node.to_integer() == 123);

    fkyaml::node node2 = {true, false};
    fkyaml::detail::node_ref_storage<fkyaml::node> storage2(std::move(node2));
    fkyaml::node released_node2 = storage2.release();
    REQUIRE(released_node2.is_sequence());
    REQUIRE(released_node2.size() == 2);
}