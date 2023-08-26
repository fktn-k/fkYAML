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
    // constructors
    static int DefaultCtorTest();
    static int SequenceTypeCtorTest();
    static int MappingTypeCtorTest();
    static int NullTypeCtorTest();
    static int BooleanTypeCtorTest();
    static int SignedIntTypeCtorTest();
    static int UnsignedIntTypeCtorTest();
    static int FloatNumberTypeCtorTest();
    static int StringTypeCtorTest();
    // factory methods
    static int EmptySequenceNodeFactoryTest();
    static int LvalueSequenceNodeFactoryTest();
    static int RvalueSequenceNodeFactoryTest();
    static int EmptyMappingNodeFactoryTest();
    static int LvalueMappingNodeFactoryTest();
    static int RvalueMappingNodeFactoryTest();
    static int BoolNodeFactoryTest();
    static int SignedIntNodeFactoryTest();
    static int UnsignedIntNodeFactoryTest();
    static int FloatNumberNodeFactoryTest();
    static int EmptyStringNodeFactoryTest();
    static int LvalueStringNodeFactoryTest();
    static int RvalueStringNodeFactoryTest();
    static int AliasNodeFactoryTest();
    static int AliasNodeFactoryThrowExceptionTest();
    // subscript operators
    static int LvalueStringSubscriptOperatorTest();
    static int ConstLvalueStringSubscriptOperatorTest();
    static int RvalueStringSubscriptOperatorTest();
    static int ConstRvalueStringSubscriptOperatorTest();
    static int IntegerSubscriptOperatorTest();
    static int ConstIntegerSubscriptOperatorTest();
    // type property getter
    static int SequenceTypeTest();
    static int MappingTypeTest();
    static int NullObjectTypeTest();
    static int BooleanTypeTest();
    static int SignedIntegerTypeTest();
    static int UnsignedIntegerTypeTest();
    static int FloatNumberTypeTest();
    static int StringTypeTest();
    // type property checkers
    static int IsSequenceTest();
    static int IsMappingTest();
    static int IsNullTest();
    static int IsBooleanTest();
    static int IsSignedIntegerTest();
    static int IsUnsignedIntegerTest();
    static int IsFloatNumberTest();
    static int IsStringTest();
    static int IsScalarTest();
    // empty checker
    static int SequenceIsEmptyTest();
    static int MappingIsEmptyTest();
    static int StringIsEmptyTest();
    static int IsEmptyThrowExceptionTest();
    // container size getter
    static int SequenceSizeTest();
    static int MappingSizeTest();
    static int StringSizeTest();
    static int SizeThrowExceptionTest();
    // anchor name property checker/getter/setter/
    static int HasAnchorNameTest();
    static int GetAnchorNameTest();
    static int FirstAddLvalueAnchorNameTest();
    static int FirstAddRvalueAnchorNameTest();
    static int TwiceAddLvalueAnchorNameTest();
    static int TwiceAddRvalueAnchorNameTest();
    // value reference getters
    static int ToSequenceTest();
    static int ConstToSequenceTest();
    static int ToSequenceThrowExceptionTest();
    static int ToMappingTest();
    static int ConstToMappingTest();
    static int ToMappingThrowExceptionTest();
    static int ToBooleanTest();
    static int ConstToBooleanTest();
    static int ToBooleanThrowExceptionTest();
    static int ToSignedIntegerTest();
    static int ConstToSignedIntegerTest();
    static int ToSignedIntegerThrowExceptionTest();
    static int ToUnsignedIntegerTest();
    static int ConstToUnsignedIntegerTest();
    static int ToUnsignedIntegerThrowExceptionTest();
    static int ToFloatNumberTest();
    static int ConstToFloatNumberTest();
    static int ToFloatNumberThrowExceptionTest();
    static int ToStringTest();
    static int ConstToStringTest();
    static int ToStringThrowExceptionTest();
    // swap
    static int SwapTest();
    static int StdSwapTest();
    // for-loops
    static int SequenceForLoopTest();
    static int ConstSequenceForLoopTest();
};

#endif /* FK_YAML_TEST_NODE_CLASS_TEST_HPP_ */
