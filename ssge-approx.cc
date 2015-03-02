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

int choose2(int n) {
    return (n * (n - 1)) / 2;
}

int sparseSplitGraphEditing(const Graph& g) {
    Set vs = g.vertices();
    int m = g.m();
    int minEdits = m;
    for (Set clique : vs.subsets()) {
	int c = clique.size();
	int m_c = g.mSubgraph(clique);
	int edits = (choose2(c) - m_c) + (m - m_c);
	if (edits < minEdits)
	    minEdits = edits;
    }
    return minEdits;
}

int sparseSplitGraphEditingApproxWorstCase(const Graph& g) {
    int n = g.n();
    int m = g.m();
    Set degs[n];
    for (int u : g.vertices())
	degs[g.deg(u)].add(u);
    int minEdits = m;
    for (int c = 0; c <= n; ++c) {
	Set clique1;
	int maxEdits = 0;
	int i;
	for (i = n - 1; clique1.size() + degs[i].size() < c; --i)
	    clique1 |= degs[i];
	for (Set clique2 : degs[i].combinations(c - clique1.size())) {
	    Set clique = clique1 | clique2;
	    assert(clique.size() == c);
	    int m_c = g.mSubgraph(clique);
	    int edits = (choose2(c) - m_c) + (m - m_c);
	    if (edits > maxEdits)
		maxEdits = edits;
	}
	if (maxEdits < minEdits)
	    minEdits = maxEdits;
    }
    return minEdits;
}

std::string graphToString(const Graph& g) {
    std::string r = "{[" + std::to_string(g.n()) + "] ";
    bool start = true;
    for (int u = 0; u < g.n(); ++u) {
        for (int v = u + 1; v < g.n(); ++v) {
            if (g.hasEdge(u, v)) {
                if (!start)
                    r += ", ";
                start = false;
                r += std::string("{") + std::to_string(u) + ", " + std::to_string(v) + '}';
            }
        }
    }
    r += '}';
    return r;
}

int main() {
    double max_f = 0;
    for (int n = 1; n <= MAXN; ++n) {
	double max_f_n = 0;
	std::cout << "--- n = " << n << std::endl;
	Graph::enumerate(n, [&max_f, &max_f_n](const Graph& g) {
		for (int u : g.vertices())
                    if (g.deg(u) == 0)
                        return;
                int k_opt = sparseSplitGraphEditing(g);
                int k = sparseSplitGraphEditingApproxWorstCase(g);
                double f = double(k) / k_opt;
                if (f > max_f_n) {
		    if (f > max_f) {
			std::cout << "* ";
			max_f = f;
		    } else {
			std::cout << "  ";
		    }
		    std::cout << "f = " << f << " (" << k << '/' << k_opt << ") " << graphToString(g) << std::endl;
		    max_f_n = f;
		}
	    }, Graph::CONNECTED);
    }

    return 0;
}
