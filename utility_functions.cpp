#include "utility_functions.h"

std::vector<std::string> Split(const std::string& str) {
    std::vector<std::string> result(1, "");

    for (const auto& c : str) {
        if (c == '$') {
            result.emplace_back();
        }
        else {
            result.back().push_back(c);
        }
    }

    return result;
}

ThreadPool::ThreadPool(size_t threadCount) {
    for (int i = 0; i < threadCount; ++i) {
        threads_.emplace_back(&ThreadPool::Worker);
    }
}

void ThreadPool::PushTask(const std::function<void()>& task) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (isActive_) {
        tasks_.push(task);
        cv_.notify_one();
    }
}

void ThreadPool::Terminate() {
    std::unique_lock<std::mutex> lock(mutex_);

    if (isActive_) {
        isActive_ = false;
        cv_.notify_all();
    }

    lock.unlock();

    for (auto& thread : threads_) {
        thread.join();
    }
}

ThreadPool::~ThreadPool() {
    Terminate();
}

void ThreadPool::Worker() {
    while (true) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]{return !tasks_.empty() || !isActive_;});

        if (!isActive_ && tasks_.empty()) {
            return;
        }

        auto task = tasks_.front();
        tasks_.pop();
        lock.unlock();
        task();
    }
}