#include <queue>
#include <random>
#include <omp.h>
#include "graph.h"

#define INF std::numeric_limits<double>::infinity()

VertexRefWMut Graph::AddVertex(vid_t vid) {
    auto vertex = Vertex(vid);
    layers_.emplace(vid, INF);
    return vertices_.emplace(vid, vertex).first->second;
}

std::optional<VertexRefWMut> Graph::GetVertexMut(vid_t vid) {
    if (auto it = vertices_.find(vid); it != vertices_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<VertexRefW> Graph::GetVertex(vid_t vid) const {
    return const_cast<Graph *>(this)->GetVertexMut(vid);
}

bool Graph::AddEdge(Vertex &v1, const Vertex &v2) {
    return v1.adjacent.insert(v2.vid).second;
}

Graph Graph::CreateRandom(size_t nodes, size_t edges) {
    auto graph = Graph();
    if (nodes == 0) return graph;
    for (size_t i = 0; i < nodes; i++) {
        graph.AddVertex(i);
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, nodes - 1);
    for (size_t i = 0; i < edges; i++) {
        bool success;
        do {
            auto v1 = dis(gen);
            auto v2 = dis(gen);
            success = graph.AddEdge(graph.GetVertexMut(v1).value(), graph.GetVertex(v2).value());
        } while (!success);
    }
    return graph;
}

void Graph::SetLayer(vid_t vid, double layer) {
    layers_[vid] = layer;
}

double Graph::GetLayer(vid_t vid) {
    return layers_.find(vid)->second;
}

void BFS(const Graph &graph, const Vertex &start, const std::function<void(const Vertex &)> &visit) {
    std::queue<VertexRefW> queue;
    std::unordered_set<vid_t> visited;
    queue.emplace(start);
    while (!queue.empty()) {
        auto vertex = queue.front();
        queue.pop();
        if (visited.find(vertex.get().vid) != visited.end()) continue;
        visit(vertex);
        visited.insert(vertex.get().vid);
        for (auto vid: vertex.get().adjacent) {
            auto v = graph.GetVertex(vid);
            if (v.has_value()) {
                queue.push(v.value());
            }
        }
    }
}

void OrderedParallelBFS(const Graph &graph, const Vertex &start, const std::function<void(const Vertex &)> &visit) {
    std::queue<VertexRefW> queue;
    std::unordered_set<vid_t> visited;
    queue.emplace(start);
    while (!queue.empty()) {
        auto vertex = queue.front();
#pragma omp critical
        {
            vertex = queue.front();
            queue.pop();
        }
        if (visited.find(vertex.get().vid) != visited.end()) continue;

#pragma omp critical
        {
            visit(vertex);
            visited.insert(vertex.get().vid);
        }

        std::vector<vid_t> adj = {};
        for (auto vid: vertex.get().adjacent) {
            adj.push_back(vid);
        }

#pragma omp for
        for (auto itr = adj.begin(); itr != adj.end(); itr++) {
            auto vid = *itr;
            auto v = graph.GetVertex(vid);
            if (v.has_value()) {
                queue.push(v.value());
            }
        }

#pragma omp barrier
    }
}

void UnOrderedParallelBFS(Graph &graph, const Vertex &start, const std::function<void(const Vertex &)> &visit) {
    std::queue<VertexRefW> queue;
    queue.emplace(start);
    graph.SetLayer(start.vid, 0.0);
    while (!queue.empty()) {
        auto vertex = queue.front();
#pragma omp critical
        {
            vertex = queue.front();
            queue.pop();
        }

#pragma omp critical
        {
            visit(vertex);
        }

        std::vector<vid_t> adj = {};
        for (auto vid: vertex.get().adjacent) {
            adj.push_back(vid);
        }

#pragma omp for
        for (auto itr = adj.begin(); itr != adj.end(); itr++) {
            auto vid = *itr;
            auto v = graph.GetVertex(vid);
            double exp_layer = graph.GetLayer(vertex.get().vid) + 1.0;
            if (v.has_value() && exp_layer < graph.GetLayer(vid)) {
#pragma omp critical
                {
                    graph.SetLayer(vid, exp_layer);
                }
                queue.push(v.value());
            }
        }

#pragma omp barrier
    }
}