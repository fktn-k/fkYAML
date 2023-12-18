#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    std::cout << "fkYAML version "
                << FK_YAML_MAJOR_VERSION << "."
                << FK_YAML_MINOR_VERSION << "."
                << FK_YAML_PATCH_VERSION << std::endl;
    return 0;
}
