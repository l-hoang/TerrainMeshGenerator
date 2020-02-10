#include "../catch.hpp"
#include "../../src/productions/Production1.h"
#include "../../src/model/Graph.h"
#include "../../src/model/Coordinates.h"
#include "../../src/model/NodeData.h"
#include "../testUtils.cpp"


vector<GNode> generateSampleGraph(Graph &graph) {
    vector<GNode> nodes;
    ConnectivityManager connManager{graph};
    nodes.push_back(connManager.createNode(NodeData{false, Coordinates{0, 0, 0}, false}));
    nodes.push_back(connManager.createNode(NodeData{false, Coordinates{0, 1, 0}, false}));
    nodes.push_back(connManager.createNode(NodeData{false, Coordinates{1, 0, 0}, false}));
    nodes.push_back(connManager.createNode(NodeData{false, Coordinates{1, 1, 0}, false}));

    connManager.createEdge(nodes[0], nodes[1], true, Coordinates{0, 0.5, 0}, 1);
    connManager.createEdge(nodes[1], nodes[3], true, Coordinates{0.5, 1, 0}, 1);
    connManager.createEdge(nodes[2], nodes[3], true, Coordinates{1, 0.5, 0}, 1);
    connManager.createEdge(nodes[0], nodes[2], true, Coordinates{0.5, 0, 0}, 1);
    connManager.createEdge(nodes[3], nodes[0], false, Coordinates{0.5, 0.5, 0}, sqrt(2));

    nodes.push_back(connManager.createInterior(nodes[0], nodes[1], nodes[3]));
    nodes.push_back(connManager.createInterior(nodes[0], nodes[3], nodes[2]));
    return nodes;
}


TEST_CASE( "Production1 Test" ) {
    galois::SharedMemSys G;
    Graph graph{};
    vector<GNode> nodes = generateSampleGraph(graph);
    nodes[5]->getData().setToRefine(true);
    galois::UserContext<GNode> ctx;
    ConnectivityManager connManager{graph};
    Production1 production{connManager};
    ProductionState pState(connManager, nodes[5]);
    production.execute(pState, ctx);

    REQUIRE(countHEdges(graph) == 3);
    REQUIRE(countVertices(graph) == 5);
}

