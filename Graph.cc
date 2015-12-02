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

#include "Graph.hh"

#include "nauty/nauty.h"

#include <algorithm>
#include <map>

// A&BvC: an A graph with an extra B vertices, each of which is attached to C vertices of the A
static const std::map<std::string, Graph> namedGraphs = {
    {"diamond", Graph::ofGraph6("Cz").canonical()},
    {"paw",     Graph::ofGraph6("Cx").canonical()},
    {"claw",    Graph::ofGraph6("Cs").canonical()},
    {"K5-e",    Graph::ofGraph6("D~k").canonical()},
    {"K4&v2",   Graph::ofGraph6("DN{").canonical()},
    {"W4",      Graph::ofGraph6("Dl{").canonical()},
    {"K4&v1",   Graph::ofGraph6("DJ{").canonical()},
    {"C4&v3",   Graph::ofGraph6("D]w").canonical()},
    {"gem",     Graph::ofGraph6("Dh{").canonical()},
    {"K2&3v2",  Graph::ofGraph6("DF{").canonical()},
    {"bowtie",  Graph::ofGraph6("D{c").canonical()},
    {"fork",    Graph::ofGraph6("DiC").canonical()},
    {"kite",    Graph::ofGraph6("DTw").canonical()},
    {"dart",    Graph::ofGraph6("DvC").canonical()},
    {"house",   Graph::ofGraph6("DUw").canonical()},
    {"banner",  Graph::ofGraph6("DrG").canonical()},
    {"tadpole", Graph::ofGraph6("DKs").canonical()},
    {"bull",    Graph::ofGraph6("D{O").canonical()},
    {"cricket", Graph::ofGraph6("DiS").canonical()},
};

Graph Graph::ofNauty(word* nautyg, int n) {
    Graph g(n);
    for (int i = 0; i < n; ++i)
	g.neighbors_[i] = Set::ofBits(reverseBits(nautyg[i]));
    return g;
}

Graph Graph::ofGraph6(std::string g6) {
    for (size_t i = 0; i < g6.size(); ++i)
        g6[i] -= 63;
    int n = g6[0];
    Graph g(n);
    int b = 0;
    for (int j = 0 ; j < n; ++j) {
        for (int i = 0; i < j; ++i) {
            int byte = 1 + (b / 6);
            int bit = 5 - (b % 6);
            if ((g6[byte] >> bit) & 1)
                g.addEdge(i, j);
            ++b;
        }
    }
    return g;
}

Graph Graph::byName(std::string name) {
    // claw, paw etc.
    auto p = namedGraphs.find(name);
    if (p != namedGraphs.end())
	return p->second;

    // Pn, Cn
    auto DIGITS = "0123456789";
    if (name.empty())
	throw std::invalid_argument("Graph::byName: empty name");
    if ((name[0] == 'P' || name[0] == 'C' || name[0] == 'K')
	&& name.find_first_not_of(DIGITS, 1) == std::string::npos) {
	auto type = name[0];
	name.erase(name.begin());
	int n = std::stoi(name);
	Graph g(n);
	if (type == 'K') {
	    for (int u = 0; u < n; ++u)
		for (int v = u + 1; v < n; ++v)
		    g.addEdge(u, v);
	} else {
	    for (int u = 0; u + 1 < n; ++u)
		g.addEdge(u, u + 1);
	    if (type == 'C')
		g.addEdge(n - 1, 0);
	}
	return g;
    }
    throw std::invalid_argument("Graph::byName: unknown graph " + name);
}

Graph Graph::cycle(int n) {
    Graph g(n);
    for (int u = 0; u + 1 < n; ++u)
	g.addEdge(u, u + 1);
    g.addEdge(n - 1, 0);
    return g;
}

bool Graph::isConnected() const {
    if (n() == 0)
	return true;
    Set q = {0};
    Set seen = {0};
    while (q.nonempty()) {
	int u = q.pop();
	q |= neighbors(u) - seen;
	seen |= neighbors(u);
	if (seen == vertices())
	    return true;
    }
    return false;
}

extern "C" {
    int geng_main(int argc, char* argv[]);
    void geng_outproc(FILE* f, word* nautyg, int n);
    int geng_prune(word* nautyg, int n, int maxn);
}

void geng_outproc(FILE*, word* nautyg, int n) {
    Graph g = Graph::ofNauty(nautyg, n);
    Graph::enumerateCallback()(g);
}

int geng_prune(word* nautyg, int n, int /*maxn*/) {
    if (!Graph::pruneCallback())
	return false;
    Graph g = Graph::ofNauty(nautyg, n);
    return Graph::pruneCallback()(g);
}

