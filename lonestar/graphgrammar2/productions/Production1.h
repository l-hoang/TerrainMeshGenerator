#ifndef GALOIS_PRODUCTION1_H
#define GALOIS_PRODUCTION1_H

static const double EPS = 1e-4;

#include "../utils/ConnectivityManager.h"
#include "../utils/utils.h"

class Production1 {
private:
    ConnectivityManager connManager;

    bool checkBasicApplicabilityCondition(const NodeData &nodeData) const {
        return nodeData.isHyperEdge && nodeData.isToRefine();
    }

    bool checkComplexApplicabilityCondition(const std::vector<GNode> &vertices,
                                            const std::vector<optional<EdgeIterator>> &edgesIterators) const {
        return !connManager.hasBrokenEdge(edgesIterators) /*|| !hasMultipleHangingVertices(vertices)*/;
    }

    int getEdgeToBreak(const std::vector<double> &lengths, const std::vector<EdgeData> &edgesData,
                       const std::vector<NodeData>& verticesData) const {
        std::vector<int> longestEdges = getLongestEdges(lengths);
        for (int longest : longestEdges) {
            if (edgesData[longest].isBorder()) {
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

    void breakElement(int edgeToBreak, const std::vector<EdgeData> &edgesData, const std::vector<GNode> &vertices,
                      const std::vector<NodeData> &verticesData, galois::UserContext<GNode> &ctx) const {
        Graph &graph = connManager.getGraph();
        bool breakingOnBorder = edgesData[edgeToBreak].isBorder();
        int neutralVertex = getNeutralVertex(edgeToBreak);

        auto edgePair = connManager.findSrc(edgesData[edgeToBreak]);
        graph.removeEdge(edgePair.first, edgePair.second);
        NodeData newNodeData = NodeData{false, edgesData[edgeToBreak].getMiddlePoint(), true};
        auto newNode = graph.createNode(newNodeData);
        graph.addNode(newNode);
        ctx.push(newNode);
        for (int i = 0; i < 3; ++i) {
            auto vertexData = verticesData[i];
            auto edge = graph.addEdge(newNode, vertices[i]);
            graph.getEdgeData(edge).setBorder(i != neutralVertex ? breakingOnBorder : false);
            graph.getEdgeData(edge).setMiddlePoint(
                    Coordinates{(newNodeData.getCoords().getX() + vertexData.getCoords().getX()) / 2.,
                                (newNodeData.getCoords().getY() + vertexData.getCoords().getY()) / 2.,
                                (newNodeData.getCoords().getZ() + vertexData.getCoords().getZ()) / 2.});
            graph.getEdgeData(edge).setLength(
                    sqrt(pow(newNodeData.getCoords().getX(), 2) + pow(newNodeData.getCoords().getY(), 2) + pow(
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
    }

    std::pair<int, int> getEdgeVertices(int edge) const {
        return std::pair<int, int>{edge, (edge + 1) % 3};
    }

    int getNeutralVertex(int edgeToBreak) const {
        return (edgeToBreak + 2) % 3;
    }

public:

    explicit Production1(const ConnectivityManager &connManager) : connManager(connManager) {}

    bool execute(GNode interior, galois::UserContext<GNode> &ctx) {
        Graph &graph = connManager.getGraph();
        NodeData &nodeData = graph.getData(interior);

        if (!checkBasicApplicabilityCondition(nodeData)) {
            return false;
        }

        const std::vector<GNode> &vertices = connManager.getNeighbours(interior);
        const std::vector<optional<EdgeIterator>> &edgesIterators = connManager.getTriangleEdges(vertices);

        if (!checkComplexApplicabilityCondition(vertices, edgesIterators)) {
            return false;
        }

        std::vector<EdgeData> edgesData;
        std::vector<double> lengths(3);
        std::vector<NodeData> verticesData;
        for (int i = 0; i < 3; ++i) {
            edgesData.push_back(graph.getEdgeData(edgesIterators[i].get()));
            lengths[i] = edgesData[i].getLength();
            verticesData.push_back(graph.getData(vertices[i]));
        }

        int edgeToBreak = getEdgeToBreak(lengths, edgesData, verticesData);
        if (edgeToBreak == -1) {
            return false;
        }

        breakElement(edgeToBreak, edgesData, vertices, verticesData, ctx);

        return true;
    }
};


#endif //GALOIS_PRODUCTION1_H
