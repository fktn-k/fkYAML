#include <cstdint>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <fkYAML/node.hpp>

int main()
{
    std::cout << std::boolalpha
                << std::is_same<std::map<std::string, fkyaml::node>, fkyaml::node::mapping_type>::value
                << std::endl;
    return 0;
}
