#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::ordered_map<std::string, fkyaml::node> om = {
        { "foo", 123 },
        { "bar", "baz" }
    };

    std::cout << om.at("foo") << std::endl;
    std::cout << om.at("bar") << std::endl;

    // accesses with a unknown key will throw an exception.
    try
    {
        fkyaml::node& n = om.at("baz");
    }
    catch (const fkyaml::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
