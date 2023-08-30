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
    fkyaml::Iterator<fkyaml::Node> iterator(moved);
    REQUIRE(iterator.Type() == fkyaml::IteratorType::MAPPING);
    REQUIRE(iterator.Key().compare("test") == 0);
    REQUIRE(iterator.Value().IsNull());
}

TEST_CASE("IteratorClassTest_AssignmentOperatorTest", "[IteratorClassTest]")
{
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
