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
#include "Graph.hh"
#include "Subgraph.hh"

#include <cstring>

namespace Invariants {

// return value MAXN indicates disconnected graph
int diameter(const Graph& g) {
    if (g.n() == 0)
	return g.n();
    int diameter = 0;
    for (int u : g.vertices()) {
	Set seen = {u};
	Set layer = {u};
	int d = 0;
	for (; ; ++d) {
	    Set nextLayer;
	    for (int v : layer)
		nextLayer |= g.neighbors(v);
	    nextLayer -= seen;
	    if (nextLayer.isEmpty())
		break;
	    seen |= nextLayer;
	    layer = nextLayer;
	}
	if (seen != g.vertices())
	    return Graph::maxn();
	diameter = std::max(diameter, d);
    }
    return diameter;
}

// return value MAXN indicates disconnected graph
int radius(const Graph& g) {
    if (g.n() == 0)
	return g.n();
    int radius = Graph::maxn();
    for (int u : g.vertices()) {
	Set seen = {u};
	Set layer = {u};
	int d = 0;
	for (; ; ++d) {
	    Set nextLayer;
	    for (int v : layer)
		nextLayer |= g.neighbors(v);
	    nextLayer -= seen;
	    if (nextLayer.isEmpty())
		break;
	    seen |= nextLayer;
	    layer = nextLayer;
	}
	if (seen != g.vertices())
	    return 0;
	radius = std::min(radius, d);
    }
    return radius;
}

bool kColorable(const Graph& g, int k, const Set options[], Set uncolored, Set freshColors) {
    if (uncolored.isEmpty())
	return true;
    int leastOptions = k + 1;
    int v = -1;
    for (int u : uncolored) {
	int o = options[u].size();
	assert(o);
	if (o < leastOptions) {
	    leastOptions = o;
	    v = u;
	}
    }
    assert(v != -1);

    Set v_options =  options[v];
    if (options[v] == freshColors) {
	const auto c = freshColors.min();
	v_options = Set({c});
	freshColors -= c;
    }
    for (int c : v_options) {
	Set options2[g.n()];
	std::memcpy(options2, options, sizeof options2);
	options2[v] = Set({c});
	for (int u : g.neighbors(v)) {
	    options2[u] -= c;
	    if (options2[u].isEmpty())
		return false;
	}
	if (kColorable(g, k, options2, uncolored - v, freshColors))
	    return true;
    }
    return false;
}

bool kColorable(const Graph& g, int k) {
    if (k == 0)
	return g.n() == 0;
    if (k == 1)
	return Classes::isIndependentSet(g);
    if (k == 2)
	return Classes::isBipartite(g);
    if (g.n() <= k)
	return true;
    Set options[g.n()];
    for (int u = 0; u < g.n(); ++u)
	options[u] = Set::ofRange(k);
    options[0] = Set({0});
    for (int u : g.neighbors(0))
	options[u] -= 0;
    return kColorable(g, k, options, g.vertices() - 0, Set::ofRange(k) - 0);
}

int coloringNumber(const Graph& g) {
    for (int k = 0; ; ++k)
	if (kColorable(g, k))
	    return k;
}

int cliqueNumber(const Graph& g) {
    int omega = 0;
    g.maximalCliques([&omega](Set clique) { omega = std::max(omega, clique.size()); });
    return omega;
}

void countIndependentSets(const Graph& g, int size, Set ext, uint64_t count[]) {
    if (ext.nonempty()) {
	int u = ext.pop();
	++count[size + 1];
	countIndependentSets(g, size + 1, ext - g.neighbors(u), count);
	countIndependentSets(g, size, ext, count);
    }
}

std::vector<uint64_t> independencePolynomial(const Graph& g) {
    std::vector<uint64_t> p(g.n() + 1);
    ++p[0];
    countIndependentSets(g, 0, g.vertices(), p.data());
    return p;
}

}  // namespace Invariants
