//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <fstream>
#include <iostream>

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>
#include <yaml_test_suite_dir.hpp>

#define SKIP(msg) SUCCEED("Skipped: " msg)
#define NOT_IMPLEMENTED(msg) FAIL("Not implemented: " msg)

static void print_label(const std::string& path) {
    std::ifstream label_file(path);
    if (label_file) {
        std::string label;
        std::getline(label_file, label);
        std::cout << "label: " << label << std::endl;
    }
}

TEST_CASE("yaml_test_suite_2AUY") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2AUY/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2AUY/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 4);

        REQUIRE(n.at(0).is_string());
        REQUIRE(n.at(0).get_tag_name() == "!!str");
        REQUIRE(n.at(0).as_str() == "a");

        REQUIRE(n.at(1).is_string());
        REQUIRE(n.at(1).as_str() == "b");

        REQUIRE(n.at(2).is_integer());
        REQUIRE(n.at(2).get_tag_name() == "!!int");
        REQUIRE(n.at(2).as_int() == 42);

        REQUIRE(n.at(3).is_string());
        REQUIRE(n.at(3).as_str() == "d");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2AUY/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 4);

        REQUIRE(n.at(0).is_string());
        REQUIRE(n.at(0).as_str() == "a");

        REQUIRE(n.at(1).is_string());
        REQUIRE(n.at(1).as_str() == "b");

        REQUIRE(n.at(2).is_integer());
        REQUIRE(n.at(2).as_int() == 42);

        REQUIRE(n.at(3).is_string());
        REQUIRE(n.at(3).as_str() == "d");
    }
}

TEST_CASE("yaml_test_suite_2CMS") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2CMS/===");
    fkyaml::node n;
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2CMS/in.yaml");
    REQUIRE(ifs);
    REQUIRE_THROWS_AS(ifs >> n, fkyaml::exception);
}

TEST_CASE("yaml_test_suite_2EBW") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2EBW/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2EBW/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 5);

        REQUIRE(n.at("a!\"#$%&\'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~").is_string());
        REQUIRE(
            n.at("a!\"#$%&\'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~").as_str() == "safe");

        REQUIRE(n.at("?foo").is_string());
        REQUIRE(n.at("?foo").as_str() == "safe question mark");

        REQUIRE(n.at(":foo").is_string());
        REQUIRE(n.at(":foo").as_str() == "safe colon");

        REQUIRE(n.at("-foo").is_string());
        REQUIRE(n.at("-foo").as_str() == "safe dash");

        REQUIRE(n.at("this is#not").is_string());
        REQUIRE(n.at("this is#not").as_str() == "a comment");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2EBW/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 5);

        REQUIRE(n.at("a!\"#$%&\'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~").is_string());
        REQUIRE(
            n.at("a!\"#$%&\'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~").as_str() == "safe");

        REQUIRE(n.at("?foo").is_string());
        REQUIRE(n.at("?foo").as_str() == "safe question mark");

        REQUIRE(n.at(":foo").is_string());
        REQUIRE(n.at(":foo").as_str() == "safe colon");

        REQUIRE(n.at("-foo").is_string());
        REQUIRE(n.at("-foo").as_str() == "safe dash");

        REQUIRE(n.at("this is#not").is_string());
        REQUIRE(n.at("this is#not").as_str() == "a comment");
    }
}

TEST_CASE("yaml_test_suite_2G84-00") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2G84/00/===");
    fkyaml::node n;
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2G84/00/in.yaml");
    REQUIRE(ifs);
    REQUIRE_THROWS_AS(ifs >> n, fkyaml::exception);
}

TEST_CASE("yaml_test_suite_2G84-01") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2G84/01/===");
    fkyaml::node n;
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2G84/01/in.yaml");
    REQUIRE(ifs);
    REQUIRE_THROWS_AS(ifs >> n, fkyaml::exception);
}

TEST_CASE("yaml_test_suite_2G84-02") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2G84/02/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2G84/02/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2G84/02/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "");
    }
}

TEST_CASE("yaml_test_suite_2G84-03") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2G84/03/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2G84/03/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2G84/03/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "");
    }
}

TEST_CASE("yaml_test_suite_2JQS") {
    SKIP("Empty keys are NOT supported.");
}

TEST_CASE("yaml_test_suite_2LFX") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2LFX/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2LFX/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "foo");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2LFX/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "foo");
    }
}

TEST_CASE("yaml_test_suite_2SXE") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2SXE/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2SXE/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 2);
        for (auto& it : n.map_items()) {
            auto& key = it.key();
            REQUIRE(key.is_string());

            if (key.as_str() == "key") {
                REQUIRE(key.is_anchor());
                REQUIRE(key.get_anchor_name() == "a:");

                REQUIRE(it.value().is_string());
                REQUIRE(it.value().is_anchor());
                REQUIRE(it.value().get_anchor_name() == "a");
                REQUIRE(it.value().as_str() == "value");
            }
            else {
                REQUIRE(key.as_str() == "foo");

                REQUIRE(it.value().is_alias());
                REQUIRE(it.value().get_anchor_name() == "a:");
                REQUIRE(it.value().as_str() == "key");
            }
        }
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2SXE/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at("key").is_string());
        REQUIRE(n.at("key").as_str() == "value");

        REQUIRE(n.at("foo").is_string());
        REQUIRE(n.at("foo").as_str() == "key");
    }
}

TEST_CASE("yaml_test_suite_2XXW") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2XXW/===");
    fkyaml::node n;

    SECTION("YAML") {
        NOT_IMPLEMENTED("explicit empty mapping value, !!set");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/2XXW/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 3);

        REQUIRE(n.at("Mark McGwire").is_null());
        REQUIRE(n.at("Sammy Sosa").is_null());
        REQUIRE(n.at("Ken Griff").is_null());
    }
}

