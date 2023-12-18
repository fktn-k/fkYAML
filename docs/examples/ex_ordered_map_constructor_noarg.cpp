#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::ordered_map<std::string, fkyaml::node> om;
    std::cout << std::boolalpha << om.empty() << std::endl;
    return 0;
}
