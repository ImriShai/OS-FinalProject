#include "Prim.hpp"

// Custom comparator for the priority queue
struct CompareEdges {
    bool operator()(const Edge& e1, const Edge& e2) {
        return e1.getWeight() > e2.getWeight(); // Reverse the comparison to create a min-heap
    }
};

Graph* Prim::operator()(Graph *g)
{
    Graph* mst = new Graph(*g, false);  // Create a new graph with the same vertices as the input graph but no edges

    std::priority_queue<Edge, std::vector<Edge>, CompareEdges> pq; // Create a priority queue with the custom comparator
    std::vector<bool> visited(g->numVertices(), false);                  // Create a vector to store the visited vertices
    visited[g->begin()->second.getId()] = true;                          // Mark the first vertex as visited
    for (auto e : g->getVertex(g->begin()->second.getId()))              // Add all edges connected to the first vertex to the priority queue
    {
        pq.push(e);
    }
    while (!pq.empty()) // Loop until all vertices are visited
    {
        Edge e = pq.top(); // Get the edge with the smallest weight
        pq.pop();
        if (visited[e.getStart().getId()] && visited[e.getEnd().getId()]) // Skip the edge if both vertices are visited
            continue;
        mst->addEdge(e);                     // Add the edge to the MST
        if (!visited[e.getStart().getId()]) // Mark the vertices as visited and add all edges connected to the vertices to the priority queue
        {
            visited[e.getStart().getId()] = true;
            for (auto edge : g->getVertex(e.getStart().getId()))
            {
                pq.push(edge);
            }
        }
        if (!visited[e.getEnd().getId()]) // Mark the vertices as visited and add all edges connected to the vertices to the priority queue
        {
            visited[e.getEnd().getId()] = true;
            for (auto edge : g->getVertex(e.getEnd().getId()))
            {
                pq.push(edge);
            }
        }
    }
    std::vector<std::vector<size_t>> dist, per;
    std::tie(dist, per) = mst->floydWarshall(); // Get the distance and parent matrices of the MST
    //update distance and parent matrices in mst
    mst->setDistances(dist);
    mst->setParent(per);

    //print the MST, the edges and the total weight
    std::cout << "MST: " << std::endl;
    for (auto e = mst->edgesBegin(); e != mst->edgesEnd(); e++)
    {
        std::cout << *e << std::endl;
    }
    std::cout << "Total weight of MST: " << mst->totalWeight() << std::endl;

    return mst; // Return the MST
}


// Graph* Prim::operator()(Graph *g)
// {

//     Graph* mst = new Graph(*g, false);  // Create a new graph with the same vertices as the input graph but no edges

//     std::priority_queue<Edge, std::vector<Edge>, std::less<>> pq; // Create a priority queue to store the edges
//     std::vector<bool> visited(g->numVertices(), false);                  // Create a vector to store the visited vertices
//     visited[g->begin()->second.getId()] = true;                          // Mark the first vertex as visited
//     for (auto e : g->getVertex(g->begin()->second.getId()))              // Add all edges connected to the first vertex to the priority queue
//     {
//         pq.push(e);
//     }
//     while (!pq.empty()) // Loop until all vertices are visited
//     {
//         Edge e = pq.top(); // Get the edge with the smallest weight
//         pq.pop();
//         if (visited[e.getStart().getId()] && visited[e.getEnd().getId()]) // Skip the edge if both vertices are visited
//             continue;
//         mst->addEdge(e);                     // Add the edge to the MST
//         if (!visited[e.getStart().getId()]) // Mark the vertices as visited and add all edges connected to the vertices to the priority queue
//         {
//             visited[e.getStart().getId()] = true;
//             for (auto edge : g->getVertex(e.getStart().getId()))
//             {
//                 pq.push(edge);
//             }
//         }
//         if (!visited[e.getEnd().getId()]) // Mark the vertices as visited and add all edges connected to the vertices to the priority queue
//         {
//             visited[e.getEnd().getId()] = true;
//             for (auto edge : g->getVertex(e.getEnd().getId()))
//             {
//                 pq.push(edge);
//             }
//         }
//     }
//     std::vector<std::vector<size_t>> dist, per;
//     std::tie(dist, per) = mst->floydWarshall(); // Get the distance and parent matrices of the MST
//     //update distance and parent matrices in mst
//     mst->setDistances(dist);
//     mst->setParent(per);

//     //print the MST, the edges and the total weight
//     std::cout << "MST: " << std::endl;
//     for (auto e = mst->edgesBegin(); e != mst->edgesEnd(); e++)
//     {
//         std::cout << *e << std::endl;
//     }
//     std::cout << "Total weight of MST: " << mst->totalWeight() << std::endl;

//     return mst; // Return the MST
// }
