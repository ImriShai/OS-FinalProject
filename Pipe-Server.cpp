
#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <vector>
#include <condition_variable>
#include <shared_mutex>
#include "GraphObj/graph.hpp"
#include "MST/MST_Strategy.hpp"
#include "MST/MST_Factory.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include "PIPE/Pipeline.hpp"
#include "PIPE/PipelineThreadPool.hpp"

#define NUM_THREADS 4 // Number of threads for pipelining

using namespace std;







// Queue for pipeline tasks
queue<PipelineTask> pipelineQueue;
mutex pipelineMutex;
condition_variable pipelineCondition;
std::shared_mutex graphMutex;  // Mutex to protect graph operations

pair<string, Graph *> MST(Graph *g, int clientFd, const string& strat) {
    // // Add the task to the pipeline
    // addPipelineTask(g, clientFd, strat);
    // return {"", nullptr};

    //should do something 
    return {"", nullptr};
}

// Pipelining function to process stages
void processPipelineTask(PipelineTask &task) {
    switch (task.currentStage) {
        case STAGE_INIT_GRAPH:
            cout << "Initializing graph for client " << task.clientFd << "..." << endl;
            // Call graph initialization logic here if needed
            task.currentStage = PipelineStage::STAGE_RUN_MST;  // Move to the next stage
            break;

        case STAGE_RUN_MST:
            cout << "Running MST algorithm for client " << task.clientFd << "..." << endl;
            {
                shared_lock<std::shared_mutex> lock(graphMutex);  // Ensure thread safety for graph access
                Graph mst = (*MST_Factory::getInstance()->createMST(task.strat))(task.g);
                task.g = &mst;  // Update the task with the MST graph
            }
            task.currentStage = PipelineStage::STAGE_COMPUTE_METRICS;  // Move to the next stage
            break;

        case STAGE_COMPUTE_METRICS:
            cout << "Computing MST metrics for client " << task.clientFd << "..." << endl;
            // Send metrics to the client
            string msg = "MST metrics computed. Sending results back to client.\n";
            send(task.clientFd, msg.c_str(), msg.size(), 0);  // Send the message back to the client
            break;
    }
}

// Function to run the pipeline in the background
void pipelineWorker() {
    while (true) {
        PipelineTask task;
        {
            unique_lock<mutex> lock(pipelineMutex);
            pipelineCondition.wait(lock, [] { return !pipelineQueue.empty(); });  // Wait for a task to be added
            task = pipelineQueue.front();  // Get the next task
            pipelineQueue.pop();  // Remove it from the queue
        }
        processPipelineTask(task);  // Process the task (move it through stages)
    }
}

// Add a task to the pipeline
void addPipelineTask(Graph* g, int clientFd, string strat) {
    PipelineTask task(g, clientFd, strat, PipelineStage::STAGE_INIT_GRAPH);  // Start the task at the initial stage
    {
        lock_guard<mutex> lock(pipelineMutex);  // Lock the pipeline queue
        pipelineQueue.push(task);  // Add the task to the queue
    }
    pipelineCondition.notify_one();  // Notify a worker thread
}

// Main function to start the pipeline
void startPipeline() {
    vector<thread> pipelineThreads;
    for (int i = 0; i < NUM_THREADS; ++i) {
        pipelineThreads.emplace_back(pipelineWorker);  // Create and start worker threads
    }
    for (thread &t : pipelineThreads) {
        t.join();  // Join all threads (ensure they finish)
    }
}


int main() {
    // Start the pipeline
    // startPipeline();
    //only for compilation
    return 0;
}
