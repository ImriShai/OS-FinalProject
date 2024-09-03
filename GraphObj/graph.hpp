#include "vertex.hpp"
#include "edge.hpp"
#include <map>
#include <unordered_set>
#include <vector>

class Graph
{

private:
    // Map to store vertices by their IDs
    std::map<int, Vertex> vertices;
    // Set to store edges in the graph
    std::unordered_set<Edge, std::hash<Edge>> edges;

    // Perform DFS to visit vertices in the graph
    void DFS_visit(Vertex v, std::map<Vertex, bool> &visited) const;

public:
    // Constructor to create an empty graph
    Graph();

    // Constructor to create a graph from a set of vertices that may already contain edges
    Graph(std::unordered_set<Vertex> &inputVxs);

    // Get the maximum degree of the graph
    size_t maxDegree() const;

    // Get the number of vertices in the graph
    size_t numVertices() const;
    // Check if the graph has a vertex
    bool hasVertex(Vertex v) const;
    // Get an iterator for the start of edges in the graph
    std::unordered_set<Edge>::iterator edgesBegin();
    // Get an iterator for the end of edges in the graph
    std::unordered_set<Edge>::iterator edgesEnd();

    // Add a vertex and all its edges to the graph
    void addVertexWithEdges(Vertex v);

    // Remove a vertex from the graph
    void removeVertexWithEdges(Vertex v);

    // Add an edge to the graph, the edge is directed from start to end
    void addEdge(Edge e);
    // Remove an edge from the graph
    void removeEdge(Edge e);

    // Get an iterator for the vertices in the graph
    std::map<int, Vertex>::iterator begin();

    // Get an iterator for the end of the vertices in the graph
    std::map<int, Vertex>::iterator end();

    // Check if the graph is complete
    bool isComplete() const;

    // Get the adjacency matrix of the graph
    std::vector<std::vector<size_t>> adjacencyMatrix();

    // Check if the graph is connected
    bool isConnected() const;

    // Get a vertex by its ID
    Vertex &getVertex(int id);
    const Vertex &getVertex(int id) const;
};