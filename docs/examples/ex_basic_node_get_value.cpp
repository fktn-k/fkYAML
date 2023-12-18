#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create a YAML node.
    fkyaml::node n = 123;
    fkyaml::node n2 = "foo";

    // get references to the value.
    auto int_val = n.get_value<int>();
    auto str_val = n2.get_value<std::string>();

    // print the values
    std::cout << int_val << std::endl;
    std::cout << str_val << std::endl;

    // specifying incompatible type throws an exception
    try
    {
        auto float_val = n2.get_value<float>();
    }
    catch (const fkyaml::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
