//  _______   __ __   __  _____   __  __  __
// |   __| |_/  |  \_/  |/  _  \ /  \/  \|  |     fkYAML: A C++ header-only YAML library (supporting code)
// |   __|  _  < \_   _/|  ___  |    _   |  |___  version 0.4.2
// |__|  |_| \__|  |_|  |_|   |_|___||___|______| https://github.com/fktn-k/fkYAML
//
// SPDX-FileCopyrightText: 2023-2025 Kensuke Fukutani <fktn.dev@gmail.com>
// SPDX-License-Identifier: MIT

#include <catch2/catch.hpp>

#include <fkYAML/node.hpp>

TEST_CASE("ReverseIterator_Ctor") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    SECTION("with base iterator") {
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit = sequence.begin();
        REQUIRE(rit.base() == sequence.begin());
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2 = mapping.begin();
        REQUIRE(rit2.base() == mapping.begin());
    }

    SECTION("with compatible reverse_iterator") {
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> rit = sequence.rbegin();
        REQUIRE(rit.base() == sequence.end());
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> rit2 = mapping.rbegin();
        REQUIRE(rit2.base() == mapping.end());
    }
}

TEST_CASE("ReverseIterator_AssignmentOperator") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    SECTION("with compatible reverse_iterator") {
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit = sequence.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2 = mapping.rbegin();

        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> crit;
        crit = rit;
        REQUIRE(crit.base() == sequence.end());

        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> crit2;
        crit2 = rit2;
        REQUIRE(crit2.base() == mapping.end());
    }
}

TEST_CASE("ReverseIterator_DereferenceOperator") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit = sequence.end();
    REQUIRE(&(*rit) == &sequence[1]);

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2 = mapping.end();
    REQUIRE(&(*rit2) == &mapping["foo"]);
}

TEST_CASE("ReverseIterator_ArrowOperator") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit = sequence.end();
    REQUIRE(rit.operator->() == &sequence[1]);

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2 = mapping.end();
    REQUIRE(rit2.operator->() == &mapping["foo"]);
}

TEST_CASE("ReverseIterator_CompoundAssignmentOperatorBySum") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit = sequence.end();
    rit += 1;
    REQUIRE(rit.operator->() == &sequence[0]);

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2 = mapping.end();
    rit2 += 1;
    REQUIRE(rit2.operator->() == &mapping["bar"]);
}

TEST_CASE("ReverseIterator_PlusOperator") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit = sequence.end();
    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit_plus = rit + 1;
    REQUIRE(rit.operator->() == &sequence[1]);
    REQUIRE(rit_plus.operator->() == &sequence[0]);

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2 = mapping.end();
    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2_plus = rit2 + 1;
    REQUIRE(rit2.operator->() == &mapping["foo"]);
    REQUIRE(rit2_plus.operator->() == &mapping["bar"]);
}

TEST_CASE("ReverseIterator_CompoundAssignmentOperatorByDifference") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit = sequence.begin();
    rit -= 1;
    REQUIRE(rit.operator->() == &sequence[0]);

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2 = mapping.begin();
    rit2 -= 1;
    REQUIRE(rit2.operator->() == &mapping["bar"]);
}

TEST_CASE("ReverseIterator_MinusOperator") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit = sequence.begin();
    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit_minus = rit - 1;
    REQUIRE(rit.base() == sequence.begin());
    REQUIRE(rit_minus.operator->() == &sequence[0]);

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2 = mapping.begin();
    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2_minus = rit2 - 1;
    REQUIRE(rit2.base() == mapping.begin());
    REQUIRE(rit2_minus.operator->() == &mapping["bar"]);
}

TEST_CASE("ReverseIterator_PreDecrementOperator") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit = sequence.begin();
    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit_predcr = --rit;
    REQUIRE(rit == rit_predcr);

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2 = mapping.begin();
    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2_predcr = --rit2;
    REQUIRE(rit2 == rit2_predcr);
}

