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

#if 1

const auto subgraphName = "paw";
const auto subgraph = Graph::byName(subgraphName);
const auto subgraphTest = Subgraph::hasTest(subgraph);
struct Out {};

int main() {
    std::vector<std::uint64_t> result;
    for (int n = 0; ; ++n) {
        std::cerr << "n = " << n << std::endl;
        int m;
        for (m = 0; m <= n * (n-1)/2; ++m) {
            if (subgraph.isConnected() && m < n - 1)
                continue;
            std::cerr << "m = " << m << std::endl;
            try {
                Graph::enumerateEdges(n, [](const Graph& g) {
                        if (!subgraphTest(g))
                            throw Out();
                    }, m, subgraph.isConnected() ? Graph::CONNECTED : 0);
            } catch (Out o) {
                continue;
            }
            break;
        }
        result.push_back(m - 1);
	std::cout << "maximum number of edges in an n-vertex graph which does not have a subgraph isomorphic to "
                  << subgraph.name() << ":" << std::endl;
	for (size_t i = 0; i < result.size(); ++i) {
	    if (i)
		std::cout << ", ";
	    std::cout << result[i];
	}
	std::cout << std::endl;        
    }
}
#else

auto subgraphName = "C4";
auto subgraph = Graph::byName(subgraphName);
//auto countSubgraph = [](const Graph& g) { return Subgraph::countInduced(g, subgraph); };
auto countSubgraph = Subgraph::countInducedC4s;

size_t numMaximalCliques(const Graph& g) {
    size_t count = 0;
    g.maximalCliques([&count](const Set&) { ++count; });
    return count;
}

int main() {
    std::vector<uint64_t> maxCounts;
    for (int n = 0; n <= 110; ++n) {
	std::cerr << "--- n = " << n << std::endl;
	uint64_t maxCount = 0;
	Graph::enumerate(n, [&maxCount](const Graph& g) {
                uint64_t count = countSubgraph(g);
		//uint64_t count = numMaximalCliques(g);
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
#endif
