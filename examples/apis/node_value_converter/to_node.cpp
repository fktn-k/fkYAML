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

void to_node(fkyaml::node& n, const book& b) {
    n = {{"title", b.title}, {"author", b.author}, {"year", b.year}};
}

} // namespace ns

int main() {
    ns::book b = {"Noman's Journey", "John Doe", 2023};

    fkyaml::node n = b;

    std::cout << n << std::endl;

    return 0;
}
