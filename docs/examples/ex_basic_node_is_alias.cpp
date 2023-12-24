#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create YAML nodes (both anchor/non-anchor)
    fkyaml::node anchor = true;
    anchor.add_anchor_name("anchor");
    fkyaml::node alias = fkyaml::node::alias_of(anchor);
    fkyaml::node non_alias = 123;

    // print the result of testing whether they are an anchor node or not.
    std::cout << std::boolalpha;
    std::cout << anchor.is_alias() << std::endl;
    std::cout << alias.is_alias() << std::endl;
    std::cout << non_alias.is_alias() << std::endl;

    return 0;
}
