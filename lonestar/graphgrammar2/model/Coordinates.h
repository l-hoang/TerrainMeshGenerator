#ifndef GALOIS_COORDINATES_H
#define GALOIS_COORDINATES_H

class Coordinates {
private:
    double x;
    double y;
    double z;
public:
    Coordinates(double x, double y, double z) : x(x), y(y), z(z) {}

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
        return 'x' + std::to_string(x) + ", y" + std::to_string(y) + ", z" + std::to_string(z);
    }

    bool operator==(const Coordinates &rhs) const {
        return x == rhs.x &&
               y == rhs.y &&
               z == rhs.z;
    }

    bool operator!=(const Coordinates &rhs) const {
        return !(rhs == *this);
    }
};
#endif //GALOIS_COORDINATES_H
