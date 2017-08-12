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
#include "Invariants.hh"

#include <cstring>
#include <algorithm>

namespace Classes {

Set twoPartition(Graph g) {
    const Set vertices = g.vertices();
    Set colored = {};
    Set black = {};
    while (colored != vertices) {
	int u = (vertices - colored).min();
	colored += u;
	black += u;
	Set queue = {u};
	bool queueIsBlack = true;
	while (!queue.isEmpty()) {
	    Set neighbors = {};
	    for (int u : queue)
		neighbors |= g.neighbors(u);
	    if ((neighbors & colored & (queueIsBlack ? black : ~black)).nonempty())
		return {};
	    queue = neighbors - colored;
	    colored |= neighbors;
	    if (!queueIsBlack)
		black |= neighbors;
	    queueIsBlack = !queueIsBlack;
	}
    }
    return black;
}

bool isBipartite(const Graph& g) {
    if (g.n() <= 2)
	return true;
    return twoPartition(g).nonempty();
}

bool isTriviallyPerfect(const Graph& g) {
    static Graph p4 = Graph::byName("P4");
    static Graph c4 = Graph::byName("C4");
    return !Subgraph::hasInduced(g, p4) && !Subgraph::hasInduced(g, c4);
}

bool isChordal(const Graph& g) {
    if (g.n() == 0)
	return true;
    Set unnumbered = g.vertices();
    int s = unnumbered.pop();
    Set numbered({s});
    while (unnumbered.nonempty()) {
	int v = 0;
	int max_number = -1;
	for (int u : unnumbered) {
	    int number = (g.neighbors(u) & numbered).size();
	    if (number > max_number) {
		max_number = number;
		v = u;
	    }
	}
	unnumbered.discard(v);
        numbered.add(v);
        Set maybeClique = g.neighbors(v) & numbered;
	for (int u : maybeClique)
	    if (!maybeClique.isSubset(g.neighbors(u) + u))
		return false;
    }
    return true;
}

bool isPerfect(const Graph& g) {
    return !Subgraph::hasOddHole(g) && !Subgraph::hasOddHole(g.complement());
}

bool isSplit(const Graph& g) {
    int n = g.n();
    if (n == 0)
	return true;
    int degs[n];
    for (int u = 0; u < n; ++u)
	degs[u] = g.deg(u);
    std::sort(degs, degs + n, std::greater<std::size_t>());
    int sum1 = 0, i;
    for (i = 0; i < n && degs[i] >= i; ++i)
        sum1 += degs[i];
    int m = i;
    int sum2 = 0;
    for (; i < n; ++i)
        sum2 += degs[i];
    int splittance2 = m * (m-1) + sum2 - sum1;
    return splittance2 == 0;
}

bool isCograph(const Graph& g) {
    static Graph p4 = Graph::byName("P4");
    return !Subgraph::hasInduced(g, p4);
}

bool isThreshold(const Graph& g) {
    return isSplit(g) && isCograph(g);
}

bool isP4Sparse(const Graph& g) {
    static auto c5 = Subgraph::hasInducedTest(Graph::byName("C5"));
    static auto p5 = Subgraph::hasInducedTest(Graph::byName("P5"));
    static auto p5C = Subgraph::hasInducedTest(Graph::byName("P5").complement());
    static auto p = Subgraph::hasInducedTest(Graph::byName("banner"));
    static auto pC = Subgraph::hasInducedTest(Graph::byName("banner").complement());
    static auto fork = Subgraph::hasInducedTest(Graph::byName("fork"));
    static auto forkC = Subgraph::hasInducedTest(Graph::byName("fork").complement());
    return !c5(g)
	&& !p5(g)
	&& !p5C(g)
	&& !p(g)
	&& !pC(g)
	&& !fork(g)
	&& !forkC(g);
}

bool isClique(const Graph& g, Set vs) {
    for (int u : vs)
	if (!vs.isSubset(g.neighbors(u) + u))
	    return false;
    return true;
}

bool isIndependentSet(const Graph& g, Set vs) {
    for (int u : vs)
        if ((g.neighbors(u) & vs).nonempty())
            return false;
    return true;
}

bool isSplitClusterGraph(const Graph& g) {
    for (Set cc : g.connectedComponents()) {
	if (!isSplit(g.subgraph(cc)))
	    return false;
    }
    return true;
}

bool isMonopolar(const Graph& g) {
    for (Set is : g.vertices().subsets())
        if (isIndependentSet(g, is) && !Subgraph::hasInducedP3(g.subgraph(g.vertices() - is)))
	    return true;
    return false;
}

bool independencePolynomialHasFactorXPlus1(const Graph& g) {
    auto p = Invariants::independencePolynomial(g);
    int64_t s = 0;
    for (size_t i = 0; i < p.size(); ++i) {
	if ((i % 2) == 0) {
	    s += int64_t(p[i]);
	} else {
	    s -= int64_t(p[i]);
	}
    }
    return s == 0;
}

bool isEulerian(const Graph& g) {
    for (int u = 0; u < g.n(); ++u)
	if (g.deg(u) % 2)
	    return false;
    return true;
}

bool isModule(const Graph& g, Set vs) {
    if (vs.size() <= 1 || vs == g.vertices())
	return false;
    int u = vs.min();
    Set ns = g.neighbors(u) - vs;
    for (int u : vs)
	if (g.neighbors(u) - vs != ns)
	    return false;
    return true;
}

bool isPrime(const Graph& g) {
    if (g.n() <= 3)
	return 0;
    for (Set vs : g.vertices().subsets()) {
	if (isModule(g, vs))
	    return false;
    }
    return true;
}

bool isWeaklyChordal(const Graph& g) {
    return !Subgraph::hasLongHole(g) && !Subgraph::hasLongHole(g.complement());
}

void dfs(const Graph& g, int dfsNumber[], int dfsParent[], int dfsOrder[], Set backEdges[], int u, int parent, int& d) {
    dfsOrder[d] = u;
    dfsNumber[u] = ++d;
    dfsParent[u] = parent;
    for (int v : g.neighbors(u) - parent) {
	if (dfsNumber[v]) {
	    backEdges[v].add(u);
	} else {
	    dfs(g, dfsNumber, dfsParent, dfsOrder, backEdges, v, u, d);
	}
    }
}

bool isTwoEdgeConnected0(const Graph& g) {
    if (g.n() < 2 || !g.isConnected())
	return false;
    Graph g2 = g;
    for (int u = 0; u < g.n(); ++u) {
	for (int v : g.neighbors(u).above(u)) {
	    g2.removeEdge(u, v);
	    if (!g2.isConnected())
		return false;
	    g2.addEdge(u, v);
	}
    }
    return true;
}

bool isTwoEdgeConnected(const Graph& g) {
    int n = g.n();
    if (n < 2)
	return false;
    int dfsNumber[n];
    int dfsParent[n];
    Set backEdges[n];
    int dfsOrder[n];
    std::memset(dfsNumber, 0, sizeof dfsNumber);
    std::memset(backEdges, 0, sizeof backEdges);
    int d = 0;
    // dfs numbering starts at 1
    dfs(g, dfsNumber, dfsParent, dfsOrder, backEdges, 0, 0, d);
    if (d != g.n())
	return false;		// disconnected
    Set visited;
    int traversed = 0;
    for (int i = 0; i < n; ++i) {
	int u = dfsOrder[i];
	if (backEdges[u].nonempty()) {
	    visited.add(u);
	    for (int v : backEdges[u]) {
		int w = v;
		while (!visited.contains(w)) {
		    visited.add(w);
		    w = dfsParent[w];
		    ++traversed;
		}
	    }
	}
    }
    return traversed == n - 1;
}

bool isTwoVertexConnected0(const Graph& g) {
    if (g.n() < 3 || !g.isConnected())
	return false;
    for (int u = 0; u < g.n(); ++u) {
	Graph g2 = g;
	g2.deleteVertex(u);
	if (!g2.isConnected())
	    return false;
    }
    return true;
}

bool isTwoVertexConnected(const Graph& g) {
    int n = g.n();
    if (n < 3)
	return false;
    int dfsNumber[n];
    int dfsParent[n];
    Set backEdges[n];
    int dfsOrder[n];
    std::memset(dfsNumber, 0, sizeof dfsNumber);
    std::memset(backEdges, 0, sizeof backEdges);
    int d = 0;
    dfs(g, dfsNumber, dfsParent, dfsOrder, backEdges, 0, 0, d);
    if (d != g.n())
	return false;		// disconnected
    Set visited;
    int traversed = 0;
    bool first = true;
    for (int i = 0; i < n; ++i) {
	int u = dfsOrder[i];
	if (backEdges[u].nonempty()) {
	    visited.add(u);
	    for (int v : backEdges[u]) {
		int w = v;
		while (!visited.contains(w)) {
		    visited.add(w);
		    w = dfsParent[w];
		    if (!first && w == u)
			return false;
		    ++traversed;
		}
		first = false;
	    }
	}
    }
    return traversed == n - 1;
}

bool isMinimallyTwoEdgeConnected(const Graph& g) {
    if (!Classes::isTwoEdgeConnected(g))
	return false;
    Graph g2 = g;
    for (int u : g.vertices()) {
	for (int v : g.neighbors(u).above(u)) {
	    g2.removeEdge(u, v);
	    if (Classes::isTwoEdgeConnected(g2))
		return false;
	    g2.addEdge(u, v);
	}
    }
    return true;
}

bool isMinimallyTwoVertexConnected(const Graph& g) {
    if (!Classes::isTwoVertexConnected(g))
	return false;
    for (int u : g.vertices()) {
	Graph g2 = g;
	g2.deleteVertex(u);
	if (Classes::isTwoVertexConnected(g2))
	    return false;
    }
    return true;
}

bool isHamiltonian(const Graph& g, int s, int t, Set path) {
    if (path.size() == g.n())
	return g.hasEdge(s, t);
    for (int u : g.neighbors(t) - path)
	if (isHamiltonian(g, s, u, path + u))
	    return true;
    return false;
}

bool isHamiltonian(const Graph& g) {
    for (int u : g.vertices())
	if (isHamiltonian(g, u, u, {u}))
	    return true;
    return false;
}

bool isWeaklyPerfect(const Graph& g) {
  return Invariants::cliqueNumber(g) == Invariants::coloringNumber(g);
}

bool isWellCovered(const Graph& g) {
    int isSize = -1;
    for (Set vs : g.vertices().subsets()) {
	if (!isIndependentSet(g, vs))
	    continue;
	Set possibleExtensions = g.vertices() - vs;
	for (int u : vs)
	    possibleExtensions -= g.neighbors(u);
	if (!possibleExtensions.isEmpty())
	    continue;
	if (isSize == -1) {
	    isSize = vs.size();
	} else {
	    if (isSize != vs.size())
		return false;
	}
    }
    return true;
}

}  // namespace Classes
