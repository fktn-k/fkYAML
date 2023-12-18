#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::node n = "foo";
    std::cout << std::boolalpha << n.is_string() << std::endl;
    return 0;
}
