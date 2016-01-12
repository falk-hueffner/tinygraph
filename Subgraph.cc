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

namespace Subgraph {

// simple and unoptimized subgraph isomorphism
uint64_t extendCountInduced(const Graph& g, const Graph& f,
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
            count += extendCountInduced(g, f, unassigned - u_g, assignment);
            assignment.pop_back();
        }
    }
    return count;
}

uint64_t countInduced(const Graph& g, const Graph& f) {
    if (g.n() < f.n())
	return 0;
    std::vector<int> assignment;
    auto numAutomorphisms = extendCountInduced(f, f, f.vertices(), assignment);
    assignment.clear();
    auto numF = extendCountInduced(g, f, g.vertices(), assignment);
    assert(numF % numAutomorphisms == 0);
    return numF / numAutomorphisms;
}

// simple and unoptimized subgraph isomorphism
bool extendHasInduced(const Graph& g, const Graph& f,
		      Set unassigned, std::vector<int>& assignment) {
    if (assignment.size() == size_t(f.n()))
        return true;
    int u_f = assignment.size();
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
            if (extendHasInduced(g, f, unassigned - u_g, assignment))
		return true;
            assignment.pop_back();
        }
    }
    return false;
}

bool hasInduced(const Graph& g, const Graph& f) {
    if (g.n() < f.n())
	return false;
    std::vector<int> assignment;
    return extendHasInduced(g, f, g.vertices(), assignment);
}

// simple and unoptimized subgraph homomorphism
bool extendHas(const Graph& g, const Graph& f,
	       Set unassigned, std::vector<int>& assignment) {
    if (assignment.size() == size_t(f.n()))
        return true;
    int u_f = assignment.size();
    for (int u_g : unassigned) {
        bool fits = true;
        for (int v_f = 0; v_f < u_f; ++v_f) {
            if (f.hasEdge(u_f, v_f) && !g.hasEdge(u_g, assignment[v_f])) {
                fits = false;
                break;
            }
        }
        if (fits) {
            assignment.push_back(u_g);
            if (extendHas(g, f, unassigned - u_g, assignment))
		return true;
            assignment.pop_back();
        }
    }
    return false;
}

bool has(const Graph& g, const Graph& f) {
    if (g.n() < f.n())
	return false;
    std::vector<int> assignment;
    return extendHas(g, f, g.vertices(), assignment);
}

std::function<bool(const Graph&)> hasTest(Graph f) {
    f = f.canonical();
//    if (f == Graph::byName("K3")     .canonical()) return hasK3;
//    if (f == Graph::byName("C4")     .canonical()) return hasC4;
    return [f](const Graph& g) {
	if (g.n() < f.n())
	    return false;
	std::vector<int> assignment;
	return extendHas(g, f, g.vertices(), assignment);
    };
}

std::function<bool(const Graph&)> hasInducedTest(Graph f) {
    f = f.canonical();
    if (f == Graph::byName("P3")     .canonical()) return hasInducedP3;
    if (f == Graph::byName("K3")     .canonical()) return hasK3;
    if (f == Graph::byName("claw")   .canonical()) return hasInducedClaw;
    if (f == Graph::byName("paw")    .canonical()) return hasInducedPaw;
    if (f == Graph::byName("C4")     .canonical()) return hasInducedC4;
    if (f == Graph::byName("diamond").canonical()) return hasInducedDiamond;
    if (f == Graph::byName("P5")     .canonical()) return hasInducedP5;
    if (f == Graph::byName("C5")     .canonical()) return hasInducedC5;
    return [f](const Graph& g) {
	if (g.n() < f.n())
	    return false;
	std::vector<int> assignment;
	return extendHasInduced(g, f, g.vertices(), assignment);
    };
}

bool hasInducedP3(const Graph& g) {
    Set todo = g.vertices();
    while (todo.nonempty()) {
	int u = todo.pop();
	Set s = g.neighbors(u) + u;
	for (int v : g.neighbors(u)) {
	    if (g.neighbors(v) + v != s)
		return true;
	    todo.discard(v);
	}
    }
    return false;
}

bool hasK3(const Graph& g) {
    for (int u = 0; u < g.n(); ++u)
	for (int v : g.neighbors(u).above(u))
	    if ((g.neighbors(u) & g.neighbors(v)).nonempty())
		return true;
    return false;
}

bool hasInducedClaw(const Graph& g) {
    for (int u = 0; u < g.n(); ++u)
	for (int v : g.nonneighbors(u).above(u))
	    for (int w : (g.nonneighbors(u) & g.nonneighbors(v)).above(v))
		if ((g.neighbors(u) & g.neighbors(v) & g.neighbors(w)).nonempty())
		    return true;
    return false;
}

