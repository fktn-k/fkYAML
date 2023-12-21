#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create YAML nodes.
    fkyaml::node null_node;
    fkyaml::node boolean_node = true;
    fkyaml::node integer_node = 256;
    fkyaml::node float_node = 3.14;
    fkyaml::node string_node = "Hello, world!";

    // call type()
    std::cout << std::boolalpha;
    std::cout << (null_node.type() == fkyaml::node::node_t::NULL_OBJECT) << std::endl;
    std::cout << (boolean_node.type() == fkyaml::node::node_t::BOOLEAN) << std::endl;
    std::cout << (integer_node.type() == fkyaml::node::node_t::INTEGER) << std::endl;
    std::cout << (float_node.type() == fkyaml::node::node_t::FLOAT_NUMBER) << std::endl;
    std::cout << (string_node.type() == fkyaml::node::node_t::STRING) << std::endl;

    return 0;
}
