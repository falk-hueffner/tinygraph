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

#include <map>

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
    static const std::map<std::string, Graph> namedGraphs = {
	{"diamond", Graph::ofGraph6("Cz")},
	{"paw",     Graph::ofGraph6("Cx")},
	{"claw",    Graph::ofGraph6("Cs")},
	{"gem",     Graph::ofGraph6("Dh{")},
	{"bowtie",  Graph::ofGraph6("D{c")},
	{"fork",    Graph::ofGraph6("DiC")},
	{"kite",    Graph::ofGraph6("DTw")},
	{"dart",    Graph::ofGraph6("DvC")},
	{"house",   Graph::ofGraph6("DUw")},
	{"banner",  Graph::ofGraph6("DrG")},
	{"tadpole", Graph::ofGraph6("DKs")},
	{"bull",    Graph::ofGraph6("D{O")},
	{"cricket", Graph::ofGraph6("DiS")},
    };
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
