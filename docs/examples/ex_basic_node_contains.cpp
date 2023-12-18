#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create a YAML mapping node.
    fkyaml::node n = {{"foo", true}, {"bar", 123}};

    // check if the node has the following keys.
    std::cout << std::boolalpha;
    std::cout << n.contains("foo") << std::endl;
    std::cout << n.contains(fkyaml::node("baz")) << std::endl;

    // create a YAML node. (not mapping)
    fkyaml::node n2 = "qux";

    // check if the node has the following key.
    std::cout << std::boolalpha << n2.contains("qux") << std::endl;

    return 0;
}
