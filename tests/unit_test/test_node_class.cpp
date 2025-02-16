//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <cmath>
#include <cfloat>
#include <deque>
#include <fstream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

// generated in test/unit_test/CMakeLists.txt
#include <test_data.hpp>

#ifdef FK_YAML_HAS_CXX_17
#include <string_view>
#endif

//
// test cases for constructors
//

TEST_CASE("Node_DefaultCtor") {
    fkyaml::node node;
    REQUIRE(node.is_null());
}

TEST_CASE("Node_SequenceTypeCtor") {
    fkyaml::node node(fkyaml::node_type::SEQUENCE);
    REQUIRE(node.is_sequence());
    REQUIRE(node.size() == 0);
}

TEST_CASE("Node_MappingTypeCtor") {
    fkyaml::node node(fkyaml::node_type::MAPPING);
    REQUIRE(node.is_mapping());
    REQUIRE(node.size() == 0);
}

TEST_CASE("Node_NullTypeCtor") {
    fkyaml::node node(fkyaml::node_type::NULL_OBJECT);
    REQUIRE(node.is_null());
}

TEST_CASE("Node_BooleanTypeCtor") {
    fkyaml::node node(fkyaml::node_type::BOOLEAN);
    REQUIRE(node.is_boolean());
    REQUIRE(node.as_bool() == false);
}

TEST_CASE("Node_IntegerTypeCtor") {
    fkyaml::node node(fkyaml::node_type::INTEGER);
    REQUIRE(node.is_integer());
    REQUIRE(node.as_int() == 0);
}

TEST_CASE("Node_FloatTypeCtor") {
    fkyaml::node node(fkyaml::node_type::FLOAT);
    REQUIRE(node.is_float_number());
    REQUIRE(node.as_float() == 0.0);
}

TEST_CASE("Node_StringTypeCtor") {
    fkyaml::node node(fkyaml::node_type::STRING);
    REQUIRE(node.is_string());
    REQUIRE(node.size() == 0);
}

TEST_CASE("Node_SequenceTypeCtor(deprecated)") {
    fkyaml::node node(fkyaml::node::node_t::SEQUENCE);
    REQUIRE(node.is_sequence());
    REQUIRE(node.size() == 0);
}

TEST_CASE("Node_MappingTypeCtor(deprecated)") {
    fkyaml::node node(fkyaml::node::node_t::MAPPING);
    REQUIRE(node.is_mapping());
    REQUIRE(node.size() == 0);
}

TEST_CASE("Node_NullTypeCtor(deprecated)") {
    fkyaml::node node(fkyaml::node::node_t::NULL_OBJECT);
    REQUIRE(node.is_null());
}

TEST_CASE("Node_BooleanTypeCtor(deprecated)") {
    fkyaml::node node(fkyaml::node::node_t::BOOLEAN);
    REQUIRE(node.is_boolean());
    REQUIRE(node.as_bool() == false);
}

TEST_CASE("Node_IntegerTypeCtor(deprecated)") {
    fkyaml::node node(fkyaml::node::node_t::INTEGER);
    REQUIRE(node.is_integer());
    REQUIRE(node.as_int() == 0);
}

TEST_CASE("Node_FloatNumberTypeCtor(deprecated)") {
    fkyaml::node node(fkyaml::node::node_t::FLOAT_NUMBER);
    REQUIRE(node.is_float_number());
    REQUIRE(node.as_float() == 0.0);
}

TEST_CASE("Node_StringTypeCtor(deprecated)") {
    fkyaml::node node(fkyaml::node::node_t::STRING);
    REQUIRE(node.is_string());
    REQUIRE(node.size() == 0);
}

TEST_CASE("Node_ThrowingSpecializationTypeCtor") {
    struct String {
        String() {
            throw fkyaml::exception();
        }
    };

    using NodeType = fkyaml::basic_node<std::vector, std::map, bool, int64_t, double, String>;
    REQUIRE_THROWS_AS(NodeType(fkyaml::node_type::STRING), fkyaml::exception);
}

TEST_CASE("Node_CtorWithCompatibleType") {
    // sequence-like types

    SECTION("1D C-style array") {
        fkyaml::node ints_node_val[2] {1, 2};
        int ints_val[2] {1, 2};

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_sequence());
            REQUIRE(n.size() == 2);
            REQUIRE(n[0].is_integer());
            REQUIRE(n[0].get_value<int>() == 1);
            REQUIRE(n[1].is_integer());
            REQUIRE(n[1].get_value<int>() == 2);
        };

        fkyaml::node ints_node(ints_node_val);
        validate(ints_node);

        fkyaml::node ints(ints_val);
        validate(ints);
    }

    SECTION("2D C-style array") {
        fkyaml::node ints_node_val[3][3] {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
        int ints_val[3][3] {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

        auto validate = [](const fkyaml::node& n, int i, int j) {
            REQUIRE(n.at(i).at(j).is_integer());
            REQUIRE(n.at(i).at(j).as_int() == i * 3 + j + 1);
        };

        fkyaml::node ints_node(ints_node_val);
        fkyaml::node ints(ints_val);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                validate(ints_node, i, j);
                validate(ints, i, j);
            }
        }
    }

    SECTION("3D C-style array") {
        fkyaml::node ints_node_val[3][3][3] {
            {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}},
            {{10, 11, 12}, {13, 14, 15}, {16, 17, 18}},
            {{19, 20, 21}, {22, 23, 24}, {25, 26, 27}}};
        int ints_val[3][3][3] {
            {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}},
            {{10, 11, 12}, {13, 14, 15}, {16, 17, 18}},
            {{19, 20, 21}, {22, 23, 24}, {25, 26, 27}}};

        auto validate = [](const fkyaml::node& n, int i, int j, int k) {
            REQUIRE(n.at(i).at(j).at(k).is_integer());
            REQUIRE(n.at(i).at(j).at(k).as_int() == i * 9 + j * 3 + k + 1);
        };

        fkyaml::node ints_node(ints_node_val);
        fkyaml::node ints(ints_val);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    validate(ints_node, i, j, k);
                    validate(ints, i, j, k);
                }
            }
        }
    }

    SECTION("std::vector") {
        std::vector<fkyaml::node> vector_node_val {fkyaml::node(true), fkyaml::node(false)};
        std::vector<bool> vector_bool_val {true, false};

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_sequence());
            REQUIRE(n.size() == 2);
            REQUIRE(n[0].is_boolean());
            REQUIRE(n[0].get_value<bool>() == true);
            REQUIRE(n[1].is_boolean());
            REQUIRE(n[1].get_value<bool>() == false);
        };

        fkyaml::node vector_node(vector_node_val);
        validate(vector_node);

        fkyaml::node vector_bool(vector_bool_val);
        validate(vector_bool);
    }

    SECTION("std::array") {
        std::array<fkyaml::node, 2> array_node_val {{fkyaml::node(true), fkyaml::node(false)}};
        std::array<bool, 2> array_bool_val {{true, false}};

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_sequence());
            REQUIRE(n.size() == 2);
            REQUIRE(n[0].is_boolean());
            REQUIRE(n[0].get_value<bool>() == true);
            REQUIRE(n[1].is_boolean());
            REQUIRE(n[1].get_value<bool>() == false);
        };

        fkyaml::node array_node(array_node_val);
        validate(array_node);

        fkyaml::node array_bool(array_bool_val);
        validate(array_bool);
    }

    SECTION("std::valarray") {
        std::valarray<fkyaml::node> valarray_node_val {fkyaml::node(true), fkyaml::node(false)};
        std::valarray<bool> valarray_bool_val {true, false};

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_sequence());
            REQUIRE(n.size() == 2);
            REQUIRE(n[0].is_boolean());
            REQUIRE(n[0].get_value<bool>() == true);
            REQUIRE(n[1].is_boolean());
            REQUIRE(n[1].get_value<bool>() == false);
        };

        fkyaml::node valarray_node(valarray_node_val);
        validate(valarray_node);

        fkyaml::node valarray_bool(valarray_bool_val);
        validate(valarray_bool);
    }

    SECTION("std::forward_list") {
        std::forward_list<fkyaml::node> forward_list_node_val {fkyaml::node(true), fkyaml::node(false)};
        std::forward_list<bool> forward_list_bool_val {true, false};

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_sequence());
            REQUIRE(n.size() == 2);
            REQUIRE(n[0].is_boolean());
            REQUIRE(n[0].get_value<bool>() == true);
            REQUIRE(n[1].is_boolean());
            REQUIRE(n[1].get_value<bool>() == false);
        };

        fkyaml::node forward_list_node(forward_list_node_val);
        validate(forward_list_node);

        fkyaml::node forward_list_bool(forward_list_bool_val);
        validate(forward_list_bool);
    }

    SECTION("std::deque") {
        std::deque<fkyaml::node> deque_node_val {fkyaml::node(true), fkyaml::node(false)};
        std::deque<bool> deque_bool_val {true, false};

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_sequence());
            REQUIRE(n.size() == 2);
            REQUIRE(n[0].is_boolean());
            REQUIRE(n[0].get_value<bool>() == true);
            REQUIRE(n[1].is_boolean());
            REQUIRE(n[1].get_value<bool>() == false);
        };

        fkyaml::node deque_node(deque_node_val);
        validate(deque_node);

        fkyaml::node deque_bool(deque_bool_val);
        validate(deque_bool);
    }

    SECTION("std::list") {
        std::list<fkyaml::node> list_node_val {fkyaml::node(true), fkyaml::node(false)};
        std::list<bool> list_bool_val {true, false};

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_sequence());
            REQUIRE(n.size() == 2);
            REQUIRE(n[0].is_boolean());
            REQUIRE(n[0].get_value<bool>() == true);
            REQUIRE(n[1].is_boolean());
            REQUIRE(n[1].get_value<bool>() == false);
        };

        fkyaml::node list_node(list_node_val);
        validate(list_node);

        fkyaml::node list_bool(list_bool_val);
        validate(list_bool);
    }

    SECTION("std::set") {
        std::set<fkyaml::node> set_node_val {fkyaml::node(true), fkyaml::node(false)};
        std::set<bool> set_bool_val {true, false};

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_sequence());
            REQUIRE(n.size() == 2);
            REQUIRE(n[0].is_boolean());
            REQUIRE(n[0].get_value<bool>() == false);
            REQUIRE(n[1].is_boolean());
            REQUIRE(n[1].get_value<bool>() == true);
        };

        fkyaml::node set_node(set_node_val);
        validate(set_node);

        fkyaml::node set_bool(set_bool_val);
        validate(set_bool);
    }

    SECTION("std::multiset") {
        std::multiset<fkyaml::node> multiset_node_val {fkyaml::node(true), fkyaml::node(false)};
        std::multiset<bool> multiset_bool_val {true, false};

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_sequence());
            REQUIRE(n.size() == 2);
            REQUIRE(n[0].is_boolean());
            REQUIRE(n[0].get_value<bool>() == false);
            REQUIRE(n[1].is_boolean());
            REQUIRE(n[1].get_value<bool>() == true);
        };

        fkyaml::node multiset_node(multiset_node_val);
        validate(multiset_node);

        fkyaml::node multiset_bool(multiset_bool_val);
        validate(multiset_bool);
    }

    SECTION("std::unordered_set") {
        std::unordered_set<fkyaml::node> unordered_set_node_val {fkyaml::node(true), fkyaml::node(false)};
        std::unordered_set<bool> unordered_set_bool_val {true, false};

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_sequence());
            REQUIRE(n.size() == 2);
            REQUIRE(n[0].is_boolean());
            bool ret = n[0].get_value<bool>();
            REQUIRE(n[1].is_boolean());
            REQUIRE(n[1].get_value<bool>() == !ret);
        };

        fkyaml::node unordered_set_node(unordered_set_node_val);
        validate(unordered_set_node);

        fkyaml::node unordered_set_bool(unordered_set_bool_val);
        validate(unordered_set_bool);
    }

    SECTION("std::unordered_multiset") {
        std::unordered_multiset<fkyaml::node> unordered_multiset_node_val {fkyaml::node(true), fkyaml::node(false)};
        std::unordered_multiset<bool> unordered_multiset_bool_val {true, false};

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_sequence());
            REQUIRE(n.size() == 2);
            REQUIRE(n[0].is_boolean());
            bool ret = n[0].get_value<bool>();
            REQUIRE(n[1].is_boolean());
            REQUIRE(n[1].get_value<bool>() == !ret);
        };

        fkyaml::node unordered_multiset_node(unordered_multiset_node_val);
        validate(unordered_multiset_node);

        fkyaml::node unordered_multiset_bool(unordered_multiset_bool_val);
        validate(unordered_multiset_bool);
    }

    SECTION("std::pair") {
        std::pair<fkyaml::node, fkyaml::node> pair_node_val = {"test", 123};
        std::pair<std::string, int> pair_val = {"test", 123};

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_sequence());
            REQUIRE(n.size() == 2);
            REQUIRE(n[0].is_string());
            REQUIRE(n[0].as_str() == "test");
            REQUIRE(n[1].is_integer());
            REQUIRE(n[1].as_int() == 123);
        };

        fkyaml::node pair_node(pair_node_val);
        validate(pair_node);

        fkyaml::node pair(pair_val);
        validate(pair);
    }

    SECTION("std::tuple") {
        std::tuple<fkyaml::node, fkyaml::node, fkyaml::node> tuple_node_val("test", 123, true);
        std::tuple<std::string, int, bool> tuple_val("test", 123, true);

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_sequence());
            REQUIRE(n.size() == 3);
            REQUIRE(n[0].is_string());
            REQUIRE(n[0].as_str() == "test");
            REQUIRE(n[1].is_integer());
            REQUIRE(n[1].as_int() == 123);
            REQUIRE(n[2].is_boolean());
            REQUIRE(n[2].as_bool() == true);
        };

        fkyaml::node tuple_node(tuple_node_val);
        validate(tuple_node);

        fkyaml::node tuple(tuple_val);
        validate(tuple);

        // regression test for https://github.com/fktn-k/fkYAML/pull/467
        std::tuple<> empty_tuple_val {};
        fkyaml::node empty_tuple_node(empty_tuple_val);
        REQUIRE(empty_tuple_node.is_sequence());
        REQUIRE(empty_tuple_node.empty());
    }

    // mapping-like types

    SECTION("std::map") {
        std::map<fkyaml::node, fkyaml::node> map_node_val;
        map_node_val.emplace("test", 123);
        map_node_val.emplace("foo", -456);
        std::map<std::string, int> map_val;
        map_val.emplace("test", 123);
        map_val.emplace("foo", -456);

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_mapping());
            REQUIRE(n.size() == 2);
            REQUIRE(n.at("test").is_integer());
            REQUIRE(n.at("test").as_int() == 123);
            REQUIRE(n.at("foo").is_integer());
            REQUIRE(n.at("foo").as_int() == -456);
        };

        fkyaml::node map_node(map_node_val);
        validate(map_node);

        fkyaml::node map(map_val);
        validate(map);
    }

    SECTION("std::multimap") {
        std::multimap<fkyaml::node, fkyaml::node> multimap_node_val;
        multimap_node_val.emplace("test", 123);
        multimap_node_val.emplace("foo", -456);
        std::multimap<std::string, int> multimap_val;
        multimap_val.emplace("test", 123);
        multimap_val.emplace("foo", -456);

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_mapping());
            REQUIRE(n.size() == 2);
            REQUIRE(n.at("test").is_integer());
            REQUIRE(n.at("test").as_int() == 123);
            REQUIRE(n.at("foo").is_integer());
            REQUIRE(n.at("foo").as_int() == -456);
        };

        fkyaml::node multimap_node(multimap_node_val);
        validate(multimap_node);

        fkyaml::node multimap(multimap_val);
        validate(multimap);
    }

    SECTION("std::unordered_map") {
        std::unordered_map<fkyaml::node, fkyaml::node> unordered_map_node_val;
        unordered_map_node_val.emplace("test", 123);
        unordered_map_node_val.emplace("foo", -456);
        std::unordered_map<std::string, int> unordered_map_val;
        unordered_map_val.emplace("test", 123);
        unordered_map_val.emplace("foo", -456);

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_mapping());
            REQUIRE(n.size() == 2);
            REQUIRE(n.at("test").is_integer());
            REQUIRE(n.at("test").as_int() == 123);
            REQUIRE(n.at("foo").is_integer());
            REQUIRE(n.at("foo").as_int() == -456);
        };

        fkyaml::node unordered_map_node(unordered_map_node_val);
        validate(unordered_map_node);

        fkyaml::node unordered_map(unordered_map_val);
        validate(unordered_map);
    }

    SECTION("std::unordered_multimap") {
        std::unordered_multimap<fkyaml::node, fkyaml::node> unordered_multimap_node_val;
        unordered_multimap_node_val.emplace("test", 123);
        unordered_multimap_node_val.emplace("foo", -456);
        std::unordered_multimap<std::string, int> unordered_multimap_val;
        unordered_multimap_val.emplace("test", 123);
        unordered_multimap_val.emplace("foo", -456);

        auto validate = [](const fkyaml::node& n) {
            REQUIRE(n.is_mapping());
            REQUIRE(n.size() == 2);
            REQUIRE(n.at("test").is_integer());
            REQUIRE(n.at("test").as_int() == 123);
            REQUIRE(n.at("foo").is_integer());
            REQUIRE(n.at("foo").as_int() == -456);
        };

        fkyaml::node unordered_multimap_node(unordered_multimap_node_val);
        validate(unordered_multimap_node);

        fkyaml::node unordered_multimap(unordered_multimap_val);
        validate(unordered_multimap);
    }

    SECTION("std::nullptr_t") {
        fkyaml::node node(nullptr);
        REQUIRE(node.is_null());
    }

    SECTION("boolean") {
        fkyaml::node node(true);
        REQUIRE(node.is_boolean());
        REQUIRE(node.as_bool() == true);
    }

    SECTION("integer") {
        fkyaml::node node(23467);
        REQUIRE(node.is_integer());
        REQUIRE(node.as_int() == 23467);
    }

    SECTION("floating point") {
        fkyaml::node node(3.14);
        REQUIRE(node.is_float_number());
        REQUIRE(node.as_float() == 3.14);
    }

    // string-like types

    SECTION("std::string") {
        fkyaml::node node(std::string("test"));
        REQUIRE(node.is_string());
        REQUIRE(node.size() == 4);
        REQUIRE(node.as_str() == "test");
    }

    SECTION("C-style char array") {
        fkyaml::node node("test");
        REQUIRE(node.is_string());
        REQUIRE(node.size() == 4);
        REQUIRE(node.as_str() == "test");
    }

    SECTION("pointer to a null-terminated char sequence") {
        const char buff[] = "test";
        const char* p_buff = &buff[0];
        fkyaml::node node(p_buff);
        REQUIRE(node.is_string());
        REQUIRE(node.size() == 4);
        REQUIRE(node.as_str() == "test");
    }

#ifdef FK_YAML_HAS_CXX_17
    SECTION("Node_StringViewCtor") {
        using namespace std::string_view_literals;
        auto node = fkyaml::node("test"sv);
        REQUIRE(node.is_string());
        REQUIRE(node.size() == 4);
        REQUIRE(node.as_str() == "test");
    }
#endif
}

TEST_CASE("Node_SequenceCopyCtor") {
    fkyaml::node n = "test";

    fkyaml::node copied = {true, "test"};
    fkyaml::node node(copied);
    REQUIRE(node.is_sequence());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 2);
    REQUIRE_NOTHROW(node[0]);
    REQUIRE(node[0].is_boolean());
    REQUIRE_NOTHROW(node[0].as_bool());
    REQUIRE(node[0].as_bool() == true);
    REQUIRE_NOTHROW(node[1]);
    REQUIRE(node[1].is_string());
    REQUIRE_NOTHROW(node[1].as_str());
    REQUIRE_NOTHROW(node[1].as_str().size());
    REQUIRE(node[1].as_str().size() == 4);
    REQUIRE(node[1].as_str().compare("test") == 0);
}

TEST_CASE("Node_MappingCopyCtor") {
    fkyaml::node copied = {{"test0", 123}, {"test1", 3.14}};
    fkyaml::node node(copied);
    REQUIRE(node.is_mapping());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 2);
    REQUIRE_NOTHROW(node["test0"]);
    REQUIRE(node["test0"].is_integer());
    REQUIRE_NOTHROW(node["test0"].as_int());
    REQUIRE(node["test0"].as_int() == 123);
    REQUIRE_NOTHROW(node["test1"]);
    REQUIRE(node["test1"].is_float_number());
    REQUIRE_NOTHROW(node["test1"].as_float());
    REQUIRE(node["test1"].as_float() == 3.14);
}

TEST_CASE("Node_NullCopyCtor") {
    fkyaml::node copied;
    fkyaml::node node(copied);
    REQUIRE(node.is_null());
}

TEST_CASE("Node_BooleanCopyCtor") {
    fkyaml::node copied = true;
    fkyaml::node node(copied);
    REQUIRE(node.is_boolean());
    REQUIRE_NOTHROW(node.as_bool());
    REQUIRE(node.as_bool() == true);
}

TEST_CASE("Node_IntegerCopyCtor") {
    fkyaml::node copied = 123;
    fkyaml::node node(copied);
    REQUIRE(node.is_integer());
    REQUIRE_NOTHROW(node.as_int());
    REQUIRE(node.as_int() == 123);
}

TEST_CASE("Node_FloatNumberCopyCtor") {
    fkyaml::node copied = 3.14;
    fkyaml::node node(copied);
    REQUIRE(node.is_float_number());
    REQUIRE_NOTHROW(node.as_float());
    REQUIRE(node.as_float() == 3.14);
}

TEST_CASE("Node_StringCopyCtor") {
    fkyaml::node copied = "test";
    fkyaml::node node(copied);
    REQUIRE(node.is_string());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 4);
    REQUIRE_NOTHROW(node.as_str());
    REQUIRE(node.as_str().compare("test") == 0);
}

TEST_CASE("Node_AliasCopyCtor") {
    fkyaml::node tmp = true;
    tmp.add_anchor_name("anchor_name");
    fkyaml::node tmp_alias = fkyaml::node::alias_of(tmp);
    fkyaml::node alias(tmp_alias);
    REQUIRE(alias.is_boolean());
    REQUIRE_NOTHROW(alias.as_bool());
    REQUIRE(alias.as_bool() == true);
}

