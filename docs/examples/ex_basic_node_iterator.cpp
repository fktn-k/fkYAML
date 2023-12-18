#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create YAML nodes.
    fkyaml::node sequence_node = {1, 2, 3};
    // get an iterator to the first sequence element.
    fkyaml::node::iterator it = sequence_node.begin();
    std::cout << *it << std::endl;
    return 0;
}
