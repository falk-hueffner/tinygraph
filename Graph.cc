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
#include <numeric>

// A&BvC: an A graph with an extra B vertices, each of which is attached to C vertices of the A
static const std::map<std::string, Graph> namedGraphs = {
    {"empty",   Graph(0).canonical()},
    {"triangle",Graph::ofGraph6("Bw").canonical()},
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
    {"R",       Graph::ofGraph6("ElCO").canonical()},
    {"domino",  Graph::ofGraph6("ErGW").canonical()},
    {"P6-comp", Graph::ofGraph6("EUzo").canonical()},
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

    auto stoi = [](const std::string& s) -> int {
	int r = 0;
	for (std::size_t i = 0; i < s.size(); ++i) {
	    if (!('0' <= s[i] && i <= '9'))
		throw std::invalid_argument("Graph::byName: invalid number");
	    r *= 10;
	    r += s[i] - '0';
	    if (r > 10000)
		throw std::invalid_argument("Graph::byName: number too large");
	}
	return r;
    };

    if (name.empty())
	throw std::invalid_argument("Graph::byName: empty name");
    if (name.find('+') != std::string::npos) {
	Graph g1 = byName(name.substr(0, name.find('+')));
	Graph g2 = byName(name.substr(name.find('+') + 1));
	int n = g1.n() + g2.n();
	if (n > maxn())
	    throw std::invalid_argument("Graph too large");
	Graph g(n);
	for (int u = 0; u < g1.n(); ++u)
	    g.neighbors_[u] = g1.neighbors_[u];
	for (int u = 0; u < g2.n(); ++u)
	    g.neighbors_[g1.n() + u] = Set::ofBits(g2.neighbors_[u].bits() << g1.n());
	return g;
    }
    auto DIGITS = "0123456789";
    auto ns = name.find_first_not_of(DIGITS, 0);
    if (ns != std::string::npos && ns > 0) {
	int n = stoi(name.substr(0, ns));
	Graph g1 = byName(name.substr(ns));
	int n2 = g1.n() * n;
	if (n2 > maxn())
	    throw std::invalid_argument("Graph too large");
	Graph g(n2);
	for (int i = 0; i < n; ++i)
	    for (Edge e : g1.edges())
		g.addEdge(e.u + g1.n() * i, e.v + g1.n() * i);
	return g;
    }
    if (name[0] == 'K' && name.find(',') != std::string::npos) {
	name.erase(name.begin());
	int n1 = stoi(name.substr(0, name.find(',')));
	int n2 = stoi(name.substr(name.find(',') + 1));
	int n = n1 + n2;
	if (n > maxn())
	    throw std::invalid_argument("Graph too large");
	Graph g(n1 + n2);
	for (int u = 0; u < n1; ++u)
	    for (int v = n1; v < n1 + n2; ++v)
		g.addEdge(u, v);
	return g;
    }
    if ((name[0] == 'P' || name[0] == 'C' || name[0] == 'K')
	&& name.find_first_not_of(DIGITS, 1) == std::string::npos) {
	auto type = name[0];
	name.erase(name.begin());
	int n = stoi(name);
	if (n > maxn())
	    throw std::invalid_argument("Graph too large");
	Graph g(n);
	if (type == 'K') {
	    for (int u = 0; u < n; ++u)
		for (int v = u + 1; v < n; ++v)
		    g.addEdge(u, v);
	} else {
	    if (type == 'C' && n == 1)
		throw std::invalid_argument("Graph::byName: self-loops not supported");
	    for (int u = 0; u + 1 < n; ++u)
		g.addEdge(u, u + 1);
	    if (type == 'C' && n != 0)
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

bignum factorial(int x) {
    bignum r = 1;
    for (int i = 2; i <= x; ++i)
	r *= i;
    return r;
}

static bignum grpsize;
void groupsize(int* /*lab*/, int* /*ptn*/, int /*level*/, int* /*orbits*/, statsblk* /*stats*/,
	       int /*tv*/, int index, int /*tcellsize*/, int /*numcells*/, int /*cc*/, int /*n*/) {
    grpsize *= index;
}

bignum Graph::numLabeledGraphs() const {
    if (n() == 0)
	return 1;
    word nautyg[n()];
    for (int i = 0; i < n(); ++i)
	nautyg[i] = reverseBits(neighbors(i).bits());
    int orbits[n()];
    DEFAULTOPTIONS_GRAPH(options);
    options.userlevelproc = groupsize;
    grpsize = 1;
    options.getcanon = true;
    int lab[n()];
    int ptn[n()];
    statsblk stats;
    word canonical[n()];
    densenauty(nautyg, lab, ptn, orbits, &options, &stats, 1, n(), canonical);
    if (stats.grpsize1 <= (uint64_t(1) << 53) && stats.grpsize2 == 0)
	assert(stats.grpsize1 == grpsize);
    return factorial(n()) / grpsize;
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
    std::vector<const char*> argv = {"geng", "-q"};
    if (flags & CONNECTED)     argv.push_back("-c");
    if (flags & TREE)          argv.push_back("-c");
    if (flags & BICONNECTED)   argv.push_back("-C");
    if (flags & TRIANGLE_FREE) argv.push_back("-t");
    if (flags & SQUARE_FREE)   argv.push_back("-f");
    if (flags & BIPARTITE)     argv.push_back("-b");
    std::string sn = std::to_string(n);
    std::string sm = std::to_string(n - 1) + ':' + std::to_string(n - 1);
    argv.push_back(sn.c_str());
    if (flags & TREE)
	argv.push_back(sm.c_str());
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

// Bron--Kerbosch
void extendCliques(const Graph& g, std::function<void(Set)> f,
		   Set clique, Set cands, Set nots) {
    if (cands.isEmpty()) {
	if (nots.isEmpty())
	    f(clique);
	return;
    }
    Set pivotNeighbors;
    int pivotScore = 0;
    for (int u : cands | nots) {
	int score = (g.neighbors(u) & cands).size();
	if (score > pivotScore) {
	    pivotNeighbors = g.neighbors(u);
	    pivotScore = score;
	}
    }
    for (int u : cands - pivotNeighbors) {
	extendCliques(g, f, clique + u, cands & g.neighbors(u), nots & g.neighbors(u));
	cands -= u;
	nots += u;
    }
}

void Graph::maximalCliques(std::function<void(Set)> f) const {
    extendCliques(*this, f, {}, vertices(), {});
}

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
    bool hasDeg0 = false;
    for (int u = 0; u < n(); ++u) {
	if (deg(u) == 0) {
	    hasDeg0 = true;
	    break;
	}
    }
    if (hasDeg0)
	r += '[' + std::to_string(n()) + "] ";
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

std::string Graph::graph6() const {
    std::string s;
    s += n() + 63;
    int k = 6;
    int x = 0;
    for (int j = 1; j < n(); ++j) {
        for (int i = 0; i < j; ++i) {
            x <<= 1;
            if (hasEdge(i, j))
                x |= 1;
            if (--k == 0) {
                s += x + 63;
                k = 6;
                x = 0;
            }
        }
    }

    if (k != 6)
        s += 63 + (x << k);
    return s;
}

std::ostream& operator<<(std::ostream& out, const Graph& g) {
    out << "# n = " << g.n() << std::endl;
    for (int u = 0; u < g.n(); ++u)
        for (int v = u + 1; v < g.n(); ++v)
            if (g.hasEdge(u, v))
                out << u << '\t' << v << std::endl;
    return out;
}
