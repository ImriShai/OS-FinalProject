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

        struct Triple{
            Graph* g;
            string msg;
            int clientFd;
        };

        // Worker struct that will represents a worker thread
        struct Worker {
            std::thread* thread;  // The worker thread
            std::function<std::string(Graph**, std::string, int)> function;  // The function that the worker will execute
            std::queue<Triple> taskQueue;  // The queue of tasks for the worker
            std::mutex* queueMutex;  // Mutex for the task queue
            std::condition_variable* condition;  // Condition variable for the worker to wait on
            std::queue<Triple>* nextTaskQueue;  // Pointer to the next worker's task queue
        };

        void workerFunction(Worker& worker, Worker* nextWorker);

        std::vector<Worker> workers;
        std::atomic<bool> stopFlag;   // atomic flag to stop the threads
    
    public:
        PAO(const std::vector<std::function<std::string(Graph**, std::string, int)>>& functions);  // the constructor get the functions to be executed by the workers and the client file descriptor
        ~PAO();
        PAO() = default;

        void addTask(Graph* mst, std::string msg, int clientFd);
        void start();
        void stop();
};