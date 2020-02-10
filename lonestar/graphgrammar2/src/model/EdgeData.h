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
    const GNode *vertex1;
    const GNode *vertex2;

public:
    EdgeData() : border(false), length(-1), middlePoint(), vertex1(nullptr), vertex2(nullptr) {};

    EdgeData(bool border, double length, Coordinates middlePoint, GNode *vertex1, GNode *vertex2) : border(border),
                                                                                                    length(length),
                                                                                                    middlePoint(
                                                                                                            middlePoint),
                                                                                                    vertex1(vertex1),
                                                                                                    vertex2(vertex2) {}

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

    const GNode *getVertex1() const {
        return vertex1;
    }

    void setVertex1(const GNode *vertex1) {
        EdgeData::vertex1 = vertex1;
    }

    const GNode *getVertex2() const {
        return vertex2;
    }

    void setVertex2(const GNode *vertex2) {
        EdgeData::vertex2 = vertex2;
    }
};


#endif //GALOIS_EDGEDATA_H
