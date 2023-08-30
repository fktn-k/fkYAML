/**
 * IteratorClassTest.cpp - implementation of test functions for the Iterator class
 *
 * Copyright (c) 2023 fktn
 * Distributed under the MIT License (https://opensource.org/licenses/MIT)
 */

#include "catch2/catch.hpp"

#include "fkYAML/Iterator.hpp"
#include "fkYAML/Node.hpp"

TEST_CASE("IteratorClassTest_SequenceCtorTest", "[IteratorClassTest]")
{
    fkyaml::Node sequence = fkyaml::Node::Sequence();
    fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
    REQUIRE(iterator.Type() == fkyaml::IteratorType::SEQUENCE);
}

TEST_CASE("IteratorClassTest_MappingCtorTest", "[IteratorClassTest]")
{
    fkyaml::Node mapping = fkyaml::Node::Mapping();
    fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
    REQUIRE(iterator.Type() == fkyaml::IteratorType::MAPPING);
}

TEST_CASE("IteratorClassTest_SequenceCopyCtorTest", "[IteratorClassTest]")
{
    fkyaml::Node sequence = fkyaml::Node::Sequence({fkyaml::Node()});
    fkyaml::Iterator<fkyaml::Node> copied(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
    fkyaml::Iterator<fkyaml::Node> iterator(copied);
    REQUIRE(iterator.Type() == fkyaml::IteratorType::SEQUENCE);
    REQUIRE(iterator->IsNull());
}

TEST_CASE("IteratorClassTest_MappingCopyCtorTest", "[IteratorClassTest]")
{
    fkyaml::Node mapping = fkyaml::Node::Mapping({{"test", fkyaml::Node()}});
    fkyaml::Iterator<fkyaml::Node> copied(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
    fkyaml::Iterator<fkyaml::Node> iterator(copied);
    REQUIRE(iterator.Type() == fkyaml::IteratorType::MAPPING);
    REQUIRE(iterator.Key().compare("test") == 0);
    REQUIRE(iterator.Value().IsNull());
}

TEST_CASE("IteratorClassTest_SequenceMoveCtorTest", "[IteratorClassTest]")
{
    fkyaml::Node sequence = fkyaml::Node::Sequence({fkyaml::Node::StringScalar("test")});
    fkyaml::Iterator<fkyaml::Node> moved(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
    fkyaml::Iterator<fkyaml::Node> iterator(std::move(moved));
    REQUIRE(iterator.Type() == fkyaml::IteratorType::SEQUENCE);
    REQUIRE(iterator->IsString());
    REQUIRE(iterator->ToString().compare("test") == 0);
}

TEST_CASE("IteratorClassTest_MappingMoveCtorTest", "[IteratorClassTest]")
{
    fkyaml::Node mapping = fkyaml::Node::Mapping({{"test", fkyaml::Node()}});
    fkyaml::Iterator<fkyaml::Node> moved(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
    fkyaml::Iterator<fkyaml::Node> iterator(std::move(moved));
    REQUIRE(iterator.Type() == fkyaml::IteratorType::MAPPING);
    REQUIRE(iterator.Key().compare("test") == 0);
    REQUIRE(iterator.Value().IsNull());
}

TEST_CASE("IteratorClassTest_AssignmentOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test self assignment.")
    {
        fkyaml::Node sequence = fkyaml::Node::Sequence({fkyaml::Node()});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());

        SECTION("Test lvalue iterator.")
        {
            iterator = iterator;
            REQUIRE(iterator.Type() == fkyaml::IteratorType::SEQUENCE);
            REQUIRE(iterator->IsNull());
        }

        SECTION("Test rvalue iterator.")
        {
            iterator = std::move(iterator);
            REQUIRE(iterator.Type() == fkyaml::IteratorType::SEQUENCE);
            REQUIRE(iterator->IsNull());
        }
    }

    SECTION("Test sequence iterators.")
    {
        fkyaml::Node copied_seq = fkyaml::Node::Sequence({fkyaml::Node::StringScalar("test")});
        fkyaml::Iterator<fkyaml::Node> copied_itr(fkyaml::SequenceIteratorTag {}, copied_seq.ToSequence().begin());
        fkyaml::Node sequence = fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false)});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());

        SECTION("Test lvalue iterator.")
        {
            iterator = copied_itr;
            REQUIRE(iterator.Type() == fkyaml::IteratorType::SEQUENCE);
            REQUIRE(iterator->IsString());
            REQUIRE(iterator->ToString().compare("test") == 0);
        }

        SECTION("Test rvalue iterator.")
        {
            iterator = std::move(copied_itr);
            REQUIRE(iterator.Type() == fkyaml::IteratorType::SEQUENCE);
            REQUIRE(iterator->IsString());
            REQUIRE(iterator->ToString().compare("test") == 0);
        }
    }

    SECTION("Test mapping iterators.")
    {
        fkyaml::Node copied_map = fkyaml::Node::Mapping({{"key", fkyaml::Node::StringScalar("test")}});
        fkyaml::Iterator<fkyaml::Node> copied_itr(fkyaml::MappingIteratorTag {}, copied_map.ToMapping().begin());
        fkyaml::Node map = fkyaml::Node::Mapping({{"foo", fkyaml::Node::BooleanScalar(false)}});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::MappingIteratorTag {}, map.ToMapping().begin());

        SECTION("Test lvalue iterator.")
        {
            iterator = copied_itr;
            REQUIRE(iterator.Type() == fkyaml::IteratorType::MAPPING);
            REQUIRE(iterator.Key().compare("key") == 0);
            REQUIRE(iterator.Value().IsString());
            REQUIRE(iterator.Value().ToString().compare("test") == 0);
        }

        SECTION("Test rvalue iterator.")
        {
            iterator = std::move(copied_itr);
            REQUIRE(iterator.Type() == fkyaml::IteratorType::MAPPING);
            REQUIRE(iterator.Key().compare("key") == 0);
            REQUIRE(iterator.Value().IsString());
            REQUIRE(iterator.Value().ToString().compare("test") == 0);
        }
    }
}

