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
            std::queue<std::pair<Graph&, std::string>> taskQueue;  // The queue of tasks for the worker
            std::mutex queueMutex;  // Mutex for the task queue
            std::condition_variable condition;  // Condition variable for the worker to wait on
            std::queue<std::pair<Graph, std::string>>* nextTaskQueue;  // Pointer to the next worker's task queue
        };

        void workerFunction(Worker& worker, Worker* nextWorker);

        std::vector<Worker> workers;
        std::atomic<bool> stopFlag;   // atomic flag to stop the threads
        int clientFd;
    
    public:
        PAO(const std::vector<std::function<void(Graph&, std::string&)>>& functions, int clientFd);  // the constructor get the functions to be executed by the workers and the client file descriptor
        ~PAO();

        void addTask(Graph &mst, std::string msg);
        void start();
        void stop();
};