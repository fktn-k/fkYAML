#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create YAML nodes.
    std::vector<fkyaml::node> nodes =
    {
        fkyaml::node::sequence(),
        {1, 2, 3},
        {{"foo", true}, {"bar", false}},
        fkyaml::node(),
        true,
        256,
        3.14,
        "Hello, world!"
    };

    for (const auto& n : nodes)
    {
        try
        {
            // call empty()
            std::cout << std::boolalpha << n.empty() << std::endl;
        }
        catch (const fkyaml::exception& e)
        {
            std::cout << "The node does not have a container nor string value." << std::endl;
        }
    }

    return 0;
}
