#pragma once
#include <functional>
#include "vertex.hpp"


class Edge
{
private:
    // The start and end vertices of the edge
    Vertex start;
    Vertex end;
    // Whether the edge is weighted and the weight of the edge
    int weight;

public:
    // Constructor to create a weighted edge
    Edge(Vertex s, Vertex e, int w = 1);

    // Default constructor
    Edge() = default;

    // Copy constructor to create an edge
    Edge(const Edge &other);

    // Getters and setters for edge properties
    Vertex &getStart();
    const Vertex &getStart() const;
    Vertex &getEnd();
    const Vertex &getEnd() const;

    int &getWeight();
    const int &getWeight() const;

    // Get the vertex at the other end of the edge
    Vertex &getOther(Vertex v);
    const Vertex &getOther(Vertex v) const;

    // Check if the edge contains a specific vertex
    bool contains(Vertex &target) const;

    // Flip the direction of the edge
    void flip();

    // Equality operator
    bool operator==(const Edge &other) const;

    //Assignment operator
    Edge &operator=(const Edge &other);

    //Less than operator
    bool operator<(const Edge &other) const;
};

namespace std
{
    template <>
    struct hash<Edge>
    {
        std::size_t operator()(const Edge &e) const
        {
            std::size_t h1 = std::hash<Vertex>{}(e.getStart());
            std::size_t h2 =std::hash<Vertex>{}(e.getEnd());
            std::size_t h3 = std::hash<int>{}(e.getWeight());
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}




