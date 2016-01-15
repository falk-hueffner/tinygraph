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

#include "Classes.hh"
#include "EulerTransform.hh"
#include "Graph.hh"
#include "Invariants.hh"
#include "Subgraph.hh"

#include <ctime>
#include <map>
#include <functional>

auto maxCpuTime = 1e6;

using PropertyTest = std::function<bool(const Graph&)>;

struct Property {
    Property(PropertyTest t, bool h, bool d)
	: test(t), hereditary(h), determinedByConnectedComponents(d) { }
    PropertyTest test;
    // if G has the property, then also every induced subgraph of G has
    bool hereditary;
    // G has the property if and only if each connected subgraph has the property
    bool determinedByConnectedComponents;
};

struct GengProperty {
    GengProperty(int f, bool h, bool d)
	: flag(f), hereditary(h), determinedByConnectedComponents(d) { }
    int flag;
    bool hereditary;
    bool determinedByConnectedComponents;
};

std::map<std::string, Property> properties = {
    {"3-colorable",   {[](const Graph& g) { return Invariants::kColorable(g, 3); }, true,  true}},
    {"4-colorable",   {[](const Graph& g) { return Invariants::kColorable(g, 4); }, true,  true}},
    {"5-colorable",   {[](const Graph& g) { return Invariants::kColorable(g, 5); }, true,  true}},
    {"6-colorable",   {[](const Graph& g) { return Invariants::kColorable(g, 6); }, true,  true}},
    {"7-colorable",   {[](const Graph& g) { return Invariants::kColorable(g, 7); }, true,  true}},
    {"8-colorable",   {[](const Graph& g) { return Invariants::kColorable(g, 8); }, true,  true}},
    {"9-colorable",   {[](const Graph& g) { return Invariants::kColorable(g, 9); }, true,  true}},
    {"P4-sparse",     {Classes::isP4Sparse,                                         true,  true}},
    {"bipartite",     {Classes::isBipartite,                                        true,  true}},
    {"chordal",       {Classes::isChordal,                                          true,  true}},
    {"eulerian",      {Classes::isEulerian,                                         false, true}},
    {"long-hole-free",{[](const Graph& g) { return !Subgraph::hasLongHole(g); },    true,  true}},
    {"monopolar",     {Classes::isMonopolar,                                        true,  true}},
    {"odd-hole-free", {[](const Graph& g) { return !Subgraph::hasOddHole(g); },     true,  true}},
    {"perfect",       {Classes::isPerfect,                                          true,  true}},
    {"prime",         {Classes::isPrime,                                            false, false}},
    {"split",         {Classes::isSplit,                                            true,  false}},
    {"split-cluster", {Classes::isSplitClusterGraph,                                true,  false}},
    {"threshold",     {Classes::isThreshold,                                        true,  false}},
    {"trivially-perfect", {Classes::isTriviallyPerfect,                             true,  true}},
    {"weakly-chordal",{Classes::isWeaklyChordal,                                    true,  true}},
};

std::map<std::string, GengProperty> gengProperties = {
    {"biconnected",           {Graph::BICONNECTED,   false, false}},
    {"triangle-free",         {Graph::TRIANGLE_FREE, true,  true}},
    {"induced-triangle-free", {Graph::TRIANGLE_FREE, true,  true}},
    {"square-free",           {Graph::SQUARE_FREE,   true,  true}},
    {"C4-free",               {Graph::SQUARE_FREE,   true,  true}},
    {"bipartite",             {Graph::BIPARTITE,     true,  true}},
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
    for (size_t i = 0; i < v.size(); ++i) {
	if (i)
	    out << ", ";
	out << v[i];
    }
    return out;
}

bool startsWith(const std::string& s, const std::string& a) {
    return s.compare(0, a.length(), a) == 0;
}

bool endsWith(const std::string& s, const std::string& e) {
    if (s.length() < e.length())
	return false;
    return s.compare(s.length() - e.length(), e.length(), e) == 0;
}

