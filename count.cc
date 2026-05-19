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
#include <optional>

auto maxCpuTime = 1e6;

using PropertyTest = std::function<bool(const Graph&)>;

struct Property {
    Property(PropertyTest t, bool h, bool d)
	: test(t), hereditary(h), determinedByConnectedComponents(d) { }
    PropertyTest test;
    // if G has the property, then also every induced subgraph of G has
    bool hereditary;
    // G has the property if and only if each connected component has the property
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
    {"cochordal",{[](const Graph& g) { return Classes::isChordal(g.complement()); },true,  true}},
    {"eulerian",      {Classes::isEulerian,                                         false, true}},
    {"hamiltonian",   {Classes::isHamiltonian,                                      false, false}},
    {"long-hole-free",{[](const Graph& g) { return !Subgraph::hasLongHole(g); },    true,  true}},
    {"monopolar",     {Classes::isMonopolar,                                        true,  true}},
    {"odd-hole-free", {[](const Graph& g) { return !Subgraph::hasOddHole(g); },     true,  true}},
    {"perfect",       {Classes::isPerfect,                                          true,  true}},
    {"well-covered",  {Classes::isWellCovered,                                      false, true}},
    {"prime",         {Classes::isPrime,                                            false, false}},
    {"split",         {Classes::isSplit,                                            true,  false}},
    {"split-cluster", {Classes::isSplitClusterGraph,                                true,  false}},
    {"threshold",     {Classes::isThreshold,                                        true,  false}},
    {"trivially-perfect", {Classes::isTriviallyPerfect,                             true,  true}},
    {"weakly-chordal",{Classes::isWeaklyChordal,                                    true,  true}},
    {"weakly-perfect",{Classes::isWeaklyPerfect,                                    false, false}},
    {"distance-hereditary", {Classes::isDistanceHereditary,                         true,  true}},
    {"AT-free",       {Classes::isATFree,                                           true,  true}},
    {"elementary",    {Classes::isElementary,                                       true,  true}},
    {"Hoang",         {Classes::isHoang,                                            true,  true}},
    {"two-split",     {Classes::isTwoSplit,                                         true,  false}},
    {"planar",        {Classes::isPlanar,                                           true,  true}},
    {"cograph",       {Classes::isCograph,                                          true,  true}},
    {"asymmetric",    {Classes::isAsymmetric,                                       false, false}},
    {"simplicial-free", {Classes::isSimplicialFree,                                 false, true}},
    {"two-edge-connected",              {Classes::isTwoEdgeConnected,               false, false}},
    {"minimally-two-edge-connected",    {Classes::isMinimallyTwoEdgeConnected,      false, false}},
    {"minimally-two-vertex-connected",  {Classes::isMinimallyTwoVertexConnected,    false, false}},
};

struct IntInvariant {
    std::function<int(const Graph&)> f;
    enum Combine { COMBINE_NONE, COMBINE_MAX, COMBINE_SUM } combine;
    bool monotone;          // f(induced subgraph) <= f(G)
    bool requiresConnected; // restrict enumeration to connected graphs
};

std::map<std::string, IntInvariant> intInvariants = {
    {"diameter", {Invariants::diameter,       IntInvariant::COMBINE_NONE, false, true }},
    {"radius",   {Invariants::radius,         IntInvariant::COMBINE_NONE, false, true }},
    {"chi",      {Invariants::coloringNumber, IntInvariant::COMBINE_MAX,  true,  false}},
    {"omega",    {Invariants::cliqueNumber,   IntInvariant::COMBINE_MAX,  true,  false}},
    {"m",        {[](const Graph& g) { return g.m(); },
		  IntInvariant::COMBINE_SUM, true, false}},
};

enum class CmpOp { LE, LT, GE, GT, EQ, NE };

