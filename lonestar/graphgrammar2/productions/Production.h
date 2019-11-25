#ifndef GALOIS_PRODUCTION_H
#define GALOIS_PRODUCTION_H

class Production {
public:
    virtual bool isPossible(GNode node, Graph &graph);
    virtual void execute(GNode node, Graph &graph);
};

#endif //GALOIS_PRODUCTION_H
