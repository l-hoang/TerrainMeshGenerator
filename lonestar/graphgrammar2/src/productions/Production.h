#ifndef GALOIS_PRODUCTION_H
#define GALOIS_PRODUCTION_H

#include "../model/ProductionState.h"

class Production {

public:
    explicit Production(const ConnectivityManager &connManager) : connManager(connManager) {}

    virtual bool execute(ProductionState &pState, galois::UserContext<GNode> &ctx) = 0;

protected:
    ConnectivityManager connManager;

    bool checkIfBrokenEdgeIsTheLongest(int brokenEdge, const std::vector<optional<EdgeIterator>> &edgesIterators,
                                       const std::vector<GNode> &vertices,
                                       const std::vector<NodeData> &verticesData) const {
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

    std::pair<int, int> getEdgeVertices(int edge) const {
        return std::pair<int, int>{edge, (edge + 1) % 3};
    }

    int getNeutralVertex(int edgeToBreak) const {
        return (edgeToBreak + 2) % 3;
    }

    void breakElementWithHangingNode(int edgeToBreak, ProductionState &pState, galois::UserContext<GNode> &ctx) const {
        GNode hangingNode = getHangingNode(edgeToBreak, pState);

        breakElementUsingNode(edgeToBreak, hangingNode, pState, ctx);

        hangingNode->getData().setHanging(false);
    }

    void breakElementWithoutHangingNode(int edgeToBreak, ProductionState &pState,
                                        galois::UserContext<GNode> &ctx) const {
        GNode newNode = createNodeOnEdge(edgeToBreak, pState, ctx);
        breakElementUsingNode(edgeToBreak, newNode, pState, ctx);
    }


    static void logg(const NodeData &interiorData, const std::vector<NodeData> &verticesData) {
        std::cout << "interior: (" << interiorData.getCoords().toString() << "), neighbours: (";
        for (auto vertex : verticesData) {
            std::cout << vertex.getCoords().toString() + ", ";
        }
        std::cout << ") ";
    }

private:
    GNode createNodeOnEdge(int edgeToBreak, ProductionState &pState, galois::UserContext<GNode> &ctx) const {
        Graph &graph = connManager.getGraph();
        const vector<galois::optional<EdgeData>> &edgesData = pState.getEdgesData();
        bool breakingOnBorder = edgesData[edgeToBreak].get().isBorder();
        int neutralVertex = getNeutralVertex(edgeToBreak);

//        const EdgeIterator &edge = pState.getEdgesIterators()[edgeToBreak].get();
//        graph.removeEdge(*(graph.getEdgeData(edge).getSrc()), edge);

//        auto edgePair = connManager.findSrc(pState.getEdgesIterators()[edgeToBreak].get());
//        auto edgePair = connManager.findSrc(edgesData[edgeToBreak].get());
//        graph.removeEdge(edgePair.first, edgePair.second);
        const std::pair<int, int> &edgeVertices = getEdgeVertices(edgeToBreak);
        connManager.removeEdge(pState.getVertices()[edgeVertices.first], pState.getVertices()[edgeVertices.second]);

        const Coordinates &newPointCoords = getNewPointCoords(pState.getVerticesData()[edgeVertices.first].getCoords(),
                                                              pState.getVerticesData()[edgeVertices.second].getCoords(),
                                                              pState.getZGetter());
        NodeData newNodeData = NodeData{false, newPointCoords, !breakingOnBorder};
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
            graph.getEdgeData(edge).setLength(
                    newNodeData.getCoords().dist(vertexData.getCoords(), pState.isVersion2D()));
        }
        return newNode;
    }

    void breakElementUsingNode(int edgeToBreak, GNode const &hangingNode, const ProductionState &pState,
                               galois::UserContext<GNode> &ctx) const {
        const std::pair<int, int> &brokenEdgeVertices = getEdgeVertices(edgeToBreak);
        Graph &graph = connManager.getGraph();
        int neutralVertex = getNeutralVertex(edgeToBreak);
        NodeData hNodeData = hangingNode->getData();
        double length = 0;
        length = hNodeData.getCoords().dist(pState.getVerticesData()[neutralVertex].getCoords(), pState.isVersion2D());
        addEdge(graph, hangingNode, pState.getVertices()[neutralVertex], false,
                length,
                (hNodeData.getCoords() + pState.getVerticesData()[neutralVertex].getCoords()) / 2);

        connManager.createInterior(hangingNode, pState.getVertices()[neutralVertex],
                                   pState.getVertices()[brokenEdgeVertices.first], ctx);
        connManager.createInterior(hangingNode, pState.getVertices()[neutralVertex],
                                   pState.getVertices()[brokenEdgeVertices.second], ctx);

        graph.removeNode(pState.getInterior());
    }

    GNode getHangingNode(int edgeToBreak, const ProductionState &pState) const {
        const std::pair<int, int> &brokenEdgeVertices = getEdgeVertices(edgeToBreak);
        return connManager.findNodeBetween(pState.getVertices()[brokenEdgeVertices.first],
                                           pState.getVertices()[brokenEdgeVertices.second]).get();
    }

    void addEdge(Graph &graph, GNode const &node1, GNode const &node2, bool border, double length,
                 const Coordinates &middlePoint) const {
        const EdgeIterator &newEdge = graph.addEdge(node1, node2);
        graph.getEdgeData(newEdge).setBorder(border);
        graph.getEdgeData(newEdge).setLength(length);
        graph.getEdgeData(newEdge).setMiddlePoint(middlePoint);
    }

    Coordinates getNewPointCoords(const Coordinates &coords1, const Coordinates &coords2,
                                  const std::function<double(double, double)> &zGetter) const {
        double x = (coords1.getX() + coords2.getX()) / 2.;
        double y = (coords1.getY() + coords2.getY()) / 2.;
        return {x, y, zGetter(x, y)};
    }
};

#endif //GALOIS_PRODUCTION_H
