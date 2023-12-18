#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create a YAML node.
    fkyaml::node n =
    {
        {"foo", 3.14},
        {"bar", true},
        {"baz", nullptr},
        {"qux", {
            {"corge", {1, 2, 3}}
        }}
    };

    // add a new value.
    n["qux"]["key"] = {"another", "value"};

    // output a YAML formatted string.
    std::cout << n << std::endl;
}
