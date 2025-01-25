//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <iostream>
#include <fkYAML/node.hpp>

int main() {
    // create a basic_node object.
    fkyaml::node n1 = {
        {"foo", true},
        {"bar", {1, 2, 3}},
        {"baz", {{"qux", 3.14}, {"corge", nullptr}}},
        {{{true, 123}}, false},
        {{1.23, 4.56, 7.89}, 123456789}};
    // set tags to some nodes.
    n1["foo"].add_tag_name("!!bool");
    n1["bar"][1].add_tag_name("!<tag:yaml.org,2002:int>");
    // set an anchor name to a node.
    n1["baz"].add_anchor_name("anchor");

    // create another one.
    fkyaml::node n2 = {"foo", 123, true};

    // serialize the basic_node objects into a string.
    std::cout << fkyaml::node::serialize_docs({n1, n2}) << std::endl;

    return 0;
}
