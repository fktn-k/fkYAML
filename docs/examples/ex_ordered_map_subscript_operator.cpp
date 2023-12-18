#include <iostream>
#include <fkYAML/node.hpp>
#include <fkYAML/ordered_map.hpp>

int main()
{
    fkyaml::ordered_map<std::string, fkyaml::node> om = {
        { "foo", 123 },
        { "bar", "baz" }
    };

    std::cout << om["foo"] << std::endl;
    std::cout << om["bar"] << std::endl;

    // accesses with a unknown key will create a new key-value pair with the default value.
    std::cout << om["qux"] << std::endl;

    return 0;
}
