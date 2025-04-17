# The First Steps

The goal of this page is to help you understand what is possible with fkYAML, and to provide some tips of how you can use it within your own project with CMake.  
As a tutorial, we will make a very simple project which utilizes fkYAML so that your understanding can be fostered with an actual, executable example.  

## :rocket: Getting fkYAML

Ideally you should be using fkYAML through its [CMake](https://cmake.org/) integration.  
Although fkYAML also provides a pkg-config file, this documentation will assume you are using CMake.  
fkYAML provides release packages in the releases pages which contain the all the necessary header files and CMake config files.  
Although you can just copy, paste and include the header files in your C++ project, it is highly recommended to use the library with CMake because the exported CMake target `fkYAML::fkYAML` will save you most of your work for the integration.  
The followings are possible ways of preparing for the use of fkYAML:  

### :file_folder: Download a release package
You can [download the latest package (fkYAML.tgz or fkYAML.zip) from here](https://github.com/fktn-k/fkYAML/releases/latest).  
The single-header versions can also be downloaded since v0.3.1 with the name of fkYAML_single_header.{tgz|zip}.  
After the download gets completed, unzip and put the package in some directory on your machine.  
The destination path can be whatever you want, and you're all set!

### :inbox_tray: Install with CMake
You can also clone the fkYAML repository and execute the following commands to install the fkYAML package on your machine.  
Make sure the CMake executable is registered to the PATH environment variable.  

```bash
$ cd /path/to/fkYAML
$ cmake -B build -S . [-DCMAKE_INSTALL_PREFIX=<path-to-install>] [-DFK_YAML_USE_SINGLE_HEADER=ON]
$ cmake --build build --target install
```

With `-DCMAKE_INSTALL_PREFIX=<path-to-install>`, you can override the default installation path.  
The default installation path would be the followings:  

| OS      | Path                      |
| ------- | ------------------------- |
| UNIX    | /usr/local                |
| Windows | C:\\Program Files\\fkYAML |

Furthermore, you can install the single-header version of fkYAML by adding the `-DFK_YAML_USE_SINGLE_HEADER=ON` parameter on configuring CMake.  

And you're all set! You can now `find_package()` the fkYAML package.

### :pushpin: Use fkYAML as a Git submodule
If you want to avoid system-wide installation, you can use the fkYAML library as a Git submodule in you C++ project instead.  
The commands would be the following (Make sure the Git executable is registered to the PATH environment.):  

```bash
$ cd /path/to/your/repo
$ git submodule add https://github.com/fktn-k/fkYAML.git [path/to/clone/fkYAML]
```

And you're all set!  
Note that you must execute the CMake `add_subdirectory()` command for the submoduled fkYAML library in your CMakeLists.txt file as explained in the CMake integration section.  

## :bulb: Use fkYAML in your C++ project

### :seedling: Create a tutorial project.

Let's start with a really simple example.  
Say you have an example.yaml file and now you want to load the contents.  
Note that the following example files assumes that you have installed the fkYAML library somewhere on your machine.  
See [the CMake Integration section](./cmake_integration.md) for the other ways and modify the implementation if necessary.  
Also, make sure the example.yaml file is encoded in either the UTF-8, UTF-16BE/LE or UTF-32BE/LE format.  

```title="Project Structure"
.
├── CMakeLists.txt
├── example.yaml
└── tutorial.cpp
```

=== "example.yaml"

    ```yaml
    --8<-- "tutorials/example.yaml"
    ```
=== "tutorial.cpp"

    ```cpp
    --8<-- "tutorials/tutorial_1.cpp:9"
    ```
=== "CMakeLists.txt"

    ```cmake

    cmake_minimum_required(VERSION 3.8)
    project(tutorial LANGUAGES CXX)

    find_package(fkYAML REQUIRED)

    add_executable(tutorial tutorial.cpp)

    # This exported CMake target sets the necessary configurations for the project.
    target_link_libraries(tutorial PUBLIC fkYAML::fkYAML)
    ```

After creating a tutorial project with the above files, execute the following commands to build the project with CMake.  

```bash
$ cd /path/to/tutorial/
$ cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
$ cmake --build build --config Release
```

Congratulation! You've got an application which loads a YAML file and then outputs the content on the console.  
If you run the tutorial executable file, you will see the output like:  

```bash
--8<-- "tutorials/tutorial_1.output"
```

### :mag: Access individual YAML nodes
You can also access each YAML node with the fkYAML APIs.  
Say you just want to care about values associated with the `title` key and ignore the others.  
You can do it by modifying the tutorial.cpp file as follows:  

```cpp title="tutorial.cpp" hl_lines="12-16"
--8<-- "tutorials/tutorial_2.cpp:9"
```

Rebuild and run the application, and you'll see the output like:  

```bash
--8<-- "tutorials/tutorial_2.output"
```

### :hammer: Generate YAML nodes from code

When you can access the specific values, you might want to generate YAML nodes programatically, for instance, to make a response parameter.  
The fkYAML library also provides a feature to realize such a need.  

You can achieve that by changing the highlighted part of the code snippet:  

```cpp title="tutorial.cpp" hl_lines="13-15 17 19-21 24-25"
--8<-- "tutorials/tutorial_3.cpp:9"
```

Rebuild and run the application, and you'll see the output like:  

```bash
--8<-- "tutorials/tutorial_3.output"
```

### :pill: Integrate with user-defined types

As described in the API Reference pages for [`from_node()`](../api/node_value_converter/from_node.md) and [`to_node()`](../api/node_value_converter/to_node.md) functions, you can specialize serialization and deserialization for user-defined types.  
Note that you don't need to implement specializations for STL types (such as std::vector or std::string) because the fkYAML library has already implemented them.  

The updated code snippet down below shows how the specializations for user-defined types can reduce boilerplate code.  

```cpp title="tutorial.cpp" hl_lines="6-31 44-45 48-51"
--8<-- "tutorials/tutorial_4.cpp:9"
```

Rebuild and run the application, and you'll see the same output as before:  

```bash
--8<-- "tutorials/tutorial_4.output"
```

The specializations highlighted above do not change the output, but they allow us to focus more on the code's intent.  
Note that the specialization with user-defined types requires the types to be default-constructible.  

## :tada: Next Steps

This page is a brief introduction to get you up and running with fkYAML, and to show the basic features of fkYAML.  
The features mentioned here can get you quite far, but there are many more.  
For more information, please visit [the API Documentation section](../api/basic_node/index.md) which has a lot of descriptions for each fkYAML APIs with example code snippets.  
