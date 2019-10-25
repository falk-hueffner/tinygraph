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

#ifndef TINYGRAPH_GRAPH_HH_INCLUDED
#define TINYGRAPH_GRAPH_HH_INCLUDED

#include <functional>
#include <stdexcept>
#include <vector>

#include <gmpxx.h>

#include "Set.hh"

using bignum = mpz_class;

struct Edge {
    Edge() { }
    Edge(int nu, int nv) {
	if (nu < nv) {
	    u = nu;
	    v = nv;
	} else {
	    u = nv;
	    v = nu;
	}
    }
    bool operator==(const Edge& other) const { return u == other.u && v == other.v; }
    bool operator<(const Edge& other) const {
	if (u != other.u)
	    return u < other.u;
	else
	    return v < other.v;
    }
    int u, v;
};

class Graph {
public:
    explicit Graph(int n, std::initializer_list<std::pair<int, int>> es = {}) : neighbors_(n) {
	assert(n >= 0 && n <= maxn());
	for (auto e : es)
	    addEdge(e.first, e.second);
    }

    static Graph ofNauty(word* g, int n);
    static Graph byName(std::string name);
    static Graph ofGraph6(std::string g6);

    static Graph cycle(int n);

    int n() const { return neighbors_.size(); }
    static constexpr int maxn() { return Set::MAX_ELEMENT + 1; }
    Set vertices() const { return Set::ofRange(n()); }
    int m() const {
	int m = 0;
	for (int u = 0; u < n(); ++u)
	    m += deg(u);
	assert((m % 2) == 0);
	return m / 2;
    }
    int deg(int u) const { return neighbors(u).size(); }
    Set neighbors(int u) const { return neighbors_[u]; }
    const Set* neighbors() const { return neighbors_.data(); }
    Set nonneighbors(int u) const { return vertices() - neighbors(u) - u; }
    bool hasEdge(int u, int v) const { return neighbors(u).contains(v); }
    bool isConnected() const;

    bool operator==(const Graph& g) const { return neighbors_ == g.neighbors_; }
    bool operator!=(const Graph& g) const { return neighbors_ != g.neighbors_; }

    void addEdge(int u, int v) {
	assert(u >= 0 && u < n());
	assert(v >= 0 && v < n());
	assert(u != v);
	neighbors_[u].add(v);
	neighbors_[v].add(u);
    }
    void removeEdge(int u, int v) {
	assert(u >= 0 && u < n());
	assert(v >= 0 && v < n());
	neighbors_[u].remove(v);
	neighbors_[v].remove(u);
    }
    void toggleEdge(int u, int v) {
	assert(u >= 0 && u < n());
	assert(v >= 0 && v < n());
	neighbors_[u].toggle(v);
	neighbors_[v].toggle(u);
    }
    void deleteVertex(int u) {
	assert(u >= 0 && u < n());
	int i = 0;
	for (int v = 0; v < n(); ++v) {
	    if (v == u)
		continue;
	    neighbors_[i++] = Set::ofBits(delbit(neighbors(v).bits(), u));
	}
	neighbors_.pop_back();
    }

    class Edges {
    public:
        Edges(const Graph& g) : g_(g) { }
        class Iterator {
        public:
            Iterator(const Graph& g, int u = 0) : g_(g), u_(u) {
		while (u_ < g_.n()) {
		    n_u_ = g_.neighbors(u_).above(u_);
		    if (n_u_.nonempty())
			break;
		    ++u_;
                }
            }
            bool operator!=(const Iterator& other) const {
                return u_ != other.u_ || n_u_ != other.n_u_;
            }
	    Edge operator*() const { return {u_, n_u_.min()}; }
            const Iterator& operator++() {
		n_u_.pop();
		while (n_u_.isEmpty()) {
		    if (++u_ >= g_.n())
			return *this;
		    n_u_ = g_.neighbors(u_).above(u_);
                }
                return *this;
            }
        private:
	    const Graph& g_;
	    int u_;
	    Set n_u_;
        };

        Iterator begin() const { return Iterator(g_); }
        Iterator end() const { return Iterator(g_, g_.n()); }

    private:
        const Graph& g_;
    };
    Edges edges() const { return Edges(*this); }

    class ConnectedComponents {
    public:
	ConnectedComponents(const Graph& g) : g_(g) { }
	class Iterator {
	public:
	    Iterator(const Set* neighbors, Set unseen)
		: neighbors_(neighbors), unseen_(unseen) {
		++*this;
	    }
	    bool operator!=(const Iterator& other) const { return cc_ != other.cc_; }
	    Set operator*() const { return cc_; }
	    const Iterator& operator++() {
		if (unseen_.isEmpty())
		    cc_  = {};
		else
		    bfs();
		return *this;
	    }
	    void bfs() {
		Set oldUnseen = unseen_;
		int u = unseen_.pop();
		Set todo = {u};
		while (!todo.isEmpty()) {
		    int v = todo.pop();
		    Set n_v = neighbors_[v];
		    todo |= n_v & unseen_;
		    unseen_ -= n_v;
		}
		cc_ = oldUnseen ^ unseen_;
	    }
	private:
	    const Set* neighbors_;
	    Set unseen_;
	    Set cc_;
	};
	Iterator begin() const { return Iterator(g_.neighbors(), g_.vertices()); }
	Iterator end()   const { return Iterator(g_.neighbors(), {}); }
    private:
	const Graph& g_;
    };
    ConnectedComponents connectedComponents() const { return ConnectedComponents(*this); }

    int mSubgraph(Set vs) const {
	int m = 0;
	for (int u : vs)
	    m += (neighbors(u) & vs).size();
	assert((m % 2) == 0);
	return m / 2;
    }

    Graph complement() const {
	Graph g(n());
	for (int u = 0; u < n(); ++u)
	    g.neighbors_[u] = vertices() - neighbors(u) - u;
	return g;
    }

    Graph subgraph(Set vs) const {
	Graph g(vs.size());
	int i = 0;
	for (int u : vs)
	    g.neighbors_[i++] = Set::ofBits(extractBits(neighbors(u).bits(), vs.bits()));
	return g;
    }

    Graph canonical() const;

    typedef std::function<void(const Graph&)> EnumerateCallback;
    typedef std::function<bool(const Graph&)> PruneCallback;
    enum {
	CONNECTED     = 1 << 0,
	BICONNECTED   = 1 << 1,
	TRIANGLE_FREE = 1 << 2,
	SQUARE_FREE   = 1 << 3,
	BIPARTITE     = 1 << 4,
	TREE          = 1 << 5,
    };
    static void enumerate(int n, EnumerateCallback f, int flags = 0);
    static void enumerate(int n, EnumerateCallback f, PruneCallback p, int flags = 0);
    static EnumerateCallback enumerateCallback() { return enumerateCallback_; }
    static PruneCallback pruneCallback() { return pruneCallback_; }

    void maximalCliques(std::function<void(Set)> f) const;
    bignum numLabeledGraphs() const;

    std::string toString() const;
    std::string graph6() const;
    std::string name() const;

private:
    static void doEnumerate(int n, EnumerateCallback f, PruneCallback p, int flags);
    std::vector<Set> neighbors_;
    static EnumerateCallback enumerateCallback_;
    static PruneCallback pruneCallback_;
};

std::ostream& operator<<(std::ostream& out, const Graph& g);

#endif  // TINYGRAPH_GRAPH_HH_INCLUDED
