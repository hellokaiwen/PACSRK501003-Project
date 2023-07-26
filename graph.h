#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <unordered_map>
#include <unordered_set>

using vid_t = uint64_t;

struct Vertex {
    const vid_t vid;
    std::unordered_set<vid_t> adjacent;
    Vertex(vid_t v) : vid(v) {}
};

using VertexRefW = std::reference_wrapper<const Vertex>;
using VertexRefWMut = std::reference_wrapper<Vertex>;

class Graph {
public:
    VertexRefWMut AddVertex(vid_t vid);

    std::optional<VertexRefWMut> GetVertexMut(vid_t vid);

    std::optional<VertexRefW> GetVertex(vid_t vid) const;

    bool AddEdge(Vertex& v1, const Vertex& v2);

    static Graph CreateRandom(size_t nodes, size_t edges);

private:
    std::unordered_map<vid_t, Vertex> vertices_;
};

void BFS(const Graph& graph, const Vertex& start, const std::function<void(const Vertex&)>& visit);

void orderedParallelBFS(const Graph& graph, const Vertex& start, const std::function<void(const Vertex&)>& visit);
