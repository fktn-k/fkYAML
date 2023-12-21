#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::node n(fkyaml::node::node_t::BOOLEAN);
    fkyaml::node n2(n);
    std::cout << n << std::endl;
    return 0;
}
