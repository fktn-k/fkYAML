# CMake Integration

Since we use [CMake](https://cmake.org/) to build fkYAML, we also provide a couple of integration points for our users.  
You can use the `fkYAML::fkYAML` interface target in CMake.  
This target populates the appropriate usage requirements for [`INTERFACE_INCLUDE_DIRECTORIES`](https://cmake.org/cmake/help/latest/prop_tgt/INTERFACE_INCLUDE_DIRECTORIES.html) to point to the appropriate include directories and [`INTERFACE_COMPILE_FEATURES`](https://cmake.org/cmake/help/latest/prop_tgt/INTERFACE_COMPILE_FEATURES.html) for the necessary C++11 flags.  

## Possible solutions

The following sub-sections show some possible ways in which you can fkYAML from an existing CMake project.  

### With `find_package()`

To use fkYAML from a CMake project, you can locate it directly with [`find_package()`](https://cmake.org/cmake/help/latest/command/find_package.html) and use the namespaced imported target from the generated package configuration.  
Note that this method requires a release package to be installed somewhere on your machine.  
The package configuration file, `fkYAMLConfig.cmake`, can be used either from an install tree or directly out of the build tree.  

??? Example

    ```cmake title="CMakeLists.txt"
    cmake_minimum_required(VERSION 3.8)
    project(ExampleProject LANGUAGES CXX)

    find_package(fkYAML 0.3.1 REQUIRED)

    add_executable(example example.cpp)
    target_link_libraries(example PRIVATE fkYAML::fkYAML)
    ```

### With `add_subdirectory()`

To embed the library directory into an existing CMake project, place the entire source tree in a subdirectory and call [`add_subdirectory()`](https://cmake.org/cmake/help/latest/command/add_subdirectory.html) in your `CMakeLists.txt` file.  

??? Example

    ```cmake title="CMakeLists.txt"
    cmake_minimum_required(VERSION 3.8)
    project(ExampleProject LANGUAGE CXX)

    add_subdirectory(./path/to/fkYAML)

    add_executable(example example.cpp)
    target_link_libraries(example PRIVATE fkYAML::fkYAML)
    ```

### With the `FetchContent` CMake module

Since CMake v3.11, [`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html) is available which automatically downloads a release as a dependency during configuration.  

??? Example

    ```cmake title="CMakeLists.txt"
    cmake_minimum_required(VERSION 3.11)
    project(ExampleProject LANGUAGES CXX)

    include(FetchContent)

    FetchContent_Declare(
        fkYAML
        GIT_REPOSITORY https://github.com/fktn-k/fkYAML.git
        GIT_TAG v0.4.2
    )
    FetchContent_MakeAvailable(fkYAML)

    add_executable(example example.cpp)
    target_link_libraries(example PRIVATE fkYAML::fkYAML)
    ```
