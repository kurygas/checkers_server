#include <QTcpSocket>
#include <vector>
#include <string>
#include <thread>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <functional>

std::vector<std::string> Split(const std::string& str);

class ThreadPool {
public:
    explicit ThreadPool(size_t threadCount);
    void PushTask(const std::function<void()>& task);
    void Terminate();
    ~ThreadPool();

private:
    void Worker();

    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<std::function<void()>> tasks_;
    std::vector<std::thread> threads_;
    bool isActive_ = true;
};
