#include <fstream>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    std::ifstream ifs("input.yaml");
    fkyaml::node n;
    ifs >> n;

    // print the deserialization result.
    std::cout << n << std::endl;

    return 0;
}
