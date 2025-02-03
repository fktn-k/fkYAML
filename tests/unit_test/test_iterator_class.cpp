//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

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
    SECTION("self assignment.") {
        fkyaml::node sequence = fkyaml::node::sequence({fkyaml::node()});
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());

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
        fkyaml::detail::iterator<fkyaml::node> copied_itr(copied_seq.as_seq().begin());
        fkyaml::node sequence = {false};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());

        SECTION("lvalue iterator") {
            iterator = copied_itr;
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_string());
            REQUIRE(iterator->as_str().compare("test") == 0);
        }

        SECTION("rvalue iterator") {
            iterator = std::move(copied_itr);
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
            REQUIRE(iterator->is_string());
            REQUIRE(iterator->as_str().compare("test") == 0);
        }
    }

    SECTION("mapping iterators") {
        fkyaml::node copied_map = {{"key", "test"}};
        fkyaml::detail::iterator<fkyaml::node> copied_itr(copied_map.as_map().begin());
        fkyaml::node map = {{"foo", false}};
        fkyaml::detail::iterator<fkyaml::node> iterator(map.as_map().begin());

        SECTION("lvalue iterator") {
            iterator = copied_itr;
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
            REQUIRE(iterator.key().as_str() == "key");
            REQUIRE(iterator.value().is_string());
            REQUIRE(iterator.value().as_str().compare("test") == 0);
        }

        SECTION("rvalue iterator") {
            iterator = std::move(copied_itr);
            REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
            REQUIRE(iterator.key().as_str() == "key");
            REQUIRE(iterator.value().is_string());
            REQUIRE(iterator.value().as_str().compare("test") == 0);
        }
    }

    SECTION("different const-ness") {
        fkyaml::node seq = {nullptr, 123};
        const fkyaml::node const_seq = {true, 3.14};
        fkyaml::detail::iterator<const fkyaml::node> const_itr = const_seq.begin();

        const_itr = seq.begin();

        REQUIRE(const_itr.type() == fkyaml::detail::iterator_t::SEQUENCE);
        REQUIRE(const_itr->is_null());
    }
}

TEST_CASE("Iterator_ArrowOperator") {
    SECTION("sequence iterator") {
        fkyaml::node seq = {"test"};
        fkyaml::detail::iterator<fkyaml::node> iterator(seq.as_seq().begin());
        REQUIRE(iterator.operator->() == &(seq.as_seq().operator[](0)));
    }

    SECTION("mapping iterator") {
        fkyaml::node map = {{"key", "test"}};
        fkyaml::detail::iterator<fkyaml::node> iterator(map.as_map().begin());
        REQUIRE(iterator.operator->() == &(map.as_map().operator[]("key")));
    }
}

TEST_CASE("Iterator_DereferenceOperator") {
    SECTION("sequence iterator") {
        fkyaml::node seq = {"test"};
        fkyaml::detail::iterator<fkyaml::node> iterator(seq.as_seq().begin());
        REQUIRE(&(iterator.operator*()) == &(seq.as_seq().operator[](0)));
    }

    SECTION("mapping iterator") {
        fkyaml::node map = fkyaml::node::mapping({{"key", "test"}});
        fkyaml::detail::iterator<fkyaml::node> iterator(map.as_map().begin());
        REQUIRE(&(iterator.operator*()) == &(map.as_map().operator[]("key")));
    }
}

