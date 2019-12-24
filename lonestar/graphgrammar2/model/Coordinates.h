#ifndef GALOIS_COORDINATES_H
#define GALOIS_COORDINATES_H

#include <ostream>
#include "../utils/utils.h"

class Coordinates {
private:

    double x;
    double y;
    double z;
public:
    Coordinates(double x, double y, double z) : x(x), y(y), z(z) {}

    Coordinates() = default;

    double getX() const {
        return x;
    }

    void setX(double x) {
        Coordinates::x = x;
    }

    double getY() const {
        return y;
    }

    void setY(double y) {
        Coordinates::y = y;
    }

    double getZ() const {
        return z;
    }

    void setZ(double z) {
        Coordinates::z = z;
    }

    void setXYZ(double x, double y, double z) {
        Coordinates::x = x;
        Coordinates::y = y;
        Coordinates::z = z;
    }

    std::string toString() const {
        return std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z);
    }

    Coordinates operator+(const Coordinates &coords) const {
        return Coordinates{x + coords.x, y + coords.y, z + coords.z};
    }

    Coordinates operator/(double div) const {
        return Coordinates{x / div, y / div, z / div};
    }

    bool operator==(const Coordinates &rhs) const {
        return equals(x, rhs.x) &&
               equals(y, rhs.y) &&
               equals(z, rhs.z);
    }

    bool operator!=(const Coordinates &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const Coordinates &rhs) const {
        if (less(x, rhs.x))
            return true;
        if (less(rhs.x, x))
            return false;
        if (less(y, rhs.y))
            return true;
        if (less(rhs.y, y))
            return false;
        return less(z, rhs.z);
    }

    bool operator>(const Coordinates &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Coordinates &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const Coordinates &rhs) const {
        return !(*this < rhs);
    }
};

#endif //GALOIS_COORDINATES_H
