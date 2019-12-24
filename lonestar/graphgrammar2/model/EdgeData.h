#ifndef GALOIS_EDGEDATA_H
#define GALOIS_EDGEDATA_H

#include "Coordinates.h"

class EdgeData {
private:
    bool border;
    double length;
    Coordinates middlePoint;

public:
    EdgeData() : border(false), length(-1), middlePoint() {};

    EdgeData(bool border, double length, Coordinates middlePoint) : border(border), length(length),
                                                                    middlePoint(middlePoint) {}

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
};


#endif //GALOIS_EDGEDATA_H