TEST_CASE("ReverseIterator_PostDecrementOperator") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit = sequence.begin();
    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit_postdcr = rit--;
    REQUIRE(rit + 1 == rit_postdcr);

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2 = mapping.begin();
    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2_postdcr = rit2--;
    REQUIRE(rit2 + 1 == rit2_postdcr);
}

TEST_CASE("ReverseIterator_PreIncrementOperator") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit = sequence.end();
    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit_preincr = ++rit;
    REQUIRE(rit == rit_preincr);

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2 = mapping.end();
    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2_preincr = ++rit2;
    REQUIRE(rit2 == rit2_preincr);
}

TEST_CASE("ReverseIterator_PostIncrementOperator") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit = sequence.end();
    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit_postincr = rit++;
    REQUIRE(rit - 1 == rit_postincr);

    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2 = mapping.end();
    fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rit2_postincr = rit2++;
    REQUIRE(rit2 - 1 == rit2_postincr);
}

TEST_CASE("ReverseIterator_EqualToOperator") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    SECTION("sequence iterator") {
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> lhs = sequence.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rhs = sequence.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> clhs = sequence.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> crhs = sequence.rbegin();

        REQUIRE(lhs == rhs);
        REQUIRE(clhs == crhs);
        REQUIRE(lhs == crhs);
        REQUIRE(clhs == rhs);
    }

    SECTION("sequence iterator") {
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> lhs = mapping.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rhs = mapping.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> clhs = mapping.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> crhs = mapping.rbegin();

        REQUIRE(lhs == rhs);
        REQUIRE(clhs == crhs);
        REQUIRE(lhs == crhs);
        REQUIRE(clhs == rhs);
    }

    SECTION("equality check between different type reverse iterators") {
        REQUIRE_THROWS_AS(sequence.rbegin() == mapping.rbegin(), fkyaml::exception);
    }
}

TEST_CASE("ReverseIterator_NotEqualToOperator") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    SECTION("sequence iterator") {
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> lhs = sequence.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rhs = sequence.rbegin() + 1;
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> clhs = sequence.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> crhs = sequence.rbegin() + 1;

        REQUIRE(lhs != rhs);
        REQUIRE(clhs != crhs);
        REQUIRE(lhs != crhs);
        REQUIRE(clhs != rhs);
    }

    SECTION("mapping iterator") {
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> lhs = mapping.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rhs = mapping.rbegin() + 1;
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> clhs = mapping.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> crhs = mapping.rbegin() + 1;

        REQUIRE(lhs != rhs);
        REQUIRE(clhs != crhs);
        REQUIRE(lhs != crhs);
        REQUIRE(clhs != rhs);
    }

    SECTION("equality check between different type reverse iterators") {
        REQUIRE_THROWS_AS(sequence.rbegin() != mapping.rbegin(), fkyaml::exception);
    }
}

TEST_CASE("ReverseIterator_LessThanOperator") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    SECTION("sequence iterator") {
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> lhs = sequence.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rhs = sequence.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> clhs = sequence.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> crhs = sequence.rbegin();

        REQUIRE_FALSE(lhs < rhs);
        REQUIRE_FALSE(clhs < crhs);
        REQUIRE_FALSE(lhs < crhs);
        REQUIRE_FALSE(clhs < rhs);
        ++rhs;
        ++crhs;
        REQUIRE(lhs < rhs);
        REQUIRE(clhs < crhs);
        REQUIRE(lhs < crhs);
        REQUIRE(clhs < rhs);
    }

    SECTION("mapping iterator") {
        REQUIRE_THROWS_AS(mapping.rbegin() < mapping.rbegin() + 1, fkyaml::exception);
    }

    SECTION("equality check between different type reverse iterators") {
        REQUIRE_THROWS_AS(sequence.rbegin() < mapping.rbegin(), fkyaml::exception);
    }
}

