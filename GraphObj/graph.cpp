#include "graph.hpp"



// Check if the graph is connected
bool Graph::isConnected() const
{ //use bfs for connected
    size_t n = numVertices();
    std::vector<std::vector<size_t>> adjMat = adjacencyMatrix();
    std::vector<bool> visited(n, false);
    std::queue<size_t> q;
    q.push(0);
    visited[0] = true;
    size_t count = 1;
    while (!q.empty())
    {
        size_t current = q.front();
        q.pop();
        for (size_t i = 0; i < n; i++)
        {
            if (adjMat[current][i] != INF && !visited[i])
            {
                visited[i] = true;
                q.push(i);
                count++;
            }
        }
    }
    return count == n;
}

// Constructor to create an empty graph
Graph::Graph() : vertices(), edges(), distances(nullptr), parent(nullptr){}

// Constructor to create a graph from a set of vertices that may already contain edges
Graph::Graph(std::unordered_set<Vertex> inputVxs) : vertices(), edges(), distances(nullptr), parent(nullptr)
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
Graph::Graph(const Graph &other, bool copyEdges) : vertices(), edges(), distances(nullptr), parent(nullptr)
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
    if(other.distances != nullptr ){
        //deep copy distances, notice it's unique_ptr
        size_t n = other.numVertices();
        distances = std::make_unique<std::vector<std::vector<size_t>>>(n, std::vector<size_t>(n, INF));
        for (size_t i = 0; i < n; i++)
        {
            for (size_t j = 0; j < n; j++)
            {
                (*distances)[i][j] = (*other.distances)[i][j];
            }
        }
    }
    if(other.parent != nullptr){
        //deep copy parent, notice it's unique_ptr
        size_t n = other.numVertices();
        parent = std::make_unique<std::vector<std::vector<size_t>>(n, std::vector<size_t>(n, INF));
        for (size_t i = 0; i < n; i++)
        {
            for (size_t j = 0; j < n; j++)
            {
                (*parent)[i][j] = (*other.parent)[i][j];
            }
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
    vertices[e.getStart().getId()].addEdge(e);
    edges.insert(e);
}

// Remove an edge from the graph
void Graph::removeEdge(Edge e)
{
    vertices[e.getStart().getId()].removeEdge(e);
    edges.erase(e);
}

void Graph::addEdge(Vertex &start, Vertex &end, size_t weight)
{
    Edge e(start, end, weight);
    addEdge(e);
    e.getStart().addEdge(e);
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
std::vector<std::vector<size_t>> Graph::adjacencyMatrix() const
{
    size_t n = numVertices();
    std::vector<std::vector<size_t>> adjMat(n, std::vector<size_t>(n, INF)); // Initialize all distances to -1, actually infinity, because of using size_t
    for(auto Edge : edges){
        adjMat[Edge.getStart().getId()][Edge.getEnd().getId()] = Edge.getWeight();
        adjMat[Edge.getEnd().getId()][Edge.getStart().getId()] = Edge.getWeight();
    }
    for(size_t i = 0; i < n; i++){
        adjMat[i][i] = 0;
    }
    return adjMat;
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

size_t Graph::totalWeight() const
{
    size_t total = 0;
    for (const auto &e : edges)
    {
        total += e.getWeight();
    }
    return total;
}

std::string Graph::longestPath(std::vector<std::vector<size_t>>& dist) const
{
    size_t n = numVertices();
    size_t maxDist = 0;
    size_t maxDistIndex = 0, maxDistIndex2 = 0;
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < n; j++)
        {
            if (dist[i][j] > maxDist && dist[i][j] != INF && i != j)
            {
                maxDist = dist[i][j];
                maxDistIndex = i;
                maxDistIndex2 = j;
            }
        }
    }
    return "Longest path is from " + std::to_string(maxDistIndex) + " to " + std::to_string(maxDistIndex2) + " with a distance of " + std::to_string(maxDist);
}

std::string Graph::longestPath() const
{
    if(distances == nullptr){
        std::vector<std::vector<size_t>> dist = getDistances().first;
        return longestPath(dist);
    }
    return longestPath(*distances);
}

//gets the distances between vertices in the graph and the parent matrix for undirected graph
std::pair<std::vector<std::vector<size_t>>, std::vector<std::vector<size_t>>> Graph::getDistances() const
{
   if(this->distances == nullptr) throw std::runtime_error("Distances not calculated");
    if(this->parent == nullptr) throw std::runtime_error("Parent not calculated");
    return std::make_pair(*distances, *parent);
   
}

double Graph::avgDistance(std::vector<std::vector<size_t>> &dist) const
{
size_t totalDist = 0;
size_t count = 0;
size_t n = dist.size();
for (size_t i = 0; i < n; i++)
{
    for (size_t j = i; j < n; j++)
    {
        totalDist += dist[i][j];
        count++;
    }
}
count -= n; // Don't count the diagonal
return static_cast<double>(totalDist) / count;
}

double Graph::avgDistance() const
{
    if(distances == nullptr){
        std::vector<std::vector<size_t>> dist = getDistances().first;
        return avgDistance(dist);
    }
    return avgDistance(*distances);
}



std::string Graph::shortestPath(size_t start, size_t end) const
{

    if (distances == nullptr || parent == nullptr)
    {

        std::vector<std::vector<size_t>> dist, parent;
        std::tie(dist, parent) = floydWarshall();
        return shortestPath(start, end, dist, parent);
    }
   
    return shortestPath(start, end, *distances, *parent);
}

std::string Graph::shortestPath(size_t start, size_t end,std::vector<std::vector<size_t>> &dist, std::vector<std::vector<size_t>> &parents) const
{

    if (start >= numVertices() || end >= numVertices())
    {
        return "Invalid vertices\n";
    }

     if (parents[start][end] == INF)
    {
        return "No path exists between " + std::to_string(start) + " and " + std::to_string(end) + "\n";
    }

    std::string path = "Shortest path from " + std::to_string(start) + " to " + std::to_string(end) + " is: ";
   
    std::vector<int> pathVec;
    pathVec.push_back(end);
    size_t current = end;
    while (current != start)
    {
        current = parents[start][current];
        pathVec.push_back(current);
    }
    //using reverse iterator to get the path in the correct order
    for (auto it = pathVec.rbegin(); it != pathVec.rend(); it++)
    {
        path += std::to_string(*it) + " -> ";
    }
    path.pop_back();
    path.pop_back();
    path.pop_back(); // Remove the last arrow

    return path + " with a distance of " + std::to_string(dist[start][end]) + "\n";
   
}

//gets the shortest path between all vertices in the graph, returns a string with all the paths in the graph for undirected graph
std::string Graph::allShortestPaths(std::vector<std::vector<size_t>> &dist,std::vector<std::vector<size_t>> &parent) const
{
    size_t n = numVertices();
    std::string paths = "Shortest paths between all vertices in the graph are: \n";
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = i + 1; j < n; j++)
        {
            paths += shortestPath(i, j, dist, parent);
        }
    }
    return paths;
}

