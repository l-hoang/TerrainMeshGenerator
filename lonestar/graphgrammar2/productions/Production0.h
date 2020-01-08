#ifndef GALOIS_PRODUCTION0_H
#define GALOIS_PRODUCTION0_H

#include "../utils/ConnectivityManager.h"
#include "../utils/utils.h"

class Production0 {
private:

    bool checkBasicApplicabilityCondition(const NodeData &nodeData) const {
        return nodeData.isHyperEdge();
    }

    static void logg(const NodeData &interiorData, const std::vector<NodeData> &verticesData) {
        std::cout << "interior: (" << interiorData.getCoords().toString() << "), neighbours: (";
        for (auto vertex : verticesData) {
            std::cout << vertex.getCoords().toString() + ", ";
        }
        std::cout << ") ";
    }

public:

    bool execute(GNode interior, galois::UserContext<GNode> &ctx) {
        NodeData &interiorData = interior->getData();

        if (!checkBasicApplicabilityCondition(interiorData)) {
            return false;
        }

        interiorData.setToRefine(true);
//        ctx.push(interior);
        return true;
    }


};


#endif //GALOIS_PRODUCTION1_H
