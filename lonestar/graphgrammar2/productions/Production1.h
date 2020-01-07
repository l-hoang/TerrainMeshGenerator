#ifndef GALOIS_PRODUCTION1_H
#define GALOIS_PRODUCTION1_H

#include "Production.h"
#include "../utils/ConnectivityManager.h"
#include "../utils/utils.h"

class Production1 : Production {
private:

    bool checkApplicabilityCondition(const NodeData &nodeData,
                                     const std::vector<optional<EdgeIterator>> &edgesIterators) const {
        return nodeData.isToRefine() && !connManager.hasBrokenEdge(edgesIterators);
    }

    int getEdgeToBreak(const std::vector<double> &lengths, const std::vector<optional<EdgeData>> &edgesData,
                       const std::vector<NodeData> &verticesData) const {
        std::vector<int> longestEdges = getLongestEdges(lengths);
        for (int longest : longestEdges) {
            if (edgesData[longest].get().isBorder()) {
                return longest;
            }
            if (!verticesData[getEdgeVertices(longest).first].isHanging() &&
                !verticesData[getEdgeVertices(longest).second].isHanging()) {

                return longest;
            }
        }
        return -1;
    }

    std::vector<int> getLongestEdges(const std::vector<double> &lengths) const {
        std::vector<int> longestEdges;
        for (int i = 0; i < 3; ++i) {
            if (!less(lengths[i], lengths[(i + 1) % 3]) && !less(lengths[i], lengths[(i + 2) % 3])) {
                longestEdges.push_back(i);
            }
        }
        return longestEdges;
    }

    GNode createInterior(GNode *node1, GNode *node2, GNode *node3, galois::UserContext<GNode> &ctx) const {
        NodeData firstInteriorData = NodeData{true, false};
        auto firstInterior = connManager.createNode(firstInteriorData, ctx);
        connManager.getGraph().addEdge(firstInterior, *node1);
        connManager.getGraph().addEdge(firstInterior, *node2);
        connManager.getGraph().addEdge(firstInterior, *node3);
        return firstInterior;
    }

    void breakElement(int edgeToBreak, GNode &interior, const std::vector<optional<EdgeData>> &edgesData,
                      const std::vector<GNode> &vertices,
                      const std::vector<NodeData> &verticesData, galois::UserContext<GNode> &ctx) const {
        Graph &graph = connManager.getGraph();
        bool breakingOnBorder = edgesData[edgeToBreak].get().isBorder();
        int neutralVertex = getNeutralVertex(edgeToBreak);

        auto edgePair = connManager.findSrc(edgesData[edgeToBreak].get());
        graph.removeEdge(edgePair.first, edgePair.second);
        NodeData newNodeData = NodeData{false, edgesData[edgeToBreak].get().getMiddlePoint(), !breakingOnBorder};
        GNode newNode = graph.createNode(newNodeData);
        graph.addNode(newNode);
        ctx.push(newNode);
        for (int i = 0; i < 3; ++i) {
            auto vertexData = verticesData[i];
            auto edge = graph.addEdge(newNode, vertices[i]);
            graph.getEdgeData(edge).setBorder(i != neutralVertex ? breakingOnBorder : false);
            graph.getEdgeData(edge).setMiddlePoint(
                    (newNodeData.getCoords().getX() + vertexData.getCoords().getX()) / 2.,
                    (newNodeData.getCoords().getY() + vertexData.getCoords().getY()) / 2.,
                    (newNodeData.getCoords().getZ() + vertexData.getCoords().getZ()) / 2.);
            graph.getEdgeData(edge).setLength(newNodeData.getCoords().dist(vertexData.getCoords()));
        }
//        NodeData secondInteriorData = NodeData{true, false};
//        auto secondInterior = connManager.createNode(secondInteriorData, ctx);
//        GNode firstInterior;
        NodeData firstInteriorData = NodeData{true, false};
        auto firstInterior = connManager.createNode(firstInteriorData, ctx);
//        std::vector<GNode> vertices2;
//        for (Graph::edge_iterator ii = graph.edge_begin(interior), ee = graph.edge_end(interior); ii != ee; ++ii) {
//            Graph::edge_iterator ii = graph.edge_begin(interior);
//            GNode v1 = graph.getEdgeDst(ii++);
//            GNode v2 = graph.getEdgeDst(ii++);
//            GNode v3 = graph.getEdgeDst(ii++);
//        }
        connManager.getGraph().addEdge(firstInterior, newNode);
        connManager.getGraph().addEdge(firstInterior, vertices[neutralVertex]);
//        connManager.getGraph().addEdge(firstInterior, vertices[0]);
//        connManager.getGraph().addEdge(firstInterior, vertices2[(edgeToBreak + 1) % 3]);

        NodeData secondInteriorData = NodeData{true, false};
        auto secondInterior = connManager.createNode(secondInteriorData, ctx);

        connManager.getGraph().addEdge(secondInterior, newNode);
        connManager.getGraph().addEdge(secondInterior, vertices[neutralVertex]);
//        connManager.getGraph().addEdge(secondInterior, connManager.getNeighbourN(interior, neutralVertex));
//        connManager.getGraph().addEdge(secondInterior, vertices2[(edgeToBreak + 2) % 3]);

//        GNode interior1 = createInterior(&newNode, vertices[neutralVertex], vertices[(edgeToBreak + 1) % 3], ctx);
//        graph.addEdge(secondInterior, newNode);
//        graph.addEdge(secondInterior, vertices[neutralVertex]);
        bool switc = false;
        for (int j = 0; j < 3; ++j) {
            if (j != neutralVertex) {
                graph.addEdge(!switc ? firstInterior : secondInterior, vertices[j]);
                const Coordinates &coords = (newNodeData.getCoords() + verticesData[neutralVertex].getCoords() +
                                             verticesData[j].getCoords()) / 3.;
                if (!switc) {
                    firstInterior->getData().setCoords(coords);
                } else {
                    secondInterior->getData().setCoords(coords);
                }
                switc = true;
            }
        }

        graph.removeNode(interior);


//        NodeData testD = NodeData{true, false};
//        connManager.createNode(testD, ctx);
    }

    std::pair<int, int> getEdgeVertices(int edge) const {
        return std::pair<int, int>{edge, (edge + 1) % 3};
    }

    int getNeutralVertex(int edgeToBreak) const {
        return (edgeToBreak + 2) % 3;
    }

    static void logg(const NodeData &interiorData, const std::vector<NodeData> &verticesData) {
        std::cout << "interior: (" << interiorData.getCoords().toString() << "), neighbours: (";
        for (auto vertex : verticesData) {
            std::cout << vertex.getCoords().toString() + ", ";
        }
        std::cout << ") ";
    }

public:

//    explicit Production1(const ConnectivityManager &connManager) : connManager(connManager) {}

    using Production::Production;

    bool execute(GNode interior, galois::UserContext<GNode> &ctx) override {
        ProductionState pState(connManager, interior);

        if (!checkApplicabilityCondition(pState.getInteriorData(), pState.getEdgesIterators())) {
            return false;
        }

        logg(pState.getInteriorData(), pState.getVerticesData());

        int edgeToBreak = getEdgeToBreak(pState.getLengths(), pState.getEdgesData(), pState.getVerticesData());
        if (edgeToBreak == -1) {
            return false;
        }

        breakElement(edgeToBreak, pState.getInterior(), pState.getEdgesData(), pState.getVertices(), pState.getVerticesData(), ctx);
        std::cout << "P1 executed ";

        return true;
    }
};


#endif //GALOIS_PRODUCTION1_H
