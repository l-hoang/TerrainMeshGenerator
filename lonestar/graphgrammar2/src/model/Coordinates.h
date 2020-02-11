#ifndef GALOIS_COORDINATES_H
#define GALOIS_COORDINATES_H

#include <ostream>
#include "../utils/utils.h"
#include "Map.h"

class Coordinates {
private:
    double x;
    double y;
    double z;
public:
    Coordinates() = default;

    Coordinates(double x, double y) : x(x), y(y), z(0.) {}

    Coordinates(double x, double y, double z) : x(x), y(y), z(z) {}

    Coordinates(double x, double y, Map &map) : x(x), y(y), z(map.get_height(x, y)) {}

    Coordinates(std::pair<double, double> coords, Map &map) : x(coords.first), y(coords.second),
                                                              z(map.get_height(coords.first, coords.second)) {}

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

    double dist(const Coordinates &rhs, bool version2D) {
        if (version2D) {
            return dist2D(rhs);
        } else {
            return dist3D(rhs);
        }
    }

    double dist3D(const Coordinates &rhs) const {
        return sqrt(pow(x - rhs.x, 2) + pow(y - rhs.y, 2) + pow(z - rhs.z, 2));
    }

    double dist2D(const Coordinates &rhs) const {
        return sqrt(pow(x - rhs.x, 2) + pow(y - rhs.y, 2));
    }

    bool isXYequal(const Coordinates &rhs) {
        return equals(x, rhs.x) &&
               equals(y, rhs.y);
    }

    std::string toString() const {
        return std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z);
    }

    Coordinates operator+(const Coordinates &rhs) const {
        return Coordinates{x + rhs.x, y + rhs.y, z + rhs.z};
    }

    Coordinates operator-(const Coordinates &rhs) const {
        return Coordinates{x - rhs.x, y - rhs.y, z - rhs.z};
    }

    Coordinates operator*(double rhs) const {
        return Coordinates{x * rhs, y * rhs, z * rhs};
    }

    Coordinates operator/(double rhs) const {
        return Coordinates{x / rhs, y / rhs, z / rhs};
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