TEST_CASE("yaml_test_suite_3ALJ") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3ALJ/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3ALJ/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at(0).is_sequence());
        REQUIRE(n.at(0).size() == 2);

        REQUIRE(n.at(0).at(0).is_string());
        REQUIRE(n.at(0).at(0).as_str() == "s1_i1");

        REQUIRE(n.at(0).at(1).is_string());
        REQUIRE(n.at(0).at(1).as_str() == "s1_i2");

        REQUIRE(n.at(1).is_string());
        REQUIRE(n.at(1).as_str() == "s2");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3ALJ/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at(0).is_sequence());
        REQUIRE(n.at(0).size() == 2);

        REQUIRE(n.at(0).at(0).is_string());
        REQUIRE(n.at(0).at(0).as_str() == "s1_i1");

        REQUIRE(n.at(0).at(1).is_string());
        REQUIRE(n.at(0).at(1).as_str() == "s1_i2");

        REQUIRE(n.at(1).is_string());
        REQUIRE(n.at(1).as_str() == "s2");
    }
}

TEST_CASE("yaml_test_suite_3GZX") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3GZX/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3GZX/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 4);

        REQUIRE(n.at("First occurrence").is_string());
        REQUIRE(n.at("First occurrence").is_anchor());
        REQUIRE(n.at("First occurrence").get_anchor_name() == "anchor");
        REQUIRE(n.at("First occurrence").as_str() == "Foo");

        REQUIRE(n.at("Second occurrence").is_string());
        REQUIRE(n.at("Second occurrence").is_alias());
        REQUIRE(n.at("Second occurrence").get_anchor_name() == "anchor");
        REQUIRE(n.at("Second occurrence").as_str() == "Foo");

        REQUIRE(n.at("Override anchor").is_string());
        REQUIRE(n.at("Override anchor").is_anchor());
        REQUIRE(n.at("Override anchor").get_anchor_name() == "anchor");
        REQUIRE(n.at("Override anchor").as_str() == "Bar");

        REQUIRE(n.at("Reuse anchor").is_string());
        REQUIRE(n.at("Reuse anchor").is_alias());
        REQUIRE(n.at("Reuse anchor").get_anchor_name() == "anchor");
        REQUIRE(n.at("Reuse anchor").as_str() == "Bar");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3GZX/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 4);

        REQUIRE(n.at("First occurrence").is_string());
        REQUIRE(n.at("First occurrence").as_str() == "Foo");

        REQUIRE(n.at("Second occurrence").is_string());
        REQUIRE(n.at("Second occurrence").as_str() == "Foo");

        REQUIRE(n.at("Override anchor").is_string());
        REQUIRE(n.at("Override anchor").as_str() == "Bar");

        REQUIRE(n.at("Reuse anchor").is_string());
        REQUIRE(n.at("Reuse anchor").as_str() == "Bar");
    }
}

TEST_CASE("yaml_test_suite_3HFZ") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3HFZ/===");
    fkyaml::node n;
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3HFZ/in.yaml");
    REQUIRE(ifs);
    REQUIRE_THROWS_AS(ifs >> n, fkyaml::exception);
}

TEST_CASE("yaml_test_suite_3MYT") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3MYT/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3MYT/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "k:#foo &a !t s");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3MYT/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "k:#foo &a !t s");
    }
}

TEST_CASE("yaml_test_suite_3R3P") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3R3P/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3R3P/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.is_anchor());
        REQUIRE(n.get_anchor_name() == "sequence");
        REQUIRE(n.size() == 1);

        REQUIRE(n.at(0).is_string());
        REQUIRE(n.at(0).as_str() == "a");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3R3P/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at(0).is_string());
        REQUIRE(n.at(0).as_str() == "a");
    }
}

TEST_CASE("yaml_test_suite_3RLN-00") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/00/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/00/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "1 leading \ttab");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/00/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "1 leading \ttab");
    }
}

TEST_CASE("yaml_test_suite_3RLN-01") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/01/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/01/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "2 leading \ttab");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/01/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "2 leading \ttab");
    }
}

TEST_CASE("yaml_test_suite_3RLN-02") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/02/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/02/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "3 leading tab");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/02/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "3 leading tab");
    }
}

TEST_CASE("yaml_test_suite_3RLN-03") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/03/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/03/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "4 leading \t  tab");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/03/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "4 leading \t  tab");
    }
}

TEST_CASE("yaml_test_suite_3RLN-04") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/04/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/04/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "5 leading \t  tab");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/04/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "5 leading \t  tab");
    }
}

TEST_CASE("yaml_test_suite_3RLN-05") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/05/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/05/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "6 leading tab");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3RLN/05/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "6 leading tab");
    }
}

TEST_CASE("yaml_test_suite_3UYS") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3UYS/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3UYS/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("escaped slash").is_string());
        REQUIRE(n.at("escaped slash").as_str() == "a/b");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/3UYS/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("escaped slash").is_string());
        REQUIRE(n.at("escaped slash").as_str() == "a/b");
    }
}

TEST_CASE("yaml_test_suite_4ABK") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4ABK/===");

    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4ABK/in.yaml");
    REQUIRE(ifs);
    fkyaml::node n;
    REQUIRE_NOTHROW(ifs >> n);

    REQUIRE(n.is_mapping());
    REQUIRE(n.size() == 3);

    REQUIRE(n.at("unquoted").is_string());
    REQUIRE(n.at("unquoted").as_str() == "separate");
    REQUIRE(n.at("http://foo.com").is_null());
    REQUIRE(n.at("omitted value").is_null());
}

TEST_CASE("yaml_test_suite_4CQQ") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4CQQ/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4CQQ/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at("plain").is_string());
        REQUIRE(n.at("plain").as_str() == "This unquoted scalar spans many lines.");

        REQUIRE(n.at("quoted").is_string());
        REQUIRE(n.at("quoted").as_str() == "So does this quoted scalar.\n");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4CQQ/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at("plain").is_string());
        REQUIRE(n.at("plain").as_str() == "This unquoted scalar spans many lines.");

        REQUIRE(n.at("quoted").is_string());
        REQUIRE(n.at("quoted").as_str() == "So does this quoted scalar.\n");
    }
}

TEST_CASE("yaml_test_suite_4EJS") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4EJS/===");
    fkyaml::node n;
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4EJS/in.yaml");
    REQUIRE(ifs);
    REQUIRE_THROWS_AS(ifs >> n, fkyaml::exception);
}

