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

#include <chrono>
#include <map>
#include <functional>

auto propertyName = "minimally-two-edge-connected";
bool connectedOnly = false;

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

static Graph p4 = Graph::byName("P4");
static Graph k4 = Graph::byName("K4");
static Graph fork = Graph::byName("fork");
static Graph p5 = Graph::byName("P5");
static Graph banner = Graph::byName("banner");
static Graph bull = Graph::byName("bull");
static Graph c5 = Graph::byName("C5");
static Graph house = Graph::byName("house");
static Graph k23 = Graph::byName("K2,3");
static Graph dart = Graph::byName("dart");
static Graph bowtie = Graph::byName("bowtie");
static Graph gem = Graph::byName("gem");
static Graph w4 = Graph::byName("W4");
static Graph k5 = Graph::byName("K5");

bool isMinimallyTwoEdgeConnected(const Graph& g) {
    if (!Classes::isTwoEdgeConnected(g))
	return false;
    Graph g2 = g;
    for (int u : g.vertices()) {
	for (int v : g.neighbors(u).above(u)) {
	    g2.removeEdge(u, v);
	    if (Classes::isTwoEdgeConnected(g2))
		return false;
	    g2.addEdge(u, v);
	}
    }
    return true;
}

bool isMinimallyTwoVertexConnected(const Graph& g) {
    if (!Classes::isTwoVertexConnected(g))
	return false;
    for (int u : g.vertices()) {
	Graph g2 = g;
	g2.deleteVertex(u);
	if (Classes::isTwoVertexConnected(g2))
	    return false;
    }
    return true;
}

std::map<std::string, Property> properties = {
    {"triangle-free",         {[](const Graph& g) { return !Subgraph::hasK3(g); },               true,  true}},
    {"squarefree",            {[](const Graph& g) { return !Subgraph::hasC4(g); },               true,  true}},
    {"chordal",               {Classes::isChordal,                                               true,  true}},
    {"induced-claw-free",     {[](const Graph& g) { return !Subgraph::hasInducedClaw(g); },      true,  true}},
    {"induced-C4-free",       {[](const Graph& g) { return !Subgraph::hasInducedC4(g); },        true,  true}},
    {"induced-P4-free",       {[](const Graph& g) { return !Subgraph::hasInduced(g, p4); },      true,  true}},
    {"induced-paw-free",      {[](const Graph& g) { return !Subgraph::hasInducedPaw(g); },       true,  true}},
    {"induced-diamond-free",  {[](const Graph& g) { return !Subgraph::hasInducedDiamond(g); },   true,  true}},
    {"K4-free",               {[](const Graph& g) { return !Subgraph::hasInduced(g, k4); },      true,  true}},
    {"induced-fork-free",     {[](const Graph& g) { return !Subgraph::hasInduced(g, fork); },    true,  true}},
    {"induced-P5-free",       {[](const Graph& g) { return !Subgraph::hasInducedP5(g); },        true,  true}},
    {"induced-banner-free",   {[](const Graph& g) { return !Subgraph::hasInduced(g, banner); },  true,  true}},
    {"induced-bull-free",     {[](const Graph& g) { return !Subgraph::hasInduced(g, bull); },    true,  true}},
    {"induced-C5-free",       {[](const Graph& g) { return !Subgraph::hasInduced(g, c5); },      true,  true}},
    {"induced-house-free",    {[](const Graph& g) { return !Subgraph::hasInduced(g, house); },   true,  true}},
    {"induced-K2,3-free",     {[](const Graph& g) { return !Subgraph::hasInduced(g, k23); },     true,  true}},
    {"induced-dart-free",     {[](const Graph& g) { return !Subgraph::hasInduced(g, dart); },    true,  true}},
    {"induced-bowtie-free",   {[](const Graph& g) { return !Subgraph::hasInduced(g, bowtie); },  true,  true}},
    {"induced-gem-free",      {[](const Graph& g) { return !Subgraph::hasInduced(g, gem); },     true,  true}},
    {"K5-free",               {[](const Graph& g) { return !Subgraph::hasInduced(g, k5); },      true,  true}},
    {"3-colorable",           {[](const Graph& g) { return Invariants::kColorable(g, 3); },      true,  true}},
    {"4-colorable",           {[](const Graph& g) { return Invariants::kColorable(g, 4); },      true,  true}},
    {"5-colorable",           {[](const Graph& g) { return Invariants::kColorable(g, 5); },      true,  true}},
    {"6-colorable",           {[](const Graph& g) { return Invariants::kColorable(g, 6); },      true,  true}},
    {"7-colorable",           {[](const Graph& g) { return Invariants::kColorable(g, 7); },      true,  true}},
    {"8-colorable",           {[](const Graph& g) { return Invariants::kColorable(g, 8); },      true,  true}},
    {"9-colorable",           {[](const Graph& g) { return Invariants::kColorable(g, 9); },      true,  true}},
    {"perfect",               {[](const Graph& g) { return Classes::isPerfect(g); },             true,  true}},
    {"split",                 {[](const Graph& g) { return Classes::isSplit(g); },               true,  false}},
    {"threshold",             {[](const Graph& g) { return Classes::isThreshold(g); },           true,  false}},
    {"P4-sparse",             {[](const Graph& g) { return Classes::isP4Sparse(g); },            true,  true}},
    {"monopolar",             {[](const Graph& g) { return Classes::isMonopolar(g); },           true,  true}},
    {"split-cluster",         {[](const Graph& g) { return Classes::isSplitClusterGraph(g); },   true,  false}},
    {"two-edge-connected",    {[](const Graph& g) { return Classes::isTwoEdgeConnected(g); },    false, false}},
    {"two-vertex-connected",  {[](const Graph& g) { return Classes::isTwoVertexConnected(g); },  false, false}},
    {"minimally-two-edge-connected",  {[](const Graph& g) { return isMinimallyTwoEdgeConnected(g); },  false, false}},
    {"minimally-two-vertex-connected",  {[](const Graph& g) { return isMinimallyTwoVertexConnected(g); },  false, false}},
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
    if (property.determinedByConnectedComponents)
	connectedOnly = false;
    std::vector<uint64_t> counts;
    std::vector<double> times;
    for (int n = 0; n <= MAXN; ++n) {
	auto tStart = std::chrono::steady_clock::now();
	std::cerr << "--- n = " << n;
	if (times.size() >= 2) {
	    auto tn = times.back();
	    auto tn1 = times[times.size() - 2];
	    auto est = tn * (tn / tn1);
	    std::cerr << " estimated time: " << est << 's' << std::endl;
	    if (est > 1e6)
		return 0;
	} else {
	    std::cerr << std::endl;
	}
	uint64_t count = 0;
	auto counter = [&count](const Graph& g) { if (property.test(g)) ++count; };
	auto flags = connectedOnly || property.determinedByConnectedComponents ? Graph::CONNECTED : 0;
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
	if (connectedOnly) {
	    std::cout << "number of connected " << propertyName
		      << " undirected unlabeled graph on n vertices:\n"
		      << counts << std::endl;
	    std::cout << "number of connected non-" << propertyName
		      << " undirected unlabeled graph on n vertices:\n"
		      << EulerTransform::connectedNonGraphs(counts) << std::endl;
	} else if (!property.determinedByConnectedComponents) {
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
