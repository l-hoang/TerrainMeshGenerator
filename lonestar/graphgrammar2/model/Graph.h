#ifndef GALOIS_GRAPH_H
#define GALOIS_GRAPH_H

typedef galois::graphs::MorphGraph<NodeData, EdgeData, false> Graph;
typedef Graph::GraphNode GNode;
typedef Graph::edge_iterator EdgeIterator;
using galois::optional;

//class GraphUtils {
//public:
//    EdgeIterator getNeighbours(GNode node, Graph &graph) {
//        return graph.in_edge_begin(node);
//        return nullptr_t ;//T
//    }

//    std::vector<GNode> getNeighboursOfNode(GNode node, Graph &graph) {
//        return std::vector<GNode>{node}; //T
//    }
//};

#endif //GALOIS_GRAPH_H