TEST_CASE("yaml_test_suite_4FJ6") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4FJ6/===");
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4FJ6/in.yaml");
    REQUIRE(ifs);
    fkyaml::node n;
    REQUIRE_NOTHROW(ifs >> n);

    REQUIRE(n.is_sequence());
    REQUIRE(n.size() == 1);

    REQUIRE(n.at(0).is_mapping());
    REQUIRE(n.at(0).size() == 1);

    auto& key = n.at(0).begin().key();
    REQUIRE(key.is_sequence());
    REQUIRE(key.size() == 2);

    REQUIRE(key.at(0).is_string());
    REQUIRE(key.at(0).as_str() == "a");

    REQUIRE(key.at(1).is_sequence());
    REQUIRE(key.at(1).size() == 2);

    REQUIRE(key.at(1).at(0).is_mapping());
    REQUIRE(key.at(1).at(0).size() == 1);

    auto& key2 = key.at(1).at(0).begin().key();
    REQUIRE(key2.is_sequence());
    REQUIRE(key2.size() == 1);

    REQUIRE(key2.at(0).is_sequence());
    REQUIRE(key2.at(0).size() == 2);

    REQUIRE(key2.at(0).at(0).is_string());
    REQUIRE(key2.at(0).at(0).as_str() == "b");

    REQUIRE(key2.at(0).at(1).is_string());
    REQUIRE(key2.at(0).at(1).as_str() == "c");

    auto& val2 = key.at(1).at(0).begin().value();
    REQUIRE(val2.is_string());
    REQUIRE(val2.as_str() == "d");

    REQUIRE(key.at(1).at(1).is_string());
    REQUIRE(key.at(1).at(1).as_str() == "e");

    auto& val = n.at(0).begin().value();
    REQUIRE(val.is_integer());
    REQUIRE(val.as_int() == 23);
}

TEST_CASE("yaml_test_suite_4GC6") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4GC6/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4GC6/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "here\'s to \"quotes\"");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4GC6/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "here\'s to \"quotes\"");
    }
}

TEST_CASE("yaml_test_suite_4H7K") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4H7K/===");
    fkyaml::node n;
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4H7K/in.yaml");
    REQUIRE(ifs);
    REQUIRE_THROWS_AS(ifs >> n, fkyaml::exception);
}

TEST_CASE("yaml_test_suite_4HVU") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4HVU/===");
    fkyaml::node n;
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4HVU/in.yaml");
    REQUIRE(ifs);
    REQUIRE_THROWS_AS(ifs >> n, fkyaml::exception);
}

TEST_CASE("yaml_test_suite_4JVG") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4JVG/===");
    fkyaml::node n;
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4JVG/in.yaml");
    REQUIRE(ifs);
    REQUIRE_THROWS_AS(ifs >> n, fkyaml::exception);
}

TEST_CASE("yaml_test_suite_4MUZ-00") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4MUZ/00/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4MUZ/00/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("foo").is_string());
        REQUIRE(n.at("foo").as_str() == "bar");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4MUZ/00/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("foo").is_string());
        REQUIRE(n.at("foo").as_str() == "bar");
    }
}

TEST_CASE("yaml_test_suite_4MUZ-01") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4MUZ/01/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4MUZ/01/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("foo").is_string());
        REQUIRE(n.at("foo").as_str() == "bar");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4MUZ/01/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("foo").is_string());
        REQUIRE(n.at("foo").as_str() == "bar");
    }
}

TEST_CASE("yaml_test_suite_4MUZ-02") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4MUZ/02/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4MUZ/02/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("foo").is_string());
        REQUIRE(n.at("foo").as_str() == "bar");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4MUZ/02/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("foo").is_string());
        REQUIRE(n.at("foo").as_str() == "bar");
    }
}

TEST_CASE("yaml_test_suite_4Q9F") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4Q9F/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4Q9F/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "ab cd\nef\n\ngh\n");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4Q9F/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "ab cd\nef\n\ngh\n");
    }
}

TEST_CASE("yaml_test_suite_4QFQ") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4QFQ/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4QFQ/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 4);

        REQUIRE(n.at(0).is_string());
        REQUIRE(n.at(0).as_str() == "detected\n");

        REQUIRE(n.at(1).is_string());
        REQUIRE(n.at(1).as_str() == "\n\n# detected\n");

        REQUIRE(n.at(2).is_string());
        REQUIRE(n.at(2).as_str() == " explicit\n");

        REQUIRE(n.at(3).is_string());
        REQUIRE(n.at(3).as_str() == "detected\n");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4QFQ/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 4);

        REQUIRE(n.at(0).is_string());
        REQUIRE(n.at(0).as_str() == "detected\n");

        REQUIRE(n.at(1).is_string());
        REQUIRE(n.at(1).as_str() == "\n\n# detected\n");

        REQUIRE(n.at(2).is_string());
        REQUIRE(n.at(2).as_str() == " explicit\n");

        REQUIRE(n.at(3).is_string());
        REQUIRE(n.at(3).as_str() == "detected\n");
    }
}

TEST_CASE("yaml_test_suite_4RWC") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4RWC/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4RWC/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 3);

        REQUIRE(n.at(0).is_integer());
        REQUIRE(n.at(0).as_int() == 1);

        REQUIRE(n.at(1).is_integer());
        REQUIRE(n.at(1).as_int() == 2);

        REQUIRE(n.at(2).is_integer());
        REQUIRE(n.at(2).as_int() == 3);
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4RWC/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 3);

        REQUIRE(n.at(0).is_integer());
        REQUIRE(n.at(0).as_int() == 1);

        REQUIRE(n.at(1).is_integer());
        REQUIRE(n.at(1).as_int() == 2);

        REQUIRE(n.at(2).is_integer());
        REQUIRE(n.at(2).as_int() == 3);
    }
}

TEST_CASE("yaml_test_suite_4UYU") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4UYU/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4UYU/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "foo: bar\": baz");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4UYU/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "foo: bar\": baz");
    }
}

