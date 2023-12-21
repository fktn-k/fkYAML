#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::node::mapping_type m = {
        {"foo", false},
        {"bar", 3.14}
    };
    fkyaml::node n = fkyaml::node::mapping(m);
    std::cout << n << std::endl;
    return 0;
}
