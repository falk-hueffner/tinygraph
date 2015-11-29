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

namespace Classes {

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

}  // namespace Classes
