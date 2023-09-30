/**
 *   __ _  __     __      __  __ _
 *  / _| | \ \   / //\   |  \/  | |
 * | |_| | _\ \_/ //  \  | \  / | |
 * |  _| |/ /\   // /\ \ | |\/| | |
 * | | |   <  | |/ ____ \| |  | | |____
 * |_| |_|\_\ |_/_/    \_\_|  |_|______|
 *
 * @file NodeTypeTraits.hpp
 * @brief Implementation of YAML node data type traits.
 * @version 0.0.0
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_NODE_TYPE_TRAITS_HPP_
#define FK_YAML_NODE_TYPE_TRAITS_HPP_

#include <type_traits>

#include "fkYAML/VersioningMacros.hpp"

/**
 * @namespace fkyaml
 * @brief namespace for fkYAML library.
 */
FK_YAML_NAMESPACE_BEGIN

// forward declaration for fkyaml::BasicNode<...>
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType>
class BasicNode;

/**
 * @struct IsBasicNode
 * @brief A struct to check the template parameter class is a kind of BasicNode template class.
 *
 * @tparam T A class to be checked if it's a kind of BasicNode template class.
 */
template <typename T>
struct IsBasicNode : std::false_type
{
};

/**
 * @brief A partial specialization of IsBasicNode for BasicNode template class.
 *
 * @tparam SequenceType A type for sequence node value containers.
 * @tparam MappingType A type for mapping node value containers.
 * @tparam BooleanType A type for boolean node values.
 * @tparam IntegerType A type for integer node values.
 * @tparam FloatNumberType A type for float number node values.
 * @tparam StringType A type for string node values.
 */
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename IntegerType, typename FloatNumberType, typename StringType>
struct IsBasicNode<BasicNode<SequenceType, MappingType, BooleanType, IntegerType, FloatNumberType, StringType>>
    : std::true_type
{
};

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_NODE_TYPE_TRAITS_HPP_ */
