#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    try
    {
        throw fkyaml::exception();
    }
    catch (const fkyaml::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
