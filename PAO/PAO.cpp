#include "PAO.hpp"

/**
 * Construct a new PAO object.
 * Get the functions to be executed by the workers and the client file descriptor.
 */
PAO::PAO(const std::vector<std::function<std::string(Graph**, std::string, int)>>& functions): stopFlag(false) {
    // filling the workers vector with the struct Worker:
    for (const auto& func : functions) {
        std::thread* trd = new std::thread();
        std::mutex* mtx = new std::mutex();
        std::condition_variable* cond = new std::condition_variable();
        workers.push_back({trd, func, std::queue<Triple>(), mtx, cond, nullptr});
    }
    // the last worker will send the message to the client:
    std::thread* trd = new std::thread();
    std::mutex* mtx = new std::mutex();
    std::condition_variable* cond = new std::condition_variable();
    workers.push_back({trd, nullptr, std::queue<Triple>(), mtx, cond, nullptr});
    // Set the nextTaskQueue pointer for each worker
    for (size_t i = 0; i < workers.size() - 1; ++i) {
        workers[i].nextTaskQueue = &workers[i + 1].taskQueue;
    }
}

PAO::~PAO() {
    // going over all the workers and deleting the threads, mutexes and condition variables
    for (auto& worker : workers) {
        if (worker.thread->joinable()) {
            worker.thread->join();
        }
        delete worker.thread;
        delete worker.queueMutex;
        delete worker.condition;
    }
    
    stop();
}

/**
 * in this function, the object will give the first thread the mst and the string and it will start working on it.
 */
void PAO::addTask(Graph* mst, std::string msg, int clientFd) {
    {
        std::lock_guard<std::mutex> lock(*(workers[0].queueMutex));
        workers[0].taskQueue.push({mst, msg, clientFd});
    }
    workers[0].condition->notify_one();  // notify the first worker to start working
}

void PAO::start() {
    stopFlag = false;
    
    for (size_t i = 0; i < workers.size(); ++i) {
        Worker* nextWorker = (i + 1 < workers.size()) ? &workers[i + 1] : nullptr;
        *(workers[i].thread) = std::thread(&PAO::workerFunction, this, std::ref(workers[i]), nextWorker);
    }
}

void PAO::stop() {
    stopFlag = true;
    for (auto& worker : workers) {
        worker.condition->notify_all();
        if (worker.thread->joinable()) {
            worker.thread->join();
        }
    }
}

/**
 * this function actually wrap the function that the worker will execute so it will not end until the stopFlag is true.
 */
void PAO::workerFunction(Worker& worker, Worker* nextWorker) {
    while (!stopFlag) {
        // creating a task:
        Triple task;
        {
            std::unique_lock<std::mutex> lock(*worker.queueMutex);
             worker.condition->wait(lock, [&]() { return stopFlag || !worker.taskQueue.empty(); });
            if (stopFlag && worker.taskQueue.empty()) return;
            task = worker.taskQueue.front();
            worker.taskQueue.pop();
        }
        if (worker.function) {
            // task.first == mst, task.second == string
            task.msg += worker.function(&task.g, task.msg, task.clientFd);  // operate the function on the mst and the string
        }
        if (nextWorker) {
            // pushing the task to the next worker:
            {
                std::lock_guard<std::mutex> lock(*nextWorker->queueMutex);
                nextWorker->taskQueue.push(task);  // actually pushes a reserence to the mst and the string
            }
            nextWorker->condition->notify_one();  // notify the next worker to start working
        } else {
            send(task.clientFd, task.msg.c_str(), task.msg.size(), 0);  // means we are in the last worker, so we send the message to the client
        }
    }
}