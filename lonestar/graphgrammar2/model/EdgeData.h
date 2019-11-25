#ifndef GALOIS_EDGEDATA_H
#define GALOIS_EDGEDATA_H

class EdgeData {
private:
    bool border;
    double length;

public:
    EdgeData() {}

    EdgeData(bool border, double length) : border(border), length(length) {}

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
};



#endif //GALOIS_EDGEDATA_H
