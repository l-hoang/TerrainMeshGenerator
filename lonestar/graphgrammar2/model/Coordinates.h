#ifndef GALOIS_COORDINATES_H
#define GALOIS_COORDINATES_H

class Coordinates {
private:
    constexpr static const double EPSILON = 1e-8;

    double x;
    double y;
    double z;
public:
    Coordinates(double x, double y, double z) : x(x), y(y), z(z) {}

    Coordinates() {}

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

    std::string toString() const {
        return  std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z);
    }

    bool operator==(const Coordinates &rhs) const {
        return abs(x - rhs.x) < EPSILON &&
               abs(y - rhs.y) < EPSILON &&
               abs(z - rhs.z) < EPSILON;
    }

    bool operator!=(const Coordinates &rhs) const {
        return !(rhs == *this);
    }
};
#endif //GALOIS_COORDINATES_H
