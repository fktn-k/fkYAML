#include <iomanip>
#include <iostream>
#include <type_traits>
#include <fkYAML/node.hpp>

int main()
{
    std::cout << std::boolalpha
                << std::is_same<
                        fkyaml::node_value_converter<double, void>,
                        fkyaml::node::value_converter_type<double, void>>::value
                << std::endl;

    fkyaml::node n = 3.14;
    double d = 0.0;
    // This leads to the same result as `d = n.get_value<double>()`.
    fkyaml::node::value_converter_type<double, void>::from_node(n, d);
    std::cout << std::setprecision(3) << d << std::endl;

    return 0;
}