TEST_CASE("yaml_test_suite_4V8U") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4V8U/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4V8U/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "plain\\value\\with\\backslashes");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4V8U/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "plain\\value\\with\\backslashes");
    }
}

TEST_CASE("yaml_test_suite_4WA9") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4WA9/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4WA9/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at(0).is_mapping());
        REQUIRE(n.at(0).size() == 2);

        REQUIRE(n.at(0).at("aaa").is_string());
        REQUIRE(n.at(0).at("aaa").as_str() == "xxx\n");

        REQUIRE(n.at(0).at("bbb").is_string());
        REQUIRE(n.at(0).at("bbb").as_str() == "xxx\n");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4WA9/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at(0).is_mapping());
        REQUIRE(n.at(0).size() == 2);

        REQUIRE(n.at(0).at("aaa").is_string());
        REQUIRE(n.at(0).at("aaa").as_str() == "xxx\n");

        REQUIRE(n.at(0).at("bbb").is_string());
        REQUIRE(n.at(0).at("bbb").as_str() == "xxx\n");
    }
}

TEST_CASE("yaml_test_suite_4ZYM") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4ZYM/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4ZYM/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 3);

        REQUIRE(n.at("plain").is_string());
        REQUIRE(n.at("plain").as_str() == "text lines");

        REQUIRE(n.at("quoted").is_string());
        REQUIRE(n.at("quoted").as_str() == "text lines");

        REQUIRE(n.at("block").is_string());
        REQUIRE(n.at("block").as_str() == "text\n \tlines\n");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/4ZYM/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 3);

        REQUIRE(n.at("plain").is_string());
        REQUIRE(n.at("plain").as_str() == "text lines");

        REQUIRE(n.at("quoted").is_string());
        REQUIRE(n.at("quoted").as_str() == "text lines");

        REQUIRE(n.at("block").is_string());
        REQUIRE(n.at("block").as_str() == "text\n \tlines\n");
    }
}

TEST_CASE("yaml_test_suite_5BVJ") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5BVJ/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5BVJ/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at("literal").is_string());
        REQUIRE(n.at("literal").as_str() == "some\ntext\n");

        REQUIRE(n.at("folded").is_string());
        REQUIRE(n.at("folded").as_str() == "some text\n");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5BVJ/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at("literal").is_string());
        REQUIRE(n.at("literal").as_str() == "some\ntext\n");

        REQUIRE(n.at("folded").is_string());
        REQUIRE(n.at("folded").as_str() == "some text\n");
    }
}

TEST_CASE("yaml_test_suite_5C5M") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5C5M/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5C5M/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at(0).is_mapping());
        REQUIRE(n.at(0).size() == 2);

        REQUIRE(n.at(0).at("one").is_string());
        REQUIRE(n.at(0).at("one").as_str() == "two");

        REQUIRE(n.at(0).at("three").is_string());
        REQUIRE(n.at(0).at("three").as_str() == "four");

        REQUIRE(n.at(1).is_mapping());
        REQUIRE(n.at(1).size() == 2);

        REQUIRE(n.at(1).at("five").is_string());
        REQUIRE(n.at(1).at("five").as_str() == "six");

        REQUIRE(n.at(1).at("seven").is_string());
        REQUIRE(n.at(1).at("seven").as_str() == "eight");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5C5M/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at(0).is_mapping());
        REQUIRE(n.at(0).size() == 2);

        REQUIRE(n.at(0).at("one").is_string());
        REQUIRE(n.at(0).at("one").as_str() == "two");

        REQUIRE(n.at(0).at("three").is_string());
        REQUIRE(n.at(0).at("three").as_str() == "four");

        REQUIRE(n.at(1).is_mapping());
        REQUIRE(n.at(1).size() == 2);

        REQUIRE(n.at(1).at("five").is_string());
        REQUIRE(n.at(1).at("five").as_str() == "six");

        REQUIRE(n.at(1).at("seven").is_string());
        REQUIRE(n.at(1).at("seven").as_str() == "eight");
    }
}

TEST_CASE("yaml_test_suite_5GBF") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5GBF/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5GBF/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at("Folding").is_string());
        REQUIRE(n.at("Folding").as_str() == "Empty line\nas a line feed");

        REQUIRE(n.at("Chomping").is_string());
        REQUIRE(n.at("Chomping").as_str() == "Clipped empty lines\n");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5GBF/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at("Folding").is_string());
        REQUIRE(n.at("Folding").as_str() == "Empty line\nas a line feed");

        REQUIRE(n.at("Chomping").is_string());
        REQUIRE(n.at("Chomping").as_str() == "Clipped empty lines\n");
    }
}

TEST_CASE("yaml_test_suite_5KJE") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5KJE/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5KJE/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at(0).is_sequence());
        REQUIRE(n.at(0).size() == 2);

        REQUIRE(n.at(0).at(0).is_string());
        REQUIRE(n.at(0).at(0).as_str() == "one");

        REQUIRE(n.at(0).at(1).is_string());
        REQUIRE(n.at(0).at(1).as_str() == "two");

        REQUIRE(n.at(1).is_sequence());
        REQUIRE(n.at(1).size() == 2);

        REQUIRE(n.at(1).at(0).is_string());
        REQUIRE(n.at(1).at(0).as_str() == "three");

        REQUIRE(n.at(1).at(1).is_string());
        REQUIRE(n.at(1).at(1).as_str() == "four");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5KJE/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at(0).is_sequence());
        REQUIRE(n.at(0).size() == 2);

        REQUIRE(n.at(0).at(0).is_string());
        REQUIRE(n.at(0).at(0).as_str() == "one");

        REQUIRE(n.at(0).at(1).is_string());
        REQUIRE(n.at(0).at(1).as_str() == "two");

        REQUIRE(n.at(1).is_sequence());
        REQUIRE(n.at(1).size() == 2);

        REQUIRE(n.at(1).at(0).is_string());
        REQUIRE(n.at(1).at(0).as_str() == "three");

        REQUIRE(n.at(1).at(1).is_string());
        REQUIRE(n.at(1).at(1).as_str() == "four");
    }
}

