#pragma once
#include <vector>
#include <thread>
#include <functional>
#include <queue>
#include <condition_variable>

class ThreadPool {
public:
    ThreadPool();
    ~ThreadPool();
    void PushTask(const std::function<void()>& task);

private:
    void Work();

    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    std::condition_variable cv_;
    std::mutex mutex_;
    bool terminated_= false;
};
