#include <iomanip>
#include <iostream>
#include <type_traits>
#include <fkYAML/node.hpp>

int main()
{
    std::cout << std::boolalpha
                << std::is_same<double, fkyaml::node::float_number_type>::value
                << std::endl;
    return 0;
}
