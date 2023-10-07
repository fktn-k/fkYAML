//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.0.1
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include "catch2/catch.hpp"

#include "fkYAML/iterator.hpp"
#include "fkYAML/node.hpp"

TEST_CASE("IteratorClassTest_SequenceCtorTest", "[IteratorClassTest]")
{
    fkyaml::node sequence = fkyaml::node::sequence();
    fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
    REQUIRE(iterator.type() == fkyaml::iterator_t::SEQUENCE);
}

TEST_CASE("IteratorClassTest_MappingCtorTest", "[IteratorClassTest]")
{
    fkyaml::node mapping = fkyaml::node::mapping();
    fkyaml::iterator<fkyaml::node> iterator(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
    REQUIRE(iterator.type() == fkyaml::iterator_t::MAPPING);
}

TEST_CASE("IteratorClassTest_SequenceCopyCtorTest", "[IteratorClassTest]")
{
    fkyaml::node sequence = fkyaml::node::sequence({fkyaml::node()});
    fkyaml::iterator<fkyaml::node> copied(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
    fkyaml::iterator<fkyaml::node> iterator(copied);
    REQUIRE(iterator.type() == fkyaml::iterator_t::SEQUENCE);
    REQUIRE(iterator->is_null());
}

TEST_CASE("IteratorClassTest_MappingCopyCtorTest", "[IteratorClassTest]")
{
    fkyaml::node mapping = fkyaml::node::mapping({{"test", fkyaml::node()}});
    fkyaml::iterator<fkyaml::node> copied(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
    fkyaml::iterator<fkyaml::node> iterator(copied);
    REQUIRE(iterator.type() == fkyaml::iterator_t::MAPPING);
    REQUIRE(iterator.key().compare("test") == 0);
    REQUIRE(iterator.value().is_null());
}

TEST_CASE("IteratorClassTest_SequenceMoveCtorTest", "[IteratorClassTest]")
{
    fkyaml::node sequence = fkyaml::node::sequence({fkyaml::node::string_scalar("test")});
    fkyaml::iterator<fkyaml::node> moved(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
    fkyaml::iterator<fkyaml::node> iterator(std::move(moved));
    REQUIRE(iterator.type() == fkyaml::iterator_t::SEQUENCE);
    REQUIRE(iterator->is_string());
    REQUIRE(iterator->to_string().compare("test") == 0);
}

TEST_CASE("IteratorClassTest_MappingMoveCtorTest", "[IteratorClassTest]")
{
    fkyaml::node mapping = fkyaml::node::mapping({{"test", fkyaml::node()}});
    fkyaml::iterator<fkyaml::node> moved(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
    fkyaml::iterator<fkyaml::node> iterator(std::move(moved));
    REQUIRE(iterator.type() == fkyaml::iterator_t::MAPPING);
    REQUIRE(iterator.key().compare("test") == 0);
    REQUIRE(iterator.value().is_null());
}

TEST_CASE("IteratorClassTest_AssignmentOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test self assignment.")
    {
        fkyaml::node sequence = fkyaml::node::sequence({fkyaml::node()});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());

        SECTION("Test lvalue iterator.")
        {
            iterator = iterator;
            REQUIRE(iterator.type() == fkyaml::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_null());
        }

        SECTION("Test rvalue iterator.")
        {
            iterator = std::move(iterator);
            REQUIRE(iterator.type() == fkyaml::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_null());
        }
    }

    SECTION("Test sequence iterators.")
    {
        fkyaml::node copied_seq = fkyaml::node::sequence({fkyaml::node::string_scalar("test")});
        fkyaml::iterator<fkyaml::node> copied_itr(fkyaml::sequence_iterator_tag {}, copied_seq.to_sequence().begin());
        fkyaml::node sequence = fkyaml::node::sequence({fkyaml::node::boolean_scalar(false)});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());

        SECTION("Test lvalue iterator.")
        {
            iterator = copied_itr;
            REQUIRE(iterator.type() == fkyaml::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_string());
            REQUIRE(iterator->to_string().compare("test") == 0);
        }

        SECTION("Test rvalue iterator.")
        {
            iterator = std::move(copied_itr);
            REQUIRE(iterator.type() == fkyaml::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_string());
            REQUIRE(iterator->to_string().compare("test") == 0);
        }
    }

    SECTION("Test mapping iterators.")
    {
        fkyaml::node copied_map = fkyaml::node::mapping({{"key", fkyaml::node::string_scalar("test")}});
        fkyaml::iterator<fkyaml::node> copied_itr(fkyaml::mapping_iterator_tag {}, copied_map.to_mapping().begin());
        fkyaml::node map = fkyaml::node::mapping({{"foo", fkyaml::node::boolean_scalar(false)}});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::mapping_iterator_tag {}, map.to_mapping().begin());

        SECTION("Test lvalue iterator.")
        {
            iterator = copied_itr;
            REQUIRE(iterator.type() == fkyaml::iterator_t::MAPPING);
            REQUIRE(iterator.key().compare("key") == 0);
            REQUIRE(iterator.value().is_string());
            REQUIRE(iterator.value().to_string().compare("test") == 0);
        }

        SECTION("Test rvalue iterator.")
        {
            iterator = std::move(copied_itr);
            REQUIRE(iterator.type() == fkyaml::iterator_t::MAPPING);
            REQUIRE(iterator.key().compare("key") == 0);
            REQUIRE(iterator.value().is_string());
            REQUIRE(iterator.value().to_string().compare("test") == 0);
        }
    }
}

TEST_CASE("IteratorClassTest_ArrowOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node seq = fkyaml::node::sequence({fkyaml::node::string_scalar("test")});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, seq.to_sequence().begin());
        REQUIRE(iterator.operator->() == &(seq.to_sequence().operator[](0)));
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node map = fkyaml::node::mapping({{"key", fkyaml::node::string_scalar("test")}});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::mapping_iterator_tag {}, map.to_mapping().begin());
        REQUIRE(iterator.operator->() == &(map.to_mapping().operator[]("key")));
    }
}

