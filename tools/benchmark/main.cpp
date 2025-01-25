//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#ifdef _MSC_VER
// suppress the C4996 warning against the usage of fopen().
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <cassert>
#include <cstring>
#include <string>

#include <benchmark/benchmark.h>

#include <fkYAML/node.hpp>
#include <yaml-cpp/yaml.h>

#ifdef FK_YAML_BM_HAS_LIBFYAML
#include <libfyaml.h>
#endif

#include <ryml.hpp>
#include <ryml_std.hpp>
#include <c4/yml/parse.hpp>

static std::string test_src {};

void prepare_test_source(char* filename) {
    FILE* fp = std::fopen(filename, "rb");
    char tmp_buf[256] {};
    std::size_t buf_size = sizeof(tmp_buf) / sizeof(char);
    std::size_t read_size = 0;
    while ((read_size = std::fread(&tmp_buf[0], sizeof(char), buf_size, fp)) > 0) {
        test_src.append(tmp_buf, tmp_buf + read_size);
    }
    std::fclose(fp);
    fp = nullptr;
}

int main(int argc, char** argv) {
    prepare_test_source(argv[1]);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    return 0;
}

//
// Benchmarking functions.
//

// fkYAML
void bm_fkyaml_parse(benchmark::State& st) {
    for (auto _ : st) {
        fkyaml::node n = fkyaml::node::deserialize(test_src);
    }

    st.SetItemsProcessed(st.iterations());
    st.SetBytesProcessed(st.iterations() * test_src.size());
}

// yaml-cpp
void bm_yamlcpp_parse(benchmark::State& st) {
    for (auto _ : st) {
        YAML::Node n = YAML::Load(test_src);
    }

    st.SetItemsProcessed(st.iterations());
    st.SetBytesProcessed(st.iterations() * test_src.size());
}

#ifdef FK_YAML_BM_HAS_LIBFYAML
// libfyaml
void bm_libfyaml_parse(benchmark::State& st) {
    const char* p_test_src = test_src.c_str();
    std::size_t test_src_size = std::distance(test_src.begin(), test_src.end());

    for (auto _ : st) {
        fy_document* p_fyd = fy_document_build_from_string(nullptr, p_test_src, test_src_size);
        (void)p_fyd;
    }

    st.SetItemsProcessed(st.iterations());
    st.SetBytesProcessed(st.iterations() * test_src.size());
}
#endif

// rapidyaml (in place)
void bm_rapidyaml_parse_inplace(benchmark::State& st) {
    std::string in_place_buff(test_src.size(), '\0');
    c4::substr c4_test_src = c4::to_substr(in_place_buff).trimr('\0');

    for (auto _ : st) {
        // ryml::parse_in_place() modifies the contents of `in_place_buff` during parsing.
        // Without the following copy, the second (and subsequent) parsing would fail.
        assert(in_place_buff.size() == test_src.size());
        std::memcpy(&in_place_buff[0], &test_src[0], in_place_buff.size());

        ryml::Tree tree = ryml::parse_in_place(c4_test_src);
    }

    st.SetItemsProcessed(st.iterations());
    st.SetBytesProcessed(st.iterations() * test_src.size());
}

// rapidyaml (arena)
void bm_rapidyaml_parse_arena(benchmark::State& st) {
    c4::csubstr src = c4::to_csubstr(test_src).trimr('\0');

    for (auto _ : st) {
        ryml::Tree tree = ryml::parse_in_arena(src);
    }

    st.SetItemsProcessed(st.iterations());
    st.SetBytesProcessed(st.iterations() * test_src.size());
}

// Register benchmarking functions.
BENCHMARK(bm_fkyaml_parse);
BENCHMARK(bm_yamlcpp_parse);

#ifdef FK_YAML_BM_HAS_LIBFYAML
BENCHMARK(bm_libfyaml_parse);
#endif

BENCHMARK(bm_rapidyaml_parse_inplace);
BENCHMARK(bm_rapidyaml_parse_arena);
