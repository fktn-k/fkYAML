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
