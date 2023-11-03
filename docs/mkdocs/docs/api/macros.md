# Macros

Some aspects of the fkYAML library can be configured by defining preprocessor macros **BEFORE** including the header files of the library.  
The following preprocessor macros are currently available.  

## Library Version

The fkYAML library defines the following preprocessor macros for the library version numbers according to [Semantic Versioning 2.0.0](https://semver.org/spec/v2.0.0.html).  
These macros are available for client applications as the metadata of this library.

| Name                  | Description                             |
| --------------------- | --------------------------------------- |
| FK_YAML_MAJOR_VERSION | the major version of the fkYAML library |
| FK_YAML_MINOR_VERSION | the minor version of the fkYAML library |
| FK_YAML_PATCH_VERSION | the patch version of the fkYAML library |

??? Example annotate "Example: print the library version"

    ```cpp
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        std::cout << "fkYAML version "
                  << FK_YAML_MAJOR_VERSION << "."
                  << FK_YAML_MINOR_VERSION << "."
                  << FK_YAML_PATCH_VERSION << std::endl
        return 0;
    }
    ```

    output:
    ```bash
    fkYAML version 0.1.3
    ```

## Library Namespaces

| Name                    | Description                                   |
| ----------------------- | --------------------------------------------- |
| FK_YAML_NAMESPACE_BEGIN | the beginning of the fkYAML library namespace |
| FK_YAML_NAMESPACE_END   | the end of the fkYAML library namespace       |

## Runtime Assertions

The fkYAML library calls [`assert()`](https://en.cppreference.com/w/cpp/error/assert) for runtime assertions by default.  
Thus, the runtime assertions can be disabled just by defining `NDEBUG`.  
You can also override the implementation by defining the following preprocessor macro.  

| Name              | Description                              |
| ----------------- | ---------------------------------------- |
| FK_YAML_ASSERT(x) | controls behavior of runtime assertions. |

??? Example annotate "Example: disable runtime assertions"

    ```cpp
    #define NDEBUG
    #include <fkYAML/node.hpp>

    ...
    ```

??? Example annotate "Example: override the implementation of runtime assertions"

    ```cpp
    #include <cstdio>
    #include <cstdlib>
    #define FK_YAML_ASSERT(x) \
        if(!(x)){std::fprintf(stderr, "assertion failed in %s", __FUNC__);std::abort();}
    #include <fkYAML/node.hpp>

    ...
    ```

## Language Supports

The fkYAML library targets C++11, but also supports some features introduced in later C++ standards.  
For those new features, the library implements some preprocessor checks to determine the C++ standard based on preprocessor macros such as `__cplusplus`, `_HAS_CXX14` or `_MSVC_LANG`.  
By defining any of the following symbols, the internal check is overridden and the provided C++ standard is unconditionally assumed.  
This can be helpful for compilers that only implement parts of the standard and would be detected incorrectly.  

| Name               | Description                               |
| ------------------ | ----------------------------------------- |
| FK_YAML_HAS_CXX_11 | supports C++11 features. (always enabled) |
| FK_YAML_HAS_CXX_14 | supports C++14 features.                  |
| FK_YAML_HAS_CXX_17 | supports C++17 features.                  |
| FK_YAML_HAS_CXX_20 | supports C++20 features.                  |

??? Example annotate "Example: force the fkYAML library to use a specific C++ standard"

    ```cpp
    // force the library to use the C++14 standard.
    #define FK_YAML_HAS_CXX_14 1
    #include <fkYAML/node.hpp>

    ...
    ```
