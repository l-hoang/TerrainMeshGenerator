#ifndef GALOIS_PRODUCTION1_H
#define GALOIS_PRODUCTION1_H

static const double EPS = 1e-4;

#include "../utils/ConnectivityManager.h"
#include "../utils/utils.h"

class Production1 {
private:
    ConnectivityManager connManager;


    int getEdgeToBreak(const std::vector<double> lengths, const std::vector<EdgeData> &data) const {
        int result = -1;
        for (int i = 0; i < 3; ++i) {
            if (!less(lengths[i], lengths[(i+1)%3])  && !less(lengths[i], lengths[(i+2)%3])) {
                result = i;
                if (data[i].isBorder()) {
                    return i;
                }
            }
        }
        return result;
    }

    int getNeutralVertex(int edgeToBreak) {
        return (edgeToBreak + 2) % 3;
    }

public:

    explicit Production1(const ConnectivityManager &connManager) : connManager(connManager) {}

    bool execute(GNode interior, Graph &graph, galois::UserContext<GNode>& ctx)  {
        NodeData &nodeData = graph.getData(interior);
        const std::vector<GNode> &vertices = connManager.getNeighbours(interior);
        if (!nodeData.isHyperEdge || !nodeData.isToRefine() ||
                connManager.hasHanging(interior, connManager.getTriangleEdges(vertices))) {
            return false;
        }

        int counter = 0;
        for (auto vertex : vertices) {
            if (graph.getData(vertex).isHanging()) {
                ++counter;
            }
        }
        if (counter >= 2) {
            return false;
        }
        std::vector<optional<EdgeIterator>> edgesIterators = connManager.getTriangleEdges(vertices);
        std::vector<EdgeData> edgesData(3);
        std::vector<double> lengths(3);
        for (int i = 0; i < 3; ++i) {
            edgesData[i] = graph.getEdgeData(edgesIterators[i].get());
            lengths[i] = edgesData[i].getLength();
        }

        int edgeToBreak = getEdgeToBreak(lengths, edgesData);
        bool isBorder = edgesData[edgeToBreak].isBorder();
        int neutralVertex = getNeutralVertex(edgeToBreak);

        auto edgePair = connManager.findSrc(edgesData[edgeToBreak]);
        graph.removeEdge(edgePair.first, edgePair.second);
        NodeData newNodeData = NodeData{false, edgesData[edgeToBreak].getMiddlePoint(), true};
        auto newNode = graph.createNode(newNodeData);
        graph.addNode(newNode);
        ctx.push(newNode);
        for(int i = 0; i < 3; ++i) {
            auto vertexData = graph.getData(vertices[i]);
            auto edge = graph.addEdge(newNode, vertices[i]);
            graph.getEdgeData(edge).setBorder(i != neutralVertex ? isBorder : false);
            graph.getEdgeData(edge).setMiddlePoint(Coordinates{(newNodeData.getCoords().getX() + vertexData.getCoords().getX()) / 2., (newNodeData.getCoords().getY() + vertexData.getCoords().getY()) / 2., (newNodeData.getCoords().getZ() + vertexData.getCoords().getZ()) / 2.});
            graph.getEdgeData(edge).setLength(sqrt(pow(newNodeData.getCoords().getX(), 2) + pow(newNodeData.getCoords().getY(), 2) + pow(
                    newNodeData.getCoords().getZ(), 2)));
        }
        const NodeData &firstInteriorData = NodeData{true, false}; //Refactor me
        const NodeData &secondInteriorData = NodeData{true, false};
        auto firstInterior = graph.createNode(firstInteriorData);
        auto secondInterior = graph.createNode(secondInteriorData);
        graph.addNode(firstInterior);
        graph.addNode(secondInterior);
        ctx.push(firstInterior);
        ctx.push(secondInterior);
        graph.addEdge(firstInterior, newNode);
        graph.addEdge(secondInterior, newNode);
        graph.addEdge(firstInterior, vertices[neutralVertex]);
        graph.addEdge(secondInterior, vertices[neutralVertex]);
        bool switc = false;
        for (int j = 0; j < 3; ++j) {
            if (j != neutralVertex) {
                graph.addEdge(!switc ? firstInterior : secondInterior, vertices[j]);
                switc = true;
            }
        }
        std::cout << "Execution of: " << (graph.getData(connManager.getNeighbours(interior)[0])).getCoords().toString() << std::endl;
        return true;
    }

//    void execute(GNode node, Graph &graph, galois::UserContext<GNode>& ctx)  {
//        auto nodeData = graph.getData(node);
//        const EdgeIterator &longestEdge = connManager.getLongestEdge(node, connManager.getTriangleEdges(node));
//        EdgeData &eData = graph.getEdgeData(longestEdge);
//        auto pair = connManager.findSrc(eData);
//        graph.removeEdge(pair.first, pair.second);
//        const NodeData &nData = NodeData{false, eData.getMiddlePoint(), true};
//        auto newNode = graph.createNode(nData);
//        graph.addNode(newNode);
//        ctx.push(newNode);
//        for(auto vertex : connManager.getNeighbours(node)) {
////            auto data = graph.getData(pair.first);
//            auto vertexData = graph.getData(vertex);
//            auto edge = graph.addEdge(newNode, vertex);
//            //TODO setBorder
//            graph.getEdgeData(edge).setMiddlePoint(Coordinates{(nData.getCoords().getX()+vertexData.getCoords().getX())/2.,(nData.getCoords().getY()+vertexData.getCoords().getY())/2.,(nData.getCoords().getZ()+vertexData.getCoords().getZ())/2.});
//            graph.getEdgeData(edge).setLength(sqrt(pow(nData.getCoords().getX(), 2) + pow(nData.getCoords().getY(), 2) + pow(
//                    nData.getCoords().getZ(), 2)));
//        }
//        std::cout << "Execution of: " << (graph.getData(connManager.getNeighbours(node)[0])).getCoords().toString() << std::endl;
//    }
};


#endif //GALOIS_PRODUCTION1_H
