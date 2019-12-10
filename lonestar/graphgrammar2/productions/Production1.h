#ifndef GALOIS_PRODUCTION1_H
#define GALOIS_PRODUCTION1_H

//class Production1 {
//public:
//    bool isPossible(GNode node, Graph &graph)  {
//        NodeData &nodeData = graph.getData(node);
//        if (!nodeData.isHyperEdge) {
//            return false;
//        }
//        auto &hyperEdge = static_cast<HyperEdge&>(nodeData);
//        auto edges = hyperEdge.getEdges(node, graph);
//        bool toRefine = hyperEdge.isToRefine();
//        bool hasHanging = hyperEdge.hasHanging(node, edges, graph);
//        bool isBorder = graph.getEdgeData(hyperEdge.getLongestEdge(node, edges, graph)).isBorder();
//        return toRefine &&
//               !hasHanging &&
//               isBorder;
//    }
//
//    void execute(GNode node, Graph &graph)  {
//        auto nodeData = graph.getData(node);
//        std::cout << "Execution of: " << static_cast<SingleNode&>(graph.getData(static_cast<HyperEdge&>(nodeData).getVertices(node, graph)[0])).getCoords().toString() << std::endl;
//    }
//};


#endif //GALOIS_PRODUCTION1_H
