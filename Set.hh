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

#ifndef TINYGRAPH_SET_HH_INCLUDED
#define TINYGRAPH_SET_HH_INCLUDED

#include <cassert>
#include <initializer_list>

#include "bits.hh"

class Set {
public:
    Set(std::initializer_list<int> xs) : bits_(0) {
	for (int x : xs)
	    add(x);
    }
    Set() : Set({}) { }

    static constexpr int MAX_ELEMENT = MAXN - 1;

    bool isEmpty() const { return bits_ == 0; }
    int size() const { return popcount(bits_); }
    bool contains(int x) const {
	assert(x >= 0 && x <= MAX_ELEMENT);
	return bits_ & (word(1) << x);
    }

    void add(int x)     { assert(x >= 0 && x <= MAX_ELEMENT); bits_ |=  (word(1) << x); }
    void discard(int x) { assert(x >= 0 && x <= MAX_ELEMENT); bits_ &= ~(word(1) << x); }
    void remove(int x)  { assert(contains(x)); discard(x); }

    bool operator==(Set other) const { return bits_ == other.bits_; }
    bool operator!=(Set other) const { return bits_ != other.bits_; }

    Set& operator&=(Set other) { bits_ &=  other.bits_; return *this; }
    Set& operator|=(Set other) { bits_ |=  other.bits_; return *this; }
    Set& operator^=(Set other) { bits_ ^=  other.bits_; return *this; }
    Set& operator-=(Set other) { bits_ &= ~other.bits_; return *this; }

    Set operator&(Set other) const { Set s = *this; return s &= other; }
    Set operator|(Set other) const { Set s = *this; return s |= other; }
    Set operator^(Set other) const { Set s = *this; return s ^= other; }
    Set operator-(Set other) const { Set s = *this; return s -= other; }

    Set& operator+=(int x) { add(x);     return *this; }
    Set& operator-=(int x) { discard(x); return *this; }

    Set operator+(int x) const { Set s = *this; return s += x; }
    Set operator-(int x) const { Set s = *this; return s -= x; }

    bool isSubset(Set other) const { return (*this - other).isEmpty(); }
    bool isSuperset(Set other) const { return other.isSubset(*this); }

private:
    explicit Set(word bits) : bits_(bits) { }
    word bits_;
};

#endif  // TINYGRAPH_SET_HH_INCLUDED
