#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create a YAML node.
    fkyaml::node n = {{"foo", true}};

    // check if the node has a tag name.
    std::cout << std::boolalpha << n.has_tag_name() << std::endl;

    // set a tag name.
    n.add_tag_name("!!map");

    // check if the node has a tag name again.
    std::cout << std::boolalpha << n.has_tag_name() << std::endl;

    return 0;
}
