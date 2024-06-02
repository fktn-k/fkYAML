# Benchmark

This tool runs benchmarking with [the Google Benchmark library](https://github.com/google/benchmark/) ([v1.8.4](https://github.com/google/benchmark/releases/tag/v1.8.4)) against fkYAML and the following C++ YAML libraries tagged with the specified versions respectively:  
* [libfyaml](https://github.com/pantoniou/libfyaml) ([v0.9](https://github.com/pantoniou/libfyaml/releases/tag/v0.9))
* [rapidyaml](https://github.com/biojppm/rapidyaml) ([v0.6.0](https://github.com/biojppm/rapidyaml/releases/tag/v0.6.0))
* [yaml-cpp](https://github.com/jbeder/yaml-cpp) ([0.8.0](https://github.com/jbeder/yaml-cpp/releases/tag/0.8.0))

Currently, the [macos.yml](https://github.com/fktn-k/fkYAML/blob/develop/tool/benchmark/macos.yml) file is used for the benchmarking which was copied from the [.github/workflows](https://github.com/fktn-k/fkYAML/tree/develop/.github/workflows) directory at the point of the commit hash [e1c740f0c93766cfdd0b704ffb46113acd6bd922](https://github.com/fktn-k/fkYAML/commit/e1c740f0c93766cfdd0b704ffb46113acd6bd922).  
More YAML files are planned to be added for the benchmarking so the resulting score would be more accurate and reliable.  

## How to Use

Build and run this tool with the following commands:

```bash
$ cd path/to/fkYAML
$ cmake -S . -B build -DCMAKE_BUILD_TYPE={Debug|Release} -DFK_YAML_RUN_BENCHMARK=ON
$ cmake --build build --config {Debug|Release}
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
