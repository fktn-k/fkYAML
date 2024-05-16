//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.3.7
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2024 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("Iterator_SequenceCtor") {
    fkyaml::node sequence = fkyaml::node::sequence();
    fkyaml::detail::iterator<fkyaml::node> iterator(
        fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
}

TEST_CASE("Iterator_MappingCtor") {
    fkyaml::node mapping = fkyaml::node::mapping();
    fkyaml::detail::iterator<fkyaml::node> iterator(
        fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
}

TEST_CASE("Iterator_SequenceCopyCtor") {
    fkyaml::node sequence = fkyaml::node::sequence({fkyaml::node()});
    fkyaml::detail::iterator<fkyaml::node> copied(
        fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
    fkyaml::detail::iterator<fkyaml::node> iterator(copied);
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
    REQUIRE(iterator->is_null());
}

TEST_CASE("Iterator_MappingCopyCtor") {
    fkyaml::node mapping = fkyaml::node::mapping({{"test", fkyaml::node()}});
    fkyaml::detail::iterator<fkyaml::node> copied(
        fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
    fkyaml::detail::iterator<fkyaml::node> iterator(copied);
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
    REQUIRE(iterator.key().get_value_ref<const std::string&>() == "test");
    REQUIRE(iterator.value().is_null());
}

TEST_CASE("Iterator_SequenceMoveCtor") {
    fkyaml::node sequence = {"test"};
    fkyaml::detail::iterator<fkyaml::node> moved(
        fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
    fkyaml::detail::iterator<fkyaml::node> iterator(std::move(moved));
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
    REQUIRE(iterator->is_string());
    REQUIRE(iterator->get_value_ref<fkyaml::node::string_type&>().compare("test") == 0);
}

TEST_CASE("Iterator_MappingMoveCtor") {
    fkyaml::node mapping = fkyaml::node::mapping({{"test", fkyaml::node()}});
    fkyaml::detail::iterator<fkyaml::node> moved(
        fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
    fkyaml::detail::iterator<fkyaml::node> iterator(std::move(moved));
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
    REQUIRE(iterator.key().get_value_ref<const std::string&>() == "test");
    REQUIRE(iterator.value().is_null());
}

TEST_CASE("Iterator_AssignmentOperator") {
    SECTION("self assignment.") {
        fkyaml::node sequence = fkyaml::node::sequence({fkyaml::node()});
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());

        SECTION("lvalue iterator") {
            iterator = *&iterator;
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_null());
        }

        SECTION("rvalue iterator") {
            iterator = std::move(*&iterator);
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_null());
        }
    }

    SECTION("sequence iterators") {
        fkyaml::node copied_seq = {"test"};
        fkyaml::detail::iterator<fkyaml::node> copied_itr(
            fkyaml::detail::sequence_iterator_tag {}, copied_seq.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::node sequence = {false};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());

        SECTION("lvalue iterator") {
            iterator = copied_itr;
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_string());
            REQUIRE(iterator->get_value_ref<fkyaml::node::string_type&>().compare("test") == 0);
        }

        SECTION("rvalue iterator") {
            iterator = std::move(copied_itr);
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_string());
            REQUIRE(iterator->get_value_ref<fkyaml::node::string_type&>().compare("test") == 0);
        }
    }

    SECTION("mapping iterators") {
        fkyaml::node copied_map = {{"key", "test"}};
        fkyaml::detail::iterator<fkyaml::node> copied_itr(
            fkyaml::detail::mapping_iterator_tag {}, copied_map.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::node map = {{"foo", false}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, map.get_value_ref<fkyaml::node::mapping_type&>().begin());

        SECTION("lvalue iterator") {
            iterator = copied_itr;
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
            REQUIRE(iterator.key().get_value_ref<const std::string&>() == "key");
            REQUIRE(iterator.value().is_string());
            REQUIRE(iterator.value().get_value_ref<fkyaml::node::string_type&>().compare("test") == 0);
        }

        SECTION("rvalue iterator") {
            iterator = std::move(copied_itr);
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
            REQUIRE(iterator.key().get_value_ref<const std::string&>() == "key");
            REQUIRE(iterator.value().is_string());
            REQUIRE(iterator.value().get_value_ref<fkyaml::node::string_type&>().compare("test") == 0);
        }
    }
}

TEST_CASE("Iterator_ArrowOperator") {
    SECTION("sequence iterator") {
        fkyaml::node seq = {"test"};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, seq.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE(iterator.operator->() == &(seq.get_value_ref<fkyaml::node::sequence_type&>().operator[](0)));
    }

    SECTION("mapping iterator") {
        fkyaml::node map = {{"key", "test"}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, map.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE(iterator.operator->() == &(map.get_value_ref<fkyaml::node::mapping_type&>().operator[]("key")));
    }
}

TEST_CASE("Iterator_DereferenceOperator") {
    SECTION("sequence iterator") {
        fkyaml::node seq = {"test"};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, seq.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE(&(iterator.operator*()) == &(seq.get_value_ref<fkyaml::node::sequence_type&>().operator[](0)));
    }

    SECTION("mapping iterator") {
        fkyaml::node map = fkyaml::node::mapping({{"key", "test"}});
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, map.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE(&(iterator.operator*()) == &(map.get_value_ref<fkyaml::node::mapping_type&>().operator[]("key")));
    }
}

TEST_CASE("Iterator_CompoundAssignmentOperatorBySum") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        iterator += 1;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        iterator += 1;
        REQUIRE(iterator.key().get_value_ref<const std::string&>() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_PlusOperator") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> after_plus_itr = iterator + 1;
        REQUIRE(after_plus_itr->is_boolean());
        REQUIRE(after_plus_itr->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> after_plus_itr = iterator + 1;
        REQUIRE(after_plus_itr.key().get_value_ref<const std::string&>() == "test1");
        REQUIRE(after_plus_itr.value().is_boolean());
        REQUIRE(after_plus_itr.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_PreIncrementOperator") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        ++iterator;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        ++iterator;
        REQUIRE(iterator.key().get_value_ref<const std::string&>() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_PostIncrementOperator") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        iterator++;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        iterator++;
        REQUIRE(iterator.key().get_value_ref<const std::string&>() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_CompoundAssignmentOperatorByDifference") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().end());
        iterator -= 1;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().end());
        iterator -= 1;
        REQUIRE(iterator.key().get_value_ref<const std::string&>() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_MinusOperator") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().end());
        fkyaml::detail::iterator<fkyaml::node> after_minus_itr = iterator - 1;
        REQUIRE(after_minus_itr->is_boolean());
        REQUIRE(after_minus_itr->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator.") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().end());
        fkyaml::detail::iterator<fkyaml::node> after_minus_itr = iterator - 1;
        REQUIRE(after_minus_itr.key().get_value_ref<const std::string&>() == "test1");
        REQUIRE(after_minus_itr.value().is_boolean());
        REQUIRE(after_minus_itr.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_PreDecrementOperator") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().end());
        --iterator;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().end());
        --iterator;
        REQUIRE(iterator.key().get_value_ref<const std::string&>() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_PostDecrementOperator") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().end());
        iterator--;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().end());
        iterator--;
        REQUIRE(iterator.key().get_value_ref<const std::string&>() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_EqualToOperator") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE(lhs == rhs);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE(lhs == rhs);
    }

    SECTION("equality check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs == rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_NotEqualToOperator") {
    SECTION("sequence iterator.") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        ++rhs;
        REQUIRE(lhs != rhs);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        ++rhs;
        REQUIRE(lhs != rhs);
    }

    SECTION("equality check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs == rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_LessThanOperator") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE_FALSE(lhs < rhs);
        ++rhs;
        REQUIRE(lhs < rhs);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs < rhs, fkyaml::exception);
    }

    SECTION("less-than check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs < rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_LessThanOrEqualToOperator") {
    SECTION("sequence iterator") {
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

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs <= rhs, fkyaml::exception);
    }

    SECTION("less-than-or-equal-to check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs <= rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_GreaterThanOperator") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE_FALSE(lhs > rhs);
        ++lhs;
        REQUIRE(lhs > rhs);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs > rhs, fkyaml::exception);
    }

    SECTION("greater-than check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs > rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_GreaterThanOrEqualToOperator") {
    SECTION("sequence iterator") {
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

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs >= rhs, fkyaml::exception);
    }

    SECTION("greater-than-or-equal-to check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_THROWS_AS(lhs >= rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_TypeGetter") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
    }
}

TEST_CASE("Iterator_KeyGetter") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE_THROWS_AS(iterator.key(), fkyaml::exception);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE_NOTHROW(iterator.key());
        REQUIRE(iterator.key().get_value_ref<const std::string&>() == "test0");
    }
}

TEST_CASE("Iterator_ValueGetter") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::sequence_iterator_tag {}, sequence.get_value_ref<fkyaml::node::sequence_type&>().begin());
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == false);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(
            fkyaml::detail::mapping_iterator_tag {}, mapping.get_value_ref<fkyaml::node::mapping_type&>().begin());
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == false);
    }
}
