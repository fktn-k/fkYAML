#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create a sequence node.
    fkyaml::node n = {"foo", "bar"};
    // get an iterator to the first element.
    fkyaml::node::iterator it = n.begin();
    std::cout << *it << std::endl;
    return 0;
}
