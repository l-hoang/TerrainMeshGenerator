#ifndef GALOIS_SINGLENODE_H
#define GALOIS_SINGLENODE_H

#include "NodeData.h"
#include "Coordinates.h"

class SingleNode : public NodeData {
private:
    Coordinates coords;

public:
    explicit SingleNode(const Coordinates &coords) : NodeData(), coords(coords) {isHyperEdge = false;}

    SingleNode(const double x, const double y, const double z) : NodeData(), coords(x,y,z) {isHyperEdge = false;}

//    bool isHyperEdge() override {
//        return false;
//    }

    const Coordinates getCoords() const {
        return coords;
    }

    void setCoords(const Coordinates &coords) {
        SingleNode::coords = coords;
    }
};
#endif //GALOIS_SINGLENODE_H
