#include <galois/graphs/Graph.h>
#include <Lonestar/BoilerPlate.h>
#include "model/EdgeData.h"
#include "model/NodeData.h"
#include "model/Graph.h"
#include "productions/Production0.h"
#include "productions/Production1.h"
#include "productions/Production2.h"
#include "productions/Production3.h"
#include "productions/Production4.h"
#include "productions/Production5.h"
#include "productions/Production6.h"
#include "utils/MyGraphFormatWriter.h"
#include "utils/utils.h"


static const char *name = "Mesh generator";
static const char *desc = "...";
static const char *url = "mesh_generator";

void generateSampleGraph(Graph &graph);

void generateSampleGraph2(Graph &graph);

void generateSampleGraph3(Graph &graph);

void afterStep(int i, Graph &graph);

int main(int argc, char **argv) {
    galois::SharedMemSys G;
    LonestarStart(argc, argv, name, desc, url);//---------
    Graph graph{};
    generateSampleGraph3(graph);

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
    Production0 production0;
    Production1 production1{graph};
    Production2 production2{graph};
    Production3 production3{graph};
    Production4 production4{graph};
    Production5 production5{graph};
    Production6 production6{graph};
    int i = 0;
    afterStep(0, graph);
    for (int j = 0; j< 5; j++) {
        galois::for_each(galois::iterate(graph.begin(), graph.end()), [&](GNode node, auto &ctx) {
            if (!graph.containsNode(node, galois::MethodFlag::WRITE)) {
                return;
            }
            if (!node->getData().isHyperEdge()) {
                return;
            }
            if (production0.execute(node, ctx)) {
//                afterStep(i, graph);
                return;
            }

        });
        galois::for_each(galois::iterate(graph.begin(), graph.end()), [&](GNode node, auto &ctx) {
            //        if (i>40) {
            //            return;
            //        }
            if (!graph.containsNode(node, galois::MethodFlag::WRITE)) {
                return;
            }
            if (!node->getData().isHyperEdge()) {
                return;
            }
            ConnectivityManager connManager{graph};
            ProductionState pState(connManager, node);
            std::cout << i++ << ": ";
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

    return 0;
}

void afterStep(int i, Graph &graph) {
    auto path = std::string("out/step") + std::to_string(i - 1) + ".mgf";
    MyGraphFormatWriter::writeToFile(graph, path);
    system((std::string("./display.sh ") + path).c_str());
    std::cout << std::endl;
}

void generateSampleGraph(Graph &graph) {
    GNode node1, node2, node3, node4, hEdge1, hEdge2;
    node1 = graph.createNode(NodeData{false, Coordinates{0, 0, 0}, false});
    node2 = graph.createNode(NodeData{false, Coordinates{0, 1, 0}, false});
    node3 = graph.createNode(NodeData{false, Coordinates{1, 0, 0}, false});
    node4 = graph.createNode(NodeData{false, Coordinates{1, 1, 0}, false});
    hEdge1 = graph.createNode(NodeData{true, true, Coordinates{0.3,0.7,0}});
    hEdge2 = graph.createNode(NodeData{true, true, Coordinates{0.7,0.3,0}});

    graph.addNode(node1);
    graph.addNode(node2);
    graph.addNode(node3);
    graph.addNode(node4);
    graph.addNode(hEdge1);
    graph.addNode(hEdge2);

    graph.addEdge(node1, node2);
    graph.getEdgeData(graph.findEdge(node1, node2)).setBorder(true);
    graph.getEdgeData(graph.findEdge(node1, node2)).setMiddlePoint(Coordinates{0, 0.5, 0});
    graph.getEdgeData(graph.findEdge(node1, node2)).setLength(1);
    graph.addEdge(node2, node4);
    graph.getEdgeData(graph.findEdge(node2, node4)).setBorder(true);
    graph.getEdgeData(graph.findEdge(node2, node4)).setMiddlePoint(Coordinates{0.5, 1, 0});
    graph.getEdgeData(graph.findEdge(node2, node4)).setLength(1);
    graph.addEdge(node3, node4);
    graph.getEdgeData(graph.findEdge(node3, node4)).setBorder(true);
    graph.getEdgeData(graph.findEdge(node3, node4)).setMiddlePoint(Coordinates{1, 0.5, 0});
    graph.getEdgeData(graph.findEdge(node3, node4)).setLength(1);
    graph.addEdge(node1, node3);
    graph.getEdgeData(graph.findEdge(node1, node3)).setBorder(true);
    graph.getEdgeData(graph.findEdge(node1, node3)).setMiddlePoint(Coordinates{0.5, 0, 0});
    graph.getEdgeData(graph.findEdge(node1, node3)).setLength(1);
    graph.addEdge(node4, node1);
    graph.getEdgeData(graph.findEdge(node4, node1)).setBorder(false);
    graph.getEdgeData(graph.findEdge(node4, node1)).setMiddlePoint(Coordinates{0.5, 0.5, 0});
    graph.getEdgeData(graph.findEdge(node4, node1)).setLength(sqrt(2));

    graph.addEdge(hEdge1, node1);
    graph.getEdgeData(graph.findEdge(hEdge1, node1)).setBorder(false);
    graph.addEdge(hEdge1, node2);
    graph.getEdgeData(graph.findEdge(hEdge1, node2)).setBorder(false);
    graph.addEdge(hEdge1, node4);
    graph.getEdgeData(graph.findEdge(hEdge1, node4)).setBorder(false);

    graph.addEdge(hEdge2, node1);
    graph.getEdgeData(graph.findEdge(hEdge2, node1)).setBorder(false);
    graph.addEdge(hEdge2, node4);
    graph.getEdgeData(graph.findEdge(hEdge2, node4)).setBorder(false);
    graph.addEdge(hEdge2, node3);
    graph.getEdgeData(graph.findEdge(hEdge2, node3)).setBorder(false);

}

void generateSampleGraph3(Graph &graph) {
    GNode node1, node2, node3, node4, node5, node6, hEdge1, hEdge2, hEdge3, hEdge4;
    node1 = graph.createNode(NodeData{false, Coordinates{0, 0, 0}, false});
    node2 = graph.createNode(NodeData{false, Coordinates{0, 1, 0}, false});
    node3 = graph.createNode(NodeData{false, Coordinates{1, 0, 0}, false});
    node4 = graph.createNode(NodeData{false, Coordinates{1, 1, 0}, false});
    node5 = graph.createNode(NodeData{false, Coordinates{1.5, 0.5, 0}, false});
    node6 = graph.createNode(NodeData{false, Coordinates{0.5, -0.5, 0}, false});
    hEdge1 = graph.createNode(NodeData{true, true, Coordinates{0.3,0.7,0}});
    hEdge2 = graph.createNode(NodeData{true, true, Coordinates{0.7,0.3,0}});
    hEdge3 = graph.createNode(NodeData{true, true, Coordinates{1.17,0.5,0}});
    hEdge4 = graph.createNode(NodeData{true, true, Coordinates{0.5,-0.17,0}});

    graph.addNode(node1);
    graph.addNode(node2);
    graph.addNode(node3);
    graph.addNode(node4);
    graph.addNode(node5);
    graph.addNode(node6);
    graph.addNode(hEdge1);
    graph.addNode(hEdge2);
    graph.addNode(hEdge3);
    graph.addNode(hEdge4);

    graph.addEdge(node1, node2);
    graph.getEdgeData(graph.findEdge(node1, node2)).setBorder(true);
    graph.getEdgeData(graph.findEdge(node1, node2)).setMiddlePoint(Coordinates{0, 0.5, 0});
    graph.getEdgeData(graph.findEdge(node1, node2)).setLength(1);
    graph.addEdge(node2, node4);
    graph.getEdgeData(graph.findEdge(node2, node4)).setBorder(true);
    graph.getEdgeData(graph.findEdge(node2, node4)).setMiddlePoint(Coordinates{0.5, 1, 0});
    graph.getEdgeData(graph.findEdge(node2, node4)).setLength(1);
    graph.addEdge(node3, node4);
    graph.getEdgeData(graph.findEdge(node3, node4)).setBorder(false);
    graph.getEdgeData(graph.findEdge(node3, node4)).setMiddlePoint(Coordinates{1, 0.5, 0});
    graph.getEdgeData(graph.findEdge(node3, node4)).setLength(1);
    graph.addEdge(node1, node3);
    graph.getEdgeData(graph.findEdge(node1, node3)).setBorder(false);
    graph.getEdgeData(graph.findEdge(node1, node3)).setMiddlePoint(Coordinates{0.5, 0, 0});
    graph.getEdgeData(graph.findEdge(node1, node3)).setLength(1);
    graph.addEdge(node4, node1);
    graph.getEdgeData(graph.findEdge(node4, node1)).setBorder(false);
    graph.getEdgeData(graph.findEdge(node4, node1)).setMiddlePoint(Coordinates{0.5, 0.5, 0});
    graph.getEdgeData(graph.findEdge(node4, node1)).setLength(sqrt(2));
    graph.addEdge(node5, node4);
    graph.getEdgeData(graph.findEdge(node5, node4)).setBorder(true);
    graph.getEdgeData(graph.findEdge(node5, node4)).setMiddlePoint(Coordinates{1.25, 0.75, 0});
    graph.getEdgeData(graph.findEdge(node5, node4)).setLength(sqrt(2)/2.);
    graph.addEdge(node5, node3);
    graph.getEdgeData(graph.findEdge(node5, node3)).setBorder(true);
    graph.getEdgeData(graph.findEdge(node5, node3)).setMiddlePoint(Coordinates{1.25, 0.25, 0});
    graph.getEdgeData(graph.findEdge(node5, node3)).setLength(sqrt(2)/2.);
    graph.addEdge(node6, node1);
    graph.getEdgeData(graph.findEdge(node6, node1)).setBorder(true);
    graph.getEdgeData(graph.findEdge(node6, node1)).setMiddlePoint(Coordinates{0.25, -0.25, 0});
    graph.getEdgeData(graph.findEdge(node6, node1)).setLength(sqrt(2)/2.);
    graph.addEdge(node6, node3);
    graph.getEdgeData(graph.findEdge(node6, node3)).setBorder(true);
    graph.getEdgeData(graph.findEdge(node6, node3)).setMiddlePoint(Coordinates{0.75, -0.25, 0});
    graph.getEdgeData(graph.findEdge(node6, node3)).setLength(sqrt(2)/2.);

    graph.addEdge(hEdge1, node1);
    graph.getEdgeData(graph.findEdge(hEdge1, node1)).setBorder(false);
    graph.addEdge(hEdge1, node2);
    graph.getEdgeData(graph.findEdge(hEdge1, node2)).setBorder(false);
    graph.addEdge(hEdge1, node4);
    graph.getEdgeData(graph.findEdge(hEdge1, node4)).setBorder(false);

    graph.addEdge(hEdge2, node1);
    graph.getEdgeData(graph.findEdge(hEdge2, node1)).setBorder(false);
    graph.addEdge(hEdge2, node4);
    graph.getEdgeData(graph.findEdge(hEdge2, node4)).setBorder(false);
    graph.addEdge(hEdge2, node3);
    graph.getEdgeData(graph.findEdge(hEdge2, node3)).setBorder(false);

    graph.addEdge(hEdge3, node5);
    graph.getEdgeData(graph.findEdge(hEdge3, node5)).setBorder(false);
    graph.addEdge(hEdge3, node4);
    graph.getEdgeData(graph.findEdge(hEdge3, node4)).setBorder(false);
    graph.addEdge(hEdge3, node3);
    graph.getEdgeData(graph.findEdge(hEdge3, node3)).setBorder(false);

    graph.addEdge(hEdge4, node1);
    graph.getEdgeData(graph.findEdge(hEdge4, node1)).setBorder(false);
    graph.addEdge(hEdge4, node3);
    graph.getEdgeData(graph.findEdge(hEdge4, node3)).setBorder(false);
    graph.addEdge(hEdge4, node6);
    graph.getEdgeData(graph.findEdge(hEdge4, node6)).setBorder(false);

}


void generateSampleGraph2(Graph &graph) {
    GNode nodes[9];
    for (int i = 0; i < 9; ++i) {
        nodes[i] = graph.createNode(NodeData{false,
                                             Coordinates{static_cast<double>(i % 3), static_cast<double>(i / 3), 0},
                                             false});
        graph.addNode(nodes[i]);
    }
    GNode hEdges[8];
    for (auto &hEdge : hEdges) {
        hEdge = graph.createNode(NodeData{true, true});
        graph.addNode(hEdge);
    }


    graph.addEdge(nodes[0], nodes[1]);
    graph.getEdgeData(graph.findEdge(nodes[0], nodes[1])).setBorder(true);
    graph.getEdgeData(graph.findEdge(nodes[0], nodes[1])).setMiddlePoint(Coordinates{0.5, 0, 0});
    graph.getEdgeData(graph.findEdge(nodes[0], nodes[1])).setLength(1);
    graph.addEdge(nodes[1], nodes[2]);
    graph.getEdgeData(graph.findEdge(nodes[1], nodes[2])).setBorder(true);
    graph.getEdgeData(graph.findEdge(nodes[1], nodes[2])).setMiddlePoint(Coordinates{1.5, 0, 0});
    graph.getEdgeData(graph.findEdge(nodes[1], nodes[2])).setLength(1);
    graph.addEdge(nodes[0], nodes[3]);
    graph.getEdgeData(graph.findEdge(nodes[0], nodes[3])).setBorder(true);
    graph.getEdgeData(graph.findEdge(nodes[0], nodes[3])).setMiddlePoint(Coordinates{0, 0.5, 0});
    graph.getEdgeData(graph.findEdge(nodes[0], nodes[3])).setLength(1);
    graph.addEdge(nodes[1], nodes[4]);
    graph.getEdgeData(graph.findEdge(nodes[1], nodes[4])).setBorder(true);
    graph.getEdgeData(graph.findEdge(nodes[1], nodes[4])).setMiddlePoint(Coordinates{1, 0.5, 0});
    graph.getEdgeData(graph.findEdge(nodes[1], nodes[4])).setLength(1);
    graph.addEdge(nodes[2], nodes[5]);
    graph.getEdgeData(graph.findEdge(nodes[2], nodes[5])).setBorder(true);
    graph.getEdgeData(graph.findEdge(nodes[2], nodes[5])).setMiddlePoint(Coordinates{2, 0.5, 0});
    graph.getEdgeData(graph.findEdge(nodes[2], nodes[5])).setLength(1);
    graph.addEdge(nodes[3], nodes[4]);
    graph.getEdgeData(graph.findEdge(nodes[3], nodes[4])).setBorder(true);
    graph.getEdgeData(graph.findEdge(nodes[3], nodes[4])).setMiddlePoint(Coordinates{0.5, 1, 0});
    graph.getEdgeData(graph.findEdge(nodes[3], nodes[4])).setLength(1);
    graph.addEdge(nodes[4], nodes[5]);
    graph.getEdgeData(graph.findEdge(nodes[4], nodes[5])).setBorder(true);
    graph.getEdgeData(graph.findEdge(nodes[4], nodes[5])).setMiddlePoint(Coordinates{1.5, 1, 0});
    graph.getEdgeData(graph.findEdge(nodes[4], nodes[5])).setLength(1);

    graph.addEdge(nodes[3], nodes[6]);
    graph.getEdgeData(graph.findEdge(nodes[3], nodes[6])).setBorder(true);
    graph.getEdgeData(graph.findEdge(nodes[3], nodes[6])).setMiddlePoint(Coordinates{0, 1.5, 0});
    graph.getEdgeData(graph.findEdge(nodes[3], nodes[6])).setLength(1);
    graph.addEdge(nodes[4], nodes[7]);
    graph.getEdgeData(graph.findEdge(nodes[4], nodes[7])).setBorder(true);
    graph.getEdgeData(graph.findEdge(nodes[4], nodes[7])).setMiddlePoint(Coordinates{1, 1.5, 0});
    graph.getEdgeData(graph.findEdge(nodes[4], nodes[7])).setLength(1);
    graph.addEdge(nodes[5], nodes[8]);
    graph.getEdgeData(graph.findEdge(nodes[5], nodes[8])).setBorder(true);
    graph.getEdgeData(graph.findEdge(nodes[5], nodes[8])).setMiddlePoint(Coordinates{2, 1.5, 0});
    graph.getEdgeData(graph.findEdge(nodes[5], nodes[8])).setLength(1);
    graph.addEdge(nodes[6], nodes[7]);
    graph.getEdgeData(graph.findEdge(nodes[6], nodes[7])).setBorder(true);
    graph.getEdgeData(graph.findEdge(nodes[6], nodes[7])).setMiddlePoint(Coordinates{0.5, 2, 0});
    graph.getEdgeData(graph.findEdge(nodes[6], nodes[7])).setLength(1);
    graph.addEdge(nodes[7], nodes[8]);
    graph.getEdgeData(graph.findEdge(nodes[7], nodes[8])).setBorder(true);
    graph.getEdgeData(graph.findEdge(nodes[7], nodes[8])).setMiddlePoint(Coordinates{1.5, 2, 0});
    graph.getEdgeData(graph.findEdge(nodes[7], nodes[8])).setLength(1);

    for (int j = 0; j < 2; ++j) {
        graph.addEdge(nodes[j], nodes[j + 4]);
        graph.getEdgeData(graph.findEdge(nodes[j], nodes[j + 4])).setBorder(true);
        graph.getEdgeData(graph.findEdge(nodes[j], nodes[j + 4])).setMiddlePoint(
                Coordinates{0.5 + j / 2, 0.5 + j % 2, 0});
        graph.getEdgeData(graph.findEdge(nodes[j], nodes[j + 4])).setLength(sqrt(2));
        graph.addEdge(nodes[j + 3], nodes[j + 7]);
        graph.getEdgeData(graph.findEdge(nodes[j + 3], nodes[j + 7])).setBorder(true);
        graph.getEdgeData(graph.findEdge(nodes[j + 3], nodes[j + 7])).setMiddlePoint(
                Coordinates{0.5 + j / 2, 0.5 + j % 2, 0});
        graph.getEdgeData(graph.findEdge(nodes[j + 3], nodes[j + 7])).setLength(sqrt(2));
    }

    for (int k = 0; k < 4; ++k) {
        graph.addEdge(hEdges[k], nodes[(k + k/2) % 9]);
        graph.addEdge(hEdges[k], nodes[(k + 1 + k/2) % 9]);
        graph.addEdge(hEdges[k], nodes[(k + 4 + k/2) % 9]);

        graph.addEdge(hEdges[(k + 4) % 8], nodes[(k + k/2) % 9]);
        graph.addEdge(hEdges[(k + 4) % 8], nodes[(k + 3 + k/2) % 9]);
        graph.addEdge(hEdges[(k + 4) % 8], nodes[(k + 4 + k/2) % 9]);
    }


}
