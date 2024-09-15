#include "Boruvka.hpp"
#include <vector>
#include <limits>

Graph* Boruvka::operator()(Graph *g)
{
    // Create a new graph to store the MST with the same vertices as the original graph but no edges
    Graph* mst = new Graph(*g, false);

    // Number of vertices in the graph
    size_t V = g->numVertices();

    // Initialize Union-Find structure
    UnionFind uf(V);

    // Initially, each vertex is its own component
    size_t numComponents = V;

    // Array to store the cheapest edge for each component
    std::vector<Edge> cheapest(V);

    // Continue until there is only one component
    while (numComponents > 1)
    {
        // Initialize the cheapest edges for each component
        for (size_t i = 0; i < V; ++i)
        {
            cheapest[i] = Edge(); // Use a default constructor or a valid edge initialization
        }

        // Iterate through all edges to find the cheapest edge for each component
        for (auto e = g->edgesBegin(); e != g->edgesEnd(); ++e)
        {
            int u = uf.find(e->getStart().getId());
            int v = uf.find(e->getEnd().getId());

            if (u != v)
            {
                if (e->getWeight() < cheapest[(size_t)u].getWeight())
                {
                    cheapest[(size_t)u] = *e;
                }
                if (e->getWeight() < cheapest[(size_t)v].getWeight())
                {
                    cheapest[(size_t)v] = *e;
                }
            }
        }

        // Add the cheapest edges to the MST and unite the components
        for (size_t i = 0; i < V; ++i)
        {
            if (cheapest[i].getStart().getId() != -1 && cheapest[i].getEnd().getId() != -1)
            {
                int u = uf.find(cheapest[i].getStart().getId());
                int v = uf.find(cheapest[i].getEnd().getId());

                if (u != v)
                {
                    mst->addEdge(cheapest[i]);
                    uf.Union(static_cast<size_t>(u), static_cast<size_t>(v));
                    --numComponents;
                }
            }
        }
    }

    // Return the MST
    return mst;
}