TEST_CASE("ReverseIterator_LessThanOrEqualToOperator") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    SECTION("sequence iterator") {
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> lhs = sequence.rbegin() + 1;
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rhs = sequence.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> clhs = sequence.rbegin() + 1;
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> crhs = sequence.rbegin();

        REQUIRE_FALSE(lhs <= rhs);
        REQUIRE_FALSE(clhs <= crhs);
        REQUIRE_FALSE(lhs <= crhs);
        REQUIRE_FALSE(clhs <= rhs);
        --lhs;
        --clhs;
        REQUIRE(lhs <= rhs);
        REQUIRE(clhs <= crhs);
        REQUIRE(lhs <= crhs);
        REQUIRE(clhs <= rhs);
        ++rhs;
        ++crhs;
        REQUIRE(lhs <= rhs);
        REQUIRE(clhs <= crhs);
        REQUIRE(lhs <= crhs);
        REQUIRE(clhs <= rhs);
    }

    SECTION("mapping iterator") {
        REQUIRE_THROWS_AS(mapping.rbegin() <= mapping.rbegin() + 1, fkyaml::exception);
    }

    SECTION("equality check between different type reverse iterators") {
        REQUIRE_THROWS_AS(sequence.rbegin() <= mapping.rbegin(), fkyaml::exception);
    }
}

TEST_CASE("ReverseIterator_GreaterThanOperator") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    SECTION("sequence iterator") {
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> lhs = sequence.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rhs = sequence.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> clhs = sequence.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> crhs = sequence.rbegin();

        REQUIRE_FALSE(lhs > rhs);
        REQUIRE_FALSE(clhs > crhs);
        REQUIRE_FALSE(lhs > crhs);
        REQUIRE_FALSE(clhs > rhs);
        ++lhs;
        ++clhs;
        REQUIRE(lhs > rhs);
        REQUIRE(clhs > crhs);
        REQUIRE(lhs > crhs);
        REQUIRE(clhs > rhs);
    }

    SECTION("mapping iterator") {
        REQUIRE_THROWS_AS(mapping.rbegin() + 1 > mapping.rbegin(), fkyaml::exception);
    }

    SECTION("equality check between different type reverse iterators") {
        REQUIRE_THROWS_AS(sequence.rbegin() > mapping.rbegin(), fkyaml::exception);
    }
}

TEST_CASE("ReverseIterator_GreaterThanOrEqualToOperator") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    SECTION("sequence iterator") {
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> lhs = sequence.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::iterator> rhs = sequence.rbegin() + 1;
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> clhs = sequence.rbegin();
        fkyaml::detail::reverse_iterator<fkyaml::node::const_iterator> crhs = sequence.rbegin() + 1;

        REQUIRE_FALSE(lhs >= rhs);
        REQUIRE_FALSE(clhs >= crhs);
        REQUIRE_FALSE(lhs >= crhs);
        REQUIRE_FALSE(clhs >= rhs);
        --rhs;
        --crhs;
        REQUIRE(lhs >= rhs);
        REQUIRE(clhs >= crhs);
        REQUIRE(lhs >= crhs);
        REQUIRE(clhs >= rhs);
        ++lhs;
        ++clhs;
        REQUIRE(lhs >= rhs);
        REQUIRE(clhs >= crhs);
        REQUIRE(lhs >= crhs);
        REQUIRE(clhs >= rhs);
    }

    SECTION("mapping iterator") {
        REQUIRE_THROWS_AS(mapping.rbegin() + 1 >= mapping.rbegin(), fkyaml::exception);
    }

    SECTION("equality check between different type reverse iterators") {
        REQUIRE_THROWS_AS(sequence.rbegin() >= mapping.rbegin(), fkyaml::exception);
    }
}

TEST_CASE("ReverseIterator_KeyGetter") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    REQUIRE_THROWS_AS(sequence.rbegin().key(), fkyaml::exception);
    REQUIRE(mapping.rbegin().key() == (mapping.end() - 1).key());
}

TEST_CASE("ReverseIterator_ValueGetter") {
    fkyaml::node sequence = {nullptr, 123};
    fkyaml::node mapping = {{"foo", 123}, {"bar", true}};

    REQUIRE(sequence.rbegin().value() == (sequence.end() - 1).value());
    REQUIRE(mapping.rbegin().value() == (mapping.end() - 1).value());
}
