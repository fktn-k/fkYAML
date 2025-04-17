[![Ubuntu](https://github.com/fktn-k/fkYAML/workflows/Ubuntu/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3AUbuntu)
[![Windows](https://github.com/fktn-k/fkYAML/workflows/Windows/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3AWindows)
[![macOS](https://github.com/fktn-k/fkYAML/workflows/macOS/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3AmacOS)
[![Coverage Status](https://coveralls.io/repos/github/fktn-k/fkYAML/badge.svg?branch=develop)](https://coveralls.io/github/fktn-k/fkYAML?branch=develop)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/cded6969c7344ea5be60ab472e13000f)](https://app.codacy.com/gh/fktn-k/fkYAML/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![CodeQL](https://github.com/fktn-k/fkYAML/workflows/CodeQL/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3ACodeQL)
[![GitHub Releases](https://img.shields.io/github/release/fktn-k/fkYAML.svg)](https://github.com/fktn-k/fkYAML/releases/latest)
[![GitHub Downloads](https://img.shields.io/github/downloads/fktn-k/fkYAML/total)](https://github.com/fktn-k/fkYAML/releases)
[![Documentation](https://img.shields.io/badge/docs-mkdocs-blue.svg)](https://fktn-k.github.io/fkYAML)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/fktn-k/fkYAML/develop/LICENSE.txt)
[![GitHub Issues](https://img.shields.io/github/issues/fktn-k/fkYAML.svg)](https://github.com/fktn-k/fkYAML/issues)
[![REUSE status](https://api.reuse.software/badge/github.com/fktn-k/fkYAML)](https://api.reuse.software/info/github.com/fktn-k/fkYAML)

# fkYAML
fkYAML is a C++ header-only library to deserialize, serialize and build YAML documents.  
It is also carefully designed and tested to work with various compilers, C++ standards and platforms.  
So, if you want portability & development speed-up, fkYAML is the way to go.  
You can add YAML support into your projects by just including the header file(s).  
This simple example (1) deserializes a YAML string into a document, (2) modifies the documentation, and finally (3) serializes the modified documentation to a YAML string.  
```cpp
#include <iostream>
#include <string>
#include <fkYAML/node.hpp>

int main() {
    // 1. Deserialize a YAML string into a document.
    //    The input can be other container types or their iterators.
    //    `deserialize` accepts `FILE*`, `std::istream` as well.
    std::string yaml = R"(
project: fkYAML
required C++ version: 11
works on:
  - Linux
  - macOS
)";
    auto node = fkyaml::node::deserialize(yaml);

    // 2. Modify it.
    //    as_seq() returns a `std::vector<fkyaml::node>&` object.
    node["maintainer"] = "fktn-k";
    node.at("works on").as_seq().emplace_back("Windows");

    // 3. Serialize the modified document to a YAML string and save it.
    std::ofstream ofs("out.yaml");
    ofs << node;
  
    // Contents of out.yaml
    //
    // maintainer: fktn-k
    // project: fkYAML
    // required C++ version: 11
    // works on:
    //   - Linux
    //   - macOS
    //   - Windows
}
```

## Table of contents
* [Design goals](#design-goals)
* [Features](#features)
* [Test suite status](#test-suite-status)
* [Supported compilers](#supported-compilers)
* [Benchmarking](#benchmarking)
* [Community Support](#community-support)
* [Testing](#testing)
* [Contributing](#contributing)
* [Used third party tools](#used-third-party-tools)
* [License](#license)

## Design Goals
There are a lot of YAML libraries written in C/C++ out there, and each may have its reasons to exist.  
fkYAML has been developed with these design goals:  

### :briefcase: **Portable**  
The library depends only on C++ standards, and is carefully designed to work on many platforms so that fkYAML can be imported into existing C++ projects written in C++11 or better.  
No external dependencies, no sub-projects, and no additional compiler flags are required.  
Although fkYAML is a library with multiple header files by default, you can use [the single-header version](https://github.com/fktn-k/fkYAML/blob/develop/single_include/fkYAML/node.hpp). You can further use [fkyaml_fwd.hpp](https://github.com/fktn-k/fkYAML/blob/develop/single_include/fkYAML/fkyaml_fwd.hpp) for forward declarations.  
Furthermore, the project supports [CMake](https://cmake.org/) and provides [the documentation exclusively for CMake integration](https://fktn-k.github.io/fkYAML/tutorials/cmake_integration).  
Those characteristics allow existing C++ project using CMake for builds to quickly incorporate YAML support with just a little effort.  

### :memo: **Well Documented**  
[The documentation](https://fktn-k.github.io/fkYAML/) provides plenty of information so that users can understand what fkYAML is and what they can achieve with fkYAML.  
For instance, [the tutorial pages](https://fktn-k.github.io/fkYAML/tutorials/) show how you can read/write/customize using fkYAML to handle YAML documents while creating a simple C++ project using CMake.  
Also, [API reference pages](https://fktn-k.github.io/fkYAML/api/) provide more detailed, exclusive descriptions with example usages for each fkYAML API.  

### :mag: **Heavily Tested**  
fkYAML has been [unit-tested](https://github.com/fktn-k/fkYAML/tree/develop/test/unit_test) and its test suite covers 100% of lines and conditions of the codebase. (You can see the actual coverage [here](https://coveralls.io/github/fktn-k/fkYAML?branch=develop).)  
We check with [Valgrind](https://valgrind.org) and the [Clang Sanitizers](https://clang.llvm.org/docs/index.html) that there are no runtime issues such as memory leak.  
Furthermore, the quality of our codebase has been checked with [Clang-Tidy](https://releases.llvm.org/14.0.0/tools/clang/tools/extra/docs/clang-tidy/index.html), [CodeQL](https://codeql.github.com/docs/) and [Codacy](https://www.codacy.com/).  
[GitHub Actions](https://docs.github.com/en/actions) workflows run against every commit pushed on the main & develop branches to ensure that the fkYAML library can be successfully built/tested with a variety of compilers, operating systems and C++ standards.  
See the [supported compilers](#supported-compilers) section for more details.

## Features

* General
    * Cross-platforms
        * Compilers: GCC, Clang, Visual Studio, etc.
        * Operating systems: Linux, macOS, Windows.
    * Easy integration
        * Header-only library. Just copy the headers to your project. CMake packages are also available in [the release page](https://github.com/fktn-k/fkYAML/releases/latest).
    * Self-contained, minimal dependencies
        * The C++ standard is the only dependency. No external dependencies, sub-projects or additional compiler flags are required.
* YAML Standard Compliance (with [the YAML specification v1.2.2](https://yaml.org/spec/1.2.2/))
    * YAML syntax
        * Support scalars, sequences, mappings in both block and flow styles.
            * Sequences and mappings are accepted as mapping keys in de/serialization. Such a key can also be passed to query a mapping value like [`operator[]`](https://fktn-k.github.io/fkYAML/api/basic_node/operator[]/) or [`at`](https://fktn-k.github.io/fkYAML/api/basic_node/at/).
            * Node values can be referenced by using [`as_seq`](https://fktn-k.github.io/fkYAML/api/basic_node/as_seq/) or [`as_map`](https://fktn-k.github.io/fkYAML/api/basic_node/as_map/), or converted to arbitrary types by [`get_value`](https://fktn-k.github.io/fkYAML/api/basic_node/get_value/) or [`get_value_inplace`](https://fktn-k.github.io/fkYAML/api/basic_node/get_value_inplace/).
            * Empty mapping keys like `: value` or `- : value` are NOT supported.
        * Support `%TAG` and `%YAML` directives
            * `%YAML` directives have no effect and YAML 1.2 is always assumed.
            * Any unknown directives are just ignored during deserialization.
        * Support tags, anchors and aliases
        * Support de/serializing a single document by [`deserialize`](https://fktn-k.github.io/fkYAML/api/basic_node/deserialize/) and [`serialize`](https://fktn-k.github.io/fkYAML/api/basic_node/serialize/).
        * Support de/serializing multiple documents in input stream as well by [`deserialize_docs`](https://fktn-k.github.io/fkYAML/api/basic_node/deserialize_docs/) and [`serialize_docs`](https://fktn-k.github.io/fkYAML/api/basic_node/deserialize_docs/).
    * Encodings
        * Support UTF-8, UTF-16 and UTF-32 encodings, including both little and big endians.
            * Character types are `char` for all the above encodings, `char8_t`(since C++20) for UTF-8, `char16_t` for UTF-16 and `char32_t` for UTF-32.
        * Support automatic detection of encodings in input stream.
            * The first character should be a BOM or an ASCII character as described in [the specification](https://yaml.org/spec/1.2.2/#52-character-encodings).
            * Otherwise, the UTF-8 encoding is assumed.
        * Support encoding validation internally.
        * A Byte Order Mark (BOM) at the beginning of input stream is automatically consumed during deserialization. Any BOMs at the other places are treated as contents.
    * Line break formats
        * Support the Unix style `\n` and the Windows style `\r\n` for deserialization.
        * Serialization uses the Unix style `\n`. No configuration is available for now.
* Customization Points
    * Node value types
        * Scalars, sequences and mappings are all stored in [`fkyaml::node`](https://fktn-k.github.io/fkYAML/api/basic_node/node/) objects as `bool` for booleans, `int64_t` for integers, `double` for floating point values, `std::string` for strings, `std::vector<fkyaml::basic_node>` for sequences and `std::map<fkyaml::basic_node, fkyaml::basic_node>` for mappings.  
        The types to use for storage are customizable to your needs by changing template parameter types of [`fkyaml::basic_node`](https://fktn-k.github.io/fkYAML/api/basic_node/).
    * Arbitrary type conversions
        * fkYAML has a number of built-in conversion functions from/to [`fkyaml::basic_node`](https://fktn-k.github.io/fkYAML/api/basic_node/) for a lot of STL container/scalar types.  
        You can implement your own [`from_node`](https://fktn-k.github.io/fkYAML/api/node_value_converter/from_node/) and [`to_node`](https://fktn-k.github.io/fkYAML/api/node_value_converter/to_node/) functions for types which are defined by your project or third party projects.  
        See [the tutorial page](https://fktn-k.github.io/fkYAML/tutorials/) for details.
* Miscellaneous
    * [fkyaml_fwd.hpp](https://github.com/fktn-k/fkYAML/blob/develop/single_include/fkYAML/fkyaml_fwd.hpp) is available for forward declarations of fkYAML APIs so unnecessary definitions will not slow down compilation.
    * Some features introduced in C++14 and better are available depending on the active C++ standard at compile time, for examples:
        * [Structured binding](https://en.cppreference.com/w/cpp/language/structured_binding) support for mappings.
        * [std::string_view](https://en.cppreference.com/w/cpp/string/basic_string_view) for input stream.
        * [std::optional](https://en.cppreference.com/w/cpp/utility/optional) for conversions from node values.

More detailed descriptions are to be found on [GitHub Pages](https://fktn-k.github.io/fkYAML).  
Here is the list you might want to know:  
* [Tutorial](https://fktn-k.github.io/fkYAML/tutorials) - getting started
* [CMake Integration](https://fktn-k.github.io/fkYAML/tutorials/cmake_integration) - CMake integration with your project
* [API references](https://fktn-k.github.io/fkYAML/api/basic_node) - all the details with examples

## Supported compilers
Compilers with complete C++11 support should compile the library without warnings.  
Actually, fkYAML is compiled and tested with 40+ different C++ compilers with different operating systems and C++ standards in GitHub Actions workflows.

| Compiler                   | Operating System                                                                                               |
| -------------------------- | -------------------------------------------------------------------------------------------------------------- |
| AppleClang 14.0.0.14000029 | [macOS 13](https://github.com/actions/runner-images/blob/main/images/macos/macos-13-Readme.md)                 |
| AppleClang 14.0.3.14030022 | [macOS 13](https://github.com/actions/runner-images/blob/main/images/macos/macos-13-Readme.md)                 |
| AppleClang 15.0.0.15000040 | [macOS 13](https://github.com/actions/runner-images/blob/main/images/macos/macos-13-Readme.md)                 |
| AppleClang 15.0.0.15000040 | [macOS 14](https://github.com/actions/runner-images/blob/main/images/macos/macos-14-Readme.md)                 |
| AppleClang 15.0.0.15000100 | [macOS 13](https://github.com/actions/runner-images/blob/main/images/macos/macos-13-Readme.md)                 |
| AppleClang 15.0.0.15000100 | [macOS 14](https://github.com/actions/runner-images/blob/main/images/macos/macos-14-Readme.md)                 |
| AppleClang 15.0.0.15000309 | [macOS 14](https://github.com/actions/runner-images/blob/main/images/macos/macos-14-Readme.md)                 |
| AppleClang 16.0.0.16000026 | [macOS 15](https://github.com/actions/runner-images/blob/main/images/macos/macos-15-Readme.md)                 |
| Clang 3.4.2                | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 3.5.2                | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 3.6.2                | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 3.7.1                | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 3.8.1                | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 3.9.1                | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 4.0.1                | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 5.0.2                | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 6.0.1                | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 7.1.0                | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 9.0.1                | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 10.0.1               | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 11.1.0               | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 12.0.1               | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 13.0.1               | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 14.0.6               | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 15.0.7               | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 16.0.6               | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 17.0.6               | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 18.1.6               | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| Clang 19.1.4               | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| GCC 4.8.5                  | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| GCC 5.3.1                  | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| GCC 6.4.0                  | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| GCC 7.5.0                  | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| GCC 8.5.0                  | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| GCC 9.5.0                  | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| GCC 10.5.0                 | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| GCC 11.4.0                 | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| GCC 12.3.0                 | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| GCC 13.3.0                 | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| GCC 14.2.0                 | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| IntelLLVM 2024.1.2         | [Ubuntu 22.04](https://github.com/actions/runner-images/blob/main/images/ubuntu/Ubuntu2204-Readme.md)          |
| MinGW-64 8.1.0             | [Windows Server 2019](https://github.com/actions/runner-images/blob/main/images/windows/Windows2019-Readme.md) |
| MinGW-64 12.2.0            | [Windows Server 2022](https://github.com/actions/runner-images/blob/main/images/windows/Windows2022-Readme.md) |
| Visual Studio 16 2019      | [Windows Server 2019](https://github.com/actions/runner-images/blob/main/images/windows/Windows2019-Readme.md) |
| Visual Studio 17 2022      | [Windows Server 2022](https://github.com/actions/runner-images/blob/main/images/windows/Windows2022-Readme.md) |

Requests for new compiler supports are welcome.  
If you encounter a problem regarding compilers, please let us know by [creating an issue](https://github.com/fktn-k/fkYAML/issues/new?assignees=&labels=kind%3A+bug&projects=&template=bug-report.yml) or a PR with the information of your Operating System so that the same issue can be reproduced.  

## Test suite status

Testing of the library with [the YAML test suite](https://github.com/yaml/yaml-test-suite) is currently a work in progress.

## Benchmarking

Though efficiency is not everything, speed and memory consumption are very important characteristics for C++ developers. Regarding speed, benchmarking scores are now available with [the dedicated benchmarking tool](./tool/benchmark/README.md) for the parsing.  
The following tables are created from the benchmarking results in the following environment:  

* CPU: AMD Ryzen 7 5800H @3.20GHz
* OS: Ubuntu22.04 (WSL2)
* Compiler: g++11.4.0

### Parsing [ubuntu.yml](https://github.com/fktn-k/fkYAML/blob/develop/tool/benchmark/cases/ubuntu.yml)

| Benchmark                          | processed bytes per second (Release) |
| ---------------------------------- | ------------------------------------ |
| fkYAML                             | 62.5049Mi/s                          |
| libfyaml                           | 39.235Mi/s                           |
| rapidyaml<br>(with mutable buff)   | 22.007Gi/s                           |
| rapidyaml<br>(with immutable buff) | 133.311Mi/s                          |
| yaml-cpp                           | 9.07876Mi/s                          |

### Parsing [citm_catalog.json](https://github.com/fktn-k/fkYAML/blob/develop/tool/benchmark/cases/citm_catalog.json)

| Benchmark                          | processed bytes per second (Release) |
| ---------------------------------- | ------------------------------------ |
| fkYAML                             | 97.216Mi/s                           |
| libfyaml                           | 57.3021Mi/s                          |
| rapidyaml<br>(with mutable buff)   | 37.9026Gi/s                          |
| rapidyaml<br>(with immutable buff) | 140.375Mi/s                          |
| yaml-cpp                           | 14.3192Mi/s                          |

### Parsing [citm_catalog.yml](https://github.com/fktn-k/fkYAML/blob/develop/tool/benchmark/cases/citm_catalog.yml)

| Benchmark                          | processed bytes per second (Release) |
| ---------------------------------- | ------------------------------------ |
| fkYAML                             | 38.7563Mi/s                          |
| libfyaml                           | 24.7526Mi/s                          |
| rapidyaml<br>(with mutable buff)   | 37.9676Gi/s                          |
| rapidyaml<br>(with immutable buff) | 68.4245Mi/s                          |
| yaml-cpp                           | 6.47003Mi/s                          |

Although [rapidyaml](https://github.com/biojppm/rapidyaml) is about 2x faster with immutable buffers and far faster with mutable buffers than fkYAML as it focuses on high performance, fkYAML is in general 70% faster than [libfyaml](https://github.com/pantoniou/libfyaml) and also about 6.5x faster than [yaml-cpp](https://github.com/jbeder/yaml-cpp).  
Note that, since fkYAML deserializes scalars into native booleans or integers during the parsing, the performance could be more faster in some use cases since there is no need for string manipulations upon data queries.  

## Community Support

### :question: **Questions**  
If you have questions regarding the fkYAML library, feel free to [open a Q&A discussion](https://github.com/fktn-k/fkYAML/discussions/new?category=q-a) at GitHub.  

### :zap: **Feature Requests**
If you find a way to improve fkYAML, let us know by [opening an Ideas discussion](https://github.com/fktn-k/fkYAML/discussions/new?category=ideas).  
It's really helpful for us if you describe what the new feature aims at and why it's needed.

### :bug: **Bug Reports**  
If you encounter an issue or find a bug, [open an issue](https://github.com/fktn-k/fkYAML/issues/new?assignees=&labels=kind%3A+bug&projects=&template=bug-report.yml).  
Please describe your problem as detailed as possible, and also mention the version of the library you are using as well as the versions of your compiler and operating system.  
Opening an issue at GitHub allows other users and contributors of this library to collabolate and helps us find a better way to resolve it.  

### :closed_lock_with_key: **Private Reports**  
If you want to make a private report (e.g., for a vulnerability or to attach an example that is not meant to be published), please send an email to <fktn.dev@gmail.com>.  

## Testing

You can execute the unit tests with the following commands.  
Make sure a CMake executable path has been added to your PATH in advance.
```bash
$ cd path/to/fkYAML
$ cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DFK_YAML_BUILD_TEST=ON
$ cmake --build build
$ ctest -C Debug --test-dir build --output-on-failure
```

## Contributing

See the [`CONTRIBUTING.md`](https://github.com/fktn-k/fkYAML/blob/develop/CONTRIBUTING.md) file for detailed information.  

## Used third party tools

The library itself depends only on C++ standards and licensed under the MIT licence. However, it is built, tested or documented with a lot of third-party tools and services. Thanks a lot!  

* [**amalgamate.py - Amalgamate C source and header files**](https://github.com/edlund/amalgamate) to generate a single header file.
* [**Catch2**](https://github.com/catchorg/Catch2) as a unit-test framework.
* [**Clang**](https://clang.llvm.org/) for compilation, coding style checks, and static/runtime analysis.
* [**CMake**](https://cmake.org/) for automation of build & testing.
* [**Codacy**](https://www.codacy.com/) for further [code analysis](https://app.codacy.com/gh/fktn-k/fkYAML/).
* [**Coveralls**](https://coveralls.io/) to measure [code coverage](https://coveralls.io/github/fktn-k/fkYAML?branch=develop).
* [**Google Benchmark**](https://github.com/google/benchmark) as a benchmarking framework.
* [**github-changelog-generator**](https://github.com/github-changelog-generator/github-changelog-generator) to generate the [CHANGELOG.md](https://github.com/fktn-k/fkYAML/tree/develop/CHANGELOG.md) file.
* [**include-what-you-use**](https://github.com/include-what-you-use/include-what-you-use) to check the fkYAML library source files are each self-contained.
* [**lcov**](https://github.com/linux-test-project/lcov) to process coverage information and generate an HTML view.
* [**Material for MkDocs**](https://squidfunk.github.io/mkdocs-material/) for the style of the documentation site.
* [**MkDocs**](https://www.mkdocs.org/) as the documentation site generator.
* [**reuse-tool**](https://github.com/fsfe/reuse-tool) to generate license/copyright headers in source files to meet [REUSE software](https://reuse.software/) recommendations.
* [**Valgrind**](https://valgrind.org/) for runtime memory leak check.

## License

fkYAML is distributed under the [MIT License](https://opensource.org/license/mit/):  

```
Copyright (c) 2023-2025 Kensuke Fukutani

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
