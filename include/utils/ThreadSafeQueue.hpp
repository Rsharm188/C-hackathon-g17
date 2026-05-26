// include/utils/ThreadSafeQueue.hpp
#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <chrono>

template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T>           queue;
    mutable std::mutex      mutex;
    std::condition_variable condVar;
    bool                    closed = false;

public:
    ThreadSafeQueue() = default;

    void push(T value) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(std::move(value));
        condVar.notify_one();
    }

    std::optional<T> pop() {
        std::lock_guard<std::mutex> lock(mutex);
        if (queue.empty()) return std::nullopt;
        T value = std::move(queue.front());
        queue.pop();
        return value;
    }

    // Blocks up to timeout_ms, returns nullopt on timeout or close
    std::optional<T> waitPop(int timeout_ms) {
        std::unique_lock<std::mutex> lock(mutex);
        condVar.wait_for(lock,
                         std::chrono::milliseconds(timeout_ms),
                         [this] { return !queue.empty() || closed; });
        if (queue.empty()) return std::nullopt;
        T value = std::move(queue.front());
        queue.pop();
        return value;
    }

    // Wake any blocked waitPop so threads can exit cleanly
    void close() {
        std::lock_guard<std::mutex> lock(mutex);
        closed = true;
        condVar.notify_all();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.size();
    }
};