#ifndef __GRAPH__
#define __GRAPH__
#include <limits>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#define INF std::numeric_limits<double>::infinity()

using std::unordered_set;
using std::unordered_map;
using std::vector;

typedef unsigned long uint_t;
typedef struct {
    uint_t min, max;
} Range;

class Vertex;
class Graph;

class Vertex {
public:
    Vertex(uint_t);
    uint_t get_id() const;
    double get_level() const;
    void set_level(double);
    bool has_neighbor(uint_t) const;
    bool add_neighbor(uint_t);
    void print() const;

private:
    uint_t this_id;
    double level = INF;  // set to INF as default
    vector<uint_t> neighbors;  // ids of neighbors

    friend class Graph;
};

class Graph {
public:
    Graph(const vector<Vertex>&);
    Graph();
    bool is_empty() const;
    bool has_vertex(uint_t id) const;
    bool add_vertex(uint_t id);
    bool add_edge(uint_t uid, uint_t vid);
    void print() const;
//    uint_t get_size() const;  // todo: helper function
//    void print_levels() const;  // todo: helper function
    void BFS(uint_t);

private:
    unordered_map<uint_t, Vertex> V;
    uint_t size = 0;
};

#endif
