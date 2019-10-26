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

#ifndef TINYGRAPH_CLASSES_HH_INCLUDED
#define TINYGRAPH_CLASSES_HH_INCLUDED

#include "Graph.hh"
#include "Subgraph.hh"

namespace Classes {

inline bool isClique(const Graph& g) { return g.m() == (g.n() * (g.n() - 1)) / 2; }
inline bool isIndependentSet(const Graph& g) {
    for (int u = 0; u < g.n(); ++u)
	if (g.neighbors(u).nonempty())
	    return false;
    return true;
}
bool isBipartite(const Graph& g);
inline bool isClusterGraph(const Graph& g) { return !Subgraph::hasInducedP3(g); }
bool isTriviallyPerfect(const Graph& g);
bool isChordal(const Graph& g);
bool isPerfect(const Graph& g);
bool isWeaklyPerfect(const Graph& g);
bool isSplit(const Graph& g);
bool isThreshold(const Graph& g);
bool isP4Sparse(const Graph& g);
bool isMonopolar(const Graph& g);
bool isSplitClusterGraph(const Graph& g);
bool independencePolynomialHasFactorXPlus1(const Graph& g);
bool isEulerian(const Graph& g);
bool isPrime(const Graph& g);
bool isWeaklyChordal(const Graph& g);
bool isHamiltonian(const Graph& g);
bool isWellCovered(const Graph& g);
bool isTree(const Graph& g);
bool isDistanceHereditary(const Graph& g);
bool isATFree(const Graph& g);
bool isElementary(const Graph& g);
bool isHoang(const Graph& g);
bool isTwoSplit(const Graph& g);
bool isPlanar(const Graph& g);
bool isCograph(const Graph& g);

bool isTwoVertexConnected(const Graph& g);
bool isTwoEdgeConnected(const Graph& g);
bool isMinimallyTwoEdgeConnected(const Graph& g);
bool isMinimallyTwoVertexConnected(const Graph& g);

}  // namespace Classes

#endif  // TINYGRAPH_CLASSES_HH_INCLUDED
