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

#ifndef TINYGRAPH_BITS_HH_INCLUDED
#define TINYGRAPH_BITS_HH_INCLUDED

#include <cstdint>

#include "wordsize.h"

#if WORDSIZE == 16
typedef uint16_t word;
#elif WORDSIZE == 32
typedef uint32_t word;
#elif WORDSIZE == 64
typedef uint64_t word;
#endif

inline int popcount(word x) {
    static_assert(sizeof (word) <= sizeof (int) ||
		  sizeof (word) == sizeof (long) ||
		  sizeof (word) == sizeof (long long),
		  "cannot determine popcount intrinsic");
    if (sizeof (word) <= sizeof (int))
	return __builtin_popcount(x);
    else if (sizeof (word) == sizeof (long))
	return __builtin_popcountl(x);
    else
	return __builtin_popcountll(x);
}

inline int ctz(word x) {
    static_assert(sizeof (word) <= sizeof (int) ||
		  sizeof (word) == sizeof (long) ||
		  sizeof (word) == sizeof (long long),
		  "cannot determine ctz intrinsic");
    if (sizeof (word) <= sizeof (int))
	return __builtin_ctz(x);
    else if (sizeof (word) == sizeof (long))
	return __builtin_ctzl(x);
    else
	return __builtin_ctzll(x);
}

inline word reverseBits(word x) {
    if (WORDSIZE == 64) {
	x = ((x >> 1) & 0x5555555555555555) | ((x & 0x5555555555555555) << 1);
	x = ((x >> 2) & 0x3333333333333333) | ((x & 0x3333333333333333) << 2);
	x = ((x >> 4) & 0x0f0f0f0f0f0f0f0f) | ((x & 0x0f0f0f0f0f0f0f0f) << 4);
	return __builtin_bswap64(x);
    } else if (WORDSIZE == 32) {
	x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);
	x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);
	x = ((x >> 4) & 0x0f0f0f0f) | ((x & 0x0f0f0f0f) << 4);
	return __builtin_bswap32(x);
    } else { // WORDSIZE == 16
	x = ((x >> 1) & 0x5555) | ((x & 0x5555) << 1);
	x = ((x >> 2) & 0x3333) | ((x & 0x3333) << 2);
	x = ((x >> 4) & 0x0f0f) | ((x & 0x0f0f) << 4);
	x = ((x >> 8) & 0x00ff) | ((x & 0x00ff) << 8);
	return x;
    }
}

inline word delbit(word x, int n) {
    word mask = (word(1) << n) - 1;
    word lo = x & mask;
    word hi = (x >> 1) & ~mask;
    return hi | lo;
}

inline word lowestBits(word x, int k) {
    word y = x;
    for (int i = 0; i < k; ++i)
	y &= y - 1;
    return x ^ y;
}

inline word highestBits(word x, int k) {
    while (popcount(x) > k)
	x &= x - 1;
    return x;
}

#endif // TINYGRAPH_BITS_HH_INCLUDED
