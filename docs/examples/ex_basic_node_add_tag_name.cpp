#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create a YAML node.
    fkyaml::node n = 123;

    // set a tag name to the node.
    n.add_tag_name("!!int");
    std::cout << n.get_tag_name() << std::endl;

    // overwrite it with a new one.
    n.add_tag_name("!!foo");
    std::cout << n.get_tag_name() << std::endl;

    return 0;
}
