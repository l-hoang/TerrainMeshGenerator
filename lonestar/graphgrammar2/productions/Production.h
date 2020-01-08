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



public:
    explicit Production(const ConnectivityManager &connManager) : connManager(connManager) {}
    virtual bool execute(ProductionState &pState, galois::UserContext<GNode> &ctx) = 0;
};

#endif //GALOIS_PRODUCTION_H
