#ifndef GALOIS_NODEDATA_H
#define GALOIS_NODEDATA_H

#include "Coordinates.h"


class NodeData {
private:
    Coordinates coords;
    bool toRefine;
public:
    bool isHyperEdge;

    NodeData(bool isHyperEdge, const Coordinates &coords) : coords(coords), isHyperEdge(isHyperEdge) {}

    NodeData(bool isHyperEdge, bool toRefine) : toRefine(toRefine), isHyperEdge(isHyperEdge), coords() {}

//    std::vector<GNode> getVertices(GNode thisNode, Graph &graph) {
//        std::vector<GNode> vertices;
//        for (Graph::edge_iterator ii = graph.edge_begin(thisNode), ee = graph.edge_end(thisNode); ii != ee; ++ii) {
//            vertices.push_back(graph.getEdgeDst(ii));
//        }
//        return vertices;
//    }
//
//    std::vector<optional<EdgeIterator>> getEdges(const GNode thisNode, Graph &graph) {
//        std::vector<optional<EdgeIterator>> edges;
//        auto vertices = getVertices(thisNode, graph);
//        for (auto vertex : vertices) {
//            EdgeIterator edge = graph.findEdge(thisNode, vertex);
//            if (edge.base() == edge.end()) {
//                edges.emplace_back(optional<EdgeIterator>());
//            } else {
//                edges.emplace_back(edge);
//            }
//        }
//        return edges;
//    }
//
//    EdgeIterator getLongestEdge(GNode thisNode, std::vector<optional<EdgeIterator>>& edges, Graph &graph) {
//        return std::max_element(edges.begin(), edges.end(), [&graph](optional<EdgeIterator> &a, optional<EdgeIterator> &b) {
//            return graph.getEdgeData(a.get()).getLength() < graph.getEdgeData(b.get()).getLength();
//        })->get();
//    }
//
//    bool hasHanging(GNode thisNode, const std::vector<optional<EdgeIterator>>& edges, Graph &graph) {
//        for(const optional<EdgeIterator>& edge : edges) {
//            if (!edge.is_initialized()) {
//                return true;
//            }
//        }
//        return false;
//    }

    const Coordinates getCoords() const {
        return coords;
    }

    void setCoords(const Coordinates &coords) {
        NodeData::coords = coords;
    }

    bool isToRefine() const {
        return toRefine;
    }

    void setToRefine(bool toRefine) {
        NodeData::toRefine = toRefine;
    }
};


#endif //GALOIS_NODEDATA_H
