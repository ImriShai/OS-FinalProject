#include "graph.hpp"

// Perform DFS to visit vertices in the graph
void Graph::DFS_visit(Vertex v, std::map<Vertex, bool> &visited) const
{
    visited[v] = true;
    for (Edge e : v)
    {
        Vertex u = e.getOther(v);
        if (!visited[u])
            DFS_visit(u, visited);
    }
}

// Constructor to create an empty graph
Graph::Graph() : vertices(), edges() {}

// Constructor to create a graph from a set of vertices that may already contain edges
Graph::Graph(std::unordered_set<Vertex> inputVxs) : vertices(), edges()
{
    // Add vertices to the graph
    for (auto v : inputVxs)
        vertices[v.getId()] = v;
    // Add edges to the graph
    for (auto v : inputVxs)
    {
        for (auto e : v)
        {
            if (inputVxs.find(e.getOther(v)) != inputVxs.end())
                edges.insert(e);
        }
    }
}

// Copy constructor with option to not copy edges
Graph::Graph(const Graph &other, bool copyEdges) : vertices(), edges()
{
    for (const auto &pair : other.vertices)
    {
        vertices[pair.first] = pair.second;
    }
    if (copyEdges)
    {
        for (const auto &e : other.edges)
        {
            edges.insert(e);
        }
    }
    else
    { //Remove all edges
        for (auto &pair : vertices)
        {
            pair.second.removeAllEdges();
        }       
}
}


// Get the maximum degree of the graph
size_t Graph::maxDegree() const
{
    int maxDeg = -1;
    for (const auto &pair : vertices)
    {
        maxDeg = std::max(maxDeg, pair.second.degree());
    }
    return (size_t)(maxDeg);
}

// Get the number of vertices in the graph
size_t Graph::numVertices() const
{
    return vertices.size();
}

// Check if the graph has a vertex
bool Graph::hasVertex(Vertex v) const
{
    return vertices.find(v.getId()) != vertices.end();
}

// Get an iterator for the start of edges in the graph
std::unordered_set<Edge>::iterator Graph::edgesBegin()
{
    return edges.begin();
}

// Get an iterator for the end of edges in the graph
std::unordered_set<Edge>::iterator Graph::edgesEnd()
{
    return edges.end();
}

// Add a vertex and all its edges to the graph
void Graph::addVertexWithEdges(Vertex v)
{
    if (vertices.find(v.getId()) != vertices.end())
        return;
    vertices[v.getId()] = v;
    for (Edge e : v)
    {
        if (vertices.find(e.getOther(v).getId()) != vertices.end())
            edges.insert(e);
    }
}

// Remove a vertex from the graph
void Graph::removeVertexWithEdges(Vertex v)
{
    if (vertices.find(v.getId()) == vertices.end())
        return;
    vertices.erase(v.getId());
    for (Edge &e : v)
    {
        edges.erase(e);
    }
}

// Add an edge to the graph, the edge is directed from start to end
void Graph::addEdge(Edge e)
{
    e.getStart().addEdge(e);
    edges.insert(e);
}

// Remove an edge from the graph
void Graph::removeEdge(Edge e)
{
    e.getStart().removeEdge(e);
    edges.erase(e);
}

// Get an iterator for the vertices in the graph
std::map<int, Vertex>::iterator Graph::begin()
{
    return vertices.begin();
}

// Get an iterator for the end of the vertices in the graph
std::map<int, Vertex>::iterator Graph::end()
{
    return vertices.end();
}

// Check if the graph is complete
bool Graph::isComplete() const
{
    int deg = numVertices() - 1;
    for (const auto &pair : vertices)
    {
        if (pair.second.degree() != deg)
            return false;
    }
    return true;
}

// Get the adjacency matrix of the graph
std::vector<std::vector<size_t>> Graph::adjacencyMatrix()
{
    size_t n = numVertices();
    std::vector<std::vector<size_t>> adjMat(n, std::vector<size_t>(n, 0));
    for (const auto &pair : vertices)
    {
        Vertex v = pair.second;
        for (Edge e : v)
        {
            Vertex u = e.getOther(v);
            adjMat[v.getId()][u.getId()] = 1;
        }
    }
    return adjMat;
}

// Check if the graph is connected
bool Graph::isConnected() const
{
    // Perform DFS to visit vertices in the graph
    Vertex start = vertices.begin()->second;
    std::map<Vertex, bool> visited;
    for (const auto &pair : vertices)
    {
        visited[pair.second] = false;
    }
    DFS_visit(start, visited);
    // Check if all vertices were visited
    for (const auto &pair : visited)
    {
        if (!pair.second)
            return false;
    }
    return true;
}

// Get a vertex by its ID
Vertex &Graph::getVertex(int id)
{
    return vertices[id];
}

const Vertex &Graph::getVertex(int id) const
{
    return vertices.at(id);
}
