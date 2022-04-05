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

#ifndef TINYGRAPH_SUBGRAPH_HH_INCLUDED
#define TINYGRAPH_SUBGRAPH_HH_INCLUDED

#include "Graph.hh"

namespace Subgraph {

std::function<uint64_t(const Graph&)> countInducedFunction(Graph f);
uint64_t countInduced(const Graph &g, const Graph& f);
uint64_t countInducedP3s(const Graph& g);
uint64_t countInducedP4s(const Graph& g);
uint64_t countInducedP5s(const Graph& g);
uint64_t countInducedClaws(const Graph& g);
uint64_t countInducedPaws(const Graph& g);
uint64_t countInducedC4s(const Graph& g);
uint64_t countInducedCycles(const Graph& g);

std::function<bool(const Graph&)> hasTest(Graph f);
std::function<bool(const Graph&)> hasInducedTest(Graph f);
bool hasInduced(const Graph &g, const Graph& f);
bool hasInducedP3(const Graph &g);
bool hasK3(const Graph &g);
bool hasK4(const Graph &g);
bool hasInducedClaw(const Graph &g);
bool hasInducedPaw(const Graph &g);
bool hasC4(const Graph &g);
bool hasInducedC4(const Graph &g);
bool hasInducedDiamond(const Graph &g);
bool hasInducedP5(const Graph &g);
bool hasInducedC5(const Graph &g);
bool hasInducedFork(const Graph &g);
bool hasInducedHouse(const Graph &g);
bool hasInducedBull(const Graph &g);
bool hasLongHole(const Graph& g);
bool hasOddHole(const Graph& g);

}  // namespace Subgraph

#endif  // TINYGRAPH_SUBGRAPH_HH_INCLUDED
