/* tinygraph -- exploring graph conjectures on small graphs
   Copyright (C) 2015  Falk Hüffner

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.	*/

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

Set verticesInP5s(const Graph& g) {
    Set r;
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
			r |= {u, v, w, x, y};
		    }
		}
	    }
	}
    }
    return r;
}

std::vector<std::pair<int, int>>* p5EditingBranch(Graph& g, int k) {
    if (k < 0)
	return nullptr;
    Set p5 = findP5(g);
    if (p5.isEmpty())
	return new std::vector<std::pair<int, int>>();
    for (Set e : p5.combinations(2)) {
	int u = e.pop();
	int v = e.pop();
	g.toggleEdge(u, v);
	auto s = p5EditingBranch(g, k - 1);
	if (s) {
	    s->push_back({u, v});
	    return s;
	}
	g.toggleEdge(u, v);
    }
    return nullptr;
}

std::vector<std::pair<int, int>> p5Editing(const Graph& g0) {
    Graph g = g0;
    for (int k = 0; ; ++k) {
	auto ps = p5EditingBranch(g, k);
	if (ps) {
	    auto s = *ps;
	    delete ps;
	    return s;
	}
    }
}

int main() {
    double max_f = 0;
    for (int n = 1; n <= MAXN; ++n) {
	double max_f_n = 0;
	std::cerr << "--- n = " << n << std::endl;
	Graph::enumerate(n, [&max_f, &max_f_n](const Graph& g) {
		Set vs = g.vertices() - verticesInP5s(g);
		if (!vs.isEmpty()) {
		    auto s = p5Editing(g);
		    for (int u : vs) {
			Graph g2 = g;
			g2.deleteVertex(u);
			auto s2 = p5Editing(g2);
			if (s.size() != s2.size()) {
			    std::cout << "Found counterexample:" << std::endl
				      << g << std::endl
				      << "Optimal solution of size " << s.size() << ':' << std::endl;
			    for (auto p : s)
				std::cout << p.first << '\t' << p.second << std::endl;
			    std::cout << std::endl << "Vertex " << u << " is not contained in any P5." << std::endl
				      << "After deleting " << u << ", optimal solution has size "
				      << s2.size() << ':' << std::endl;
			    for (auto p : s2) {
				auto v2 = [u](int v) { return v < u ? v : v + 1; };
				std::cout << v2(p.first) << '\t' << v2(p.second) << std::endl;
			    }
			    exit(0);
			}
		    }
		}
	    }, Graph::CONNECTED);
    }
    return 0;
}
