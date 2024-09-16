#include "Tarjan.hpp"

// The () operator implements the Kruskal's algorithm to find the MST
Graph* Tarjan::operator()(Graph *g)
{
    // Create a new graph to store the MST with the same vertices as the original graph but no edges
    Graph* mst = new Graph(*g, false);

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
        if (parent[(size_t)v] == v)
            return v;
        return parent[(size_t)v] = find(parent[(size_t)v]);
    };

    // Union function to unite two subsets
    auto unionSets = [&parent, &find](int u, int v) {
        int rootU = find((size_t)u);
        int rootV = find((size_t)v);
        if (rootU != rootV)
            parent[(size_t)rootU] = rootV;
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
        if (mst->numEdges() == V - 1)
            break;
    }
    //Cache the distance and parent matrices of the MST for future use
    std::vector<std::vector<size_t>> dist, per;
    std::tie(dist, per) = mst->floydWarshall(); // Get the distance and parent matrices of the MST
    // Update distance and parent matrices in mst
    mst->setDistances(dist);
    mst->setParent(per); 

    // Return the MST
    return mst;
}