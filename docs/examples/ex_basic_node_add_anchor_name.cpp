#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create a YAML node.
    fkyaml::node n = 123;

    // add an anchor name to the node.
    n.add_anchor_name("anchor");
    std::cout << n.get_anchor_name() << std::endl;

    // overwrite it with a new one.
    n.add_anchor_name("anchor2");
    std::cout << n.get_anchor_name() << std::endl;

    return 0;
}
