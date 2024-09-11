#include "PAO.hpp"

PAO::PAO(const std::vector<std::function<void(Graph&, std::string&)>>& functions, int clientFd) : stopFlag(false), clientFd(clientFd) {
    for (const auto& func : functions) {
        workers.push_back({std::thread(), func, std::queue<std::pair<Graph, std::string>>(), std::mutex(), std::condition_variable()});
    }
    workers.push_back({std::thread(), nullptr, std::queue<std::pair<Graph, std::string>>(), std::mutex(), std::condition_variable()});
}

PAO::~PAO() {
    stop();
}

void PAO::addTask(Graph mst, std::string msg) {
    {
        std::lock_guard<std::mutex> lock(workers[0].queueMutex);
        workers[0].taskQueue.push({mst, msg});
    }
    workers[0].condition.notify_one();
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

void PAO::workerFunction(Worker& worker, Worker* nextWorker) {
    while (!stopFlag) {
        std::pair<Graph, std::string> task;
        {
            std::unique_lock<std::mutex> lock(worker.queueMutex);
            worker.condition.wait(lock, [&]() { return stopFlag || !worker.taskQueue.empty(); });
            if (stopFlag && worker.taskQueue.empty()) return;
            task = worker.taskQueue.front();
            worker.taskQueue.pop();
        }
        if (worker.function) {
            worker.function(task.first, task.second);
        }
        if (nextWorker) {
            {
                std::lock_guard<std::mutex> lock(nextWorker->queueMutex);
                nextWorker->taskQueue.push(task);
            }
            nextWorker->condition.notify_one();
        } else {
            send(clientFd, task.second.c_str(), task.second.size(), 0);
        }
    }
}