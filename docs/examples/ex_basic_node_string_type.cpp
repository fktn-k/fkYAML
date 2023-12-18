#include <iomanip>
#include <iostream>
#include <string>
#include <type_traits>
#include <fkYAML/node.hpp>

int main()
{
    std::cout << std::boolalpha
                << std::is_same<std::string, fkyaml::node::string_type>::value
                << std::endl;
    return 0;
}
