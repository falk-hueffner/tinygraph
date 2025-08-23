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

#include "Graph.hh"

#include <set>
#include <string>
#include "catch.hh"

TEST_CASE("Graph basic operations", "[Graph]") {
    SECTION("empty graph") {
        Graph g(0);
        REQUIRE(g.n() == 0);
        REQUIRE(g.m() == 0);
        REQUIRE(g.isConnected());
    }

    SECTION("single vertex") {
        Graph g(1);
        REQUIRE(g.n() == 1);
        REQUIRE(g.m() == 0);
        REQUIRE(g.deg(0) == 0);
        REQUIRE(g.isConnected());
        REQUIRE(g.vertices() == Set({0}));
    }

    SECTION("two vertices") {
        Graph g(2);
        REQUIRE_FALSE(g.isConnected());

        g.addEdge(0, 1);
        REQUIRE(g.m() == 1);
        REQUIRE(g.deg(0) == 1);
        REQUIRE(g.deg(1) == 1);
        REQUIRE(g.hasEdge(0, 1));
        REQUIRE(g.hasEdge(1, 0));
        REQUIRE(g.isConnected());
    }

    SECTION("triangle") {
        Graph g(3, {{0, 1}, {1, 2}, {0, 2}});
        REQUIRE(g.n() == 3);
        REQUIRE(g.m() == 3);
        REQUIRE(g.deg(0) == 2);
        REQUIRE(g.deg(1) == 2);
        REQUIRE(g.deg(2) == 2);
        REQUIRE(g.isConnected());
    }
}

TEST_CASE("Graph edge operations", "[Graph]") {
    Graph g(4);

    SECTION("add edge") {
        g.addEdge(0, 1);
        REQUIRE(g.hasEdge(0, 1));
        REQUIRE(g.hasEdge(1, 0));
        REQUIRE(g.m() == 1);
        REQUIRE(g.neighbors(0) == Set({1}));
        REQUIRE(g.neighbors(1) == Set({0}));
    }

    SECTION("remove edge") {
        g.addEdge(0, 1);
        g.addEdge(1, 2);
        REQUIRE(g.m() == 2);

        g.removeEdge(0, 1);
        REQUIRE_FALSE(g.hasEdge(0, 1));
        REQUIRE(g.m() == 1);
        REQUIRE(g.neighbors(0).isEmpty());
        REQUIRE(g.neighbors(1) == Set({2}));
    }

    SECTION("toggle edge") {
        REQUIRE_FALSE(g.hasEdge(0, 1));
        g.toggleEdge(0, 1);
        REQUIRE(g.hasEdge(0, 1));
        g.toggleEdge(0, 1);
        REQUIRE_FALSE(g.hasEdge(0, 1));
    }
}

TEST_CASE("Graph byName method", "[Graph]") {
    SECTION("named graphs") {
        REQUIRE(Graph::byName("empty").n() == 0);

        Graph triangle = Graph::byName("triangle");
        REQUIRE(triangle.n() == 3);
        REQUIRE(triangle.m() == 3);

        Graph claw = Graph::byName("claw");
        REQUIRE(claw.n() == 4);
        REQUIRE(claw.m() == 3);
    }

    SECTION("complete graphs") {
        Graph k1 = Graph::byName("K1");
        REQUIRE(k1.n() == 1);
        REQUIRE(k1.m() == 0);

        Graph k3 = Graph::byName("K3");
        REQUIRE(k3.n() == 3);
        REQUIRE(k3.m() == 3);

        Graph k4 = Graph::byName("K4");
        REQUIRE(k4.n() == 4);
        REQUIRE(k4.m() == 6);
    }

    SECTION("path graphs") {
        Graph p1 = Graph::byName("P1");
        REQUIRE(p1.n() == 1);
        REQUIRE(p1.m() == 0);

        Graph p3 = Graph::byName("P3");
        REQUIRE(p3.n() == 3);
        REQUIRE(p3.m() == 2);
        REQUIRE(p3.deg(0) == 1);
        REQUIRE(p3.deg(1) == 2);
        REQUIRE(p3.deg(2) == 1);
    }

    SECTION("cycle graphs") {
        Graph c3 = Graph::byName("C3");
        REQUIRE(c3.n() == 3);
        REQUIRE(c3.m() == 3);

        Graph c4 = Graph::byName("C4");
        REQUIRE(c4.n() == 4);
        REQUIRE(c4.m() == 4);
        for (int i = 0; i < 4; ++i) {
            REQUIRE(c4.deg(i) == 2);
        }
    }

    SECTION("complete bipartite graphs") {
        Graph k23 = Graph::byName("K2,3");
        REQUIRE(k23.n() == 5);
        REQUIRE(k23.m() == 6);
        REQUIRE(k23.deg(0) == 3);
        REQUIRE(k23.deg(1) == 3);
        REQUIRE(k23.deg(2) == 2);
        REQUIRE(k23.deg(3) == 2);
        REQUIRE(k23.deg(4) == 2);
    }

    SECTION("disjoint union") {
        Graph k2k2 = Graph::byName("K2+K2");
        REQUIRE(k2k2.n() == 4);
        REQUIRE(k2k2.m() == 2);
        REQUIRE_FALSE(k2k2.isConnected());

        Graph p3p2 = Graph::byName("P3+P2");
        REQUIRE(p3p2.n() == 5);
        REQUIRE(p3p2.m() == 3);
    }

    SECTION("multiple copies") {
        Graph three_k2 = Graph::byName("3K2");
        REQUIRE(three_k2.n() == 6);
        REQUIRE(three_k2.m() == 3);
        REQUIRE_FALSE(three_k2.isConnected());
    }
}

