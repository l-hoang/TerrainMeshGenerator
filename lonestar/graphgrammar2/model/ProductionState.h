#ifndef GALOIS_PRODUCTIONSTATE_H
#define GALOIS_PRODUCTIONSTATE_H

using std::vector;

class ProductionState {
private:
    GNode &interior;
    NodeData &interiorData;
    vector<galois::optional<EdgeData>> edgesData;
    vector<double> lengths;
    vector<NodeData> verticesData;
    const vector<GNode> vertices;
    const vector<optional<EdgeIterator>> edgesIterators;

public:
    ProductionState(ConnectivityManager &connManager, GNode &interior) : interior(interior),
                                                                         interiorData(interior->getData()),
                                                                         vertices(connManager.getNeighbours(interior)),
                                                                         edgesIterators(connManager.getTriangleEdges(
                                                                                 vertices)) {
        Graph &graph = connManager.getGraph();
        for (int i = 0; i < 3; ++i) {
            auto maybeEdgeIter = edgesIterators[i];
            edgesData.push_back(maybeEdgeIter ? graph.getEdgeData(maybeEdgeIter.get())
                                              : galois::optional<EdgeData>());//TODO: Probably copying
            lengths.push_back(maybeEdgeIter ? edgesData[i].get().getLength() : -1);
            verticesData.push_back(graph.getData(vertices[i]));//TODO: Probably copying
        }
    }

    GNode &getInterior() const {
        return interior;
    }

    NodeData &getInteriorData() const {
        return interiorData;
    }

    const vector<galois::optional<EdgeData>> &getEdgesData() const {
        return edgesData;
    }

    const vector<double> &getLengths() const {
        return lengths;
    }

    const vector<NodeData> &getVerticesData() const {
        return verticesData;
    }

    const vector<GNode> &getVertices() const {
        return vertices;
    }

    const vector<optional<EdgeIterator>> &getEdgesIterators() const {
        return edgesIterators;
    }
};


#endif //GALOIS_PRODUCTIONSTATE_H
