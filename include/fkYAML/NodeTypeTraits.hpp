/**
 * @file NodeTypeTraits.hpp
 * @brief Implementation of YAML node data type traits.
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
    typename BooleanType, typename SignedIntegerType, typename UnsignedIntegerType, typename FloatNumberType,
    typename StringType>
class BasicNode;

template <typename T>
struct IsBasicNode : std::false_type
{
};

template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename SignedIntegerType, typename UnsignedIntegerType, typename FloatNumberType,
    typename StringType>
struct IsBasicNode<BasicNode<
    SequenceType, MappingType, BooleanType, SignedIntegerType, UnsignedIntegerType, FloatNumberType, StringType>>
    : std::true_type
{
};

FK_YAML_NAMESPACE_END

#endif /* FK_YAML_NODE_TYPE_TRAITS_HPP_ */