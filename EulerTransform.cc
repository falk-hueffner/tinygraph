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

#include "EulerTransform.hh"

namespace EulerTransform {

mpz_class combinations(const mpz_class& n, const mpz_class& k) {
    if (k > n)
	throw std::domain_error("combinations");
    if (k > n / 2)
	return combinations(n, n - k);
    if (!k.fits_ulong_p())
	throw std::domain_error("combinations");
    mpz_class r;
    mpz_bin_ui(r.get_mpz_t(), n.get_mpz_t(), k.get_ui());
    return r;
}

bignum n_multicombinations(bignum n, bignum k) {
    if (k == 0)
	return 0;
    return combinations(k + n - 1, n);
}

// based on http://code.activestate.com/recipes/218332-generator-for-integer-partitions/#c5
// by Tim Peters
void integerPartitions(int n, std::function<void(Set, const std::vector<int>&)> f) {
    if (n == 0) {
        f({}, {});
	return;
    }

    std::vector<int> ms(n + 1);
    ms[n] = 1;
    Set keys = {n};
    f(keys, ms);

    while(keys != Set({1})) {
	int reuse;
        if (keys.min() == 1) {
            keys.discard(1);
	    reuse = ms[1];
	    ms[1] = 0;
	} else {
            reuse = 0;
	}
        int i = keys.min();
        int newcount = --ms[i];
        reuse += i;
        if (newcount == 0) {
	    keys.discard(keys.min());
	    ms[i] = 0;
	}
        keys.add(--i);
	div_t qr = div(reuse, i);
        ms[i] = qr.quot;
	int r = qr.rem;
        if (r) {
            ms[r] = 1;
            keys.add(r);
	}
	f(keys, ms);
    }
}

std::vector<bignum> transform(const std::vector<bignum>& seq) {
    std::vector<bignum> transformed;
    for (int n = 0; n < int(seq.size()); ++n) {
        bignum count = 0;
	integerPartitions(n,
			  [&count,&seq](Set keys, const std::vector<int>& mult) {
			      bignum c = 1;
			      for (auto size : keys)
				  c *= n_multicombinations(mult[size], seq[size]);
			      count += c;
			  });
        transformed.push_back(count);
    }
    return transformed;
}

std::vector<bignum> transform(const std::vector<uint64_t>& s) {
    std::vector<bignum> sBig(s.begin(), s.end());
    return transform(sBig);
}

// https://oeis.org/A001349
const std::vector<bignum> numConnectedGraphs = {
    1_mpz,
    1_mpz,
    1_mpz,
    2_mpz,
    6_mpz,
    21_mpz,
    112_mpz,
    853_mpz,
    11117_mpz,
    261080_mpz,
    11716571_mpz,
    1006700565_mpz,
    164059830476_mpz,
    50335907869219_mpz,
    29003487462848061_mpz,
    31397381142761241960_mpz,
    63969560113225176176277_mpz,
    245871831682084026519528568_mpz,
    1787331725248899088890200576580_mpz,
    24636021429399867655322650759681644_mpz,
    645465483198722799426731128794502283004_mpz,
    32219627385046589818232044119082157323436797_mpz,
    3070814262175729723895568454399186829509550171755_mpz,
    559943868821088067965169467298102645124904586852569700_mpz,
    195704346352067869424144939394112759539046378875336729014803_mpz,
    131331197864429267343038461571121231901357546349778703464744110480_mpz,
    169487487069103860309498044496181634419481050608589608741506974117661153_mpz,
    421259837031894153474761394722982198262661349959958867827522253208309529657428_mpz,
    2019295578418395388334710354184231114050311766824615116653385182748332016100781822978_mpz,
    18691350703182535102936037539278223408795964939210324175289556058530041603266454027210155407_mpz,
    334494297617902927474062588988771420592400340448497175735486787573919763092664433461017585013705594_mpz,
};

std::vector<bignum> nonGraphs(const std::vector<bignum>& seq) {
    // https://oeis.org/A000088
    static std::vector<bignum> numGraphs = transform(numConnectedGraphs);
    if (seq.size() > numGraphs.size())
	throw std::domain_error("need more data in EulerTransform::nonGraphs");
    std::vector<bignum> result(seq.size());
    for (size_t i = 0; i < seq.size(); ++i)
	result[i] = numGraphs[i] - seq[i];
    return result;
}

std::vector<bignum> nonGraphs(const std::vector<uint64_t>& s) {
    std::vector<bignum> sBig(s.begin(), s.end());
    return nonGraphs(sBig);
}

std::vector<bignum> connectedNonGraphs(const std::vector<bignum>& seq) {
    if (seq.size() > numConnectedGraphs.size())
	throw std::domain_error("need more data in EulerTransform::connectedNonGraphs");
    std::vector<bignum> result(seq.size());
    for (size_t i = 0; i < seq.size(); ++i)
	result[i] = numConnectedGraphs[i] - seq[i];
    return result;
}

std::vector<bignum> connectedNonGraphs(const std::vector<uint64_t>& s) {
    std::vector<bignum> sBig(s.begin(), s.end());
    return connectedNonGraphs(sBig);
}

}  // namespace EulerTransform
