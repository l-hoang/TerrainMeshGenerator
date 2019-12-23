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

    NodeData(bool isHyperEdge, const Coordinates &coords, bool hanging) : coords(coords), isHyperEdge(isHyperEdge),
                                                                          hanging(hanging) {}

    NodeData(bool isHyperEdge, bool toRefine) : toRefine(toRefine), isHyperEdge(isHyperEdge), coords() {}

    NodeData(bool isHyperEdge, bool toRefine, Coordinates coords) : toRefine(toRefine), isHyperEdge(isHyperEdge),
                                                                    coords(coords) {}

    Coordinates getCoords() const {
        return coords;
    }

    void setCoords(const Coordinates &coords) {
        NodeData::coords.setXYZ(coords.getX(), coords.getY(), coords.getZ());
    }

    void setCoords(const double x, const double y, const double z) {
        NodeData::coords.setXYZ(x, y, z);
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

    bool operator==(const NodeData &rhs) const {
        return isHyperEdge == rhs.isHyperEdge &&
               coords == rhs.coords &&
               (isHyperEdge ? toRefine == rhs.toRefine : hanging == rhs.hanging);
    }

    bool operator!=(const NodeData &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const NodeData &rhs) const {
        if (isHyperEdge < rhs.isHyperEdge)
            return true;
        if (rhs.isHyperEdge < isHyperEdge)
            return false;
        if (coords < rhs.coords)
            return true;
        if (rhs.coords < coords)
            return false;
        if (isHyperEdge) {
            return toRefine < rhs.toRefine;
        } else {
            return hanging < rhs.hanging;
        }
    }

    bool operator>(const NodeData &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const NodeData &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const NodeData &rhs) const {
        return !(*this < rhs);
    }
};


#endif //GALOIS_NODEDATA_H
