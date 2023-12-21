#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create a YAML node.
    fkyaml::node anchor_node = "test";

    // add an anchor name to the node.
    anchor_node.add_anchor_name("anchor");

    // create an alias YAML node.
    fkyaml::node alias_node = fkyaml::node::alias_of(anchor_node);

    // print the value in the alias node.
    std::cout<< alias_node.get_value_ref<fkyaml::node::string_type&>() << std::endl;

    return 0;
}
