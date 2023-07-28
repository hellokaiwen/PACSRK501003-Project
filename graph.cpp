#include <queue>
#include <random>
#include <iostream>
#include "omp.h"
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

void SerialBFS(const Graph& graph, const Vertex& start) {
    std::queue<VertexRefW> queue;
    std::unordered_set<vid_t> visited;
    queue.emplace(start);
    while (!queue.empty()) {
        auto vertex = queue.front();
        queue.pop();
        if (visited.contains(vertex.get().vid)) continue;
        // visit(vertex);
        visited.insert(vertex.get().vid);
        for (auto vid: vertex.get().adjacent) {
            auto v = graph.GetVertex(vid);
            if (v.has_value()) {
                queue.push(v.value());
            }
        }
    }
}

void Graph::BFS(const Vertex& start) {
    std::unordered_map<vid_t, int> levels = {};
    for (const auto& v : vertices_)
        levels.insert({v.first, -1});
    levels[start.vid] = 0;
    int level = 1;
    std::vector<vid_t> frontier = {};
    std::vector<vid_t> next_frontier = {};
    frontier.push_back(start.vid);
    // std::cout << start.vid << " ";
    while (!frontier.empty()) {
        for (auto itr = frontier.begin(); itr != frontier.end(); itr++) {
            auto uid = *itr;
            auto u = this->GetVertex(uid);
            for (auto vid : u->get().adjacent) {
                if (levels[vid] == -1) {
                    next_frontier.push_back(vid);
                    levels[vid] = level;
                    // std::cout << vid << " ";
                }
            }
        }
        frontier = next_frontier;
        next_frontier.clear();
        level++;
    }
}

void Graph::ParallelBFS(const Vertex& start, int num_threads) {
    std::unordered_map<vid_t, int> levels = {};
    for (const auto& v : vertices_)
        levels.insert({v.first, -1});
    levels[start.vid] = 0;
    int level = 1;
    std::vector<vid_t> frontier = {};
    std::vector<vid_t> next_frontier = {};
    frontier.push_back(start.vid);
    // std::cout << start.vid << " ";
    while (!frontier.empty()) {
        #pragma omp parallel num_threads(num_threads)
        {
            std::vector<vid_t> local_frontier = {};
            #pragma omp for nowait
            for (auto itr = frontier.begin(); itr != frontier.end(); itr++) {
                auto uid = *itr;
                auto u = this->GetVertex(uid);
                for (auto vid : u->get().adjacent) {
                    if (levels[vid] == -1) {
                        int insert;
                        #pragma omp atomic capture
                        {
                            insert = levels[vid];
                            levels[vid] = level;
                        }
                        if (insert == -1) {
                            local_frontier.push_back(vid);
                            // std::cout << vid << " ";
                        }
                    }
                }
            }
            #pragma omp critical
            {
                for (auto vid : local_frontier) {
                    next_frontier.push_back(vid);
                }
            }
        }
        frontier = next_frontier;
        next_frontier.clear();
        level++;
    }
}
