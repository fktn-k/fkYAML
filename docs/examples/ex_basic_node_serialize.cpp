#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create a basic_node object.
    fkyaml::node n = {
        {"foo", true},
        {"bar", {1, 2, 3}},
        {"baz", {
            {"qux", 3.14},
            {"corge", nullptr}
        }}
    };

    // serialize the basic_node object.
    std::cout << fkyaml::node::serialize(n) << std::endl;

    return 0;
}
