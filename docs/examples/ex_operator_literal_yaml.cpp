#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // bring the user-defined literals into this scope.
    using namespace fkyaml::literals::yaml_literals;

    // deserialize a YAML string encoded in the UTF-8 format.
    fkyaml::node n_u8 = u8"foo: bar"_yaml;
    std::cout << n_u8 << std::endl;

    // deserialize a YAML string encoded in the UTF-16 format.
    fkyaml::node n_u16 = u"foo: bar"_yaml;
    std::cout << n_u16 << std::endl;

    // deserialize a YAML string encoded in the UTF-32 format.
    fkyaml::node n_u32 = U"foo: bar"_yaml;
    std::cout << n_u32 << std::endl;

    return 0;
}
