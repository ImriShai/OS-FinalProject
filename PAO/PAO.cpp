#include "PAO.hpp"

/**
 * Construct a new PAO object.
 * Get the functions to be executed by the workers and the client file descriptor.
 */
PAO::PAO(const std::vector<std::function<void(Graph&, std::string&)>>& functions, int clientFd) : stopFlag(false), clientFd(clientFd) {
    // filling the workers vector with the struct Worker:
    for (const auto& func : functions) {
        workers.push_back({std::thread(), func, std::queue<std::pair<Graph, std::string>>(), std::mutex(), std::condition_variable(), nullptr});
    }
    // the last worker will send the message to the client:
    workers.push_back({std::thread(), nullptr, std::queue<std::pair<Graph, std::string>>(), std::mutex(), std::condition_variable(), nullptr});

    // Set the nextTaskQueue pointer for each worker
    for (size_t i = 0; i < workers.size() - 1; ++i) {
        workers[i].nextTaskQueue = &workers[i + 1].taskQueue;
    }
}

PAO::~PAO() {
    stop();
}

/**
 * in this function, the object will give the first thread the mst and the string and it will start working on it.
 */
void PAO::addTask(Graph &mst, std::string msg) {
    {
        std::lock_guard<std::mutex> lock(workers[0].queueMutex);
        workers[0].taskQueue.push({mst, msg});
    }
    workers[0].condition.notify_one();  // notify the first worker to start working
}

void PAO::start() {
    stopFlag = false;
    
    for (size_t i = 0; i < workers.size(); ++i) {
        Worker* nextWorker = (i + 1 < workers.size()) ? &workers[i + 1] : nullptr;
        workers[i].thread = std::thread(&PAO::workerFunction, this, std::ref(workers[i]), nextWorker);
    }
}

void PAO::stop() {
    stopFlag = true;
    for (auto& worker : workers) {
        worker.condition.notify_all();
        if (worker.thread.joinable()) {
            worker.thread.join();
        }
    }
}

/**
 * this function actually wrap the function that the worker will execute so it will not end until the stopFlag is true.
 */
void PAO::workerFunction(Worker& worker, Worker* nextWorker) {
    while (!stopFlag) {
        // creating a task:
        std::pair<Graph, std::string> task;
        {
            std::unique_lock<std::mutex> lock(worker.queueMutex);
            worker.condition.wait(lock, [&]() { return stopFlag || !worker.taskQueue.empty(); });
            if (stopFlag && worker.taskQueue.empty()) return;
            task = worker.taskQueue.front();
            worker.taskQueue.pop();
        }
        if (worker.function) {
            // task.first == mst, task.second == string
            worker.function(task.first, task.second);  // operate the function on the mst and the string
        }
        if (nextWorker) {
            // pushing the task to the next worker:
            {
                std::lock_guard<std::mutex> lock(nextWorker->queueMutex);
                nextWorker->taskQueue.push(task);  // actually pushes a reserence to the mst and the string
            }
            nextWorker->condition.notify_one();  // notify the next worker to start working
        } else {
            send(clientFd, task.second.c_str(), task.second.size(), 0);  // means we are in the last worker, so we send the message to the client
        }
    }
}