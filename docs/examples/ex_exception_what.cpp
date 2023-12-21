#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    try
    {
        throw fkyaml::exception("An error message.");
    }
    catch (const fkyaml::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
