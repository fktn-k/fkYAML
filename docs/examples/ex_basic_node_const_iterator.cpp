#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create YAML nodes.
    const fkyaml::node sequence_node = {1, 2, 3};
    // get an iterator to the first sequence element.
    fkyaml::node::const_iterator it = sequence_node.begin();
    std::cout << *it << std::endl;
    return 0;
}
