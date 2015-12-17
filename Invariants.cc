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
#include "Subgraph.hh"

#include <cstring>

namespace Invariants {

bool kColorable(const Graph& g, int k, const Set options[], Set uncolored) {
    int leastOptions = k + 1;
    int v = -1;
    for (int u : uncolored) {
	int o = options[u].size();
	//std::cerr << u << ' ' << options[u] << o << std::endl;
	assert(o);
	if (o < leastOptions) {
	    leastOptions = o;
	    v = u;
	}
    }
    if (v == -1)
	return true;
    //std::cerr << "branch on " << v << options[v] << std::endl;
    for (int c : options[v]) {
	Set options2[g.n()];
	std::memcpy(options2, options, sizeof options2);
	options2[v] = Set({c});
	for (int u : g.neighbors(v)) {
	    options2[u] -= c;
	    if (options2[u].isEmpty())
		return false;
	}
	if (kColorable(g, k, options2, uncolored - v))
	    return true;
    }
    return false;
}

bool kColorable(const Graph& g, int k) {
    if (k == 0)
	return g.n() == 0;
    if (g.n() == 0)
	return true;
    Set options[g.n()];
    for (int u = 0; u < g.n(); ++u)
	options[u] = Set::ofRange(k);
    options[0] = Set({0});
    for (int u : g.neighbors(0))
	options[u] -= 0;
    return kColorable(g, k, options, g.vertices() - 0);
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

}  // namespace Invariants
