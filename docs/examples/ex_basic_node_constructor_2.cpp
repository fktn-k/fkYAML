#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::node n(fkyaml::node::node_t::INTEGER);
    std::cout << n << std::endl;
    return 0;
}
