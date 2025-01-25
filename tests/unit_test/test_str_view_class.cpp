//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <cstring>
#include <string>
#include <sstream>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("StrView_DefaultCtor") {
    fkyaml::detail::str_view sv;
    REQUIRE(sv.data() == nullptr);
    REQUIRE(sv.size() == 0);
    REQUIRE(sv.length() == 0);
    REQUIRE(sv.empty());
}

TEST_CASE("StrView_CharPtrCtor") {
    const char str[] = "hello world!";
    const char* p_str = &str[0];
    fkyaml::detail::str_view sv(p_str);

    REQUIRE(sv.compare("hello world!") == 0);
    REQUIRE(sv.size() == 12);
    REQUIRE(sv.length() == 12);
    REQUIRE(sv.begin() == &str[0]);
    REQUIRE(sv.end() == &str[0] + 12);
}

TEST_CASE("StrView_StdStringCtor") {
    std::string str = "hello world!";
    fkyaml::detail::str_view sv(str);

    REQUIRE(sv.compare("hello world!") == 0);
    REQUIRE(sv.size() == 12);
    REQUIRE(sv.length() == 12);
    REQUIRE(sv.begin() == str.data());
    REQUIRE(sv.end() == str.data() + 12);
}

TEST_CASE("StrView_CopyCtor") {
    const char str[] = "hello world!";
    fkyaml::detail::str_view view = str;
    fkyaml::detail::str_view sv(view);

    REQUIRE(sv.compare("hello world!") == 0);
    REQUIRE(sv.size() == 12);
    REQUIRE(sv.length() == 12);
    REQUIRE(sv.begin() == &str[0]);
    REQUIRE(sv.end() == &str[0] + 12);
}

TEST_CASE("StrView_MoveCtor") {
    const char str[] = "hello world!";
    fkyaml::detail::str_view view = str;
    fkyaml::detail::str_view sv(std::move(view));

    REQUIRE(sv.compare("hello world!") == 0);
    REQUIRE(sv.size() == 12);
    REQUIRE(sv.length() == 12);
    REQUIRE(sv.begin() == &str[0]);
    REQUIRE(sv.end() == &str[0] + 12);
}

TEST_CASE("StrView_CopyAssignmentOperator") {
    const char str[] = "hello world!";
    fkyaml::detail::str_view view = str;
    fkyaml::detail::str_view sv {};
    sv = view;

    REQUIRE(sv.compare("hello world!") == 0);
    REQUIRE(sv.size() == 12);
    REQUIRE(sv.length() == 12);
    REQUIRE(sv.begin() == &str[0]);
    REQUIRE(sv.end() == &str[0] + 12);
}

TEST_CASE("StrView_MoveAssignmentOperator") {
    const char str[] = "hello world!";
    fkyaml::detail::str_view view = str;
    fkyaml::detail::str_view sv {};
    sv = std::move(view);

    REQUIRE(sv.compare("hello world!") == 0);
    REQUIRE(sv.size() == 12);
    REQUIRE(sv.length() == 12);
    REQUIRE(sv.begin() == &str[0]);
    REQUIRE(sv.end() == &str[0] + 12);
}

TEST_CASE("StrView_ReverseIterators") {
    const char str[] = "hello world!";
    fkyaml::detail::str_view sv = str;

    REQUIRE(&*sv.rbegin() == &str[11]);
    REQUIRE(sv.rend().operator->() + 1 == &str[0]);
    REQUIRE(&*sv.crbegin() == &str[11]);
    REQUIRE(sv.crend().operator->() + 1 == &str[0]);
}

TEST_CASE("StrView_MaxSize") {
    fkyaml::detail::str_view sv = "foo";
    REQUIRE(sv.max_size() == static_cast<size_t>(std::numeric_limits<ptrdiff_t>::max()));
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
    char buffer[] = "aaaaaaaaaa";

    sv.copy(buffer, 4);
    REQUIRE(std::strncmp(buffer, "xxxxaaaaaa", sizeof(buffer)) == 0);

    char buffer1[] = "bbbbbbbbbbbbbbbbbbbb";
    sv.copy(buffer1, 20 /*intentionally too large*/);
    REQUIRE(std::strncmp(buffer1, "xxxxxxxxxxbbbbbbbbbb", sizeof(buffer1)) == 0);

    REQUIRE_THROWS_AS(sv.copy(buffer, 4, sv.length() + 1), fkyaml::out_of_range);
}

TEST_CASE("StrView_Substr") {
    fkyaml::detail::str_view sv = "xxxxxxxxxx";

    REQUIRE(sv.substr(4) == "xxxxxx");
    REQUIRE(sv.substr(3, 2) == "xx");
    REQUIRE_THROWS_AS(sv.substr(20), fkyaml::out_of_range);
}

