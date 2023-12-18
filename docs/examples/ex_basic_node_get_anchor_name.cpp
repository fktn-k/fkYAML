#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create a YAML node.
    fkyaml::node n = 123;

    // try to get an anchor name before any anchor name has been set.
    try
    {
        std::cout << n.get_anchor_name() << std::endl;
    }
    catch (const fkyaml::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    // add an anchor name to the node.
    n.add_anchor_name("anchor");
    std::cout << n.get_anchor_name() << std::endl;

    return 0;
}
