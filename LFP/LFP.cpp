#include "LFP.hpp"

using namespace std;

LFP::LFP(int num_threads) : stopFlag(false) {  // Constructor
    for (int i = 0; i < num_threads; ++i) {  // Create threads and add them to the vector
        threads.emplace_back(&LFP::worker, this, i);  // arguments: function, object, id
        threadIDs.push_back(i);
    }
}

LFP::~LFP() {
    stop();  
}

void LFP::addTask(function<void()> task) {
    cout << "Adding task to the LFP queue\n" << endl;
    {
        lock_guard<mutex> lock(queueMutex);  // Lock the mutex
        taskQueue.push(task);  // Add the task to the queue
    }
    condition.notify_one();  // Notify one of the threads
}


void LFP::start() {
    stopFlag = false;  
}

void LFP::stop() {
    {
        lock_guard<mutex> lock(queueMutex);  // Lock the mutex
        stopFlag = true;
    }
    condition.notify_all();
    for (thread &thread : threads) {
        if (thread.joinable()) {
            thread.join();  // Join the threads
        }
    }
}

void LFP::worker(int id) {
    while (true) {  // Worker function to process tasks in the queue
        function<void()> task;  // Task to be executed
        {
            unique_lock<mutex> lock(queueMutex);
            condition.wait(lock, [this]() { return stopFlag || !taskQueue.empty(); });
            if (stopFlag && taskQueue.empty()) return;
            task = taskQueue.front();
            taskQueue.pop();
        }
        std::cout << "Executing task from the LFP queue by thread " << id << endl << endl;
        task();  // Execute the task
        
    }
}