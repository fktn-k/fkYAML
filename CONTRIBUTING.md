# How to contribute

Thank you for visiting the fkYAML project. If you make any contribution for this project, it will surely be some help for us and other users in the future.  

To make it as easy as possible for you to contribute and for me to keep an overview, here are a few guidelines which should help us avoid all kinds of unnecessary work or disappointment. And of course, this document itself is subject to discussion, so please [create a discussion](https://github.com/fktn-k/fkYAML/discussions) or a pull request if you find a way to improve it!

## Private reports

Usually, all issues are tracked publicly on [GitHub](https://github.com/fktn-k/fkYAML/issues). If you want to make a private report (e.g., for a vulnerability or to attach an example that is not meant to be published), please send an email to <fktn.dev@gmail.com>.

## Prerequisites

Please create a [discussion](https://github.com/fktn-k/fkYAML/discussions) or [issue](https://github.com/fktn-k/fkYAML/issues/new/choose), assuming one does not already exist, and describe your concern.  
Note you need a [GitHub account](https://github.com/signup/free) for this.

## Describe your request/issue

Clearly describe the request/issue:

- If you propose a change or addition, try to give an **example** how the improved code could look like or how to use it.
- If it is a bug, please describe how to **reproduce** it. If possible, attach a complete example which demonstrates the error. Please also state what you **expected** to happen instead of the error.
- If you found a compilation error, please tell us which **compiler** (version and operating system) you used and paste the (relevant part of) the error messages to the ticket.

For questions, feature or support requests, please [open a discussion](https://github.com/fktn-k/fkYAML/discussions/new).  

## Contribution Steps

Basically, follow [the contribution guideline](https://docs.github.com/en/get-started/quickstart/contributing-to-projects) on the GitHub Docs.  
To make changes to the fkYAML project, you need to edit the following files:

### 1. [`include/fkYAML/**.hpp`](https://github.com/fktn-k/fkYAML/tree/develop/include/fkYAML)

These files are the sources of the fkYAML library. After making changes in those files, please run `make amalgamate` to regenerate [`single_include/fkYAML/node.hpp`](https://github.com/fktn-k/fkYAML/tree/develop/single_include/fkYAML/node.hpp) at least before making a PR. If the `make` command is unavailable on your local environment, the following commands will do the same:  

```bash
$ cd path/to/fkYAML
$ python3 ./tool/amalgamation/amalgamate.py -c ./tool/amalgamation/fkYAML.json -s . --verbose=yes
```

Note that the amalgamation tool assumes that your current directory is at the root of the fkYAML project.  

### 2. [`test/unit_test/*.cpp`](https://github.com/fktn-k/fkYAML/tree/develop/test/unit_test)

These files contain the [Catch2](https://github.com/catchorg/Catch2) unit tests from which current coverage data is generated. (Click [here](https://coveralls.io/github/fktn-k/fkYAML) to see the current coverage of the library's code.) If you have added or changed a feature, please also modify a unit test to the associated file(s) to keep covering 100% of the lines/branches in the fkYAML library. The unit tests can be compiled and executed with:  

```bash
$ cd path/to/fkYAML
$ cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DFK_YAML_BUILD_TEST=ON
$ cmake --build build --config Debug
$ ctest -C Debug --test-dir build --output-on-failure
```

Furthermore, you can test the single-header version of fkYAML by passing `-DFK_YAML_USE_SINGLE_HEADER=ON` when you configure CMake.  

### 3. [`docs/mkdocs/docs/**.md`](https://github.com/fktn-k/fkYAML/tree/develop/docs/mkdocs/docs)

These files are the sources of the documentation from which [MkDocs](https://www.mkdocs.org/) generates the documentation. If you have added or changed a feature, please also add sufficient descriptions so that the documentation is kept up-to-date.  

Also, you can build & check the updated documentation on your local environment by executing the following commands:

```bash
$ cd path/to/fkYAML
$ make -C docs/mkdocs serve
```

The commands above will automatically install all the dependencies using [Python venv](https://docs.python.org/3.10/library/venv.html) with the [`requirements.txt`](https://github.com/fktn-k/fkYAML/blob/develop/docs/mkdocs/requirements.txt) file. The generated documentation will be available by accessing http://127.0.0.1:8000/.

## Before Opening a PR

[GitHub Actions](https://github.com/fktn-k/fkYAML/actions) will test the updated project with the following Clang tools with the specific versions listed down below once you open a PR for your changes. So, it would be more efficient to check if your changes follow the predefined rules on your local environment in advance.  

| Clang Tool Name                                                                                   | Version |
| ------------------------------------------------------------------------------------------------- | ------- |
| [Clang-Format](https://releases.llvm.org/14.0.0/tools/clang/docs/ClangFormat.html)                | 14.0.0  |
| [Clang-Tidy](https://releases.llvm.org/14.0.0/tools/clang/tools/extra/docs/clang-tidy/index.html) | 14.0.0  |

Those Clang tools, however, do not seem to gurantee backward compatibility and their behaviours vary from version to version. So, it is highly recommended to use the specific versions listed above to avoid unnecessary confusion.  

## Note

When you open a pull request, fkYAML will automatically be built/tested with (1) various combinations of compilers and operating systems and (2) [Valgrind](https://valgrind.org/) and [Clang Sanitizers](https://clang.llvm.org/docs/index.html) to detect runtime issues (e.g., memory leaks), on [GitHub Actions](https://github.com/fktn-k/fkYAML/actions) once you open a pull request.  
These can result in failing builds and/or unit tests which run successfully on your local environment.  
As a policy of this project, however, all the workflow checks must be passed before merging.  

## Please don't

- The C++11 support varies between different **compilers** and versions. Please note the [list of supported compilers](https://github.com/fktn-k/fkYAML/blob/develop/README.md#supported-compilers). Some compilers like GCC 4.7 (and earlier), Clang 3.3 (and earlier), or Microsoft Visual Studio 13.0 and earlier are known not to work due to missing or incomplete C++11 support. Please refrain from proposing changes that work around these compiler's limitations with `#ifdef`s or other means.
- Please refrain from proposing changes that would **break [YAML](https://yaml.org/) specifications**. If you propose a conformant extension of YAML to be supported by the library, please motivate this extension.
- Please do not open pull requests that address **multiple issues**.

## Help Wanted

The following areas really need contribution:

- Extending the **continuous integration** to support more compilers, platforms, package managers, and/or operating systems, such as Android NDK, Intel's Compiler, or 32bit operating systems.
- Introducing **benchmarks** to measure the processing cost of this library. Though efficiency is not everything, speed and memory consumption are very important characteristics for C++ developers. So, having proper comparisons would be reasonable for both contributors and users.
