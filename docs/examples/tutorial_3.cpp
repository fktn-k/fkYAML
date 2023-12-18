#include <fstream>
#include <iostream>
#include <utility>
#include <fkYAML/node.hpp>

int main()
{
    // open a YAML file. Other streams or strings are also usable as an input.
    std::ifstream ifs("example.yaml");

    // deserialize the loaded file contents.
    fkyaml::node root = fkyaml::node::deserialize(ifs);

    // create an empty YAML sequence node.
    fkyaml::node response = {{ "recommends", fkyaml::node::sequence() }};
    auto& recommends = response["recommends"].get_value_ref<fkyaml::node::sequence_type&>();

    // generate recommendations by extracting "title" & "author" values.
    for (auto& novel_node : root["novels"])
    {
        // create a recommendation node with an initializer list.
        fkyaml::node recommend = {
            { "title", novel_node["title"] },
            { "author", novel_node["author"] }
        };
        recommends.emplace_back(std::move(recommend));
    }

    // print the response YAML nodes.
    std::cout << response << std::endl;

    return 0;
}
