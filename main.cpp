#include "graph.cpp"
#include <iostream>
#include <sstream>
using std::istringstream;

vector<uint_t> VERTEX_IDS {};

uint_t random(const Range&);
void initialize_random_graph(uint_t, const Range&, Graph&);

int main(int argc, char *argv[]) {
//    vector<Vertex> V {};
//    for (uint_t i = 0; i < 8; i++)
//        V.push_back(Vertex(i));
//    Graph g(V);
//    g.add_edge(0, 1);
//    g.add_edge(0, 2);
//    g.add_edge(1, 3);
//    g.add_edge(2, 4);
//    g.add_edge(3, 5);
//    g.add_edge(3, 6);
//    g.add_edge(4, 7);
//    g.BFS(0);

//    uint_t min, max, n;
//    istringstream iss_min(argv[1]);
//    istringstream iss_max(argv[2]);
//    istringstream iss_n(argv[3]);
//    iss_min >> min;
//    iss_max >> max;
//    iss_n >> n;

    Range range = {0, 10000};
    Graph g;
    initialize_random_graph(10000, range, g);
    uint_t rand_root_id = VERTEX_IDS[rand() % VERTEX_IDS.size()];
    g.BFS(rand_root_id);

//    vector<Vertex> V = {Vertex(0), Vertex(1), Vertex(2), Vertex(3)};
//    Graph g(V);
//    g.add_edge(0, 1);
//    g.add_edge(0, 2);
//    g.add_edge(1, 2);
//    g.add_edge(2, 0);
//    g.add_edge(2, 3);
//    g.add_edge(3, 3);
//    g.BFS(2);
    return 0;
}

uint_t random(const Range& range) {
    uint_t delta = range.max - range.min + 1;
    return rand() % delta + range.min;
}

void initialize_random_graph(uint_t n, const Range& range, Graph& g) {
    uint_t i;
    for (i = 1; i <= n; i++) {
        uint_t uid = random(range);
        bool added = g.add_vertex(uid);
        if (added) {
            VERTEX_IDS.push_back(uid);
            uint_t randid = random(range);
            if (g.has_vertex(randid)) {
                g.add_edge(randid, uid);
            }
        }
    }
}
