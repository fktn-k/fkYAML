#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // A `fkyaml::node` object has `yaml_version_t::VER_1_2` by default.
    fkyaml::node n;
    std::cout << std::boolalpha
              << (n.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_2)
              << std::endl;

    return 0;
}