bool hasInducedClaw(const Graph& g, int u) {
    for (int v : g.neighbors(u))
	for (int w : (g.neighbors(u) - g.neighbors(v)).above(v))
	    if ((g.neighbors(u) - g.neighbors(v) - g.neighbors(w)).nonempty())
		return true;
    return false;
}

bool hasInducedPaw(const Graph& g) {
    for (int u = 0; u < g.n(); ++u)
	for (int v : g.neighbors(u).above(u))
	    for (int w : g.neighbors(u) & g.neighbors(v))
		if ((g.nonneighbors(u) & g.nonneighbors(v) & g.neighbors(w)).nonempty())
		    return true;
    return false;
}

bool hasC4(const Graph& g) {
    for (int u = 0; u < g.n(); ++u)
	for (int v : g.neighbors(u).above(u))
	    for (int w : g.neighbors(u).above(v))
		if (((g.neighbors(v) & g.neighbors(w)) - u).nonempty())
		    return true;
    return false;
}

bool hasInducedC4(const Graph& g) {
    for (int u = 0; u < g.n(); ++u)
	for (int v : g.neighbors(u))
	    for (int w : (g.neighbors(u) - g.neighbors(v)).above(v))
		if (((g.neighbors(v) & g.neighbors(w)) - g.neighbors(u) - u).nonempty())
		    return true;
    return false;
}

bool hasInducedDiamond(const Graph& g) {
    for (int u = 0; u < g.n(); ++u)
	for (int v : g.neighbors(u))
	    for (int w : g.neighbors(u).above(v) & g.neighbors(v))
		if (((g.neighbors(v) & g.neighbors(w)) - g.neighbors(u) - u).nonempty())
		    return true;
    return false;
}

bool hasInducedP5(const Graph& g) {
    // u--v--w--x--y
    for (int w = 0; w < g.n(); ++w)
	for (int v : g.neighbors(w))
	    for (int x : (g.neighbors(w) - g.neighbors(v)).above(v))
		for (int u : g.neighbors(v) - g.neighbors(w) - g.neighbors(x))
		    if ((g.neighbors(x) - g.neighbors(w) - g.neighbors(v) - g.neighbors(u)).nonempty())
			return true;
    return false;
}

bool hasInducedC5(const Graph& g) {
    for (int u = 0; u < g.n() - 4; ++u)
	for (int l1 : g.neighbors(u).above(u))
	    for (int r1 : g.neighbors(u).above(l1) - g.neighbors(l1))
		for (int l2 : g.neighbors(l1) - g.neighbors(u) - g.neighbors(r1))
		    if (((g.neighbors(l2) - g.neighbors(l1) - g.neighbors(u)) & g.neighbors(r1)).nonempty())
			return true;
    return false;
}

uint64_t countInducedP3s(const Graph& g) {
    int n = g.n();
    uint64_t num = 0;
    for (int u = 0; u < n; ++u) {
        // count non-edges within G[N(u)]
        Set n_u = g.neighbors(u);
        int n_n = n_u.size();
        num += (n_n * (n_n - 1)) / 2 - g.mSubgraph(n_u);
    }
    return num;
}

uint64_t countInducedP4s(const Graph& g) {
    int n = g.n();
    uint64_t count = 0;
    // u--v--w--x
    for (int v = 0; v < n; ++v)
	for (int w : g.neighbors(v).above(v))
	    for (int u : g.neighbors(v) - g.neighbors(w))
		count += (g.neighbors(w) - g.neighbors(v) - g.neighbors(u)).size();
    return count;
}

uint64_t countInducedP5s(const Graph& g) {
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

bool oddHoleExtend(const Graph& g, int l, int r, Set out) {
    for (int l2 : g.neighbors(l) - g.neighbors(r) - out) {
	Set r2s = g.neighbors(r) - g.neighbors(l) - out;
	if ((g.neighbors(l2) & r2s).nonempty())
	    return true;
	for (int r2 : r2s)
	    if (oddHoleExtend(g, l2, r2, (out + l + r) | g.neighbors(l) | g.neighbors(r)))
		return true;
    }
    return false;
}

// induced cycle of odd length >= 5 (5, 7, 9, ...)
bool hasOddHole(const Graph& g) {
    for (int u = 0; u < g.n(); ++u) {
	for (int l : g.neighbors(u).above(u)) {
	    for (int r : g.neighbors(u).above(l) - g.neighbors(l)) {
		Set out = g.vertices().belowEq(u) | g.neighbors(u);
		if (oddHoleExtend(g, l, r, out))
		    return true;
	    }
	}
    }
    return false;
}

}  // namespace Subgraph
