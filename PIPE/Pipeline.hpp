// Pipeline.hpp

#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include "../GraphObj/graph.hpp"
#include "PipelineThreadPool.hpp"

enum PipelineStage {
    STAGE_INIT_GRAPH,
    STAGE_RUN_MST,
    STAGE_COMPUTE_METRICS
};

// Task structure to represent a pipeline task
struct PipelineTask {
    Graph* g;
    int clientFd;
    std::string strat;
    PipelineStage currentStage;

    PipelineTask(Graph* graph, int fd, const std::string& strategy, PipelineStage stage = PipelineStage::STAGE_INIT_GRAPH)
        : g(graph), clientFd(fd), strat(strategy), currentStage(stage) {}
    PipelineTask() : g(nullptr), clientFd(-1), strat(""), currentStage(PipelineStage::STAGE_INIT_GRAPH) {}
};

// Pipelining system
class Pipeline {
public:
    Pipeline(int numThreads);
    void addTask(Graph* g, int clientFd, const std::string& strat);

private:
    std::queue<PipelineTask> taskQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
    PipelineThreadPool threadPool;

    // Worker function to process pipeline tasks
    void processPipelineTask(PipelineTask& task);

    // Thread to handle the pipeline processing
    void pipelineWorker();
};

#endif // PIPELINE_HPP
