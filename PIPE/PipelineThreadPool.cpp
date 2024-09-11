// PipelineThreadPool.cpp

#include "PipelineThreadPool.hpp"

PipelineThreadPool::PipelineThreadPool(int numThreads) : stop(false) {
    for (int i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] { worker(); });
    }
}

PipelineThreadPool::~PipelineThreadPool() {
    {
        std::unique_lock<std::mutex> lock(tasksMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void PipelineThreadPool::addTask(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(tasksMutex);
        tasks.push(std::move(task));
    }
    condition.notify_one();
}

void PipelineThreadPool::worker() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(tasksMutex);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });
            if (stop && tasks.empty()) {
                return;
            }
            task = std::move(tasks.front());
            tasks.pop();
        }
        task(); // Execute the task
    }
}
