# tinygraph

This project provides efficient data structures for checking conjectures on small graphs (*n* &le; 64). A graph with *n* vertices is represented as a adjacency matrix of *n* machine words, which can be efficiently manipulated by bit twiddling.

The default maximum number of vertices is 32. To change this, edit `bits.hh`.

Enumeration of graphs is handled by [nauty](http://cs.anu.edu.au/~bdm/nauty/).