TEST_CASE("Iterator_CompoundAssignmentOperatorBySum") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());
        iterator += 1;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.begin());
        iterator += 1;
        REQUIRE(iterator.key().as_str() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_PlusOperator") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());
        fkyaml::detail::iterator<fkyaml::node> after_plus_itr = iterator + 1;
        REQUIRE(after_plus_itr->is_boolean());
        REQUIRE(after_plus_itr->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.begin());
        fkyaml::detail::iterator<fkyaml::node> after_plus_itr = iterator + 1;
        REQUIRE(after_plus_itr.key().as_str() == "test1");
        REQUIRE(after_plus_itr.value().is_boolean());
        REQUIRE(after_plus_itr.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_PreIncrementOperator") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());
        ++iterator;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.begin());
        ++iterator;
        REQUIRE(iterator.key().as_str() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_PostIncrementOperator") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());
        iterator++;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.begin());
        iterator++;
        REQUIRE(iterator.key().as_str() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_CompoundAssignmentOperatorByDifference") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.end());
        iterator -= 1;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.end());
        iterator -= 1;
        REQUIRE(iterator.key().as_str() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_MinusOperator") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.end());
        fkyaml::detail::iterator<fkyaml::node> after_minus_itr = iterator - 1;
        REQUIRE(after_minus_itr->is_boolean());
        REQUIRE(after_minus_itr->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator.") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.end());
        fkyaml::detail::iterator<fkyaml::node> after_minus_itr = iterator - 1;
        REQUIRE(after_minus_itr.key().as_str() == "test1");
        REQUIRE(after_minus_itr.value().is_boolean());
        REQUIRE(after_minus_itr.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_PreDecrementOperator") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.end());
        --iterator;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.end());
        --iterator;
        REQUIRE(iterator.key().as_str() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_PostDecrementOperator") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.end());
        iterator--;
        REQUIRE(iterator->is_boolean());
        REQUIRE(iterator->get_value<fkyaml::node::boolean_type>() == true);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.end());
        iterator--;
        REQUIRE(iterator.key().as_str() == "test1");
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == true);
    }
}

TEST_CASE("Iterator_EqualToOperator") {
    SECTION("sequence iterator") {
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

    SECTION("mapping iterator") {
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

    SECTION("equality check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs == rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_NotEqualToOperator") {
    SECTION("sequence iterator.") {
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

    SECTION("mapping iterator") {
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

    SECTION("equality check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs != rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_LessThanOperator") {
    SECTION("sequence iterator") {
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

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(mapping.begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs < rhs, fkyaml::exception);
    }

    SECTION("less-than check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs < rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_LessThanOrEqualToOperator") {
    SECTION("sequence iterator") {
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

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(mapping.begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs <= rhs, fkyaml::exception);
    }

    SECTION("less-than-or-equal-to check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs <= rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_GreaterThanOperator") {
    SECTION("sequence iterator") {
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

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(mapping.begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs > rhs, fkyaml::exception);
    }

    SECTION("greater-than check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs > rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_GreaterThanOrEqualToOperator") {
    SECTION("sequence iterator") {
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

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> lhs(mapping.begin());
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs >= rhs, fkyaml::exception);
    }

    SECTION("greater-than-or-equal-to check between different type iterators") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> lhs(sequence.begin());
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> rhs(mapping.begin());
        REQUIRE_THROWS_AS(lhs >= rhs, fkyaml::exception);
    }
}

TEST_CASE("Iterator_TypeGetter") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());
        REQUIRE(iterator.type() == fkyaml::detail::iterator_t::SEQUENCE);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.begin());
        REQUIRE(iterator.type() == fkyaml::detail::iterator_t::MAPPING);
    }
}

TEST_CASE("Iterator_KeyGetter") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());
        REQUIRE_THROWS_AS(iterator.key(), fkyaml::exception);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.begin());
        REQUIRE_NOTHROW(iterator.key());
        REQUIRE(iterator.key().as_str() == "test0");
    }
}

TEST_CASE("Iterator_ValueGetter") {
    SECTION("sequence iterator") {
        fkyaml::node sequence = {false, true};
        fkyaml::detail::iterator<fkyaml::node> iterator(sequence.begin());
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == false);
    }

    SECTION("mapping iterator") {
        fkyaml::node mapping = {{"test0", false}, {"test1", true}};
        fkyaml::detail::iterator<fkyaml::node> iterator(mapping.begin());
        REQUIRE(iterator.value().is_boolean());
        REQUIRE(iterator.value().get_value<fkyaml::node::boolean_type>() == false);
    }
}
