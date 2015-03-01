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

Set findP5(const Graph& g) {
    int n = g.n();
    for (int u = 0; u < n; ++u) {
	for (int v : g.neighbors(u)) {
	    for (int w : g.neighbors(v)) {
		if (w == u || g.hasEdge(w, u))
		    continue;
		for (int x : g.neighbors(w)) {
		    if (g.hasEdge(x, v) || g.hasEdge(x, u))
			continue;
		    for (int y : g.neighbors(x)) {
			if (y < u || g.hasEdge(y, w) || g.hasEdge(y, v) || g.hasEdge(y, u))
			    continue;
			return {u, v, w, x, y};
		    }
		}
	    }
	}
    }
    return {};
}

bool p5EditingBranch(Graph& g, int k) {
    if (k < 0)
	return false;
    Set p5 = findP5(g);
    if (p5.isEmpty())
	return true;
    for (Set e : p5.combinations(2)) {
	int u = e.pop();
	int v = e.pop();
	g.toggleEdge(u, v);
	if (p5EditingBranch(g, k - 1))
	    return true;
	g.toggleEdge(u, v);
    }
    return false;
}

int p5Editing(const Graph& g0) {
    Graph g = g0;
    for (int k = 0; ; ++k) {
	if (p5EditingBranch(g, k))
	    return k;
    }
}

int main() {
    int max_k = 0;
    for (int n = 1; n < MAXN; ++n) {
	std::cout << "*** n = " << n << std::endl;
	Graph::enumerate(n, [&max_k](const Graph& g) {
		int k = p5Editing(g);
		if (k > max_k) {
		    std::cout << "k = " << k << std::endl << g << std::endl;
		    max_k = k;
		}
	    });
    }

    return 0;
}
