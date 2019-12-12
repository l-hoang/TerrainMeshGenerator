#ifndef GALOIS_GRAPH_H
#define GALOIS_GRAPH_H

typedef galois::graphs::MorphGraph<NodeData, EdgeData, false> Graph;
typedef Graph::GraphNode GNode;
typedef Graph::edge_iterator EdgeIterator;
using galois::optional;

#endif //GALOIS_GRAPH_H
