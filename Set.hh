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
#include <iostream>

#include "bits.hh"

class Set {
public:
    Set(std::initializer_list<int> xs) : bits_(0) {
	for (int x : xs)
	    add(x);
    }
    Set() : bits_(0) { }
    static Set ofBits(word s) { return Set(s); }
    static Set ofRange(int x) {
	assert(x >= 0 && x <= WORDSIZE);
	return Set(x == 0 ? 0 : word(-1) >> (WORDSIZE - x));
    }

    static constexpr int MAX_ELEMENT = WORDSIZE - 1;

    bool isEmpty() const { return bits_ == 0; }
    int size() const { return popcount(bits_); }
    bool contains(int x) const {
	assert(x >= 0 && x <= MAX_ELEMENT);
	return bits_ & (word(1) << x);
    }

    void add(int x)     { assert(x >= 0 && x <= MAX_ELEMENT); bits_ |=  (word(1) << x); }
    void discard(int x) { assert(x >= 0 && x <= MAX_ELEMENT); bits_ &= ~(word(1) << x); }
    void toggle(int x)  { assert(x >= 0 && x <= MAX_ELEMENT); bits_ ^=  (word(1) << x); }
    void remove(int x)  { assert(contains(x)); discard(x); }
    int pop() { assert(bits_); int x = ctz(bits_); bits_ &= bits_ - 1; return x; }

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

    class Iterator {
	friend Set;
    public:
	bool operator!=(Iterator it) const { return bits_ != it.bits_; }
	int operator*() const { return ctz(bits_); }
	Iterator& operator++() { bits_ &= bits_ - 1; return *this; }
    private:
	Iterator(word bits) : bits_(bits) { }
	word bits_;
    };
    Iterator begin() const { return Iterator(bits_); }
    Iterator end() const { return Iterator(0); }

    class Subsets {
    public:
	Subsets(word set) : set_(set) { }
	class Iterator {
	    friend Set;
	public:
	    // hack to get correct for() semantics without unneccessary comparisons
	    bool operator!=(Iterator) const { return !done_; }
	    Set operator*() const { return Set(subset_); }
	    Iterator& operator++() { subset_ = (subset_ - set_) & set_; done_ = subset_ == 0; return *this; }
	private:
	    Iterator(word set, bool done) : set_(set), subset_(0), done_(done) { }
	    word set_;
	    word subset_;
	    bool done_;
	};
	Iterator begin() const { return Iterator(set_, false); }
	Iterator end() const { return Iterator(set_, true); }
    private:
	word set_;
    };
    Subsets subsets() { return Subsets(bits_); }

    class Combinations {
    public:
	Combinations(word set, int k) : set_(set), k_(k) { }
	class Iterator {
	    friend Set;
	public:
	    bool operator!=(Iterator it) const { return comb_ != it.comb_; }
	    Set operator*() const { return Set(comb_); }
	    Iterator& operator++() {
		if (comb_ == 0) {
		    // special case for k = 0; need to yield the empty
		    // set, so we need to make sure end() != begin()
		    comb_ = 1;
		    return *this;
		}
		word h = (-comb_ & (comb_ ^ set_)) - 1;
		word l = set_;
		for (int i = 0; i < popcount(h & comb_) - 1; ++i)
		    l &= l - 1;
		comb_ = (set_ & h) ^ l;
		return *this;
	    }
	private:
	    Iterator(word set, word comb) : set_(set), comb_(comb) { }
	    word set_;
	    word comb_;
	};
	Iterator begin() const {
	    if (k_ > popcount(set_))
		return Iterator(0, 0);
	    else
		return Iterator(set_, lowestBits(set_, k_));
	}
	Iterator end() const {
	    if (k_ > popcount(set_)) {
		 return Iterator(0, 0);
	    } else {
		Iterator it(set_, highestBits(set_, k_));
		return ++it;
	    }
	}
    private:
	word set_;
	int k_;
    };
    Combinations combinations(int k) { return Combinations(bits_, k); }

private:
    explicit Set(word bits) : bits_(bits) { }
    word bits_;
};

std::ostream& operator<<(std::ostream& out, Set s);

#endif  // TINYGRAPH_SET_HH_INCLUDED
