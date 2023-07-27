#include <cstdio>
#include <iostream>

#include "graph.h"
#include "log.h"

const int NODES = 20;
const int EDGES = NODES * 5;

int main() {
    auto graph = Graph::CreateRandom(NODES, EDGES);
    LOG("Graph generated for %d nodes and %d edges\n", NODES, EDGES);
    auto& start = graph.GetVertex(0).value().get();
    for (int i = 0; i < NODES; i++) {
        auto& vertex = graph.GetVertex(i).value().get();
        LOG("%lu: ", vertex.vid);
        for (auto vid: vertex.adjacent) {
            LOG("%lu ", vid);
        }
        LOG("\n");
    }
    std::cout << "Serial BFS\n";
    BFS(graph, start, [&](const Vertex& v) {
        LOG("%lu ", v.vid);
    });
    std::cout << "\n";
    std::cout << "orderedParallelBFS\n";
    OrderedParallelBFS(graph, start, [&](const Vertex& v) {
        LOG("%lu ", v.vid);
    });
    std::cout << "\n";
    std::cout << "unOrderedParallelBFS\n";
    UnOrderedParallelBFS(graph, start, [&](const Vertex& v) {
        LOG("%lu ", v.vid);
    });
    return 0;
}
