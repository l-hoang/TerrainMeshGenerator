#ifndef GALOIS_CONNECTIVITYMANAGER_H
#define GALOIS_CONNECTIVITYMANAGER_H


class ConnectivityManager {
private:
    Graph &graph;
public:
    ConnectivityManager(Graph &graph) : graph(graph) {}

    std::vector<GNode> getNeighbours(GNode node) {
        std::vector<GNode> vertices;
        for (Graph::edge_iterator ii = graph.edge_begin(node), ee = graph.edge_end(node); ii != ee; ++ii) {
            vertices.push_back(graph.getEdgeDst(ii));
        }
        return vertices;
    }

    GNode getEdgeSrc(const EdgeIterator &edge, GNode neighbour) {
        for (GNode candidate : getNeighbours(neighbour)) {
            for (const EdgeIterator &maybeTheEdge : graph.edges(candidate)) {
                if (maybeTheEdge == edge) {
                    return candidate;
                }
            }
        }
        std::cerr << "Cannot find EdgeSrc." << std::endl;
        return nullptr;
    }

    std::vector<optional<EdgeIterator>> getEdges(const GNode node) {
        std::vector<optional<EdgeIterator>> edges;
        auto vertices = getNeighbours(node);
        for (int i = 0; i<3;i++) {
            auto vertex = vertices[i];
            EdgeIterator edge = graph.findEdge(vertices[(i+1)%3], vertex);
            if (edge.base() == edge.end()) {
                edges.emplace_back(optional<EdgeIterator>());
            } else {
                edges.emplace_back(edge);
            }
        }
        return edges;
    }

    EdgeIterator getLongestEdge(GNode node, std::vector<optional<EdgeIterator>> edges) {

        return std::max_element(edges.begin(), edges.end(), [&](optional<EdgeIterator> &a, optional<EdgeIterator> &b) {
            return graph.getEdgeData(a.get()).getLength() < graph.getEdgeData(b.get()).getLength();
        })->get();
    }

    bool hasHanging(GNode node, const std::vector<optional<EdgeIterator>> &edges) {
        for (const optional<EdgeIterator> &edge : edges) {
            if (!edge.is_initialized()) {
                return true;
            }
        }
        return false;
    }

    std::pair<GNode, EdgeIterator> findSrc(EdgeData edge) {
        std::pair<GNode, EdgeIterator> result;
        for (auto n : graph) {
            for (const auto &e : graph.edges(n)) {
                if (graph.getEdgeData(e).getMiddlePoint() == edge.getMiddlePoint()) {
                    result.first = n;
                    result.second = e;
                }
            }
        }
        return result;
    }
};


#endif //GALOIS_CONNECTIVITYMANAGER_H
