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

}  // namespace EulerTransform
