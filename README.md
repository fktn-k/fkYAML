[![Ubuntu](https://github.com/fktn-k/fkYAML/workflows/Ubuntu/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3AUbuntu)
[![Windows](https://github.com/fktn-k/fkYAML/workflows/Windows/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3AWindows)
[![macOS](https://github.com/fktn-k/fkYAML/workflows/macOS/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3AmacOS)
[![Coverage Status](https://coveralls.io/repos/github/fktn-k/fkYAML/badge.svg?branch=develop)](https://coveralls.io/github/fktn-k/fkYAML?branch=develop)
[![CodeQL](https://github.com/fktn-k/fkYAML/workflows/CodeQL/badge.svg)](https://github.com/fktn-k/fkYAML/actions?query=workflow%3ACodeQL)
[![GitHub Releases](https://img.shields.io/github/release/fktn-k/fkYAML.svg)](https://github.com/fktn-k/fkYAML/releases/latest)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/fktn-k/fkYAML/develop/LICENSE.txt)
[![GitHub Issues](https://img.shields.io/github/issues/fktn-k/fkYAML.svg)](https://github.com/fktn-k/fkYAML/issues)

# fkYAML
fkYAML is a C++ header-only C++ library to deal with YAML.  
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
- [How to execute the unit tests](#how-to-execute-the-unit-tests)
- [Supported compilers](#supported-compilers)
- [License](#license)
- [Contact](#contact)
- [Used third-party tools](#used-third-party-tools)

## Design Goals
There are a lot of YAML library out there, and each may have its reason to exist.  
The fkYAML library has two design goals:  
- **Quick integration**.  
  The whole code depends only on C++ standards on any kind of platforms so that it can be quickly integrated into any projects written in C++11 or later.  
  No extenal dependencies, no sub-project, or no additional compiler flags are required.  
  By just adding the fkYAML libary to your own C++ projects, everything should work fine with no extra requirements.  
  If there should arise any issue, it's our fault. Please let us know by creating an issue or a PR to fix it up.

- **Seriously tested**.  
  The fkYAML library has been [unit-tested](https://github.com/fktn-k/fkYAML/tree/develop/test/unit_test) and covers as many lines and conditions of the code as possible. (You can see the actual coverage [here](https://coveralls.io/github/fktn-k/fkYAML?branch=develop).)  
  Furthermore, we checked with [Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/), [Clang Sanitizers](https://clang.llvm.org/docs/index.html) and [CodeQL](https://codeql.github.com/docs/) that there are no static/runtime issues in the fkYAML library.  
  CI workflows run against every commit where any .cpp or .hpp files get changed to check that the fkYAML library can be successfully built/tested on various platforms and compilers with C++ standards.  
  See the supported environments section for more details.

## How to Integrate with existing C++ projects

If your C++ project uses [CMake](https://cmake.org/) as the build system tool, there are several ways to integrate it with the fkYAML library:  

- **use `add_subdirectory()`**  
  ```cmake
  cmake_minimum_required(VERSION 3.8)
  project(YourProjectName)

  add_subdirectory(path/to/fkYAML)

  add_executable(your_app main.cpp)
  target_link_libraries(your_app PRIVATE fkYAML::fkYAML)
  ```

- **use the `FetchContent` module**  
  ```cmake
  cmake_minimum_required(VERSION 3.11)
  project(YourProjectName)

  include(FetchContent)
  FetchContent_Declare(
    fkYAML
    GIT_REPOSITORY https://github.com/fktn-k/fkYAML.git
    GIT_TAG        develop
  )
  FetchContent_MakeAvailable(fkYAML)

  add_executable(your_app main.cpp)
  target_link_library(your_app PRIVATE fkYAML::fkYAML)
  ```
- **use `find_package()`**  
  Make sure the fkYAML library has been installed to your local machine.  
  See the [How to install](#how-to-install) section for detail.  
  ```cmake
  cmake_minimum_required(VERSION 3.8)
  project(YourProjectName)

  find_package(fkYAML REQUIRED)

  add_executable(your_app main.cpp)
  target_link_library(your_app PRIVATE fkYAML::fkYAML)
  ```

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

### Deserialize YAML formatted strings

The `Deserializer` class provides an API for deserializing a YAML string into `Node` objects.  

```cpp
#include <cassert>
#include "fkYAML/Deserializer.hpp"
#include "fkYAML/Node.hpp"

// ...

fkyaml::Deserializer deserializer;
fkyaml::Node root = deserializer.Deserialize("foo: test\nbar: 3.14\nbuz: true");

// You can check that the `root` object has specific keys with the fkyaml::Node::Contains API.
assert(root.Contains("foo"));
assert(root.Contains("buz"));

// You can check types of YAML node values associated to each key like the followings:
assert(root["foo"].Type() == fkyaml::NodeType::STRING);
assert(root["foo"].IsStringScalar());
assert(root["bar"].Type() == fkyaml::NodeType::FLOAT_NUMBER);
assert(root["bar"].IsFloatNumber());
assert(root["buz"].Type() == fkyaml::NodeType::BOOLEAN);
assert(root["buz"].IsBooleanScalar());

// You can get references to YAML node values like the followings:
assert(root["foo"].ToString() == "test");
assert(root["foo"].ToFloatNumber() == 3.14);
assert(root["buz"].ToBoolean() == true);
```

### Serializing YAML node values

The `Serializer` class provides an API for serializing YAML node values into a string.  

```cpp
#include <cmath>
#include <string>
#include "fkYAML/Deserializer.hpp"
#include "fkYAML/Node.hpp"

// ...

fkyaml::Node root = fkyaml::Node::Mapping({
    { "foo", fkyaml::Node::StringScalar("test") },
    { "bar", fkyaml::Node::Sequence({
        fkyaml::Node::FloatNumberScalar(3.14),
        fkyaml::Node::FloatNumberScalar(std::nan(""))
    }) },
    { "buz", fkyaml::Node::BooleanScalar(true) }
});

fkyaml::Serializer serializer;

std::string str = serializer.Serialize(root);
// foo: test
// bar:
//   - 3.14
//   - .nan
// buz: true
```

### Build YAML nodes programatically

The `Node` class provides APIs for building YAML nodes programatically.  

```cpp
#include "fkYAML/Node.hpp"

// ...

// Create an empty YAML node (mapping).
fkyaml::Node root = fkyaml::Node::Mapping();

// Add a string scalar node.
root["foo"] = fkyaml::Node::StringScalar("test");

// Add a sequence node containing floating number scalar nodes.
root["bar"] = fkyaml::Node::Sequence({ 
    fkyaml::Node::FloatNumberScalar(3.14),
    fkyaml::Node::FloatNumberScalar(std::nan(""))
});

// Add a boolean node.
root["buz"] = fkyaml::Node::BooleanScalar(true);

// Instead, you can build YAML nodes all at once.
fkyaml::Node another_root = fkyaml::Node::Mapping({
    { "foo", fkyaml::Node::StringScalar("test") },
    { "bar", fkyaml::Node::Sequence({
        fkyaml::Node::FloatNumberScalar(3.14),
        fkyaml::Node::FloatNumberScalar(std::nan(""))
    }) },
    { "buz", fkyaml::Node::BooleanScalar(true) }
});
```

## How to execute the unit tests

You can execute the unit tests with the following commands.  
```bash
$ cd path/to/fkYAML
$ mkdir build && cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Debug -DFK_YAML_BUILD_TEST=ON
$ cmake --build . --config Debug
$ ctest -C Debug --output-on-failure
```

## Supported compilers
Currently, the following compilers are known to work and used in CI workflows:

| Compiler | Operating System | CI provider |
|----------|------------------|-------------|
| AppleClang 11.0.3.11030032 | macOS 11.7.9 | GitHub Actions |
| AppleClang 12.0.0.12000032 | macOS 11.7.9 | GitHub Actions |
| AppleClang 12.0.5.12050022 | macOS 11.7.9 | GitHub Actions |
| AppleClang 13.0.0.13000029 | macOS 11.7.9 | GitHub Actions |
| AppleClang 13.0.0.13000029 | macOS 12.6.8 | GitHub Actions |
| AppleClang 13.1.6.13160021 | macOS 12.6.8 | GitHub Actions |
| AppleClang 14.0.0.14000029 | macOS 12.6.8 | GitHub Actions |
| Clang 11.1.0 | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 12.0.1 | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 13.0.1 | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 14.0.0 | Ubuntu 22.04.3 LTS | GitHub Actions |
| Clang 15.0.7 | Ubuntu 22.04.3 LTS | GitHub Actions |
| GCC 9.5.0  | Ubuntu 22.04.3 LTS | GitHub Actions |
| GCC 10.5.0 | Ubuntu 22.04.3 LTS | GitHub Actions |
| GCC 11.4.0 | Ubuntu 22.04.3 LTS | GitHub Actions |
| GCC 12.3.0 | Ubuntu 22.04.3 LTS | GitHub Actions |
| MinGW-64 8.1.0 | Windows-10.0.17763 | GitHub Actions |
| MinGW-64 12.2.0 | Windows-10.0.20348 | GitHub Actions |
| Visual Studio 16 2019 MSVC 19.29.30152.0 | Windows-10.0.17763 | GitHub Actions |
| Visual Studio 17 2022 MSVC 19.35.32217.1 | Windows-10.0.20348 | GitHub Actions |

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

## Contact

If you have questions regarding the fkYAML library, feel free to [open an issue](https://github.com/fktn-k/fkYAML/issues) at GitHub.  
Please describe your request, problem, or question as detailed as possible, and also mention the version of the library you are using as well as the version of your compiler and operating system.  
Opening an issue at GitHub allows other users and contributors to this library to collabolate.  
If you want to make a private report (e.g., for a vulnerability or to attach an example that is not meant to be published), please send an email to <fktn.dev@gmail.com>.  

## Used third-party tools

The fkYAML library itself depends only on C++ standards and licensed under the MIT licence.  
However, it is built, tested and documented with a lot of third-party tools and services.  

- [**Clang**](https://clang.llvm.org/) for compilation, coding style checks, and/or static/runtime analysis.
- [**CMake**](https://cmake.org/) for automation of build & testing.
- [**Coveralls**](https://coveralls.io/) to measure [code coverage](https://coveralls.io/github/fktn-k/fkYAML?branch=develop).
- [**Catch2**](https://github.com/catchorg/Catch2) as a unit-test framework.
- [**Doxygen**](https://www.doxygen.nl/) as the API documentation generator.
- [**include-what-you-use**](https://github.com/include-what-you-use/include-what-you-use) to check the fkYAML library source files are each self-contained.
- [**lcov**](https://ltp.sourceforge.net/coverage/lcov.php) to process coverage information and create an HTML result view.