int main(int argc, char* argv[]) {
    bool determinedByConnectedComponents = true;
    bool hereditary = true;
    bool connectedOnly = false;
    PropertyTest propertyTest = 0;
    std::string propertyName = "";
    int gengFlags = 0;
    for (int i = 1; i < argc; ++i) {
	std::string type = argv[i];
	PropertyTest test = 0;
	if (type == "connected") {
	    connectedOnly = true;
	    gengFlags |= Graph::CONNECTED;
	    continue;
	} else if (gengProperties.find(type) != gengProperties.end()) {
	    auto p = gengProperties.find(type)->second;
	    gengFlags |= p.flag;
	    hereditary &= p.hereditary;
	    determinedByConnectedComponents &= p.determinedByConnectedComponents;
	    if (propertyName != "")
		propertyName += ' ';
	    propertyName += type;
	    continue;
	} else if (properties.find(type) != properties.end()) {
	    auto p = properties.find(type)->second;
	    test = p.test;
	    hereditary &= p.hereditary;
	    determinedByConnectedComponents &= p.determinedByConnectedComponents;
	    if (propertyName != "")
		propertyName += ' ';
	    propertyName += type;
	} else if (endsWith(type, "-free")) {
	    if (propertyName != "")
		propertyName += ' ';
	    type = type.substr(0, type.length() - std::string("-free").length());
	    bool induced = false;
	    if (startsWith(type, "induced-")) {
		type = type.substr(std::string("induced-").length());
		induced = true;
		propertyName += "(induced) ";
	    } else {
		propertyName += "(not necessarily induced) ";
	    }
	    Graph f = Graph::byName(type);
	    propertyName += f.name() + "-free";
	    test = std::not1(induced ? Subgraph::hasInducedTest(f) : Subgraph::hasTest(f));
	    determinedByConnectedComponents &= f.isConnected();
	} else {
	    std::cerr << "unknown graph class\n";
	    exit(1);
	}
	if (!propertyTest)
	    propertyTest = test;
	else
	    propertyTest = [propertyTest, test](const Graph& g) { return propertyTest(g) && test(g); };
    }
    if (determinedByConnectedComponents)
	gengFlags |= Graph::CONNECTED;
    bool doPrune = hereditary && propertyTest;
    if (!propertyTest)
	propertyTest = [](const Graph&) { return true; };
    std::vector<uint64_t> counts;
    std::vector<double> times;
    for (int n = 0; n <= MAXN; ++n) {
	auto tStart = double(std::clock()) / CLOCKS_PER_SEC;
	std::cerr << "--- n = " << n;
	if (times.size() >= 2) {
	    auto tn = times.back();
	    auto tn1 = times[times.size() - 2];
	    auto est = tn * (tn / tn1);
	    std::cerr << " estimated time: " << est << 's' << std::endl;
	    if (est > maxCpuTime)
		return 0;
	} else {
	    std::cerr << std::endl;
	}
	uint64_t count = 0;
	auto counter = [&count,&propertyTest](const Graph& g) { if (propertyTest(g)) ++count; };
	if (doPrune) {
	    Graph::enumerate(n, counter, std::not1(propertyTest), gengFlags);
	} else {
	    Graph::enumerate(n, counter, gengFlags);
	}
	counts.push_back(count);
	auto tEnd = double(std::clock()) / CLOCKS_PER_SEC;
	double t = tEnd - tStart;
	times.push_back(t);
	std::cerr << "time: " << t << 's' << std::endl;
	if (connectedOnly && !determinedByConnectedComponents) {
	    std::cout << "number of connected " << propertyName
		      << " undirected unlabeled graph on n vertices:\n"
		      << counts << std::endl;
	    std::cout << "number of connected non-" << propertyName
		      << " undirected unlabeled graph on n vertices:\n"
		      << EulerTransform::connectedNonGraphs(counts) << std::endl;
	} else if (!determinedByConnectedComponents) {
	    std::cout << "number of " << propertyName
		      << " undirected unlabeled graph on n vertices:\n"
		      << counts << std::endl;
	    std::cout << "number of non-" << propertyName
		      << " undirected unlabeled graph on n vertices:\n"
		      << EulerTransform::nonGraphs(counts) << std::endl;
	} else {
	    auto countsGeneral = EulerTransform::transform(counts);
	    std::cout << "number of " << propertyName
		      << " undirected unlabeled graph on n vertices:\n"
		      << countsGeneral << std::endl;
	    std::cout << "number of non-" << propertyName
		      << " undirected unlabeled graph on n vertices:\n"
		      << EulerTransform::nonGraphs(countsGeneral) << std::endl;
	    std::cout << "number of " << propertyName
		      << " connected undirected unlabeled graph on n vertices:\n"
		      << counts << std::endl;
	    std::cout << "number of non-" << propertyName
		      << " connected undirected unlabeled graph on n vertices:\n"
		      << EulerTransform::connectedNonGraphs(counts) << std::endl;
	}
    }

    return 0;
}
