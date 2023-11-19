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
You can [download the latest package (fkYAML.tgz for UNIX or fkYAML.zip for Windows) from here](https://github.com/fktn-k/fkYAML/releases/latest).  
After the download gets completed, unzip and put the package in some directory on your machine.  
The destination path can be whatever you want, and you're all set!

### :inbox_tray: Install with CMake
You can also clone the fkYAML repository and execute the following commands to install the fkYAML package on your machine.  
Make sure the CMake executable is registered to the PATH environment variable.  

```bash
$ cd /path/to/fkYAML
$ cmake -B build -S . [-DCMAKE_INSTALL_PREFIX=<path-to-install>]
$ cmake --build build --target install
```

With `-DCMAKE_INSTALL_PREFIX=<path-to-install>`, you can override the default installation path.  
The default installation path would be the followings:  

| OS      | Path                      |
| ------- | ------------------------- |
| UNIX    | /usr/local                |
| Windows | C:\\Program Files\\fkYAML |

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
See [the CMake Integration section]() for the other ways and modify the implementation if necessary.  

```title="Project Structure"
.
├── CMakeLists.txt
├── example.yaml
└── tutorial.cpp
```

=== "example.yaml"

    ```yaml
    novels:
      - title: Robinson Crusoe
        author: Daniel Defoe
        year: 1678
      - title: Frankenstein
        author: Jane Austen
        year: 1818
      - title: Moby-Dick
        author: Herman Melville
        year: 1851
      - title: Brave New World
        author: Aldous Huxley
        year: 1932
      - title: Never Let Me Go
        author: Kazuo Ishiguro
        year: 2005
    ```
=== "tutorial.cpp"

    ```cpp
    #include <fstream>
    #include <iostream>
    #include <fkYAML/node.hpp>

    int main()
    {
        // open a YAML file. Other streams or strings are also usable as an input.
        std::ifstream ifs("example.yaml");

        // deserialize the loaded file contents.
        fkyaml::node root = fkyaml::node::deserialize(ifs);

        // print the deserialized YAML nodes by serializing them back.
        std::cout << root << std::endl;
        return 0;
    }
    ```
=== "CMakeLists.txt"

    ```cmake

    cmake_minimum_required(VERSION 3.8)
    project(tutorial LANGUAGES CXX)

    find_package(fkYAML REQUIRED)

    add_executable(tutorial tutorial.cpp)

    # This exported CMake target sets the necessary configurations for the project.
    target_link_library(tutorial PUBLIC fkYAML::fkYAML)
    ```

After creating a tutorial project with the above files, execute the following commands to build the project with CMake.  

```bash
$ cd /path/to/tutorial/
$ cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
$ cmake --build build --config --config Release
```

Congratulation! You've got an application which loads a YAML file and then outputs the content on the console.  
If you run the tutorial executable file, you will see the output like:  

```bash
novels:
  -
    title: Robinson Crusoe
    author: Daniel Defoe
    year: 1678
  -
    title: Frankenstein
    author: Mary Shelly
    year: 1818
  -
    title: Moby-Dick
    author: Herman Melville
    year: 1851
  -
    title: Brave New World
    author: Aldous Huxley
    year: 1932
  -
    title: Never Let Me Go
    author: Kazuo Ishiguro
    year: 2005
```

### :mag: Access individual YAML nodes
You can also access each YAML node with the fkYAML APIs.  
Say you just want to care about values associated with the `title` key and ignore the others.  
You can do it by modifying the tutorial.cpp file as follows:  

```cpp title="tutorial.cpp" hl_lines="13-18"
#include <fstream>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // open a YAML file. Other streams or strings are also usable as an input.
    std::ifstream ifs("example.yaml");

    // deserialize the loaded file contents.
    fkyaml::node root = fkyaml::node::deserialize(ifs);

    // print only values associated with "title" key.
    for (auto& novel_node : root["novels"])
    {
        // get reference to the "title" value with `get_value_ref` function.
        std::cout << novel_node["title"].get_value_ref<std::string&>() << std::endl;
    }

    return 0;
}
```

Rebuild and run the application, and you'll see the output like:  

```bash
Robinson Crusoe
Frankenstein
Moby-Dick
Brave New World
Never Let Me Go
```

### :hammer: Generate YAML nodes from code

When you can access the specific values, you might want to generate YAML nodes programatically, for instance, to make a response parameter.  
The fkYAML library also provides a feature to realize such a need.  

You can achieve that by changing the highlighted part of the code snippet:  

```cpp title="tutorial.cpp" hl_lines="14-16 18 21-26 29-30"
#include <fstream>
#include <iostream>
#include <utility>
#include <fkYAML/node.hpp>

int main()
{
    // open a YAML file. Other streams or strings are also usable as an input.
    std::ifstream ifs("example.yaml");

    // deserialize the loaded file contents.
    fkyaml::node root = fkyaml::node::deserialize(ifs);

    // create an empty YAML sequence node.
    fkyaml::node response = { "recommends", fkyaml::node::sequence() };
    auto& recommends = response["recommends"].get_value_ref<fkyaml::node::sequence_type&>();

    // generate recommendations by extracting "title" & "author" values.
    for (auto& novel_node : root["novels"])
    {
        // create a recommendation node with an initializer list.
        fkyaml::node recommend = {
            { "title", novel_node["title"] },
            { "author", novel_node["author"] }
        };
        recommends.emplace_back(std::move(recommends));
    }

    // print the response YAML nodes.
    std::cout << response << std::endl;

    return 0;
}
```

Rebuild and run the application, and you'll see the output like:  

```bash
recommends:
  -
    title: Robinson Crusoe
    author: Daniel Defoe
  -
    title: Frankenstein
    author: Mary Shelly
  -
    title: Moby-Dick
    author: Herman Melville
  -
    title: Brave New World
    author: Aldous Huxley
  -
    title: Never Let Me Go
    author: Kazuo Ishiguro
```

### :pill: Integrate with user-defined types

As described in the API Reference pages for [`from_node()`](../api/node_value_converter/from_node.md) and [`to_node()`](../api/node_value_converter/to_node.md) functions, you can specialize serialization and deserialization for user-defined types.  
Note that you don't need to implement specializations for STL types (such as std::vector or std::string) because the fkYAML library has already implemented them.  

The updated code snippet down below shows how the specializations for user-defined types can reduce boilerplate code.  

```cpp title="tutorial.cpp" hl_lines="6-39 53-54 56-57 59-61"
#include <fstream>
#include <iostream>
#include <utility>
#include <fkYAML/node.hpp>

// creating a namespace is not mandatory.
namespace ns
{

struct novel
{
    std::string title;
    std::string author;
    int year;
};

struct recommend
{
    std::string title;
    std::string author;
};

// overloads must be defined in the same namespace as user-defined types.
void from_node(const fkyaml::node& node, novel& novel)
{
    novel.title = node["title"].get_value_ref<const std::string&>();
    novel.author = node["author"].get_value_ref<const std::string&>();
    novel.year = node["year"].get_value<int>();
}

void to_node(fkyaml::node& node, const recommend& recommend)
{
    node = fkyaml::node {
        { "title", recommend.title },
        { "author", recommend.author }
    };
}

} // namespace ns

int main()
{
    // open a YAML file. Other streams or strings are also usable as an input.
    std::ifstream ifs("example.yaml");

    // deserialize the loaded file contents.
    fkyaml::node root = fkyaml::node::deserialize(ifs);

    // create an empty YAML sequence node.
    fkyaml::node response = { "recommends", fkyaml::node::sequence() };
    auto& recommends = response["recommends"].get_value_ref<fkyaml::node::sequence_type&>();

    // get novels directly from the node.
    auto novels = root["novels"].get_value<std::vector<ns::novel>>();

    // generate recommendations by extracting "title" & "author" values.
    for (auto& novel : novels)
    {
        // create a recommendation node directly with a recommend object.
        ns::recommend recommend = { std::move(novel.title), std::move(novel.author) };
        recommends.emplace_back(recommend);
    }

    // print the response YAML nodes.
    std::cout << response << std::endl;

    return 0;
}
```

The specializations highlighted above do not change the output, but they allow us to focus more on what the code is to achive.  
Note that the specialization with user-defined types requires the types to be default-constructible.  

## :tada: Next Steps

This page is a brief introduction to get you up and running with fkYAML, and to show the basic features of fkYAML.  
The features mentioned here can get you quite far, but there are many more.  
For more information, please visit [the API Documentation section](../api/basic_node/index.md) which has a lot of descriptions for each fkYAML APIs with example code snippets.  
