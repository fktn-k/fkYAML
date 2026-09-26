//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.5.0
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <fstream>

#include <doctest/doctest.h>

#include <fkYAML/node.hpp>
#include "test_data.hpp"

#ifdef _MSC_VER
#define DISABLE_C4996 __pragma(warning(push)) __pragma(warning(disable : 4996))
#define ENABLE_C4996 __pragma(warning(pop))
#else
#define DISABLE_C4996
#define ENABLE_C4996
#endif

TEST_CASE("StringWriter_Write") {
    std::string s;
    fkyaml::detail::string_writer writer(s);
    writer.write("Hello World", 5);
    REQUIRE(s == "Hello");
    writer.write("", 0);
    REQUIRE(s == "Hello");
    writer.write(" World", 6);
    REQUIRE(s == "Hello World");
}

TEST_CASE("FileWriter_Write") {
    DISABLE_C4996
    std::FILE* p_file = std::fopen(FK_YAML_TEST_OUTPUT_DIR "/test_output_file.txt", "w");
    ENABLE_C4996

    REQUIRE(p_file != nullptr);

    fkyaml::detail::file_writer writer(p_file);
    writer.write("Hello World", 5);
    writer.write("", 0);
    writer.write(" World", 6);
    std::fclose(p_file);

    std::ifstream ifs(FK_YAML_TEST_OUTPUT_DIR "/test_output_file.txt");
    REQUIRE(ifs.is_open());
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    CHECK(content == "Hello World");
}

TEST_CASE("OstreamWriter_Write") {
    std::ofstream ofs(FK_YAML_TEST_OUTPUT_DIR "/test_output_ostream.txt");
    REQUIRE(ofs.is_open());
    fkyaml::detail::ostream_writer writer(ofs);
    writer.write("Hello World", 5);
    writer.write("", 0);
    writer.write(" World", 6);
    ofs.close();

    std::ifstream ifs(FK_YAML_TEST_OUTPUT_DIR "/test_output_ostream.txt");
    REQUIRE(ifs.is_open());
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    CHECK(content == "Hello World");
}

TEST_CASE("OutputAdapter_Write") {
    std::string s;
    fkyaml::detail::string_writer string_writer(s);
    fkyaml::detail::output_adapter writer(string_writer);
    writer.write("Hello World", 5);
    writer.write(" World", 6);
    CHECK(s == "Hello World");
}
