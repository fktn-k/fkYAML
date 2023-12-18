#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::node n({true, false});
    std::cout << n << std::endl;
    return 0;
}
