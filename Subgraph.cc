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

// simple and unoptimized subgraph isomorphism
static uint64_t extendCount(const Graph& g, const Graph& f,
			    Set unassigned, std::vector<int>& assignment) {
    if (assignment.size() == size_t(f.n()))
        return 1;
    int u_f = assignment.size();
    uint64_t count = 0;
    for (int u_g : unassigned) {
        bool fits = true;
        for (int v_f = 0; v_f < u_f; ++v_f) {
            if (f.hasEdge(u_f, v_f) != g.hasEdge(u_g, assignment[v_f])) {
                fits = false;
                break;
            }
        }
        if (fits) {
            assignment.push_back(u_g);
            count += extendCount(g, f, unassigned - u_g, assignment);
            assignment.pop_back();
        }
    }
    return count;
}

uint64_t countSubgraphs(const Graph& g, const Graph& f) {
    if (g.n() < f.n())
	return 0;
    std::vector<int> assignment;
    auto numAutomorphisms = extendCount(f, f, f.vertices(), assignment);
    assignment.clear();
    auto numF = extendCount(g, f, g.vertices(), assignment);
    assert(numF % numAutomorphisms == 0);
    return numF / numAutomorphisms;
}

uint64_t countP5s(const Graph& g) {
    int n = g.n();
    uint64_t count = 0;
    // u--v--w--x--y
    for (int w = 0; w < n; ++w) {
	for (int v : g.neighbors(w)) {
	    for (int x : (g.neighbors(w) - g.neighbors(v)).above(v)) {
		for (int u : g.neighbors(v) - g.neighbors(w) - g.neighbors(x)) {
		    count += (g.neighbors(x) - g.neighbors(w) - g.neighbors(v) - g.neighbors(u)).size();
		}
	    }
	}
    }
    return count;
}
