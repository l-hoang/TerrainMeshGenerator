#ifndef GALOIS_UTILS_H
#define GALOIS_UTILS_H

bool equals(double a, double b) {
    return fabs(a - b) < EPS;
}

bool greater(double a, double b) {
    return a - b >= EPS;
}

bool less(double a, double b) {
    return a - b <= -EPS;
}

#endif //GALOIS_UTILS_H
