#include <iostream>
#include <fkYAML/node.hpp>

namespace ns
{

struct book
{
    std::string title;
    std::string author;
    int year;
};

void from_node(const fkyaml::node& n, book& b)
{
    b.title  = n["title"].get_value_ref<const fkyaml::node::string_type&>();
    b.author = n["author"].get_value_ref<const fkyaml::node::string_type&>();
    b.year   = n["year"].get_value<int>();
}

} // namespace ns

int main()
{
    fkyaml::node n = {
        { "title", "Noman's Journey" },
        { "author", "John Doe" },
        { "year", 2023 },
    };

    auto b = n.get_value<ns::book>();

    std::cout << "\"" << b.title << "\" was written by " << b.author
                << " in " << b.year << "." << std::endl;

    return 0;
}
