//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iostream>
#include <fkYAML/node.hpp>

int main() {
    // create a YAML node.
    fkyaml::node n = "foo";

    // try to get a tag name before any tag name has been set.
    try {
        std::cout << n.get_resolved_tag_name() << std::endl;
    }
    catch (const fkyaml::exception& e) {
        std::cout << e.what() << std::endl;
    }

    // The tag name (!!str) is set during deserialization.
    n = fkyaml::node::deserialize("- !!str foo");
    std::cout << n.at(0).get_resolved_tag_name() << std::endl;

    // "%TAG" directive changes the prefix.
    std::string input = "%TAG !! tag:example.com,2026:\n"
                        "---\n"
                        "- !!str foo\n";
    n = fkyaml::node::deserialize(input);
    std::cout << n.at(0).get_resolved_tag_name() << std::endl;

    // You can also set a tag name manually.
    n = "foo";
    n.add_tag_name("!!str");
    std::cout << n.get_resolved_tag_name() << std::endl;

    return 0;
}
