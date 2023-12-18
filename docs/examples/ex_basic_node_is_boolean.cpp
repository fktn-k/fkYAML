#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::node n = true;
    std::cout << std::boolalpha << n.is_boolean() << std::endl;
    return 0;
}
