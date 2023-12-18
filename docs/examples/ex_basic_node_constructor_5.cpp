#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    double pi = 3.141592;
    fkyaml::node n = pi;
    std::cout << n << std::endl;
    return 0;
}
