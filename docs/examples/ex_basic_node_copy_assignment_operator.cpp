#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::node n = true;
    fkyaml::node n2 = 123;
    n = n2;

    std::cout << std::boolalpha << n.is_integer() << std::endl;
    std::cout << n.get_value<std::int64_t>() << std::endl;

    return 0;
}