TEST_CASE("IteratorClassTest_ArrowOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node seq = fkyaml::Node::Sequence({fkyaml::Node::StringScalar("test")});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, seq.ToSequence().begin());
        REQUIRE(iterator.operator->() == &(seq.ToSequence().operator[](0)));
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node map = fkyaml::Node::Mapping({{"key", fkyaml::Node::StringScalar("test")}});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::MappingIteratorTag {}, map.ToMapping().begin());
        REQUIRE(iterator.operator->() == &(map.ToMapping().operator[]("key")));
    }
}

TEST_CASE("IteratorClassTest_DereferenceOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node seq = fkyaml::Node::Sequence({fkyaml::Node::StringScalar("test")});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, seq.ToSequence().begin());
        REQUIRE(&(iterator.operator*()) == &(seq.ToSequence().operator[](0)));
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node map = fkyaml::Node::Mapping({{"key", fkyaml::Node::StringScalar("test")}});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::MappingIteratorTag {}, map.ToMapping().begin());
        REQUIRE(&(iterator.operator*()) == &(map.ToMapping().operator[]("key")));
    }
}

TEST_CASE("IteratorClassTest_CompoundAssignmentOperatorBySumTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        iterator += 1;
        REQUIRE(iterator->IsBoolean());
        REQUIRE(iterator->ToBoolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        iterator += 1;
        REQUIRE(iterator.Key().compare("test1") == 0);
        REQUIRE(iterator.Value().IsBoolean());
        REQUIRE(iterator.Value().ToBoolean() == true);
    }
}

TEST_CASE("IteratorClassTest_PlusOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        fkyaml::Iterator<fkyaml::Node> after_plus_itr = iterator + 1;
        REQUIRE(after_plus_itr->IsBoolean());
        REQUIRE(after_plus_itr->ToBoolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        fkyaml::Iterator<fkyaml::Node> after_plus_itr = iterator + 1;
        REQUIRE(after_plus_itr.Key().compare("test1") == 0);
        REQUIRE(after_plus_itr.Value().IsBoolean());
        REQUIRE(after_plus_itr.Value().ToBoolean() == true);
    }
}

TEST_CASE("IteratorClassTest_PreIncrementOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        ++iterator;
        REQUIRE(iterator->IsBoolean());
        REQUIRE(iterator->ToBoolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        ++iterator;
        REQUIRE(iterator.Key().compare("test1") == 0);
        REQUIRE(iterator.Value().IsBoolean());
        REQUIRE(iterator.Value().ToBoolean() == true);
    }
}

TEST_CASE("IteratorClassTest_PostIncrementOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        iterator++;
        REQUIRE(iterator->IsBoolean());
        REQUIRE(iterator->ToBoolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        iterator++;
        REQUIRE(iterator.Key().compare("test1") == 0);
        REQUIRE(iterator.Value().IsBoolean());
        REQUIRE(iterator.Value().ToBoolean() == true);
    }
}

// FIXME

TEST_CASE("IteratorClassTest_CompoundAssignmentOperatorByDifferenceTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().end());
        iterator -= 1;
        REQUIRE(iterator->IsBoolean());
        REQUIRE(iterator->ToBoolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::MappingIteratorTag {}, mapping.ToMapping().end());
        iterator -= 1;
        REQUIRE(iterator.Key().compare("test1") == 0);
        REQUIRE(iterator.Value().IsBoolean());
        REQUIRE(iterator.Value().ToBoolean() == true);
    }
}

TEST_CASE("IteratorClassTest_MinusOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().end());
        fkyaml::Iterator<fkyaml::Node> after_minus_itr = iterator - 1;
        REQUIRE(after_minus_itr->IsBoolean());
        REQUIRE(after_minus_itr->ToBoolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::MappingIteratorTag {}, mapping.ToMapping().end());
        fkyaml::Iterator<fkyaml::Node> after_minus_itr = iterator - 1;
        REQUIRE(after_minus_itr.Key().compare("test1") == 0);
        REQUIRE(after_minus_itr.Value().IsBoolean());
        REQUIRE(after_minus_itr.Value().ToBoolean() == true);
    }
}

