#include "thread_pool.h"

ThreadPool::ThreadPool() {
    for (int i = 0; i < std::thread::hardware_concurrency(); ++i) {
        threads_.emplace_back(&ThreadPool::Work);
    }
}

void ThreadPool::PushTask(const std::function<void()>& task) {
    std::lock_guard<std::mutex> lock(mutex_);
    tasks_.push(task);
    cv_.notify_one();
}

void ThreadPool::Work() {
    while (true) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]{return !tasks_.empty() || terminated_;});

        if (terminated_) {
            return;
        }

        auto task = tasks_.front();
        tasks_.pop();
        lock.unlock();
        task();
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        terminated_ = true;

        while (!tasks_.empty()) {
            tasks_.pop();
        }
    }

    for (auto& th : threads_) {
        th.join();
    }
}
