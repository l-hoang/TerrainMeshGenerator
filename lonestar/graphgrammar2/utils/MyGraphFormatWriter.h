#ifndef GALOIS_MYGRAPHFORMATWRITER_H
#define GALOIS_MYGRAPHFORMATWRITER_H


#include <fstream>

class MyGraphFormatWriter {
private:
    void writeToMyGraphFormat(const std::vector<std::pair<int, Coordinates>> &nodes, const std::vector<std::pair<int, int>> &edges, std::string path) {
        std::ofstream file;
        file.open("graph.mgf");
        for (auto node : nodes) {
            file << "N,n" << node.first << "," << node.second.getX() << "," << node.second.getY() << "," << node.second.getZ()
                 << std::endl;
        }
        int  i = 0;
        for (auto edge : edges) {
            file << "E,e" << i++ << ",n" << edge.first << ",n" << edge.second << std::endl;
        }
    }

    void addEdge(std::vector<std::pair<int, int>> &edges, int firstNodeId, int secondNodeId) {
        if (!findEdge(firstNodeId, secondNodeId, edges).is_initialized()) {
            edges.emplace_back(std::pair<int, int>(firstNodeId, secondNodeId));
        }
    }

    galois::optional<std::pair<int, int>> findEdge(int first, int second, std::vector<std::pair<int, int>> &edges) {
        for (auto edge : edges) {
            if ((edge.first == first && edge.second == second) || (edge.first == second && edge.second == first)) {
                return galois::optional<std::pair<int, int>>(edge);
            }
        }
        return galois::optional<std::pair<int, int>>();
    }

    int getNodeId(std::vector<std::pair<int, Coordinates>> &nodes, int &nodesIter, NodeData &data) {
        for (auto pair : nodes) {
            if (pair.second == data.getCoords()) {
                return pair.first;
            }
        }
        nodes.emplace_back(std::pair<int, Coordinates>(nodesIter, data.getCoords()));
        return nodesIter++;
    }


public:
    void writeToFile(Graph &graph, std::string path) {
        std::vector<std::pair<int, Coordinates>> nodes;
        std::vector<std::pair<int, int>> edges;
        std::vector<int> hEdges;
        int nodesIter = 0;
        for (auto node: graph) {
            NodeData &data = graph.getData(node);
            if (!data.isHyperEdge) {
                int firstNodeId = getNodeId(nodes, nodesIter, data);
                for (const EdgeIterator &e : graph.edges(node)) {
                    NodeData dstNode = graph.getData(graph.getEdgeDst(e));
                    if (!dstNode.isHyperEdge) {
                        int secondNodeId = getNodeId(nodes, nodesIter, dstNode);
                        addEdge(edges, firstNodeId, secondNodeId);
                    }
                }
            }
        }
        writeToMyGraphFormat(nodes, edges, path);
    }
};


#endif //GALOIS_MYGRAPHFORMATWRITER_H
