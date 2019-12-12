#ifndef GALOIS_NODEDATA_H
#define GALOIS_NODEDATA_H

#include "Coordinates.h"


class NodeData {
private:
    Coordinates coords;
    bool toRefine;
    bool hanging;
public:
    bool isHyperEdge;

    NodeData(bool isHyperEdge, const Coordinates &coords, bool hanging) : coords(coords), isHyperEdge(isHyperEdge), hanging(hanging) {}

    NodeData(bool isHyperEdge, bool toRefine) : toRefine(toRefine), isHyperEdge(isHyperEdge), coords() {}

    Coordinates getCoords() const {
        return coords;
    }

    void setCoords(const Coordinates &coords) {
        NodeData::coords = coords;
    }

    bool isToRefine() const {
        return toRefine;
    }

    void setToRefine(bool toRefine) {
        NodeData::toRefine = toRefine;
    }

    bool isHanging() const {
        return hanging;
    }

    void setHanging(bool hanging) {
        NodeData::hanging = hanging;
    }
};


#endif //GALOIS_NODEDATA_H
