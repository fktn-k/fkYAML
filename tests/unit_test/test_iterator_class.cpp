//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.4
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2026 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <doctest/doctest.h>

#include <fkYAML/node.hpp>

TEST_CASE("Iterator_SequenceCtor") {
    fkyaml::node sequence = fkyaml::node::sequence();
    fkyaml::detail::iterator<fkyaml::node> iterator(sequence.as_seq().begin());
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
}

TEST_CASE("Iterator_MappingCtor") {
    fkyaml::node mapping = fkyaml::node::mapping();
    fkyaml::detail::iterator<fkyaml::node> iterator(mapping.as_map().begin());
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
}

TEST_CASE("Iterator_SequenceCopyCtor") {
    fkyaml::node sequence = fkyaml::node::sequence({fkyaml::node()});
    fkyaml::detail::iterator<fkyaml::node> copied(sequence.begin());
    fkyaml::detail::iterator<fkyaml::node> iterator(copied);
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
    REQUIRE(iterator->is_null());
}

TEST_CASE("Iterator_MappingCopyCtor") {
    fkyaml::node mapping = fkyaml::node::mapping({{"test", fkyaml::node()}});
    fkyaml::detail::iterator<fkyaml::node> copied(mapping.begin());
    fkyaml::detail::iterator<fkyaml::node> iterator(copied);
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
    REQUIRE(iterator.key().as_str() == "test");
    REQUIRE(iterator.value().is_null());
}

TEST_CASE("Iterator_CtorDifferentConstness") {
    fkyaml::node seq = {nullptr, 123};
    fkyaml::detail::iterator<const fkyaml::node> const_itr = seq.begin();

    REQUIRE(const_itr.type() == fkyaml::detail::iterator_t::SEQUENCE);
    REQUIRE(const_itr->is_null());
}

TEST_CASE("Iterator_SequenceMoveCtor") {
    fkyaml::node sequence = {"test"};
    fkyaml::detail::iterator<fkyaml::node> moved(sequence.begin());
    fkyaml::detail::iterator<fkyaml::node> iterator(std::move(moved));
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
    REQUIRE(iterator->is_string());
    REQUIRE(iterator->as_str().compare("test") == 0);
}

TEST_CASE("Iterator_MappingMoveCtor") {
    fkyaml::node mapping = fkyaml::node::mapping({{"test", fkyaml::node()}});
    fkyaml::detail::iterator<fkyaml::node> moved(mapping.begin());
    fkyaml::detail::iterator<fkyaml::node> iterator(std::move(moved));
    REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
    REQUIRE(iterator.key().as_str() == "test");
    REQUIRE(iterator.value().is_null());
}

TEST_CASE("Iterator_AssignmentOperator") {
    SUBCASE("self assignment.") {
        fkyaml::node sequence = fkyaml::node::sequence({fkyaml::node()});
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());

        SUBCASE("lvalue iterator") {
            iterator = *&iterator;
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_null());
        }

        SUBCASE("rvalue iterator") {
            iterator = std::move(*&iterator);
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_null());
        }
    }

    SUBCASE("sequence iterators") {
        fkyaml::node copied_seq = {"test"};
        fkyaml::detail::iterator<fkyaml::node> copied_itr(copied_seq.as_seq().begin());
        fkyaml::node sequence = {false};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());

        SUBCASE("lvalue iterator") {
            iterator = copied_itr;
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_string());
            REQUIRE(iterator->as_str().compare("test") == 0);
        }

        SUBCASE("rvalue iterator") {
            iterator = std::move(copied_itr);
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_string());
            REQUIRE(iterator->as_str().compare("test") == 0);
        }
    }

    SUBCASE("mapping iterators") {
        fkyaml::node copied_map = {{"key", "test"}};
        fkyaml::detail::iterator<fkyaml::node> copied_itr(copied_map.as_map().begin());
        fkyaml::node map = {{"foo", false}};
        fkyaml::detail::iterator<fkyaml::node> iterator(map.as_map().begin());

        SUBCASE("lvalue iterator") {
            iterator = copied_itr;
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
            REQUIRE(iterator.key().as_str() == "key");
            REQUIRE(iterator.value().is_string());
            REQUIRE(iterator.value().as_str().compare("test") == 0);
        }

        SUBCASE("rvalue iterator") {
            iterator = std::move(copied_itr);
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
            REQUIRE(iterator.key().as_str() == "key");
            REQUIRE(iterator.value().is_string());
            REQUIRE(iterator.value().as_str().compare("test") == 0);
        }
    }

    SUBCASE("different const-ness") {
        fkyaml::node seq = {nullptr, 123};
        const fkyaml::node const_seq = {true, 3.14};
        fkyaml::detail::iterator<const fkyaml::node> const_itr = const_seq.begin();

        const_itr = seq.begin();

        REQUIRE(const_itr.type() == fkyaml::detail::iterator_t::SEQUENCE);
        REQUIRE(const_itr->is_null());
    }
}

