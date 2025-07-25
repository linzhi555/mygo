#pragma once
#include <cstddef>
#include <mutex>
#include <queue>
namespace mygo {

template <typename T>
class Channel {
 public:
  T Pop() {
    std::unique_lock<std::mutex> lock_guard(mutex_);
    T t = std::move(queue_.front());
    queue_.pop();
    return t;
  }

  size_t Size() {
    std::unique_lock<std::mutex> lock_guard(mutex_);
    return queue_.size();
  }

  void Push(T t) {
    std::unique_lock<std::mutex> lock_guard(mutex_);
    queue_.push(std::move(t));
  }

 private:
  std::mutex mutex_;
  std::queue<T> queue_;
};

}  // namespace mygo
