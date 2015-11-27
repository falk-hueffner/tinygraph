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
//auto countSubgraph = [](const Graph& g) { return Subgraph::count(g, subgraph); };
auto countSubgraph = Subgraph::countInducedP3s;

int main() {
    std::vector<uint64_t> maxCounts;
    for (int n = 0; n <= MAXN; ++n) {
	std::cerr << "--- n = " << n << std::endl;
	uint64_t maxCount = 0;
	Graph::enumerate(n, [&maxCount](const Graph& g) {
		uint64_t count = countSubgraph(g);
                if (count > maxCount) {
		    std::cerr << count << ": " << g.toString() << std::endl;
		    maxCount = count;
		}
	    });
	maxCounts.push_back(maxCount);
	std::cout << "maximum number of induced " << subgraphName
		  << " in an undirected unlabeled graph on n vertices:" << std::endl;
	for (size_t i = 0; i < maxCounts.size(); ++i) {
	    if (i)
		std::cout << ", ";
	    std::cout << maxCounts[i];
	}
	std::cout << std::endl;
    }

    return 0;
}
