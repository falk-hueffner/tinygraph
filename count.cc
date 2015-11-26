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
#include "Subgraph.hh"

auto subgraphName = "P3";
auto subgraph = Graph::byName(subgraphName);
// auto hasSubgraph = hasP3;
auto hasSubgraph = [](const Graph& g){ return Subgraph::contains(g, subgraph); };

int main() {
    std::vector<uint64_t> counts;
    for (int n = 0; n <= MAXN; ++n) {
	std::cerr << "--- n = " << n << std::endl;
	uint64_t count = 0;
	Graph::enumerate(n, [&count](const Graph& g) {
		if (!hasSubgraph(g))
		    ++count;
	    });
	counts.push_back(count);
	std::cout << "number of undirected unlabeled graph on n vertices that do not contain a "
		  << subgraphName << " as an induced subgraph" << std::endl;
	for (size_t i = 0; i < counts.size(); ++i) {
	    if (i)
		std::cout << ", ";
	    std::cout << counts[i];
	}
	std::cout << std::endl;
    }

    return 0;
}