TEST_CASE("Graph byName error handling", "[Graph]") {
    SECTION("invalid inputs") {
        REQUIRE_THROWS_AS(Graph::byName(""), std::invalid_argument);
        REQUIRE_THROWS_AS(Graph::byName("unknown"), std::invalid_argument);
        REQUIRE_THROWS_AS(Graph::byName("K"), std::invalid_argument);
        REQUIRE_THROWS_AS(Graph::byName("P"), std::invalid_argument);
        REQUIRE_THROWS_AS(Graph::byName("C"), std::invalid_argument);
        REQUIRE_THROWS_AS(Graph::byName("C1"), std::invalid_argument);
    }

    SECTION("explicit empty graphs") {
        Graph k0 = Graph::byName("K0");
        REQUIRE(k0.n() == 0);

        Graph p0 = Graph::byName("P0");
        REQUIRE(p0.n() == 0);
    }

    SECTION("invalid numbers") {
        REQUIRE_THROWS_AS(Graph::byName("Ka"), std::invalid_argument);
        REQUIRE_THROWS_AS(Graph::byName("K3a"), std::invalid_argument);
        REQUIRE_THROWS_AS(Graph::byName("P2b"), std::invalid_argument);
    }

    SECTION("too large graphs") {
        REQUIRE_THROWS_AS(Graph::byName("K10001"), std::invalid_argument);
        REQUIRE_THROWS_AS(Graph::byName("P20000"), std::invalid_argument);
    }
}

TEST_CASE("Graph connectivity", "[Graph]") {
    SECTION("empty graph") {
        Graph empty(0);
        REQUIRE(empty.isConnected());
    }

    SECTION("single vertex") {
        Graph single(1);
        REQUIRE(single.isConnected());
    }

    SECTION("connected graphs") {
        REQUIRE(Graph::byName("K3").isConnected());
        REQUIRE(Graph::byName("P3").isConnected());
        REQUIRE(Graph::byName("C4").isConnected());
        REQUIRE(Graph::byName("claw").isConnected());

        Graph path(4);
        path.addEdge(0, 1);
        path.addEdge(1, 2);
        path.addEdge(2, 3);
        REQUIRE(path.isConnected());

        Graph star(5);
        for (int i = 1; i < 5; ++i) {
            star.addEdge(0, i);
        }
        REQUIRE(star.isConnected());
    }

    SECTION("disconnected graphs") {
        REQUIRE_FALSE(Graph::byName("K2+K2").isConnected());
        REQUIRE_FALSE(Graph::byName("2K3").isConnected());

        Graph two_comp(5);
        two_comp.addEdge(0, 1);
        two_comp.addEdge(3, 4);
        REQUIRE_FALSE(two_comp.isConnected());

        Graph isolated_start(3);
        isolated_start.addEdge(1, 2);
        REQUIRE_FALSE(isolated_start.isConnected());

        Graph isolated_mid(3);
        isolated_mid.addEdge(0, 2);
        REQUIRE_FALSE(isolated_mid.isConnected());

        Graph multiple_isolated(5);
        multiple_isolated.addEdge(1, 2);
        REQUIRE_FALSE(multiple_isolated.isConnected());
    }

    SECTION("edge cases") {
        Graph two_no_edge(2);
        REQUIRE_FALSE(two_no_edge.isConnected());

        Graph two_one_edge(2);
        two_one_edge.addEdge(0, 1);
        REQUIRE(two_one_edge.isConnected());

        for (int n = 1; n <= 5; ++n) {
            Graph kn = Graph::byName("K" + std::to_string(n));
            REQUIRE(kn.isConnected());
        }
    }
}