TEST_CASE("StrView_Compare") {
    fkyaml::detail::str_view sv = "a";
    REQUIRE(sv.compare("a") == 0);
    REQUIRE(sv.compare("b") == -1);
    REQUIRE(sv.compare("`") == 1);

    // skip checks of comparisons with a large string object if int == ptrdiff_t
    if (static_cast<std::ptrdiff_t>(std::numeric_limits<int>::max()) < std::numeric_limits<std::ptrdiff_t>::max()) {
        constexpr std::size_t long_str_size = static_cast<std::size_t>(std::numeric_limits<int>::max()) + 4u;
        char* p_long_str = (char*)std::malloc(long_str_size);
        for (std::size_t i = 0; i < long_str_size - 1; i++) {
            p_long_str[i] = 'a';
        }
        p_long_str[long_str_size - 1] = '\0';
        fkyaml::detail::str_view view(p_long_str, long_str_size - 1);

        REQUIRE(sv.compare(p_long_str) == std::numeric_limits<int>::min());
        fkyaml::detail::str_view view_4a = "aaaa";
        REQUIRE(view.compare(long_str_size - 1u - 4u, 4u, view_4a.data()) == 0);
        REQUIRE(view.compare(long_str_size - 1u - 4u, 4u, view_4a.data(), view_4a.size()) == 0);
        REQUIRE(view.compare(long_str_size - 1u - 4u, 4u, view_4a, 0, view_4a.size()) == 0);
        REQUIRE(view.compare(long_str_size - 1u - 4u, 4u, view_4a) == 0);
        REQUIRE(sv.compare(view) == std::numeric_limits<int>::min());
        REQUIRE(view.compare(sv) == std::numeric_limits<int>::max());

        std::free(p_long_str);
        p_long_str = nullptr;
    }
}

TEST_CASE("StrView_StartsWith") {
    fkyaml::detail::str_view view_4a = "aaaa";
    fkyaml::detail::str_view sv = "aaaa";

    REQUIRE(sv.starts_with(view_4a));
    REQUIRE(sv.starts_with('a'));
    REQUIRE(sv.starts_with(view_4a.data()));
}

TEST_CASE("StrView_EndsWith") {
    fkyaml::detail::str_view view_4a = "aaaa";
    fkyaml::detail::str_view sv = "aaaa";

    REQUIRE(sv.ends_with(view_4a));
    REQUIRE(sv.ends_with('a'));
    REQUIRE(sv.ends_with(view_4a.data()));
}

TEST_CASE("StrView_Contains") {
    fkyaml::detail::str_view sv = "aaaa";
    fkyaml::detail::str_view view = "aa";

    REQUIRE(sv.contains(view));
    REQUIRE(sv.contains("a"));
    REQUIRE(sv.contains('a'));
}

TEST_CASE("StrView_Find") {
    fkyaml::detail::str_view sv = "aaaabbcd";

    REQUIRE(sv.find('a', 0) == 0);
    REQUIRE(sv.find('a', sv.size()) == fkyaml::detail::str_view::npos);
    REQUIRE(sv.find('e', 0) == fkyaml::detail::str_view::npos);

    REQUIRE(sv.find("aa", 0, 2) == 0);
    REQUIRE(sv.find("aa", 2, 2) == 2);
    REQUIRE(sv.find("aa", 2, 0) == 2);
    REQUIRE(sv.find("aa", sv.size() + 1, 0) == fkyaml::detail::str_view::npos);
    REQUIRE(sv.find("aa", sv.size() + 1, 2) == fkyaml::detail::str_view::npos);
    REQUIRE(sv.find("bcd", 0, 3) == 5);
    REQUIRE(sv.find("efg", 0, 3) == fkyaml::detail::str_view::npos);
    REQUIRE(sv.find("aaaabbcde", 0, 9) == fkyaml::detail::str_view::npos);
}

TEST_CASE("StrView_ReverseFind") {
    fkyaml::detail::str_view sv = "aaaabbcd";

    REQUIRE(sv.rfind('a') == 3);
    REQUIRE(sv.rfind('a', 0) == 0);
    REQUIRE(sv.rfind('e') == fkyaml::detail::str_view::npos);

    fkyaml::detail::str_view empty_sv = "";
    REQUIRE(empty_sv.rfind('a', 0));

    REQUIRE(sv.rfind("aaaa", 0) == 0);
    REQUIRE(sv.rfind("aaaa", 0, 4) == 0);
    REQUIRE(sv.rfind("aaaa", 0, sv.size() + 1) == fkyaml::detail::str_view::npos);
    REQUIRE(sv.rfind("efg", 2, 3) == fkyaml::detail::str_view::npos);
}

TEST_CASE("StrView_FindFirstOf") {
    fkyaml::detail::str_view sv = "aaaabbcd";

    REQUIRE(sv.find_first_of('b', 2) == 4);
    REQUIRE(sv.find_first_of("abc") == 0);
    REQUIRE(sv.find_first_of("abc", 0, 3) == 0);
    REQUIRE(sv.find_first_of("abc", 0, 0) == fkyaml::detail::str_view::npos);
    REQUIRE(sv.find_first_of("efg", 0, 3) == fkyaml::detail::str_view::npos);
    REQUIRE(sv.find_first_of(fkyaml::detail::str_view {"abc"}) == 0);
}

