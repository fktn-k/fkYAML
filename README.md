[![Ubuntu](https://github.com/fktn-k/fkYAML/workflows/Ubuntu/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3AUbuntu)
[![Windows](https://github.com/fktn-k/fkYAML/workflows/Windows/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3AWindows)
[![macOS](https://github.com/fktn-k/fkYAML/workflows/macOS/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3AmacOS)
[![Coverage Status](https://coveralls.io/repos/github/fktn-k/fkYAML/badge.svg?branch=develop)](https://coveralls.io/github/fktn-k/fkYAML?branch=develop)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/cded6969c7344ea5be60ab472e13000f)](https://app.codacy.com/gh/fktn-k/fkYAML/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![CodeQL](https://github.com/fktn-k/fkYAML/workflows/CodeQL/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3ACodeQL)
[![GitHub Releases](https://img.shields.io/github/release/fktn-k/fkYAML.svg)](https://github.com/fktn-k/fkYAML/releases/latest)
[![Documentation](https://img.shields.io/badge/docs-here-blue.svg)](https://fktn-k.github.io/fkYAML)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/fktn-k/fkYAML/develop/LICENSE.txt)
[![GitHub Issues](https://img.shields.io/github/issues/fktn-k/fkYAML.svg)](https://github.com/fktn-k/fkYAML/issues)
[![REUSE status](https://api.reuse.software/badge/github.com/fktn-k/fkYAML)](https://api.reuse.software/info/github.com/fktn-k/fkYAML)

# fkYAML
fkYAML is a C++ header-only YAML library.  
If you want portability & development speed-up, fkYAML is the way to go.  
No need to build a library only for a third party project.  
You can add YAML support into your projects by just including header files where it's needed.  

## Table of contents
- [Design goals](#design-goals)
- [Community Support](#community-support)
- [How to use fkYAML](#how-to-use-fkyaml)
- [How to test fkYAML](#how-to-test-fkYAML)
- [Supported compilers](#supported-compilers)
- [Benchmarking](#benchmarking)
- [License](#license)
- [Used third-party tools](#used-third-party-tools)

## Design Goals
There are a lot of YAML libraries out there, and each may have its reason to exist.  
fkYAML has been developed with these design goals:  

### :briefcase: **Portable**  
The library depends only on C++ standards, and is carefully designed to work on many platforms so that fkYAML can be imported into existing C++ projects written in C++11 or better.  
No external dependencies, no sub-project, or no additional compiler flags are required.  
Although fkYAML is a library with multiple header files by default, you can use the single-header version ([single_include/fkYAML/node.hpp](https://github.com/fktn-k/fkYAML/blob/develop/single_include/fkYAML/node.hpp)) located in the [single_include](https://github.com/fktn-k/tree/develop/single_include) directory. You can further use [single_include/fkYAML/fkyaml_fwd.hpp](https://github.com/fktn-k/fkYAML/blob/develop/single_include/fkYAML/fkyaml_fwd.hpp) for forward declarations.  
Furthermore, the project supports [CMake](https://cmake.org/) and provides [the documentation exclusively for CMake integration](https://fktn-k.github.io/fkYAML/tutorials/cmake_integration).  
Those characteristics allow existing C++ project using CMake for builds to quickly incorporate YAML support with just a little effort.  

### :memo: **Well Documented**  
[The documentation](https://fktn-k.github.io/fkYAML/) provides plenty of information so that users can understand what fkYAML is and what they can achieve with fkYAML.  
For instance, [the tutorial pages](https://fktn-k.github.io/fkYAML/tutorials/) show how you can read/write/customize fkYAML to handle YAML documents while creating a simple C++ project using CMake.  
More detailed, exclusive descriptions with example usages for each fkYAML API are available in [the API reference pages](https://fktn-k.github.io/fkYAML/api/basic_node/).  

### :mag: **Heavily Tested**  
fkYAML has been [unit-tested](https://github.com/fktn-k/fkYAML/tree/develop/test/unit_test) and its test suite covers 100% of lines and conditions of the codebase. (You can see the actual coverage [here](https://coveralls.io/github/fktn-k/fkYAML?branch=develop).)  
We check with [Valgrind](https://valgrind.org) and the [Clang Sanitizers](https://clang.llvm.org/docs/index.html) that there are no runtime issues such as memory leak.  
Furthermore, the quality of our codebase has been checked with [Clang-Tidy](https://releases.llvm.org/14.0.0/tools/clang/tools/extra/docs/clang-tidy/index.html) and [CodeQL](https://codeql.github.com/docs/).  
[GitHub Actions](https://docs.github.com/en/actions) workflows run against every commit pushed on the main & develop branches to ensure that the fkYAML library can be successfully built/tested with a variety of compilers and C++ standards.  
See the [supported compilers](#supported-compilers) section for more details.

## Community Support

### :question: **Questions**  
If you have questions regarding the fkYAML library, feel free to [open a Q&A discussion](https://github.com/fktn-k/fkYAML/discussions/new?category=q-a) at GitHub.  

### :zap: Feature Requests
If you have feature requests for the fkYAML library, let us know by [opening an Ideas discussion](https://github.com/fktn-k/fkYAML/discussions/new?category=ideas).  
It's really helpful for us if you describe what the new feature aims at and why it's needed.

### :bug: **Bug Reports**  
If you encounter a issue or find a bug, [open an issue](https://github.com/fktn-k/fkYAML/issues/new?assignees=&labels=kind%3A+bug&projects=&template=bug-report.yml).  
Please describe your problem as detailed as possible, and also mention the version of the library you are using as well as the versions of your compiler and operating system.  
Opening an issue at GitHub allows other users and contributors to this library to collabolate.  

### :closed_lock_with_key: **Private Reports**  
If you want to make a private report (e.g., for a vulnerability or to attach an example that is not meant to be published), please send an email to <fktn.dev@gmail.com>.  

## How to use fkYAML

[The documentation](https://fktn-k.github.io/fkYAML) is available on GitHub Pages.  
Here is the list you might want to know:  
* [Tutorial](https://fktn-k.github.io/fkYAML/tutorials) - getting started
* [CMake Integration](https://fktn-k.github.io/fkYAML/tutorials/cmake_integration) - CMake integration with your project
* [API references](https://fktn-k.github.io/fkYAML/api/basic_node) - all the details with examples

## How to test fkYAML

You can execute the unit tests with the following commands.  
Make sure the CMake executable has been registered to your PATH.
```bash
$ cd path/to/fkYAML
$ cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DFK_YAML_BUILD_TEST=ON
$ cmake --build build --config Debug
$ ctest -C Debug --test-dir build --output-on-failure
```

## Supported compilers
Currently, the following compilers are known to work and used in GitHub Actions workflows:

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
If you encounter a problem regarding compilers, please let us know by [creating an issue](https://github.com/fktn-k/fkYAML/issues/new?assignees=&labels=kind%3A+bug&projects=&template=bug-report.yml) or a PR with the information of your Operating System so that the same situation can be reproduced.  

## Benchmarking

Though efficiency is not everything, speed and memory consumption are very important characteristics for C++ developers. Regarding speed, benchmarking scores are now available with [the dedicated benchmarking tool](./tool/benchmark/README.md) for the parsing.  
The following tables are created from the benchmarking results in the following environment:
* CPU: AMD Ryzen 7 5800H @3.20GHz
* OS: Ubuntu22.04 (WSL2)
* Compiler: g++11.4.0

### Parsing [ubuntu.yml](https://github.com/fktn-k/fkYAML/blob/develop/tool/benchmark/cases/ubuntu.yml)

| Benchmark                          | processed bytes per second (Release) |
| ---------------------------------- | ------------------------------------ |
| fkYAML                             | 60.1886Mi/s                          |
| libfyaml                           | 34.9689Mi/s                          |
| rapidyaml<br>(with mutable buff)   | 19.4415Gi/s                          |
| rapidyaml<br>(with immutable buff) | 139.381Mi/s                          |
| yaml-cpp                           | 8.8139Mi/s                           |

### Parsing [citm_catalog.json](https://github.com/fktn-k/fkYAML/blob/develop/tool/benchmark/cases/citm_catalog.json)

| Benchmark                          | processed bytes per second (Release) |
| ---------------------------------- | ------------------------------------ |
| fkYAML                             | 91.1523Mi/s                          |
| libfyaml                           | 51.8156Mi/s                          |
| rapidyaml<br>(with mutable buff)   | 29.7284Gi/s                          |
| rapidyaml<br>(with immutable buff) | 140.764Mi/s                          |
| yaml-cpp                           | 14.8521Mi/s                          |

### Parsing [citm_catalog.yml](https://github.com/fktn-k/fkYAML/blob/develop/tool/benchmark/cases/citm_catalog.yml)

| Benchmark                          | processed bytes per second (Release) |
| ---------------------------------- | ------------------------------------ |
| fkYAML                             | 36.0492Mi/s                          |
| libfyaml                           | 21.547Mi/s                           |
| rapidyaml<br>(with mutable buff)   | 22.6904Gi/s                          |
| rapidyaml<br>(with immutable buff) | 64.3972Mi/s                          |
| yaml-cpp                           | 6.2337Mi/s                           |

Although [rapidyaml](https://github.com/biojppm/rapidyaml) is about 2x faster with immutable buffers and far faster with mutable buffers than fkYAML as it focuses on high performance, fkYAML is in general 70% faster than [libfyaml](https://github.com/pantoniou/libfyaml) and also about 6x faster than [yaml-cpp](https://github.com/jbeder/yaml-cpp).  
Note that, since fkYAML deserializes scalars into native booleans or integers during the parsing, the performance could be more faster in some use cases since there is no need for string manipulations upon data queries.  

## License

This project is distributed under the [MIT License](https://opensource.org/license/mit/):  

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

## How to contribute

See the [`CONTRIBUTING.md`](https://github.com/fktn-k/fkYAML/blob/develop/CONTRIBUTING.md) file for detailed information.  

## Used third-party tools

The library itself depends only on C++ standards and licensed under the MIT licence.  
However, it is built, tested or documented with a lot of third-party tools and services.  
Thanks a lot!  

- [**amalgamate.py - Amalgamate C source and header files**](https://github.com/edlund/amalgamate) to generate a single header file.
- [**Clang**](https://clang.llvm.org/) for compilation, coding style checks, and static/runtime analysis.
- [**CMake**](https://cmake.org/) for automation of build & testing.
- [**Codacy**](https://www.codacy.com/) for further [code analysis](https://app.codacy.com/gh/fktn-k/fkYAML/).
- [**Coveralls**](https://coveralls.io/) to measure [code coverage](https://coveralls.io/github/fktn-k/fkYAML?branch=develop).
- [**Catch2**](https://github.com/catchorg/Catch2) as a unit-test framework.
- [**Google Benchmark**](https://github.com/google/benchmark) as a benchmarking framework.
- [**github-changelog-generator**](https://github.com/github-changelog-generator/github-changelog-generator) to generate the [CHANGELOG.md](https://github.com/fktn-k/fkYAML/tree/develop/CHANGELOG.md) file.
- [**include-what-you-use**](https://github.com/include-what-you-use/include-what-you-use) to check the fkYAML library source files are each self-contained.
- [**lcov**](https://github.com/linux-test-project/lcov) to process coverage information and generate an HTML view.
- [**Material for MkDocs**](https://squidfunk.github.io/mkdocs-material/) for the style of the documentation site.
- [**MkDocs**](https://www.mkdocs.org/) as the documentation site generator.
- [**reuse-tool**](https://github.com/fsfe/reuse-tool) to generate license/copyright headers in source files to meet [REUSE software](https://reuse.software/) recommendations.
- [**Valgrind**](https://valgrind.org/) for runtime memory leak check.
