#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::node::sequence_type s = {fkyaml::node(true), fkyaml::node(false)};
    fkyaml::node n = fkyaml::node::sequence(s);
    std::cout << n << std::endl;
    return 0;
}
