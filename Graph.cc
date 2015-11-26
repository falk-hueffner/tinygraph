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

Graph Graph::ofNauty(word* nautyg, int n) {
    Graph g(n);
    for (int i = 0; i < n; ++i)
	g.neighbors_[i] = Set::ofBits(reverseBits(nautyg[i]));
    return g;
}

Graph Graph::byName(std::string name) {
    auto DIGITS = "0123456789";
    if (name.empty())
	throw std::invalid_argument("Graph::byName: empty name");
    if (name[0] == 'P' && name.find_first_not_of(DIGITS, 1) == std::string::npos) {
	name.erase(name.begin());
	int n = std::stoi(name);
	Graph g(n);
	for (int u = 0; u + 1 < n; ++u)
	    g.addEdge(u, u + 1);
	return g;
    }
    throw std::invalid_argument("Graph::byName: unknown graph " + name);
}

extern "C" {
    int geng_main(int argc, char* argv[]);
    void geng_outproc(FILE* f, word* nautyg, int n);
}

void geng_outproc(FILE*, word* nautyg, int n) {
    Graph g = Graph::ofNauty(nautyg, n);
    Graph::enumerateCallback()(g);
}

void Graph::enumerate(int n, EnumerateCallback f, int flags) {
    if (enumerateCallback_)
	throw std::runtime_error("generating graphs is not reentrant");
    if (n == 0) {
	f(Graph(0));
	return;
    }
    enumerateCallback_ = f;
    std::vector<const char*> argv = {"geng", "-q" };
    if (flags & CONNECTED)
	argv.push_back("-c");
    std::string sn = std::to_string(n);
    argv.push_back(sn.c_str());
    argv.push_back(nullptr);
    geng_main(argv.size() - 1, const_cast<char**>(argv.data()));
    enumerateCallback_ = nullptr;
}

Graph::EnumerateCallback Graph::enumerateCallback_;

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