TEST_CASE("Node_SequenceMoveCtor") {
    fkyaml::node moved = {true, "test"};
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_sequence());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 2);
    REQUIRE_NOTHROW(node[0]);
    REQUIRE(node[0].is_boolean());
    REQUIRE_NOTHROW(node[0].as_bool());
    REQUIRE(node[0].as_bool() == true);
    REQUIRE_NOTHROW(node[1]);
    REQUIRE(node[1].is_string());
    REQUIRE_NOTHROW(node[1].as_str());
    REQUIRE_NOTHROW(node[1].as_str().size());
    REQUIRE(node[1].as_str().size() == 4);
    REQUIRE(node[1].as_str().compare("test") == 0);
}

TEST_CASE("Node_MappingMoveCtor") {
    fkyaml::node moved = {{"test0", 123}, {"test1", 3.14}};
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_mapping());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 2);
    REQUIRE_NOTHROW(node["test0"]);
    REQUIRE(node["test0"].is_integer());
    REQUIRE_NOTHROW(node["test0"].as_int());
    REQUIRE(node["test0"].as_int() == 123);
    REQUIRE_NOTHROW(node["test1"]);
    REQUIRE(node["test1"].is_float_number());
    REQUIRE_NOTHROW(node["test1"].as_float());
    REQUIRE(node["test1"].as_float() == 3.14);
}

TEST_CASE("Node_NullMoveCtor") {
    fkyaml::node moved;
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_null());
}

TEST_CASE("Node_BooleanMoveCtor") {
    fkyaml::node moved = true;
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_boolean());
    REQUIRE_NOTHROW(node.as_bool());
    REQUIRE(node.as_bool() == true);
}

TEST_CASE("Node_IntegerMoveCtor") {
    fkyaml::node moved = 123;
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_integer());
    REQUIRE_NOTHROW(node.as_int());
    REQUIRE(node.as_int() == 123);
}

TEST_CASE("Node_FloatNumberMoveCtor") {
    fkyaml::node moved = 3.14;
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_float_number());
    REQUIRE_NOTHROW(node.as_float());
    REQUIRE(node.as_float() == 3.14);
}

TEST_CASE("Node_StringMoveCtor") {
    fkyaml::node moved = "test";
    fkyaml::node node(std::move(moved));
    REQUIRE(node.is_string());
    REQUIRE_NOTHROW(node.size());
    REQUIRE(node.size() == 4);
    REQUIRE_NOTHROW(node.as_str());
    REQUIRE(node.as_str().compare("test") == 0);
}

TEST_CASE("Node_AliasMoveCtor") {
    fkyaml::node tmp = true;
    tmp.add_anchor_name("anchor_name");
    fkyaml::node tmp_alias = fkyaml::node::alias_of(tmp);
    fkyaml::node alias(std::move(tmp_alias));
    REQUIRE(alias.is_boolean());
    REQUIRE_NOTHROW(alias.as_bool());
    REQUIRE(alias.as_bool() == true);
}

TEST_CASE("Node_InitializerListCtor") {
    fkyaml::node node = {
        {"foo", 3.14},
        {true, 123},
        {567, {true, false}},
        {{true, 1.57}, nullptr},
        {{{"bar", nullptr}}, {{"baz", "qux"}}}};

    REQUIRE(node.is_mapping());
    REQUIRE(node.size() == 5);

    REQUIRE(node.contains("foo"));
    REQUIRE(node["foo"].is_float_number());
    REQUIRE(node["foo"].as_float() == 3.14);

    REQUIRE(node.contains(true));
    REQUIRE(node[true].is_integer());
    REQUIRE(node[true].as_int() == 123);

    REQUIRE(node.contains(567));
    REQUIRE(node[567].is_sequence());
    REQUIRE(node[567].size() == 2);
    REQUIRE(node[567][0].is_boolean());
    REQUIRE(node[567][0].as_bool() == true);
    REQUIRE(node[567][1].is_boolean());
    REQUIRE(node[567][1].as_bool() == false);

    REQUIRE(node.contains(fkyaml::node {true, 1.57}));
    REQUIRE(node[fkyaml::node {true, 1.57}].is_null());

    REQUIRE(node.contains(fkyaml::node {{"bar", nullptr}}));
    REQUIRE(node[fkyaml::node {{"bar", nullptr}}].is_mapping());
    REQUIRE(node[fkyaml::node {{"bar", nullptr}}].size() == 1);
    REQUIRE(node[fkyaml::node {{"bar", nullptr}}].contains("baz"));
    REQUIRE(node[fkyaml::node {{"bar", nullptr}}]["baz"].is_string());
    REQUIRE(node[fkyaml::node {{"bar", nullptr}}]["baz"].as_str() == "qux");
}

TEST_CASE("Node_CopyAssignmentOperator") {
    fkyaml::node node(123);
    fkyaml::node copied(true);

    node = copied;
    REQUIRE(node.is_boolean());
    REQUIRE(node.get_value<bool>() == true);
    REQUIRE(copied.is_boolean());
    REQUIRE(copied.get_value<bool>() == true);
}

//
// test cases for deserialization
//

/// @brief
/// A utility iterator class which implements LegacyInputIterator.
/// https://en.cppreference.com/w/cpp/named_req/InputIterator
template <typename T>
class legacy_input_iterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    legacy_input_iterator() = default;
    legacy_input_iterator(pointer p_val) noexcept
        : mp_val(p_val) {
    }
    legacy_input_iterator(const legacy_input_iterator&) = default;
    legacy_input_iterator(legacy_input_iterator&&) = default;
    legacy_input_iterator& operator=(const legacy_input_iterator&) = default;
    legacy_input_iterator& operator=(legacy_input_iterator&&) = default;
    ~legacy_input_iterator() = default;

    legacy_input_iterator& operator++() noexcept {
        ++mp_val;
        return *this;
    }

    reference operator*() noexcept {
        return *mp_val;
    }

    bool operator==(const legacy_input_iterator& rhs) const noexcept {
        return mp_val == rhs.mp_val;
    }

    bool operator!=(const legacy_input_iterator& rhs) const noexcept {
        return mp_val != rhs.mp_val;
    }

private:
    pointer mp_val {nullptr};
};

TEST_CASE("Node_Deserialize") {
    char source[] = "foo: bar";
    char16_t source_u16[] = u"foo: bar";
    char32_t source_u32[] = U"foo: bar";
    std::stringstream ss;
    ss << source;

    auto begin = legacy_input_iterator<char> {&source[0]};
    auto end = legacy_input_iterator<char> {&source[8]};
    auto begin_u16 = legacy_input_iterator<char16_t> {&source_u16[0]};
    auto end_u16 = legacy_input_iterator<char16_t> {&source_u16[8]};
    auto begin_u32 = legacy_input_iterator<char32_t> {&source_u32[0]};
    auto end_u32 = legacy_input_iterator<char32_t> {&source_u32[8]};

    fkyaml::node node = GENERATE_REF(
        fkyaml::node::deserialize("foo: bar"),
        fkyaml::node::deserialize(source),
        fkyaml::node::deserialize(&source[0], &source[8]),
        fkyaml::node::deserialize(begin, end),
        fkyaml::node::deserialize(begin_u16, end_u16),
        fkyaml::node::deserialize(begin_u32, end_u32),
        fkyaml::node::deserialize(std::string(source)),
        fkyaml::node::deserialize(ss));

    REQUIRE(node.is_mapping());
    REQUIRE(node.size() == 1);
    REQUIRE(node.contains("foo"));
    REQUIRE(node["foo"].is_string());
    REQUIRE(node["foo"].as_str() == "bar");
}

#if FK_YAML_HAS_CHAR8_T
TEST_CASE("Node_Deserialize_char8_t") {
    char8_t source_u8[] = u8"foo: bar";
    auto begin_u8 = legacy_input_iterator<char8_t> {&source_u8[0]};
    auto end_u8 = legacy_input_iterator<char8_t> {&source_u8[8]};

    fkyaml::node node = GENERATE_REF(
        fkyaml::node::deserialize(&source_u8[0], &source_u8[8]), fkyaml::node::deserialize(begin_u8, end_u8));

    REQUIRE(node.is_mapping());
    REQUIRE(node.size() == 1);
    REQUIRE(node.contains("foo"));
    REQUIRE(node["foo"].is_string());
    REQUIRE(node["foo"].as_str() == "bar");
}
#endif // FK_YAML_HAS_CHAR8_T

TEST_CASE("Node_DeserializeDocs") {
    char source[] = "foo: bar\n"
                    "...\n"
                    "- true\n"
                    "- 3.14\n"
                    "- Null";
    char16_t source_u16[] = u"foo: bar\n"
                            "...\n"
                            "- true\n"
                            "- 3.14\n"
                            "- Null";
    char32_t source_u32[] = U"foo: bar\n"
                            "...\n"
                            "- true\n"
                            "- 3.14\n"
                            "- Null";
    std::stringstream ss;
    ss << source;

    auto begin = legacy_input_iterator<char> {&source[0]};
    auto end = legacy_input_iterator<char> {&source[33]};
    auto begin_u16 = legacy_input_iterator<char16_t> {&source_u16[0]};
    auto end_u16 = legacy_input_iterator<char16_t> {&source_u16[33]};
    auto begin_u32 = legacy_input_iterator<char32_t> {&source_u32[0]};
    auto end_u32 = legacy_input_iterator<char32_t> {&source_u32[33]};

    std::vector<fkyaml::node> docs = GENERATE_REF(
        fkyaml::node::deserialize_docs("foo: bar\n"
                                       "...\n"
                                       "- true\n"
                                       "- 3.14\n"
                                       "- Null"),
        fkyaml::node::deserialize_docs(source),
        fkyaml::node::deserialize_docs(&source[0], &source[33]),
        fkyaml::node::deserialize_docs(begin, end),
        fkyaml::node::deserialize_docs(begin_u16, end_u16),
        fkyaml::node::deserialize_docs(begin_u32, end_u32),
        fkyaml::node::deserialize_docs(std::string(source)),
        fkyaml::node::deserialize_docs(ss));

    REQUIRE(docs.size() == 2);
    fkyaml::node& root0 = docs[0];
    REQUIRE(root0.is_mapping());
    REQUIRE(root0.size() == 1);
    REQUIRE(root0.contains("foo"));
    fkyaml::node& foo_node = root0["foo"];
    REQUIRE(foo_node.is_string());
    REQUIRE(foo_node.as_str() == "bar");

    fkyaml::node& root1 = docs[1];
    REQUIRE(root1.is_sequence());
    REQUIRE(root1.size() == 3);
    fkyaml::node& seq0 = root1[0];
    REQUIRE(seq0.is_boolean());
    REQUIRE(seq0.get_value<bool>() == true);
    fkyaml::node& seq1 = root1[1];
    REQUIRE(seq1.is_float_number());
    REQUIRE(seq1.get_value<double>() == 3.14);
    fkyaml::node& seq2 = root1[2];
    REQUIRE(seq2.is_null());
}

#if FK_YAML_HAS_CHAR8_T
TEST_CASE("Node_DeserializeDocs_char8_t") {
    char8_t source_u8[] = u8"foo: bar\n"
                          "...\n"
                          "- true\n"
                          "- 3.14\n"
                          "- Null";
    auto begin_u8 = legacy_input_iterator<char8_t> {&source_u8[0]};
    auto end_u8 = legacy_input_iterator<char8_t> {&source_u8[33]};

    std::vector<fkyaml::node> docs = GENERATE_REF(
        fkyaml::node::deserialize_docs(&source_u8[0], &source_u8[33]),
        fkyaml::node::deserialize_docs(begin_u8, end_u8));

    REQUIRE(docs.size() == 2);
    fkyaml::node& root0 = docs[0];
    REQUIRE(root0.is_mapping());
    REQUIRE(root0.size() == 1);
    REQUIRE(root0.contains("foo"));
    fkyaml::node& foo_node = root0["foo"];
    REQUIRE(foo_node.is_string());
    REQUIRE(foo_node.as_str() == "bar");

    fkyaml::node& root1 = docs[1];
    REQUIRE(root1.is_sequence());
    REQUIRE(root1.size() == 3);
    fkyaml::node& seq0 = root1[0];
    REQUIRE(seq0.is_boolean());
    REQUIRE(seq0.get_value<bool>() == true);
    fkyaml::node& seq1 = root1[1];
    REQUIRE(seq1.is_float_number());
    REQUIRE(seq1.get_value<double>() == 3.14);
    fkyaml::node& seq2 = root1[2];
    REQUIRE(seq2.is_null());
}
#endif // FK_YAML_HAS_CHAR8_T

TEST_CASE("Node_ExtractionOperator") {
    fkyaml::node node;
    std::ifstream ifs(FK_YAML_TEST_DATA_DIR "/extraction_operator_test_data.yml");
    ifs >> node;

    REQUIRE(node.is_mapping());
    REQUIRE(node.size() == 3);
    REQUIRE(node["foo"].get_value<int>() == 123);
    REQUIRE(node["bar"].is_null());
    REQUIRE(node["baz"].get_value<bool>() == true);
}

TEST_CASE("Node_UserDefinedLiteralYaml") {
    SECTION("char sequences literals with using fkyaml::literals") {
        using namespace fkyaml::literals;
        fkyaml::node node = "en: hello\njp: konnichiwa"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].as_str() == "hello");
        REQUIRE(node["jp"].as_str() == "konnichiwa");
    }

    SECTION("char sequences literals with using fkyaml::yaml_literals") {
        using namespace fkyaml::yaml_literals;
        fkyaml::node node = "en: hello\njp: konnichiwa"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].as_str() == "hello");
        REQUIRE(node["jp"].as_str() == "konnichiwa");
    }

    SECTION("char sequences literals with using fkyaml::literals::yaml_literals") {
        using namespace fkyaml::literals::yaml_literals;
        fkyaml::node node = "en: hello\njp: konnichiwa"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].as_str() == "hello");
        REQUIRE(node["jp"].as_str() == "konnichiwa");
    }

    SECTION("char sequences of u8\"\" literals with using fkyaml::literals") {
        using namespace fkyaml::literals;
        fkyaml::node node = u8"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].as_str() == "hello");
        REQUIRE(node["jp"].as_str() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char sequences with u8 literal literals with using fkyaml::yaml_literals") {
        using namespace fkyaml::yaml_literals;
        fkyaml::node node = u8"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].as_str() == "hello");
        REQUIRE(node["jp"].as_str() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char sequences of u8 literal literals with using fkyaml::literals::yaml_literals") {
        using namespace fkyaml::literals::yaml_literals;
        fkyaml::node node = u8"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].as_str() == "hello");
        REQUIRE(node["jp"].as_str() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char16_t sequences with using fkyaml::literals") {
        using namespace fkyaml::literals;
        fkyaml::node node = u"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].as_str() == "hello");
        REQUIRE(node["jp"].as_str() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char16_t sequences with using fkyaml::yaml_literals") {
        using namespace fkyaml::yaml_literals;
        fkyaml::node node = u"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].as_str() == "hello");
        REQUIRE(node["jp"].as_str() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char16_t sequences with using fkyaml::literals::yaml_literals") {
        using namespace fkyaml::literals::yaml_literals;
        fkyaml::node node = u"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].as_str() == "hello");
        REQUIRE(node["jp"].as_str() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char32_t sequences with using fkyaml::literals") {
        using namespace fkyaml::literals;
        fkyaml::node node = U"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].as_str() == "hello");
        REQUIRE(node["jp"].as_str() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char32_t sequences with using fkyaml::yaml_literals") {
        using namespace fkyaml::yaml_literals;
        fkyaml::node node = U"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].as_str() == "hello");
        REQUIRE(node["jp"].as_str() == reinterpret_cast<const char*>(u8"こんにちは"));
    }

    SECTION("char32_t sequences with using fkyaml::literals::yaml_literals") {
        using namespace fkyaml::literals::yaml_literals;
        fkyaml::node node = U"en: hello\njp: こんにちは"_yaml;

        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 2);
        REQUIRE(node["en"].as_str() == "hello");
        REQUIRE(node["jp"].as_str() == reinterpret_cast<const char*>(u8"こんにちは"));
    }
}

//
// test cases for serialization
//

TEST_CASE("Node_Serialize") {
    fkyaml::node node = fkyaml::node::deserialize("foo: bar");
    REQUIRE(fkyaml::node::serialize(node) == "foo: bar\n");
}

TEST_CASE("Node_SerializeDocs") {
    std::vector<fkyaml::node> docs = fkyaml::node::deserialize_docs("foo: bar\n"
                                                                    "...\n"
                                                                    "123: true");
    REQUIRE(fkyaml::node::serialize_docs(docs) == "foo: bar\n...\n123: true\n");
}

TEST_CASE("Node_InsertionOperator") {
    fkyaml::node node = {{"foo", 123}, {"bar", nullptr}, {"baz", true}};
    std::stringstream ss;
    ss << node;

    REQUIRE(ss.str() == "bar: null\nbaz: true\nfoo: 123\n");
}

//
// test cases for factory methods
//

TEST_CASE("Node_SequenceNodeFactory") {
    SECTION("empty sequence") {
        fkyaml::node node = fkyaml::node::sequence();
        REQUIRE(node.is_sequence());
        REQUIRE(node.size() == 0);
    }

    SECTION("non-empty sequence") {
        fkyaml::node::sequence_type seq(3);

        SECTION("lvalue sequence") {
            fkyaml::node node = fkyaml::node::sequence(seq);
            REQUIRE(node.is_sequence());
            REQUIRE(node.size() == 3);
            for (int i = 0; i < 3; ++i) {
                REQUIRE(node[i].is_null());
            }
        }

        SECTION("lvalue sequence") {
            fkyaml::node node = fkyaml::node::sequence(std::move(seq));
            REQUIRE(node.is_sequence());
            REQUIRE(node.size() == 3);
            for (int i = 0; i < 3; ++i) {
                REQUIRE(node[i].is_null());
            }
        }
    }
}

TEST_CASE("Node_MappingNodeFactory") {
    SECTION("empty mapping") {
        fkyaml::node node = fkyaml::node::mapping();
        REQUIRE(node.is_mapping());
        REQUIRE(node.size() == 0);
    }

    SECTION("non-empty mapping node factory methods.") {
        fkyaml::node::mapping_type map {{"test", true}};

        SECTION("lvalue mapping") {
            fkyaml::node node = fkyaml::node::mapping(map);
            REQUIRE(node.is_mapping());
            REQUIRE(node.size() == 1);
            REQUIRE(node["test"].is_boolean());
            REQUIRE(node["test"].as_bool() == true);
        }

        SECTION("rvalue mapping") {
            fkyaml::node node = fkyaml::node::mapping(std::move(map));
            REQUIRE(node.is_mapping());
            REQUIRE(node.size() == 1);
            REQUIRE(node["test"].is_boolean());
            REQUIRE(node["test"].as_bool() == true);
        }
    }
}

TEST_CASE("Node_BooleanNodeFactory") {
    auto boolean = GENERATE(true, false);
    fkyaml::node node = boolean;
    REQUIRE(node.is_boolean());
    REQUIRE(node.as_bool() == boolean);
}

TEST_CASE("Node_IntegerNodeFactory") {
    auto integer = GENERATE(
        std::numeric_limits<fkyaml::node::integer_type>::min(),
        0,
        std::numeric_limits<fkyaml::node::integer_type>::max());
    fkyaml::node node = integer;
    REQUIRE(node.is_integer());
    REQUIRE(node.as_int() == integer);
}

TEST_CASE("Node_FloatNumberNodeFactory") {
    auto float_val = GENERATE(
        std::numeric_limits<fkyaml::node::float_number_type>::min(),
        3.141592,
        std::numeric_limits<fkyaml::node::float_number_type>::max());
    fkyaml::node node = float_val;
    REQUIRE(node.is_float_number());
    REQUIRE(node.as_float() == float_val);
}

TEST_CASE("Node_StringNodeFactory") {
    SECTION("empty string") {
        fkyaml::node node = "";
        REQUIRE(node.is_string());
        REQUIRE(node.size() == 0);
    }

    SECTION("lvalue string") {
        fkyaml::node::string_type str("test");
        fkyaml::node node = str;
        REQUIRE(node.is_string());
        REQUIRE(node.size() == str.size());
        REQUIRE(node.as_str() == str);
    }

    SECTION("rvalue string") {
        fkyaml::node node = "test";
        REQUIRE(node.is_string());
        REQUIRE(node.size() == 4);
        REQUIRE(node.as_str().compare("test") == 0);
    }
}

TEST_CASE("Node_AliasNodeFactory") {
    fkyaml::node anchor = "alias_test";

    SECTION("without anchor name.") {
        REQUIRE_THROWS_AS(fkyaml::node::alias_of(anchor), fkyaml::exception);
    }

    SECTION("with an empty anchor name") {
        anchor.add_anchor_name("");
        REQUIRE_THROWS_AS(fkyaml::node::alias_of(anchor), fkyaml::exception);
    }

    SECTION("with an alias node") {
        anchor.add_anchor_name("anchor");
        fkyaml::node alias = fkyaml::node::alias_of(anchor);
        REQUIRE_THROWS_AS(fkyaml::node::alias_of(alias), fkyaml::exception);
    }

    SECTION("with an anchor node") {
        anchor.add_anchor_name("anchor_name");
        REQUIRE_NOTHROW(fkyaml::node::alias_of(anchor));
        fkyaml::node alias = fkyaml::node::alias_of(anchor);
        REQUIRE(alias.is_string());
        REQUIRE(alias.as_str().compare("alias_test") == 0);
    }
}

//
// test cases for subscript operators
//

TEST_CASE("Node_SubscriptOperator") {
    SECTION("mapping") {
        fkyaml::node::mapping_type map {{"test", fkyaml::node()}};

        SECTION("non-const string value") {
            fkyaml::node node = fkyaml::node::mapping(map);

            SECTION("non-const lvalue string") {
                std::string key = "test";
                REQUIRE_NOTHROW(node[key]);
                REQUIRE(node[key].is_null());
            }

            SECTION("non-const rvalue string") {
                REQUIRE_NOTHROW(node["test"]);
                REQUIRE(node["test"].is_null());
            }
        }

        SECTION("const string value") {
            const fkyaml::node node = fkyaml::node::mapping(map);
            std::string key = "test";

            SECTION("const lvalue string") {
                REQUIRE_NOTHROW(node[key]);
            }

            SECTION("const rvalue string") {
                REQUIRE_NOTHROW(node["test"]);
            }
        }

#ifdef FK_YAML_HAS_CXX_17
        SECTION("string view value") {
            std::string_view key = "test";
            REQUIRE(map[key].is_null());
        }
#endif

        SECTION("non-const string node") {
            fkyaml::node node = fkyaml::node::mapping(map);
            fkyaml::node node_key = "test";

            SECTION("non-const lvalue string node") {
                REQUIRE_NOTHROW(node[node_key]);
            }

            SECTION("non-const rvalue string node") {
                REQUIRE_NOTHROW(node[std::move(node_key)]);
            }
        }

        SECTION("const string node") {
            const fkyaml::node node = fkyaml::node::mapping(map);
            fkyaml::node node_key = "test";

            SECTION("non-const lvalue string node") {
                REQUIRE_NOTHROW(node[node_key]);
            }

            SECTION("non-const rvalue string node") {
                REQUIRE_NOTHROW(node[std::move(node_key)]);
            }
        }
    }

    SECTION("sequence") {
        fkyaml::node node = fkyaml::node::sequence();
        node.as_seq().emplace_back();

        SECTION("non-const integer value") {
            REQUIRE_NOTHROW(node[0]);
        }

        SECTION("const integer value") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node[0]);
        }

        SECTION("non-const integer node") {
            REQUIRE_NOTHROW(node[fkyaml::node(0)]);
        }

        SECTION("const integer node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node[fkyaml::node(0)]);
        }

        SECTION("non-const node with a non-integer value") {
            REQUIRE_THROWS_AS(node[fkyaml::node::sequence_type()], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[fkyaml::node::mapping_type()], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[nullptr], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[false], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[0.0], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[""], fkyaml::type_error);
        }

        SECTION("const node with a non-integer value") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node[fkyaml::node::sequence_type()], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[fkyaml::node::mapping_type()], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[nullptr], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[false], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[0.0], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[""], fkyaml::type_error);
        }

        SECTION("non-const node with a non-integer node") {
            REQUIRE_THROWS_AS(node[fkyaml::node::sequence()], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[fkyaml::node::mapping()], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[fkyaml::node()], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[fkyaml::node(false)], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[fkyaml::node(0.0)], fkyaml::type_error);
            REQUIRE_THROWS_AS(node[fkyaml::node("")], fkyaml::type_error);
        }

        SECTION("const node with a non-integer node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node[fkyaml::node::sequence()], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[fkyaml::node::mapping()], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[fkyaml::node()], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[fkyaml::node(false)], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[fkyaml::node(0.0)], fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node[fkyaml::node("")], fkyaml::type_error);
        }
    }

    SECTION("scalar") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));
        fkyaml::node node_key = 0;

        SECTION("non-const node with an integer") {
            REQUIRE_THROWS_AS(node[0], fkyaml::type_error);
        }

        SECTION("const node with an integer") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node[0], fkyaml::type_error);
        }

        SECTION("non-const node with an integer node") {
            REQUIRE_THROWS_AS(node[node_key], fkyaml::type_error);
        }

        SECTION("const node with an integer node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node[node_key], fkyaml::type_error);
        }
    }
}

