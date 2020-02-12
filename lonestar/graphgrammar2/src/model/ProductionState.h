#ifndef GALOIS_PRODUCTIONSTATE_H
#define GALOIS_PRODUCTIONSTATE_H

#include "Graph.h"
#include "../utils/ConnectivityManager.h"

using std::vector;

class ProductionState {
private:
    GNode &interior;
    NodeData &interiorData;
    vector<NodeData> verticesData;
    const vector<GNode> vertices;
    const vector<optional<EdgeIterator>> edgesIterators;
    vector<galois::optional<EdgeData>> edgesData;
    vector<double> lengths;
    vector<int> brokenEdges;
    bool version2D;
    std::function<double(double, double)> zGetter;

public:
    ProductionState(ConnectivityManager &connManager, GNode &interior, bool version2D,
                    std::function<double(double, double)> zGetter) : interior(interior), interiorData(interior->getData()),
                                                             vertices(connManager.getNeighbours(interior)),
                                                             edgesIterators(connManager.getTriangleEdges(vertices)),
                                                             version2D(version2D), zGetter(zGetter) {
        Graph &graph = connManager.getGraph();
        for (int i = 0; i < 3; ++i) {
            auto maybeEdgeIter = edgesIterators[i];
            edgesData.push_back(maybeEdgeIter ? graph.getEdgeData(maybeEdgeIter.get())
                                              : galois::optional<EdgeData>());//TODO: Look for possible optimization
            lengths.push_back(maybeEdgeIter ? edgesData[i].get().getLength() : -1);
            verticesData.push_back(graph.getData(vertices[i]));//TODO: Look for possible optimization
            if (!maybeEdgeIter) {
                brokenEdges.push_back(i);
            }
        }
    }

    std::vector<int> getLongestEdges() const {
        std::vector<int> longestEdges;
        for (int i = 0; i < 3; ++i) {
            if (!less(lengths[i], lengths[(i + 1) % 3]) && !less(lengths[i], lengths[(i + 2) % 3])) {
                longestEdges.push_back(i);
            }
        }
        return longestEdges;
    }

    int getAnyBrokenEdge() const {
        if (!brokenEdges.empty()) {
            return brokenEdges[0];
        } else {
            return -1;
        }
    }

    std::vector<int> getLongestEdgesIncludingBrokenOnes() const {
        std::vector<double> verticesDistances(3);
        for (int i = 0; i < 3; ++i) {
            verticesDistances[i] = verticesData[i].getCoords().dist(verticesData[(i + 1) % 3].getCoords(), version2D);
        }
        return indexesOfMaxElems(verticesDistances);
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

    const vector<int> &getBrokenEdges() const {
        return brokenEdges;
    }

    bool isVersion2D() const {
        return version2D;
    }

    const std::function<double(double, double)> &getZGetter() const {
        return zGetter;
    }
};


#endif //GALOIS_PRODUCTIONSTATE_H
