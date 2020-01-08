#ifndef GALOIS_PRODUCTION3_H
#define GALOIS_PRODUCTION3_H

#include "Production.h"
#include "../utils/ConnectivityManager.h"
#include "../utils/utils.h"

class Production3 : Production {
private:

    bool checkApplicabilityCondition(const std::vector<optional<EdgeIterator>> &edgesIterators) const {
        return connManager.countBrokenEdges(edgesIterators) == 1;
    }

    int getBrokenEdge(const std::vector<galois::optional<EdgeIterator>> &edgesIterators) const {
        for (int i = 0; i < 3; ++i) {
            if (!edgesIterators[i].is_initialized()) {
                return i;
            }
        }
        return -1;
    }

    bool checkIfBrokenEdgeIsTheLongest(int brokenEdge, const std::vector<optional<EdgeIterator>> &edgesIterators,
                                       const std::vector<GNode> &vertices, const std::vector<NodeData> &verticesData) {
        std::vector<double> lengths(4);
        Graph &graph = connManager.getGraph();
        for (int i = 0, j = 0; i < 3; ++i) {
            if (i != brokenEdge) {
                lengths[j++] = graph.getEdgeData(edgesIterators[i].get()).getLength();
            } else {
                const std::pair<int, int> &brokenEdgeVertices = getEdgeVertices(brokenEdge);
                GNode &hangingNode = connManager.findNodeBetween(vertices[brokenEdgeVertices.first],
                                                                 vertices[brokenEdgeVertices.second]).get();
                lengths[2] = graph.getEdgeData(
                        graph.findEdge(vertices[brokenEdgeVertices.first], hangingNode)).getLength();
                lengths[3] = graph.getEdgeData(
                        graph.findEdge(vertices[brokenEdgeVertices.second], hangingNode)).getLength();
            }
        }
        return !less(lengths[2] + lengths[3], lengths[0]) && !less(lengths[2] + lengths[3], lengths[1]);
    }

//    std::vector<int> getLongestEdges(const std::vector<double> &lengths) const {
//        std::vector<int> longestEdges;
//        for (int i = 0; i < 3; ++i) {
//            if (!less(lengths[i], lengths[(i + 1) % 3]) && !less(lengths[i], lengths[(i + 2) % 3])) {
//                longestEdges.push_back(i);
//            }
//        }
//        return longestEdges;
//    }


    void breakElement(int edgeToBreak, ProductionState &pState, galois::UserContext<GNode> &ctx) const {
        Graph &graph = connManager.getGraph();
        const std::pair<int, int> &brokenEdgeVertices = getEdgeVertices(edgeToBreak);
        int neutralVertex = getNeutralVertex(edgeToBreak);
        GNode &hangingNode = connManager.findNodeBetween(pState.getVertices()[brokenEdgeVertices.first],
                                                         pState.getVertices()[brokenEdgeVertices.second]).get();
        NodeData &hNodeData = graph.getData(hangingNode);

        addEdge(graph, hangingNode, pState.getVertices()[neutralVertex], false,
                hNodeData.getCoords().dist(pState.getVerticesData()[neutralVertex].getCoords()),
                (hNodeData.getCoords() + pState.getVerticesData()[neutralVertex].getCoords()) / 2);

        connManager.createInterior(hangingNode, pState.getVertices()[neutralVertex],
                                   pState.getVertices()[brokenEdgeVertices.first], ctx);
        connManager.createInterior(hangingNode, pState.getVertices()[neutralVertex],
                                   pState.getVertices()[brokenEdgeVertices.second], ctx);

        hNodeData.setHanging(false);

        graph.removeNode(pState.getInterior());
    }

    void addEdge(Graph &graph, GNode const &node1, GNode const &node2, bool border, double length,
                 const Coordinates &middlePoint) const {
        const EdgeIterator &newEdge = graph.addEdge(node1, node2);
        graph.getEdgeData(newEdge).setBorder(border);
        graph.getEdgeData(newEdge).setLength(length);
        graph.getEdgeData(newEdge).setMiddlePoint(
                middlePoint);
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

    using Production::Production;

    bool execute(ProductionState &pState, galois::UserContext<GNode> &ctx) override {
        if (!checkApplicabilityCondition(pState.getEdgesIterators())) {
            return false;
        }

        int brokenEdge = getBrokenEdge(pState.getEdgesIterators());
        assert(brokenEdge != -1);

        if (checkIfBrokenEdgeIsTheLongest(brokenEdge, pState.getEdgesIterators(), pState.getVertices(),
                                          pState.getVerticesData())) {
            return false;
        }

        logg(pState.getInteriorData(), pState.getVerticesData());

        const vector<int> &longestEdges = getLongestEdges(pState.getLengths());

        for (int longest : longestEdges) {
            if (pState.getEdgesData()[longest].get().isBorder()) {
                breakElement(longest, pState, ctx);
                std::cout << "P3 executed ";
                return true;
            }
        }
        for (int longest : longestEdges) {
            if (!pState.getVerticesData()[getEdgeVertices(longest).first].isHanging() &&
                !pState.getVerticesData()[getEdgeVertices(longest).second].isHanging()) {

                breakElement(longest, pState, ctx);
                std::cout << "P3 executed ";
                return true;
            }
        }
        return false;
    }

};


#endif //GALOIS_PRODUCTION3_H