static bool findOp(const std::string& s, size_t& opStart, size_t& opLen, CmpOp& op) {
    for (size_t i = 0; i < s.size(); ++i) {
	if (i + 1 < s.size()) {
	    auto two = s.substr(i, 2);
	    if (two == "<=") { opStart = i; opLen = 2; op = CmpOp::LE; return true; }
	    if (two == ">=") { opStart = i; opLen = 2; op = CmpOp::GE; return true; }
	    if (two == "==") { opStart = i; opLen = 2; op = CmpOp::EQ; return true; }
	    if (two == "!=") { opStart = i; opLen = 2; op = CmpOp::NE; return true; }
	}
	char c = s[i];
	if (c == '<') { opStart = i; opLen = 1; op = CmpOp::LT; return true; }
	if (c == '>') { opStart = i; opLen = 1; op = CmpOp::GT; return true; }
	if (c == '=') { opStart = i; opLen = 1; op = CmpOp::EQ; return true; }
    }
    return false;
}

static const char* opName(CmpOp op) {
    switch (op) {
    case CmpOp::LE: return "<=";
    case CmpOp::LT: return "<";
    case CmpOp::GE: return ">=";
    case CmpOp::GT: return ">";
    case CmpOp::EQ: return "==";
    case CmpOp::NE: return "!=";
    }
    return "?";
}

