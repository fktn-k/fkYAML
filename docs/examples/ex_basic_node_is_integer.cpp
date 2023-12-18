#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::node n = 123;
    std::cout << std::boolalpha << n.is_integer() << std::endl;
    return 0;
}
