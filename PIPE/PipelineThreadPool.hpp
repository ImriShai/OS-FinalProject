// PipelineThreadPool.hpp

#ifndef PIPELINE_THREAD_POOL_HPP
#define PIPELINE_THREAD_POOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class PipelineThreadPool {
public:
    PipelineThreadPool(int numThreads);
    ~PipelineThreadPool();

    // Add a task to the thread pool
    void addTask(std::function<void()> task);

private:
    std::vector<std::thread> workers;       // Worker threads
    std::queue<std::function<void()>> tasks; // Task queue

    std::mutex tasksMutex;                   // Mutex for the task queue
    std::condition_variable condition;       // Condition variable to notify threads
    bool stop;                               // Flag to stop the thread pool

    // Worker function that each thread runs
    void worker();
};

#endif // PIPELINE_THREAD_POOL_HPP
