#include <queue>
#include <random>
#include <omp.h>
#include "graph.h"

VertexRefWMut Graph::AddVertex(vid_t vid) {
    auto vertex = Vertex(vid);
    return vertices_.emplace(vid, vertex).first->second;
}

std::optional<VertexRefWMut> Graph::GetVertexMut(vid_t vid) {
    if (auto it = vertices_.find(vid); it != vertices_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<VertexRefW> Graph::GetVertex(vid_t vid) const {
    return const_cast<Graph*>(this)->GetVertexMut(vid);
}

bool Graph::AddEdge(Vertex& v1, const Vertex& v2) {
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

void BFS(const Graph& graph, const Vertex& start, const std::function<void(const Vertex&)>& visit) {
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

void orderedParallelBFS(const Graph& graph, const Vertex& start, const std::function<void(const Vertex&)>& visit) {
    std::queue<VertexRefW> queue;
    std::unordered_set<vid_t> visited;
    queue.emplace(start);
    #pragma omp parallel
    {
        while (!queue.empty()) {
            auto vertex = queue.front();
            #pragma omp critical
            {
                if (!queue.empty()) {
                    vertex = queue.front();
                    queue.pop();
                }
            }
            if (vertex.get().vid == -1) continue; // Continue if the vertex is empty
            #pragma omp for schedule(dynamic)
            for (auto vid : vertex.get().adjacent) {
                auto v = graph.GetVertex(vid);
                if (v.has_value() && visited.find(vid) == visited.end()) {
                    #pragma omp critical
                    {
                        visited.insert(vid);
                        queue.push(v.value());
                    }
                }
            }
            #pragma omp single
            {
                if (!queue.empty()) {
                    visit(vertex.get());
                }
            }

            #pragma omp barrier
        }
    }
}