#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create a YAML node.
    fkyaml::node n = 123;

    // try to get a tag name before any tag name has been set.
    try
    {
        std::cout << n.get_tag_name() << std::endl;
    }
    catch (const fkyaml::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    // set a tag name to the node.
    n.add_tag_name("!!int");
    std::cout << n.get_tag_name() << std::endl;

    return 0;
}
