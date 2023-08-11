/**
 * NodeType.hpp - Definitions of YAML node data types.
 * 
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_NODE_TYPE_HPP_
#define FK_YAML_NODE_TYPE_HPP_

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

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

class Node;

using NodeSequenceType    = std::vector<Node>;
using NodeMappingType     = std::unordered_map<std::string, Node>;
using NodeBooleanType     = bool;
using NodeSignedIntType   = int64_t;
using NodeUnsignedIntType = uint64_t;
using NodeFloatNumberType = double;
using NodeStringType      = std::string;

} // namespace fkyaml

#endif /* FK_YAML_NODE_TYPE_HPP_ */