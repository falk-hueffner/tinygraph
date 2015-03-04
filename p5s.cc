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

uint64_t numP5s(const Graph& g) {
    int n = g.n();
    uint64_t num = 0;
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
			++num;
		    }
		}
	    }
	}
    }
    return num;
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
    uint64_t maxNum = 0;
    for (int n = 1; n <= MAXN; ++n) {
	std::cerr << "--- n = " << n << std::endl;
	Graph::enumerate(n, [&maxNum](const Graph& g) {
		uint64_t num = numP5s(g);
                if (num > maxNum) {
		    std::cout << num << ": " << graphToString(g) << std::endl;
		    maxNum = num;
		}
	    });
    }

    return 0;
}
