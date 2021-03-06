#ifndef GALOIS_UTILS_H
#define GALOIS_UTILS_H

static const double EPS = 1e-4;

bool equals(double a, double b) { return fabs(a - b) < EPS; }

bool greater(double a, double b) { return a - b >= EPS; }

bool less(double a, double b) { return a - b <= -EPS; }

std::vector<int> indexesOfMaxElems(std::vector<double> elems) {
  std::vector<int> result;
  if (elems.empty()) {
    return result;
  }
  double currentMax = elems[0];
  result.push_back(0);
  for (unsigned long i = 1; i < elems.size(); ++i) {
    if (greater(elems[i], currentMax)) {
      result.clear();
      result.push_back(i);
      currentMax = elems[i];
    } else if (equals(elems[i], currentMax)) {
      result.push_back(i);
    }
  }
  return result;
}

#endif // GALOIS_UTILS_H