TEST_CASE("IteratorClassTest_DereferenceOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node seq = fkyaml::node::sequence({fkyaml::node::string_scalar("test")});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, seq.to_sequence().begin());
        REQUIRE(&(iterator.operator*()) == &(seq.to_sequence().operator[](0)));
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node map = fkyaml::node::mapping({{"key", fkyaml::node::string_scalar("test")}});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::mapping_iterator_tag {}, map.to_mapping().begin());
        REQUIRE(&(iterator.operator*()) == &(map.to_mapping().operator[]("key")));
    }
}

TEST_CASE("IteratorClassTest_CompoundAssignmentOperatorBySumTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        iterator += 1;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->to_boolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        iterator += 1;
        REQUIRE(iterator.key().compare("test1") == 0);
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().to_boolean() == true);
    }
}

TEST_CASE("IteratorClassTest_PlusOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        fkyaml::iterator<fkyaml::node> after_plus_itr = iterator + 1;
        REQUIRE(after_plus_itr->is_boolean());
        REQUIRE(after_plus_itr->to_boolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        fkyaml::iterator<fkyaml::node> after_plus_itr = iterator + 1;
        REQUIRE(after_plus_itr.key().compare("test1") == 0);
        REQUIRE(after_plus_itr.value().is_boolean());
        REQUIRE(after_plus_itr.value().to_boolean() == true);
    }
}

TEST_CASE("IteratorClassTest_PreIncrementOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        ++iterator;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->to_boolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        ++iterator;
        REQUIRE(iterator.key().compare("test1") == 0);
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().to_boolean() == true);
    }
}

TEST_CASE("IteratorClassTest_PostIncrementOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        iterator++;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->to_boolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        iterator++;
        REQUIRE(iterator.key().compare("test1") == 0);
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().to_boolean() == true);
    }
}

// FIXME

TEST_CASE("IteratorClassTest_CompoundAssignmentOperatorByDifferenceTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().end());
        iterator -= 1;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->to_boolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().end());
        iterator -= 1;
        REQUIRE(iterator.key().compare("test1") == 0);
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().to_boolean() == true);
    }
}

TEST_CASE("IteratorClassTest_MinusOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().end());
        fkyaml::iterator<fkyaml::node> after_minus_itr = iterator - 1;
        REQUIRE(after_minus_itr->is_boolean());
        REQUIRE(after_minus_itr->to_boolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().end());
        fkyaml::iterator<fkyaml::node> after_minus_itr = iterator - 1;
        REQUIRE(after_minus_itr.key().compare("test1") == 0);
        REQUIRE(after_minus_itr.value().is_boolean());
        REQUIRE(after_minus_itr.value().to_boolean() == true);
    }
}

