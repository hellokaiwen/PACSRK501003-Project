#include "graph.h"
#include <algorithm>

void print_array(uint_t*, uint_t, uint_t);

Vertex::Vertex(uint_t id) : this_id(id), neighbors({}) {}

uint_t Vertex::get_id() const { return this->this_id; }

double Vertex::get_level() const { return this->level; }

void Vertex::set_level(double l) { this->level = l; }

/* Precondition: every vertex in a given graph has a unique identifier */
bool Vertex::has_neighbor(uint_t id) const {
    return std::find(this->neighbors.begin(), this->neighbors.end(), id) != this->neighbors.end();
}

/* Precondition: id is a valid vertex id */
bool Vertex::add_neighbor(uint_t id) {
    if (has_neighbor(id) || id == this_id)
        return false;
    this->neighbors.push_back(id);
    return true;
}

void Vertex::print() const {
    printf("%lu: ", this->this_id);
    for (uint_t id : this->neighbors)
        printf("%lu ", id);
    printf("\n");
}

Graph::Graph(const vector<Vertex>& vertices) {
    for (const Vertex& v : vertices) {
        this->V.insert({v.get_id(), v});
        size++;
    }
}

Graph::Graph() : V({}) {}  // initialize an empty graph

bool Graph::is_empty() const { return !size; }

bool Graph::has_vertex(uint_t id) const { return V.find(id) != V.end(); }

bool Graph::add_vertex(uint_t id) {
    if (this->has_vertex(id))
        return false;
    V.insert({id, Vertex(id)});
    size++;
    return true;
}

/* Precondition: graph is undirected */
bool Graph::add_edge(uint_t uid, uint_t vid) {
    if (!this->has_vertex(uid) || !this->has_vertex(vid))
        return false;
    Vertex& u = V.find(uid)->second;
    Vertex& v = V.find(vid)->second;
    u.add_neighbor(v.get_id());
    v.add_neighbor(u.get_id());
    return true;
}

void Graph::print() const {
    std::cout << "size: " << size << std::endl;
    for (const auto& itr : this->V) {
        (itr.second).print();
    }
}

//// todo: helper function
//void Graph::print_levels() const {
//    for (auto itr = V.begin(); itr != V.end(); itr++)
//        std::cout << itr->first << " : " << (itr->second).get_level() << "\n";
//    std::cout << std::endl;
//}
//
//// todo: helper function
//uint_t Graph::get_size() const { return size; }

/* Precondition: root_id is a valid vertex id */
void Graph::BFS(uint_t root_id) {
    uint_t front = 0, rear = 0;
    uint_t* work_set = new uint_t[size];
    work_set[front] = root_id;
    Vertex& root = (Vertex&)V.find(root_id)->second;
    root.set_level(0);

    do {
        uint_t vid = work_set[front];
        Vertex& v = (Vertex&)V.find(vid)->second;
        for (uint_t nid : v.neighbors) {
            Vertex& n = (Vertex&)V.find(nid)->second;
            double n_level = n.get_level();
            double level = v.get_level() + 1;
            if (level < n_level) {
                n.set_level(level);
                work_set[++rear] = n.get_id();
            }
        }
        front++;
    } while (front < rear);

    ((Vertex)V.find(root_id)->second).set_level(INF);
    print_array(work_set, 0, rear);
    delete[] work_set;
}

void print_array(uint_t* arr, uint_t start, uint_t end) {
    uint_t i;
    for (i = start; i <= end; i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;
}