#include <iostream>
#include <fkYAML/node.hpp>

int main()
{
    // create YAML nodes.
    fkyaml::node n1 = 123;
    fkyaml::node n2 = "foo";

    // swap the internally stored data between n1 & n2.
    using std::swap;
    swap(n1, n2);

    // print the swapped values.
    std::cout << n1.get_value_ref<std::string&>() << std::endl;
    std::cout << n2.get_value<std::int64_t>() << std::endl;
    return 0;
}
