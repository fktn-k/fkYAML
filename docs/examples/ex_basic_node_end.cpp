#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create a sequence node.
    fkyaml::node n = {"foo", "bar"};
    // get an iterator to the past-the-last element.
    fkyaml::node::iterator it = n.end();
    // decrement the iterator to point to the last element.
    --it;
    std::cout << *it << std::endl;
    return 0;
}
