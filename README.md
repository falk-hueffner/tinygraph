# tinygraph

This C++11 project provides efficient data structures for
small graphs (*n* &le; 64). A graph with *n* vertices
is represented as a adjacency matrix of *n* machine words, which can
be efficiently manipulated by bit twiddling. A few examples are
provided.

## Extremal graphs

* In `extremal.cc`, the maximum number of *P*<sub>3</sub> that a graph
  on *n* vertices can have as induced subgraph is determined. Here, a
  *P*<sub>3</sub> is a path with 3 vertices. It turns out that the
  numbers are

  0, 0, 1, 4, 9, 18, 30, 48, 70, 100, 135

  matching
  [OEIS sequence A111384: ⌊*n*/2⌋ * ⌈*n*/2⌉ * (*n*-2)/2](https://oeis.org/A111384).

## Checking conjectures

* The file `p5editing.cc` examines a conjecture about a data reduction
  rule for the *P*<sub>5</sub>-Free Editing problem, that is, the
  problem of adding and deleting a minimum number of edges in a graph
  to make it *P*<sub>5</sub>-free, where *P*<sub>5</sub> is the
  induced subgraph of a path with 5 vertices. The conjecture is that
  vertices that are not in any *P*<sub>5</sub> can be omitted, since
  it is not meaningful to delete them. The program finds a
  counterexample with 8 vertices.

* In `ssge-approx.cc`, the approximation factor of an approximation
  algorithm for Sparse Split Graph Editing is examined. The worst case
  found is factor 2.5 for a graph with 6 vertices, and graphs with
  more vertices seem to have better factors. The best proven bound for
  this algorithm is a factor of 3 [(F. Hüffner, C. Komusiewicz, and
  A. Nichterlein: Editing graphs into few cliques: complexity,
  approximation, and kernelization schemes. WADS
  2015.)](http://www.user.tu-berlin.de/hueffner/clique-edit-wads15.pdf)

The default maximum number of vertices is 32. To change this, edit
`wordsize.h`.

Enumeration of graphs up to isomorphism is currently handled by
[nauty](http://cs.anu.edu.au/~bdm/nauty/), which comes with its own
license (roughly, free for non-commercial use).

