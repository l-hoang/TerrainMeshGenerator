#include <galois/graphs/Graph.h>
#include <Lonestar/BoilerPlate.h>
#include "model/EdgeData.h"
#include "model/NodeData.h"
#include "model/Graph.h"
#include "model/Map.h"
#include "productions/Production0.h"
#include "productions/TerrainConditionChecker.h"
#include "productions/Production1.h"
#include "productions/Production2.h"
#include "productions/Production3.h"
#include "productions/Production4.h"
#include "productions/Production5.h"
#include "productions/Production6.h"
#include "utils/MyGraphFormatWriter.h"
#include "utils/utils.h"
#include "utils/GraphGenerator.h"
#include "Readers/SrtmReader.h"


static const char *name = "Mesh generator";
static const char *desc = "...";
static const char *url = "mesh_generator";

void afterStep(int i, Graph &graph);

int main(int argc, char **argv) {
    galois::SharedMemSys G;
    LonestarStart(argc, argv, name, desc, url);//---------
    Graph graph{};
    GraphGenerator::generateSampleGraph(graph);

    galois::reportPageAlloc("MeminfoPre1");
    // Tighter upper bound for pre-alloc, useful for machines with limited memory,
    // e.g., Intel MIC. May not be enough for deterministic execution
    constexpr size_t NODE_SIZE = sizeof(**graph.begin());

    galois::preAlloc(5 * galois::getActiveThreads() +
                     NODE_SIZE * 32 * graph.size() /
                     galois::runtime::pagePoolSize());

    galois::reportPageAlloc("MeminfoPre2");


    MyGraphFormatWriter::writeToFile(graph, "out/graph.mgf");
    system("./display.sh out/graph.mgf");

    SrtmReader reader;
    Map * map = reader.read_SRTM(19.5, 50.5, 19.7, 50.3, "data");

    ConnectivityManager connManager{graph};
    TerrainConditionChecker checker = TerrainConditionChecker(*map);
    Production1 production1{connManager};
    Production2 production2{connManager};
    Production3 production3{connManager};
    Production4 production4{connManager};
    Production5 production5{connManager};
    Production6 production6{connManager};
    int i = 0;
//    afterStep(0, graph);
    for (int j = 0; j< 5; j++) {
        galois::for_each(galois::iterate(graph.begin(), graph.end()), [&](GNode node, auto &ctx) {
            if (!graph.containsNode(node, galois::MethodFlag::WRITE)) {
                return;
            }
            if (!node->getData().isHyperEdge()) {
                return;
            }
            if (checker.execute(node,100., connManager)) {
//                afterStep(i, graph);
                return;
            }

        });
        galois::for_each(galois::iterate(graph.begin(), graph.end()), [&](GNode node, auto &ctx) {
            if (!graph.containsNode(node, galois::MethodFlag::WRITE)) {
                return;
            }
            if (!node->getData().isHyperEdge()) {
                return;
            }
            ConnectivityManager connManager{graph};
            ProductionState pState(connManager, node);
//            std::cout << i++ << ": ";
            if (production1.execute(pState, ctx)) {
                afterStep(i, graph);
                return;
            }
            if (production2.execute(pState, ctx)) {
                afterStep(i, graph);
                return;
            }
            if (production3.execute(pState, ctx)) {
                afterStep(i, graph);
                return;
            }
            if (production4.execute(pState, ctx)) {
                afterStep(i, graph);
                return;
            }
            if (production5.execute(pState, ctx)) {
                afterStep(i, graph);
                return;
            }
            if (production6.execute(pState, ctx)) {
                afterStep(i, graph);
                return;
            }

        });

    }

    MyGraphFormatWriter::writeToFile(graph, "out/graph.mgf");
    system("./display.sh out/graph.mgf");

    delete map;
    return 0;
}

void afterStep(int i, Graph &graph) {
    auto path = std::string("out/step") + std::to_string(i - 1) + ".mgf";
//    MyGraphFormatWriter::writeToFile(graph, path);
//    system((std::string("./display.sh ") + path).c_str());
//    std::cout << std::endl;
}
