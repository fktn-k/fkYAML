#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    fkyaml::ordered_map<std::string, fkyaml::node> om = {
        { "foo", 123 },
        { "bar", "baz" }
    };

    // search for a value with an existing key.
    auto itr = om.find("foo");
    if (itr != om.end())
    {
        std::cout << itr->second << std::endl;
    }

    // search for a value with a key which does not exist.
    auto itr2 = om.find("qux");
    if (itr2 == om.end())
    {
        std::cout << "key does not exist." << std::endl;
    }

    return 0;
}
