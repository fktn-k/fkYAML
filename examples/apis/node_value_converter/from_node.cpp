//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iostream>
#include <fkYAML/node.hpp>

namespace ns {

struct book {
    std::string title;
    std::string author;
    int year;
};

void from_node(const fkyaml::node& n, book& b) {
    b.title = n["title"].as_str();
    b.author = n["author"].as_str();
    b.year = n["year"].get_value<int>();
}

} // namespace ns

int main() {
    fkyaml::node n = {
        {"title", "Noman's Journey"},
        {"author", "John Doe"},
        {"year", 2023},
    };

    auto b = n.get_value<ns::book>();

    std::cout << "\"" << b.title << "\" was written by " << b.author << " in " << b.year << "." << std::endl;

    return 0;
}
