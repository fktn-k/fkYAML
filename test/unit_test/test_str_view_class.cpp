//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.11
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <string>

#include <catch2/catch.hpp>

#include <fkYAML/detail/str_view.hpp>

TEST_CASE("StrView_GeneralUseCase") {
    const char str[] = "hello world!";
    fkyaml::detail::str_view sv = str;
    REQUIRE(sv.compare("hello world!") == 0);
    REQUIRE(sv.size() == 12);
    REQUIRE(sv.length() == 12);
    REQUIRE(sv.begin() == &str[0]);
    REQUIRE(sv.end() == &str[12]);

    std::string str2 = "HELLO WORLD!";
    sv = str2;
    REQUIRE(sv.compare("HELLO WORLD!") == 0);
    REQUIRE(sv.size() == 12);
    REQUIRE(sv.length() == 12);
    REQUIRE(sv.begin() == str2.data());
    REQUIRE(sv.end() == str2.data() + 12);

    fkyaml::detail::str_view sv2 = sv;
    REQUIRE(sv2.compare("HELLO WORLD!") == 0);
    REQUIRE(sv2.size() == 12);
    REQUIRE(sv2.length() == 12);
    REQUIRE(sv2.begin() == str2.data());
    REQUIRE(sv2.end() == str2.data() + 12);
}

TEST_CASE("StrView_DefaultCtor") {
    fkyaml::detail::str_view sv;
    REQUIRE(sv.data() == nullptr);
    REQUIRE(sv.size() == 0);
    REQUIRE(sv.length() == 0);
    REQUIRE(sv.empty());
}

TEST_CASE("StrView_MaxSize") {
    fkyaml::detail::str_view sv = "foo";
    REQUIRE(sv.max_size() == std::numeric_limits<ptrdiff_t>::max());
}

TEST_CASE("StrView_SubscriptOperator") {
    fkyaml::detail::str_view sv = "foo";
    REQUIRE(sv.size() == 3);
    REQUIRE(sv[0] == 'f');
    REQUIRE(sv[1] == 'o');
    REQUIRE(sv[2] == 'o');
}

TEST_CASE("StrView_At") {
    fkyaml::detail::str_view sv = "foo";
    REQUIRE(sv.size() == 3);
    REQUIRE(sv.at(0) == 'f');
    REQUIRE(sv.at(1) == 'o');
    REQUIRE(sv.at(2) == 'o');
    REQUIRE_THROWS_AS(sv.at(3), fkyaml::out_of_range);
    REQUIRE_THROWS_AS(sv.at(4), fkyaml::out_of_range);
}

TEST_CASE("StrView_Front") {
    const char str[] = "foo";
    fkyaml::detail::str_view sv = str;
    REQUIRE(sv.front() == 'f');
    REQUIRE(&sv.front() == &str[0]);
}

TEST_CASE("StrView_Back") {
    const char str[] = "bar";
    fkyaml::detail::str_view sv = str;
    REQUIRE(sv.back() == 'r');
    REQUIRE(&sv.back() == &str[2]);
}

TEST_CASE("StrView_Data") {
    const char str[] = "bar";
    fkyaml::detail::str_view sv = str;
    REQUIRE(sv.data() == str);

    fkyaml::detail::str_view sv2 {};
    REQUIRE(sv2.data() == nullptr);
}

TEST_CASE("StrView_RemovePrefix") {
    const char str[] = "bar";
    fkyaml::detail::str_view sv = str;
    REQUIRE(sv.data() == str);
    REQUIRE(sv.size() == 3);
    sv.remove_prefix(1);
    REQUIRE(sv.data() == str + 1);
    REQUIRE(sv.size() == 2);
}

TEST_CASE("StrView_RemoveSuffix") {
    const char str[] = "bar";
    fkyaml::detail::str_view sv = str;
    REQUIRE(sv.data() == str);
    REQUIRE(sv.size() == 3);
    sv.remove_suffix(1);
    REQUIRE(sv.data() == str);
    REQUIRE(sv.size() == 2);
}

TEST_CASE("StrView_Swap") {
    fkyaml::detail::str_view sv = "foo";
    fkyaml::detail::str_view sv2 = "barr";
    const char* p_sv_data = sv.data();
    const char* p_sv2_data = sv2.data();

    sv.swap(sv2);
    REQUIRE(sv.size() == 4);
    REQUIRE(sv.data() == p_sv2_data);
    REQUIRE(sv2.size() == 3);
    REQUIRE(sv2.data() == p_sv_data);
}

TEST_CASE("StrView_Copy") {
    fkyaml::detail::str_view sv = "xxxxxxxxxx";
    std::string buffer(10, 'a');

    sv.copy(buffer.data(), 4);
    REQUIRE(buffer == "xxxxaaaaaa");

    buffer = "bbbbbbbbbbbbbbbbbbbb";
    sv.copy(buffer.data(), 20 /*intentionally too large*/);
    REQUIRE(buffer == "xxxxxxxxxxbbbbbbbbbb");
}
