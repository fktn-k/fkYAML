#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::node n = 3.14;
    std::cout << std::boolalpha << n.is_float_number() << std::endl;
    return 0;
}
