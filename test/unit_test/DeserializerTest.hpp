/**
 * DeserializerTest.hpp - declaration of test functions for the Deserializer class
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_TEST_DESERIALIZER_TEST_HPP_
#define FK_YAML_TEST_DESERIALIZER_TEST_HPP_

class DeserializerTest
{
public:
    static int DeserializeMappingObject1();
    static int DeserializeNestedFlowSequenceObject1();
    static int DeserializeNestedFlowMappingObject1();
    static int DeserializeNestedBlockSequenceObject1();
    static int DeserializeNestedBlockSequenceObject2();
    static int DeserializeNestedBlockMappingObject1();
};

#endif /* FK_YAML_TEST_DESERIALIZER_TEST_HPP_ */