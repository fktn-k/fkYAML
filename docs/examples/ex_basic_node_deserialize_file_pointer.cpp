#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <fkYAML/node.hpp>

int main()
{
    // deserialize a YAML string.
    FILE* p_file = std::fopen("input.yaml", "r");
    if (!p_file)
    {
        // You must not pass a null FILE pointer.
        return -1;
    }
    fkyaml::node n = fkyaml::node::deserialize(p_file);

    // check the deserialization result.
    std::cout << n["foo"].get_value<bool>() << std::endl;
    std::cout << n["bar"].get_value<std::int64_t>() << std::endl;
    std::cout << std::setprecision(3) << n["baz"].get_value<double>() << std::endl;

    return 0;
}