TEST_CASE("yaml_test_suite_5LLU") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5LLU/===");
    fkyaml::node n;
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5LLU/in.yaml");
    REQUIRE(ifs);
    REQUIRE_THROWS_AS(ifs >> n, fkyaml::exception);
}

TEST_CASE("yaml_test_suite_5MUD") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5MUD/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5MUD/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("foo").is_string());
        REQUIRE(n.at("foo").as_str() == "bar");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5MUD/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("foo").is_string());
        REQUIRE(n.at("foo").as_str() == "bar");
    }
}

TEST_CASE("yaml_test_suite_5NYZ") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5NYZ/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5NYZ/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("key").is_string());
        REQUIRE(n.at("key").as_str() == "value");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5NYZ/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("key").is_string());
        REQUIRE(n.at("key").as_str() == "value");
    }
}

TEST_CASE("yaml_test_suite_5T43") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5T43/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5T43/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at(0).is_mapping());
        REQUIRE(n.at(0).size() == 1);

        REQUIRE(n.at(0).at("key").is_string());
        REQUIRE(n.at(0).at("key").as_str() == "value");

        REQUIRE(n.at(1).is_mapping());
        REQUIRE(n.at(1).size() == 1);

        REQUIRE(n.at(1).at("key").is_string());
        REQUIRE(n.at(1).at("key").as_str() == ":value");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5T43/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at(0).is_mapping());
        REQUIRE(n.at(0).size() == 1);

        REQUIRE(n.at(0).at("key").is_string());
        REQUIRE(n.at(0).at("key").as_str() == "value");

        REQUIRE(n.at(1).is_mapping());
        REQUIRE(n.at(1).size() == 1);

        REQUIRE(n.at(1).at("key").is_string());
        REQUIRE(n.at(1).at("key").as_str() == ":value");
    }
}

TEST_CASE("yaml_test_suite_5TRB") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5TRB/===");
    fkyaml::node n;
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5TRB/in.yaml");
    REQUIRE(ifs);
    REQUIRE_THROWS_AS(ifs >> n, fkyaml::exception);
}

TEST_CASE("yaml_test_suite_5TYM") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5TYM/===");

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5TYM/in.yaml");
        REQUIRE(ifs);
        std::vector<fkyaml::node> docs;
        REQUIRE_NOTHROW(docs = fkyaml::node::deserialize_docs(ifs));

        REQUIRE(docs.size() == 2);

        REQUIRE(docs.at(0).is_string());
        REQUIRE(docs.at(0).get_tag_name() == "!m!light");
        REQUIRE(docs.at(0).as_str() == "fluorescent");

        REQUIRE(docs.at(1).is_string());
        REQUIRE(docs.at(1).get_tag_name() == "!m!light");
        REQUIRE(docs.at(1).as_str() == "green");
    }

    SECTION("JSON") {
        SKIP("Malformed JSON generated from multiple YAML documents.");
    }
}

TEST_CASE("yaml_test_suite_5U3A") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5U3A/===");
    fkyaml::node n;
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5U3A/in.yaml");
    REQUIRE(ifs);
    REQUIRE_THROWS_AS(ifs >> n, fkyaml::exception);
}

TEST_CASE("yaml_test_suite_5WE3") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5WE3/===");
    fkyaml::node n;

    SECTION("YAML") {
        NOT_IMPLEMENTED("explicit empty mapping value");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/5WE3/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at("explicit key").is_null());

        REQUIRE(n.at("block key\n").is_sequence());
        REQUIRE(n.at("block key\n").size() == 2);

        REQUIRE(n.at("block key\n").at(0).is_string());
        REQUIRE(n.at("block key\n").at(0).as_str() == "one");

        REQUIRE(n.at("block key\n").at(1).is_string());
        REQUIRE(n.at("block key\n").at(1).as_str() == "two");
    }
}

TEST_CASE("yaml_test_suite_6BCT") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6BCT/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6BCT/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at(0).is_mapping());
        REQUIRE(n.at(0).size() == 1);

        REQUIRE(n.at(0).at("foo").is_string());
        REQUIRE(n.at(0).at("foo").as_str() == "bar");

        REQUIRE(n.at(1).is_sequence());
        REQUIRE(n.at(1).size() == 2);

        REQUIRE(n.at(1).at(0).is_string());
        REQUIRE(n.at(1).at(0).as_str() == "baz");

        REQUIRE(n.at(1).at(1).is_string());
        REQUIRE(n.at(1).at(1).as_str() == "baz");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6BCT/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at(0).is_mapping());
        REQUIRE(n.at(0).size() == 1);

        REQUIRE(n.at(0).at("foo").is_string());
        REQUIRE(n.at(0).at("foo").as_str() == "bar");

        REQUIRE(n.at(1).is_sequence());
        REQUIRE(n.at(1).size() == 2);

        REQUIRE(n.at(1).at(0).is_string());
        REQUIRE(n.at(1).at(0).as_str() == "baz");

        REQUIRE(n.at(1).at(1).is_string());
        REQUIRE(n.at(1).at(1).as_str() == "baz");
    }
}

TEST_CASE("yaml_test_suite_6BFJ") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6BFJ/===");
    fkyaml::node n;
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6BFJ/in.yaml");
    REQUIRE(ifs);
    REQUIRE_NOTHROW(ifs >> n);

    REQUIRE(n.is_mapping());
    REQUIRE(n.size() == 1);
    REQUIRE(n.is_anchor());
    REQUIRE(n.get_anchor_name() == "mapping");

    auto& key = n.begin().key();
    REQUIRE(key.is_sequence());
    REQUIRE(key.size() == 3);
    REQUIRE(key.is_anchor());
    REQUIRE(key.get_anchor_name() == "key");

    REQUIRE(key.at(0).is_string());
    REQUIRE(key.at(0).is_anchor());
    REQUIRE(key.at(0).get_anchor_name() == "item");
    REQUIRE(key.at(0).as_str() == "a");

    REQUIRE(key.at(1).is_string());
    REQUIRE(key.at(1).as_str() == "b");

    REQUIRE(key.at(2).is_string());
    REQUIRE(key.at(2).as_str() == "c");

    auto& val = n.begin().value();
    REQUIRE(val.is_string());
    REQUIRE(val.as_str() == "value");
}