//
// test cases for operators for comparisons between nodes
//

TEST_CASE("Node_EqualToOperator") {
    SECTION("The same type and value") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, nullptr},
            fkyaml::node {true, true},
            fkyaml::node {123, 123},
            fkyaml::node {3.14, 3.14},
            fkyaml::node {"foo", "foo"});
        REQUIRE(params[0] == params[1]);
    }

    SECTION("The same type but different values") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {false, 456, "bar"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"baz", 456}, {"qux", false}}},
            fkyaml::node {true, false},
            fkyaml::node {123, 456},
            fkyaml::node {3.14, 1.41},
            fkyaml::node {"foo", "bar"});
        REQUIRE_FALSE(params[0] == params[1]);
    }

    SECTION("Different types") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {{true, 123, "foo"}, nullptr},
            fkyaml::node {{true, 123, "foo"}, true},
            fkyaml::node {{true, 123, "foo"}, 123},
            fkyaml::node {{true, 123, "foo"}, 3.14},
            fkyaml::node {{true, 123, "foo"}, "foo"},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, nullptr},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, true},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 123},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 3.14},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, "foo"},
            fkyaml::node {nullptr, {true, 123, "foo"}},
            fkyaml::node {nullptr, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, true},
            fkyaml::node {nullptr, 123},
            fkyaml::node {nullptr, 3.14},
            fkyaml::node {nullptr, "foo"},
            fkyaml::node {true, {true, 123, "foo"}},
            fkyaml::node {true, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, nullptr},
            fkyaml::node {true, 123},
            fkyaml::node {true, 3.14},
            fkyaml::node {true, "foo"},
            fkyaml::node {123, {true, 123, "foo"}},
            fkyaml::node {123, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {123, nullptr},
            fkyaml::node {123, true},
            fkyaml::node {123, 3.14},
            fkyaml::node {123, "foo"},
            fkyaml::node {3.14, {true, 123, "foo"}},
            fkyaml::node {3.14, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {3.14, nullptr},
            fkyaml::node {3.14, true},
            fkyaml::node {3.14, 123},
            fkyaml::node {3.14, "foo"},
            fkyaml::node {"foo", {true, 123, "foo"}},
            fkyaml::node {"foo", {{"foo", 123}, {"bar", true}}},
            fkyaml::node {"foo", nullptr},
            fkyaml::node {"foo", true},
            fkyaml::node {"foo", 123},
            fkyaml::node {"foo", 3.14});
        REQUIRE_FALSE(params[0] == params[1]);
    }
}

TEST_CASE("Node_NotEqualToOperator") {
    SECTION("The same type and value") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, nullptr},
            fkyaml::node {true, true},
            fkyaml::node {123, 123},
            fkyaml::node {3.14, 3.14},
            fkyaml::node {"foo", "foo"});
        REQUIRE_FALSE(params[0] != params[1]);
    }

    SECTION("The same type but different values") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {false, 456, "bar"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"baz", 456}, {"qux", false}}},
            fkyaml::node {true, false},
            fkyaml::node {123, 456},
            fkyaml::node {3.14, 1.41},
            fkyaml::node {"foo", "bar"});
        REQUIRE(params[0] != params[1]);
    }

    SECTION("Different types") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {{true, 123, "foo"}, nullptr},
            fkyaml::node {{true, 123, "foo"}, true},
            fkyaml::node {{true, 123, "foo"}, 123},
            fkyaml::node {{true, 123, "foo"}, 3.14},
            fkyaml::node {{true, 123, "foo"}, "foo"},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, nullptr},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, true},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 123},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 3.14},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, "foo"},
            fkyaml::node {nullptr, {true, 123, "foo"}},
            fkyaml::node {nullptr, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, true},
            fkyaml::node {nullptr, 123},
            fkyaml::node {nullptr, 3.14},
            fkyaml::node {nullptr, "foo"},
            fkyaml::node {true, {true, 123, "foo"}},
            fkyaml::node {true, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, nullptr},
            fkyaml::node {true, 123},
            fkyaml::node {true, 3.14},
            fkyaml::node {true, "foo"},
            fkyaml::node {123, {true, 123, "foo"}},
            fkyaml::node {123, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {123, nullptr},
            fkyaml::node {123, true},
            fkyaml::node {123, 3.14},
            fkyaml::node {123, "foo"},
            fkyaml::node {3.14, {true, 123, "foo"}},
            fkyaml::node {3.14, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {3.14, nullptr},
            fkyaml::node {3.14, true},
            fkyaml::node {3.14, 123},
            fkyaml::node {3.14, "foo"},
            fkyaml::node {"foo", {true, 123, "foo"}},
            fkyaml::node {"foo", {{"foo", 123}, {"bar", true}}},
            fkyaml::node {"foo", nullptr},
            fkyaml::node {"foo", true},
            fkyaml::node {"foo", 123},
            fkyaml::node {"foo", 3.14});
        REQUIRE(params[0] != params[1]);
    }
}

TEST_CASE("Node_LessThanOperator") {
    SECTION("The same type and value") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, nullptr},
            fkyaml::node {true, true},
            fkyaml::node {123, 123},
            fkyaml::node {3.14, 3.14},
            fkyaml::node {"foo", "foo"});
        REQUIRE_FALSE(params[0] < params[1]);
    }

    SECTION("The same type and the target value is less than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123}, {true, 123, "foo"}},
            fkyaml::node {{{"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {false, true},
            fkyaml::node {123, 456},
            fkyaml::node {3.14, 4.25},
            fkyaml::node {"foo", "qux"});
        REQUIRE(params[0] < params[1]);
    }

    SECTION("The same type but the target value is greater than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"bar", true}}},
            fkyaml::node {true, false},
            fkyaml::node {456, 123},
            fkyaml::node {4.25, 3.14},
            fkyaml::node {"qux", "foo"});
        REQUIRE_FALSE(params[0] < params[1]);
    }

    SECTION("The numeric value of the target type is less than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {{true, 123, "foo"}, nullptr},
            fkyaml::node {{true, 123, "foo"}, true},
            fkyaml::node {{true, 123, "foo"}, 123},
            fkyaml::node {{true, 123, "foo"}, 3.14},
            fkyaml::node {{true, 123, "foo"}, "foo"},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, nullptr},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, true},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 123},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 3.14},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, "foo"},
            fkyaml::node {nullptr, true},
            fkyaml::node {nullptr, 123},
            fkyaml::node {nullptr, 3.14},
            fkyaml::node {nullptr, "foo"},
            fkyaml::node {true, 123},
            fkyaml::node {true, 3.14},
            fkyaml::node {true, "foo"},
            fkyaml::node {123, 3.14},
            fkyaml::node {123, "foo"},
            fkyaml::node {3.14, "foo"});
        REQUIRE(params[0] < params[1]);
    }

    SECTION("The numeric value of the target type is greater than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {true, 123, "foo"}},
            fkyaml::node {nullptr, {true, 123, "foo"}},
            fkyaml::node {nullptr, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, {true, 123, "foo"}},
            fkyaml::node {true, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, nullptr},
            fkyaml::node {123, {true, 123, "foo"}},
            fkyaml::node {123, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {123, nullptr},
            fkyaml::node {123, true},
            fkyaml::node {3.14, {true, 123, "foo"}},
            fkyaml::node {3.14, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {3.14, nullptr},
            fkyaml::node {3.14, true},
            fkyaml::node {3.14, 123},
            fkyaml::node {"foo", {true, 123, "foo"}},
            fkyaml::node {"foo", {{"foo", 123}, {"bar", true}}},
            fkyaml::node {"foo", nullptr},
            fkyaml::node {"foo", true},
            fkyaml::node {"foo", 123},
            fkyaml::node {"foo", 3.14});
        REQUIRE_FALSE(params[0] < params[1]);
    }
}

TEST_CASE("Node_LessThanOrEqualToOperator") {
    SECTION("The same type and value") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, nullptr},
            fkyaml::node {true, true},
            fkyaml::node {123, 123},
            fkyaml::node {3.14, 3.14},
            fkyaml::node {"foo", "foo"});
        REQUIRE(params[0] <= params[1]);
    }

    SECTION("The same type and the target value is less than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123}, {true, 123, "foo"}},
            fkyaml::node {{{"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {false, true},
            fkyaml::node {123, 456},
            fkyaml::node {3.14, 4.25},
            fkyaml::node {"foo", "qux"});
        REQUIRE(params[0] <= params[1]);
    }

    SECTION("The same type but the target value is greater than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"bar", true}}},
            fkyaml::node {true, false},
            fkyaml::node {456, 123},
            fkyaml::node {4.25, 3.14},
            fkyaml::node {"qux", "foo"});
        REQUIRE_FALSE(params[0] <= params[1]);
    }

    SECTION("The numeric value of the target type is less than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {{true, 123, "foo"}, nullptr},
            fkyaml::node {{true, 123, "foo"}, true},
            fkyaml::node {{true, 123, "foo"}, 123},
            fkyaml::node {{true, 123, "foo"}, 3.14},
            fkyaml::node {{true, 123, "foo"}, "foo"},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, nullptr},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, true},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 123},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 3.14},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, "foo"},
            fkyaml::node {nullptr, true},
            fkyaml::node {nullptr, 123},
            fkyaml::node {nullptr, 3.14},
            fkyaml::node {nullptr, "foo"},
            fkyaml::node {true, 123},
            fkyaml::node {true, 3.14},
            fkyaml::node {true, "foo"},
            fkyaml::node {123, 3.14},
            fkyaml::node {123, "foo"},
            fkyaml::node {3.14, "foo"});
        REQUIRE(params[0] <= params[1]);
    }

    SECTION("The numeric value of the target type is greater than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {true, 123, "foo"}},
            fkyaml::node {nullptr, {true, 123, "foo"}},
            fkyaml::node {nullptr, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, {true, 123, "foo"}},
            fkyaml::node {true, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, nullptr},
            fkyaml::node {123, {true, 123, "foo"}},
            fkyaml::node {123, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {123, nullptr},
            fkyaml::node {123, true},
            fkyaml::node {3.14, {true, 123, "foo"}},
            fkyaml::node {3.14, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {3.14, nullptr},
            fkyaml::node {3.14, true},
            fkyaml::node {3.14, 123},
            fkyaml::node {"foo", {true, 123, "foo"}},
            fkyaml::node {"foo", {{"foo", 123}, {"bar", true}}},
            fkyaml::node {"foo", nullptr},
            fkyaml::node {"foo", true},
            fkyaml::node {"foo", 123},
            fkyaml::node {"foo", 3.14});
        REQUIRE_FALSE(params[0] <= params[1]);
    }
}

TEST_CASE("Node_GreaterThanOperator") {
    SECTION("The same type and value") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, nullptr},
            fkyaml::node {true, true},
            fkyaml::node {123, 123},
            fkyaml::node {3.14, 3.14},
            fkyaml::node {"foo", "foo"});
        REQUIRE_FALSE(params[0] > params[1]);
    }

    SECTION("The same type and the target value is less than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123}, {true, 123, "foo"}},
            fkyaml::node {{{"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {false, true},
            fkyaml::node {123, 456},
            fkyaml::node {3.14, 4.25},
            fkyaml::node {"foo", "qux"});
        REQUIRE_FALSE(params[0] > params[1]);
    }

    SECTION("The same type but the target value is greater than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"bar", true}}},
            fkyaml::node {true, false},
            fkyaml::node {456, 123},
            fkyaml::node {4.25, 3.14},
            fkyaml::node {"qux", "foo"});
        REQUIRE(params[0] > params[1]);
    }

    SECTION("The numeric value of the target type is less than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {{true, 123, "foo"}, nullptr},
            fkyaml::node {{true, 123, "foo"}, true},
            fkyaml::node {{true, 123, "foo"}, 123},
            fkyaml::node {{true, 123, "foo"}, 3.14},
            fkyaml::node {{true, 123, "foo"}, "foo"},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, nullptr},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, true},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 123},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 3.14},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, "foo"},
            fkyaml::node {nullptr, true},
            fkyaml::node {nullptr, 123},
            fkyaml::node {nullptr, 3.14},
            fkyaml::node {nullptr, "foo"},
            fkyaml::node {true, 123},
            fkyaml::node {true, 3.14},
            fkyaml::node {true, "foo"},
            fkyaml::node {123, 3.14},
            fkyaml::node {123, "foo"},
            fkyaml::node {3.14, "foo"});
        REQUIRE_FALSE(params[0] > params[1]);
    }

    SECTION("The numeric value of the target type is greater than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {true, 123, "foo"}},
            fkyaml::node {nullptr, {true, 123, "foo"}},
            fkyaml::node {nullptr, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, {true, 123, "foo"}},
            fkyaml::node {true, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, nullptr},
            fkyaml::node {123, {true, 123, "foo"}},
            fkyaml::node {123, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {123, nullptr},
            fkyaml::node {123, true},
            fkyaml::node {3.14, {true, 123, "foo"}},
            fkyaml::node {3.14, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {3.14, nullptr},
            fkyaml::node {3.14, true},
            fkyaml::node {3.14, 123},
            fkyaml::node {"foo", {true, 123, "foo"}},
            fkyaml::node {"foo", {{"foo", 123}, {"bar", true}}},
            fkyaml::node {"foo", nullptr},
            fkyaml::node {"foo", true},
            fkyaml::node {"foo", 123},
            fkyaml::node {"foo", 3.14});
        REQUIRE(params[0] > params[1]);
    }
}

TEST_CASE("Node_GreaterThanOrEqualToOperator") {
    SECTION("The same type and value") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123, "foo"}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {nullptr, nullptr},
            fkyaml::node {true, true},
            fkyaml::node {123, 123},
            fkyaml::node {3.14, 3.14},
            fkyaml::node {"foo", "foo"});
        REQUIRE(params[0] >= params[1]);
    }

    SECTION("The same type and the target value is less than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123}, {true, 123, "foo"}},
            fkyaml::node {{{"bar", true}}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {false, true},
            fkyaml::node {123, 456},
            fkyaml::node {3.14, 4.25},
            fkyaml::node {"foo", "qux"});
        REQUIRE_FALSE(params[0] >= params[1]);
    }

    SECTION("The same type but the target value is greater than the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {true, 123}},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {{"bar", true}}},
            fkyaml::node {true, false},
            fkyaml::node {456, 123},
            fkyaml::node {4.25, 3.14},
            fkyaml::node {"qux", "foo"});
        REQUIRE(params[0] >= params[1]);
    }

    SECTION("The numeric value of the target type is less than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{true, 123, "foo"}, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {{true, 123, "foo"}, nullptr},
            fkyaml::node {{true, 123, "foo"}, true},
            fkyaml::node {{true, 123, "foo"}, 123},
            fkyaml::node {{true, 123, "foo"}, 3.14},
            fkyaml::node {{true, 123, "foo"}, "foo"},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, nullptr},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, true},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 123},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, 3.14},
            fkyaml::node {{{"foo", 123}, {"bar", true}}, "foo"},
            fkyaml::node {nullptr, true},
            fkyaml::node {nullptr, 123},
            fkyaml::node {nullptr, 3.14},
            fkyaml::node {nullptr, "foo"},
            fkyaml::node {true, 123},
            fkyaml::node {true, 3.14},
            fkyaml::node {true, "foo"},
            fkyaml::node {123, 3.14},
            fkyaml::node {123, "foo"},
            fkyaml::node {3.14, "foo"});
        REQUIRE_FALSE(params[0] >= params[1]);
    }

    SECTION("The numeric value of the target type is greater than that of the compared one") {
        auto params = GENERATE(
            fkyaml::node {{{"foo", 123}, {"bar", true}}, {true, 123, "foo"}},
            fkyaml::node {nullptr, {true, 123, "foo"}},
            fkyaml::node {nullptr, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, {true, 123, "foo"}},
            fkyaml::node {true, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {true, nullptr},
            fkyaml::node {123, {true, 123, "foo"}},
            fkyaml::node {123, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {123, nullptr},
            fkyaml::node {123, true},
            fkyaml::node {3.14, {true, 123, "foo"}},
            fkyaml::node {3.14, {{"foo", 123}, {"bar", true}}},
            fkyaml::node {3.14, nullptr},
            fkyaml::node {3.14, true},
            fkyaml::node {3.14, 123},
            fkyaml::node {"foo", {true, 123, "foo"}},
            fkyaml::node {"foo", {{"foo", 123}, {"bar", true}}},
            fkyaml::node {"foo", nullptr},
            fkyaml::node {"foo", true},
            fkyaml::node {"foo", 123},
            fkyaml::node {"foo", 3.14});
        REQUIRE(params[0] >= params[1]);
    }
}

//
// test cases for type property getter/checkers
//

TEST_CASE("Node_GetType") {
    using NodeTypePair = std::pair<fkyaml::node, fkyaml::node_type>;
    auto type_pair = GENERATE(
        NodeTypePair(fkyaml::node::sequence(), fkyaml::node_type::SEQUENCE),
        NodeTypePair(fkyaml::node::mapping(), fkyaml::node_type::MAPPING),
        NodeTypePair(fkyaml::node(), fkyaml::node_type::NULL_OBJECT),
        NodeTypePair(fkyaml::node(false), fkyaml::node_type::BOOLEAN),
        NodeTypePair(fkyaml::node(0), fkyaml::node_type::INTEGER),
        NodeTypePair(fkyaml::node(0.0), fkyaml::node_type::FLOAT),
        NodeTypePair(fkyaml::node(""), fkyaml::node_type::STRING));

    SECTION("non-alias node types") {
        REQUIRE(type_pair.first.get_type() == type_pair.second);
    }

    SECTION("alias node types") {
        type_pair.first.add_anchor_name("anchor_name");
        fkyaml::node alias = fkyaml::node::alias_of(type_pair.first);
        REQUIRE(alias.get_type() == type_pair.second);
    }
}

TEST_CASE("Node_Type(deprecated)") {
    using NodeTypePair = std::pair<fkyaml::node, fkyaml::node::node_t>;
    auto type_pair = GENERATE(
        NodeTypePair(fkyaml::node::sequence(), fkyaml::node::node_t::SEQUENCE),
        NodeTypePair(fkyaml::node::mapping(), fkyaml::node::node_t::MAPPING),
        NodeTypePair(fkyaml::node(), fkyaml::node::node_t::NULL_OBJECT),
        NodeTypePair(fkyaml::node(false), fkyaml::node::node_t::BOOLEAN),
        NodeTypePair(fkyaml::node(0), fkyaml::node::node_t::INTEGER),
        NodeTypePair(fkyaml::node(0.0), fkyaml::node::node_t::FLOAT_NUMBER),
        NodeTypePair(fkyaml::node(""), fkyaml::node::node_t::STRING));

    SECTION("non-alias node types") {
        REQUIRE(type_pair.first.type() == type_pair.second);
    }

    SECTION("alias node types") {
        type_pair.first.add_anchor_name("anchor_name");
        fkyaml::node alias = fkyaml::node::alias_of(type_pair.first);
        REQUIRE(alias.type() == type_pair.second);
    }
}

TEST_CASE("Node_IsSequence") {
    SECTION("sequence node type") {
        fkyaml::node node = fkyaml::node::sequence();

        SECTION("non-alias sequence node type") {
            REQUIRE(node.is_sequence());
        }

        SECTION("alias sequence node type") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_sequence());
        }
    }

    SECTION("non-sequence node types") {
        auto node = GENERATE(
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-sequence node types") {
            REQUIRE_FALSE(node.is_sequence());
        }

        SECTION("alias non-sequence node types.") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_sequence());
        }
    }
}

