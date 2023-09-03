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

namespace fkyaml
{

// forward declaration for fkyaml::BasicNode<...>
template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename SignedIntegerType, typename UnsignedIntegerType, typename FloatNumberType,
    typename StringType>
class BasicNode;

template <typename T>
struct is_basic_node : std::false_type
{
};

template <
    template <typename, typename...> class SequenceType, template <typename, typename, typename...> class MappingType,
    typename BooleanType, typename SignedIntegerType, typename UnsignedIntegerType, typename FloatNumberType,
    typename StringType>
struct is_basic_node<BasicNode<
    SequenceType, MappingType, BooleanType, SignedIntegerType, UnsignedIntegerType, FloatNumberType, StringType>>
    : std::true_type
{
};

} // namespace fkyaml

#endif /* FK_YAML_NODE_TYPE_TRAITS_HPP_ */