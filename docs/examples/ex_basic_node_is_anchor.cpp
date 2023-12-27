#include <iomanip>
#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create YAML nodes (both anchor/non-anchor)
    fkyaml::node anchor = true;
    anchor.add_anchor_name("anchor");
    fkyaml::node non_anchor = 123;

    // print the result of testing whether they are an anchor node or not.
    std::cout << std::boolalpha << anchor.is_anchor() << std::endl;
    std::cout << non_anchor.is_anchor() << std::endl;

    return 0;
}
