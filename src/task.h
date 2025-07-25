#pragma once
#include <functional>
namespace mygo {
class Task {
 public:
  virtual ~Task() {};
  virtual void run() = 0;
};

class RunOnce : public Task {
 public:
  RunOnce(std::function<void()> f) { func_ = f; }
  void run() override { func_(); }
  ~RunOnce() override = default;

 private:
  std::function<void()> func_;
};

}  // namespace mygo
