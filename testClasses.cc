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

#include "Classes.hh"

#include "catch.hh"

TEST_CASE("chordal", "[Classes]" ) {
    REQUIRE(Classes::isChordal(Graph::byName("K2")));
}

TEST_CASE("tree", "[Classes]") {
    REQUIRE_FALSE(Classes::isTree(Graph::byName("K0")));
    REQUIRE(Classes::isTree(Graph::byName("K1")));
    REQUIRE(Classes::isTree(Graph::byName("P3")));
    REQUIRE(Classes::isTree(Graph::byName("claw")));
    REQUIRE_FALSE(Classes::isTree(Graph::byName("triangle")));
    REQUIRE_FALSE(Classes::isTree(Graph::byName("K2+K1")));
}

TEST_CASE("two-edge-connected", "[Classes]") {
    REQUIRE_FALSE(Classes::isTwoEdgeConnected(Graph::byName("K1")));
    REQUIRE_FALSE(Classes::isTwoEdgeConnected(Graph::byName("K2")));
    REQUIRE(Classes::isTwoEdgeConnected(Graph::byName("triangle")));
    REQUIRE_FALSE(Classes::isTwoEdgeConnected(Graph::byName("P3")));
    REQUIRE_FALSE(Classes::isTwoEdgeConnected(Graph::byName("K2+K2")));
    REQUIRE(Classes::isTwoEdgeConnected(Graph::byName("bowtie")));
}

TEST_CASE("two-vertex-connected", "[Classes]") {
    REQUIRE_FALSE(Classes::isTwoVertexConnected(Graph::byName("K1")));
    REQUIRE_FALSE(Classes::isTwoVertexConnected(Graph::byName("K2")));
    REQUIRE(Classes::isTwoVertexConnected(Graph::byName("triangle")));
    REQUIRE_FALSE(Classes::isTwoVertexConnected(Graph::byName("P3")));
    REQUIRE_FALSE(Classes::isTwoVertexConnected(Graph::byName("claw")));
    REQUIRE_FALSE(Classes::isTwoVertexConnected(Graph::byName("K2+K2")));
    REQUIRE_FALSE(Classes::isTwoVertexConnected(Graph::byName("bowtie")));
}
