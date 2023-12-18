#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::node n;
    n.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_1);
    fkyaml::node n2;
    n2.set_yaml_version(fkyaml::node::yaml_version_t::VER_1_2);

    std::cout << std::boolalpha;
    std::cout << (n.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_1) << std::endl;
    std::cout << (n2.get_yaml_version() == fkyaml::node::yaml_version_t::VER_1_2) << std::endl;

    return 0;
}