TEST_CASE("Node_IsMapping") {
    SECTION("mapping node type") {
        fkyaml::node node = fkyaml::node::mapping();

        SECTION("non-alias mapping node type") {
            REQUIRE(node.is_mapping());
        }

        SECTION("alias mapping node type") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_mapping());
        }
    }

    SECTION("non-mapping node types") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-mapping node types") {
            REQUIRE_FALSE(node.is_mapping());
        }

        SECTION("alias non-mapping node types.") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_mapping());
        }
    }
}

TEST_CASE("Node_IsNull") {
    SECTION("null node type") {
        fkyaml::node node = fkyaml::node();

        SECTION("non-alias null node type") {
            REQUIRE(node.is_null());
        }

        SECTION("alias null node type") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_null());
        }
    }

    SECTION("non-null node types") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-null node types") {
            REQUIRE_FALSE(node.is_null());
        }

        SECTION("alias non-null node types") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_null());
        }
    }
}

TEST_CASE("Node_IsBoolean") {
    SECTION("boolean node type") {
        fkyaml::node node = false;

        SECTION("non-alias boolean node type") {
            REQUIRE(node.is_boolean());
        }

        SECTION("alias boolean node type") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_boolean());
        }
    }

    SECTION("non-boolean node types") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-boolean node types") {
            REQUIRE_FALSE(node.is_boolean());
        }

        SECTION("alias non-boolean node types") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_boolean());
        }
    }
}

TEST_CASE("Node_IsInteger") {
    SECTION("integer node type.") {
        fkyaml::node node = 0;

        SECTION("non-alias integer node type.") {
            REQUIRE(node.is_integer());
        }

        SECTION("alias integer node type.") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_integer());
        }
    }

    SECTION("non-integer node types.") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-integer node types") {
            REQUIRE_FALSE(node.is_integer());
        }

        SECTION("alias non-integer node types") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_integer());
        }
    }
}

TEST_CASE("Node_IsFloatNumber") {
    SECTION("float number node type") {
        fkyaml::node node = 0.0;

        SECTION("non-alias float number node type") {
            REQUIRE(node.is_float_number());
        }

        SECTION("alias float number node type") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_float_number());
        }
    }

    SECTION("non-float-number node types") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(""));

        SECTION("non-alias non-float-number node types") {
            REQUIRE_FALSE(node.is_float_number());
        }

        SECTION("alias non-float-number node types") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_float_number());
        }
    }
}

TEST_CASE("Node_IsString") {
    SECTION("string node type") {
        fkyaml::node node = "";

        SECTION("non-alias string node type") {
            REQUIRE(node.is_string());
        }

        SECTION("alias string node type") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_string());
        }
    }

    SECTION("non-string node types") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0));

        SECTION("non-alias non-string node types") {
            REQUIRE_FALSE(node.is_string());
        }

        SECTION("alias non-string node types") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_string());
        }
    }
}

TEST_CASE("Node_IsScalar") {
    SECTION("scalar node types") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));

        SECTION("non-alias scalar node types") {
            REQUIRE(node.is_scalar());
        }

        SECTION("alias scalar node types") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE(alias.is_scalar());
        }
    }

    SECTION("non-scalar node types") {
        auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping());

        SECTION("non-alias non-scalar node types") {
            REQUIRE_FALSE(node.is_scalar());
        }

        SECTION("alias non-scalar node types") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_FALSE(alias.is_scalar());
        }
    }
}

//
// test cases for emptiness checker
//

TEST_CASE("Node_Empty") {
    SECTION("container and string scalar nodes") {
        SECTION("empty node") {
            auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping(), fkyaml::node(""));

            SECTION("empty non-alias node") {
                REQUIRE_NOTHROW(node.empty());
                REQUIRE(node.empty());
            }

            SECTION("empty alias node") {
                node.add_anchor_name("anchor_name");
                fkyaml::node alias = fkyaml::node::alias_of(node);
                REQUIRE_NOTHROW(alias.empty());
                REQUIRE(alias.empty());
            }
        }

        SECTION("non-empty node") {
            auto node = GENERATE(
                fkyaml::node::sequence(fkyaml::node::sequence_type(3)),
                fkyaml::node::mapping(fkyaml::node::mapping_type {{"test", fkyaml::node()}}),
                fkyaml::node("test"));

            SECTION("non-empty non-alias node") {
                REQUIRE_NOTHROW(node.empty());
                REQUIRE_FALSE(node.empty());
            }

            SECTION("non-empty alias node") {
                node.add_anchor_name("anchor_name");
                fkyaml::node alias = fkyaml::node::alias_of(node);
                REQUIRE_NOTHROW(alias.empty());
                REQUIRE_FALSE(alias.empty());
            }
        }
    }

    SECTION("non-string scalar nodes") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0));

        SECTION("non-const non-alias node") {
            REQUIRE_THROWS_AS(node.empty(), fkyaml::type_error);
        }

        SECTION("const non-alias node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.empty(), fkyaml::type_error);
        }

        SECTION("non-const alias node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.empty(), fkyaml::type_error);
        }

        SECTION("const alias node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.empty(), fkyaml::type_error);
        }
    }
}

//
// test cases for mapping key existence checker
//

TEST_CASE("Node_Contains") {
    SECTION("mapping") {
        fkyaml::node node = fkyaml::node::mapping({{"test", fkyaml::node()}});

        SECTION("mapping node with a string key") {
            REQUIRE(node.contains("test"));
        }

#ifdef FK_YAML_HAS_CXX_17
        SECTION("mapping node with a string view key") {
            using namespace std::string_view_literals;
            REQUIRE(node.contains("test"sv));
        }
#endif

        SECTION("mapping node with a string node key") {
            fkyaml::node node_key = "test";
            REQUIRE(node.contains(node_key));
        }
    }

    SECTION("non-mapping") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-mapping node with a key") {
            REQUIRE_FALSE(node.contains("test"));
        }

        SECTION("non-mapping node with a node key") {
            fkyaml::node node_key = "test";
            REQUIRE_FALSE(node.contains(node_key));
        }
    }
}

//
// test cases for container size getter
//

TEST_CASE("Node_Size") {
    SECTION("container and string scalar nodes") {
        auto node = GENERATE(
            fkyaml::node::sequence({fkyaml::node(), fkyaml::node(), fkyaml::node()}),
            fkyaml::node::mapping({{"test0", fkyaml::node()}, {"test1", fkyaml::node()}, {"test2", fkyaml::node()}}),
            fkyaml::node("tmp"));

        SECTION("non-const non-alias node") {
            REQUIRE_NOTHROW(node.size());
            REQUIRE(node.size() == 3);
        }

        SECTION("const node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(node.size());
            REQUIRE(const_node.size() == 3);
        }

        SECTION("alias node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.size());
            REQUIRE(alias.size() == 3);
        }

        SECTION("const alias node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.size());
            REQUIRE(alias.size() == 3);
        }
    }

    SECTION("non-string scalar nodes") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0));

        SECTION("non-const non-alias node") {
            REQUIRE_THROWS_AS(node.size(), fkyaml::type_error);
        }

        SECTION("const non-alias node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.size(), fkyaml::type_error);
        }

        SECTION("non-const alias node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.size(), fkyaml::type_error);
        }

        SECTION("const alias node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.size(), fkyaml::type_error);
        }
    }
}

//
// test cases for container element accessor with bounds checks
//

TEST_CASE("Node_At") {
    SECTION("mapping with existing key argument") {
        fkyaml::node::mapping_type map {{"test", fkyaml::node()}};

        SECTION("non-const string value") {
            fkyaml::node node = fkyaml::node::mapping(map);

            SECTION("non-const lvalue string value") {
                std::string key = "test";
                REQUIRE_NOTHROW(node.at(key));
                REQUIRE(node[key].is_null());
            }

            SECTION("non-const rvalue string value") {
                REQUIRE_NOTHROW(node.at("test"));
                REQUIRE(node.at("test").is_null());
            }
        }

        SECTION("const string value") {
            const fkyaml::node node = fkyaml::node::mapping(map);
            std::string key = "test";

            SECTION("const lvalue string node") {
                REQUIRE_NOTHROW(node.at(key));
            }

            SECTION("const rvalue string node") {
                REQUIRE_NOTHROW(node.at("test"));
            }
        }

#ifdef FK_YAML_HAS_CXX_17
        SECTION("string view value") {
            std::string_view key = "test";
            REQUIRE(map.at(key).is_null());
        }
#endif

        SECTION("non-const string node") {
            fkyaml::node node = fkyaml::node::mapping(map);
            fkyaml::node node_key = "test";

            SECTION("non-const lvalue string node") {
                REQUIRE_NOTHROW(node.at(node_key));
            }

            SECTION("non-const rvalue string node") {
                REQUIRE_NOTHROW(node.at(std::move(node_key)));
            }
        }

        SECTION("const string node") {
            const fkyaml::node node = fkyaml::node::mapping(map);
            fkyaml::node node_key = "test";

            SECTION("non-const lvalue string node") {
                REQUIRE_NOTHROW(node.at(node_key));
            }

            SECTION("non-const rvalue string node") {
                REQUIRE_NOTHROW(node.at(std::move(node_key)));
            }
        }
    }

    SECTION("mapping with non-existing key argument") {
        fkyaml::node node = {{"foo", 123}};

        SECTION("compatible type objects") {
            REQUIRE_THROWS_AS(node.at(fkyaml::node::sequence_type()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node::mapping_type()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(nullptr), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(true), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(123), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(3.14), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at("bar"), fkyaml::out_of_range);
        }

        SECTION("compatible type objects (const)") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::sequence_type()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::mapping_type()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(nullptr), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(true), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(123), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(3.14), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at("bar"), fkyaml::out_of_range);
        }

        SECTION("basic_node objects") {
            REQUIRE_THROWS_AS(node.at(fkyaml::node::sequence()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node::mapping()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node(true)), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node(123)), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node(3.14)), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node("bar")), fkyaml::out_of_range);
        }

        SECTION("basic_node objects (const)") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::sequence()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::mapping()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node()), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node(true)), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node(123)), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node(3.14)), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node("bar")), fkyaml::out_of_range);
        }
    }

    SECTION("sequence with integer argument") {
        fkyaml::node node = fkyaml::node::sequence();
        node.as_seq().emplace_back();

        SECTION("non-const integer value") {
            REQUIRE_NOTHROW(node.at(0));
        }

        SECTION("const integer value") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.at(0));
        }

        SECTION("non-const integer node") {
            REQUIRE_NOTHROW(node.at(fkyaml::node(0)));
        }

        SECTION("const integer node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.at(fkyaml::node(0)));
        }
    }

    SECTION("sequence with non-integer argument") {
        fkyaml::node node = fkyaml::node::sequence();
        node.as_seq().emplace_back();

        SECTION("non-const node with a non-integer value") {
            REQUIRE_THROWS_AS(node.at(fkyaml::node::sequence_type()), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(fkyaml::node::mapping_type()), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(nullptr), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(false), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(0.0), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(""), fkyaml::type_error);
        }

        SECTION("const node with a non-integer value") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::sequence_type()), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::mapping_type()), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(nullptr), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(false), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(0.0), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(""), fkyaml::type_error);
        }

        SECTION("non-const node with a non-integer node") {
            REQUIRE_THROWS_AS(node.at(fkyaml::node::mapping()), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(fkyaml::node::sequence()), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(fkyaml::node()), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(fkyaml::node(false)), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.at(fkyaml::node("")), fkyaml::type_error);
        }

        SECTION("const node with a non-integer node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::mapping()), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node::sequence()), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node()), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node(false)), fkyaml::type_error);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node("")), fkyaml::type_error);
        }
    }

    SECTION("sequence with out-of-bounds integer argument") {
        fkyaml::node node = fkyaml::node::sequence();
        node.as_seq().emplace_back();

        SECTION("non-const argument") {
            REQUIRE_THROWS_AS(node.at(1), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(node.at(fkyaml::node(1)), fkyaml::out_of_range);
        }

        SECTION("const argument") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.at(1), fkyaml::out_of_range);
            REQUIRE_THROWS_AS(const_node.at(fkyaml::node(1)), fkyaml::out_of_range);
        }
    }

    SECTION("scalar") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));
        fkyaml::node node_key = 0;

        SECTION("non-const node with an integer.") {
            REQUIRE_THROWS_AS(node.at(0), fkyaml::type_error);
        }

        SECTION("const node with an integer") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.at(0), fkyaml::type_error);
        }

        SECTION("non-const node with an integer node") {
            REQUIRE_THROWS_AS(node.at(node_key), fkyaml::type_error);
        }

        SECTION("const node with an integer node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.at(node_key), fkyaml::type_error);
        }
    }
}

//
// test cases for YAML version property getter/setter
//

TEST_CASE("Node_SetYamlVersionType") {
    fkyaml::node node;
    node.set_yaml_version_type(fkyaml::yaml_version_type::VERSION_1_1);
    REQUIRE(node.get_yaml_version_type() == fkyaml::yaml_version_type::VERSION_1_1);

    node.set_yaml_version_type(fkyaml::yaml_version_type::VERSION_1_2);
    REQUIRE(node.get_yaml_version_type() == fkyaml::yaml_version_type::VERSION_1_2);
}

TEST_CASE("Node_GetYamlVersionType") {
    fkyaml::node node;
    REQUIRE(node.get_yaml_version_type() == fkyaml::yaml_version_type::VERSION_1_2);

    node.set_yaml_version_type(fkyaml::yaml_version_type::VERSION_1_1);
    REQUIRE(node.get_yaml_version_type() == fkyaml::yaml_version_type::VERSION_1_1);
}

TEST_CASE("Node_SetYamlVersion(deprecated)") {
    fkyaml::node node;
    node.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_1);
    REQUIRE(node.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_1);

    node.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_2);
    REQUIRE(node.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_2);
}

TEST_CASE("Node_GetYamlVersion(deprecated)") {
    fkyaml::node node;
    REQUIRE(node.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_2);

    node.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_1);
    REQUIRE(node.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_1);
}

//
// test cases for anchor related APIs
//

TEST_CASE("Node_IsAlias") {
    fkyaml::node node;

    SECTION("without anchor name") {
        REQUIRE_FALSE(node.is_alias());
    }

    SECTION("an anchor node") {
        node.add_anchor_name("anchor");
        REQUIRE_FALSE(node.is_alias());
    }

    SECTION("an alias node") {
        fkyaml::node anchor;
        anchor.add_anchor_name("anchor");
        node = fkyaml::node::alias_of(anchor);
        REQUIRE(node.is_alias());
    }
}

TEST_CASE("Node_IsAnchor") {
    fkyaml::node node;

    SECTION("without anchor name") {
        REQUIRE_FALSE(node.is_anchor());
    }

    SECTION("an anchor node") {
        node.add_anchor_name("anchor");
        REQUIRE(node.is_anchor());
    }

    SECTION("an alias node") {
        fkyaml::node anchor;
        anchor.add_anchor_name("anchor");
        node = fkyaml::node::alias_of(anchor);
        REQUIRE_FALSE(node.is_anchor());
    }
}

TEST_CASE("Node_HasAnchorName") {
    fkyaml::node node;

    SECTION("without anchor name") {
        REQUIRE_FALSE(node.has_anchor_name());
    }

    SECTION("with anchor name") {
        node.add_anchor_name("anchor_name");
        REQUIRE(node.has_anchor_name());
    }
}

TEST_CASE("Node_GetAnchorName") {
    fkyaml::node node;

    SECTION("without anchor name") {
        REQUIRE_THROWS_AS(node.get_anchor_name(), fkyaml::exception);
    }

    SECTION("with anchor name") {
        node.add_anchor_name("anchor_name");
        REQUIRE_NOTHROW(node.get_anchor_name());
        REQUIRE(node.get_anchor_name().compare("anchor_name") == 0);
    }
}

TEST_CASE("Node_AddAnchorName") {
    fkyaml::node node;
    std::string anchor_name = "anchor_name";

    SECTION("lvalue anchor name") {
        node.add_anchor_name(anchor_name);
        REQUIRE_NOTHROW(node.get_anchor_name());
        REQUIRE(node.get_anchor_name().compare("anchor_name") == 0);
    }

    SECTION("rvalue anchor name") {
        node.add_anchor_name(std::move(anchor_name));
        REQUIRE_NOTHROW(node.get_anchor_name());
        REQUIRE(node.get_anchor_name().compare("anchor_name") == 0);
    }

    SECTION("overwrite an existing anchor name with lvalue anchor name") {
        node.add_anchor_name(anchor_name);
        std::string overwritten_name = "overwritten_name";
        node.add_anchor_name(overwritten_name);
        REQUIRE_NOTHROW(node.get_anchor_name());
        REQUIRE_FALSE(node.get_anchor_name().compare("anchor_name") == 0);
        REQUIRE(node.get_anchor_name().compare("overwritten_name") == 0);
    }

    SECTION("overwrite an existing anchor name with rvalue anchor name") {
        node.add_anchor_name(anchor_name);
        node.add_anchor_name("overwritten_name");
        REQUIRE_NOTHROW(node.get_anchor_name());
        REQUIRE_FALSE(node.get_anchor_name().compare("anchor_name") == 0);
        REQUIRE(node.get_anchor_name().compare("overwritten_name") == 0);
    }
}

//
// test cases for tag related APIs
//

TEST_CASE("Node_HasTagName") {
    fkyaml::node node;

    SECTION("node with tag name") {
        node.add_tag_name("tag_name");
        REQUIRE(node.has_tag_name());
    }

    SECTION("node without tag name") {
        REQUIRE_FALSE(node.has_tag_name());
    }
}

TEST_CASE("Node_GetTagName") {
    fkyaml::node node;

    SECTION("node without tag name.") {
        REQUIRE_THROWS_AS(node.get_tag_name(), fkyaml::exception);
    }

    SECTION("node with tag name.") {
        node.add_tag_name("tag_name");
        REQUIRE_NOTHROW(node.get_tag_name());
        REQUIRE(node.get_tag_name() == "tag_name");
    }
}

TEST_CASE("Node_AddTagName") {
    fkyaml::node node;
    std::string tag_name = "tag_name";

    SECTION("lvalue tag name") {
        node.add_tag_name(tag_name);
        REQUIRE_NOTHROW(node.get_tag_name());
        REQUIRE(node.get_tag_name().compare("tag_name") == 0);
    }

    SECTION("rvalue tag name") {
        node.add_tag_name(std::move(tag_name));
        REQUIRE_NOTHROW(node.get_tag_name());
        REQUIRE(node.get_tag_name().compare("tag_name") == 0);
    }

    SECTION("overwrite an existing tag name") {
        node.add_tag_name(tag_name);
        node.add_tag_name("overwritten_name");
        REQUIRE_NOTHROW(node.get_tag_name());
        REQUIRE_FALSE(node.get_tag_name().compare("tag_name") == 0);
        REQUIRE(node.get_tag_name().compare("overwritten_name") == 0);
    }
}

//
// test cases for value getters (copy)
//

struct not_default_constructible {
    not_default_constructible() = delete;
    explicit not_default_constructible(int i)
        : value(i) {
    }
    not_default_constructible(const not_default_constructible&) = default;
    int value;
};
inline void from_node(const fkyaml::node& n, not_default_constructible& ndc) noexcept {
    ndc.value = n.get_value<int>();
}

struct string_wrap {
    string_wrap() = default;
    string_wrap& operator=(const std::string& _str) {
        str = _str;
        return *this;
    }
    std::string str {};
};

template <typename T, typename U>
using get_value_fn_t = decltype(std::declval<T>().template get_value<U>());

