#pragma once
#include "edge.hpp"
// #include "constants.hpp"
#include <vector>
#include <algorithm>

class Vertex{

private:

    // ID of the vertex
     int id;

    // Flag to mark the vertex as visited
    bool visited;
    
    // List to store the edges connected to the vertex
    std::vector<Edge> edges;

    public:

    // Constructor to create a vertex with a given ID
    Vertex(int id);

    //Default constructor
    Vertex() = default;
        
    
    // Getters and setters for vertex properties
    int& getId();
    const int& getId() const;
    
    
    bool& getVisited() { return visited; }
    const bool& getVisited() const { return visited; }
  
    // Add an edge to the vertex
    void addEdge(Edge e);

    // Remove an edge from the vertex
    void removeEdge(Edge e);

    // Get the degree of the vertex (number of edges)
    int degree() const;

    // Get an iterator for the edges connected to the vertex
    std::vector<Edge>::iterator begin();
    std::vector<Edge>::iterator end();

    // Check if the vertex has an edge connecting to a specific target vertex
    bool hasEdge(Vertex target) const;

    bool operator==(const Vertex& other) const;

};

// Hash function for the Vertex class
namespace std {
    template <>
    struct hash<Vertex> {
        std::size_t operator()(const Vertex& v) const {
            // Compute the hash value for the vertex ID
            return std::hash<int>()(v.getId());
        }
    };
}
