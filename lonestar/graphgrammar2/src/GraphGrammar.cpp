#include <galois/graphs/Graph.h>
#include <Lonestar/BoilerPlate.h>
#include "model/EdgeData.h"
#include "model/NodeData.h"
#include "model/Graph.h"
#include "model/Map.h"
#include "conditions/TerrainConditionChecker.h"
#include "conditions/DummyConditionChecker.h"
#include "productions/Production1.h"
#include "productions/Production2.h"
#include "productions/Production3.h"
#include "productions/Production4.h"
#include "productions/Production5.h"
#include "productions/Production6.h"
#include "utils/MyGraphFormatWriter.h"
#include "utils/utils.h"
#include "utils/GraphGenerator.h"
#include "readers/SrtmReader.h"
#include "readers/AsciiReader.h"
#include "utils/Config.h"


static const char *name = "Mesh generator";
static const char *desc = "...";
static const char *url = "mesh_generator";

void afterStep(int i, Graph &graph);

bool basicCondition(const Graph &graph, GNode &node);

int main(int argc, char **argv) {
    Config config = Config{argc, argv};

    galois::SharedMemSys G;
    if (config.cores > 0) {
        galois::setActiveThreads(config.cores);
    }

    LonestarStart(argc, argv, name, desc, url);//---------
    Graph graph{};

    galois::reportPageAlloc("MeminfoPre1");
    // Tighter upper bound for pre-alloc, useful for machines with limited memory,
    // e.g., Intel MIC. May not be enough for deterministic execution
    constexpr size_t NODE_SIZE = sizeof(**graph.begin());

    galois::preAlloc(5 * galois::getActiveThreads() +
                     NODE_SIZE * 32 * graph.size() /
                     galois::runtime::pagePoolSize());

    galois::reportPageAlloc("MeminfoPre2");

//    AsciiReader reader;
//    Map *map = reader.read("data/test2.asc");
    galois::gDebug("Initial configuration set.");
    SrtmReader reader;
    Map *map = reader.read(config.W, config.N, config.E, config.S, config.dataDir.c_str());
    galois::gDebug("Terrain data read.");
//    GraphGenerator::generateSampleGraph(graph);
//    GraphGenerator::generateSampleGraphWithData(graph, *map, 0, map->getLength() - 1, map->getWidth() - 1, 0, config.version2D);
    GraphGenerator::generateSampleGraphWithDataWithConversionToUtm(graph, *map, config.W, config.N, config.E, config.S,
                                                                   config.version2D);
    galois::gDebug("Initial graph generated");

    ConnectivityManager connManager{graph};
//    DummyConditionChecker checker = DummyConditionChecker();
    TerrainConditionChecker checker = TerrainConditionChecker(config.tolerance, connManager, *map);
    Production1 production1{
            connManager}; //TODO: consider boost pointer containers, as they are believed to be better optimized
    Production2 production2{connManager};
    Production3 production3{connManager};
    Production4 production4{connManager};
    Production5 production5{connManager};
    Production6 production6{connManager};
    vector<Production *> productions = {&production1, &production2, &production3, &production4, &production5,
                                        &production6};
    galois::gDebug("Loop is being started...");
//    afterStep(0, graph);
    for (int j = 0; j < config.steps; j++) {
        galois::for_each(galois::iterate(graph.begin(), graph.end()), [&](GNode node, auto &ctx) {
            if (basicCondition(graph, node)) {
                checker.execute(node);
            }
        });
        galois::gDebug("Condition chceking in step ", j, " finished.");
        galois::StatTimer step(("step" + std::to_string(j)).c_str());
        step.start();
        galois::for_each(galois::iterate(graph.begin(), graph.end()), [&](GNode node, auto &ctx) {
            if (!basicCondition(graph, node)) {
                return;
            }
            ConnectivityManager connManager{graph};
            ProductionState pState(connManager, node, config.version2D,
                                   [&map](double x, double y) -> double { return map->get_height(x, y); });
            for (Production *production : productions) {
                if (production->execute(pState, ctx)) {
                    afterStep(j, graph);
                    return;
                }
            }
        }, galois::loopname(("step" + std::to_string(j)).c_str()));
        step.stop();
        galois::gDebug("Step ", j, " finished.");
    }
    galois::gDebug("All steps finished.");

    MyGraphFormatWriter::writeToFile(graph, config.output);
    galois::gDebug("Graph written to file ", config.output);
    if (config.display) {
        system((std::string("./display.sh ") + config.output).c_str());
    }

    delete map;
    return 0;
}

bool basicCondition(const Graph &graph, GNode &node) {
    return graph.containsNode(node, galois::MethodFlag::WRITE) && node->getData().isHyperEdge();
}

void afterStep(int i, Graph &graph) {
    auto path = std::string("out/step") + std::to_string((i - 1)) + ".mgf";
    MyGraphFormatWriter::writeToFile(graph, path);
//    system((std::string("./display.sh ") + path).c_str());
//    std::cout << std::endl;
}
