/* tinygraph -- exploring graph conjectures on small graphs
   Copyright (C) 2019  Falk Hüffner

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

#include <ctime>
#include <map>
#include <set>
#include <iostream>

#include "Classes.hh"
#include "Invariants.hh"
#include "EulerTransform.hh"

using PropertyTest = std::function<bool(const Graph&)>;

const auto invariant = [](const Graph& g) { return Invariants::coloringNumber(g); };
const bool connectedOnly = true;

int n0 = 1;
int k0 = 2;

template<typename T>
std::vector<std::vector<T>> cumulativeCounts(const std::vector<std::vector<T>>& counts) {
    std::vector<std::vector<T>> cumulativeCounts(counts.size());
    for (std::size_t n = 0; n < counts.size(); ++n) {
	T sum = 0;
	for (const auto& count : counts[n]) {
	    sum += count;
	    cumulativeCounts[n].push_back(sum);
	}
    }
    return cumulativeCounts;
}

template<typename T>
void output(const std::vector<std::vector<T>>& counts) {
    int maxk = 0;
    for (const auto& ks : counts)
	maxk = std::max(maxk, int(ks.size() + 1));
    if (counts.empty())
	return;
    for (int k = k0; k <= maxk; ++k) {
	for (int n = n0; n < int(counts.size()); ++n) {
	    if (n > n0)
		std::cout << ", ";
	    const auto x = k < int(counts[n].size()) ? counts[n][k] : 0;
	    std::cout << x;
	}
	std::cout << std::endl;
    }

    bool first =  true;
    for (int n = n0; n < int(counts.size()); ++n) {
	for (int k = k0 + (n - n0) - 1; k >= k0; --k) {
	    if (!first)
		std::cout << ", ";
	    first = false;
	    const auto x = k < int(counts[n].size()) ? counts[n][k] : 0;
	    std::cout << x;
	}
    }
    std::cout << std::endl;
}

int main() {
    std::vector<std::vector<uint64_t>> counts;
    std::vector<double> times;
    for (int n = n0; ; ++n) {
	auto tStart = double(std::clock()) / CLOCKS_PER_SEC;
	std::cerr << "\n\n--- n = " << n;
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
	counts.resize(n + 1);
	auto counter = [&counts, n](const Graph& g) {
			   const auto k = invariant(g);
			   if (k >= int(counts[n].size()))
			       counts[n].resize(k + 1);
			   ++counts[n][k];
		       };
	Graph::enumerate(n, counter, connectedOnly ? Graph::CONNECTED : 0);
	auto tEnd = double(std::clock()) / CLOCKS_PER_SEC;
	auto t = tEnd - tStart;
	times.push_back(t);
	std::cerr << "time: " << t << 's' << std::endl;
	std::cout << (connectedOnly ? "connected " : "") << "graphs, counts starting at n = " << n0 << ", k = " << k0 << std::endl;
	output(counts);
	std::cout << "\n" << (connectedOnly ? "connected " : "")
		  << "graphs, cumulative counts starting at n = " << n0 << ", k = " << k0 << std::endl;
	output(cumulativeCounts(counts));
    }

    return 0;
}
