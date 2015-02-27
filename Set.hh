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

#include <initializer_list>

#include "bits.hh"

class Set {
public:
    Set(std::initializer_list<int> xs) : bits_(0) {
	for (int x : xs)
	    add(x);
    }
    Set() : Set({}) { }

    int size() const { return popcount(bits_); }

    void add(int x) { bits_ |= (word(1) << x); }

private:
    word bits_;
};

#endif  // TINYGRAPH_SET_HH_INCLUDED
