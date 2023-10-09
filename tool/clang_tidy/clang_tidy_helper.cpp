//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.1
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include "fkYAML/detail/assert.hpp"
#include "fkYAML/detail/from_node.hpp"
#include "fkYAML/detail/iterator.hpp"
#include "fkYAML/detail/lexical_analyzer.hpp"
#include "fkYAML/detail/node_t.hpp"
#include "fkYAML/detail/stl_supplement.hpp"
#include "fkYAML/detail/to_node.hpp"
#include "fkYAML/detail/type_traits.hpp"
#include "fkYAML/detail/version_macros.hpp"
#include "fkYAML/detail/yaml_version_t.hpp"
#include "fkYAML/deserializer.hpp"
#include "fkYAML/exception.hpp"
#include "fkYAML/node_value_converter.hpp"
#include "fkYAML/node.hpp"
#include "fkYAML/ordered_map.hpp"
#include "fkYAML/serializer.hpp"

// This is just a dummy main function for compilation.
int main()
{
    return 0;
}
