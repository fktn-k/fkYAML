# How to contribute

Thank you for visiting the fkYAML project. If you make any contribution for this project, it will surely be some help for other users in the future.  

To make it as easy as possible for you to contribute and for me to keep an overview, here are a few guidelines which should help us avoid all kinds of unnecessary work or disappointment. And of course, this document is subject to discussion, so please [create an issue](https://github.com/fktn-k/fkYAML/issues/new/choose) or a pull request if you find a way to improve it!

## Private reports

Usually, all issues are tracked publicly on [GitHub](https://github.com/fktn-k/fkYAML/issues). If you want to make a private report (e.g., for a vulnerability or to attach an example that is not meant to be published), please send an email to <fktn.dev@gmail.com>.

## Prerequisites

Please [create an issue](https://github.com/fktn-k/fkYAML/issues/new/choose), assuming one does not already exist, and describe your concern. Note you need a [GitHub account](https://github.com/signup/free) for this.

## Describe your issue

Clearly describe the issue:

- If it is a bug, please describe how to **reproduce** it. If possible, attach a complete example which demonstrates the error. Please also state what you **expected** to happen instead of the error.
- If you propose a change or addition, try to give an **example** how the improved code could look like or how to use it.
- If you found a compilation error, please tell us which **compiler** (version and operating system) you used and paste the (relevant part of) the error messages to the ticket.

For questions, feature or support requests, please [open a discussion](https://github.com/fktn-k/fkYAML/discussions/new).

## Files to change

To make changes, you need to edit the following files:

1. [`include/fkYAML/*`](https://github.com/fktn-k/fkYAML/tree/develop/include/fkYAML) - These files are the sources of the fkYAML library.

2. [`test/unit_test/*.cpp`](https://github.com/fktn-k/fkYAML/tree/develop/test/unit_test) - These files contain the [Catch2](https://github.com/catchorg/Catch2) unit tests from which current coverage data is generated. (Click [here](https://coveralls.io/github/fktn-k/fkYAML) to see current coverage of the library's code.)

   If you add or change a feature, please also add a unit test to the associated file(s). The unit tests can be compiled and executed with

   ```sh
   $ mkdir build
   $ cd build
   $ cmake .. -DFK_YAML_BUILD_TEST=ON
   $ cmake --build .
   $ ctest
   ```

   The test cases are also executed with several different compilers on [GitHub Actions](https://github.com/fktn-k/fkYAML/actions) once you push a commit or open a pull request.

## Note

If you open a pull request, the code will be automatically tested with [Clang Sanitizers](https://clang.llvm.org/docs/index.html) to detect runtime issues (e.g., memory leaks). This can result in failing unit tests which run successfully without Clang Sanitizers.

## Please don't

- The C++11 support varies between different **compilers** and versions. Please note the [list of supported compilers](https://github.com/fktn-k/fkYAML/blob/develop/README.md#supported-compilers). Some compilers like GCC 4.7 (and earlier), Clang 3.3 (and earlier), or Microsoft Visual Studio 13.0 and earlier are known not to work due to missing or incomplete C++11 support. Please refrain from proposing changes that work around these compiler's limitations with `#ifdef`s or other means.
- Please refrain from proposing changes that would **break [YAML](https://yaml.org/) specifications**. If you propose a conformant extension of YAML to be supported by the library, please motivate this extension.
- Please do not open pull requests that address **multiple issues**.

## Wanted

The following areas really need contribution:

- Extending the **continuous integration** to support more compilers, platforms, package managers, and/or operating systems, such as Android NDK, Intel's Compiler, or 32bit OSs.
- Introducing **benchmarks** to measure the processing cost of this library. Though efficiency is not everything, speed and memory consumption are very important characteristics for C++ developers, so having proper comparisons would be reasonable for both contributors and users.
