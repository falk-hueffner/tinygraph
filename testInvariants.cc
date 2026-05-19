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

TEST_CASE("girth", "[Invariants]") {
    SECTION("empty graph is acyclic") {
        REQUIRE(Invariants::girth(Graph(0)) == Graph::maxn() + 1);
    }
    SECTION("tree is acyclic") {
        REQUIRE(Invariants::girth(Graph::byName("P5")) == Graph::maxn() + 1);
    }
    SECTION("triangle") {
        REQUIRE(Invariants::girth(Graph::byName("K3")) == 3);
    }
    SECTION("C4, C5, C6, C7") {
        REQUIRE(Invariants::girth(Graph::cycle(4)) == 4);
        REQUIRE(Invariants::girth(Graph::cycle(5)) == 5);
        REQUIRE(Invariants::girth(Graph::cycle(6)) == 6);
        REQUIRE(Invariants::girth(Graph::cycle(7)) == 7);
    }
    SECTION("K4 has girth 3") {
        REQUIRE(Invariants::girth(Graph::byName("K4")) == 3);
    }
    SECTION("K_{3,3} has girth 4") {
        REQUIRE(Invariants::girth(Graph::byName("K3,3")) == 4);
    }
    SECTION("Petersen graph has girth 5") {
        // graph6 string for the Petersen graph
        Graph g = Graph::ofGraph6("IsP@OkWHG");
        REQUIRE(Invariants::girth(g) == 5);
    }
    SECTION("disconnected: minimum over components") {
        Graph g(8);
        // C4 on {0,1,2,3}
        g.addEdge(0, 1); g.addEdge(1, 2); g.addEdge(2, 3); g.addEdge(3, 0);
        // K3 on {4,5,6}
        g.addEdge(4, 5); g.addEdge(5, 6); g.addEdge(6, 4);
        // isolated vertex 7
        REQUIRE(Invariants::girth(g) == 3);
    }
}
