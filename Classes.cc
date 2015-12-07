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

#include <cstring>
#include <algorithm>

namespace Classes {

void dfs(const Graph& g, int dfsNumber[], int dfsParent[], int dfsOrder[], Set backEdges[], int u, int parent, int& d) {
    dfsOrder[d] = u;
    dfsNumber[u] = ++d;
    dfsParent[u] = parent;
    for (int v : g.neighbors(u) - parent) {
	//std::cerr << "dfs " << u << " -> " << v << std::endl;
	if (dfsNumber[v]) {
	    backEdges[v].add(u);
	    //std::cerr << "dfs " << u << " -> " << v << std::endl;
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
		//std::cerr << "backedge " << u << ' ' << v << std::endl;
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

#if 0
bool isTwoEdgeConnected3(const Graph& g) {
    bool r0 = isTwoEdgeConnected0(g);
    bool r1 = isTwoEdgeConnected1(g);
    if (r0 != r1) {
	std::cerr << r0 << r1 << g.toString() << std::endl;
	abort();
    }
    return r0;
}
#endif

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
    static Graph c5 = Graph::byName("C5");
    static Graph p5 = Graph::byName("P5");
    static Graph p5C = p5.complement();
    static Graph p = Graph::byName("banner");
    static Graph pC = p.complement();
    static Graph fork = Graph::byName("fork");
    static Graph forkC = fork.complement();
    return !Subgraph::hasInduced(g, c5)
	&& !Subgraph::hasInduced(g, p5)
	&& !Subgraph::hasInduced(g, p5C)
	&& !Subgraph::hasInduced(g, p)
	&& !Subgraph::hasInduced(g, pC)
	&& !Subgraph::hasInduced(g, fork)
	&& !Subgraph::hasInduced(g, forkC);
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

}  // namespace Classes
