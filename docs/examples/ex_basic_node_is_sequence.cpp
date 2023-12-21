#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::node n = {1, 2, 3};
    std::cout << std::boolalpha << n.is_sequence() << std::endl;
    return 0;
}
