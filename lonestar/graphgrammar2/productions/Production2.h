#ifndef GALOIS_PRODUCTION2_H
#define GALOIS_PRODUCTION2_H

#include "../utils/ConnectivityManager.h"
#include "../utils/utils.h"

class Production2 {
private:
    ConnectivityManager connManager;

    bool checkBasicApplicabilityCondition(const NodeData &nodeData) const {
        return nodeData.isHyperEdge();
    }

    bool checkComplexApplicabilityCondition(const std::vector<optional<EdgeIterator>> &edgesIterators) const {
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
                                       const std::vector<GNode> &vertices, std::vector<NodeData> verticesData) {
        std::vector<double> lengths(4);
        Graph &graph = connManager.getGraph();
        for (int i = 0, j = 0; i < 3; ++i) {
            if (i != brokenEdge) {
                lengths[j++] = graph.getEdgeData(edgesIterators[i].get()).getLength();
            } else {
                const std::pair<int, int> &brokenEdgeVertices = getEdgeVertices(brokenEdge);
                GNode &hangingNode = connManager.findNodeBetween(vertices[brokenEdgeVertices.first],
                                                                 vertices[brokenEdgeVertices.second],
                                                                 verticesData[getNeutralVertex(brokenEdge)]
                                                                         .getCoords()).get();
                lengths[2] = graph.getEdgeData(
                        graph.findEdge(vertices[brokenEdgeVertices.first], hangingNode)).getLength();
                lengths[3] = graph.getEdgeData(
                        graph.findEdge(vertices[brokenEdgeVertices.second], hangingNode)).getLength();
            }
        }
        return !less(lengths[2] + lengths[3], lengths[0]) && !less(lengths[2] + lengths[3], lengths[1]);
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

    void breakElement(int edgeToBreak, const GNode &hangingNode, GNode &interior, const std::vector<GNode> &vertices,
                      const std::vector<NodeData> &verticesData, galois::UserContext<GNode> &ctx) const {
        Graph &graph = connManager.getGraph();
//        bool breakingOnBorder = edgesData[edgeToBreak].isBorder();
        int neutralVertex = getNeutralVertex(edgeToBreak);

//        auto edgePair = connManager.findSrc(edgesData[edgeToBreak]);
//        graph.removeEdge(edgePair.first, edgePair.second);
//        NodeData newNodeData = NodeData{false, edgesData[edgeToBreak].getMiddlePoint(), true};
//        auto newNode = graph.createNode(newNodeData);
//        graph.addNode(newNode);
//        ctx.push(newNode);

        NodeData &hNodeData = graph.getData(hangingNode);


        const EdgeIterator &newEdge = graph.addEdge(hangingNode, vertices[getNeutralVertex(edgeToBreak)]);
        graph.getEdgeData(newEdge).setBorder(false);
        graph.getEdgeData(newEdge).setMiddlePoint(
                (hNodeData.getCoords() + verticesData[getNeutralVertex(edgeToBreak)].getCoords()) / 2);

//        for (int i = 0; i < 3; ++i) {
//            auto vertexData = verticesData[i];
//            auto edge = graph.addEdge(newNode, vertices[i]);
//            graph.getEdgeData(edge).setBorder(i != neutralVertex ? breakingOnBorder : false);
//            graph.getEdgeData(edge).setMiddlePoint(
//                    Coordinates{(newNodeData.getCoords().getX() + vertexData.getCoords().getX()) / 2.,
//                                (newNodeData.getCoords().getY() + vertexData.getCoords().getY()) / 2.,
//                                (newNodeData.getCoords().getZ() + vertexData.getCoords().getZ()) / 2.});
//            graph.getEdgeData(edge).setLength(
//                    sqrt(pow(newNodeData.getCoords().getX(), 2) + pow(newNodeData.getCoords().getY(), 2) + pow(
//                            newNodeData.getCoords().getZ(), 2)));
//        }
//        const NodeData &firstInteriorData = NodeData{true, false}; //Refactor me
//        const NodeData &secondInteriorData = NodeData{true, false};
//        auto firstInterior = graph.createNode(firstInteriorData);
//        auto secondInterior = graph.createNode(secondInteriorData);
//        graph.addNode(firstInterior);
//        graph.addNode(secondInterior);
//        ctx.push(firstInterior);
//        ctx.push(secondInterior);
//        graph.addEdge(firstInterior, hangingNode);
//        graph.addEdge(secondInterior, hangingNode);
//        graph.addEdge(firstInterior, vertices[neutralVertex]);
//        graph.addEdge(secondInterior, vertices[neutralVertex]);
//        bool switc = false;
//        for (int j = 0; j < 3; ++j) {
//            if (j != neutralVertex) {
//                graph.addEdge(!switc ? firstInterior : secondInterior, vertices[j]);
//                switc = true;
//            }
//        }



        NodeData firstInteriorData = NodeData{true, true};
        auto firstInterior = connManager.createNode(firstInteriorData, ctx);
//        std::vector<GNode> vertices2;
//        for (Graph::edge_iterator ii = graph.edge_begin(interior), ee = graph.edge_end(interior); ii != ee; ++ii) {
//            Graph::edge_iterator ii = graph.edge_begin(interior);
//            GNode v1 = graph.getEdgeDst(ii++);
//            GNode v2 = graph.getEdgeDst(ii++);
//            GNode v3 = graph.getEdgeDst(ii++);
//        }
        connManager.getGraph().addEdge(firstInterior, hangingNode);
        connManager.getGraph().addEdge(firstInterior, vertices[neutralVertex]);
//        connManager.getGraph().addEdge(firstInterior, vertices[0]);
//        connManager.getGraph().addEdge(firstInterior, vertices2[(edgeToBreak + 1) % 3]);

        NodeData secondInteriorData = NodeData{true, true};
        auto secondInterior = connManager.createNode(secondInteriorData, ctx);

        connManager.getGraph().addEdge(secondInterior, hangingNode);
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
                if (!switc) {
                    firstInterior->getData().setCoords((hNodeData.getCoords() + verticesData[neutralVertex].getCoords() +
                                                 verticesData[j].getCoords()) / 3.);
                } else {
                    secondInterior->getData().setCoords((hNodeData.getCoords() + verticesData[neutralVertex].getCoords() +
                                                  verticesData[j].getCoords()) / 3.);
                }
                switc = true;
            }
        }

