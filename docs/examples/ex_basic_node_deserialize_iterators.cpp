#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <fkYAML/node.hpp>

int main()
{
    // deserialize a YAML string.
    std::array<char, 14> input = {
        'f', 'o', 'o', ':', ' ', 't', 'r', 'u', 'e', 'd', 'u', 'm', 'm', 'y'
    };
    fkyaml::node n = fkyaml::node::deserialize(input.begin(), input.begin() + 9);

    // check the deserialization result.
    std::cout << std::boolalpha << n["foo"].get_value<bool>() << std::endl;

    return 0;
}