TEST_CASE("Node_GetValue_GetValueInplace") {
    SECTION("sequence") {
        fkyaml::node node {true, false};

        SECTION("sequence value (1D C-style array)") {
            STATIC_REQUIRE_FALSE(fkyaml::detail::is_detected<get_value_fn_t, const fkyaml::node&, int(&)[2]>::value);

            int ints_1d[2] {0, 0};
            fkyaml::node array_1d {1, 2};
            array_1d.get_value_inplace(ints_1d);
            REQUIRE(ints_1d[0] == 1);
            REQUIRE(ints_1d[1] == 2);
        }

        SECTION("sequence value (2D C-style array)") {
            STATIC_REQUIRE_FALSE(fkyaml::detail::is_detected<get_value_fn_t, const fkyaml::node&, int(&)[3][3]>::value);

            int ints_2d[3][3] {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
            fkyaml::node array_2d {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
            array_2d.get_value_inplace(ints_2d);
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    REQUIRE(ints_2d[i][j] == i * 3 + j + 1);
                }
            }
        }

        SECTION("sequence value (3D C-style array)") {
            STATIC_REQUIRE_FALSE(
                fkyaml::detail::is_detected<get_value_fn_t, const fkyaml::node&, int(&)[3][3][3]>::value);

            int ints_3d[3][3][3] {
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
                {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}};
            fkyaml::node array_3d {
                {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}},
                {{10, 11, 12}, {13, 14, 15}, {16, 17, 18}},
                {{19, 20, 21}, {22, 23, 24}, {25, 26, 27}}};
            array_3d.get_value_inplace(ints_3d);
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    for (int k = 0; k < 3; k++) {
                        REQUIRE(ints_3d[i][j][k] == i * 9 + j * 3 + k + 1);
                    }
                }
            }
        }

        SECTION("sequence value (std::vector)") {
            auto vector_node = node.get_value<std::vector<fkyaml::node>>();
            REQUIRE(vector_node.size() == 2);
            REQUIRE(vector_node[0].is_boolean());
            REQUIRE(vector_node[0].get_value<bool>() == true);
            REQUIRE(vector_node[1].is_boolean());
            REQUIRE(vector_node[1].get_value<bool>() == false);

            std::vector<fkyaml::node> vector_node_inplace {};
            node.get_value_inplace(vector_node_inplace);
            REQUIRE(vector_node_inplace.size() == 2);
            REQUIRE(vector_node_inplace[0].is_boolean());
            REQUIRE(vector_node_inplace[0].get_value<bool>() == true);
            REQUIRE(vector_node_inplace[1].is_boolean());
            REQUIRE(vector_node_inplace[1].get_value<bool>() == false);

            auto vector_bool = node.get_value<std::vector<bool>>();
            REQUIRE(vector_bool.size() == 2);
            REQUIRE(vector_bool[0] == true);
            REQUIRE(vector_bool[1] == false);

            std::vector<bool> vector_bool_inplace {};
            node.get_value_inplace(vector_bool_inplace);
            REQUIRE(vector_bool_inplace.size() == 2);
            REQUIRE(vector_bool_inplace[0] == true);
            REQUIRE(vector_bool_inplace[1] == false);
        }

        SECTION("sequence value (std::array)") {
            auto array_node = node.get_value<std::array<fkyaml::node, 2>>();
            REQUIRE(array_node[0].is_boolean());
            REQUIRE(array_node[0].get_value<bool>() == true);
            REQUIRE(array_node[1].is_boolean());
            REQUIRE(array_node[1].get_value<bool>() == false);

            std::array<fkyaml::node, 2> array_node_inplace {{}};
            node.get_value_inplace(array_node_inplace);
            REQUIRE(array_node_inplace[0].is_boolean());
            REQUIRE(array_node_inplace[0].get_value<bool>() == true);
            REQUIRE(array_node_inplace[1].is_boolean());
            REQUIRE(array_node_inplace[1].get_value<bool>() == false);

            auto array_bool = node.get_value<std::array<bool, 2>>();
            REQUIRE(array_bool[0] == true);
            REQUIRE(array_bool[1] == false);

            std::array<bool, 2> array_bool_inplace {{}};
            node.get_value_inplace(array_bool_inplace);
            REQUIRE(array_bool_inplace[0] == true);
            REQUIRE(array_bool_inplace[1] == false);
        }

        SECTION("sequence value (std::valarray)") {
            auto valarray_node = node.get_value<std::valarray<fkyaml::node>>();
            REQUIRE(valarray_node[0].is_boolean());
            REQUIRE(valarray_node[0].get_value<bool>() == true);
            REQUIRE(valarray_node[1].is_boolean());
            REQUIRE(valarray_node[1].get_value<bool>() == false);

            std::valarray<fkyaml::node> valarray_node_inplace {};
            node.get_value_inplace(valarray_node_inplace);
            REQUIRE(valarray_node_inplace[0].is_boolean());
            REQUIRE(valarray_node_inplace[0].get_value<bool>() == true);
            REQUIRE(valarray_node_inplace[1].is_boolean());
            REQUIRE(valarray_node_inplace[1].get_value<bool>() == false);

            auto valarray_bool = node.get_value<std::valarray<bool>>();
            REQUIRE(valarray_bool[0] == true);
            REQUIRE(valarray_bool[1] == false);

            std::valarray<bool> valarray_bool_inplace {};
            node.get_value_inplace(valarray_bool_inplace);
            REQUIRE(valarray_bool_inplace[0] == true);
            REQUIRE(valarray_bool_inplace[1] == false);
        }

        SECTION("sequence value (std::forward_list)") {
            auto forward_list_node = node.get_value<std::forward_list<fkyaml::node>>();
            REQUIRE(forward_list_node.begin()->is_boolean());
            REQUIRE(forward_list_node.begin()->get_value<bool>() == true);
            REQUIRE((std::next(forward_list_node.begin()))->is_boolean());
            REQUIRE((std::next(forward_list_node.begin()))->get_value<bool>() == false);

            std::forward_list<fkyaml::node> forward_list_node_inplace {};
            node.get_value_inplace(forward_list_node_inplace);
            REQUIRE(forward_list_node_inplace.begin()->is_boolean());
            REQUIRE(forward_list_node_inplace.begin()->get_value<bool>() == true);
            REQUIRE((std::next(forward_list_node_inplace.begin()))->is_boolean());
            REQUIRE((std::next(forward_list_node_inplace.begin()))->get_value<bool>() == false);

            auto forward_list_bool = node.get_value<std::forward_list<bool>>();
            REQUIRE(*forward_list_bool.begin() == true);
            REQUIRE(*std::next(forward_list_bool.begin()) == false);

            std::forward_list<bool> forward_list_bool_inplace {};
            node.get_value_inplace(forward_list_bool_inplace);
            REQUIRE(*forward_list_bool_inplace.begin() == true);
            REQUIRE(*std::next(forward_list_bool_inplace.begin()) == false);
        }

        SECTION("sequence value (std::deque)") {
            auto deque_node = node.get_value<std::deque<fkyaml::node>>();
            REQUIRE(deque_node.size() == 2);
            REQUIRE(deque_node[0].is_boolean());
            REQUIRE(deque_node[0].get_value<bool>() == true);
            REQUIRE(deque_node[1].is_boolean());
            REQUIRE(deque_node[1].get_value<bool>() == false);

            std::deque<fkyaml::node> deque_node_inplace {};
            node.get_value_inplace(deque_node_inplace);
            REQUIRE(deque_node_inplace.size() == 2);
            REQUIRE(deque_node_inplace[0].is_boolean());
            REQUIRE(deque_node_inplace[0].get_value<bool>() == true);
            REQUIRE(deque_node_inplace[1].is_boolean());
            REQUIRE(deque_node_inplace[1].get_value<bool>() == false);

            auto deque_bool = node.get_value<std::deque<bool>>();
            REQUIRE(deque_bool.size() == 2);
            REQUIRE(deque_bool[0] == true);
            REQUIRE(deque_bool[1] == false);

            std::deque<bool> deque_bool_inplace {};
            node.get_value_inplace(deque_bool_inplace);
            REQUIRE(deque_bool_inplace.size() == 2);
            REQUIRE(deque_bool_inplace[0] == true);
            REQUIRE(deque_bool_inplace[1] == false);
        }

        SECTION("sequence value (std::list)") {
            auto list_node = node.get_value<std::list<fkyaml::node>>();
            REQUIRE(list_node.size() == 2);
            auto list_node_itr = list_node.begin();
            REQUIRE(list_node_itr->is_boolean());
            REQUIRE(list_node_itr->get_value<bool>() == true);
            list_node_itr++;
            REQUIRE(list_node_itr->is_boolean());
            REQUIRE(list_node_itr->get_value<bool>() == false);

            std::list<fkyaml::node> list_node_inplace {};
            node.get_value_inplace(list_node_inplace);
            REQUIRE(list_node_inplace.size() == 2);
            auto list_node_inplace_itr = list_node_inplace.begin();
            REQUIRE(list_node_inplace_itr->is_boolean());
            REQUIRE(list_node_inplace_itr->get_value<bool>() == true);
            list_node_inplace_itr++;
            REQUIRE(list_node_inplace_itr->is_boolean());
            REQUIRE(list_node_inplace_itr->get_value<bool>() == false);

            auto list_bool = node.get_value<std::list<bool>>();
            REQUIRE(list_bool.size() == 2);
            REQUIRE(*list_bool.begin() == true);
            REQUIRE(*(std::next(list_bool.begin())) == false);

            std::list<bool> list_bool_inplace {};
            node.get_value_inplace(list_bool_inplace);
            REQUIRE(list_bool_inplace.size() == 2);
            REQUIRE(*list_bool_inplace.begin() == true);
            REQUIRE(*(std::next(list_bool_inplace.begin())) == false);
        }

        SECTION("sequence value (std::set)") {
            auto set_node = node.get_value<std::set<fkyaml::node>>();
            REQUIRE(set_node.size() == 2);
            REQUIRE(set_node.find(fkyaml::node(true)) != set_node.end());
            REQUIRE(set_node.find(fkyaml::node(false)) != set_node.end());

            std::set<fkyaml::node> set_node_inplace {};
            node.get_value_inplace(set_node_inplace);
            REQUIRE(set_node_inplace.size() == 2);
            REQUIRE(set_node_inplace.find(fkyaml::node(true)) != set_node_inplace.end());
            REQUIRE(set_node_inplace.find(fkyaml::node(false)) != set_node_inplace.end());

            auto set_bool = node.get_value<std::set<bool>>();
            REQUIRE(set_bool.size() == 2);
            REQUIRE(set_bool.find(true) != set_bool.end());
            REQUIRE(set_bool.find(false) != set_bool.end());

            std::set<bool> set_bool_inplace {};
            node.get_value_inplace(set_bool_inplace);
            REQUIRE(set_bool_inplace.size() == 2);
            REQUIRE(set_bool_inplace.find(true) != set_bool_inplace.end());
            REQUIRE(set_bool_inplace.find(false) != set_bool_inplace.end());
        }

        SECTION("sequence value (std::multiset)") {
            auto mset_node = node.get_value<std::multiset<fkyaml::node>>();
            REQUIRE(mset_node.size() == 2);
            REQUIRE(mset_node.find(fkyaml::node(true)) != mset_node.end());
            REQUIRE(mset_node.find(fkyaml::node(false)) != mset_node.end());

            std::multiset<fkyaml::node> mset_node_inplace {};
            node.get_value_inplace(mset_node_inplace);
            REQUIRE(mset_node_inplace.size() == 2);
            REQUIRE(mset_node_inplace.find(fkyaml::node(true)) != mset_node_inplace.end());
            REQUIRE(mset_node_inplace.find(fkyaml::node(false)) != mset_node_inplace.end());

            auto mset_bool = node.get_value<std::multiset<fkyaml::node>>();
            REQUIRE(mset_bool.size() == 2);
            REQUIRE(mset_bool.find(true) != mset_bool.end());
            REQUIRE(mset_bool.find(false) != mset_bool.end());

            std::multiset<fkyaml::node> mset_bool_inplace {};
            node.get_value_inplace(mset_bool_inplace);
            REQUIRE(mset_bool_inplace.size() == 2);
            REQUIRE(mset_bool_inplace.find(true) != mset_bool_inplace.end());
            REQUIRE(mset_bool_inplace.find(false) != mset_bool_inplace.end());
        }

        SECTION("sequence value (std::unordered_set)") {
            auto uset_node = node.get_value<std::unordered_set<fkyaml::node>>();
            REQUIRE(uset_node.size() == 2);
            REQUIRE(uset_node.find(fkyaml::node(true)) != uset_node.end());
            REQUIRE(uset_node.find(fkyaml::node(false)) != uset_node.end());

            std::unordered_set<fkyaml::node> uset_node_inplace {};
            node.get_value_inplace(uset_node_inplace);
            REQUIRE(uset_node_inplace.size() == 2);
            REQUIRE(uset_node_inplace.find(fkyaml::node(true)) != uset_node_inplace.end());
            REQUIRE(uset_node_inplace.find(fkyaml::node(false)) != uset_node_inplace.end());

            auto uset_bool = node.get_value<std::unordered_set<bool>>();
            REQUIRE(uset_bool.size() == 2);
            REQUIRE(uset_bool.find(true) != uset_bool.end());
            REQUIRE(uset_bool.find(false) != uset_bool.end());

            std::unordered_set<bool> uset_bool_inplace {};
            node.get_value_inplace(uset_bool_inplace);
            REQUIRE(uset_bool_inplace.size() == 2);
            REQUIRE(uset_bool_inplace.find(true) != uset_bool_inplace.end());
            REQUIRE(uset_bool_inplace.find(false) != uset_bool_inplace.end());
        }

        SECTION("sequence value (std::unordered_multiset)") {
            auto umset_node = node.get_value<std::unordered_multiset<fkyaml::node>>();
            REQUIRE(umset_node.size() == 2);
            REQUIRE(umset_node.find(fkyaml::node(true)) != umset_node.end());
            REQUIRE(umset_node.find(fkyaml::node(false)) != umset_node.end());

            std::unordered_multiset<fkyaml::node> umset_node_inplace {};
            node.get_value_inplace(umset_node_inplace);
            REQUIRE(umset_node_inplace.size() == 2);
            REQUIRE(umset_node_inplace.find(fkyaml::node(true)) != umset_node_inplace.end());
            REQUIRE(umset_node_inplace.find(fkyaml::node(false)) != umset_node_inplace.end());

            auto umset_bool = node.get_value<std::unordered_multiset<bool>>();
            REQUIRE(umset_bool.size() == 2);
            REQUIRE(umset_bool.find(true) != umset_bool.end());
            REQUIRE(umset_bool.find(false) != umset_bool.end());

            std::unordered_multiset<bool> umset_bool_inplace {};
            node.get_value_inplace(umset_bool_inplace);
            REQUIRE(umset_bool_inplace.size() == 2);
            REQUIRE(umset_bool_inplace.find(true) != umset_bool_inplace.end());
            REQUIRE(umset_bool_inplace.find(false) != umset_bool_inplace.end());
        }

        SECTION("sequence value (std::stack)") {
            auto stack_node = node.get_value<std::stack<fkyaml::node>>();
            REQUIRE(stack_node.size() == 2);
            REQUIRE(stack_node.top().is_boolean());
            REQUIRE(stack_node.top().get_value<bool>() == false);
            stack_node.pop();
            REQUIRE(stack_node.top().is_boolean());
            REQUIRE(stack_node.top().get_value<bool>() == true);

            std::stack<fkyaml::node> stack_node_inplace {};
            node.get_value_inplace(stack_node_inplace);
            REQUIRE(stack_node_inplace.size() == 2);
            REQUIRE(stack_node_inplace.top().is_boolean());
            REQUIRE(stack_node_inplace.top().get_value<bool>() == false);
            stack_node_inplace.pop();
            REQUIRE(stack_node_inplace.top().is_boolean());
            REQUIRE(stack_node_inplace.top().get_value<bool>() == true);

            auto stack_bool = node.get_value<std::stack<bool>>();
            REQUIRE(stack_bool.size() == 2);
            REQUIRE(stack_bool.top() == false);
            stack_bool.pop();
            REQUIRE(stack_bool.top() == true);

            std::stack<bool> stack_bool_inplace {};
            stack_bool_inplace.push(true);
            stack_bool_inplace.push(false);
            node.get_value_inplace(stack_bool_inplace);
            REQUIRE(stack_bool_inplace.size() == 2);
            REQUIRE(stack_bool_inplace.top() == false);
            stack_bool_inplace.pop();
            REQUIRE(stack_bool_inplace.top() == true);
        }

        SECTION("sequence value (std::queue)") {
            auto queue_node = node.get_value<std::queue<fkyaml::node>>();
            REQUIRE(queue_node.size() == 2);
            REQUIRE(queue_node.front().is_boolean());
            REQUIRE(queue_node.front().get_value<bool>() == true);
            queue_node.pop();
            REQUIRE(queue_node.front().is_boolean());
            REQUIRE(queue_node.front().get_value<bool>() == false);

            std::queue<fkyaml::node> queue_node_inplace {};
            node.get_value_inplace(queue_node_inplace);
            REQUIRE(queue_node_inplace.size() == 2);
            REQUIRE(queue_node_inplace.front().is_boolean());
            REQUIRE(queue_node_inplace.front().get_value<bool>() == true);
            queue_node_inplace.pop();
            REQUIRE(queue_node_inplace.front().is_boolean());
            REQUIRE(queue_node_inplace.front().get_value<bool>() == false);

            auto queue_bool = node.get_value<std::queue<bool>>();
            REQUIRE(queue_bool.size() == 2);
            REQUIRE(queue_bool.front() == true);
            queue_bool.pop();
            REQUIRE(queue_bool.front() == false);

            std::queue<bool> queue_bool_inplace {};
            node.get_value_inplace(queue_bool_inplace);
            REQUIRE(queue_bool_inplace.size() == 2);
            REQUIRE(queue_bool_inplace.front() == true);
            queue_bool_inplace.pop();
            REQUIRE(queue_bool_inplace.front() == false);
        }

        SECTION("sequence value (std::priority_queue)") {
            auto pqueue_node = node.get_value<std::priority_queue<fkyaml::node>>();
            REQUIRE(pqueue_node.size() == 2);
            REQUIRE(pqueue_node.top().is_boolean());
            REQUIRE(pqueue_node.top().get_value<bool>() == true);
            pqueue_node.pop();
            REQUIRE(pqueue_node.top().is_boolean());
            REQUIRE(pqueue_node.top().get_value<bool>() == false);

            std::priority_queue<fkyaml::node> pqueue_node_inplace {};
            node.get_value_inplace(pqueue_node_inplace);
            REQUIRE(pqueue_node_inplace.size() == 2);
            REQUIRE(pqueue_node_inplace.top().is_boolean());
            REQUIRE(pqueue_node_inplace.top().get_value<bool>() == true);
            pqueue_node_inplace.pop();
            REQUIRE(pqueue_node_inplace.top().is_boolean());
            REQUIRE(pqueue_node_inplace.top().get_value<bool>() == false);

            auto pqueue_bool = node.get_value<std::priority_queue<bool>>();
            REQUIRE(pqueue_bool.size() == 2);
            REQUIRE(pqueue_bool.top() == true);
            pqueue_bool.pop();
            REQUIRE(pqueue_bool.top() == false);

            std::priority_queue<bool> pqueue_bool_inplace {};
            node.get_value_inplace(pqueue_bool_inplace);
            REQUIRE(pqueue_bool_inplace.size() == 2);
            REQUIRE(pqueue_bool_inplace.top() == true);
            pqueue_bool_inplace.pop();
            REQUIRE(pqueue_bool_inplace.top() == false);
        }

        SECTION("non-sequence value") {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::mapping_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::boolean_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::integer_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::float_number_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::string_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<string_wrap>(), fkyaml::type_error);
        }
    }

    SECTION("mapping") {
        fkyaml::node node {{"test", 123}, {"foo", -456}};

        SECTION("mapping value (std::map)") {
            auto map_node = node.get_value<std::map<fkyaml::node, fkyaml::node>>();
            REQUIRE(map_node.size() == 2);
            REQUIRE(map_node.find("test") != map_node.end());
            REQUIRE(map_node.at("test").is_integer());
            REQUIRE(map_node.at("test").get_value<int>() == 123);
            REQUIRE(map_node.find("foo") != map_node.end());
            REQUIRE(map_node.at("foo").is_integer());
            REQUIRE(map_node.at("foo").get_value<int>() == -456);

            std::map<fkyaml::node, fkyaml::node> map_node_inplace {};
            node.get_value_inplace(map_node_inplace);
            REQUIRE(map_node_inplace.size() == 2);
            REQUIRE(map_node_inplace.find("test") != map_node_inplace.end());
            REQUIRE(map_node_inplace.at("test").is_integer());
            REQUIRE(map_node_inplace.at("test").get_value<int>() == 123);
            REQUIRE(map_node_inplace.find("foo") != map_node_inplace.end());
            REQUIRE(map_node_inplace.at("foo").is_integer());
            REQUIRE(map_node_inplace.at("foo").get_value<int>() == -456);

            auto map_compat = node.get_value<std::map<std::string, int>>();
            REQUIRE(map_compat.size() == 2);
            REQUIRE(map_compat.find("test") != map_compat.end());
            REQUIRE(map_compat.at("test") == 123);
            REQUIRE(map_compat.find("foo") != map_compat.end());
            REQUIRE(map_compat.at("foo") == -456);

            std::map<std::string, int> map_compat_inplace {};
            node.get_value_inplace(map_compat_inplace);
            REQUIRE(map_compat_inplace.size() == 2);
            REQUIRE(map_compat_inplace.find("test") != map_compat_inplace.end());
            REQUIRE(map_compat_inplace.at("test") == 123);
            REQUIRE(map_compat_inplace.find("foo") != map_compat_inplace.end());
            REQUIRE(map_compat_inplace.at("foo") == -456);
        }

        SECTION("mapping value (std::multimap)") {
            auto mmap_node = node.get_value<std::multimap<fkyaml::node, fkyaml::node>>();
            REQUIRE(mmap_node.size() == 2);
            REQUIRE(mmap_node.find("test") != mmap_node.end());
            auto mmap_node_test_range = mmap_node.equal_range("test");
            REQUIRE(std::distance(mmap_node_test_range.first, mmap_node_test_range.second) == 1);
            REQUIRE(mmap_node_test_range.first->second.is_integer());
            REQUIRE(mmap_node_test_range.first->second.get_value<int>() == 123);
            REQUIRE(mmap_node.find("foo") != mmap_node.end());
            auto mmap_node_foo_range = mmap_node.equal_range("foo");
            REQUIRE(std::distance(mmap_node_test_range.first, mmap_node_test_range.second) == 1);
            REQUIRE(mmap_node_foo_range.first->second.is_integer());
            REQUIRE(mmap_node_foo_range.first->second.get_value<int>() == -456);

            std::multimap<fkyaml::node, fkyaml::node> mmap_node_inplace {};
            node.get_value_inplace(mmap_node_inplace);
            REQUIRE(mmap_node_inplace.size() == 2);
            REQUIRE(mmap_node_inplace.find("test") != mmap_node_inplace.end());
            auto mmap_node_inplace_test_range = mmap_node_inplace.equal_range("test");
            REQUIRE(std::distance(mmap_node_inplace_test_range.first, mmap_node_inplace_test_range.second) == 1);
            REQUIRE(mmap_node_inplace_test_range.first->second.is_integer());
            REQUIRE(mmap_node_inplace_test_range.first->second.get_value<int>() == 123);
            REQUIRE(mmap_node_inplace.find("foo") != mmap_node_inplace.end());
            auto mmap_node_inplace_foo_range = mmap_node_inplace.equal_range("foo");
            REQUIRE(std::distance(mmap_node_inplace_test_range.first, mmap_node_inplace_test_range.second) == 1);
            REQUIRE(mmap_node_inplace_foo_range.first->second.is_integer());
            REQUIRE(mmap_node_inplace_foo_range.first->second.get_value<int>() == -456);

            auto mmap_compat = node.get_value<std::multimap<std::string, int>>();
            REQUIRE(mmap_compat.size() == 2);
            REQUIRE(mmap_compat.find("test") != mmap_compat.end());
            auto mmap_compat_test_range = mmap_compat.equal_range("test");
            REQUIRE(std::distance(mmap_compat_test_range.first, mmap_compat_test_range.second) == 1);
            REQUIRE(mmap_compat_test_range.first->second == 123);
            REQUIRE(mmap_compat.find("foo") != mmap_compat.end());
            auto mmap_compat_foo_range = mmap_compat.equal_range("foo");
            REQUIRE(std::distance(mmap_compat_test_range.first, mmap_compat_test_range.second) == 1);
            REQUIRE(mmap_compat_foo_range.first->second == -456);

            std::multimap<std::string, int> mmap_compat_inplace {};
            node.get_value_inplace(mmap_compat_inplace);
            REQUIRE(mmap_compat_inplace.size() == 2);
            REQUIRE(mmap_compat_inplace.find("test") != mmap_compat_inplace.end());
            auto mmap_compat_inplace_test_range = mmap_compat_inplace.equal_range("test");
            REQUIRE(std::distance(mmap_compat_inplace_test_range.first, mmap_compat_inplace_test_range.second) == 1);
            REQUIRE(mmap_compat_inplace_test_range.first->second == 123);
            REQUIRE(mmap_compat_inplace.find("foo") != mmap_compat_inplace.end());
            auto mmap_compat_inplace_foo_range = mmap_compat_inplace.equal_range("foo");
            REQUIRE(std::distance(mmap_compat_inplace_test_range.first, mmap_compat_inplace_test_range.second) == 1);
            REQUIRE(mmap_compat_inplace_foo_range.first->second == -456);
        }

        SECTION("mapping value (std::unordered_map)") {
            auto umap_node = node.get_value<std::unordered_map<fkyaml::node, fkyaml::node>>();
            REQUIRE(umap_node.size() == 2);
            REQUIRE(umap_node.find("test") != umap_node.end());
            REQUIRE(umap_node.at("test").is_integer());
            REQUIRE(umap_node.at("test").get_value<int>() == 123);
            REQUIRE(umap_node.find("foo") != umap_node.end());
            REQUIRE(umap_node.at("foo").is_integer());
            REQUIRE(umap_node.at("foo").get_value<int>() == -456);

            std::unordered_map<fkyaml::node, fkyaml::node> umap_node_inplace {};
            node.get_value_inplace(umap_node_inplace);
            REQUIRE(umap_node_inplace.size() == 2);
            REQUIRE(umap_node_inplace.find("test") != umap_node_inplace.end());
            REQUIRE(umap_node_inplace.at("test").is_integer());
            REQUIRE(umap_node_inplace.at("test").get_value<int>() == 123);
            REQUIRE(umap_node_inplace.find("foo") != umap_node_inplace.end());
            REQUIRE(umap_node_inplace.at("foo").is_integer());
            REQUIRE(umap_node_inplace.at("foo").get_value<int>() == -456);

            auto umap_compat = node.get_value<std::unordered_map<std::string, int>>();
            REQUIRE(umap_compat.size() == 2);
            REQUIRE(umap_compat.find("test") != umap_compat.end());
            REQUIRE(umap_compat.at("test") == 123);
            REQUIRE(umap_compat.find("foo") != umap_compat.end());
            REQUIRE(umap_compat.at("foo") == -456);

            std::unordered_map<std::string, int> umap_compat_inplace {};
            node.get_value_inplace(umap_compat_inplace);
            REQUIRE(umap_compat_inplace.size() == 2);
            REQUIRE(umap_compat_inplace.find("test") != umap_compat_inplace.end());
            REQUIRE(umap_compat_inplace.at("test") == 123);
            REQUIRE(umap_compat_inplace.find("foo") != umap_compat_inplace.end());
            REQUIRE(umap_compat_inplace.at("foo") == -456);

            fkyaml::node mapping {
                {{123, 456, 789}, nullptr},
                {{{true, false}}, nullptr},
                {nullptr, nullptr},
                {true, nullptr},
                {123, nullptr},
                {3.14, nullptr},
                {"foo", nullptr}};
            auto umap_various_keys = mapping.get_value<std::unordered_map<fkyaml::node, fkyaml::node>>();
            REQUIRE(umap_various_keys.size() == 7);
            REQUIRE(umap_various_keys.find({123, 456, 789}) != umap_various_keys.end());
            REQUIRE(umap_various_keys.find({{true, false}}) != umap_various_keys.end());
            REQUIRE(umap_various_keys.find(nullptr) != umap_various_keys.end());
            REQUIRE(umap_various_keys.find(true) != umap_various_keys.end());
            REQUIRE(umap_various_keys.find(123) != umap_various_keys.end());
            REQUIRE(umap_various_keys.find(3.14) != umap_various_keys.end());
            REQUIRE(umap_various_keys.find("foo") != umap_various_keys.end());
            for (auto itr : umap_various_keys) {
                REQUIRE(itr.second.is_null());
            }
        }

        SECTION("mapping value (std::unordered_multimap)") {
            auto ummap_node = node.get_value<std::unordered_multimap<fkyaml::node, fkyaml::node>>();
            REQUIRE(ummap_node.size() == 2);
            REQUIRE(ummap_node.find("test") != ummap_node.end());
            auto ummap_node_test_range = ummap_node.equal_range("test");
            REQUIRE(std::distance(ummap_node_test_range.first, ummap_node_test_range.second) == 1);
            REQUIRE(ummap_node_test_range.first->second.is_integer());
            REQUIRE(ummap_node_test_range.first->second.get_value<int>() == 123);
            REQUIRE(ummap_node.find("foo") != ummap_node.end());
            auto ummap_node_foo_range = ummap_node.equal_range("foo");
            REQUIRE(std::distance(ummap_node_test_range.first, ummap_node_test_range.second) == 1);
            REQUIRE(ummap_node_foo_range.first->second.is_integer());
            REQUIRE(ummap_node_foo_range.first->second.get_value<int>() == -456);

            std::unordered_multimap<fkyaml::node, fkyaml::node> ummap_node_inplace {};
            node.get_value_inplace(ummap_node_inplace);
            REQUIRE(ummap_node_inplace.size() == 2);
            REQUIRE(ummap_node_inplace.find("test") != ummap_node_inplace.end());
            auto ummap_node_inplace_test_range = ummap_node_inplace.equal_range("test");
            REQUIRE(std::distance(ummap_node_inplace_test_range.first, ummap_node_inplace_test_range.second) == 1);
            REQUIRE(ummap_node_inplace_test_range.first->second.is_integer());
            REQUIRE(ummap_node_inplace_test_range.first->second.get_value<int>() == 123);
            REQUIRE(ummap_node_inplace.find("foo") != ummap_node_inplace.end());
            auto ummap_node_inplace_foo_range = ummap_node_inplace.equal_range("foo");
            REQUIRE(std::distance(ummap_node_inplace_test_range.first, ummap_node_inplace_test_range.second) == 1);
            REQUIRE(ummap_node_inplace_foo_range.first->second.is_integer());
            REQUIRE(ummap_node_inplace_foo_range.first->second.get_value<int>() == -456);

            auto ummap_compat = node.get_value<std::unordered_multimap<std::string, int>>();
            REQUIRE(ummap_compat.size() == 2);
            REQUIRE(ummap_compat.find("test") != ummap_compat.end());
            auto ummap_compat_test_range = ummap_compat.equal_range("test");
            REQUIRE(std::distance(ummap_compat_test_range.first, ummap_compat_test_range.second) == 1);
            REQUIRE(ummap_compat_test_range.first->second == 123);
            REQUIRE(ummap_compat.find("foo") != ummap_compat.end());
            auto ummap_compat_foo_range = ummap_compat.equal_range("foo");
            REQUIRE(std::distance(ummap_compat_test_range.first, ummap_compat_test_range.second) == 1);
            REQUIRE(ummap_compat_foo_range.first->second == -456);

            std::unordered_multimap<std::string, int> ummap_compat_inplace {};
            node.get_value_inplace(ummap_compat_inplace);
            REQUIRE(ummap_compat_inplace.size() == 2);
            REQUIRE(ummap_compat_inplace.find("test") != ummap_compat_inplace.end());
            auto ummap_compat_inplace_test_range = ummap_compat_inplace.equal_range("test");
            REQUIRE(std::distance(ummap_compat_inplace_test_range.first, ummap_compat_inplace_test_range.second) == 1);
            REQUIRE(ummap_compat_inplace_test_range.first->second == 123);
            REQUIRE(ummap_compat_inplace.find("foo") != ummap_compat_inplace.end());
            auto ummap_compat_inplace_foo_range = ummap_compat_inplace.equal_range("foo");
            REQUIRE(std::distance(ummap_compat_inplace_test_range.first, ummap_compat_inplace_test_range.second) == 1);
            REQUIRE(ummap_compat_inplace_foo_range.first->second == -456);
        }

        SECTION("non-mapping values") {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::sequence_type>(), fkyaml::type_error);
            int dummy_array_1d[2] {};
            REQUIRE_THROWS_AS(node.get_value_inplace(dummy_array_1d), fkyaml::type_error);
            int dummy_array_2d[2][2] {};
            REQUIRE_THROWS_AS(node.get_value_inplace(dummy_array_2d), fkyaml::type_error);
            int dummy_array_3d[2][2][2] {};
            REQUIRE_THROWS_AS(node.get_value_inplace(dummy_array_3d), fkyaml::type_error);
            using array_t = std::array<int, 2>;
            REQUIRE_THROWS_AS(node.get_value<array_t>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<std::valarray<double>>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<std::forward_list<int>>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<std::stack<int>>(), fkyaml::type_error);
            using pair_t = std::pair<bool, int>;
            REQUIRE_THROWS_AS(node.get_value<pair_t>(), fkyaml::type_error);
            using tuple_t = std::tuple<bool, int, float>;
            REQUIRE_THROWS_AS(node.get_value<tuple_t>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::boolean_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::integer_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::float_number_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::string_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<string_wrap>(), fkyaml::type_error);
        }
    }

    SECTION("null node value") {
        fkyaml::node node(nullptr);

        SECTION("null type") {
            auto null = node.get_value<std::nullptr_t>();
            REQUIRE(null == nullptr);

            std::nullptr_t null_inplace;
            node.get_value_inplace(null_inplace);
            REQUIRE(null_inplace == nullptr);
        }

        SECTION("non-null compatible types") {
            REQUIRE(node.get_value<bool>() == false);
            bool bool_inplace = true;
            node.get_value_inplace(bool_inplace);
            REQUIRE(bool_inplace == false);

            REQUIRE(node.get_value<uint8_t>() == 0);
            uint8_t ui8_inplace = 1;
            node.get_value_inplace(ui8_inplace);
            REQUIRE(ui8_inplace == 0);

            REQUIRE(node.get_value<uint16_t>() == 0);
            uint16_t ui16_inplace = 1;
            node.get_value_inplace(ui16_inplace);
            REQUIRE(ui16_inplace == 0);

            REQUIRE(node.get_value<uint32_t>() == 0);
            uint32_t ui32_inplace = 1;
            node.get_value_inplace(ui32_inplace);
            REQUIRE(ui32_inplace == 0);

            REQUIRE(node.get_value<uint64_t>() == 0);
            uint64_t ui64_inplace = 1;
            node.get_value_inplace(ui64_inplace);
            REQUIRE(ui64_inplace == 0);

            REQUIRE(node.get_value<int8_t>() == 0);
            int8_t i8_inplace = 1;
            node.get_value_inplace(i8_inplace);
            REQUIRE(i8_inplace == 0);

            REQUIRE(node.get_value<int16_t>() == 0);
            int16_t i16_inplace = 1;
            node.get_value_inplace(i16_inplace);
            REQUIRE(i16_inplace == 0);

            REQUIRE(node.get_value<int32_t>() == 0);
            int32_t i32_inplace = 1;
            node.get_value_inplace(i32_inplace);
            REQUIRE(i32_inplace == 0);

            REQUIRE(node.get_value<int64_t>() == 0);
            int64_t i64_inplace = 1;
            node.get_value_inplace(i64_inplace);
            REQUIRE(i64_inplace == 0);

            REQUIRE(node.get_value<float>() == 0.f);
            float float_inplace = 1;
            node.get_value_inplace(float_inplace);
            REQUIRE(float_inplace == 0.f);

            REQUIRE(node.get_value<double>() == 0.);
            double double_inplace = 1;
            node.get_value_inplace(double_inplace);
            REQUIRE(double_inplace == 0.);

            REQUIRE(node.get_value<long double>() == 0.l);
            long double long_double_inplace = 1;
            node.get_value_inplace(long_double_inplace);
            REQUIRE(long_double_inplace == 0.l);
        }

        SECTION("non-null incompatible types") {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::sequence_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::mapping_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::string_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<string_wrap>(), fkyaml::type_error);
        }
    }

    SECTION("boolean node value") {
        fkyaml::node true_node(true);
        fkyaml::node false_node(false);

        SECTION("boolean type") {
            REQUIRE(true_node.get_value<bool>() == true);
            REQUIRE(false_node.get_value<bool>() == false);

            bool true_inplace = false;
            bool false_inplace = true;
            true_node.get_value_inplace(true_inplace);
            false_node.get_value_inplace(false_inplace);
            REQUIRE(true_inplace == true);
            REQUIRE(false_inplace == false);
        }

        SECTION("non-boolean compatible types (true)") {
            REQUIRE(true_node.get_value<uint8_t>() == 1);
            uint8_t ui8_inplace = 0;
            true_node.get_value_inplace(ui8_inplace);
            REQUIRE(ui8_inplace == 1);

            REQUIRE(true_node.get_value<uint16_t>() == 1);
            uint16_t ui16_inplace = 0;
            true_node.get_value_inplace(ui16_inplace);
            REQUIRE(ui16_inplace == 1);

            REQUIRE(true_node.get_value<uint32_t>() == 1);
            uint32_t ui32_inplace = 0;
            true_node.get_value_inplace(ui32_inplace);
            REQUIRE(ui32_inplace == 1);

            REQUIRE(true_node.get_value<uint64_t>() == 1);
            uint64_t ui64_inplace = 0;
            true_node.get_value_inplace(ui64_inplace);
            REQUIRE(ui64_inplace == 1);

            REQUIRE(true_node.get_value<int8_t>() == 1);
            int8_t i8_inplace = 0;
            true_node.get_value_inplace(i8_inplace);
            REQUIRE(i8_inplace == 1);

            REQUIRE(true_node.get_value<int16_t>() == 1);
            int16_t i16_inplace = 0;
            true_node.get_value_inplace(i16_inplace);
            REQUIRE(i16_inplace == 1);

            REQUIRE(true_node.get_value<int32_t>() == 1);
            int32_t i32_inplace = 0;
            true_node.get_value_inplace(i32_inplace);
            REQUIRE(i32_inplace == 1);

            REQUIRE(true_node.get_value<int64_t>() == 1);
            int64_t i64_inplace = 0;
            true_node.get_value_inplace(i64_inplace);
            REQUIRE(i64_inplace == 1);

            REQUIRE(true_node.get_value<float>() == 1.f);
            float float_inplace = 0;
            true_node.get_value_inplace(float_inplace);
            REQUIRE(float_inplace == 1.f);

            REQUIRE(true_node.get_value<double>() == 1.);
            double double_inplace = 0;
            true_node.get_value_inplace(double_inplace);
            REQUIRE(double_inplace == 1.);

            REQUIRE(true_node.get_value<long double>() == 1.l);
            long double long_double_inplace = 0;
            true_node.get_value_inplace(long_double_inplace);
            REQUIRE(long_double_inplace == 1.l);
        }

        SECTION("non-boolean compatible types (false)") {
            REQUIRE(false_node.get_value<uint8_t>() == 0);
            uint8_t ui8_inplace = 1;
            false_node.get_value_inplace(ui8_inplace);
            REQUIRE(ui8_inplace == 0);

            REQUIRE(false_node.get_value<uint16_t>() == 0);
            uint16_t ui16_inplace = 1;
            false_node.get_value_inplace(ui16_inplace);
            REQUIRE(ui16_inplace == 0);

            REQUIRE(false_node.get_value<uint32_t>() == 0);
            uint32_t ui32_inplace = 1;
            false_node.get_value_inplace(ui32_inplace);
            REQUIRE(ui32_inplace == 0);

            REQUIRE(false_node.get_value<uint64_t>() == 0);
            uint64_t ui64_inplace = 1;
            false_node.get_value_inplace(ui64_inplace);
            REQUIRE(ui64_inplace == 0);

            REQUIRE(false_node.get_value<int8_t>() == 0);
            int8_t i8_inplace = 1;
            false_node.get_value_inplace(i8_inplace);
            REQUIRE(i8_inplace == 0);

            REQUIRE(false_node.get_value<int16_t>() == 0);
            int16_t i16_inplace = 1;
            false_node.get_value_inplace(i16_inplace);
            REQUIRE(i16_inplace == 0);

            REQUIRE(false_node.get_value<int32_t>() == 0);
            int32_t i32_inplace = 1;
            false_node.get_value_inplace(i32_inplace);
            REQUIRE(i32_inplace == 0);

            REQUIRE(false_node.get_value<int64_t>() == 0);
            int64_t i64_inplace = 1;
            false_node.get_value_inplace(i64_inplace);
            REQUIRE(i64_inplace == 0);

            REQUIRE(false_node.get_value<float>() == 0.f);
            float float_inplace = 1;
            false_node.get_value_inplace(float_inplace);
            REQUIRE(float_inplace == 0.f);

            REQUIRE(false_node.get_value<double>() == 0.);
            double double_inplace = 1;
            false_node.get_value_inplace(double_inplace);
            REQUIRE(double_inplace == 0.);

            REQUIRE(false_node.get_value<long double>() == 0.l);
            long double long_double_inplace = 1;
            false_node.get_value_inplace(long_double_inplace);
            REQUIRE(long_double_inplace == 0.l);
        }

        SECTION("non-boolean incompatible types") {
            REQUIRE_THROWS_AS(true_node.get_value<fkyaml::node::sequence_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(true_node.get_value<fkyaml::node::mapping_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(true_node.get_value<std::nullptr_t>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(true_node.get_value<fkyaml::node::string_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(true_node.get_value<string_wrap>(), fkyaml::type_error);
        }
    }

    SECTION("integer node value") {
        fkyaml::node node(123);

        SECTION("integer types") {
            REQUIRE(node.get_value<int8_t>() == 123);
            int8_t i8_inplace = 0;
            node.get_value_inplace(i8_inplace);
            REQUIRE(i8_inplace == 123);

            REQUIRE(node.get_value<int16_t>() == 123);
            int16_t i16_inplace = 0;
            node.get_value_inplace(i16_inplace);
            REQUIRE(i16_inplace == 123);

            REQUIRE(node.get_value<int32_t>() == 123);
            int32_t i32_inplace = 0;
            node.get_value_inplace(i32_inplace);
            REQUIRE(i32_inplace == 123);

            REQUIRE(node.get_value<int64_t>() == 123);
            int64_t i64_inplace = 0;
            node.get_value_inplace(i64_inplace);
            REQUIRE(i64_inplace == 123);

            REQUIRE(node.get_value<uint8_t>() == 123);
            uint8_t ui8_inplace = 0;
            node.get_value_inplace(ui8_inplace);
            REQUIRE(ui8_inplace == 123);

            REQUIRE(node.get_value<uint16_t>() == 123);
            uint16_t ui16_inplace = 0;
            node.get_value_inplace(ui16_inplace);
            REQUIRE(ui16_inplace == 123);

            REQUIRE(node.get_value<uint32_t>() == 123);
            uint32_t ui32_inplace = 0;
            node.get_value_inplace(ui32_inplace);
            REQUIRE(ui32_inplace == 123);

            REQUIRE(node.get_value<uint64_t>() == 123);
            uint64_t ui64_inplace = 0;
            node.get_value_inplace(ui64_inplace);
            REQUIRE(ui64_inplace == 123);
        }

        SECTION("non-integer compatible types (positive)") {
            REQUIRE(node.get_value<bool>() == true);
            bool bool_inplace = false;
            node.get_value_inplace(bool_inplace);
            REQUIRE(bool_inplace == true);

            REQUIRE(node.get_value<float>() == 123.f);
            float float_inplace = 0.f;
            node.get_value_inplace(float_inplace);
            REQUIRE(float_inplace == 123.f);

            REQUIRE(node.get_value<double>() == 123.);
            double double_inplace = 0.;
            node.get_value_inplace(double_inplace);
            REQUIRE(double_inplace == 123.);

            REQUIRE(node.get_value<long double>() == 123.l);
            long double long_double_inplace = 0.l;
            node.get_value_inplace(long_double_inplace);
            REQUIRE(long_double_inplace == 123.l);
        }

        SECTION("non-integer compatible types (negative)") {
            node = -123;
            REQUIRE(node.get_value<bool>() == true);
            bool bool_inplace = false;
            node.get_value_inplace(bool_inplace);
            REQUIRE(bool_inplace == true);

            REQUIRE(node.get_value<float>() == -123.f);
            float float_inplace = 0.f;
            node.get_value_inplace(float_inplace);
            REQUIRE(float_inplace == -123.f);

            REQUIRE(node.get_value<double>() == -123.);
            double double_inplace = 0.;
            node.get_value_inplace(double_inplace);
            REQUIRE(double_inplace == -123.);

            REQUIRE(node.get_value<long double>() == -123.l);
            long double long_double_inplace = 0.l;
            node.get_value_inplace(long_double_inplace);
            REQUIRE(long_double_inplace == -123.l);
        }

        SECTION("non-integer compatible types (zero)") {
            node = 0;
            REQUIRE(node.get_value<bool>() == false);
            bool bool_inplace = true;
            node.get_value_inplace(bool_inplace);
            REQUIRE(bool_inplace == false);

            REQUIRE(node.get_value<float>() == 0.f);
            float float_inplace = 1.f;
            node.get_value_inplace(float_inplace);
            REQUIRE(float_inplace == 0.f);

            REQUIRE(node.get_value<double>() == 0.);
            double double_inplace = 1.;
            node.get_value_inplace(double_inplace);
            REQUIRE(double_inplace == 0.);

            REQUIRE(node.get_value<long double>() == 0.l);
            long double long_double_inplace = 1.l;
            node.get_value_inplace(long_double_inplace);
            REQUIRE(long_double_inplace == 0.l);
        }

        SECTION("non-integer incompatible types") {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::sequence_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::mapping_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::string_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<string_wrap>(), fkyaml::type_error);
        }

        SECTION("underflowable integer type") {
            fkyaml::node negative_int_node(std::numeric_limits<fkyaml::node::integer_type>::min());
            REQUIRE_THROWS_AS(negative_int_node.get_value<int8_t>(), fkyaml::exception);
            REQUIRE_THROWS_AS(negative_int_node.get_value<uint64_t>(), fkyaml::exception);
        }

        SECTION("overflowable integer type") {
            fkyaml::node large_int_node(std::numeric_limits<fkyaml::node::integer_type>::max());
            REQUIRE_THROWS_AS(large_int_node.get_value<int8_t>(), fkyaml::exception);
        }
    }

    SECTION("float number node value") {
        fkyaml::node node(3.14);

        SECTION("positive float values") {
            REQUIRE(std::abs(node.get_value<float>() - 3.14) < std::numeric_limits<float>::epsilon());
            float float_inplace = 0.f;
            node.get_value_inplace(float_inplace);
            REQUIRE(std::abs(float_inplace - 3.14) < std::numeric_limits<float>::epsilon());

            REQUIRE(std::abs(node.get_value<double>() - 3.14) < std::numeric_limits<double>::epsilon());
            double double_inplace = 0.;
            node.get_value_inplace(double_inplace);
            REQUIRE(std::abs(double_inplace - 3.14) < std::numeric_limits<double>::epsilon());

            REQUIRE(std::abs(node.get_value<long double>() - 3.14) < std::numeric_limits<long double>::epsilon());
            long double long_double_inplace = 0.;
            node.get_value_inplace(long_double_inplace);
            REQUIRE(std::abs(long_double_inplace - 3.14) < std::numeric_limits<long double>::epsilon());
        }

        SECTION("zero float values") {
            node = 0.0;
            REQUIRE(std::abs(node.get_value<float>() - 0.0) < std::numeric_limits<float>::epsilon());
            float float_inplace = 1.f;
            node.get_value_inplace(float_inplace);
            REQUIRE(std::abs(float_inplace - 0.0) < std::numeric_limits<float>::epsilon());

            REQUIRE(std::abs(node.get_value<double>() - 0.0) < std::numeric_limits<double>::epsilon());
            double double_inplace = 1.;
            node.get_value_inplace(double_inplace);
            REQUIRE(std::abs(double_inplace - 0.0) < std::numeric_limits<double>::epsilon());

            REQUIRE(std::abs(node.get_value<long double>() - 0.0) < std::numeric_limits<long double>::epsilon());
            long double long_double_inplace = 1.l;
            node.get_value_inplace(long_double_inplace);
            REQUIRE(std::abs(long_double_inplace - 0.0) < std::numeric_limits<long double>::epsilon());
        }

        SECTION("negative float values") {
            node = -3.14;
            REQUIRE(std::abs(node.get_value<float>() - (-3.14)) < std::numeric_limits<float>::epsilon());
            float float_inplace = 0.f;
            node.get_value_inplace(float_inplace);
            REQUIRE(std::abs(float_inplace - (-3.14)) < std::numeric_limits<float>::epsilon());

            REQUIRE(std::abs(node.get_value<double>() - (-3.14)) < std::numeric_limits<double>::epsilon());
            double double_inplace = 0.;
            node.get_value_inplace(double_inplace);
            REQUIRE(std::abs(double_inplace - (-3.14)) < std::numeric_limits<double>::epsilon());

            REQUIRE(std::abs(node.get_value<long double>() - (-3.14)) < std::numeric_limits<long double>::epsilon());
            long double long_double_inplace = 0.;
            node.get_value_inplace(long_double_inplace);
            REQUIRE(std::abs(long_double_inplace - (-3.14)) < std::numeric_limits<long double>::epsilon());
        }

        SECTION("non-float compatible types (positive)") {
            REQUIRE(node.get_value<bool>() == true);
            bool bool_inplace = false;
            node.get_value_inplace(bool_inplace);
            REQUIRE(bool_inplace == true);

            REQUIRE(node.get_value<uint8_t>() == 3);
            uint8_t ui8_inplace = 0;
            node.get_value_inplace(ui8_inplace);
            REQUIRE(ui8_inplace == 3);

            REQUIRE(node.get_value<uint16_t>() == 3);
            uint16_t ui16_inplace = 0;
            node.get_value_inplace(ui16_inplace);
            REQUIRE(ui16_inplace == 3);

            REQUIRE(node.get_value<uint32_t>() == 3);
            uint32_t ui32_inplace = 0;
            node.get_value_inplace(ui32_inplace);
            REQUIRE(ui32_inplace == 3);

            REQUIRE(node.get_value<uint64_t>() == 3);
            uint64_t ui64_inplace = 0;
            node.get_value_inplace(ui64_inplace);
            REQUIRE(ui64_inplace == 3);

            REQUIRE(node.get_value<int8_t>() == 3);
            int8_t i8_inplace = 0;
            node.get_value_inplace(i8_inplace);
            REQUIRE(i8_inplace == 3);

            REQUIRE(node.get_value<int16_t>() == 3);
            int16_t i16_inplace = 0;
            node.get_value_inplace(i16_inplace);
            REQUIRE(i16_inplace == 3);

            REQUIRE(node.get_value<int32_t>() == 3);
            int32_t i32_inplace = 0;
            node.get_value_inplace(i32_inplace);
            REQUIRE(i32_inplace == 3);

            REQUIRE(node.get_value<int64_t>() == 3);
            int64_t i64_inplace = 0;
            node.get_value_inplace(i64_inplace);
            REQUIRE(i64_inplace == 3);
        }

        SECTION("non-float compatible types (negative)") {
            node = -3.14;
            REQUIRE(node.get_value<bool>() == true);
            bool bool_inplace = false;
            node.get_value_inplace(bool_inplace);
            REQUIRE(bool_inplace == true);

            REQUIRE(node.get_value<int8_t>() == -3);
            int8_t i8_inplace = 0;
            node.get_value_inplace(i8_inplace);
            REQUIRE(i8_inplace == -3);

            REQUIRE(node.get_value<int16_t>() == -3);
            int16_t i16_inplace = 0;
            node.get_value_inplace(i16_inplace);
            REQUIRE(i16_inplace == -3);

            REQUIRE(node.get_value<int32_t>() == -3);
            int32_t i32_inplace = 0;
            node.get_value_inplace(i32_inplace);
            REQUIRE(i32_inplace == -3);

            REQUIRE(node.get_value<int64_t>() == -3);
            int64_t i64_inplace = 0;
            node.get_value_inplace(i64_inplace);
            REQUIRE(i64_inplace == -3);
        }

        SECTION("non-float compatible types (zero)") {
            node = 0.0;
            REQUIRE(node.get_value<bool>() == false);
            bool bool_inplace = true;
            node.get_value_inplace(bool_inplace);
            REQUIRE(bool_inplace == false);

            REQUIRE(node.get_value<uint8_t>() == 0);
            uint8_t ui8_inplace = 1;
            node.get_value_inplace(ui8_inplace);
            REQUIRE(ui8_inplace == 0);

            REQUIRE(node.get_value<uint16_t>() == 0);
            uint16_t ui16_inplace = 1;
            node.get_value_inplace(ui16_inplace);
            REQUIRE(ui16_inplace == 0);

            REQUIRE(node.get_value<uint32_t>() == 0);
            uint32_t ui32_inplace = 1;
            node.get_value_inplace(ui32_inplace);
            REQUIRE(ui32_inplace == 0);

            REQUIRE(node.get_value<uint64_t>() == 0);
            uint64_t ui64_inplace = 1;
            node.get_value_inplace(ui64_inplace);
            REQUIRE(ui64_inplace == 0);

            REQUIRE(node.get_value<int8_t>() == 0);
            int8_t i8_inplace = 1;
            node.get_value_inplace(i8_inplace);
            REQUIRE(i8_inplace == 0);

            REQUIRE(node.get_value<int16_t>() == 0);
            int16_t i16_inplace = 1;
            node.get_value_inplace(i16_inplace);
            REQUIRE(i16_inplace == 0);

            REQUIRE(node.get_value<int32_t>() == 0);
            int32_t i32_inplace = 1;
            node.get_value_inplace(i32_inplace);
            REQUIRE(i32_inplace == 0);

            REQUIRE(node.get_value<int64_t>() == 0);
            int64_t i64_inplace = 1;
            node.get_value_inplace(i64_inplace);
            REQUIRE(i64_inplace == 0);
        }

        SECTION("non-float incompatible types") {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::sequence_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::mapping_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::string_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<string_wrap>(), fkyaml::type_error);
        }

        SECTION("underflowable float number type") {
            fkyaml::node negative_float_node(std::numeric_limits<fkyaml::node::float_number_type>::lowest());
            REQUIRE_THROWS_AS(negative_float_node.get_value<float>(), fkyaml::exception);
        }

        SECTION("overflowable float number type") {
            fkyaml::node large_float_node(std::numeric_limits<fkyaml::node::float_number_type>::max());
            REQUIRE_THROWS_AS(large_float_node.get_value<float>(), fkyaml::exception);
        }

        SECTION("invalid float-to-int conversion") {
            node = -3.14;
            REQUIRE_THROWS_AS(node.get_value<uint32_t>(), fkyaml::exception);
            REQUIRE_THROWS_AS(node.get_value<uint64_t>(), fkyaml::exception);

            node = 256.0;
            REQUIRE_THROWS_AS(node.get_value<int8_t>(), fkyaml::exception);
        }
    }

    SECTION("string node value") {
        fkyaml::node node("test");

        SECTION("string value (std::string)") {
            auto str = node.get_value<std::string>();
            REQUIRE(str.size() == 4);
            REQUIRE(str == "test");

            std::string str_inplace {};
            node.get_value_inplace(str_inplace);
            REQUIRE(str_inplace.size() == 4);
            REQUIRE(str_inplace == "test");
        }

        SECTION("string value (string_wrap)") {
            auto str_wrap = node.get_value<string_wrap>();
            REQUIRE(str_wrap.str.size() == 4);
            REQUIRE(str_wrap.str == "test");

            string_wrap str_wrap_inplace {};
            node.get_value_inplace(str_wrap_inplace);
            REQUIRE(str_wrap_inplace.str.size() == 4);
            REQUIRE(str_wrap_inplace.str == "test");
        }

        SECTION("string value (fkyaml::detail::str_view)") {
            auto str_view = node.get_value<fkyaml::detail::str_view>();
            REQUIRE(str_view.size() == 4);
            REQUIRE(str_view == "test");

            fkyaml::detail::str_view str_view_inplace {};
            node.get_value_inplace(str_view_inplace);
            REQUIRE(str_view_inplace.size() == 4);
            REQUIRE(str_view_inplace == "test");
        }

#ifdef FK_YAML_HAS_CXX_17
        SECTION("string value (std::string_view)") {
            auto str_view = node.get_value<std::string_view>();
            REQUIRE(str_view.size() == 4);
            REQUIRE(str_view == "test");

            std::string_view str_view_inplace {};
            node.get_value_inplace(str_view_inplace);
            REQUIRE(str_view_inplace.size() == 4);
            REQUIRE(str_view_inplace == "test");
        }
#endif

        SECTION("non-string values") {
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::sequence_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::mapping_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<std::nullptr_t>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::boolean_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::integer_type>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(node.get_value<fkyaml::node::float_number_type>(), fkyaml::type_error);
        }
    }

    SECTION("std::pair") {
        fkyaml::node n {123, "test"};

        auto pair_node = n.get_value<std::pair<fkyaml::node, fkyaml::node>>();
        REQUIRE(pair_node.first.is_integer());
        REQUIRE(pair_node.first.get_value<int>() == 123);
        REQUIRE(pair_node.second.is_string());
        REQUIRE(pair_node.second.get_value<std::string>() == "test");

        std::pair<fkyaml::node, fkyaml::node> pair_node_inplace {};
        n.get_value_inplace(pair_node_inplace);
        REQUIRE(pair_node_inplace.first.is_integer());
        REQUIRE(pair_node_inplace.first.get_value<int>() == 123);
        REQUIRE(pair_node_inplace.second.is_string());
        REQUIRE(pair_node_inplace.second.get_value<std::string>() == "test");

        auto pair_val = n.get_value<std::pair<int, std::string>>();
        REQUIRE(pair_val.first == 123);
        REQUIRE(pair_val.second == "test");

        std::pair<int, std::string> pair_val_inplace {};
        n.get_value_inplace(pair_val_inplace);
        REQUIRE(pair_val_inplace.first == 123);
        REQUIRE(pair_val_inplace.second == "test");
    }

    SECTION("std::tuple") {
        fkyaml::node n {123, "test", true};

        auto tuple_node = n.get_value<std::tuple<fkyaml::node, fkyaml::node, fkyaml::node>>();
        REQUIRE(std::get<0>(tuple_node).is_integer());
        REQUIRE(std::get<0>(tuple_node).get_value<int>() == 123);
        REQUIRE(std::get<1>(tuple_node).is_string());
        REQUIRE(std::get<1>(tuple_node).get_value<std::string>() == "test");
        REQUIRE(std::get<2>(tuple_node).is_boolean());
        REQUIRE(std::get<2>(tuple_node).get_value<bool>() == true);

        std::tuple<fkyaml::node, fkyaml::node, fkyaml::node> tuple_node_inplace {};
        n.get_value_inplace(tuple_node_inplace);
        REQUIRE(std::get<0>(tuple_node_inplace).is_integer());
        REQUIRE(std::get<0>(tuple_node_inplace).get_value<int>() == 123);
        REQUIRE(std::get<1>(tuple_node_inplace).is_string());
        REQUIRE(std::get<1>(tuple_node_inplace).get_value<std::string>() == "test");
        REQUIRE(std::get<2>(tuple_node_inplace).is_boolean());
        REQUIRE(std::get<2>(tuple_node_inplace).get_value<bool>() == true);

        auto tuple_val = n.get_value<std::tuple<int, std::string, bool>>();
        REQUIRE(std::get<0>(tuple_val) == 123);
        REQUIRE(std::get<1>(tuple_val) == "test");
        REQUIRE(std::get<2>(tuple_val) == true);

        std::tuple<int, std::string, bool> tuple_val_inplace {};
        n.get_value_inplace(tuple_val_inplace);
        REQUIRE(std::get<0>(tuple_val_inplace) == 123);
        REQUIRE(std::get<1>(tuple_val_inplace) == "test");
        REQUIRE(std::get<2>(tuple_val_inplace) == true);

        fkyaml::node empty_seq = fkyaml::node::sequence();
        REQUIRE_NOTHROW(empty_seq.get_value<std::tuple<>>());
        std::tuple<> empty_tuple_inplace {};
        REQUIRE_NOTHROW(empty_seq.get_value_inplace(empty_tuple_inplace));
    }

#ifdef FK_YAML_HAS_CXX_17
    SECTION("std::optional") {
        fkyaml::node n {true, false};

        auto opt_vec = n.get_value<std::optional<std::vector<bool>>>();
        REQUIRE(opt_vec.has_value());
        REQUIRE(opt_vec.value().size() == 2);
        REQUIRE(opt_vec.value().at(0) == true);
        REQUIRE(opt_vec.value().at(1) == false);

        std::optional<std::vector<bool>> opt_vec_inplace {};
        n.get_value_inplace(opt_vec_inplace);
        REQUIRE(opt_vec_inplace.has_value());
        REQUIRE(opt_vec_inplace.value().size() == 2);
        REQUIRE(opt_vec_inplace.value().at(0) == true);
        REQUIRE(opt_vec_inplace.value().at(1) == false);

        auto opt_bool = n.get_value<std::optional<bool>>();
        REQUIRE_FALSE(opt_bool.has_value());

        std::optional<bool> opt_bool_inplace {};
        n.get_value_inplace(opt_bool_inplace);
        REQUIRE_FALSE(opt_bool_inplace.has_value());
    }
#endif

    SECTION("from alias node") {
        fkyaml::node anchor = 123;
        anchor.add_anchor_name("anchor");
        fkyaml::node alias = fkyaml::node::alias_of(anchor);
        REQUIRE(alias.get_value<int>() == 123);

        int int_inplace = 0;
        alias.get_value_inplace(int_inplace);
        REQUIRE(int_inplace == 123);
    }

    SECTION("not default constructible type") {
        // get_value() requires its output type to be default constructible
        STATIC_REQUIRE_FALSE(
            fkyaml::detail::is_detected<get_value_fn_t, const fkyaml::node&, not_default_constructible>::value);

        // but get_value_inplace() accepts types which are not default constructible.
        not_default_constructible ndc(0);
        fkyaml::node int_node = 1;
        int_node.get_value_inplace(ndc);
        REQUIRE(ndc.value == 1);
    }

    SECTION("unsupported types") {
        STATIC_REQUIRE_FALSE(fkyaml::detail::is_detected<get_value_fn_t, const fkyaml::node&, int*>::value);
        STATIC_REQUIRE_FALSE(fkyaml::detail::is_detected<get_value_fn_t, const fkyaml::node&, int(&)[2]>::value);
        STATIC_REQUIRE_FALSE(fkyaml::detail::is_detected<get_value_fn_t, const fkyaml::node&, int&>::value);
    }
}

template <typename T, typename U>
using get_value_or_fn_t = decltype(std::declval<T>().template get_value_or<U>(std::declval<U>()));

TEST_CASE("Node_GetValueOr") {
    SECTION("sequence (success)") {
        fkyaml::node node = {true, false};

        std::vector<fkyaml::node> vector_node_def {false, true};
        auto vector_node_ldef_ret = node.get_value_or<std::vector<fkyaml::node>>(vector_node_def);
        auto vector_node_rdef_ret = node.get_value_or<std::vector<fkyaml::node>>(std::move(vector_node_def));

        auto validate_nodes = [](const std::vector<fkyaml::node>& nodes) {
            REQUIRE(nodes.size() == 2);
            REQUIRE(nodes.at(0).is_boolean());
            REQUIRE(nodes.at(0).as_bool() == true);
            REQUIRE(nodes.at(1).is_boolean());
            REQUIRE(nodes.at(1).as_bool() == false);
        };
        validate_nodes(vector_node_ldef_ret);
        validate_nodes(vector_node_rdef_ret);

        std::vector<bool> vector_bool_def {false, true};
        auto vector_bool_ldef_ret = node.get_value_or<std::vector<bool>>(vector_bool_def);
        auto vector_bool_rdef_ret = node.get_value_or<std::vector<bool>>(std::move(vector_bool_def));

        auto validate_bools = [](const std::vector<bool>& bools) {
            REQUIRE(bools.size() == 2);
            REQUIRE(bools.at(0) == true);
            REQUIRE(bools.at(1) == false);
        };
        validate_bools(vector_bool_ldef_ret);
        validate_bools(vector_bool_rdef_ret);
    }

    SECTION("sequence (failure)") {
        fkyaml::node node = nullptr;

        std::vector<fkyaml::node> vector_node_def {false, true};
        auto vector_node_ldef_ret = node.get_value_or<std::vector<fkyaml::node>>(vector_node_def);
        auto vector_node_rdef_ret = node.get_value_or<std::vector<fkyaml::node>>(std::move(vector_node_def));

        auto validate_nodes = [](const std::vector<fkyaml::node>& nodes) {
            REQUIRE(nodes.size() == 2);
            REQUIRE(nodes.at(0).is_boolean());
            REQUIRE(nodes.at(0).as_bool() == false);
            REQUIRE(nodes.at(1).is_boolean());
            REQUIRE(nodes.at(1).as_bool() == true);
        };
        // validate_nodes(vector_node_ldef_ret);
        validate_nodes(vector_node_rdef_ret);

        std::vector<bool> vector_bool_def {false, true};
        auto vector_bool_ldef_ret = node.get_value_or<std::vector<bool>>(vector_bool_def);
        auto vector_bool_rdef_ret = node.get_value_or<std::vector<bool>>(std::move(vector_bool_def));

        auto validate_bools = [](const std::vector<bool>& bools) {
            REQUIRE(bools.size() == 2);
            REQUIRE(bools.at(0) == false);
            REQUIRE(bools.at(1) == true);
        };
        validate_bools(vector_bool_ldef_ret);
        validate_bools(vector_bool_rdef_ret);
    }

    SECTION("mapping (success)") {
        fkyaml::node node = {{"foo", true}, {"bar", false}};

        std::map<fkyaml::node, fkyaml::node> map_node_def {{"foo", false}, {"bar", true}};
        auto map_node_ldef_ret = node.get_value_or<std::map<fkyaml::node, fkyaml::node>>(map_node_def);
        auto map_node_rdef_ret = node.get_value_or<std::map<fkyaml::node, fkyaml::node>>(std::move(map_node_def));

        auto validate_nodes = [](const std::map<fkyaml::node, fkyaml::node>& nodes) {
            REQUIRE(nodes.size() == 2);
            REQUIRE(nodes.at("foo").is_boolean());
            REQUIRE(nodes.at("foo").as_bool() == true);
            REQUIRE(nodes.at("bar").is_boolean());
            REQUIRE(nodes.at("bar").as_bool() == false);
        };
        validate_nodes(map_node_ldef_ret);
        validate_nodes(map_node_rdef_ret);

        std::map<std::string, bool> map_bool_def {{"foo", false}, {"bar", true}};
        auto map_bool_ldef_ret = node.get_value_or<std::map<std::string, bool>>(map_bool_def);
        auto map_bool_rdef_ret = node.get_value_or<std::map<std::string, bool>>(std::move(map_bool_def));

        auto validate_bools = [](const std::map<std::string, bool>& bools) {
            REQUIRE(bools.size() == 2);
            REQUIRE(bools.at("foo") == true);
            REQUIRE(bools.at("bar") == false);
        };
        validate_bools(map_bool_ldef_ret);
        validate_bools(map_bool_rdef_ret);
    }

    SECTION("mapping (failure)") {
        fkyaml::node node = nullptr;

        std::map<fkyaml::node, fkyaml::node> map_node_def {{"foo", false}, {"bar", true}};
        auto map_node_ldef_ret = node.get_value_or<std::map<fkyaml::node, fkyaml::node>>(map_node_def);
        auto map_node_rdef_ret = node.get_value_or<std::map<fkyaml::node, fkyaml::node>>(std::move(map_node_def));

        auto validate_nodes = [](const std::map<fkyaml::node, fkyaml::node>& nodes) {
            REQUIRE(nodes.size() == 2);
            REQUIRE(nodes.at("foo").is_boolean());
            REQUIRE(nodes.at("foo").as_bool() == false);
            REQUIRE(nodes.at("bar").is_boolean());
            REQUIRE(nodes.at("bar").as_bool() == true);
        };
        validate_nodes(map_node_ldef_ret);
        validate_nodes(map_node_rdef_ret);

        std::map<std::string, bool> map_bool_def {{"foo", false}, {"bar", true}};
        auto map_bool_ldef_ret = node.get_value_or<std::map<std::string, bool>>(map_bool_def);
        auto map_bool_rdef_ret = node.get_value_or<std::map<std::string, bool>>(std::move(map_bool_def));

        auto validate_bools = [](const std::map<std::string, bool>& bools) {
            REQUIRE(bools.size() == 2);
            REQUIRE(bools.at("foo") == false);
            REQUIRE(bools.at("bar") == true);
        };
        validate_bools(map_bool_ldef_ret);
        validate_bools(map_bool_rdef_ret);
    }

    SECTION("boolean (success)") {
        fkyaml::node node = true;

        bool def = false;
        auto ret = node.get_value_or<bool>(def);
        REQUIRE(ret == true);
    }

    SECTION("boolean (failure)") {
        fkyaml::node node = "foo";

        bool def = false;
        auto ret = node.get_value_or<bool>(def);
        REQUIRE(ret == false);
    }

    SECTION("integer (success)") {
        fkyaml::node node = 123;

        int def = 456;
        auto ret = node.get_value_or<int>(def);
        REQUIRE(ret == 123);
    }

    SECTION("integer (failure)") {
        fkyaml::node node = "foo";

        int def = 456;
        auto ret = node.get_value_or<int>(def);
        REQUIRE(ret == 456);
    }

    SECTION("float (success)") {
        fkyaml::node node = 3.14;

        double def = 1.41;
        auto ret = node.get_value_or<double>(def);
        REQUIRE(std::abs(ret - 3.14) < std::numeric_limits<double>::epsilon());
    }

    SECTION("float (failure)") {
        fkyaml::node node = "foo";

        double def = 1.41;
        auto ret = node.get_value_or<double>(def);
        REQUIRE(std::abs(ret - 1.41) < std::numeric_limits<double>::epsilon());
    }

    SECTION("string (success)") {
        fkyaml::node node = "foo";

        std::string def_str = "default";
        const char def_cstr[] = "default";

        auto ret_ldef_str = node.get_value_or<std::string>(def_str);
        auto ret_rdef_str = node.get_value_or<std::string>(std::move(def_str));
        auto ret_cstr = node.get_value_or<std::string>(def_cstr);
        auto ret_lit = node.get_value_or<std::string>("default");

        REQUIRE(ret_ldef_str == "foo");
        REQUIRE(ret_rdef_str == "foo");
        REQUIRE(ret_cstr == "foo");
        REQUIRE(ret_lit == "foo");
    }

    SECTION("string (failure)") {
        fkyaml::node node = nullptr;

        std::string def_str = "default";
        const char def_cstr[] = "default";

        auto ret_ldef_str = node.get_value_or<std::string>(def_str);
        auto ret_rdef_str = node.get_value_or<std::string>(std::move(def_str));
        auto ret_cstr = node.get_value_or<std::string>(def_cstr);
        auto ret_lit = node.get_value_or<std::string>("default");

        REQUIRE(ret_ldef_str == "default");
        REQUIRE(ret_rdef_str == "default");
        REQUIRE(ret_cstr == "default");
        REQUIRE(ret_lit == "default");
    }

    SECTION("unsupported types") {
        // C-style array types
        STATIC_REQUIRE_FALSE(fkyaml::detail::is_detected<get_value_or_fn_t, const fkyaml::node&, int(&)[2]>::value);
        STATIC_REQUIRE_FALSE(fkyaml::detail::is_detected<get_value_or_fn_t, const fkyaml::node&, int(&)[3][3]>::value);
        STATIC_REQUIRE_FALSE(
            fkyaml::detail::is_detected<get_value_or_fn_t, const fkyaml::node&, int(&)[3][3][3]>::value);

        // pointer types
        STATIC_REQUIRE_FALSE(fkyaml::detail::is_detected<get_value_or_fn_t, const fkyaml::node&, int*>::value);
        STATIC_REQUIRE_FALSE(fkyaml::detail::is_detected<get_value_or_fn_t, const fkyaml::node&, int(&)[2]>::value);

        // reference types
        STATIC_REQUIRE_FALSE(fkyaml::detail::is_detected<get_value_or_fn_t, const fkyaml::node&, int&>::value);
    }
}

//
// test cases for value reference getters
//

TEST_CASE("Node_AsSeq") {
    SECTION("sequence") {
        fkyaml::node node =
            fkyaml::node::sequence(fkyaml::node::sequence_type {fkyaml::node(), fkyaml::node(), fkyaml::node()});

        SECTION("non-alias sequence node") {
            REQUIRE_NOTHROW(node.as_seq());
            REQUIRE(node.as_seq().size() == 3);
            for (int i = 0; i < 3; ++i) {
                REQUIRE(node.as_seq()[i].is_null());
            }
        }

        SECTION("const non-alias sequence node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.as_seq());
            REQUIRE(const_node.as_seq().size() == 3);
            for (int i = 0; i < 3; ++i) {
                REQUIRE(node.as_seq()[i].is_null());
            }
        }

        SECTION("alias sequence node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.as_seq());
            REQUIRE(alias.as_seq().size() == 3);
            for (int i = 0; i < 3; ++i) {
                REQUIRE(alias.as_seq()[i].is_null());
            }
        }

        SECTION("const alias sequence node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.as_seq());
            REQUIRE(alias.as_seq().size() == 3);
            for (int i = 0; i < 3; ++i) {
                REQUIRE(alias.as_seq()[i].is_null());
            }
        }
    }

    SECTION("non-sequence") {
        auto node = GENERATE(
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-sequence nodes") {
            REQUIRE_THROWS_AS(node.as_seq(), fkyaml::type_error);
        }

        SECTION("const non-alias non-sequence nodes") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.as_seq(), fkyaml::type_error);
        }

        SECTION("alias non-sequence nodes") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.as_seq(), fkyaml::type_error);
        }

        SECTION("const alias non-sequence nodes") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.as_seq(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_AsMap") {
    SECTION("mapping") {
        fkyaml::node node = fkyaml::node::mapping(fkyaml::node::mapping_type {
            {"test0", fkyaml::node()}, {"test1", fkyaml::node()}, {"test2", fkyaml::node()}});

        SECTION("non-alias mapping node") {
            REQUIRE_NOTHROW(node.as_map());
            REQUIRE(node.as_map().size() == 3);
            REQUIRE(node["test0"].is_null());
            REQUIRE(node["test1"].is_null());
            REQUIRE(node["test2"].is_null());
        }

        SECTION("const non-alias mapping node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.as_map());
            REQUIRE(const_node.as_map().size() == 3);
            REQUIRE(const_node["test0"].is_null());
            REQUIRE(const_node["test1"].is_null());
            REQUIRE(const_node["test2"].is_null());
        }

        SECTION("alias mapping node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.as_map());
            REQUIRE(alias.as_map().size() == 3);
            REQUIRE(alias["test0"].is_null());
            REQUIRE(alias["test1"].is_null());
            REQUIRE(alias["test2"].is_null());
        }

        SECTION("const alias mapping node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.as_map());
            REQUIRE(alias["test0"].is_null());
            REQUIRE(alias["test1"].is_null());
            REQUIRE(alias["test2"].is_null());
        }
    }

    SECTION("non-mapping") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-mapping nodes") {
            REQUIRE_THROWS_AS(node.as_map(), fkyaml::type_error);
        }

        SECTION("const non-alias non-mapping nodes") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.as_map(), fkyaml::type_error);
        }

        SECTION("alias non-mapping nodes") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.as_map(), fkyaml::type_error);
        }

        SECTION("const alias non-mapping nodes") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.as_map(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_AsBool") {
    SECTION("boolean") {
        fkyaml::node node = true;

        SECTION("non-alias boolean node") {
            REQUIRE_NOTHROW(node.as_bool());
            REQUIRE(node.as_bool() == true);
        }

        SECTION("const non-alias boolean node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.as_bool());
            REQUIRE(const_node.as_bool() == true);
        }

        SECTION("alias boolean node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.as_bool());
            REQUIRE(alias.as_bool() == true);
        }

        SECTION("const alias boolean node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.as_bool());
            REQUIRE(alias.as_bool() == true);
        }
    }

    SECTION("non-boolean") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(0),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-boolean nodes") {
            REQUIRE_THROWS_AS(node.as_bool(), fkyaml::type_error);
        }

        SECTION("const non-alias non-boolean nodes") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.as_bool(), fkyaml::type_error);
        }

        SECTION("alias non-boolean nodes") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.as_bool(), fkyaml::type_error);
        }

        SECTION("const alias non-boolean nodes") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.as_bool(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_AsInt") {
    SECTION("integer") {
        fkyaml::node::integer_type integer = -123;
        fkyaml::node node = integer;

        SECTION("non-alias integer node") {
            REQUIRE_NOTHROW(node.as_int());
            REQUIRE(node.as_int() == integer);
        }

        SECTION("const non-alias integer node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.as_int());
            REQUIRE(const_node.as_int() == integer);
        }

        SECTION("alias  integer node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.as_int());
            REQUIRE(alias.as_int() == integer);
        }

        SECTION("const alias  integer node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.as_int());
            REQUIRE(alias.as_int() == integer);
        }
    }

    SECTION("non-integer") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0.0),
            fkyaml::node(""));

        SECTION("non-alias non-integer nodes") {
            REQUIRE_THROWS_AS(node.as_int(), fkyaml::type_error);
        }

        SECTION("const non-alias non-integer nodes") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.as_int(), fkyaml::type_error);
        }

        SECTION("alias non-integer nodes") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.as_int(), fkyaml::type_error);
        }

        SECTION("const alias non-integer nodes") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.as_int(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_AsFloat") {
    SECTION("floating point number") {
        fkyaml::node::float_number_type float_val = 123.45;
        fkyaml::node node = float_val;

        SECTION("non-alias float number node") {
            REQUIRE_NOTHROW(node.as_float());
            REQUIRE(node.as_float() == float_val);
        }

        SECTION("const non-alias float number node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.as_float());
            REQUIRE(const_node.as_float() == float_val);
        }

        SECTION("alias float number node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.as_float());
            REQUIRE(alias.as_float() == float_val);
        }

        SECTION("const alias float number node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.as_float());
            REQUIRE(alias.as_float() == float_val);
        }
    }

    SECTION("not floating point number") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(""));

        SECTION("non-alias non-float-number nodes") {
            REQUIRE_THROWS_AS(node.as_float(), fkyaml::type_error);
        }

        SECTION("const non-alias non-float-number nodes") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.as_float(), fkyaml::type_error);
        }

        SECTION("alias non-float-number nodes") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.as_float(), fkyaml::type_error);
        }

        SECTION("const alias non-float-number nodes") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.as_float(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_AsStr") {
    SECTION("string") {
        fkyaml::node::string_type str = "test";
        fkyaml::node node = str;

        SECTION("non-alias string node") {
            REQUIRE_NOTHROW(node.as_str());
            REQUIRE(node.as_str() == str);
        }

        SECTION("const non-alias string node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.as_str());
            REQUIRE(const_node.as_str() == str);
        }

        SECTION("alias string node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.as_str());
            REQUIRE(alias.as_str() == str);
        }

        SECTION("const alias string node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.as_str());
            REQUIRE(alias.as_str() == str);
        }
    }

    SECTION("non-string") {
        auto node = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node::mapping(),
            fkyaml::node(),
            fkyaml::node(false),
            fkyaml::node(0),
            fkyaml::node(0.0));

        SECTION("non-alias non-string nodes") {
            REQUIRE_THROWS_AS(node.as_str(), fkyaml::type_error);
        }

        SECTION("const non-alias non-string nodes") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.as_str(), fkyaml::type_error);
        }

        SECTION("alias non-string nodes") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.as_str(), fkyaml::type_error);
        }

        SECTION("const alias non-string nodes") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_THROWS_AS(alias.as_str(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_GetValueRef(deprecated)") {
    SECTION("sequence") {
        SECTION("valid") {
            auto seq = fkyaml::node::sequence();
            REQUIRE_NOTHROW(seq.get_value_ref<fkyaml::node::sequence_type&>());
            REQUIRE_NOTHROW(seq.get_value_ref<const fkyaml::node::sequence_type&>());
        }

        SECTION("invalid") {
            auto non_seq = GENERATE(
                fkyaml::node::mapping(),
                fkyaml::node(),
                fkyaml::node(false),
                fkyaml::node(0),
                fkyaml::node(0.0),
                fkyaml::node(""));
            REQUIRE_THROWS_AS(non_seq.get_value_ref<fkyaml::node::sequence_type&>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(non_seq.get_value_ref<const fkyaml::node::sequence_type&>(), fkyaml::type_error);
        }
    }

    SECTION("mapping") {
        SECTION("valid") {
            auto map = fkyaml::node::mapping();
            REQUIRE_NOTHROW(map.get_value_ref<fkyaml::node::mapping_type&>());
            REQUIRE_NOTHROW(map.get_value_ref<const fkyaml::node::mapping_type&>());
        }

        SECTION("invalid") {
            auto non_map = GENERATE(
                fkyaml::node::sequence(),
                fkyaml::node(),
                fkyaml::node(false),
                fkyaml::node(0),
                fkyaml::node(0.0),
                fkyaml::node(""));
            REQUIRE_THROWS_AS(non_map.get_value_ref<fkyaml::node::mapping_type&>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(non_map.get_value_ref<const fkyaml::node::mapping_type&>(), fkyaml::type_error);
        }
    }

    SECTION("boolean") {
        SECTION("valid") {
            fkyaml::node boolean = true;
            REQUIRE_NOTHROW(boolean.get_value_ref<fkyaml::node::boolean_type&>());
            REQUIRE_NOTHROW(boolean.get_value_ref<const fkyaml::node::boolean_type&>());
        }

        SECTION("invalid") {
            auto non_bool = GENERATE(
                fkyaml::node::sequence(),
                fkyaml::node::mapping(),
                fkyaml::node(),
                fkyaml::node(0),
                fkyaml::node(0.0),
                fkyaml::node(""));
            REQUIRE_THROWS_AS(non_bool.get_value_ref<fkyaml::node::boolean_type&>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(non_bool.get_value_ref<const fkyaml::node::boolean_type&>(), fkyaml::type_error);
        }
    }

    SECTION("integer") {
        SECTION("valid") {
            fkyaml::node integer = 0;
            REQUIRE_NOTHROW(integer.get_value_ref<fkyaml::node::integer_type&>());
            REQUIRE_NOTHROW(integer.get_value_ref<const fkyaml::node::integer_type&>());
        }

        SECTION("invalid") {
            auto non_int = GENERATE(
                fkyaml::node::sequence(),
                fkyaml::node::mapping(),
                fkyaml::node(),
                fkyaml::node(false),
                fkyaml::node(0.0),
                fkyaml::node(""));
            REQUIRE_THROWS_AS(non_int.get_value_ref<fkyaml::node::integer_type&>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(non_int.get_value_ref<const fkyaml::node::integer_type&>(), fkyaml::type_error);
        }
    }

    SECTION("float") {
        SECTION("valid") {
            fkyaml::node float_val = 0.0;
            REQUIRE_NOTHROW(float_val.get_value_ref<fkyaml::node::float_number_type&>());
            REQUIRE_NOTHROW(float_val.get_value_ref<const fkyaml::node::float_number_type&>());
        }

        SECTION("invalid") {
            auto non_float = GENERATE(
                fkyaml::node::sequence(),
                fkyaml::node::mapping(),
                fkyaml::node(),
                fkyaml::node(false),
                fkyaml::node(0),
                fkyaml::node(""));
            REQUIRE_THROWS_AS(non_float.get_value_ref<fkyaml::node::float_number_type&>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(non_float.get_value_ref<const fkyaml::node::float_number_type&>(), fkyaml::type_error);
        }
    }

    SECTION("string") {
        SECTION("valid") {
            fkyaml::node string = "";
            REQUIRE_NOTHROW(string.get_value_ref<fkyaml::node::string_type&>());
            REQUIRE_NOTHROW(string.get_value_ref<const fkyaml::node::string_type&>());
        }

        SECTION("invalid") {
            auto non_string = GENERATE(
                fkyaml::node::sequence(),
                fkyaml::node::mapping(),
                fkyaml::node(),
                fkyaml::node(false),
                fkyaml::node(0),
                fkyaml::node(0.0));
            REQUIRE_THROWS_AS(non_string.get_value_ref<fkyaml::node::string_type&>(), fkyaml::type_error);
            REQUIRE_THROWS_AS(non_string.get_value_ref<const fkyaml::node::string_type&>(), fkyaml::type_error);
        }
    }
}

//
// test cases for iterator getters
//

TEST_CASE("Node_Begin") {
    SECTION("container nodes") {
        auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping());

        SECTION("non-const non-alias container node") {
            REQUIRE_NOTHROW(node.begin());
        }

        SECTION("const non-alias container node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.begin());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.begin());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.begin());
        }
    }

    SECTION("scalar nodes") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));

        SECTION("non-const node") {
            REQUIRE_THROWS_AS(node.begin(), fkyaml::type_error);
        }

        SECTION("const node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.begin(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_ConstBegin") {
    SECTION("container nodes") {
        auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping());

        SECTION("non-const non-alias container node") {
            REQUIRE_NOTHROW(node.cbegin());
        }

        SECTION("const non-alias container node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.cbegin());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.cbegin());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.cbegin());
        }
    }

    SECTION("scalar nodes") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));

        SECTION("non-const node") {
            REQUIRE_THROWS_AS(node.cbegin(), fkyaml::type_error);
        }

        SECTION("const node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.cbegin(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_End") {
    SECTION("container nodes") {
        auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping());

        SECTION("non-const non-alias container node") {
            REQUIRE_NOTHROW(node.end());
        }

        SECTION("const non-alias container node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.end());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.end());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.end());
        }
    }

    SECTION("scalar nodes") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));

        SECTION("non-const throwing node") {
            REQUIRE_THROWS_AS(node.end(), fkyaml::type_error);
        }

        SECTION("const throwing node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.end(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_ConstEnd") {
    SECTION("container nodes") {
        auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping());

        SECTION("non-const non-alias container node") {
            REQUIRE_NOTHROW(node.cend());
        }

        SECTION("const non-alias container node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.cend());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.cend());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.cend());
        }
    }

    SECTION("scalar nodes") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));

        SECTION("non-const throwing node") {
            REQUIRE_THROWS_AS(node.cend(), fkyaml::type_error);
        }

        SECTION("const throwing node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.cend(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_ReverseBegin") {
    SECTION("container nodes") {
        auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping());

        SECTION("non-const non-alias container node") {
            REQUIRE_NOTHROW(node.rbegin());
        }

        SECTION("const non-alias container node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.rbegin());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.rbegin());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.rbegin());
        }
    }

    SECTION("scalar nodes") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));

        SECTION("non-const throwing node") {
            REQUIRE_THROWS_AS(node.rbegin(), fkyaml::type_error);
        }

        SECTION("const throwing node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.rbegin(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_ConstReverseBegin") {
    SECTION("container nodes") {
        auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping());

        SECTION("non-const non-alias container node") {
            REQUIRE_NOTHROW(node.crbegin());
        }

        SECTION("const non-alias container node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.crbegin());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.crbegin());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.crbegin());
        }
    }

    SECTION("scalar nodes") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));

        SECTION("non-const throwing node") {
            REQUIRE_THROWS_AS(node.crbegin(), fkyaml::type_error);
        }

        SECTION("const throwing node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.crbegin(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_ReverseEnd") {
    SECTION("container nodes") {
        auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping());

        SECTION("non-const non-alias container node") {
            REQUIRE_NOTHROW(node.rend());
        }

        SECTION("const non-alias container node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.rend());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.rend());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.rend());
        }
    }

    SECTION("scalar nodes") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));

        SECTION("non-const throwing node") {
            REQUIRE_THROWS_AS(node.rend(), fkyaml::type_error);
        }

        SECTION("const throwing node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.rend(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_ConstReverseEnd") {
    SECTION("container nodes") {
        auto node = GENERATE(fkyaml::node::sequence(), fkyaml::node::mapping());

        SECTION("non-const non-alias container node") {
            REQUIRE_NOTHROW(node.crend());
        }

        SECTION("const non-alias container node") {
            const fkyaml::node const_node = node;
            REQUIRE_NOTHROW(const_node.crend());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.crend());
        }

        SECTION("non-const alias container node") {
            node.add_anchor_name("anchor_name");
            const fkyaml::node alias = fkyaml::node::alias_of(node);
            REQUIRE_NOTHROW(alias.crend());
        }
    }

    SECTION("scalar nodes") {
        auto node = GENERATE(fkyaml::node(), fkyaml::node(false), fkyaml::node(0), fkyaml::node(0.0), fkyaml::node(""));

        SECTION("non-const throwing node") {
            REQUIRE_THROWS_AS(node.crend(), fkyaml::type_error);
        }

        SECTION("const throwing node") {
            const fkyaml::node const_node = node;
            REQUIRE_THROWS_AS(const_node.crend(), fkyaml::type_error);
        }
    }
}

