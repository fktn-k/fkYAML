//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.5.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <doctest/doctest.h>

#include <fkYAML/detail/chunked_arena.hpp>

TEST_CASE("ChunkedArena_AllocateAndAccess") {
    fkyaml::detail::chunked_arena<int, 2> arena;

    const auto first_id = arena.allocate();
    const auto second_id = arena.allocate();
    const auto third_id = arena.allocate();

    REQUIRE(first_id == 0);
    REQUIRE(second_id == 1);
    REQUIRE(third_id == 2);

    arena.get(first_id) = 10;
    arena.get(second_id) = 20;
    arena.get(third_id) = 30;

    const fkyaml::detail::chunked_arena<int, 2>& const_arena = arena;
    REQUIRE(const_arena.get(first_id) == 10);
    REQUIRE(const_arena.get(second_id) == 20);
    REQUIRE(const_arena.get(third_id) == 30);
}

TEST_CASE("ChunkedArena_DeallocateReusesIdsInReverseOrder") {
    fkyaml::detail::chunked_arena<int, 2> arena;

    const auto first_id = arena.allocate();
    const auto second_id = arena.allocate();
    const auto third_id = arena.allocate();
    arena.get(first_id) = 10;
    arena.get(second_id) = 20;
    arena.get(third_id) = 30;

    arena.deallocate(first_id);
    arena.deallocate(third_id);

    REQUIRE(arena.allocate() == third_id);
    REQUIRE(arena.allocate() == first_id);
    REQUIRE(arena.get(second_id) == 20);
}

TEST_CASE("ChunkedArena_MoveOperationsPreserveAllocatedElements") {
    fkyaml::detail::chunked_arena<int, 2> source;
    const auto first_id = source.allocate();
    const auto second_id = source.allocate();
    source.get(first_id) = 10;
    source.get(second_id) = 20;

    fkyaml::detail::chunked_arena<int, 2> moved(std::move(source));
    REQUIRE(moved.get(first_id) == 10);
    REQUIRE(moved.get(second_id) == 20);

    fkyaml::detail::chunked_arena<int, 2> assigned;
    assigned = std::move(moved);
    REQUIRE(assigned.get(first_id) == 10);
    REQUIRE(assigned.get(second_id) == 20);
}
