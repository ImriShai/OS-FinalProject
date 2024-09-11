#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <string>
#include <utility>
#include "../ServerUtils/serverUtils.hpp"
#include "../GraphObj/graph.hpp"


class PAO {
    private:
        // Worker struct that will represent a worker thread
        struct Worker {
            std::thread thread;  // The worker thread
            std::function<void(Graph&, std::string&)> function;  // The function that the worker will execute
            std::queue<std::pair<Graph, std::string>> taskQueue;  // The queue of tasks for the worker
            std::mutex queueMutex;  // Mutex for the task queue
            std::condition_variable condition;  // Condition variable for the worker to wait on
        };

        void workerFunction(Worker& worker, Worker* nextWorker);

        std::vector<Worker> workers;
        std::atomic<bool> stopFlag;
        int clientFd;
    
    public:
        PAO(const std::vector<std::function<void(Graph&, std::string&)>>& functions, int clientFd);
        ~PAO();

        void addTask(Graph mst, std::string msg);
        void start();
        void stop();
};