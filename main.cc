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

bool f(Set s1, Set s2) {
    return s1.isSubset(s2);
}

int main() {
    //Graph g(5, {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}});
    Set s{0, 1, 2, 5, 9, 13, 31, 5};

    std::cout << s << std::endl;

    return 0;
}
