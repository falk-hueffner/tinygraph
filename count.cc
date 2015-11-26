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

// Count the number of graphs on n nodes with a certain property.

#include "Graph.hh"
#include "Subgraph.hh"
#include "EulerTransform.hh"

#include <map>

auto propertyName = "claw-free";

using PropertyTest = std::function<bool(const Graph&)>;

struct Property {
    Property(PropertyTest t, bool d) : test(t), determinedByConnectedComponents(d) { }
    PropertyTest test;
    bool determinedByConnectedComponents;
};

bool clawFree(const Graph& g) {
    static const Graph claw = Graph::byName("claw");
    return !Subgraph::contains(g, claw);
}

std::map<std::string, Property> properties = {
    {"claw-free", {clawFree, true}},
};

auto property = properties.at(propertyName);

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
    for (size_t i = 0; i < v.size(); ++i) {
	if (i)
	    out << ", ";
	out << v[i];
    }
    return out;
}

int main() {
    std::vector<uint64_t> counts;
    for (int n = 0; n <= MAXN; ++n) {
	std::cerr << "--- n = " << n << std::endl;
	uint64_t count = 0;
	Graph::enumerate(n, [&count](const Graph& g) {if (property.test(g)) ++count; },
			 property.determinedByConnectedComponents ? Graph::CONNECTED : 0);
	counts.push_back(count);
	if (!property.determinedByConnectedComponents) {
	    std::cout << "number of " << propertyName
		      << " undirected unlabeled graph on n vertices:\n"
		      << counts << std::endl;
	} else {
	    auto countsGeneral = EulerTransform::transform(counts);
	    std::cout << "number of " << propertyName
		      << " undirected unlabeled graph on n vertices:\n"
		      << countsGeneral << std::endl;
	    std::cout << "number of " << propertyName
		      << " connected undirected unlabeled graph on n vertices:\n"
		      << counts << std::endl;
	}
    }

    return 0;
}