        hNodeData.setHanging(false);

        graph.removeNode(interior);
    }

    std::pair<int, int> getEdgeVertices(int edge) const {
        return std::pair<int, int>{edge, (edge + 1) % 3};
    }

    int getNeutralVertex(int edgeToBreak) const {
        return (edgeToBreak + 2) % 3;
    }

    static void logg(const NodeData &interiorData, const std::vector<NodeData>& verticesData) {
        std::cout << "interior: (" << interiorData.getCoords().toString() << "), neighbours: (";
        for (auto vertex : verticesData) {
            std::cout << vertex.getCoords().toString() + ", ";
        }
        std::cout << ") ";
    }

public:

    explicit Production2(const ConnectivityManager &connManager) : connManager(connManager) {}

    bool execute(GNode interior, galois::UserContext<GNode> &ctx) {
        Graph &graph = connManager.getGraph();
        NodeData &interiorData = graph.getData(interior);

        if (!checkBasicApplicabilityCondition(interiorData)) {
            return false;
        }

        const std::vector<GNode> &vertices = connManager.getNeighbours(interior);
        const std::vector<optional<EdgeIterator>> &edgesIterators = connManager.getTriangleEdges(vertices);

        if (!checkComplexApplicabilityCondition(edgesIterators)) {
            return false;
        }

        std::vector<NodeData> verticesData;
        for (int i = 0; i < 3; ++i) {
            verticesData.push_back(graph.getData(vertices[i]));
        }

        logg(interiorData, verticesData);

        int brokenEdge = getBrokenEdge(edgesIterators);
        assert(brokenEdge != -1);

        if (!checkIfBrokenEdgeIsTheLongest(brokenEdge, edgesIterators, vertices, verticesData)) {
            return false;
        }
        const std::pair<int, int> &brokenEdgeVertices = getEdgeVertices(brokenEdge);
        GNode &hangingNode = connManager.findNodeBetween(vertices[brokenEdgeVertices.first],
                                                         vertices[brokenEdgeVertices.second],
                                                         verticesData[getNeutralVertex(brokenEdge)].getCoords()).get();

        breakElement(brokenEdge, hangingNode, interior, vertices, verticesData, ctx);
        std::cout << "P2 executed ";
        return true;
    }

};


#endif //GALOIS_PRODUCTION2_H