TEST_CASE("yaml_test_suite_6CA3") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6CA3/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6CA3/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.empty());
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6CA3/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.empty());
    }
}

TEST_CASE("yaml_test_suite_6CK3") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6CK3/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6CK3/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 3);

        REQUIRE(n.at(0).is_string());
        REQUIRE(n.at(0).get_tag_name() == "!local");
        REQUIRE(n.at(0).as_str() == "foo");

        REQUIRE(n.at(1).is_string());
        REQUIRE(n.at(1).get_tag_name() == "!!str");
        REQUIRE(n.at(1).as_str() == "bar");

        REQUIRE(n.at(2).is_string());
        REQUIRE(n.at(2).get_tag_name() == "!e!tag%21");
        REQUIRE(n.at(2).as_str() == "baz");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6CK3/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_sequence());
        REQUIRE(n.size() == 3);

        REQUIRE(n.at(0).is_string());
        REQUIRE(n.at(0).as_str() == "foo");

        REQUIRE(n.at(1).is_string());
        REQUIRE(n.at(1).as_str() == "bar");

        REQUIRE(n.at(2).is_string());
        REQUIRE(n.at(2).as_str() == "baz");
    }
}

TEST_CASE("yaml_test_suite_6FWR") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6FWR/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6FWR/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "ab\n\n \n");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6FWR/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "ab\n\n \n");
    }
}

TEST_CASE("yaml_test_suite_6H3V") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6H3V/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6H3V/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("foo: bar\\").is_string());
        REQUIRE(n.at("foo: bar\\").as_str() == "baz\'");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6H3V/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("foo: bar\\").is_string());
        REQUIRE(n.at("foo: bar\\").as_str() == "baz\'");
    }
}

TEST_CASE("yaml_test_suite_6HB6") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6HB6/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6HB6/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("Not indented").is_mapping());
        REQUIRE(n.at("Not indented").size() == 2);

        REQUIRE(n.at("Not indented").at("By one space").is_string());
        REQUIRE(
            n.at("Not indented").at("By one space").as_str() ==
            "By four\n  spaces\n");

        REQUIRE(n.at("Not indented").at("Flow style").is_sequence());
        REQUIRE(n.at("Not indented").at("Flow style").size() == 3);

        REQUIRE(n.at("Not indented").at("Flow style").at(0).is_string());
        REQUIRE(n.at("Not indented").at("Flow style").at(0).as_str() == "By two");

        REQUIRE(n.at("Not indented").at("Flow style").at(1).is_string());
        REQUIRE(
            n.at("Not indented").at("Flow style").at(1).as_str() == "Also by two");

        REQUIRE(n.at("Not indented").at("Flow style").at(2).is_string());
        REQUIRE(
            n.at("Not indented").at("Flow style").at(2).as_str() == "Still by two");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6HB6/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("Not indented").is_mapping());
        REQUIRE(n.at("Not indented").size() == 2);

        REQUIRE(n.at("Not indented").at("By one space").is_string());
        REQUIRE(
            n.at("Not indented").at("By one space").as_str() ==
            "By four\n  spaces\n");

        REQUIRE(n.at("Not indented").at("Flow style").is_sequence());
        REQUIRE(n.at("Not indented").at("Flow style").size() == 3);

        REQUIRE(n.at("Not indented").at("Flow style").at(0).is_string());
        REQUIRE(n.at("Not indented").at("Flow style").at(0).as_str() == "By two");

        REQUIRE(n.at("Not indented").at("Flow style").at(1).is_string());
        REQUIRE(
            n.at("Not indented").at("Flow style").at(1).as_str() == "Also by two");

        REQUIRE(n.at("Not indented").at("Flow style").at(2).is_string());
        REQUIRE(
            n.at("Not indented").at("Flow style").at(2).as_str() == "Still by two");
    }
}

TEST_CASE("yaml_test_suite_6JQW") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6JQW/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6JQW/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "\\//||\\/||\n// ||  ||__\n");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6JQW/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "\\//||\\/||\n// ||  ||__\n");
    }
}

TEST_CASE("yaml_test_suite_6JTT") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6JTT/===");
    fkyaml::node n;
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6JTT/in.yaml");
    REQUIRE(ifs);
    REQUIRE_THROWS_AS(ifs >> n, fkyaml::exception);
}

TEST_CASE("yaml_test_suite_6JWB") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6JWB/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6JWB/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("foo").is_sequence());
        REQUIRE(n.at("foo").get_tag_name() == "!!seq");
        REQUIRE(n.at("foo").size() == 2);

        REQUIRE(n.at("foo").at(0).is_string());
        REQUIRE(n.at("foo").at(0).get_tag_name() == "!!str");
        REQUIRE(n.at("foo").at(0).as_str() == "a");

        REQUIRE(n.at("foo").at(1).is_mapping());
        REQUIRE(n.at("foo").at(1).get_tag_name() == "!!map");
        REQUIRE(n.at("foo").at(1).size() == 1);

        REQUIRE(n.at("foo").at(1).at("key").is_string());
        REQUIRE(n.at("foo").at(1).at("key").get_tag_name() == "!!str");
        REQUIRE(n.at("foo").at(1).at("key").as_str() == "value");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6JWB/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 1);

        REQUIRE(n.at("foo").is_sequence());
        REQUIRE(n.at("foo").size() == 2);

        REQUIRE(n.at("foo").at(0).is_string());
        REQUIRE(n.at("foo").at(0).as_str() == "a");

        REQUIRE(n.at("foo").at(1).is_mapping());
        REQUIRE(n.at("foo").at(1).size() == 1);

        REQUIRE(n.at("foo").at(1).at("key").is_string());
        REQUIRE(n.at("foo").at(1).at("key").as_str() == "value");
    }
}

