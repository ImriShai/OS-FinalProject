#include "edge.hpp"
#include "vertex.hpp"

// Constructor to create a weighted edge
Edge::Edge(Vertex s, Vertex e, int w ) : start(s), end(e), weight(w) {}

// Copy constructor to create an edge
Edge::Edge(const Edge &other) : start(other.start), end(other.end), weight(other.weight) {}

// Getters and setters for edge properties
Vertex &Edge::getStart() { return start; }
const Vertex &Edge::getStart() const { return start; }
Vertex &Edge::getEnd() { return end; }
const Vertex &Edge::getEnd() const { return end; }

int &Edge::getWeight() { return weight; }
const int &Edge::getWeight() const { return weight; }

// Get the vertex at the other end of the edge
Vertex &Edge::getOther(Vertex v)
{
    return start == v ? end : start;
}

const Vertex &Edge::getOther(Vertex v) const
{
    return start == v ? end : start;
}

// Check if the edge contains a specific vertex
bool Edge::contains(Vertex &target) const
{
    return start == target || end == target;
}

// Flip the direction of the edge
void Edge::flip()
{
    // Remove the edge from the start vertex's edge list and add it to the end vertex's edge list
    start.removeEdge(*this);
    end.addEdge(*this);

    // Swap the start and end vertices
    auto temp = start;
    start = end;
    end = temp;
}

// Equality operator
bool Edge::operator==(const Edge &other) const
{
    return start == other.start && end == other.end && weight == other.weight;
}
