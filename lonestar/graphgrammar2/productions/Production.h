#ifndef GALOIS_PRODUCTION_H
#define GALOIS_PRODUCTION_H

#include "../model/ProductionState.h"

class Production {
protected:
    ConnectivityManager connManager;

public:
    explicit Production(const ConnectivityManager &connManager) : connManager(connManager) {}
    virtual bool execute(GNode interior, galois::UserContext<GNode> &ctx) = 0;
};

#endif //GALOIS_PRODUCTION_H
