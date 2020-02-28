#ifndef GALOIS_DUMMYCONDITIONCHECKER_H
#define GALOIS_DUMMYCONDITIONCHECKER_H

#include "ConditionChecker.h"

class DummyConditionChecker : ConditionChecker {
public:
  bool execute(GNode& node) override {
    NodeData& nodeData = node->getData();
    if (!nodeData.isHyperEdge()) {
      return false;
    }
    nodeData.setToRefine(true);
    return true;
  }
};

#endif // GALOIS_DUMMYCONDITIONCHECKER_H
