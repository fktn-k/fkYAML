#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::node n = {{"foo", true}};
    std::cout << std::boolalpha << n.is_mapping() << std::endl;
    return 0;
}
