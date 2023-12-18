#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::ordered_map<std::string, fkyaml::node> om = {
        { "foo", 123 },
        { "bar", "baz" }
    };

    for (auto& pair : om)
    {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    return 0;
}