std::map<std::string, GengProperty> gengProperties = {
    {"biconnected",           {Graph::BICONNECTED,   false, false}},
    {"two-vertex-connected",  {Graph::BICONNECTED,   false, false}},
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
    bool hereditary = true;
    bool connectedOnly = false;
    PropertyTest propertyTest = 0;
    std::string propertyName = "";
    int gengFlags = 0;
    bool isRegular = false;
    std::optional<int> regularDegree;
    int i = 1;
    bool countLabeled = false;
    if (i < argc && std::string(argv[i]) == "-l") {
	countLabeled = true;
	++i;
    }
    bool determinedByConnectedComponents = !countLabeled;
    for (; i < argc; ++i) {
	std::string type = argv[i];
	PropertyTest test = 0;
	if (type == "connected") {
	    connectedOnly = true;
	    gengFlags |= Graph::CONNECTED;
	    continue;
	} else if (type == "tree") {
	    connectedOnly = true;
	    gengFlags |= Graph::TREE;
	    continue;
	} else if (type == "regular"
		   || (endsWith(type, "-regular")
		       && type.find_first_not_of("0123456789") == type.size() - std::string("-regular").size())) {
	    if (isRegular) {
		std::cerr << "regular specified more than once\n";
		exit(1);
	    }
	    isRegular = true;
	    hereditary = false;
	    if (type != "regular")
		regularDegree = std::stoi(type.substr(0, type.size() - std::string("-regular").size()));
	    else
		determinedByConnectedComponents = false;
	    if (propertyName != "")
		propertyName += ' ';
	    propertyName += type;
	    continue;
	}
	{
	    size_t opStart, opLen;
	    CmpOp op;
	    if (findOp(type, opStart, opLen, op)
		&& intInvariants.count(type.substr(0, opStart))) {
		std::string name = type.substr(0, opStart);
		int rhs;
		try {
		    rhs = std::stoi(type.substr(opStart + opLen));
		} catch (...) {
		    std::cerr << "invalid number in " << type << '\n';
		    exit(1);
		}
		const auto& inv = intInvariants.find(name)->second;
		auto f = inv.f;
		switch (op) {
		case CmpOp::LE: test = [f, rhs](const Graph& g) { return f(g) <= rhs; }; break;
		case CmpOp::LT: test = [f, rhs](const Graph& g) { return f(g) <  rhs; }; break;
		case CmpOp::GE: test = [f, rhs](const Graph& g) { return f(g) >= rhs; }; break;
		case CmpOp::GT: test = [f, rhs](const Graph& g) { return f(g) >  rhs; }; break;
		case CmpOp::EQ: test = [f, rhs](const Graph& g) { return f(g) == rhs; }; break;
		case CmpOp::NE: test = [f, rhs](const Graph& g) { return f(g) != rhs; }; break;
		}
		bool upperBound = (op == CmpOp::LE || op == CmpOp::LT);
		bool isHereditary = inv.monotone && upperBound;
		bool isDByCC = (inv.combine == IntInvariant::COMBINE_MAX) && upperBound;
		hereditary &= isHereditary;
		determinedByConnectedComponents &= isDByCC;
		if (inv.requiresConnected) {
		    connectedOnly = true;
		    gengFlags |= Graph::CONNECTED;
		}
		if (propertyName != "")
		    propertyName += ' ';
		propertyName += name;
		propertyName += opName(op);
		propertyName += std::to_string(rhs);
		if (!propertyTest)
		    propertyTest = test;
		else
		    propertyTest = [propertyTest, test](const Graph& g) { return propertyTest(g) && test(g); };
		continue;
	    }
	}
	if (gengProperties.find(type) != gengProperties.end()) {
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
	    auto hasSubgraph = induced ? Subgraph::hasInducedTest(f) : Subgraph::hasTest(f);
	    test = [hasSubgraph](const Graph& g) { return !hasSubgraph(g); };
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
    std::vector<bignum> counts;
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
	bignum count = 0;
	auto counter = [&count,&propertyTest,&countLabeled](const Graph& g) {
			   if (propertyTest(g))
			       count += countLabeled ? g.numLabeledGraphs() : 1;
		       };
	auto prune = [propertyTest](const Graph& g) { return !propertyTest(g); };
	auto enumerate = [&](std::optional<int> mindeg, std::optional<int> maxdeg) {
	    if (doPrune)
		Graph::enumerate(n, counter, prune, gengFlags, mindeg, maxdeg);
	    else
		Graph::enumerate(n, counter, gengFlags, mindeg, maxdeg);
	};
	auto feasibleRegular = [n, gengFlags](int k) {
	    if (k < 0 || k >= n)
		return false;
	    if ((n * k) % 2 != 0)
		return false;
	    int m = n * k / 2;
	    if ((gengFlags & Graph::CONNECTED) && m < n - 1)
		return false;
	    if ((gengFlags & Graph::BICONNECTED) && n > 2 && (k < 2 || m < n))
		return false;
	    if ((gengFlags & Graph::TREE) && m != n - 1)
		return false;
	    return true;
	};
	if (isRegular && n > 0) {
	    if (regularDegree) {
		if (feasibleRegular(*regularDegree))
		    enumerate(*regularDegree, *regularDegree);
	    } else {
		for (int k = 0; k < n; ++k) {
		    if (!feasibleRegular(k))
			continue;
		    enumerate(k, k);
		}
	    }
	} else {
	    enumerate(std::nullopt, std::nullopt);
	}
	counts.push_back(count);
	auto tEnd = double(std::clock()) / CLOCKS_PER_SEC;
	double t = tEnd - tStart;
	times.push_back(t);
	std::cerr << "time: " << t << 's' << std::endl;
	std::string un = countLabeled ? "" : "un";
	if (connectedOnly && !determinedByConnectedComponents) {
	    std::cout << "number of connected " << propertyName
		      << " undirected " << un << "labeled graph on n vertices:\n"
		      << counts << std::endl;
	    if (!countLabeled)
		std::cout << "number of connected non-" << propertyName
			  << " undirected " << un << "labeled graph on n vertices:\n"
			  << EulerTransform::connectedNonGraphs(counts) << std::endl;
	} else if (!determinedByConnectedComponents) {
	    std::cout << "number of " << propertyName
		      << " undirected " << un << "labeled graph on n vertices:\n"
		      << counts << std::endl;
	    if (!countLabeled)
		std::cout << "number of non-" << propertyName
			  << " undirected " << un << "labeled graph on n vertices:\n"
			  << EulerTransform::nonGraphs(counts) << std::endl;
	} else {
	    auto countsGeneral = EulerTransform::transform(counts);
	    std::cout << "number of " << propertyName
		      << " undirected " << un << "labeled graph on n vertices:\n"
		      << countsGeneral << std::endl;
	    if (!countLabeled) {
		std::cout << "number of non-" << propertyName
			  << " undirected " << un << "labeled graph on n vertices:\n"
			  << EulerTransform::nonGraphs(countsGeneral) << std::endl;
		std::cout << "number of " << propertyName
			  << " connected undirected " << un << "labeled graph on n vertices:\n"
			  << counts << std::endl;
		std::cout << "number of non-" << propertyName
			  << " connected undirected " << un << "labeled graph on n vertices:\n"
			  << EulerTransform::connectedNonGraphs(counts) << std::endl;
	    }
	}
    }

    return 0;
}
