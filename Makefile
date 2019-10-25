# tinygraph -- exploring graph conjectures on small graphs
# Copyright (C) 2015  Falk Hüffner
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

EXECS	  = count count-table extremal ssge-approx p5editing forbidden-subgraphs

CC	  = gcc
CXX	  = g++

CFLAGS	  = -Ofast -march=native -g
CXXFLAGS  = -std=c++11 $(CFLAGS) -W -Wall -Werror
GMP_LIBS  = -lgmp -lgmpxx

GENG_OBJ = gtools.o nauty1.o nautil1.o naugraph1.o schreier.o naurng.o planarity.o
COMMON_OBJ = Set.o Graph.o geng.o $(addprefix nauty/,$(GENG_OBJ))

all: .deps nauty $(EXECS)

count: Classes.o Subgraph.o Invariants.o EulerTransform.o $(COMMON_OBJ) count.o
	$(CXX) $(CXXFLAGS) $^ $(GMP_LIBS) -o $@

count-table: Classes.o Subgraph.o Invariants.o EulerTransform.o $(COMMON_OBJ) count-table.o
	$(CXX) $(CXXFLAGS) $^ $(GMP_LIBS) -o $@

extremal: Subgraph.o $(COMMON_OBJ) extremal.o
	$(CXX) $(CXXFLAGS) $^ $(GMP_LIBS) -o $@

ssge-approx: ssge-approx.o $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $^ $(GMP_LIBS) -o $@

p5editing: p5editing.o $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $^ $(GMP_LIBS) -o $@

forbidden-subgraphs: forbidden-subgraphs.o Classes.o Invariants.o Subgraph.o $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $^ $(GMP_LIBS) -o $@

test: testMain
	./testMain

testMain: testMain.o testBits.o testSet.o testClasses.o testSubgraph.o testEulerTransform.o \
		$(COMMON_OBJ) Invariants.o Classes.o Subgraph.o EulerTransform.o
	$(CXX) $(CXXFLAGS) $^ $(GMP_LIBS) -o $@

Graph.o: nauty

nauty: nauty26r6.tar.gz wordsize.h
	rm -rf nauty26r6 nauty
	tar -xvvzf nauty26r6.tar.gz
	ln -s nauty26r6 nauty
	(cd nauty && CFLAGS="$(CFLAGS)" ./configure --enable-wordsize=$$(perl -n -e'/define WORDSIZE (\d+)/ && print $$1' ../wordsize.h) && make $(GENG_OBJ))

nauty26r6.tar.gz:
	wget http://users.cecs.anu.edu.au/~bdm/nauty/nauty26r6.tar.gz

.deps:
	mkdir -p .deps

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -MD -o $@ $<
	@cp $*.d .deps/$*.P;					    \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> .deps/$*.P;    \
		rm -f $*.d

clean:
	rm -rf *.o $(EXECS) testMain nauty26r6 nauty core gmon.out

-include $(wildcard .deps/*.P)
