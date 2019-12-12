#ifndef GALOIS_EDGEDATA_H
#define GALOIS_EDGEDATA_H

#include "Coordinates.h"

class EdgeData {
private:
    bool border;
    double length;
    Coordinates middlePoint;

public:
    EdgeData() {}

    EdgeData(bool border, double length, Coordinates middlePoint) : border(border), length(length),
                                                                    middlePoint(middlePoint) {}

    void init(bool border) {
        setBorder(border);
    }

    bool isBorder() const {
        return border;
    }

    void setBorder(bool border) {
        EdgeData::border = border;
    }

    double getLength() const {
        return length;
    }

    void setLength(double length) {
        EdgeData::length = length;
    }

    const Coordinates &getMiddlePoint() const {
        return middlePoint;
    }

    void setMiddlePoint(const Coordinates &middlePoint) {
        EdgeData::middlePoint = middlePoint;
    }
};


#endif //GALOIS_EDGEDATA_H
