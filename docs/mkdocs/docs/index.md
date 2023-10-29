# fkYAML
fkYAML is a C++ header-only YAML library.  
If you want portability & development speed-up, fkYAML is the way to go.  
No need to build a library only for a third party project.  
You can add YAML support into your projects by just including header files where it's needed.  

## Design Goals
There are a lot of YAML library out there, and each may have its reason to exist.  
The fkYAML library has 3 design goals:  

#### :briefcase: **Portable**  
:   The whole code depends only on C++ standards, and is designed to work on any kind of platforms so that it can be imported into any C++ projects written in C++11 or later.  
    No external dependencies, no sub-project, or no additional compiler flags are required.  
    Besides that, the project supports [CMake](https://cmake.org/), which is widely used among various kinds of C++ projects as the de-facto standard software build system.  
    We believe those features will surely be useful for a lot of C++ developers.  

#### :hammer: **Quickly Integrated**  
:   By just copy-and-paste or use from CMake the fkYAML libary to your own C++ projects, everything should work fine with no other extra requirements than a compiler which supports C++.  
    Just follow the [How to integrate with existing C++ projects](#how-to-integrate-with-existing-c-projects) section, and you will be able to add YAML support to your own project.  
    If there should arise any issue, it might be our fault. Please let us know by creating an issue or a PR to fix it up.

#### :memo: **Heavily Tested**  
:   The fkYAML library has been [unit-tested](https://github.com/fktn-k/fkYAML/tree/develop/test/unit_test) and covers 100% of lines and conditions of the codebase. (You can see the actual coverage [here](https://coveralls.io/github/fktn-k/fkYAML?branch=develop).)  
    We check with [Valgrind](https://valgrind.org) and the [Clang Sanitizers](https://clang.llvm.org/docs/index.html) that there are no runtime issues such as memory leak.  
    Furthermore, the quality of our codebase has been checked with [Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/) and [CodeQL](https://codeql.github.com/docs/).  
    [GitHub Actions](https://docs.github.com/en/actions) workflows run against every commit pushed on the main & develop branches to ensure that the fkYAML library can be successfully built/tested with a variety of compilers and C++ standards.  
    See the [supported compilers](#supported-compilers) section for more details.