TEST_CASE("yaml_test_suite_6KGN") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6KGN/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6KGN/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at("a").is_null());
        REQUIRE(n.at("a").is_anchor());
        REQUIRE(n.at("a").get_anchor_name() == "anchor");

        REQUIRE(n.at("b").is_null());
        REQUIRE(n.at("b").is_alias());
        REQUIRE(n.at("b").get_anchor_name() == "anchor");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6KGN/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at("a").is_null());

        REQUIRE(n.at("b").is_null());
    }
}

TEST_CASE("yaml_test_suite_6LVF") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6LVF/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6LVF/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "foo");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6LVF/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == "foo");
    }
}

TEST_CASE("yaml_test_suite_6M2F") {
    SKIP("Empty keys are NOT supported.");
}

TEST_CASE("yaml_test_suite_6PBE") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6PBE/===");
    fkyaml::node n;
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6PBE/in.yaml");
    REQUIRE(ifs);
    REQUIRE_NOTHROW(ifs >> n);

    REQUIRE(n.is_mapping());
    REQUIRE(n.size() == 1);

    auto& key = n.begin().key();
    REQUIRE(key.is_sequence());
    REQUIRE(key.size() == 2);

    REQUIRE(key.at(0).is_string());
    REQUIRE(key.at(0).as_str() == "a");

    REQUIRE(key.at(1).is_string());
    REQUIRE(key.at(1).as_str() == "b");

    auto& val = n.begin().value();
    REQUIRE(val.is_sequence());
    REQUIRE(val.size() == 2);

    REQUIRE(val.at(0).is_string());
    REQUIRE(val.at(0).as_str() == "c");

    REQUIRE(val.at(1).is_string());
    REQUIRE(val.at(1).as_str() == "d");
}

TEST_CASE("yaml_test_suite_6S55") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6S55/===");
    fkyaml::node n;
    std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6S55/in.yaml");
    REQUIRE(ifs);
    REQUIRE_THROWS_AS(ifs >> n, fkyaml::exception);
}

TEST_CASE("yaml_test_suite_6SLA") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6SLA/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6SLA/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at("foo\nbar:baz\tx \\$%^&*()x").is_integer());
        REQUIRE(n.at("foo\nbar:baz\tx \\$%^&*()x").as_int() == 23);

        REQUIRE(n.at("x\\ny:z\\tx $%^&*()x").is_integer());
        REQUIRE(n.at("x\\ny:z\\tx $%^&*()x").as_int() == 24);
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6SLA/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 2);

        REQUIRE(n.at("foo\nbar:baz\tx \\$%^&*()x").is_integer());
        REQUIRE(n.at("foo\nbar:baz\tx \\$%^&*()x").as_int() == 23);

        REQUIRE(n.at("x\\ny:z\\tx $%^&*()x").is_integer());
        REQUIRE(n.at("x\\ny:z\\tx $%^&*()x").as_int() == 24);
    }
}

TEST_CASE("yaml_test_suite_6VJK") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6VJK/===");
    fkyaml::node n;
    std::string expected = "Sammy Sosa completed another fine season with great stats.\n"
                           "\n"
                           "  63 Home Runs\n"
                           "  0.288 Batting Average\n"
                           "\n"
                           "What a year!\n";

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6VJK/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == expected);
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6VJK/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == expected);
    }
}

TEST_CASE("yaml_test_suite_6WLZ") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6WLZ/===");

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6WLZ/in.yaml");
        REQUIRE(ifs);
        std::vector<fkyaml::node> docs;
        REQUIRE_NOTHROW(docs = fkyaml::node::deserialize_docs(ifs));

        REQUIRE(docs.size() == 2);

        REQUIRE(docs.at(0).is_string());
        REQUIRE(docs.at(0).get_tag_name() == "!foo");
        REQUIRE(docs.at(0).as_str() == "bar");

        REQUIRE(docs.at(1).is_string());
        REQUIRE(docs.at(1).get_tag_name() == "!foo");
        REQUIRE(docs.at(1).as_str() == "bar");
    }

    SECTION("JSON") {
        SKIP("Malformed JSON generated from multiple YAML documents.");
    }
}

TEST_CASE("yaml_test_suite_6WPF") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6WPF/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6WPF/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == " foo\nbar\nbaz ");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6WPF/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == " foo\nbar\nbaz ");
    }
}

TEST_CASE("yaml_test_suite_6XDY") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6XDY/===");

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6XDY/in.yaml");
        REQUIRE(ifs);
        std::vector<fkyaml::node> docs;
        REQUIRE_NOTHROW(docs = fkyaml::node::deserialize_docs(ifs));

        REQUIRE(docs.size() == 2);
        REQUIRE(docs.at(0).is_null());
        REQUIRE(docs.at(1).is_null());
    }

    SECTION("JSON") {
        SKIP("Malformed JSON generated from multiple YAML documents.");
    }
}

TEST_CASE("yaml_test_suite_6ZKB") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6ZKB/===");

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/6ZKB/in.yaml");
        REQUIRE(ifs);
        std::vector<fkyaml::node> docs;
        REQUIRE_NOTHROW(docs = fkyaml::node::deserialize_docs(ifs));

        REQUIRE(docs.size() == 3);

        REQUIRE(docs.at(0).is_string());
        REQUIRE(docs.at(0).as_str() == "Document");

        REQUIRE(docs.at(1).is_null());

        REQUIRE(docs.at(2).is_mapping());
        REQUIRE(docs.at(2).size() == 1);

        REQUIRE(docs.at(2).at("matches %").is_integer());
        REQUIRE(docs.at(2).at("matches %").as_int() == 20);
    }

    SECTION("JSON") {
        SKIP("Malformed JSON generated from multiple YAML documents.");
    }
}

TEST_CASE("yaml_test_suite_7A4E") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/7A4E/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/7A4E/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == " 1st non-empty\n2nd non-empty 3rd non-empty ");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/7A4E/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_string());
        REQUIRE(n.as_str() == " 1st non-empty\n2nd non-empty 3rd non-empty ");
    }
}

