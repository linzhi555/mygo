#pragma once
#include <chrono>
#include <functional>
namespace mygo {

class Loop;
class Task {
 public:
  enum class State {
    Finish,
    NotFinish,
    Error,
  };

  virtual ~Task() {};

  virtual State run(Loop* l) = 0;
};

class RunOnce : public Task {
 public:
  RunOnce(std::function<void(Loop*)> f) { func_ = f; }
  State run(Loop* l) override {
    if (did_run_) return State::Finish;
    func_(l);
    did_run_ = true;
    return State::Finish;
  }
  ~RunOnce() override = default;

 protected:
  bool did_run_ = false;
  std::function<void(Loop*)> func_;
};

using Duration = std::chrono::milliseconds;
using Time = std::chrono::steady_clock::time_point;

class RunOnceDelay : public RunOnce {
 public:
  RunOnceDelay(std::function<void(Loop*)> f, std::chrono::milliseconds delay)
      : RunOnce(f) {
    init_time_ = std::chrono::steady_clock::now();
    delay_ = delay;
  }
  State run(Loop* l) override {
    if (did_run_) return State::Finish;
    if (timeOut()) {
      func_(l);
      did_run_ = true;
      return State::Finish;
    }
    return State::NotFinish;
  }

  bool timeOut() {
    Time now = std::chrono::steady_clock::now();
    Duration elapsed = std::chrono::duration_cast<Duration>(now - init_time_);
    if (elapsed > delay_) {
      return true;
    }

    return false;
  }
  ~RunOnceDelay() override = default;

 private:
  Time init_time_;
  Duration delay_;
};

}  // namespace mygo