Graph Graph::canonical() const {
    word nautyg[n()];
    for (int i = 0; i < n(); ++i)
	nautyg[i] = reverseBits(neighbors(i).bits());
    int orbits[n()];
    DEFAULTOPTIONS_GRAPH(options);
    options.getcanon = true;
    int lab[n()];
    int ptn[n()];
    statsblk stats;
    word canonical[n()];
    densenauty(nautyg, lab, ptn, orbits, &options, &stats, 1, n(), canonical);
    Graph g(n());
    for (int i = 0; i < n(); ++i)
	g.neighbors_[i] = Set::ofBits(reverseBits(canonical[i]));
    return g;
}

void Graph::doEnumerate(int n, EnumerateCallback f, PruneCallback p, int flags) {
    if (enumerateCallback_)
	throw std::runtime_error("generating graphs is not reentrant");
    if (n == 0) {
	f(Graph(0));
	return;
    }
    enumerateCallback_ = f;
    pruneCallback_ = p;
    std::vector<const char*> argv = {"geng", "-q" };
    if (flags & CONNECTED)
	argv.push_back("-c");
    std::string sn = std::to_string(n);
    argv.push_back(sn.c_str());
    argv.push_back(nullptr);
    geng_main(argv.size() - 1, const_cast<char**>(argv.data()));
    enumerateCallback_ = nullptr;
    pruneCallback_ = nullptr;
}

void Graph::enumerate(int n, EnumerateCallback f, int flags) {
    doEnumerate(n, f, nullptr, flags);
}

void Graph::enumerate(int n, EnumerateCallback f, PruneCallback p, int flags) {
    doEnumerate(n, f, p, flags);
}

Graph::EnumerateCallback Graph::enumerateCallback_;
Graph::PruneCallback Graph::pruneCallback_;

static std::string connectedGraphName(Graph g) {
    Graph gCanon = g.canonical();
    for (auto p : namedGraphs) {
	if (p.second == gCanon)
	    return p.first;
    }

    int n = g.n();
    int degs[n];
    for (int u = 0; u < n; ++u)
	degs[u] = g.deg(u);
    std::sort(degs, degs + n);
    if (degs[0] == n - 1 && degs[n - 1] == n - 1)
	return std::string("K") + std::to_string(n);
    if (degs[n - 1] == 2) {
	if (degs[0] == 1)
	    return std::string("P") + std::to_string(n);
	else
	    return std::string("C") + std::to_string(n);
    }
    Graph gComplement = g.complement();
    if (gComplement.isConnected()) {
	for (int u = 0; u < n; ++u)
	    degs[u] = gComplement.deg(u);
	std::sort(degs, degs + n);
	if (degs[n - 1] == 2) {
	    if (degs[0] == 1)
		return std::string("cP") + std::to_string(n);
	    else
		return std::string("cC") + std::to_string(n);
	}
    }

    // complete bipartite graph?
    Set p1 = g.neighbors(0);
    if (!p1.isEmpty()) {
	Set p2 = g.neighbors(p1.min());
	bool fits = true;
	for (int u = 0; u < n; ++u) {
	    if (g.neighbors(u) != p1 && g.neighbors(u) != p2) {
		fits = false;
		break;
	    }
	}
	if (fits) {
	    int s1 = p1.size(), s2 = p2.size();
	    if (s1 > s2)
		std::swap(s1, s2);
	    return std::string("K") + std::to_string(s1) + ',' + std::to_string(s2);
	}
    }
    return g.toString();
}

std::string Graph::name() const {
    std::vector<std::string> names;
    for (auto cc : connectedComponents())
	names.push_back(connectedGraphName(subgraph(cc)));
    std::sort(names.begin(), names.end());
    std::string r;
    size_t k = 1;
    for (size_t i = 0; i < names.size(); ++i) {
	if (i + 1 < names.size() && names[i] == names[i+1]) {
	    ++k;
	    continue;
	}
	if (i >= k)
	    r += '+';
	if (k > 1)
	    r += std::to_string(k);
	r += names[i];
	k = 1;
    }
    return r;
}

std::string Graph::toString() const {
    std::string r = "{";
    bool start = true;
    for (int u = 0; u < n(); ++u) {
        for (int v = u + 1; v < n(); ++v) {
            if (hasEdge(u, v)) {
                if (!start)
                    r += ',';
                start = false;
                r += '{' + std::to_string(u) + ',' + std::to_string(v) + '}';
            }
        }
    }
    r += '}';
    return r;
}

std::ostream& operator<<(std::ostream& out, const Graph& g) {
    out << "# n = " << g.n() << std::endl;
    for (int u = 0; u < g.n(); ++u)
        for (int v = u + 1; v < g.n(); ++v)
            if (g.hasEdge(u, v))
                out << u << '\t' << v << std::endl;
    return out;
}
