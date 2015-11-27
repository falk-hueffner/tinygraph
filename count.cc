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

#include <chrono>
#include <map>
#include <functional>

auto propertyName = "claw-free";

using PropertyTest = std::function<bool(const Graph&)>;

struct Property {
    Property(PropertyTest t, bool h, bool d)
	: test(t), hereditary(h), determinedByConnectedComponents(d) { }
    PropertyTest test;
    bool hereditary;
    bool determinedByConnectedComponents;
};

std::map<std::string, Property> properties = {
    {"claw-free", {[](const Graph& g) { return !Subgraph::containsClaw(g); }, true, true}},
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
    std::vector<double> times;
    for (int n = 0; n <= MAXN; ++n) {
	auto tStart = std::chrono::steady_clock::now();
	std::cerr << "--- n = " << n;
	if (times.size() >= 2) {
	    auto tn = times.back();
	    auto tn1 = times[times.size() - 2];
	    std::cerr << " estimated time: " << tn * (tn / tn1) << 's' << std::endl;
	} else {
	    std::cerr << std::endl;
	}
	uint64_t count = 0;
	auto counter = [&count](const Graph& g) { if (property.test(g)) ++count; };
	auto flags = property.determinedByConnectedComponents ? Graph::CONNECTED : 0;
	if (property.hereditary) {
	    Graph::enumerate(n, counter, std::not1(property.test), flags);
	} else {
	    Graph::enumerate(n, counter, flags);
	}
	counts.push_back(count);
	auto tEnd = std::chrono::steady_clock::now();
	double t =  (std::chrono::duration_cast<std::chrono::duration<double>>(tEnd - tStart)).count();
	times.push_back(t);
	std::cerr << "time: " << t << 's' << std::endl;
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
