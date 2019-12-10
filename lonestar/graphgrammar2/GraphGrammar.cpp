#include <galois/graphs/Graph.h>
#include <Lonestar/BoilerPlate.h>
#include "model/EdgeData.h"
#include "model/HyperEdge.h"
#include "model/SingleNode.h"
#include "model/Graph.h"
#include "productions/Production.h"
#include "productions/Production1.h"
#include "utils/MyGraphFormatWriter.h"
//#include "GraphManager.h"



static const char *name = "Mesh generator";
static const char *desc = "...";
static const char *url = "mesh_generator";

void generateSampleGraph(Graph &graph);

int main(int argc, char **argv) {
    galois::SharedMemSys G;
    std::cout << "Hello1\n\n";
    LonestarStart(argc, argv, name, desc, url);//---------
    Graph graph{};
    generateSampleGraph(graph);
    std::cout << "Hello";

    galois::reportPageAlloc("MeminfoPre1");
    // Tighter upper bound for pre-alloc, useful for machines with limited memory,
    // e.g., Intel MIC. May not be enough for deterministic execution
    constexpr size_t NODE_SIZE = sizeof(**graph.begin());
    std::cout << galois::getActiveThreads() << std::endl;
    std::cout << graph.size() << std::endl;
    std::cout << galois::runtime::pagePoolSize() << std::endl;

    galois::preAlloc(5 * galois::getActiveThreads() +
                     NODE_SIZE * 32 * graph.size() /
                     galois::runtime::pagePoolSize());

    galois::reportPageAlloc("MeminfoPre2");

    MyGraphFormatWriter writer;
    writer.writeToFile(graph, "graph.mgf");

//    Production1 production1;

//    galois::do_all(galois::iterate(graph.begin(), graph.end()), [&](GNode node) {
//        if (production1.isPossible(node, graph)) {
//            production1.execute(node, graph);
//        }
//    });


    return 0;
}

void generateSampleGraph(Graph &graph) {
    GNode node1, node2, node3, node4, hEdge1, hEdge2;
    node1 = graph.createNode(NodeData{false, Coordinates{0, 0, 0}});
    node2 = graph.createNode(NodeData{false, Coordinates{0, 1, 0}});
    node3 = graph.createNode(NodeData{false, Coordinates{1, 0, 0}});
    node4 = graph.createNode(NodeData{false, Coordinates{1, 1, 0}});
    hEdge1 = graph.createNode(NodeData{true, true});
    hEdge2 = graph.createNode(NodeData{true, true});

    graph.addNode(node1);
    graph.addNode(node2);
    graph.addNode(node3);
    graph.addNode(node4);
    graph.addNode(hEdge1);
    graph.addNode(hEdge2);

    graph.addEdge(node1, node2);
    graph.getEdgeData(graph.findEdge(node1, node2)).setBorder(true);
    graph.addEdge(node2, node4);
    graph.getEdgeData(graph.findEdge(node2, node4)).setBorder(true);
    graph.addEdge(node3, node4);
    graph.getEdgeData(graph.findEdge(node3, node4)).setBorder(true);
    graph.addEdge(node4, node1);
    graph.getEdgeData(graph.findEdge(node4, node1)).setBorder(true);
    graph.addEdge(node1, node3);
    graph.getEdgeData(graph.findEdge(node1, node3)).setBorder(false);

    graph.addEdge(hEdge1, node1);
    graph.getEdgeData(graph.findEdge(hEdge1, node1)).setBorder(false);
    graph.addEdge(hEdge1, node2);
    graph.getEdgeData(graph.findEdge(hEdge1, node2)).setBorder(false);
    graph.addEdge(hEdge1, node3);
    graph.getEdgeData(graph.findEdge(hEdge1, node3)).setBorder(false);

    graph.addEdge(hEdge2, node1);
    graph.getEdgeData(graph.findEdge(hEdge2, node1)).setBorder(false);
    graph.addEdge(hEdge2, node4);
    graph.getEdgeData(graph.findEdge(hEdge2, node4)).setBorder(false);
    graph.addEdge(hEdge2, node3);
    graph.getEdgeData(graph.findEdge(hEdge2, node3)).setBorder(false);
}
