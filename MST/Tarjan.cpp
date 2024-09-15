#include "Tarjan.hpp"


// The () operator implements the KKT algorithm to find the MST
Graph* KKT::operator()(Graph *g)
{
    // Create a new graph to store the MST
    Graph* mst = new Graph();

    // Extract edges from the original graph and sort them by weight
    std::vector<Edge> edges(g->edgesBegin(), g->edgesEnd());
    std::sort(edges.begin(), edges.end(), [](const Edge &a, const Edge &b) {
        return a.getWeight() < b.getWeight();
    });

    // Number of vertices in the graph
    size_t V = g->numVertices();

    // Vector to store the parent of each vertex in the MST
    std::vector<int> parent(V);
    for (size_t i = 0; i < V; ++i)
        parent[i] = i;

    // Find function to find the root of a vertex
    std::function<int(int)> find = [&parent, &find](int v) {
        if (parent[v] == v)
            return v;
        return parent[v] = find(parent[v]);
    };

    // Union function to unite two subsets
    auto unionSets = [&parent, &find](int u, int v) {
        int rootU = find(u);
        int rootV = find(v);
        if (rootU != rootV)
            parent[rootU] = rootV;
    };

    // Iterate through the edges in sorted order
    for (const auto &edge : edges)
    {
        int u = edge.getStart().getId();
        int v = edge.getEnd().getId();

        // If the vertices belong to different sets, add the edge to the MST
        if (find(u) != find(v))
        {
            mst->addEdge(edge);
            unionSets(u, v);
        }

        // Stop if we have added V - 1 edges to the MST
        if (mst->numVertices() == V - 1)
            break;
    }

    // Return the MST
    return mst;
}

