# Benchmark

This tool runs benchmarking of deserialization performance with [the Google Benchmark library](https://github.com/google/benchmark/) (tag: [v1.8.4](https://github.com/google/benchmark/releases/tag/v1.8.4), the latest version supporting C++11) against fkYAML and some C++ YAML library.  

## Used YAML library for Comparison

Currently, the following C++ YAML libraries tagged with the specified versions respectively:  
* [libfyaml](https://github.com/pantoniou/libfyaml) ([v0.9](https://github.com/pantoniou/libfyaml/releases/tag/v0.9))
* [rapidyaml](https://github.com/biojppm/rapidyaml) ([v0.7.2](https://github.com/biojppm/rapidyaml/releases/tag/v0.7.2))
* [yaml-cpp](https://github.com/jbeder/yaml-cpp) ([0.8.0](https://github.com/jbeder/yaml-cpp/releases/tag/0.8.0))

Currently, the following files are located in the [tool/benchmark/cases](https://github.com/fktn-k/fkYAML/tree/develop/tool/benchmark/cases) directory for benchmarking:  

* ubuntu.yml
  * a copy from the [.github/workflows](https://github.com/fktn-k/fkYAML/tree/develop/.github/workflows) directory at the point of the commit [7092059](https://github.com/fktn-k/fkYAML/commit/709205948966a8415b76aa8a87371133a04fd675)
  * for the benchmarking of an ordinary size/content benchmark
* citm_catalog.json
  * a copy from the [miloyip/nativejson-benchmark](https://github.com/miloyip/nativejson-benchmark) repository (tag: [v1.0.0](https://github.com/miloyip/nativejson-benchmark/releases/tag/v1.0.0))
  * for the benchmarking of a large JSON file
* citm_catalog.yml
  * a YAML version of citm_catalog.json
  * for the benchmarking of a large YAML file

Suggestions are always welcome. Please open a discussion or create a PR.

## How to Use

Build and run this tool with the following commands:

```bash
$ cd path/to/fkYAML
$ cmake -S . -B build -DCMAKE_BUILD_TYPE={Debug|Release} -DFK_YAML_RUN_BENCHMARK=ON
$ cmake --build build --config {Debug|Release}

# You can specify an arbitrary input file such as /foo/bar/sample.yml or /foo/bar/sample.json
$ ./build/tool/benchmark/benchmarker ./tool/benchmark/macos.yml
```

Then, you should see a console ouput from the Google Benchmark library in the following format.  

```bash
-------------------------------------------------------------------------------------
Benchmark                           Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------------
bm_fkyaml_parse                 xxxxx ns        xxxxx ns        xxxxx bytes_per_second=xx.xxxxMi/s items_per_second=xx.xxxxk/s
...
```

Visit [the user guide](https://github.com/google/benchmark/blob/v1.8.4/docs/user_guide.md) in the Google Benchmark repository for more information on the output format.  