TEST_CASE("StrView_FindLastOf") {
    fkyaml::detail::str_view sv = "aaaabbcd";

    REQUIRE(sv.find_last_of('a') == 3);
    REQUIRE(sv.find_last_of("a") == 3);
    REQUIRE(sv.find_last_of("a", 0, 1) == 0);
    REQUIRE(sv.find_last_of("a", 0, sv.size() + 1) == fkyaml::detail::str_view::npos);
    REQUIRE(sv.find_last_of(fkyaml::detail::str_view {"aa"}) == 3);
}

TEST_CASE("StrView_FindFirstNotOf") {
    fkyaml::detail::str_view sv = "aaaabbcd";

    REQUIRE(sv.find_first_not_of('a') == 4);
    REQUIRE(sv.find_first_not_of('a', sv.size()) == fkyaml::detail::str_view::npos);

    REQUIRE(sv.find_first_not_of("a") == 4);
    REQUIRE(sv.find_first_not_of("a", 3) == 4);
    REQUIRE(sv.find_first_not_of("a", sv.size()) == fkyaml::detail::str_view::npos);
    REQUIRE(sv.find_first_not_of(fkyaml::detail::str_view {"a"}) == 4);
}

TEST_CASE("StrView_FindLastNotOf") {
    fkyaml::detail::str_view sv = "aaaabbcd";

    REQUIRE(sv.find_last_not_of('d') == 6);
    REQUIRE(sv.find_last_not_of('b', 5) == 3);
    fkyaml::detail::str_view empty_sv = "";
    REQUIRE(empty_sv.find_last_not_of('b') == fkyaml::detail::str_view::npos);

    REQUIRE(sv.find_last_not_of("bcd") == 3);
    REQUIRE(sv.find_last_not_of("bcd", 5) == 3);
    REQUIRE(empty_sv.find_last_not_of("bcd") == fkyaml::detail::str_view::npos);
    REQUIRE(sv.find_last_not_of(fkyaml::detail::str_view {"bcd"}) == 3);
}

TEST_CASE("StrView_EqualToOperator") {
    fkyaml::detail::str_view sv0 = "abc";
    fkyaml::detail::str_view sv1 = "abc";
    std::string str = "abc";
    const char arr[] = "abc";
    REQUIRE(sv0 == sv1);
    REQUIRE(sv0 == str);
    REQUIRE(str == sv0);
    REQUIRE(sv0 == arr);
    REQUIRE(arr == sv0);
}

TEST_CASE("StrView_NotEqualToOperator") {
    fkyaml::detail::str_view sv0 = "abc";
    fkyaml::detail::str_view sv1 = "edf";
    std::string str = "edf";
    const char arr[] = "edf";
    REQUIRE(sv0 != sv1);
    REQUIRE(sv0 != str);
    REQUIRE(str != sv0);
    REQUIRE(sv0 != arr);
    REQUIRE(arr != sv0);
}

TEST_CASE("StrView_LessThanOperator") {
    fkyaml::detail::str_view sv = "a";
    REQUIRE_FALSE(sv < fkyaml::detail::str_view {"a"});
    REQUIRE(sv < fkyaml::detail::str_view {"b"});
    REQUIRE_FALSE(sv < fkyaml::detail::str_view {"`"});
}

TEST_CASE("StrView_LessThanOrEqualToOperator") {
    fkyaml::detail::str_view sv = "a";
    REQUIRE(sv <= fkyaml::detail::str_view {"a"});
    REQUIRE(sv <= fkyaml::detail::str_view {"b"});
    REQUIRE_FALSE(sv <= fkyaml::detail::str_view {"`"});
}

TEST_CASE("StrView_GreaterThanOperator") {
    fkyaml::detail::str_view sv = "a";
    REQUIRE_FALSE(sv > fkyaml::detail::str_view {"a"});
    REQUIRE_FALSE(sv > fkyaml::detail::str_view {"b"});
    REQUIRE(sv > fkyaml::detail::str_view {"`"});
}

TEST_CASE("StrView_GreaterThanOrEqualToOperator") {
    fkyaml::detail::str_view sv = "a";
    REQUIRE(sv >= fkyaml::detail::str_view {"a"});
    REQUIRE_FALSE(sv >= fkyaml::detail::str_view {"b"});
    REQUIRE(sv >= fkyaml::detail::str_view {"`"});
}

TEST_CASE("StrView_InsertionOperator") {
    std::ostringstream ss {};
    fkyaml::detail::str_view sv = "abc";
    REQUIRE(ss.str().empty());
    ss << sv;
    REQUIRE(ss.str() == "abc");
}
