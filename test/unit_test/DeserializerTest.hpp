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
    static int DeserializeFlowSequenceObject1();
    static int DeserializeFlowMappingObject1();
    static int DeserializeBlockSequenceObject1();
    static int DeserializeBlockSequenceObject2();
    static int DeserializeBlockMappingObject1();
};

#endif /* FK_YAML_TEST_DESERIALIZER_TEST_HPP_ */