TEST_CASE("IteratorClassTest_PreDecrementOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().end());
        --iterator;
        REQUIRE(iterator->IsBoolean());
        REQUIRE(iterator->ToBoolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::MappingIteratorTag {}, mapping.ToMapping().end());
        --iterator;
        REQUIRE(iterator.Key().compare("test1") == 0);
        REQUIRE(iterator.Value().IsBoolean());
        REQUIRE(iterator.Value().ToBoolean() == true);
    }
}

TEST_CASE("IteratorClassTest_PostDecrementOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().end());
        iterator--;
        REQUIRE(iterator->IsBoolean());
        REQUIRE(iterator->ToBoolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::MappingIteratorTag {}, mapping.ToMapping().end());
        iterator--;
        REQUIRE(iterator.Key().compare("test1") == 0);
        REQUIRE(iterator.Value().IsBoolean());
        REQUIRE(iterator.Value().ToBoolean() == true);
    }
}

TEST_CASE("IteratorClassTest_EqualToOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        REQUIRE(lhs == rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        REQUIRE(lhs == rhs);
    }

    SECTION("Test equality check between different type iterators.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        REQUIRE_THROWS_AS(lhs == rhs, fkyaml::Exception);
    }
}

TEST_CASE("IteratorClassTest_NotEqualToOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        ++rhs;
        REQUIRE(lhs != rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        ++rhs;
        REQUIRE(lhs != rhs);
    }

    SECTION("Test equality check between different type iterators.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        REQUIRE_THROWS_AS(lhs == rhs, fkyaml::Exception);
    }
}

TEST_CASE("IteratorClassTest_LessThanOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        REQUIRE_FALSE(lhs < rhs);
        ++rhs;
        REQUIRE(lhs < rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        REQUIRE_THROWS_AS(lhs < rhs, fkyaml::Exception);
    }

    SECTION("Test less-than check between different type iterators.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        REQUIRE_THROWS_AS(lhs < rhs, fkyaml::Exception);
    }
}

TEST_CASE("IteratorClassTest_LessThanOrEqualToOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        ++lhs;
        REQUIRE_FALSE(lhs <= rhs);
        --lhs;
        REQUIRE(lhs <= rhs);
        ++rhs;
        REQUIRE(lhs < rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        REQUIRE_THROWS_AS(lhs <= rhs, fkyaml::Exception);
    }

    SECTION("Test less-than-or-equal-to check between different type iterators.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        REQUIRE_THROWS_AS(lhs <= rhs, fkyaml::Exception);
    }
}

TEST_CASE("IteratorClassTest_GreaterThanOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        REQUIRE_FALSE(lhs > rhs);
        ++lhs;
        REQUIRE(lhs > rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        REQUIRE_THROWS_AS(lhs > rhs, fkyaml::Exception);
    }

    SECTION("Test greater-than check between different type iterators.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        REQUIRE_THROWS_AS(lhs > rhs, fkyaml::Exception);
    }
}

TEST_CASE("IteratorClassTest_GreaterThanOrEqualToOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        ++rhs;
        REQUIRE_FALSE(lhs >= rhs);
        --rhs;
        REQUIRE(lhs >= rhs);
        ++lhs;
        REQUIRE(lhs >= rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        REQUIRE_THROWS_AS(lhs >= rhs, fkyaml::Exception);
    }

    SECTION("Test greater-than-or-equal-to check between different type iterators.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> lhs(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> rhs(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        REQUIRE_THROWS_AS(lhs >= rhs, fkyaml::Exception);
    }
}

TEST_CASE("IteratorClassTest_TypeGetterTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        REQUIRE(iterator.Type() == fkyaml::IteratorType::SEQUENCE);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        REQUIRE(iterator.Type() == fkyaml::IteratorType::MAPPING);
    }
}

TEST_CASE("IteratorClassTest_KeyGetterTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        REQUIRE_THROWS_AS(iterator.Key(), fkyaml::Exception);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        REQUIRE_NOTHROW(iterator.Key());
        REQUIRE(iterator.Key().compare("test0") == 0);
    }
}

TEST_CASE("IteratorClassTest_ValueGetterTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::Node sequence =
            fkyaml::Node::Sequence({fkyaml::Node::BooleanScalar(false), fkyaml::Node::BooleanScalar(true)});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::SequenceIteratorTag {}, sequence.ToSequence().begin());
        REQUIRE(iterator.Value().IsBoolean());
        REQUIRE(iterator.Value().ToBoolean() == false);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::Node mapping = fkyaml::Node::Mapping(
            {{"test0", fkyaml::Node::BooleanScalar(false)}, {"test1", fkyaml::Node::BooleanScalar(true)}});
        fkyaml::Iterator<fkyaml::Node> iterator(fkyaml::MappingIteratorTag {}, mapping.ToMapping().begin());
        REQUIRE(iterator.Value().IsBoolean());
        REQUIRE(iterator.Value().ToBoolean() == false);
    }
}
