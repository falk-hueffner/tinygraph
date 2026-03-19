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

#include "Invariants.hh"

#include "catch.hh"

TEST_CASE("diameter and radius", "[Invariants]") {
    SECTION("empty graph") {
        Graph g(0);
        REQUIRE(Invariants::diameter(g) == 0);
        REQUIRE(Invariants::radius(g) == 0);
    }

    SECTION("connected path") {
        Graph g = Graph::byName("P3");
        REQUIRE(Invariants::diameter(g) == 2);
        REQUIRE(Invariants::radius(g) == 1);
    }

    SECTION("disconnected graph") {
        Graph g(3);
        g.addEdge(0, 1);
        REQUIRE(Invariants::diameter(g) == Graph::maxn());
        REQUIRE(Invariants::radius(g) == Graph::maxn());
    }
}
