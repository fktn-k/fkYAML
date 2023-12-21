#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create YAML nodes.
    std::vector<fkyaml::node> nodes =
    {
        {1, 2, 3},
        {{"foo", true}, {"bar", false}, {"baz", true}},
        fkyaml::node(),
        true,
        256,
        3.14,
        "foo"
    };

    for (const auto& n : nodes)
    {
        try
        {
            // call size()
            std::cout << n.size() << std::endl;
        }
        catch (const fkyaml::exception& e)
        {
            std::cout << "The node does not have a container nor string value." << std::endl;
        }
    }

    return 0;
}
