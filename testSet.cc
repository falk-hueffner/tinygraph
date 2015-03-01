/* tinygraph -- exploring graph conjectures on small graphs
   Copyright (C) 2015  Falk Hüffner

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.  */

#include "Set.hh"

#include "catch.hh"

TEST_CASE("Set", "[Set]") {
    Set empty;
    Set s1 = {0, 2, 6};
    Set s2 = s1;
    s2.add(1);
    s2.add(Set::MAX_ELEMENT);
    Set s3 = {1, 2, 4, 5};

    SECTION("size") {
	REQUIRE(empty.size() == 0);
        REQUIRE(s1.size() == 3);
        REQUIRE(s2.size() == 5);
    }
    SECTION("contains") {
	REQUIRE(s1.contains(0));
	REQUIRE(!s1.contains(1));
	REQUIRE(s1.contains(2));
	REQUIRE(!s1.contains(3));
	REQUIRE(!s1.contains(Set::MAX_ELEMENT));
	REQUIRE(s2.contains(0));
	REQUIRE(s2.contains(1));
	REQUIRE(s2.contains(2));
	REQUIRE(!s2.contains(3));
	REQUIRE(s2.contains(Set::MAX_ELEMENT));
    }
    SECTION("operations") {
	REQUIRE(empty == Set());
	REQUIRE(s1 == s1);
	REQUIRE(s1 != s2);
	REQUIRE(!(s1 == s2));
	REQUIRE((s1 & s2) == s1);
	REQUIRE((s1 & s3) == Set({2}));
	REQUIRE((s2 & s3) == Set({1, 2}));
	REQUIRE((s1 | s2) == s2);
	REQUIRE((s1 | s3) == Set({0, 1, 2, 4, 5, 6}));
	REQUIRE((s1 ^ s1) == empty);
	REQUIRE((s1 ^ s3) == Set({0, 1, 4, 5, 6}));
	REQUIRE((s1 - s3) == Set({0, 6}));
	REQUIRE((s1 - s2) == empty);
	REQUIRE((s2 - s1) == Set({1, Set::MAX_ELEMENT}));
	REQUIRE((s2 - s2) == empty);

	Set s3 = s1;
	s3 += 2;
	REQUIRE(s3 == s1 + 2);
	s3 += 4;
	REQUIRE(s3 == Set({0, 2, 4, 6}));
	s3 -= 0;
	REQUIRE(s3 == Set({2, 4, 6}));
	s3 -= 1;
	REQUIRE(s3 == Set({2, 4, 6}));

	REQUIRE(((s1 + 1) + Set::MAX_ELEMENT) == s2);
	REQUIRE(((s2 - 1) - Set::MAX_ELEMENT) == s1);

	REQUIRE(s2.isSubset(s2));
	REQUIRE(s2.isSuperset(s2));
	REQUIRE(s1.isSubset(s2));
	REQUIRE(!s2.isSubset(s1));
	REQUIRE(!s1.isSuperset(s2));
	REQUIRE(s2.isSuperset(s1));
    }
    SECTION("enumerate") {
	int n = 0;
	for (int x : s2) {
	    REQUIRE(s2.contains(x));
	    ++n;
	}
	REQUIRE(n == s2.size());
	for (int x : empty)
	    REQUIRE(false);

    }
    SECTION("subsets") {
	int n = 0;
	for (Set s : s2.subsets()) {
	    REQUIRE(s.isSubset(s2));
	    ++n;
	}
	REQUIRE(n == 1 << s2.size());

	n = 0;
	for (Set s : empty.subsets()) {
	    REQUIRE(s == empty);
	    ++n;
	}
	REQUIRE(n == 1);
	n = 0;
	for (Set s : Set({Set::MAX_ELEMENT}).subsets()) {
	    REQUIRE(((s == empty) || (s == Set({Set::MAX_ELEMENT}))));
	    ++n;
	}
	REQUIRE(n == 2);
    }
}
