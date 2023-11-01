//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.1.3
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#include <fkYAML/detail/iterator.hpp>
#include <fkYAML/node.hpp>

TEST_CASE("IteratorClassTest_SequenceCtorTest", "[IteratorClassTest]")
{
    fkyaml::node sequence = fkyaml::node::sequence();
    fkyaml::detail::iterator<fkyaml::node> iterator(
        fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
}

TEST_CASE("IteratorClassTest_MappingCtorTest", "[IteratorClassTest]")
{
    fkyaml::node mapping = fkyaml::node::mapping();
    fkyaml::detail::iterator<fkyaml::node> iterator(
        fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
}

TEST_CASE("IteratorClassTest_SequenceCopyCtorTest", "[IteratorClassTest]")
{
    fkyaml::node sequence = fkyaml::node::sequence({fkyaml::node()});
    fkyaml::detail::iterator<fkyaml::node> copied(
        fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
    fkyaml::detail::iterator<fkyaml::node> iterator(copied);
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
    REQUIRE(iterator->is_null());
}

TEST_CASE("IteratorClassTest_MappingCopyCtorTest", "[IteratorClassTest]")
{
    fkyaml::node mapping = fkyaml::node::mapping({{"test", fkyaml::node()}});
    fkyaml::detail::iterator<fkyaml::node> copied(
        fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
    fkyaml::detail::iterator<fkyaml::node> iterator(copied);
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
    REQUIRE(iterator.key().compare("test") == 0);
    REQUIRE(iterator.value().is_null());
}

TEST_CASE("IteratorClassTest_SequenceMoveCtorTest", "[IteratorClassTest]")
{
    fkyaml::node sequence = {"test"};
    fkyaml::detail::iterator<fkyaml::node> moved(
        fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
    fkyaml::detail::iterator<fkyaml::node> iterator(std::move(moved));
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
    REQUIRE(iterator->is_string());
    REQUIRE(iterator->get_value_ref<fkyaml::node::string_type&>().compare("test") == 0);
}

TEST_CASE("IteratorClassTest_MappingMoveCtorTest", "[IteratorClassTest]")
{
    fkyaml::node mapping = fkyaml::node::mapping({{"test", fkyaml::node()}});
    fkyaml::detail::iterator<fkyaml::node> moved(
        fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
    fkyaml::detail::iterator<fkyaml::node> iterator(std::move(moved));
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
    REQUIRE(iterator.key().compare("test") == 0);
    REQUIRE(iterator.value().is_null());
}

TEST_CASE("IteratorClassTest_AssignmentOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test self assignment.")
    {
        fkyaml::node sequence = fkyaml::node::sequence({fkyaml::node()});
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());

        SECTION("Test lvalue iterator.")
        {
            iterator = iterator;
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_null());
        }

        SECTION("Test rvalue iterator.")
        {
            iterator = std::move(iterator);
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_null());
        }
    }

    SECTION("Test sequence iterators.")
    {
        fkyaml::node copied_seq = {"test"};
        fkyaml::detail::iterator<fkyaml::node> copied_itr(
            fkyaml::detail::sequence_iterator_tag {}, copied_seq.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::node sequence = {false};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());

        SECTION("Test lvalue iterator.")
        {
            iterator = copied_itr;
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_string());
            REQUIRE(iterator->get_value_ref<fkyaml::node::string_type&>().compare("test") == 0);
        }

        SECTION("Test rvalue iterator.")
        {
            iterator = std::move(copied_itr);
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_string());
            REQUIRE(iterator->get_value_ref<fkyaml::node::string_type&>().compare("test") == 0);
        }
    }

    SECTION("Test mapping iterators.")
    {
        fkyaml::node copied_map = {{"key", "test"}};
        fkyaml::detail::iterator<fkyaml::node> copied_itr(
            fkyaml::detail::mapping_iterator_tag {}, copied_map.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::node map = {{"foo", false}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, map.get_value_ref<fkyaml::node::mapping_type&>().begin());

        SECTION("Test lvalue iterator.")
        {
            iterator = copied_itr;
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
            REQUIRE(iterator.key().compare("key") == 0);
            REQUIRE(iterator.value().is_string());
            REQUIRE(iterator.value().get_value_ref<fkyaml::node::string_type&>().compare("test") == 0);
        }

        SECTION("Test rvalue iterator.")
        {
            iterator = std::move(copied_itr);
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
            REQUIRE(iterator.key().compare("key") == 0);
            REQUIRE(iterator.value().is_string());
            REQUIRE(iterator.value().get_value_ref<fkyaml::node::string_type&>().compare("test") == 0);
        }
    }
}

TEST_CASE("IteratorClassTest_ArrowOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node seq = {"test"};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, seq.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE(iterator.operator->() == &(seq.get_value_ref<fkyaml::node::sequence_type&>().operator[](0)));
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node map = {{"key", "test"}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, map.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE(iterator.operator->() == &(map.get_value_ref<fkyaml::node::mapping_type&>().operator[]("key")));
    }
}

TEST_CASE("IteratorClassTest_DereferenceOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node seq = {"test"};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, seq.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE(&(iterator.operator*()) == &(seq.get_value_ref<fkyaml::node::sequence_type&>().operator[](0)));
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node map = fkyaml::node::mapping({{"key", "test"}});
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, map.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE(&(iterator.operator*()) == &(map.get_value_ref<fkyaml::node::mapping_type&>().operator[]("key")));
    }
}

