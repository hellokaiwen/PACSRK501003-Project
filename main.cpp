#include <cstdio>
#include <omp.h>
#include <iostream>
#include "graph.h"
#include "log.h"

const int NODES = 100000;
const int EDGES = NODES * 2;

int main(int argc, char *argv[]) {
    auto graph = Graph::CreateRandom(NODES, EDGES);
    LOG("Graph generated for %d nodes and %d edges\n", NODES, EDGES);
	
    auto& start = graph.GetVertex(0).value().get();
//    for (int i = 0; i < NODES; i++) {
//        auto& vertex = graph.GetVertex(i).value().get();
//        LOG("%lu: ", vertex.vid);
//        for (auto vid: vertex.adjacent) {
//            LOG("%lu ", vid);
//        }
//        LOG("\n");
//    }

// [&](const Vertex& v) {LOG("%lu ", v.vid);}

    double init = omp_get_wtime();
    SerialBFS(graph, start);
    double end = omp_get_wtime();
    std::cout << "SERIAL BFS EXECUTION TIME: " << end - init << "\n";

    init = omp_get_wtime();
    graph.BFS(start);
    end = omp_get_wtime();
    std::cout << "GRAPH.BFS EXECUTION TIME: " << end - init << "\n";

    init = omp_get_wtime();
    graph.ParallelBFS(start, 12);
	end = omp_get_wtime();
    std::cout << "PARALLEL BFS EXECUTION TIME: " << end - init << "\n";

    return 0;
}
