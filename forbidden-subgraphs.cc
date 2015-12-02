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

#include "Classes.hh"

using PropertyTest = std::function<bool(const Graph&)>;

PropertyTest property = Classes::isSplit;

bool isMinimalForbidden(const Graph& g, PropertyTest p) {
    if (p(g))
	return false;
    for (int u : g.vertices()) {
	Graph g2 = g;
	g2.deleteVertex(u);
	if (!p(g2))
	    return false;
    }
    return true;
}

int main() {
    for (int n = 0; n <= MAXN; ++n) {
	std::cerr << "--- n = " << n << std::endl;
	Graph::enumerate(n, [](const Graph& g) {
		if (isMinimalForbidden(g, property))
		    std::cout << g.toString() << std::endl;
	    });
    }
    return 0;
}
