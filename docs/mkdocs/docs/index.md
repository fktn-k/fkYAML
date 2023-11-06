# fkYAML
fkYAML is a C++ header-only YAML library.  
If you want portability & development speed-up, fkYAML is the way to go.  
No need to build a library only for a third party project.  
You can add YAML support into your projects by just including header files where it's needed.  

## Design Goals
There are a lot of YAML libraries out there, and each may have its reason to exist.  
fkYAML has been developed with these design goals:  

### :briefcase: **Portable**  
:   The whole code depends only on C++ standards, and is carefully designed to work on many platforms so that fkYAML can be imported into existing C++ projects written in C++11 or later.  
    No external dependencies, no sub-project, or no additional compiler flags are required.  
    Furthermore, the project supports [CMake](https://cmake.org/) and provides [the documentation exclusively for CMake integration](https://fktn-k.github.io/fkYAML/tutorials/cmake_integration).  
    Those characteristics allow existing C++ project using CMake for builds to quickly incorporate YAML support with just a little effort.  

### :memo: **Well Documented**  
:   [The documentation](https://fktn-k.github.io/fkYAML/) provides plenty of information so that users can understand what fkYAML is and what they can achieve with fkYAML.  
    For instance, [the tutorial pages](https://fktn-k.github.io/fkYAML/tutorials/) show how you can read/write/customize fkYAML to handle YAML documents while creating a simple C++ project using CMake.  
    More detailed, exclusive descriptions with example usages for each fkYAML API are available in [the API reference pages](api/basic_node/index.md).  

### :mag: **Heavily Tested**  
:   The fkYAML library has been [unit-tested](https://github.com/fktn-k/fkYAML/tree/develop/test/unit_test) and covers 100% of lines and conditions of the codebase. (You can see the actual coverage [here](https://coveralls.io/github/fktn-k/fkYAML?branch=develop).)  
    We check with [Valgrind](https://valgrind.org) and the [Clang Sanitizers](https://clang.llvm.org/docs/index.html) that there are no runtime issues such as memory leak.  
    Furthermore, the quality of our codebase has been checked with [Clang-Tidy](https://clang.llvm.org/extra/clang-tidy/) and [CodeQL](https://codeql.github.com/docs/).  
    [GitHub Actions](https://docs.github.com/en/actions) workflows run against every commit pushed on the main & develop branches to ensure that the fkYAML library can be successfully built/tested with a variety of compilers and C++ standards.  
    See the [supported compilers](home/supported_compilers.md) section for more details.