TEST_CASE("Node_MapItems") {
    int i = 0;
    fkyaml::node keys[2] = {"bar", "foo"};
    fkyaml::node values[2] = {-456, 123};

    SECTION("non-const version") {
        fkyaml::node map = {{"foo", 123}, {"bar", -456}};

        for (auto& entry : map.map_items()) {
            REQUIRE(entry.key() == keys[i]);
            REQUIRE(entry.value() == values[i]);
            ++i;
        }

        i = 0;
        for (const auto& entry : map.map_items()) {
            REQUIRE(entry.key() == keys[i]);
            REQUIRE(entry.value() == values[i]);
            ++i;
        }
    }

    SECTION("const version") {
        const fkyaml::node c_map = {{"foo", 123}, {"bar", -456}};

        for (const auto& c_entry : c_map.map_items()) {
            REQUIRE(c_entry.key() == keys[i]);
            REQUIRE(c_entry.value() == values[i]);
            ++i;
        }
    }

    SECTION("not mapping") {
        auto non_mapping = GENERATE(
            fkyaml::node::sequence(),
            fkyaml::node(nullptr),
            fkyaml::node(true),
            fkyaml::node(123),
            fkyaml::node(3.14),
            fkyaml::node("foo"));
        const auto const_non_mapping = non_mapping;
        REQUIRE_THROWS_AS(non_mapping.map_items(), fkyaml::type_error);
        REQUIRE_THROWS_AS(const_non_mapping.map_items(), fkyaml::type_error);
    }

#if defined(__cpp_structured_bindings) && __cpp_structured_bindings >= 201606L
    SECTION("structured bindings") {
        fkyaml::node map = {{"foo", 123}, {"bar", -456}};

        for (auto& [key, value] : map.map_items()) {
            REQUIRE(key == keys[i]);
            REQUIRE(value == values[i]);
            ++i;
        }
    }
#endif
}

//
// test cases for swap
//

TEST_CASE("Node_Swap") {
    fkyaml::node lhs_node = true;
    fkyaml::node rhs_node = 123;
    lhs_node.swap(rhs_node);
    REQUIRE(lhs_node.is_integer());
    REQUIRE(lhs_node.as_int() == 123);
    REQUIRE(rhs_node.is_boolean());
    REQUIRE(rhs_node.as_bool() == true);
}

TEST_CASE("Node_ADLSwap") {
    fkyaml::node lhs_node = true;
    fkyaml::node rhs_node = 123;

    using std::swap;
    swap(lhs_node, rhs_node);

    REQUIRE(lhs_node.is_integer());
    REQUIRE(lhs_node.as_int() == 123);
    REQUIRE(rhs_node.is_boolean());
    REQUIRE(rhs_node.as_bool() == true);
}
