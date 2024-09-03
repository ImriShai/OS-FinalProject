#include "vertex.hpp"
#include "edge.hpp"


// Constructor to create a vertex with a given ID
Vertex::Vertex(size_t id) : id(id) {}

// Getters and setters for vertex properties

size_t &Vertex::getId() { return id; }
const size_t &Vertex::getId() const { return id; }

bool &Vertex::getVisited() { return visited; }
const bool &Vertex::getVisited() const { return visited; }

// Add an edge to the vertex
void Vertex::addEdge(Edge e)
{
    edges.push_back(e);
}

// Remove an edge from the vertex
void Vertex::removeEdge(Edge e)
{
    edges.erase(std::remove(edges.begin(), edges.end(), e), edges.end());
}

// Get the degree of the vertex (number of edges)
int Vertex::degree() const
{
    return edges.size();
}

// Get an iterator for the edges connected to the vertex
std::vector<Edge>::iterator Vertex::begin()
{
    return edges.begin();
}
std::vector<Edge>::iterator Vertex::end()
{
    return edges.end();
}

// Check if the vertex has an edge connecting to a specific target vertex
bool Vertex::hasEdge(Vertex target) const
{
    for (auto e : edges)
    {
        if (e.contains(target))
            return true;
    }
    return false;
}

bool Vertex::operator==(const Vertex &other) const
{
    return id == other.id;
}