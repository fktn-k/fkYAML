/**
 * NodeClassTest.hpp - declaration of test functions for the Node class
 * 
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#ifndef FK_YAML_TEST_NODE_CLASS_TEST_HPP_
#define FK_YAML_TEST_NODE_CLASS_TEST_HPP_

class NodeClassTest
{
public:
    static int DefaultCtorTest();
    static int SequenceNodeFactoryTest();
    static int MappingNodeFactoryTest();
    static int BoolNodeFactoryTest();
    static int SignedIntNodeFactoryTest();
    static int UnsignedIntNodeFactoryTest();
    static int FloatNumberNodeFactoryTest();
    static int StringNodeFactoryTest();
    static int SequenceForLoopTest();
    static int ConstSequenceForLoopTest();
};

#endif /* FK_YAML_TEST_NODE_CLASS_TEST_HPP_ */
