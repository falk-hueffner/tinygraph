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

#include "Subgraph.hh"

#include "catch.hh"

TEST_CASE("Subgraph", "[Subgraph]") {
    Graph p3 = Graph::byName("P3");
    Graph paw = Graph::byName("paw");
    Graph k3 = Graph::byName("K3");
    Graph c4 = Graph::byName("C4");
    Graph diamond = Graph::byName("diamond");
    Graph k4 = Graph::byName("K4");
    Graph claw = Graph::byName("claw");
    Graph p4 = Graph::byName("P4");
    Graph p5 = Graph::byName("P5");
    Graph c5 = Graph::byName("C5");
    Graph c7 = Graph::byName("C7");
    Graph c9 = Graph::byName("C9");
    for (int n = 0; n <= 9; ++n) {
	Graph::enumerate(n, [&](const Graph& g) {
		REQUIRE((Subgraph::countInduced(g, p3) > 0)  == Subgraph::hasInduced(g, p3));
		REQUIRE((Subgraph::countInduced(g, paw) > 0) == Subgraph::hasInduced(g, paw));
		REQUIRE(Subgraph::hasInducedP3(g)   == Subgraph::hasInduced(g, p3));
		REQUIRE(Subgraph::hasK3(g)          == Subgraph::hasInduced(g, k3));
		REQUIRE(Subgraph::hasInducedPaw(g)  == Subgraph::hasInduced(g, paw));
		REQUIRE(Subgraph::hasInducedClaw(g) == Subgraph::hasInduced(g, claw));
		REQUIRE(Subgraph::hasInducedPaw(g)  == Subgraph::hasInduced(g, paw));
		REQUIRE(Subgraph::hasC4(g)          ==(Subgraph::hasInduced(g, c4)
				                    || Subgraph::hasInduced(g, diamond)
					            || Subgraph::hasInduced(g, k4)));
		REQUIRE(Subgraph::hasInducedC4(g)   == Subgraph::hasInduced(g, c4));
		REQUIRE(Subgraph::hasInducedDiamond(g) == Subgraph::hasInduced(g, diamond));
		REQUIRE(Subgraph::hasInducedP5(g)   == Subgraph::hasInduced(g, p5));
		REQUIRE(Subgraph::countInducedP3s(g) == Subgraph::countInduced(g, p3));
		REQUIRE(Subgraph::countInducedP4s(g) == Subgraph::countInduced(g, p4));
		REQUIRE(Subgraph::countInducedP5s(g) == Subgraph::countInduced(g, p5));
		REQUIRE(Subgraph::hasOddHole(g) == (Subgraph::hasInduced(g, c5)
						 || Subgraph::hasInduced(g, c7)
						 || Subgraph::hasInduced(g, c9)));
	    });
    }
}
