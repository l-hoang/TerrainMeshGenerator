#ifndef GALOIS_NODEDATA_H
#define GALOIS_NODEDATA_H

#include "Coordinates.h"

class NodeData {
private:
  bool hyperEdge;
  Coordinates coords;
  bool toRefine;
  bool hanging;

public:
  NodeData(bool isHyperEdge, const Coordinates& coords, bool hanging)
      : hyperEdge(isHyperEdge), coords(), toRefine(false), hanging(hanging) {
    setCoords(coords);
  }

  NodeData(bool isHyperEdge, bool toRefine)
      : hyperEdge(isHyperEdge), coords(), toRefine(toRefine), hanging(false) {}

  NodeData(bool isHyperEdge, bool toRefine, Coordinates coords)
      : hyperEdge(isHyperEdge), coords(), toRefine(toRefine), hanging(false) {
    setCoords(coords);
  }

  Coordinates getCoords() const { return coords; }

  void setCoords(const Coordinates& coordinates) {
    NodeData::coords.setXYZ(coordinates.getX(), coordinates.getY(),
                            coordinates.getZ());
  }

  void setCoords(const double x, const double y, const double z) {
    NodeData::coords.setXYZ(x, y, z);
  }

  bool isToRefine() const { return toRefine; }

  void setToRefine(bool refine) { NodeData::toRefine = refine; }

  bool isHanging() const { return hanging; }

  void setHanging(bool hangingNode) { NodeData::hanging = hangingNode; }

  bool isHyperEdge() const { return hyperEdge; }

  bool operator==(const NodeData& rhs) const {
    return hyperEdge == rhs.hyperEdge && coords == rhs.coords &&
           (hyperEdge ? toRefine == rhs.toRefine : hanging == rhs.hanging);
  }

  bool operator!=(const NodeData& rhs) const { return !(rhs == *this); }

  bool operator<(const NodeData& rhs) const {
    if (hyperEdge < rhs.hyperEdge)
      return true;
    if (rhs.hyperEdge < hyperEdge)
      return false;
    if (coords < rhs.coords)
      return true;
    if (rhs.coords < coords)
      return false;
    if (hyperEdge) {
      return toRefine < rhs.toRefine;
    } else {
      return hanging < rhs.hanging;
    }
  }

  bool operator>(const NodeData& rhs) const { return rhs < *this; }

  bool operator<=(const NodeData& rhs) const { return !(rhs < *this); }

  bool operator>=(const NodeData& rhs) const { return !(*this < rhs); }
};

#endif // GALOIS_NODEDATA_H
