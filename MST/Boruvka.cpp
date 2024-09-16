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
            cheapest[i].getWeight() = std::numeric_limits<size_t>::max();
        }

        // Iterate through all edges to find the cheapest edge for each component
        for (auto e = g->edgesBegin(); e != g->edgesEnd(); ++e)
        {
            size_t u = uf.find(e->getStart().getId());
            size_t v = uf.find(e->getEnd().getId());
          

            if (u != v)
            {
                if (e->getWeight() < cheapest[u].getWeight())
                {
                    cheapest[u] = *e;
                }
                if (e->getWeight() < cheapest[v].getWeight())
                {
                    cheapest[v] = *e;
                }
            }
            
        }

     

        // Add the cheapest edges to the MST and unite the components
        for (size_t i = 0; i < V; ++i)
        {
            if (cheapest[i].getStart().getId() != -1 && cheapest[i].getEnd().getId() != -1)
            {
                size_t u = uf.find(cheapest[i].getStart().getId());
                size_t v = uf.find(cheapest[i].getEnd().getId());

                if (u != v)
                {
                    mst->addEdge(cheapest[i]);
                    uf.Union(u,v);
                    --numComponents;
                }
            }
        }
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