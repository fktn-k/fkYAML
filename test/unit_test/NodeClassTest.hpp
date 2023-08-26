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
    static int SequenceAliasTypeTest();
    static int MappingTypeTest();
    static int MappingAliasTypeTest();
    static int NullObjectTypeTest();
    static int NullObjectAliasTypeTest();
    static int BooleanTypeTest();
    static int BooleanAliasTypeTest();
    static int SignedIntegerTypeTest();
    static int SignedIntegerAliasTypeTest();
    static int UnsignedIntegerTypeTest();
    static int UnsignedIntegerAliasTypeTest();
    static int FloatNumberTypeTest();
    static int FloatNumberAliasTypeTest();
    static int StringTypeTest();
    static int StringAliasTypeTest();
    // type property checkers
    static int IsSequenceTest();
    static int IsSequenceAliasTest();
    static int IsMappingTest();
    static int IsMappingAliasTest();
    static int IsNullTest();
    static int IsNullAliasTest();
    static int IsBooleanTest();
    static int IsBooleanAliasTest();
    static int IsSignedIntegerTest();
    static int IsSignedIntegerAliasTest();
    static int IsUnsignedIntegerTest();
    static int IsUnsignedIntegerAliasTest();
    static int IsFloatNumberTest();
    static int IsFloatNumberAliasTest();
    static int IsStringTest();
    static int IsStringAliasTest();
    static int IsScalarTest();
    static int IsScalarAliasTest();
    static int IsAliasTest();
    // empty checker
    static int SequenceIsEmptyTest();
    static int SequenceAliasIsEmptyTest();
    static int MappingIsEmptyTest();
    static int MappingAliasIsEmptyTest();
    static int StringIsEmptyTest();
    static int StringAliasIsEmptyTest();
    static int IsEmptyThrowExceptionTest();
    static int AliasIsEmptyThrowExceptionTest();
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
    static int ToSequenceAliasTest();
    static int ConstToSequenceTest();
    static int ConstToSequenceAliasTest();
    static int ToSequenceThrowExceptionTest();
    static int ToSequenceAliasThrowExceptionTest();
    static int ToMappingTest();
    static int ToMappingAliasTest();
    static int ConstToMappingTest();
    static int ConstToMappingAliasTest();
    static int ToMappingThrowExceptionTest();
    static int ToMappingAliasThrowExceptionTest();
    static int ToBooleanTest();
    static int ToBooleanAliasTest();
    static int ConstToBooleanTest();
    static int ConstToBooleanAliasTest();
    static int ToBooleanThrowExceptionTest();
    static int ToBooleanAliasThrowExceptionTest();
    static int ToSignedIntegerTest();
    static int ToSignedIntegerAliasTest();
    static int ConstToSignedIntegerTest();
    static int ConstToSignedIntegerAliasTest();
    static int ToSignedIntegerThrowExceptionTest();
    static int ToSignedIntegerAliasThrowExceptionTest();
    static int ToUnsignedIntegerTest();
    static int ToUnsignedIntegerAliasTest();
    static int ConstToUnsignedIntegerTest();
    static int ConstToUnsignedIntegerAliasTest();
    static int ToUnsignedIntegerThrowExceptionTest();
    static int ToUnsignedIntegerAliasThrowExceptionTest();
    static int ToFloatNumberTest();
    static int ToFloatNumberAliasTest();
    static int ConstToFloatNumberTest();
    static int ConstToFloatNumberAliasTest();
    static int ToFloatNumberThrowExceptionTest();
    static int ToFloatNumberAliasThrowExceptionTest();
    static int ToStringTest();
    static int ToStringAliasTest();
    static int ConstToStringTest();
    static int ConstToStringAliasTest();
    static int ToStringThrowExceptionTest();
    static int ToStringAliasThrowExceptionTest();
    // swap
    static int SwapTest();
    static int StdSwapTest();
    // for-loops
    static int SequenceForLoopTest();
    static int ConstSequenceForLoopTest();
};

#endif /* FK_YAML_TEST_NODE_CLASS_TEST_HPP_ */
