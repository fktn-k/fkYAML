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

void to_node(fkyaml::node& n, const book& b)
{
    n = {
        { "title", b.title },
        { "author", b.author },
        { "year", b.year }
    };
}

} // namespace ns

int main()
{
    ns::book b = { "Noman's Journey", "John Doe", 2023 };

    fkyaml::node n = b;

    std::cout << n << std::endl;

    return 0;
}
