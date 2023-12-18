#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create a YAML node.
    fkyaml::node n = {{"foo", true}};

    // check if the node has an anchor name.
    std::cout << std::boolalpha << n.has_anchor_name() << std::endl;

    // set an anchor name.
    n.add_anchor_name("anchor");

    // check if the node has an anchor name again.
    std::cout << std::boolalpha << n.has_anchor_name() << std::endl;

    return 0;
}
