#ifndef GALOIS_PRODUCTION_H
#define GALOIS_PRODUCTION_H

#include "../model/ProductionState.h"

class Production {
protected:
    ConnectivityManager connManager;

    std::vector<int> getLongestEdges(const std::vector<double> &lengths) const {
        std::vector<int> longestEdges;
        for (int i = 0; i < 3; ++i) {
            if (!less(lengths[i], lengths[(i + 1) % 3]) && !less(lengths[i], lengths[(i + 2) % 3])) {
                longestEdges.push_back(i);
            }
        }
        return longestEdges;
    }

    int getBrokenEdge(const std::vector<galois::optional<EdgeIterator>> &edgesIterators) const {
        const vector<int> &brokenEdges = getBrokenEdges(edgesIterators);
        if (!brokenEdges.empty()) {
            return brokenEdges[0];
        } else {
            return -1;
        }
    }

    std::vector<int> getBrokenEdges(const std::vector<galois::optional<EdgeIterator>> &edgesIterators) const {
        std::vector<int> result;
        for (int i = 0; i < 3; ++i) {
            if (!edgesIterators[i]) {
                result.push_back(i);
            }
        }
        return result;
    }

    std::vector<int> getLongestEdgesIncludingBrokenOnes(const std::vector<NodeData> &verticesData) const {
        std::vector<double> lengths(3);
        for (int i = 0; i < 3; ++i) {
            lengths[i] = verticesData[i].getCoords().dist(verticesData[(i+1)%3].getCoords());
        }
        return indexesOfMaxElems(lengths);
    }

    std::pair<int, int> getEdgeVertices(int edge) const {
        return std::pair<int, int>{edge, (edge + 1) % 3};
    }


    void breakElementWithHangingNode(int edgeToBreak, ProductionState &pState, galois::UserContext<GNode> &ctx) const {
        const std::pair<int, int> &brokenEdgeVertices = getEdgeVertices(edgeToBreak);
        GNode &hangingNode = connManager.findNodeBetween(pState.getVertices()[brokenEdgeVertices.first],
                                                         pState.getVertices()[brokenEdgeVertices.second]).get();

        breakElementUsingNode(edgeToBreak, hangingNode, pState, ctx);

        hangingNode->getData().setHanging(false);
    }

    void breakElementWithoutHangingNode(int edgeToBreak, ProductionState &pState, galois::UserContext<GNode> &ctx) const {
        GNode newNode = createNodeOnEdge(edgeToBreak, pState, ctx);

        breakElementUsingNode(edgeToBreak, newNode, pState, ctx);
    }

    GNode createNodeOnEdge(int edgeToBreak, ProductionState &pState, galois::UserContext<GNode> &ctx) const {
        Graph &graph = connManager.getGraph();
        const vector<galois::optional<EdgeData>> &edgesData = pState.getEdgesData();
        bool breakingOnBorder = edgesData[edgeToBreak].get().isBorder();
        int neutralVertex = getNeutralVertex(edgeToBreak);

        auto edgePair = connManager.findSrc(edgesData[edgeToBreak].get());
        graph.removeEdge(edgePair.first, edgePair.second);
        NodeData newNodeData = NodeData{false, edgesData[edgeToBreak].get().getMiddlePoint(), !breakingOnBorder};
        GNode newNode = graph.createNode(newNodeData);
        graph.addNode(newNode);
        ctx.push(newNode);
        for (int i = 0; i < 3; ++i) {
            auto vertexData = pState.getVerticesData()[i];
            auto edge = graph.addEdge(newNode, pState.getVertices()[i]);
            graph.getEdgeData(edge).setBorder(i != neutralVertex ? breakingOnBorder : false);
            graph.getEdgeData(edge).setMiddlePoint(
                    (newNodeData.getCoords().getX() + vertexData.getCoords().getX()) / 2.,
                    (newNodeData.getCoords().getY() + vertexData.getCoords().getY()) / 2.,
                    (newNodeData.getCoords().getZ() + vertexData.getCoords().getZ()) / 2.);
            graph.getEdgeData(edge).setLength(newNodeData.getCoords().dist(vertexData.getCoords()));
        }
        return newNode;
    }

    void breakElementUsingNode(int edgeToBreak, GNode const &hangingNode, const ProductionState &pState,
                               galois::UserContext<GNode> &ctx) const {
        const std::pair<int, int> &brokenEdgeVertices = getEdgeVertices(edgeToBreak);
        Graph &graph = connManager.getGraph();
        int neutralVertex = getNeutralVertex(edgeToBreak);
        NodeData hNodeData = hangingNode->getData();
        addEdge(graph, hangingNode, pState.getVertices()[neutralVertex], false,
                hNodeData.getCoords().dist(pState.getVerticesData()[neutralVertex].getCoords()),
                (hNodeData.getCoords() + pState.getVerticesData()[neutralVertex].getCoords()) / 2);

        connManager.createInterior(hangingNode, pState.getVertices()[neutralVertex],
                                   pState.getVertices()[brokenEdgeVertices.first], ctx);
        connManager.createInterior(hangingNode, pState.getVertices()[neutralVertex],
                                   pState.getVertices()[brokenEdgeVertices.second], ctx);

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


    static void logg(const NodeData &interiorData, const std::vector<NodeData> &verticesData) {
        std::cout << "interior: (" << interiorData.getCoords().toString() << "), neighbours: (";
        for (auto vertex : verticesData) {
            std::cout << vertex.getCoords().toString() + ", ";
        }
        std::cout << ") ";
    }


public:
    explicit Production(const ConnectivityManager &connManager) : connManager(connManager) {}
    virtual bool execute(ProductionState &pState, galois::UserContext<GNode> &ctx) = 0;
};

#endif //GALOIS_PRODUCTION_H