std::string Graph::allShortestPaths() const
{
    if(distances == nullptr){
        std::vector<std::vector<size_t>> dist, parent;
        std::tie(dist, parent) = floydWarshall();
        return allShortestPaths(dist, parent);
    }
    return allShortestPaths(*distances, *parent);
}


// Output the graph to a stream
std::ostream &operator<<(std::ostream &os, const Graph &g)
{
    std::vector<std::vector<size_t>> dist, parents;
    // Get the distances between vertices in the graph and the parent matrix
    std::tie(dist, parents) = g.getDistances();
    os << "Graph with " << g.numVertices() << " vertices and " << g.edges.size() << " edges\n";
    os << "Total weight of edges: " <<  g.totalWeight() << "\n";
    os <<  g.longestPath(dist) << "\n";
    os << "The average distance between vertices is: " << g.avgDistance(dist) << "\n";
    os << "The shortest paths are: \n" << g.allShortestPaths(dist,parents) << "\n";
    return os;
}

std::string Graph::stats() const
{
    std::vector<std::vector<size_t>> dist, parents;
    // Get the distances between vertices in the graph and the parent matrix
   if(distances == nullptr || parent == nullptr){
        std::tie(dist, parents) = floydWarshall();
    }
    else{
        std::tie(dist, parents) = getDistances();
    }
    std::string stats = "Graph with " + std::to_string(numVertices()) + " vertices and " + std::to_string(edges.size()) + " edges\n";
    stats += "Total weight of edges: " + std::to_string(totalWeight()) + "\n";
    stats += longestPath(dist) + "\n";
    stats += "The average distance between vertices is: " + std::to_string(avgDistance(dist)) + "\n";
    stats += "The shortest paths are: \n" + allShortestPaths(dist, parents) + "\n";
    return stats;
}

void Graph::setDistances(std::vector<std::vector<size_t>> dist)
{
    distances = std::make_unique<std::vector<std::vector<size_t>>>(dist);
}

void Graph::setParent(std::vector<std::vector<size_t>> pare)
{
   parent = std::make_unique<std::vector<std::vector<size_t>>>(pare);
}

