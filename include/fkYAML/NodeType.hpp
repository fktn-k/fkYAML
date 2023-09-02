/**
 * @file NodeType.hpp
 * @brief Definitions of YAML node data types.
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_NODE_TYPE_HPP_
#define FK_YAML_NODE_TYPE_HPP_

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
namespace fkyaml
{

/**
 * @enum NodeType
 * @brief Definition of node value types.
 */
enum class NodeType
{
    SEQUENCE,         //!< sequence value type
    MAPPING,          //!< mapping value type
    NULL_OBJECT,      //!< null value type
    BOOLEAN,          //!< boolean value type
    SIGNED_INTEGER,   //!< signed integer value type
    UNSIGNED_INTEGER, //!< unsigned integer value type
    FLOAT_NUMBER,     //!< float number value type
    STRING,           //!< string value type
};

} // namespace fkyaml

#endif /* FK_YAML_NODE_TYPE_HPP_ */