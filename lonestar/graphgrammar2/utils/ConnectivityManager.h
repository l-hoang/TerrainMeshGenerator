#ifndef GALOIS_CONNECTIVITYMANAGER_H
#define GALOIS_CONNECTIVITYMANAGER_H


class ConnectivityManager {
private:
    Graph &graph;
public:
    ConnectivityManager(Graph &graph) : graph(graph) {}

    std::vector<GNode> getNeighbours(GNode node) const {
        std::vector<GNode> vertices;
        for (Graph::edge_iterator ii = graph.edge_begin(node), ee = graph.edge_end(node); ii != ee; ++ii) {
            vertices.push_back(graph.getEdgeDst(ii));
        }
        return vertices;
    }

    GNode getNeighbourN(GNode node, int n) const {
//        std::vector<GNode> vertices;
        int i = 0;
        Graph::edge_iterator ii = graph.edge_begin(node);
        for (Graph::edge_iterator  ee = graph.edge_end(node); ii != ee || i < n; ++ii) {
//            vertices.push_back(graph.getEdgeDst(ii));
        }
        return GNode(ii->first());
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

    std::vector<optional<EdgeIterator>> getTriangleEdges(std::vector<GNode> vertices) {
        std::vector<optional<EdgeIterator>> edges;
        for (int i = 0; i<3;i++) {
            edges.emplace_back(getEdge(vertices[i], vertices[(i + 1) % 3]));
        }
        return edges;
    }

    optional<EdgeIterator> getEdge(const GNode &v1, const GNode &v2) const {
        EdgeIterator edge = graph.findEdge(v1, v2);
        galois::optional<EdgeIterator> maybeEdge = getOptionalEdge(edge);
        return maybeEdge;
    }

    optional<EdgeIterator> getOptionalEdge(const EdgeIterator &edge) const {
        if (edge.base() == edge.end()) {
            return galois::optional<EdgeIterator>();
        } else {
            return galois::optional<EdgeIterator>(edge);
        }
    }

    EdgeIterator getLongestEdge(GNode node, std::vector<optional<EdgeIterator>> edges) {

        return std::max_element(edges.begin(), edges.end(), [&](optional<EdgeIterator> &a, optional<EdgeIterator> &b) {
            return graph.getEdgeData(a.get()).getLength() < graph.getEdgeData(b.get()).getLength();
        })->get();
    }

    bool hasBrokenEdge(const std::vector<optional<EdgeIterator>> &edges) const {
        return countBrokenEdges(edges) > 0;
    }

    bool countBrokenEdges(const std::vector<optional<EdgeIterator>> &edges) const {
        int counter = 0;
        for (const optional<EdgeIterator> &edge : edges) {
            if (!edge.is_initialized()) {
                counter++;
            }
        }
        return counter;
    }

    std::pair<GNode, EdgeIterator> findSrc(EdgeData edge) const {
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

    optional<GNode> findNodeBetween(GNode node1, GNode node2, Coordinates notThere) {
         std::vector<GNode> neighbours1 = getNeighbours(node1);
         std::vector<GNode> neighbours2 = getNeighbours(node2);
        for (GNode &iNode : neighbours1) {
            auto iNodeData = graph.getData(iNode);
            for (GNode &jNode : neighbours2) {
                if (iNodeData.getCoords() == graph.getData(jNode).getCoords() && iNodeData.getCoords() != notThere) {
                    return optional<GNode>(iNode);
                }
            }
        }
        return optional<GNode>();
    }

    GNode createNode(NodeData &nodeData, galois::UserContext<GNode> &ctx) const {
        auto node = graph.createNode(nodeData);
        graph.addNode(node);
        ctx.push(node);
        return node;
    }

    Graph &getGraph() const {
        return graph;
    }
};


#endif //GALOIS_CONNECTIVITYMANAGER_H
