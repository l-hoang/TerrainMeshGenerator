#ifndef GALOIS_CONDITIONCHECKER_H
#define GALOIS_CONDITIONCHECKER_H


#include "../model/Graph.h"

class ConditionChecker {
public:
    virtual bool execute(GNode &node) = 0;
};


#endif //GALOIS_CONDITIONCHECKER_H