TEST_CASE("IteratorClassTest_PreDecrementOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().end());
        --iterator;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->to_boolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().end());
        --iterator;
        REQUIRE(iterator.key().compare("test1") == 0);
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().to_boolean() == true);
    }
}

TEST_CASE("IteratorClassTest_PostDecrementOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().end());
        iterator--;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->to_boolean() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().end());
        iterator--;
        REQUIRE(iterator.key().compare("test1") == 0);
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().to_boolean() == true);
    }
}

TEST_CASE("IteratorClassTest_EqualToOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        REQUIRE(lhs == rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        REQUIRE(lhs == rhs);
    }

    SECTION("Test equality check between different type iterators.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        REQUIRE_THROWS_AS(lhs == rhs, fkyaml::exception);
    }
}

TEST_CASE("IteratorClassTest_NotEqualToOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        ++rhs;
        REQUIRE(lhs != rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        ++rhs;
        REQUIRE(lhs != rhs);
    }

    SECTION("Test equality check between different type iterators.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        REQUIRE_THROWS_AS(lhs == rhs, fkyaml::exception);
    }
}

TEST_CASE("IteratorClassTest_LessThanOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        REQUIRE_FALSE(lhs < rhs);
        ++rhs;
        REQUIRE(lhs < rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        REQUIRE_THROWS_AS(lhs < rhs, fkyaml::exception);
    }

    SECTION("Test less-than check between different type iterators.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        REQUIRE_THROWS_AS(lhs < rhs, fkyaml::exception);
    }
}

TEST_CASE("IteratorClassTest_LessThanOrEqualToOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        ++lhs;
        REQUIRE_FALSE(lhs <= rhs);
        --lhs;
        REQUIRE(lhs <= rhs);
        ++rhs;
        REQUIRE(lhs < rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        REQUIRE_THROWS_AS(lhs <= rhs, fkyaml::exception);
    }

    SECTION("Test less-than-or-equal-to check between different type iterators.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        REQUIRE_THROWS_AS(lhs <= rhs, fkyaml::exception);
    }
}

TEST_CASE("IteratorClassTest_GreaterThanOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        REQUIRE_FALSE(lhs > rhs);
        ++lhs;
        REQUIRE(lhs > rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        REQUIRE_THROWS_AS(lhs > rhs, fkyaml::exception);
    }

    SECTION("Test greater-than check between different type iterators.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        REQUIRE_THROWS_AS(lhs > rhs, fkyaml::exception);
    }
}

TEST_CASE("IteratorClassTest_GreaterThanOrEqualToOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        ++rhs;
        REQUIRE_FALSE(lhs >= rhs);
        --rhs;
        REQUIRE(lhs >= rhs);
        ++lhs;
        REQUIRE(lhs >= rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        REQUIRE_THROWS_AS(lhs >= rhs, fkyaml::exception);
    }

    SECTION("Test greater-than-or-equal-to check between different type iterators.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> lhs(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> rhs(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        REQUIRE_THROWS_AS(lhs >= rhs, fkyaml::exception);
    }
}

TEST_CASE("IteratorClassTest_TypeGetterTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        REQUIRE(iterator.type() == fkyaml::iterator_t::SEQUENCE);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        REQUIRE(iterator.type() == fkyaml::iterator_t::MAPPING);
    }
}

TEST_CASE("IteratorClassTest_KeyGetterTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        REQUIRE_THROWS_AS(iterator.key(), fkyaml::exception);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        REQUIRE_NOTHROW(iterator.key());
        REQUIRE(iterator.key().compare("test0") == 0);
    }
}

TEST_CASE("IteratorClassTest_ValueGetterTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence =
            fkyaml::node::sequence({fkyaml::node::boolean_scalar(false), fkyaml::node::boolean_scalar(true)});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::sequence_iterator_tag {}, sequence.to_sequence().begin());
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().to_boolean() == false);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = fkyaml::node::mapping(
            {{"test0", fkyaml::node::boolean_scalar(false)}, {"test1", fkyaml::node::boolean_scalar(true)}});
        fkyaml::iterator<fkyaml::node> iterator(fkyaml::mapping_iterator_tag {}, mapping.to_mapping().begin());
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().to_boolean() == false);
    }
}
