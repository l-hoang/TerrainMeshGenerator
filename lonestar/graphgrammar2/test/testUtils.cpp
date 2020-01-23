
int countHEdges(Graph &graph) {
    int counter = 0;
    for(auto n : graph) {
        if (n->getData().isHyperEdge()) {
            ++counter;
        }
    }
    return counter;
}

int countVertices(Graph &graph) {
    int counter = 0;
    for(auto n : graph) {
        if (!(n->getData().isHyperEdge())) {
            ++counter;
        }
    }
    return counter;
}
