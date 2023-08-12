/**
 * NodeType.hpp - Definitions of YAML node data types.
 * 
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_NODE_TYPE_HPP_
#define FK_YAML_NODE_TYPE_HPP_

namespace fkyaml
{

enum class NodeType
{
    SEQUENCE,
    MAPPING,
    NULL_OBJECT,
    BOOLEAN,
    SIGNED_INTEGER,
    UNSIGNED_INTEGER,
    FLOAT_NUMBER,
    STRING,
};

} // namespace fkyaml

#endif /* FK_YAML_NODE_TYPE_HPP_ */