TEST_CASE("yaml_test_suite_7BMT") {
    print_label(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/7BMT/===");
    fkyaml::node n;

    SECTION("YAML") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/7BMT/in.yaml");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 7);

        REQUIRE(n.at("top1").is_mapping());
        REQUIRE(n.at("top1").is_anchor());
        REQUIRE(n.at("top1").get_anchor_name() == "node1");
        REQUIRE(n.at("top1").size() == 1);

        auto& top1_key = n.at("top1").begin().key();
        REQUIRE(top1_key.is_string());
        REQUIRE(top1_key.is_anchor());
        REQUIRE(top1_key.get_anchor_name() == "k1");
        REQUIRE(top1_key.as_str() == "key1");

        auto& top1_val = n.at("top1").begin().value();
        REQUIRE(top1_val.is_string());
        REQUIRE(top1_val.as_str() == "one");

        REQUIRE(n.at("top2").is_mapping());
        REQUIRE(n.at("top2").is_anchor());
        REQUIRE(n.at("top2").get_anchor_name() == "node2");
        REQUIRE(n.at("top2").size() == 1);

        auto& top2_key = n.at("top2").begin().key();
        REQUIRE(top2_key.is_string());
        REQUIRE(top2_key.as_str() == "key2");

        auto& top2_val = n.at("top2").begin().value();
        REQUIRE(top2_val.is_string());
        REQUIRE(top2_val.as_str() == "two");

        REQUIRE(n.at("top3").is_mapping());
        REQUIRE(n.at("top3").size() == 1);

        auto& top3_key = n.at("top3").begin().key();
        REQUIRE(top3_key.is_string());
        REQUIRE(top3_key.is_anchor());
        REQUIRE(top3_key.get_anchor_name() == "k3");
        REQUIRE(top3_key.as_str() == "key3");

        auto& top3_val = n.at("top3").begin().value();
        REQUIRE(top3_val.is_string());
        REQUIRE(top3_val.as_str() == "three");

        REQUIRE(n.at("top4").is_mapping());
        REQUIRE(n.at("top4").is_anchor());
        REQUIRE(n.at("top4").get_anchor_name() == "node4");
        REQUIRE(n.at("top4").size() == 1);

        auto& top4_key = n.at("top4").begin().key();
        REQUIRE(top4_key.is_string());
        REQUIRE(top4_key.is_anchor());
        REQUIRE(top4_key.get_anchor_name() == "k4");
        REQUIRE(top4_key.as_str() == "key4");

        auto& top4_val = n.at("top4").begin().value();
        REQUIRE(top4_val.is_string());
        REQUIRE(top4_val.as_str() == "four");

        REQUIRE(n.at("top5").is_mapping());
        REQUIRE(n.at("top5").is_anchor());
        REQUIRE(n.at("top5").get_anchor_name() == "node5");
        REQUIRE(n.at("top5").size() == 1);

        auto& top5_key = n.at("top5").begin().key();
        REQUIRE(top5_key.is_string());
        REQUIRE(top5_key.as_str() == "key5");

        auto& top5_val = n.at("top5").begin().value();
        REQUIRE(top5_val.is_string());
        REQUIRE(top5_val.as_str() == "five");

        REQUIRE(n.at("top6").is_string());
        REQUIRE(n.at("top6").is_anchor());
        REQUIRE(n.at("top6").get_anchor_name() == "val6");
        REQUIRE(n.at("top6").as_str() == "six");

        REQUIRE(n.at("top7").is_string());
        REQUIRE(n.at("top7").is_anchor());
        REQUIRE(n.at("top7").get_anchor_name() == "val7");
        REQUIRE(n.at("top7").as_str() == "seven");
    }

    SECTION("JSON") {
        std::ifstream ifs(FK_YAML_YAML_TEST_SUITE_ROOT_DIR "/7BMT/in.json");
        REQUIRE(ifs);
        REQUIRE_NOTHROW(ifs >> n);

        REQUIRE(n.is_mapping());
        REQUIRE(n.size() == 7);

        REQUIRE(n.at("top1").is_mapping());
        REQUIRE(n.at("top1").size() == 1);

        auto& top1_key = n.at("top1").begin().key();
        REQUIRE(top1_key.is_string());
        REQUIRE(top1_key.as_str() == "key1");

        auto& top1_val = n.at("top1").begin().value();
        REQUIRE(top1_val.is_string());
        REQUIRE(top1_val.as_str() == "one");

        REQUIRE(n.at("top2").is_mapping());
        REQUIRE(n.at("top2").size() == 1);

        auto& top2_key = n.at("top2").begin().key();
        REQUIRE(top2_key.is_string());
        REQUIRE(top2_key.as_str() == "key2");

        auto& top2_val = n.at("top2").begin().value();
        REQUIRE(top2_val.is_string());
        REQUIRE(top2_val.as_str() == "two");

        REQUIRE(n.at("top3").is_mapping());
        REQUIRE(n.at("top3").size() == 1);

        auto& top3_key = n.at("top3").begin().key();
        REQUIRE(top3_key.is_string());
        REQUIRE(top3_key.as_str() == "key3");

        auto& top3_val = n.at("top3").begin().value();
        REQUIRE(top3_val.is_string());
        REQUIRE(top3_val.as_str() == "three");

        REQUIRE(n.at("top4").is_mapping());
        REQUIRE(n.at("top4").size() == 1);

        auto& top4_key = n.at("top4").begin().key();
        REQUIRE(top4_key.is_string());
        REQUIRE(top4_key.as_str() == "key4");

        auto& top4_val = n.at("top4").begin().value();
        REQUIRE(top4_val.is_string());
        REQUIRE(top4_val.as_str() == "four");

        REQUIRE(n.at("top5").is_mapping());
        REQUIRE(n.at("top5").size() == 1);

        auto& top5_key = n.at("top5").begin().key();
        REQUIRE(top5_key.is_string());
        REQUIRE(top5_key.as_str() == "key5");

        auto& top5_val = n.at("top5").begin().value();
        REQUIRE(top5_val.is_string());
        REQUIRE(top5_val.as_str() == "five");

        REQUIRE(n.at("top6").is_string());
        REQUIRE(n.at("top6").as_str() == "six");

        REQUIRE(n.at("top7").is_string());
        REQUIRE(n.at("top7").as_str() == "seven");
    }
}
