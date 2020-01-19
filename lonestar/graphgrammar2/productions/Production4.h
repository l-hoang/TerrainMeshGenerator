#ifndef GALOIS_PRODUCTION4_H
#define GALOIS_PRODUCTION4_H

#include "Production.h"
#include "../utils/ConnectivityManager.h"
#include "../utils/utils.h"

class Production4 : Production {
private:

    bool checkApplicabilityCondition(const std::vector<optional<EdgeIterator>> &edgesIterators) const {
        return connManager.countBrokenEdges(edgesIterators) == 2;
    }

public:

    using Production::Production;

    bool execute(ProductionState &pState, galois::UserContext<GNode> &ctx) override {
        if (!checkApplicabilityCondition(pState.getEdgesIterators())) {
            return false;
        }
        logg(pState.getInteriorData(), pState.getVerticesData());

        const vector<int> &longestEdges = getLongestEdgesIncludingBrokenOnes(pState.getVerticesData());
        for (int longest : longestEdges) {
            const vector<int> &brokenEdges = getBrokenEdges(pState.getEdgesIterators());
            if (std::find(brokenEdges.begin(), brokenEdges.end(), longest) != brokenEdges.end()) {
                breakElementWithHangingNode(longest, pState, ctx);
                std::cout << "P4 executed ";
                return true;
            }
        }
        return false;
    }

};


#endif //GALOIS_PRODUCTION4_H
