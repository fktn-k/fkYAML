#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::node n;
    std::cout << std::boolalpha << n.is_null() << std::endl;
    return 0;
}