TEST_CASE("IteratorClassTest_CompoundAssignmentOperatorBySumTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        iterator += 1;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        iterator += 1;
        REQUIRE(iterator.key().compare("test1") == 0);
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("IteratorClassTest_PlusOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> after_plus_itr = iterator + 1;
        REQUIRE(after_plus_itr->is_boolean());
        REQUIRE(after_plus_itr->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> after_plus_itr = iterator + 1;
        REQUIRE(after_plus_itr.key().compare("test1") == 0);
        REQUIRE(after_plus_itr.value().is_boolean());
        REQUIRE(after_plus_itr.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("IteratorClassTest_PreIncrementOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        ++iterator;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        ++iterator;
        REQUIRE(iterator.key().compare("test1") == 0);
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("IteratorClassTest_PostIncrementOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        iterator++;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        iterator++;
        REQUIRE(iterator.key().compare("test1") == 0);
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("IteratorClassTest_CompoundAssignmentOperatorByDifferenceTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().end());
        iterator -= 1;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().end());
        iterator -= 1;
        REQUIRE(iterator.key().compare("test1") == 0);
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("IteratorClassTest_MinusOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().end());
        fkyaml::detail::iterator<fkyaml::node> after_minus_itr = iterator - 1;
        REQUIRE(after_minus_itr->is_boolean());
        REQUIRE(after_minus_itr->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().end());
        fkyaml::detail::iterator<fkyaml::node> after_minus_itr = iterator - 1;
        REQUIRE(after_minus_itr.key().compare("test1") == 0);
        REQUIRE(after_minus_itr.value().is_boolean());
        REQUIRE(after_minus_itr.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("IteratorClassTest_PreDecrementOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().end());
        --iterator;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().end());
        --iterator;
        REQUIRE(iterator.key().compare("test1") == 0);
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("IteratorClassTest_PostDecrementOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().end());
        iterator--;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().end());
        iterator--;
        REQUIRE(iterator.key().compare("test1") == 0);
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("IteratorClassTest_EqualToOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE(lhs == rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE(lhs == rhs);
    }

    SECTION("Test equality check between different type iterators.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs == rhs, fkyaml::exception);
    }
}

TEST_CASE("IteratorClassTest_NotEqualToOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        ++rhs;
        REQUIRE(lhs != rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        ++rhs;
        REQUIRE(lhs != rhs);
    }

    SECTION("Test equality check between different type iterators.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs == rhs, fkyaml::exception);
    }
}

TEST_CASE("IteratorClassTest_LessThanOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE_FALSE(lhs < rhs);
        ++rhs;
        REQUIRE(lhs < rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs < rhs, fkyaml::exception);
    }

    SECTION("Test less-than check between different type iterators.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs < rhs, fkyaml::exception);
    }
}

TEST_CASE("IteratorClassTest_LessThanOrEqualToOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        ++lhs;
        REQUIRE_FALSE(lhs <= rhs);
        --lhs;
        REQUIRE(lhs <= rhs);
        ++rhs;
        REQUIRE(lhs < rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs <= rhs, fkyaml::exception);
    }

    SECTION("Test less-than-or-equal-to check between different type iterators.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs <= rhs, fkyaml::exception);
    }
}

TEST_CASE("IteratorClassTest_GreaterThanOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE_FALSE(lhs > rhs);
        ++lhs;
        REQUIRE(lhs > rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs > rhs, fkyaml::exception);
    }

    SECTION("Test greater-than check between different type iterators.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs > rhs, fkyaml::exception);
    }
}

TEST_CASE("IteratorClassTest_GreaterThanOrEqualToOperatorTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        ++rhs;
        REQUIRE_FALSE(lhs >= rhs);
        --rhs;
        REQUIRE(lhs >= rhs);
        ++lhs;
        REQUIRE(lhs >= rhs);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs >= rhs, fkyaml::exception);
    }

    SECTION("Test greater-than-or-equal-to check between different type iterators.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs >= rhs, fkyaml::exception);
    }
}

TEST_CASE("IteratorClassTest_TypeGetterTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
    }
}

TEST_CASE("IteratorClassTest_KeyGetterTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE_THROWS_AS(iterator.key(), fkyaml::exception);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_NOTHROW(iterator.key());
        REQUIRE(iterator.key().compare("test0") == 0);
    }
}

TEST_CASE("IteratorClassTest_ValueGetterTest", "[IteratorClassTest]")
{
    SECTION("Test sequence iterator.")
    {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == false);
    }

    SECTION("Test mapping iterator.")
    {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == false);
    }
}
