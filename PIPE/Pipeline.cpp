// Pipeline.cpp

#include "Pipeline.hpp"
#include <shared_mutex>
#include <sys/types.h>
#include <sys/socket.h>
#include "../MST/MST_Factory.hpp"

extern std::shared_mutex graphMutex;  // Mutex to protect access to the graph


Pipeline::Pipeline(int numThreads) : threadPool(numThreads) {
    // Launch a single pipeline worker to process tasks from the queue
    std::thread(&Pipeline::pipelineWorker, this).detach();
}

void Pipeline::addTask(Graph* g, int clientFd, const std::string& strat) {
    PipelineTask task(g, clientFd, strat);
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(task);
    }
    condition.notify_one();
}

void Pipeline::pipelineWorker() {
    while (true) {
        PipelineTask task(nullptr, -1, "");
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return !taskQueue.empty(); });
            task = taskQueue.front();
            taskQueue.pop();
        }
        processPipelineTask(task);
    }
}

void Pipeline::processPipelineTask(PipelineTask& task) {
    switch (task.currentStage) {
        case STAGE_INIT_GRAPH:
            std::cout << "Initializing graph..." << std::endl;
            task.currentStage = STAGE_RUN_MST;
            threadPool.addTask([this, task] () mutable {
                this->addTask(task.g, task.clientFd, task.strat);
            });
            break;

        case STAGE_RUN_MST:
            std::cout << "Running MST algorithm..." << std::endl;
            {
                std::shared_lock<std::shared_mutex> lock(graphMutex);
                Graph mst = (*MST_Factory::getInstance()->createMST(task.strat))(task.g);
                task.g = &mst;
            }
            task.currentStage = STAGE_COMPUTE_METRICS;
            threadPool.addTask([this, task] () mutable {
                this->addTask(task.g, task.clientFd, task.strat);
            });
            break;

        case STAGE_COMPUTE_METRICS:
            std::cout << "Computing MST metrics..." << std::endl;
            // Send metrics back to client
            std::string msg = "MST metrics computed.\n";
            send(task.clientFd, msg.c_str(), msg.size(), 0);
            break;
    }
}
