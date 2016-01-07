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

}  // namespace Classes
