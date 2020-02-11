#ifndef GALOIS_EDGEDATA_H
#define GALOIS_EDGEDATA_H

#include "Coordinates.h"
#include "NodeData.h"


class EdgeData {
    typedef galois::graphs::MorphGraph<NodeData, EdgeData, false> Graph;
    typedef Graph::GraphNode GNode;
private:
    bool border;
    double length;
    Coordinates middlePoint;
    const GNode *src;
    const GNode *dst;

public:
    EdgeData() : border(false), length(-1), middlePoint(), src(nullptr), dst(nullptr) {};

    EdgeData(bool border, double length, Coordinates middlePoint, GNode *src, GNode *dst) : border(border),
                                                                                            length(length),
                                                                                            middlePoint(middlePoint),
                                                                                            src(src), dst(dst) {}

    bool isBorder() const {
        return border;
    }

    void setBorder(bool isBorder) {
        EdgeData::border = isBorder;
    }

    double getLength() const {
        return length;
    }

    void setLength(double l) {
        EdgeData::length = l;
    }

    const Coordinates &getMiddlePoint() const {
        return middlePoint;
    }

    void setMiddlePoint(const Coordinates &coordinates) {
        EdgeData::middlePoint.setXYZ(coordinates.getX(), coordinates.getY(), coordinates.getZ());
    }

    void setMiddlePoint(const double x, const double y, const double z) {
        EdgeData::middlePoint.setXYZ(x, y, z);
    }

    const GNode *getSrc() const {
        return src;
    }

    void setSrc(const GNode *src) {
        EdgeData::src = src;
    }

    const GNode *getDst() const {
        return dst;
    }

    void setDst(const GNode *dst) {
        EdgeData::dst = dst;
    }
};


#endif //GALOIS_EDGEDATA_H
