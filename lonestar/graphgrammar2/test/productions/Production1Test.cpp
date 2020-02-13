#include "../catch.hpp"
#include "../../src/productions/Production1.h"
#include "../../src/model/Graph.h"
#include "../../src/model/Coordinates.h"
#include "../../src/model/NodeData.h"
#include "../testUtils.cpp"



TEST_CASE( "Production1 Test" ) {
    galois::SharedMemSys G;
    Graph graph{};
    vector<GNode> nodes = generateSampleGraph(graph);
    nodes[5]->getData().setToRefine(true);
    galois::UserContext<GNode> ctx;
    ConnectivityManager connManager{graph};
    Production1 production{connManager};
    ProductionState pState(connManager, nodes[5], false, [](double x, double y){ return 0.;});
    production.execute(pState, ctx);

    REQUIRE(countHEdges(graph) == 3);
    REQUIRE(countVertices(graph) == 5);
}