TEST_CASE("Iterator_ArrowOperator") {
    SUBCASE("sequence iterator") {
        fkyaml::node seq = {"test"};
        fkyaml::detail::iterator<fkyaml::node> iterator(seq.as_seq().begin());
        REQUIRE(iterator.operator->() == &(seq.as_seq().operator[](0)));
    }

    SUBCASE("mapping iterator") {
        fkyaml::node map = {{"key", "test"}};
        fkyaml::detail::iterator<fkyaml::node> iterator(map.as_map().begin());
        REQUIRE(iterator.operator->() == &(map.as_map().operator[]("key")));
    }
}

TEST_CASE("Iterator_DereferenceOperator") {
    SUBCASE("sequence iterator") {
        fkyaml::node seq = {"test"};
        fkyaml::detail::iterator<fkyaml::node> iterator(seq.as_seq().begin());
        REQUIRE(&(iterator.operator*()) == &(seq.as_seq().operator[](0)));
    }

    SUBCASE("mapping iterator") {
        fkyaml::node map = fkyaml::node::mapping({{"key", "test"}});
        fkyaml::detail::iterator<fkyaml::node> iterator(map.as_map().begin());
        REQUIRE(&(iterator.operator*()) == &(map.as_map().operator[]("key")));
    }
}

TEST_CASE("Iterator_CompoundAssignmentOperatorBySum") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());
        iterator += 1;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.begin());
        iterator += 1;
        REQUIRE(iterator.key().as_str() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_PlusOperator") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());
        fkyaml::detail::iterator<fkyaml::node> after_plus_itr = iterator + 1;
        REQUIRE(after_plus_itr->is_boolean());
        REQUIRE(after_plus_itr->get_value<fkyaml::node::boolean_type>() == true);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.begin());
        fkyaml::detail::iterator<fkyaml::node> after_plus_itr = iterator + 1;
        REQUIRE(after_plus_itr.key().as_str() == "test1");
        REQUIRE(after_plus_itr.value().is_boolean());
        REQUIRE(after_plus_itr.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_PreIncrementOperator") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());
        ++iterator;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.begin());
        ++iterator;
        REQUIRE(iterator.key().as_str() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_PostIncrementOperator") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());
        iterator++;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.begin());
        iterator++;
        REQUIRE(iterator.key().as_str() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_CompoundAssignmentOperatorByDifference") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.end());
        iterator -= 1;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.end());
        iterator -= 1;
        REQUIRE(iterator.key().as_str() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_MinusOperator") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.end());
        fkyaml::detail::iterator<fkyaml::node> after_minus_itr = iterator - 1;
        REQUIRE(after_minus_itr->is_boolean());
        REQUIRE(after_minus_itr->get_value<fkyaml::node::boolean_type>() == true);
    }

    SUBCASE("mapping iterator.") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.end());
        fkyaml::detail::iterator<fkyaml::node> after_minus_itr = iterator - 1;
        REQUIRE(after_minus_itr.key().as_str() == "test1");
        REQUIRE(after_minus_itr.value().is_boolean());
        REQUIRE(after_minus_itr.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_PreDecrementOperator") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.end());
        --iterator;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.end());
        --iterator;
        REQUIRE(iterator.key().as_str() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_PostDecrementOperator") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.end());
        iterator--;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.end());
        iterator--;
        REQUIRE(iterator.key().as_str() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_EqualToOperator") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> itr(sequence.begin());
        fkyaml::detail::iterator<fkyaml::node> itr2(sequence.begin());
        fkyaml::detail::iterator<const fkyaml::node> const_itr(sequence.begin());
        fkyaml::detail::iterator<const fkyaml::node> const_itr2(sequence.begin());

        REQUIRE(itr == itr2);
        REQUIRE(const_itr == const_itr2);
        REQUIRE(itr == const_itr);
        REQUIRE(const_itr == itr);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> itr(mapping.begin());
        fkyaml::detail::iterator<fkyaml::node> itr2(mapping.begin());
        fkyaml::detail::iterator<const fkyaml::node> const_itr(mapping.begin());
        fkyaml::detail::iterator<const fkyaml::node> const_itr2(mapping.begin());

        REQUIRE(itr == itr2);
        REQUIRE(const_itr == const_itr2);
        REQUIRE(itr == const_itr);
        REQUIRE(const_itr == itr);
    }

    SUBCASE("equality check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs == rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_NotEqualToOperator") {
    SUBCASE("sequence iterator.") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(sequence.begin() + 1);
        fkyaml::detail::iterator<const fkyaml::node> const_lhs(sequence.begin());
        fkyaml::detail::iterator<const fkyaml::node> const_rhs(sequence.begin() + 1);

        REQUIRE(lhs != rhs);
        REQUIRE(const_lhs != const_rhs);
        REQUIRE(lhs != const_rhs);
        REQUIRE(const_lhs != rhs);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(mapping.begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin() + 1);
        fkyaml::detail::iterator<const fkyaml::node> const_lhs(mapping.begin());
        fkyaml::detail::iterator<const fkyaml::node> const_rhs(mapping.begin() + 1);

        REQUIRE(lhs != rhs);
        REQUIRE(const_lhs != const_rhs);
        REQUIRE(lhs != const_rhs);
        REQUIRE(const_lhs != rhs);
    }

    SUBCASE("equality check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs != rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_LessThanOperator") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(sequence.begin());
        fkyaml::detail::iterator<const fkyaml::node> const_lhs(sequence.begin());
        fkyaml::detail::iterator<const fkyaml::node> const_rhs(sequence.begin());

        REQUIRE_FALSE(lhs < rhs);
        REQUIRE_FALSE(const_lhs < const_rhs);
        REQUIRE_FALSE(lhs < const_rhs);
        REQUIRE_FALSE(const_lhs < rhs);
        ++rhs;
        ++const_rhs;
        REQUIRE(lhs < rhs);
        REQUIRE(const_lhs < const_rhs);
        REQUIRE(lhs < const_rhs);
        REQUIRE(const_lhs < rhs);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(mapping.begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs < rhs, fkyaml::exception);
    }

    SUBCASE("less-than check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs < rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_LessThanOrEqualToOperator") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin() + 1);
        fkyaml::detail::iterator<fkyaml::node> rhs(sequence.begin());
        fkyaml::detail::iterator<const fkyaml::node> const_lhs(sequence.begin() + 1);
        fkyaml::detail::iterator<const fkyaml::node> const_rhs(sequence.begin());

        REQUIRE_FALSE(lhs <= rhs);
        REQUIRE_FALSE(const_lhs <= const_rhs);
        REQUIRE_FALSE(lhs <= const_rhs);
        REQUIRE_FALSE(const_lhs <= rhs);
        --lhs;
        --const_lhs;
        REQUIRE(lhs <= rhs);
        REQUIRE(const_lhs <= const_rhs);
        REQUIRE(lhs <= const_rhs);
        REQUIRE(const_lhs <= rhs);
        ++rhs;
        ++const_rhs;
        REQUIRE(lhs <= rhs);
        REQUIRE(const_lhs <= const_rhs);
        REQUIRE(lhs <= const_rhs);
        REQUIRE(const_lhs <= rhs);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(mapping.begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs <= rhs, fkyaml::exception);
    }

    SUBCASE("less-than-or-equal-to check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs <= rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_GreaterThanOperator") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(sequence.begin());
        fkyaml::detail::iterator<const fkyaml::node> const_lhs(sequence.begin());
        fkyaml::detail::iterator<const fkyaml::node> const_rhs(sequence.begin());

        REQUIRE_FALSE(lhs > rhs);
        REQUIRE_FALSE(const_lhs > const_rhs);
        REQUIRE_FALSE(lhs > const_rhs);
        REQUIRE_FALSE(const_lhs > rhs);
        ++lhs;
        ++const_lhs;
        REQUIRE(lhs > rhs);
        REQUIRE(const_lhs > const_rhs);
        REQUIRE(lhs > const_rhs);
        REQUIRE(const_lhs > rhs);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(mapping.begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs > rhs, fkyaml::exception);
    }

    SUBCASE("greater-than check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs > rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_GreaterThanOrEqualToOperator") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(sequence.begin() + 1);
        fkyaml::detail::iterator<const fkyaml::node> const_lhs(sequence.begin());
        fkyaml::detail::iterator<const fkyaml::node> const_rhs(sequence.begin() + 1);

        REQUIRE_FALSE(lhs >= rhs);
        REQUIRE_FALSE(const_lhs >= const_rhs);
        REQUIRE_FALSE(lhs >= const_rhs);
        REQUIRE_FALSE(const_lhs >= rhs);
        --rhs;
        --const_rhs;
        REQUIRE(lhs >= rhs);
        REQUIRE(const_lhs >= const_rhs);
        REQUIRE(lhs >= const_rhs);
        REQUIRE(const_lhs >= rhs);
        ++lhs;
        ++const_lhs;
        REQUIRE(lhs >= rhs);
        REQUIRE(const_lhs >= const_rhs);
        REQUIRE(lhs >= const_rhs);
        REQUIRE(const_lhs >= rhs);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(mapping.begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs >= rhs, fkyaml::exception);
    }

    SUBCASE("greater-than-or-equal-to check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs >= rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_TypeGetter") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());
        REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.begin());
        REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
    }
}

TEST_CASE("Iterator_KeyGetter") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());
        REQUIRE_THROWS_AS(iterator.key(), fkyaml::exception);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.begin());
        REQUIRE_NOTHROW(iterator.key());
        REQUIRE(iterator.key().as_str() == "test0");
    }
}

TEST_CASE("Iterator_ValueGetter") {
    SUBCASE("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == false);
    }

    SUBCASE("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.begin());
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == false);
    }
}
