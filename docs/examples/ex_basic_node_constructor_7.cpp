#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::node n = {true, false};
    std::cout << n << std::endl;

    fkyaml::node n2 = {{"foo", 1024}};
    std::cout << n2 << std::endl;
    return 0;
}
