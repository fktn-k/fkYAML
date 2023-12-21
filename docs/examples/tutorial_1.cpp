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
