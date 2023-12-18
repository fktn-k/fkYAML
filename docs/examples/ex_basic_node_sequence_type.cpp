#include <iomanip>
#include <iostream>
#include <type_traits>
#include <vector>
#include <fkYAML/node.hpp>

int main()
{
    std::cout << std::boolalpha
                << std::is_same<std::vector<fkyaml::node>, fkyaml::node::sequence_type>::value
                << std::endl;
    return 0;
}
