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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#include "nauty/planarity.h"
#pragma GCC diagnostic pop

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

static bool isCograph(const Graph& g, const bool mustBeDisconnected) {
    if (g.n() < 4)
	return true;
    for (const auto c : g.connectedComponents()) {
	if (c.size() == g.n() && mustBeDisconnected)
	    return false;
	if (c.size() >= 4 && !isCograph(g.subgraph(c).complement(), true))
	    return false;
    }
    return true;
}

bool isCograph(const Graph& g) {
    return isCograph(g, false);
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
    if (g.n() == 0)
	return false; // could also be considered true
    else if (g.n() == 1)
	return true;  // could also be considered false
    else if (g.n() == 2)
	return false; // needs to be special-cased, code below would give wrong result
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

int dist(const Graph& g, int u, int v) {
    if (u == v)
	return 0;
    Set reached = {u};
    Set expanded = {};
    int d = 1;
    while (true) {
	Set toExpand = reached - expanded;
	if (toExpand.isEmpty())
	    return 1000000;
	for (int w : toExpand) {
	    reached |= g.neighbors(w);
	    if (reached.contains(v))
		return d;
	}
	++d;
	expanded |= toExpand;
    }
}

bool isDistanceHereditary(const Graph& g) {
    int d[g.n()][g.n()];
    for (int u : g.vertices())
	for (int v : g.vertices().above(u))
	    d[u][v] = d[v][u] =  dist(g, u, v);
    // for every four vertices u, v, w, and x, at least two of the
    // three sums of distances d(u,v)+d(w,x), d(u,w)+d(v,x), and
    // d(u,x)+d(v,w) are equal to each other
    for (int u : g.vertices()) {
	for (int v : g.vertices().above(u)) {
	    for (int w : g.vertices().above(v)) {
		for (int x : g.vertices().above(w)) {
		    int d1 = d[u][v] + d[w][x];
		    if (d1 > g.n()) continue;
		    int d2 = d[u][w] + d[v][x];
		    if (d2 > g.n()) continue;
		    int d3 = d[u][x] + d[v][w];
		    if (d3 > g.n()) continue;
		    int eq1 = d1 == d2;
		    int eq2 = d1 == d3;
		    int eq3 = d2 == d3;
		    if (!(eq1 || eq2 || eq3))
			return false;
		}
	    }
	}
    }
    return true;
}

// An asteroidal triple is an independent set of three vertices such
// that each pair is joined by a path that avoids the neighborhood of
// the third.
bool isAsteroidalTriple(const Graph& g, int u, int v, int w) {
    if (g.hasEdge(u, v) || g.hasEdge(u, w) || g.hasEdge(v, w))
	return false;
    auto vs_u = g.vertices() - g.neighbors(u);
    auto g_u = g.subgraph(vs_u);
    if (dist(g_u, vs_u.below(v).size(), vs_u.below(w).size()) > g.n()) return false;
    auto vs_v = g.vertices() - g.neighbors(v);
    auto g_v = g.subgraph(vs_v);
    if (dist(g_v, vs_v.below(u).size(), vs_v.below(w).size()) > g.n()) return false;
    auto vs_w = g.vertices() - g.neighbors(w);
    auto g_w = g.subgraph(vs_w);
    if (dist(g_w, vs_w.below(u).size(), vs_w.below(v).size()) > g.n()) return false;
    return true;
}


bool isATFree(const Graph& g) {
    for (auto u : g.vertices())
	for (auto v : g.nonneighbors(u).above(u))
	    for (auto w : (g.nonneighbors(u) & g.nonneighbors(v)).above(v))
		if (isAsteroidalTriple(g, u, v, w))
		    return false;
    return true;
}

bool isElementary(const Graph& g) {
    Edge edges[g.n() * g.n()];
    int es[g.n()][g.n()];
    int m = 0;
    for (auto e : g.edges()) {
	edges[m] = e;
	es[e.u][e.v] = es[e.v][e.u] = m;
	++m;
    }
    bool isColored[m];
    bool isBlack[m];
    memset(isColored, 0, sizeof isColored);
    for (int e = 0; e < m; ++e) {
	if (isColored[e])
	    continue;
	isBlack[e] = true;
	isColored[e] = true;
	int queue[m];
	auto qbegin = queue;
	auto qend = queue;
	*qend++ = e;
	while (qbegin != qend) {
	    const auto f = *qbegin++;
	    const auto u = edges[f].u;
	    const auto v = edges[f].v;
	    assert(g.hasEdge(u, v));
	    assert(isColored[f]);
	    const auto nextIsBlack = !isBlack[f];
	    for (const auto w : g.neighbors(u) & g.nonneighbors(v)) {
		const auto e_uw = es[u][w];
		if (isColored[e_uw]) {
		    if (isBlack[e_uw] != nextIsBlack)
			return false;
		} else {
		    isBlack[e_uw] = nextIsBlack;
		    isColored[e_uw] = true;
		    *qend++ = e_uw;
		}
	    }
	    for (const auto w : g.neighbors(v) & g.nonneighbors(u)) {
		const auto e_vw = es[v][w];
		if (isColored[e_vw]) {
		    if (isBlack[e_vw] != nextIsBlack)
			return false;
		} else {
		    isBlack[e_vw] = nextIsBlack;
		    isColored[e_vw] = true;
		    *qend++ = e_vw;
		}
	    }
	}
    }
    return true;
}

bool isHoang(const Graph& g) {
    Edge edges[g.n() * g.n()];
    int es[g.n()][g.n()];
    int m = 0;
    for (auto e : g.edges()) {
	edges[m] = e;
	es[e.u][e.v] = es[e.v][e.u] = m;
	++m;
    }
    bool isColored[m];
    bool isBlack[m];
    memset(isColored, 0, sizeof isColored);
    for (int e = 0; e < m; ++e) {
	if (isColored[e])
	    continue;
	isBlack[e] = true;
	isColored[e] = true;
	int queue[m];
	auto qbegin = queue;
	auto qend = queue;
	*qend++ = e;
	while (qbegin != qend) {
	    const auto f = *qbegin++;
	    const auto u = edges[f].u;
	    const auto v = edges[f].v;
	    assert(g.hasEdge(u, v));
	    assert(isColored[f]);
	    const auto nextIsBlack = !isBlack[f];
	    for (int f2 = 0; f2 < m; ++f2) {
		const auto u2 = edges[f2].u;
		if (u2 == u || u2 == v)
		    continue;
		const auto v2 = edges[f2].v;
		if (v2 == u || v2 == v)
		    continue;
		if (g.hasEdge(u, u2) + g.hasEdge(u, v2) + g.hasEdge(v, u2) + g.hasEdge(v, v2) != 1)
		    continue;
		if (isColored[f2]) {
		    if (isBlack[f2] != nextIsBlack)
			return false;
		} else {
		    isBlack[f2] = nextIsBlack;
		    isColored[f2] = true;
		    *qend++ = f2;
		}
	    }
	}
    }
    return true;
}

bool isTwoSplit(const Graph& g) {
    for (auto vs : g.vertices().subsets())
	if (isSplit(g.subgraph(vs)) && isSplit(g.subgraph(g.vertices() - vs)))
	    return true;
    return false;
}

bool isPlanar(const Graph& g) {
    int n = g.n();
    if (n == 0)
	return true;
    std::vector<t_ver_sparse_rep> V(n);
    std::vector<t_adjl_sparse_rep> A(2 * g.m() + 1);

    for (int i = 0, k = 0; i < n; ++i) {
	if (g.deg(i) == 0) {
	    V[i].first_edge = NIL;
	} else {
	     V[i].first_edge = k;
	     for (int j : g.neighbors(i)) {
		 A[k].end_vertex = j;
		 A[k].next = k + 1;
		 ++k;
		 if (A[k - 1].end_vertex == i) {  // loops go in twice
		     A[k].end_vertex = i;
		     A[k].next = k + 1;
		     k++;
		 }
	     }
	     A[k-1].next = NIL;
	}
    }

    int c;
    t_dlcl **dfs_tree, **back_edges, **mult_edges;
    int edge_pos, v, w;
    boolean ans;
    t_ver_edge *embed_graph;

    ans = sparseg_adjl_is_planar(V.data(), n, A.data(), &c,
                                 &dfs_tree, &back_edges, &mult_edges,
                                 &embed_graph, &edge_pos, &v, &w);

    sparseg_dlcl_delete(dfs_tree, n);
    sparseg_dlcl_delete(back_edges, n);
    sparseg_dlcl_delete(mult_edges, n);
    embedg_VES_delete(embed_graph, n);

    return ans;
}

}  // namespace Classes
