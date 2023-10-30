[![Ubuntu](https://github.com/fktn-k/fkYAML/workflows/Ubuntu/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3AUbuntu)
[![Windows](https://github.com/fktn-k/fkYAML/workflows/Windows/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3AWindows)
[![macOS](https://github.com/fktn-k/fkYAML/workflows/macOS/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3AmacOS)
[![Coverage Status](https://coveralls.io/repos/github/fktn-k/fkYAML/badge.svg?branch=develop)](https://coveralls.io/github/fktn-k/fkYAML?branch=develop)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/cded6969c7344ea5be60ab472e13000f)](https://app.codacy.com/gh/fktn-k/fkYAML/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![CodeQL](https://github.com/fktn-k/fkYAML/workflows/CodeQL/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3ACodeQL)
[![GitHub Releases](https://img.shields.io/github/release/fktn-k/fkYAML.svg)](https://github.com/fktn-k/fkYAML/releases/latest)
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
- [How to integrate with existing C++ projects](#how-to-integrate-with-existing-c-projects)
- [How to install](#how-to-install)
- [Example usages](#example-usages)
  - [Deserialize YAML formatted strings](#deserialize-yaml-formatted-strings)
  - [Serialize YAML node values](#serializing-yaml-node-values)
  - [Build YAML nodes programatically](#build-yaml-nodes-programatically)
  - [Customize serialization/deserialization](#customize-serializationdeserialization)
- [How to execute the unit tests](#how-to-execute-the-unit-tests)
- [Supported compilers](#supported-compilers)
- [License](#license)
- [Contact](#contact)
- [Used third-party tools](#used-third-party-tools)

## Design Goals
There are a lot of YAML library out there, and each may have its reason to exist.  
The fkYAML library has 3 design goals:  

#### :briefcase: **Portable**.  
The whole code depends only on C++ standards, and is designed to work on any kind of platforms so that it can be imported into any C++ projects written in C++11 or later.  
No external dependencies, no sub-project, or no additional compiler flags are required.  
Besides that, the project supports [CMake](https://cmake.org/), which is widely used among various kinds of C++ projects as the de-facto standard software build system.  
We believe those features will surely be useful for a lot of C++ developers.  

#### :hammer: **Quickly integrated**.  
By just copy-and-paste or use from CMake the fkYAML libary to your own C++ projects, everything should work fine with no other extra requirements than a compiler which supports C++.  
Just follow the [How to integrate with existing C++ projects](#how-to-integrate-with-existing-c-projects) section, and you will be able to add YAML support to your own project.  
If there should arise any issue, it might be our fault. Please let us know by creating an issue or a PR to fix it up.

#### :memo: **Heavily tested**.  
The fkYAML library has been [unit-tested](https://github.com/fktn-k/fkYAML/tree/develop/test/unit_test) and covers 100% of lines and conditions of the codebase. (You can see the actual coverage [here](https://coveralls.io/github/fktn-k/fkYAML?branch=develop).)  
We check with [Valgrind](https://valgrind.org) and the [Clang Sanitizers](https://clang.llvm.org/docs/index.html) that there are no runtime issues such as memory leak.  
Furthermore, the quality of our codebase has been checked with [Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/) and [CodeQL](https://codeql.github.com/docs/).  
[GitHub Actions](https://docs.github.com/en/actions) workflows run against every commit pushed on the main & develop branches to ensure that the fkYAML library can be successfully built/tested with a variety of compilers and C++ standards.  
See the [supported compilers](#supported-compilers) section for more details.

## Support

#### :question: **Questions**  
If you have questions regarding the fkYAML library, feel free to [open a Q&A discussion](https://github.com/fktn-k/fkYAML/discussions/new?category=q-a) at GitHub.  

#### :zap: Feature Requests
If you have feature requests for the fkYAML library, let us know by [opening an Ideas discussion](https://github.com/fktn-k/fkYAML/discussions/new?category=ideas).  
It's really helpful for us if you describe what the new feature aims at and/or why it's needed.

#### :bug: **Bug Reports**  
If you encounter a issue or find a bug, [open an issue](https://github.com/fktn-k/fkYAML/issues/new?assignees=&labels=kind%3A+bug&projects=&template=bug-report.yml).  
Please describe your request, problem, or question as detailed as possible, and also mention the version of the library you are using as well as the version of your compiler and operating system.  
Opening an issue at GitHub allows other users and contributors to this library to collabolate.  

#### :closed_lock_with_key: Private Reports
 If you want to make a private report (e.g., for a vulnerability or to attach an example that is not meant to be published), please send an email to <fktn.dev@gmail.com>.  

## How to Integrate with existing C++ projects

As briefly described above, the fkYAML library can be quickly introduced into you own project.  
You can use it by just copy and past the fkYAML source files to your projects.  
Also, if your C++ project uses [CMake](https://cmake.org/) as the build system, there are several ways to integrate it with the fkYAML library:  

- **use `add_subdirectory()`**  
  <details>
  <summary>See an example code snippet.</summary><div>

  ```cmake
  cmake_minimum_required(VERSION 3.8)
  project(YourProjectName)

  add_subdirectory(path/to/fkYAML)

  add_executable(your_app main.cpp)
  target_link_libraries(your_app PRIVATE fkYAML::fkYAML)
  ```

  </div>

- **use the `FetchContent` CMake module**  
  <details>
  <summary>See an example code snippet.</summary><div>

  ```cmake
  cmake_minimum_required(VERSION 3.11)
  project(YourProjectName)

  include(FetchContent)
  FetchContent_Declare(
    fkYAML
    GIT_REPOSITORY https://github.com/fktn-k/fkYAML.git
    GIT_TAG v0.1.3
  )
  FetchContent_MakeAvailable(fkYAML)

  add_executable(your_app main.cpp)
  target_link_library(your_app PRIVATE fkYAML::fkYAML)
  ```

  </div></details>

- **use `find_package()`**  
  Make sure the fkYAML library has been installed to your local machine.  
  See the [How to install](#how-to-install) section for detail.  

  <details>
  <summary>See an example code snippet.</summary><div>

  ```cmake
  cmake_minimum_required(VERSION 3.8)
  project(YourProjectName)

  find_package(fkYAML REQUIRED)

  add_executable(your_app main.cpp)
  target_link_library(your_app PRIVATE fkYAML::fkYAML)
  ```

  </div></details>

## How to install
The fkYAML library can be installed to your local machine by the following commands:  

```bash
$ cd path/to/fkYAML
$ mkdir build && cd build
$ cmake .. [-DCMAKE_INSTALL_PREFIX="path/to/install/dir"]
$ cmake --build . --target install
```

The above commands will install the fkYAML library with the default installation path prefix as CMake describes [here](https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html).  
You can customize the installation path prefix with `-DCMAKE_INSTALL_PREFIX"` at CMake configuration.  

## Example usages

Here are some examples to give you an idea how to use the fkYAML library.  
For more information, click [here](https://fktn-k.github.io/fkYAML/) to see the API documentation.

### Deserialize YAML formatted strings

The `Deserializer` class provides an API for deserializing a YAML string into `node` objects.  

<details>
<summary>See an example code snippet.</summary><div>

```cpp
#include <cassert>
#include <fkYAML/node.hpp>

// ...

fkyaml::node root = fkyaml::node::deserialize("foo: test\nbar: 3.14\nbaz: true");

// You can check that the `root` object has specific keys with the fkyaml::Node::Contains API.
assert(root.contains("foo"));
assert(root.contains("bar"));
assert(root.contains("baz"));

// You can check types of YAML node values associated to each key like the followings:
assert(root["foo"].is_string());
assert(root["bar"].is_float_number());
assert(root["baz"].is_boolean());

// You can get references to YAML node values like the followings:
assert(root["foo"].get_value_ref<std::string&>() == "test");
assert(root["bar"].get_value_ref<double&>() == 3.14);
assert(root["baz"].get_value_ref<bool&>() == true);

// You can get values of YAML node like the followings:
assert(root["foo"].get_value<std::string>() == "test");
assert(root["bar"].get_value<double>() == 3.14);
assert(root["baz"].get_value<bool>() == true);
```
</div></details>

### Serializing YAML node values

The `Serializer` class provides an API for serializing YAML node values into a string.  

<details>
<summary>See an example code snippet.</summary><div>

```cpp
#include <cmath>
#include <string>
#include <fkYAML/node.hpp>

// ...

fkyaml::node root = {
    { std::string("foo"), std::string("test") },
    { std::string("bar"), { 3.14, std::nan("") } },
    { std::string("baz"), true }
};

std::string str = fkyaml::node::serialize(root);
// foo: test
// bar:
//   - 3.14
//   - .nan
// baz: true
```

</div></details>

### Build YAML nodes programatically

The `node` class provides APIs for building YAML nodes programatically.  

<details>
<summary>See an example code snippet.</summary><div>

```cpp
#include <fkYAML/node.hpp>

// ...

// Create an empty YAML node (mapping).
fkyaml::node root = fkyaml::node::mapping();

// Add a string scalar node.
root["foo"] = std::string("test");

// Add a sequence node containing floating number scalar nodes.
root["bar"] = { 3.14, std::nan("") };

// Add a boolean node.
root["baz"] = true;

// Instead, you can build YAML nodes all at once.
fkyaml::node another_root = {
    { std::string("foo"), std::string("test") },
    { std::string("bar"), { 3.14, std::nan("") } },
    { std::string("baz"), true }
};
```

</div></details>

### Customize serialization/deserialization

To make your own custom types convertible from/to `fkyaml::node` type, you can implement your own `to_node()` & `from_node()` **outside** of the fkyaml namespace.
(Those functions will be called when you use `fkyaml::node::get_value\<CustomType\>` to get a CustomType object out of a `fkyaml::node` object.)

<details>
<summary>See an example code snippet.</summary><div>

```cpp
#include <fkYAML/node_value_converter.hpp>

namespace ns
{

struct CustomType
{
    std::string foo;
    bool bar;
    double baz;
};

// overload to_node() with the CustomType type.
void to_node(fkyaml::node& n, const CustomType& c)
{
    n = fkyaml::node::mapping({
        { "foo", fkyaml::node(c.foo) },
        { "bar", fkyaml::node(c.bar) },
        { "baz", fkyaml::node(c.baz) }
    });
}

// overload from_node() with the CustomType type.
void from_node(const fkyaml::node& n, CustomType& c)
{
    c.foo = n.get_value_ref<const std::string&>();
    c.bar = n.get_value<bool>();
    c.baz = n.get_value<double>();
}

} // namespace ns

// ...

CustomType c { "yaml", true, 3.14 };
// convert from CustomType to fkyaml::node.
fkyaml::node n(c);
// convert from fkyaml::node to CustomType.
CustomType another_c = n.get_value<CustomType>();
```
To achieve customization like the above, your CustomType **MUST** be default constructible.  
Furthermore, to make your own implementations of `to_node()` & `from_node()` called with the mechanism of ADL, they **MUST** be declared/implemented **OUTSIDE** of the fkyaml namespace.

## How to execute the unit tests

You can execute the unit tests with the following commands.  
```bash
$ cd path/to/fkYAML
$ mkdir build && cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Debug -DFK_YAML_BUILD_TEST=ON
$ cmake --build . --config Debug
$ ctest -C Debug --output-on-failure
```

</div></details>

## Supported compilers
Currently, the following compilers are known to work and used in CI workflows:

<details>
<summary>See the supported OS/compiler table.</summary><div>

| Compiler | Operating System | CI provider |
|----------|------------------|-------------|
| AppleClang 11.0.3.11030032 | macOS 11.7.9 | GitHub Actions |
| AppleClang 12.0.0.12000032 | macOS 11.7.9 | GitHub Actions |
| AppleClang 12.0.5.12050022 | macOS 11.7.9 | GitHub Actions |
| AppleClang 13.0.0.13000029 | macOS 11.7.9 | GitHub Actions |
| AppleClang 13.0.0.13000029 | macOS 12.6.8 | GitHub Actions |
| AppleClang 13.1.6.13160021 | macOS 12.6.8 | GitHub Actions |
| AppleClang 14.0.0.14000029 | macOS 12.6.8 | GitHub Actions |
| Clang 3.5.2  | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 3.6.2  | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 3.7.1  | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 3.8.1  | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 3.9.1  | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 4.0.1  | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 5.0.2  | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 6.0.1  | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 7.1.0  | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 9.0.1  | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 10.0.1 | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 11.1.0 | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 12.0.1 | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 13.0.1 | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 14.0.6 | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 15.0.7 | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 16.0.6 | Ubuntu 22.04.3 LTS | GitHub Actions |
| GCC 9.5.0  | Ubuntu 22.04.3 LTS | GitHub Actions |
| GCC 10.5.0 | Ubuntu 22.04.3 LTS | GitHub Actions |
| GCC 11.4.0 | Ubuntu 22.04.3 LTS | GitHub Actions |
| GCC 12.3.0 | Ubuntu 22.04.3 LTS | GitHub Actions |
| GCC 13.2.0 | Ubuntu 22.04.3 LTS | GitHub Actions |
| MinGW-64 8.1.0 | Windows-10.0.17763 | GitHub Actions |
| MinGW-64 12.2.0 | Windows-10.0.20348 | GitHub Actions |
| Visual Studio 16 2019 MSVC 19.29.30152.0 | Windows-10.0.17763 | GitHub Actions |
| Visual Studio 17 2022 MSVC 19.35.32217.1 | Windows-10.0.20348 | GitHub Actions |

</div></details>

Requests for new compiler supports are welcome.  
If you encounter a problem regarding compilers, please let us know by creating an issue or a PR with the information of your Operating System so that we can reproduce the same situation.  

## License

This project is distributed under the [MIT License](https://opensource.org/license/mit/):  

Copyright (c) 2023 fktn  

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

## Used third-party tools

The fkYAML library itself depends only on C++ standards and licensed under the MIT licence.  
However, it is built, tested and documented with a lot of third-party tools and services.  
Thanks a lot!  

- [**Clang**](https://clang.llvm.org/) for compilation, coding style checks, and/or static/runtime analysis.
- [**CMake**](https://cmake.org/) for automation of build & testing.
- [**cmake-format**](https://github.com/cheshirekow/cmake_format) as a linter for CMake scripts.
- [**Coveralls**](https://coveralls.io/) to measure [code coverage](https://coveralls.io/github/fktn-k/fkYAML?branch=develop).
- [**Catch2**](https://github.com/catchorg/Catch2) as a unit-test framework.
- [**Doxygen**](https://www.doxygen.nl/) as the API documentation generator.
- [**github-changelog-generator**](https://github.com/github-changelog-generator/github-changelog-generator) to generate the [CHANGELOG.md](https://github.com/fktn-k/fkYAML/tree/develop/CHANGELOG.md) file.
- [**include-what-you-use**](https://github.com/include-what-you-use/include-what-you-use) to check the fkYAML library source files are each self-contained.
- [**lcov**](https://ltp.sourceforge.net/coverage/lcov.php) to generate coverage data.
- [**reuse-tool**](https://github.com/fsfe/reuse-tool) to generate license/copyright headers in source files to meet [REUSE software](https://reuse.software/) recommendations.
- [**Valgrind**](https://valgrind.org/) for runtime memory leak check.