TEST_CASE("Graph complement", "[Graph]") {
    SECTION("empty graph complement") {
        Graph empty(3);
        Graph comp = empty.complement();
        REQUIRE(comp.n() == 3);
        REQUIRE(comp.m() == 3);
    }

    SECTION("complete graph complement") {
        Graph k3 = Graph::byName("K3");
        Graph comp = k3.complement();
        REQUIRE(comp.n() == 3);
        REQUIRE(comp.m() == 0);
    }

    SECTION("path complement") {
        Graph p3 = Graph::byName("P3");
        Graph comp = p3.complement();
        REQUIRE(comp.n() == 3);
        REQUIRE(comp.m() == 1);
        REQUIRE(comp.hasEdge(0, 2));
    }
}

TEST_CASE("Graph cycle method", "[Graph]") {
    SECTION("invalid small cycles") {
        REQUIRE_THROWS_AS(Graph::cycle(0), std::invalid_argument);
        REQUIRE_THROWS_AS(Graph::cycle(1), std::invalid_argument);
        REQUIRE_THROWS_AS(Graph::cycle(2), std::invalid_argument);
    }

    SECTION("triangle cycle") {
        Graph c3 = Graph::cycle(3);
        REQUIRE(c3.n() == 3);
        REQUIRE(c3.m() == 3);
        REQUIRE(c3.hasEdge(0, 1));
        REQUIRE(c3.hasEdge(1, 2));
        REQUIRE(c3.hasEdge(2, 0));
        REQUIRE(c3.isConnected());
        for (int i = 0; i < 3; ++i) {
            REQUIRE(c3.deg(i) == 2);
        }
    }

    SECTION("square cycle") {
        Graph c4 = Graph::cycle(4);
        REQUIRE(c4.n() == 4);
        REQUIRE(c4.m() == 4);
        REQUIRE(c4.hasEdge(0, 1));
        REQUIRE(c4.hasEdge(1, 2));
        REQUIRE(c4.hasEdge(2, 3));
        REQUIRE(c4.hasEdge(3, 0));
        REQUIRE(c4.isConnected());
        for (int i = 0; i < 4; ++i) {
            REQUIRE(c4.deg(i) == 2);
        }
    }

    SECTION("equivalence with byName") {
        for (int n = 3; n <= 6; ++n) {  // Only test valid cycles (n >= 3)
            Graph cycle1 = Graph::cycle(n);
            Graph cycle2 = Graph::byName("C" + std::to_string(n));
            REQUIRE(cycle1.n() == cycle2.n());
            REQUIRE(cycle1.m() == cycle2.m());
            // Note: We don't check exact equality because canonical forms might differ
        }
    }
}

TEST_CASE("Graph edges iteration", "[Graph]") {
    SECTION("empty graph") {
        Graph g(3);
        int count = 0;
        for (Edge e : g.edges()) {
            (void)e;
            ++count;
        }
        REQUIRE(count == 0);
    }

    SECTION("triangle edges") {
        Graph triangle = Graph::byName("K3");
        std::set<Edge> unique_edges;
        int count = 0;

        for (Edge e : triangle.edges()) {
            unique_edges.insert(e);
            ++count;
        }

        REQUIRE(count == 3);
        REQUIRE(unique_edges.count({0, 1}) == 1);
        REQUIRE(unique_edges.count({0, 2}) == 1);
        REQUIRE(unique_edges.count({1, 2}) == 1);
    }
}

TEST_CASE("Graph connected components", "[Graph]") {
    SECTION("connected graph") {
        Graph triangle = Graph::byName("K3");
        int count = 0;
        for (Set component : triangle.connectedComponents()) {
            REQUIRE(component == triangle.vertices());
            ++count;
        }
        REQUIRE(count == 1);
    }

    SECTION("disconnected graph") {
        Graph disjoint = Graph::byName("K2+K2");
        int count = 0;
        std::vector<Set> components;
        for (Set component : disjoint.connectedComponents()) {
            components.push_back(component);
            ++count;
        }
        REQUIRE(count == 2);
        REQUIRE(components[0].size() == 2);
        REQUIRE(components[1].size() == 2);
    }
}
