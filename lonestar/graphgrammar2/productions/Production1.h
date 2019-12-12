#ifndef GALOIS_PRODUCTION1_H
#define GALOIS_PRODUCTION1_H

#include "../utils/ConnectivityManager.h"

class Production1 {
private:
    ConnectivityManager connManager;
public:
    Production1(const ConnectivityManager &connManager) : connManager(connManager) {}

    bool isPossible(GNode node, Graph &graph)  {
        NodeData &nodeData = graph.getData(node);
        if (!nodeData.isHyperEdge) {
            return false;
        }
        bool hanging = false;
        for (auto vertex : connManager.getNeighbours(node)) {
            if (graph.getData(vertex).isHanging()) {
                hanging = true;
            }
        }
        return nodeData.isToRefine() &&
                !connManager.hasHanging(node, connManager.getEdges(node)) &&
                !hanging;
    }

    void execute(GNode node, Graph &graph, galois::UserContext<GNode>& ctx)  {
        auto nodeData = graph.getData(node);
        const EdgeIterator &longestEdge = connManager.getLongestEdge(node, connManager.getEdges(node));
//        const GNode &node1 = longestEdge->first();
//        auto newNode = graph.createNode(NodeData{false, Coordinates{0.5,0.5,0}});
        GNode node2 = graph.getEdgeDst(longestEdge);
        EdgeData &eData = graph.getEdgeData(longestEdge);
        auto pair = connManager.findSrc(eData);
        graph.removeEdge(pair.first, pair.second);
        const NodeData &nData = NodeData{false, eData.getMiddlePoint(), true};
        auto newNode = graph.createNode(nData);
        graph.addNode(newNode);
        ctx.push(newNode);
        for(auto vertex : connManager.getNeighbours(node)) {
//            auto data = graph.getData(pair.first);
            auto vertexData = graph.getData(vertex);
            auto edge = graph.addEdge(newNode, vertex);
            //TODO setBorder
            graph.getEdgeData(edge).setMiddlePoint(Coordinates{(nData.getCoords().getX()+vertexData.getCoords().getX())/2.,(nData.getCoords().getY()+vertexData.getCoords().getY())/2.,(nData.getCoords().getZ()+vertexData.getCoords().getZ())/2.});
            graph.getEdgeData(edge).setLength(sqrt(pow(nData.getCoords().getX(), 2) + pow(nData.getCoords().getY(), 2) + pow(
                    nData.getCoords().getZ(), 2)));
        }
        

//        GNode node12 = connManager.getEdgeSrc(longestEdge, node);
//        graph.removeEdge(node1, longestEdge);
//        graph.removeEdge(node2, longestEdge);
//        for (GNode n : connManager.getNeighbours(node)) {
//            graph.addEdge(n, newNode);
//            graph.getEdgeData(graph.findEdge(node1, node2));
//        }


        std::cout << "Execution of: " << (graph.getData(connManager.getNeighbours(node)[0])).getCoords().toString() << std::endl;
    }
};


#endif //GALOIS_PRODUCTION1_H
