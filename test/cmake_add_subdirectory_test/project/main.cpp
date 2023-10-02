#include <iostream>

#include "fkYAML/Node.hpp"
#include "fkYAML/Deserializer.hpp"

int main()
{
    fkyaml::Deserializer deserializer;
    fkyaml::Node node = deserializer.Deserialize("test: true");

    std::cout << "test: " << node["test"].ToString() << std::endl;

    return 0;
}
